// \brief Declaration od the class State

#include "osStateMachine/State.h"
#include "osStateMachine/EndState.h"
#include "osStateMachine/HistoryState.h"
#include "osStateMachine/IStateWriter.h"
#include "osStateMachine/StartState.h"
#include "osStateMachine/Transition.h"
#include "osStateMachine/StateMachineException.h"
#include "osStateMachine/StateWalker.h"
#include "osStateMachine/StateMachineRes.h"
#include "osCore/Misc/Scope.h"

namespace NS_OSBASE::statemachine {

    /*
     * \class State
     */
    StatePtr State::create(StatePtr pParent, const std::string &name) {
        if (pParent == nullptr) {
            return createRoot(name);
        }

        auto pState = std::shared_ptr<State>(new State(pParent, name));

        if (pParent != nullptr) {
            pParent->addChild(pState);
        }

        return pState;
    }

    StatePtr State::createRoot(const std::string &name) {
        return std::shared_ptr<State>(new State(nullptr, name));
    }

    State::State(StatePtr pParent, const std::string &name)
        : AbstractState(pParent, name),
          m_entryActionName(ENTRY_ACTION_DEFAULT_NAME + name),
          m_exitActionName(EXIT_ACTION_DEFAULT_NAME + name) {
    }

    State::~State() {
        auto const pParent = getParent();
        if (pParent != nullptr) {
            pParent->removeChild(shared_from_this());
        }
    }

    bool State::isState() const {
        return true;
    }

    AbstractStatePtr State::doImplicitTransition(const std::any &value) {
        // check history state
        auto const pHistoryState = getHistoryState();
        if (pHistoryState != nullptr) {
            auto pImplicitState = pHistoryState->doImplicitTransition(value);
            if (pImplicitState != nullptr) {
                if (auto const pDelegate = getDelegate(); pDelegate != nullptr) {
                    pDelegate->onBeginTransition(Event::create("", value), pHistoryState, nullptr);
                    pDelegate->onEndTransition(pImplicitState);
                }

                return pImplicitState;
            }
        }

        // check start state
        auto const pStartState = getStartState();
        if (pStartState != nullptr) {
            if (auto const pDelegate = getDelegate(); pDelegate != nullptr) {
                pDelegate->onBeginTransition(Event::create("", value), shared_from_this(), nullptr);
                pDelegate->onEndTransition(pStartState);
            }

            auto pImplicitTransition = pStartState->doImplicitTransition(value);
            if (pImplicitTransition != nullptr) {
                return pImplicitTransition;
            }

            return pStartState;
        }

        return AbstractState::doImplicitTransition(value);
    }

    void State::setTransitionDelegate(ITransitionDelegatePtr pDelegate) {
        if (getDelegate() == pDelegate) {
            return;
        }

        AbstractState::setTransitionDelegate(pDelegate);
        for (auto &&pSubState : StateWalker<WalkType::direct>(std::dynamic_pointer_cast<State>(shared_from_this()))) {
            pSubState->setTransitionDelegate(pDelegate);
        }
    }

    void State::setEntryAction(const ActionType &action) {
        m_entryAction = action;
    }

    std::string State::getEntryActionName() const {
        if (m_entryAction) {
            return m_entryActionName;
        }

        return {};
    }

    void State::setEntryActionName(const std::string &name) {
        m_entryActionName = name;
    }

    void State::setExitAction(const ActionType &action) {
        m_exitAction = action;
    }

    std::string State::getExitActionName() const {
        if (m_exitAction) {
            return m_exitActionName;
        }

        return {};
    }

    void State::setExitActionName(const std::string &name) {
        m_exitActionName = name;
    }

    void State::doEntry(const std::any &arg) {
        if (m_entryAction) {
            m_entryAction(arg);
            if (auto const pDelegate = getDelegate(); pDelegate != nullptr) {
                pDelegate->onEntryState(std::dynamic_pointer_cast<State>(shared_from_this()));
            }
        }

        // update the history state
        auto const pParent = getParent();
        if (pParent != nullptr) {
            auto const pHistoryState = pParent->getHistoryState();
            if (pHistoryState != nullptr) {
                pHistoryState->setSavedState(shared_from_this());
            }
        }
    }

    void State::doExit(const std::any &arg) {
        if (m_exitAction) {
            m_exitAction(arg);
            if (auto const pDelegate = getDelegate(); pDelegate != nullptr) {
                pDelegate->onExitState(std::dynamic_pointer_cast<State>(shared_from_this()));
            }
        }

        // update the history state
        auto const pParent = getParent();
        if (pParent != nullptr) {
            auto const pHistoryState = pParent->getHistoryState();
            if (pHistoryState != nullptr) {
                pHistoryState->setSavedState(nullptr);
            }
        }
    }

    bool State::hasStartState() const {
        return std::find_if(m_pChildren.cbegin(), m_pChildren.cend(), [](auto const &pSubState) { return pSubState->isStartState(); }) !=
               m_pChildren.cend();
    }

    StartStatePtr State::setStartState(const std::string &name) {
        m_pChildren.erase(std::remove_if(m_pChildren.begin(), m_pChildren.end(), [](auto const &pState) { return pState->isStartState(); }),
            m_pChildren.cend());

        auto pStartState = StartStatePtr(new StartState(std::dynamic_pointer_cast<State>(shared_from_this()), name));
        m_pChildren.push_back(pStartState);
        return pStartState;
    }

    bool State::hasHistoryState() const {
        return std::find_if(m_pChildren.cbegin(), m_pChildren.cend(), [](auto const &pSubState) { return pSubState->isHistoryState(); }) !=
               m_pChildren.cend();
    }

    HistoryStatePtr State::setHistoryState() {
        m_pChildren.erase(
            std::remove_if(m_pChildren.begin(), m_pChildren.end(), [](auto const &pState) { return pState->isHistoryState(); }),
            m_pChildren.cend());

        auto pHistoryState = HistoryStatePtr(new HistoryState(std::dynamic_pointer_cast<State>(shared_from_this())));
        m_pChildren.push_back(pHistoryState);
        return pHistoryState;
    }

    EndStatePtr State::addEndState(const std::string &name) {
        auto pEndState = EndStatePtr(new EndState(std::dynamic_pointer_cast<State>(shared_from_this()), name));
        m_pChildren.push_back(pEndState);
        return pEndState;
    }

    void State::write(IStateWriterPtr pStateWriter) {
        if (pStateWriter == nullptr) {
            throw StateMachineException("Unable to write - null writer");
        }

        pStateWriter->writeState(std::dynamic_pointer_cast<State>(shared_from_this()));
    }

    void State::addChild(AbstractStatePtr pChildState) {
        m_pChildren.push_back(pChildState);
        pChildState->setTransitionDelegate(getDelegate());
    }

    void State::removeChild(AbstractStatePtr pChildState) {
        m_pChildren.erase(std::remove(m_pChildren.begin(), m_pChildren.end(), pChildState), m_pChildren.cend());
    }

    HistoryStatePtr State::getHistoryState() const {
        auto const itHistoryState =
            std::find_if(m_pChildren.cbegin(), m_pChildren.cend(), [](auto const pSubState) { return pSubState->isHistoryState(); });
        if (itHistoryState != m_pChildren.cend()) {
            return std::dynamic_pointer_cast<HistoryState>(*itHistoryState);
        }

        return nullptr;
    }

    StartStatePtr State::getStartState() const {
        auto const itStartState =
            std::find_if(m_pChildren.cbegin(), m_pChildren.cend(), [](auto const pSubState) { return pSubState->isStartState(); });
        if (itStartState != m_pChildren.cend()) {
            return std::dynamic_pointer_cast<StartState>(*itStartState);
        }

        return nullptr;
    }

#pragma region Iterator
    /*
     * \class State::Iterator
     */
    AbstractStatePtrs State::Iterator::s_empty;

    State::Iterator::Iterator(StatePtr pState)
        : m_pState(pState),
          m_rangeIterator(pState != nullptr ? RangeIterator{ pState->m_pChildren.begin(), pState->m_pChildren.end() }
                                            : RangeIterator{ s_empty.begin(), s_empty.end() }) {
    }

    AbstractStatePtr State::Iterator::operator*() const {
        if (m_rangeIterator.itCurrent != m_rangeIterator.itEnd) {
            return *m_rangeIterator.itCurrent;
        }

        return nullptr;
    }

    AbstractStatePtr State::Iterator::operator->() const {
        return operator*();
    }

    State::Iterator &State::Iterator::operator++() {
        if (m_rangeIterator.itCurrent != m_rangeIterator.itEnd) {
            ++m_rangeIterator.itCurrent;
        }

        return *this;
    }

    bool State::Iterator::operator!=(const State::Iterator &other) const {
        if (m_pState == nullptr) {
            return other.m_rangeIterator.itCurrent != other.m_rangeIterator.itEnd;
        }

        if (other.m_pState == nullptr) {
            return m_rangeIterator.itCurrent != m_rangeIterator.itEnd;
        }

        return m_pState != other.m_pState || m_rangeIterator.itCurrent != other.m_rangeIterator.itCurrent;
    }

    bool State::Iterator::operator==(const Iterator &other) const {
        return !(operator!=(other));
    }
#pragma endregion

#pragma region prefix iterator
    /*
     * \class State::PrefixIterator
     */
    State::PrefixIterator::PrefixIterator(StatePtr pState) : m_pState(pState) {
    }

    AbstractStatePtr State::PrefixIterator::operator*() const {
        if (m_pState != nullptr) {
            return m_pState;
        }

        if (m_stackIterator.empty())
            return nullptr;

        return *m_stackIterator.top();
    }

    AbstractStatePtr State::PrefixIterator::operator->() const {
        return operator*();
    }

    State::PrefixIterator &State::PrefixIterator::operator++() {
        static auto const nullIterator       = Iterator();
        static auto const nullPrefixIterator = PrefixIterator();
        if (m_pState != nullptr) {
            if (!m_pState->m_pChildren.empty()) {
                m_stackIterator.push(Iterator(m_pState));
            }
            m_pState.reset();
            return *this;
        }

        if (m_stackIterator.empty()) {
            return *this;
        }

        auto &itFront = m_stackIterator.top();
        if (itFront != nullIterator) {
            // try to go down
            auto const pSubState = std::dynamic_pointer_cast<StatePtr::element_type>(*itFront);

            if (pSubState != nullptr) {
                const Iterator itSubState(pSubState);
                if (itSubState != nullIterator) {
                    m_stackIterator.push(Iterator(pSubState));
                    return *this;
                }
            }

            // try to go next
            ++itFront;
            if (itFront != nullIterator) {
                return *this;
            }
        }

        // go up and next
        if (!m_stackIterator.empty()) {
            do {
                m_stackIterator.pop();
                if (!m_stackIterator.empty()) {
                    auto &itUp = m_stackIterator.top();
                    ++itUp;
                    if (itUp != nullIterator) {
                        break;
                    }
                }
            } while (*this != nullPrefixIterator);
        }

        return *this;
    }

    bool State::PrefixIterator::operator!=(const PrefixIterator &other) const {
        if (m_pState != nullptr) {
            return m_pState != other.m_pState;
        }

        if (other.m_pState != nullptr) {
            return true;
        }

        if (m_stackIterator.size() != other.m_stackIterator.size()) {
            return true;
        }

        if (m_stackIterator.empty()) {
            return false;
        }

        return m_stackIterator.top() != other.m_stackIterator.top();
    }

    bool State::PrefixIterator::operator==(const PrefixIterator &other) const {
        return !(operator!=(other));
    }
#pragma endregion

} // namespace NS_OSBASE::statemachine
