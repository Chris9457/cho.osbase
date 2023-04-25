// \brief Declaration of the class StateMachine

#include "osStateMachine/StateMachine.h"
#include "osStateMachine/StateMachineRes.h"
#include "osStateMachine/StateMachineException.h"
#include "osStateMachine/AbstractState.h"
#include "osStateMachine/StateMachineRecord.h"
#include "osData/Log.h"
#include <iomanip>

#include "osStateMachine/Transition.h"

namespace nsdata = NS_OSBASE::data;

namespace NS_OSBASE::statemachine {

    /*
     * \class StateMachine::StateDelegate
     */
    class StateMachine::StateDelegate : public AbstractState::ITransitionDelegate {
    public:
        struct ActionSerie {
            EventPtr pEvent;
            std::vector<std::any> actions;
        };
        using ActionSeries = std::vector<ActionSerie>;

        StateDelegate(StateMachine &stateMachine) : m_stateMachine(stateMachine) {
            std::ostringstream oss;
            oss << "0x" << std::setw(sizeof(&m_stateMachine)) << std::setfill('0') << std::hex
                << &m_stateMachine; // NOLINT(bugprone-sizeof-expression)
            m_stateMachineRecord.id = oss.str();
        }

        void onBeginTransition(EventPtr pEvent, AbstractStatePtr pFromState, TransitionPtr pTransition) override {
            if (m_stateMachineRecord.transitionRecords.empty()) {
                if (!m_stateMachine.getName().empty()) {
                    m_stateMachineRecord.name = m_stateMachine.getName();
                }

                if (auto const pCurrentState = m_pCurrentState.lock(); pCurrentState != nullptr) {
                    m_stateMachineRecord.currentState = pCurrentState->getFullName();
                }
            }

            TransitionRecord transitionRecord;
            transitionRecord.timestamp = nsdata::LoggerTimestamp().toString();

            if (pEvent != nullptr && !pEvent->getName().empty()) {
                transitionRecord.evtName = pEvent->getName();
            }

            if (pTransition != nullptr) {
                if (!pTransition->getName().empty()) {
                    transitionRecord.name = pTransition->getName();
                }

                if (!pTransition->getGuardName().empty()) {
                    transitionRecord.guardName = pTransition->getGuardName();
                }
            }

            if (pFromState != nullptr) {
                transitionRecord.fromState = pFromState->getFullName();
            }
            m_stateMachineRecord.transitionRecords.emplace_back(transitionRecord);
        }

        void onEndTransition(AbstractStatePtr pToState) override {
            if (pToState == nullptr || m_stateMachineRecord.transitionRecords.empty()) {
                return;
            }
            m_stateMachineRecord.transitionRecords.back().toState = pToState->getFullName();
        }

        void onEntryState(StatePtr pState) override {
            if (pState == nullptr) {
                return;
            }
            addActionName(pState->getEntryActionName());
        }

        void onExitState(StatePtr pState) override {
            if (pState == nullptr) {
                return;
            }
            addActionName(pState->getExitActionName());
        }

        void onActionTransition(TransitionPtr pTransition) override {
            if (pTransition == nullptr) {
                return;
            }
            addActionName(pTransition->getActionName());
        }

        void setCurrentState(AbstractStatePtr pState) {
            m_pCurrentState = pState;
        }

        void log(const std::string &failure = "") {
            if (!failure.empty()) {
                m_stateMachineRecord.failure = failure;
            }
            oslog::info(1ull << m_stateMachine.getLogChannel())
                << nsdata::LoggerMessage<StateMachineRecord>(LOGGER_KEY_STATEMACHINE, m_stateMachineRecord) << oslog::end();

            m_stateMachineRecord.transitionRecords.clear();
            m_stateMachineRecord.failure.reset();
        }

    private:
        void addActionName(const std::string &actionName) {
            if (m_stateMachineRecord.transitionRecords.empty()) {
                return;
            }
            const ActionRecord actionRecord = { nsdata::LoggerTimestamp().toString(), actionName };
            if (!m_stateMachineRecord.transitionRecords.back().actionRecords.has_value()) {
                m_stateMachineRecord.transitionRecords.back().actionRecords = ActionRecords{ actionRecord };
            } else {
                m_stateMachineRecord.transitionRecords.back().actionRecords.value().emplace_back(actionRecord);
            }
        }

        StateMachine &m_stateMachine;
        std::string m_stateMachineId;
        StateMachineRecord m_stateMachineRecord;
        AbstractStateWPtr m_pCurrentState;
    };

    /*
     * \class StateMachine
     */
    StateMachinePtr StateMachine::create(application::TaskLoopPtr pTaskLoop) {
        if (pTaskLoop == nullptr) {
            throw StateMachineException("Cannot create the state machine - the task loop is null!");
        }

        return StateMachinePtr(new StateMachine(pTaskLoop));
    }

    StateMachine::~StateMachine() {
        if (isStarted()) {
            stop();
        }
    }

    const std::string &StateMachine::getName() const {
        return m_name;
    }

    void StateMachine::setName(const std::string &name) {
        m_name = name;
    }

    StateMachine::StateMachine(application::TaskLoopPtr pTaskLoop)
        : m_pTaskLoop(pTaskLoop), m_pStateDelegate(std::make_shared<StateDelegate>(*this)) {
    }

    StatePtr StateMachine::getRootState() const {
        return m_pRoot;
    }

    StatePtr StateMachine::setRootState(StatePtr pRoot) {
        if (pRoot == nullptr) {
            throw StateMachineException("Root state null!");
        }

        if (isStarted()) {
            throw StateMachineException("StateMachine is started!");
        }

        m_pRoot = pRoot;
        m_pRoot->setTransitionDelegate(m_pStateDelegate);
        return m_pRoot;
    }

    void StateMachine::start() {
        if (m_pRoot == nullptr) {
            throw StateMachineException("Cannot start the state machine - no state root assigned!");
        }

        if (!m_pTaskLoop->isRunning()) {
            throw StateMachineException("Cannot start the state machine - the task loop is not running!");
        }

        m_pCurrentState = m_pRoot;
        raiseEvent(""); // call implicit transition when starting
    }

    void StateMachine::stop() {
        if (!isStarted()) {
            throw StateMachineException("StateMachine not started!");
        }
        m_pCurrentState.reset();
    }

    bool StateMachine::isStarted() const {
        return !m_pCurrentState.expired();
    }

    void StateMachine::raiseEvent(EventPtr pEvent) {
        std::lock_guard lock(m_mutexEvent);

        if (!isStarted()) {
            throw StateMachineException("StateMachine not started!");
        }

        if (pEvent == nullptr) {
            pEvent = Event::create(std::string());
        }

        m_pTaskLoop->pushMethod(shared_from_this(), &StateMachine::doTransition, pEvent);
    }

    void StateMachine::raiseEvent(const std::string &evtName, const std::any &value) {
        raiseEvent(Event::create(evtName, value));
    }

    AbstractStatePtr StateMachine::getCurrentState() const {
        return m_pCurrentState.lock();
    }

    unsigned char StateMachine::getLogChannel() const {
        return m_logChannel;
    }

    void StateMachine::setLogChannel(const unsigned char channel) {
        m_logChannel = std::min(channel, static_cast<unsigned char>(sizeof(unsigned long long) - 1));
    }

    void StateMachine::setDelegate(ITransitionDelegatePtr pDelegate) {
        m_pTransitionDelegate = pDelegate;
    }

    void StateMachine::doTransition(EventPtr pEvent) {
        auto const pCurrenState = m_pCurrentState.lock();
        auto const pDelegate    = m_pTransitionDelegate.lock();

        if (pCurrenState != nullptr) {
            try {
                m_pStateDelegate->setCurrentState(m_pCurrentState.lock());
                auto const pNewState =
                    pEvent->getName().empty() ? pCurrenState->doImplicitTransition(pEvent->getValue()) : pCurrenState->doTransition(pEvent);
                if (pNewState != nullptr) {
                    m_pCurrentState = pNewState;
                }

                if (pDelegate != nullptr) {
                    pDelegate->onTransitionSucceed(m_pCurrentState.lock());
                }

                m_pStateDelegate->log();
            } catch (const std::exception &e) {
                // TO DO: must rollback the state mahine
                if (pDelegate != nullptr) {
                    pDelegate->onTransitionError(m_pCurrentState.lock(), e.what());
                }

                m_pStateDelegate->log(e.what());
            }
        }
    }
} // namespace NS_OSBASE::statemachine
