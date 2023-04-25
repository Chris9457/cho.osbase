// \brief Daclaration of the class Transition

#include "osStateMachine/Transition.h"
#include "osStateMachine/StateMachineRes.h"

namespace NS_OSBASE::statemachine {
    /*
     * \class Transition
     */
    TransitionPtr Transition::create(AbstractStatePtr pToState) {
        return TransitionPtr(new Transition(pToState));
    }

    Transition::Transition(AbstractStatePtr pToState)
        : m_pToState(pToState), m_guardName(GUARD_DEFAULT_NAME), m_actionName(ACTION_TRANSITION_DEFAULT_NAME) {
    }

    const std::string &Transition::getName() const {
        return m_name;
    }

    void Transition::setName(const std::string &name) {
        m_name = name;
    }

    AbstractStatePtr Transition::getToState() const {
        return m_pToState.lock();
    }

    void Transition::setGuard(const GuardType &guard) {
        m_guard = guard;
    }

    bool Transition::checkGuard(const std::any &value) const {
        if (m_guard) {
            return m_guard(value);
        }

        return true;
    }

    std::string Transition::getGuardName() const {
        if (m_guard) {
            return m_guardName;
        }

        return {};
    }

    void Transition::setGuardName(const std::string &name) {
        m_guardName = name;
    }

    std::string Transition::getActionName() const {
        if (m_action) {
            return m_actionName;
        }

        return {};
    }

    void Transition::setActionName(const std::string &name) {
        m_actionName = name;
    }

    void Transition::setAction(const ActionType &action) {
        m_action = action;
    }

    void Transition::doAction(const std::any &value) {
        if (m_action) {
            m_action(value);
            if (auto const pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                pDelegate->onActionTransition(shared_from_this());
            }
        }
    }

    bool Transition::isInternal() const {
        return m_pToState.expired();
    }

    AbstractState::ITransitionDelegatePtr Transition::getDelegate() const {
        return m_pDelegate.lock();
    }

    void Transition::setDelegate(AbstractState::ITransitionDelegatePtr pDelegate) {
        m_pDelegate = pDelegate;
    }
} // namespace NS_OSBASE::statemachine
