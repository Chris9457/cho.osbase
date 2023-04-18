// \brief Declaration of the AbstractState

#include "osStateMachine/AbstractState.h"
#include "osCore/Misc/Scope.h"
#include "osStateMachine/Transition.h"
#include "osStateMachine/State.h"

namespace cho::osbase::statemachine {

    /*
     * \class AbstractState
     */
    AbstractState::AbstractState(StatePtr pParent, const std::string &name) : m_pParent(pParent), m_name(name) {
    }

    const std::string &AbstractState::getName() const {
        return m_name;
    }

    std::string AbstractState::getFullName(const std::string &separator) const {
        std::string fullName = separator + getName();
        auto const pParent   = m_pParent.lock();
        if (pParent != nullptr) {
            fullName = pParent->getFullName(separator) + fullName;
        }

        return fullName;
    }

    StatePtr AbstractState::getParent() const {
        return m_pParent.lock();
    }

    size_t AbstractState::getDepth() const {
        auto const pParent = getParent();
        if (pParent == nullptr) {
            return 0;
        }

        return 1 + pParent->getDepth();
    }

    bool AbstractState::isState() const {
        return false;
    }

    bool AbstractState::isStartState() const {
        return false;
    }

    bool AbstractState::isEndState() const {
        return false;
    }

    bool AbstractState::isHistoryState() const {
        return false;
    }

    TransitionPtr AbstractState::addTransition(AbstractStatePtr pToState, const std::string &evtName) {
        auto pTransition = Transition::create(pToState);
        m_pTransitionMap.insert({ evtName, pTransition });
        return pTransition;
    }

    TransitionPtr AbstractState::addInternalTransition(const std::string &evtName) {
        return addTransition(nullptr, evtName);
    }

    TransitionPtr AbstractState::addImplicitTransition(AbstractStatePtr pToState) {
        return addTransition(pToState, "");
    }

    AbstractStatePtr AbstractState::doTransition(EventPtr pEvent) {
        if (pEvent == nullptr) {
            return shared_from_this();
        }

        auto const itTransition =
            std::find_if(m_pTransitionMap.cbegin(), m_pTransitionMap.cend(), [&pEvent](auto const &evtNameTransition) {
                return evtNameTransition.first == pEvent->getName() && evtNameTransition.second->checkGuard(pEvent->getValue());
            });

        if (itTransition != m_pTransitionMap.cend()) {
            auto const value       = pEvent->getValue();
            auto const pTransition = itTransition->second;
            auto pToState          = pTransition->getToState();

            {
                auto const guard = core::make_scope_exit([this, &pToState]() {
                    if (auto const pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                        pDelegate->onEndTransition(pToState);
                    }
                });

                if (auto const pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                    pDelegate->onBeginTransition(pEvent, shared_from_this(), pTransition);
                }

                if (pToState == nullptr) {
                    // internal transition, no update of the current state
                    pTransition->doAction(value);
                    return nullptr;
                }

                auto const pLCAState = shared_from_this() == pToState ? getParent() : getLCAState(pToState);

                // Execute all exit actions until the LCA state
                doExitUntilLCAState(pLCAState, value);

                // Execute the action of the transition
                pTransition->doAction(value);

                // Execute all entry actions from the LCA state
                pToState->doEntryFromLCAState(pLCAState, value);
            }

            // Perform implicit transitions
            if (auto pImplicitState = pToState->doImplicitTransition(value); pImplicitState != nullptr) {
                return pImplicitState;
            }

            return pToState;
        }

        if (!pEvent->getName().empty()) { // don't check parent state for implicit events
            auto const pParent = getParent();
            if (pParent != nullptr) {
                return pParent->doTransition(pEvent);
            }
        }

        if (auto const pDelegate = getDelegate(); pDelegate != nullptr && !pEvent->getName().empty()) {
            pDelegate->onBeginTransition(pEvent, nullptr, nullptr);
            pDelegate->onEndTransition(nullptr);
        }

        return nullptr;
    }

    const AbstractState::TransitionMap &AbstractState::getTransitions() const {
        return m_pTransitionMap;
    }

    void AbstractState::removeTransition(TransitionPtr pTransition) {
        auto const itTransition = std::find_if(m_pTransitionMap.cbegin(),
            m_pTransitionMap.cend(),
            [&pTransition](auto const pCurrentTransition) { return pTransition == pCurrentTransition.second; });
        if (itTransition != m_pTransitionMap.cend()) {
            m_pTransitionMap.erase(itTransition);
        }
    }

    AbstractStatePtr AbstractState::doImplicitTransition(const std::any &value) {
        return doTransition(Event::create(std::string(), value));
    }

    AbstractState::ITransitionDelegatePtr AbstractState::getDelegate() const {
        return m_pDelegate.lock();
    }

    void AbstractState::setTransitionDelegate(ITransitionDelegatePtr pDelegate) {
        if (m_pDelegate.lock() == pDelegate) {
            return;
        }

        m_pDelegate = pDelegate;
        for (auto &&[evtName, pTransition] : m_pTransitionMap) {
            pTransition->setDelegate(pDelegate);
        }
    }

    StatePtr AbstractState::getLCAState(AbstractStatePtr pOther) {
        if (this == pOther.get()) {
            return std::dynamic_pointer_cast<State>(shared_from_this());
        }

        if (pOther == nullptr) {
            return nullptr;
        }

        // Align to the same detph
        auto const depth      = getDepth();
        auto const otherDepth = pOther->getDepth();
        if (depth > otherDepth) {
            return getParent()->getLCAState(pOther);
        }

        if (depth < otherDepth) {
            return pOther->getParent()->getLCAState(shared_from_this());
        }

        // Same depth, find recursively the ancestor
        if (getDepth() == 0) {
            return nullptr;
        }

        return getParent()->getLCAState(pOther->getParent());
    }

    void AbstractState::doEntryFromLCAState(StatePtr pLCAState, const std::any &value) {
        auto const pState = std::dynamic_pointer_cast<State>(shared_from_this());
        if (pState == pLCAState) {
            return;
        }

        auto const pParent = getParent();
        if (pParent != nullptr) {
            pParent->doEntryFromLCAState(pLCAState, value);
        }

        if (pState != nullptr) {
            pState->doEntry(value);
        }
    }

    void AbstractState::doExitUntilLCAState(StatePtr pLCAState, const std::any &value) {
        auto const pState = std::dynamic_pointer_cast<State>(shared_from_this());
        if (pState == pLCAState) {
            return;
        }

        if (pState != nullptr) {
            pState->doExit(value);
        }

        auto const pParent = getParent();
        if (pParent != nullptr) {
            pParent->doExitUntilLCAState(pLCAState, value);
        }
    }
} // namespace cho::osbase::statemachine

std::ostream &operator<<(std::ostream &os, const cho::osbase::statemachine::AbstractState &state) {
    os << state.getName();
    return os;
}
