// \brief Daclaration of the class Transition

#pragma once
#include "Event.h"
#include "AbstractState.h"
#include <functional>
#include <vector>

namespace cho::osbase::statemachine {

    class Transition;
    using TransitionPtr  = std::shared_ptr<Transition>;
    using TransitionPtrs = std::vector<TransitionPtr>;

    /**
     * \brief represent a transition
     * \ingroup PACKAGE_OSSTATE
     */
    class Transition final : public std::enable_shared_from_this<Transition> {
    public:
        using GuardType  = std::function<bool(const std::any &)>; //!< alias for the prototype for the transition guards
        using ActionType = std::function<void(const std::any &)>; //!< alias for the prototype for the transition actions

        static TransitionPtr create(AbstractStatePtr pToState = nullptr); //!< instanciate a transition

        const std::string &getName() const;    //!< return the associated name (empty by default)
        void setName(const std::string &name); //!< set the name of the transition (used for the export)

        AbstractStatePtr getToState() const; //!< return the end state of the transition

        void setGuard(const GuardType &guard);        //!< assign the guard
        bool checkGuard(const std::any &value) const; //!< check the guard to indicate if the transition can be done

        std::string getGuardName() const;           //!< return the guard name
        void setGuardName(const std::string &name); //!< assign the guard name

        void setAction(const ActionType &action); //!< assign the action associated to the transition
        void doAction(const std::any &value);     //!< do the action associated to the transition

        std::string getActionName() const;           //!< return the action name
        void setActionName(const std::string &name); //!< set the action name

        bool isInternal() const; //!< indicate if the transition is internal (aka to state is null)

        AbstractState::ITransitionDelegatePtr getDelegate() const;         //!< return the current delegate
        void setDelegate(AbstractState::ITransitionDelegatePtr pDelegate); //!< assign the delegate

    private:
        explicit Transition(AbstractStatePtr pToState);

        std::string m_name;
        AbstractStateWPtr m_pToState;
        GuardType m_guard;
        std::string m_guardName;
        ActionType m_action;
        std::string m_actionName;
        AbstractState::ITransitionDelegateWPtr m_pDelegate;
    };
} // namespace cho::osbase::statemachine
