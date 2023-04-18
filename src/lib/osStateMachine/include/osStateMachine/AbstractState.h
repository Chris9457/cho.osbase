// \brief Declaration of the AbstractState

#pragma once
#include "Event.h"
#include "osCore/Misc/NonCopyable.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>

/**
 * \defgroup PACKAGE_OSSTATE States
 *
 * \brief This package gathers classes and functions to manage the states
 *
 * \ingroup PACKAGE_OSSTATEMACHINE
 */

namespace cho::osbase::statemachine {

    class Transition;
    using TransitionPtr  = std::shared_ptr<Transition>;
    using TransitionPtrs = std::vector<TransitionPtr>;

    class State;
    using StatePtr  = std::shared_ptr<State>;
    using StateWPtr = std::weak_ptr<State>;

    class AbstractState;
    using AbstractStatePtr  = std::shared_ptr<AbstractState>;
    using AbstractStateWPtr = std::weak_ptr<AbstractState>;
    using AbstractStatePtrs = std::vector<AbstractStatePtr>;

    constexpr char STATE_SEPARATOR[] = "/"; //!< default separator used for the full state name

    /**
     * \brief Base class of states
     * \ingroup PACKAGE_OSSTATE
     */
    class AbstractState : public std::enable_shared_from_this<AbstractState> {

    public:
        /**
         * \brief Delegate used to indicate the different step of a transition
         */
        class ITransitionDelegate : core::NonCopyable {
        public:
            virtual ~ITransitionDelegate() = default; //!< dtor

            virtual void onBeginTransition(EventPtr pEvent,
                AbstractStatePtr pFromState,
                TransitionPtr pTransition)                             = 0; //!< called when a transition begins
            virtual void onEndTransition(AbstractStatePtr pToState)    = 0; //!< called when a transition ends
            virtual void onEntryState(StatePtr pState)                 = 0; //!< called when a state entry action is called
            virtual void onExitState(StatePtr pState)                  = 0; //!< called when a state exit action is called
            virtual void onActionTransition(TransitionPtr pTransition) = 0; //!< called when a transition action is called
        };
        using ITransitionDelegatePtr  = std::shared_ptr<ITransitionDelegate>; //!< alias for a shared pointer on IDelegate
        using ITransitionDelegateWPtr = std::weak_ptr<ITransitionDelegate>;   //!< alias for a weak pointer on IDelegate

        using TransitionMap = std::unordered_multimap<std::string, TransitionPtr>; //!< associate an event name with a
                                                                                   //!< transition
        virtual ~AbstractState() = default;                                        //!< dtor

        size_t getDepth() const;            //!< return the hierarchical depth of the state - 0 for the root
        const std::string &getName() const; //!< name of the state
        virtual std::string getFullName(const std::string &separator = STATE_SEPARATOR) const; //!< return the full path name
        StatePtr getParent() const;                                                            //!< parent of the state, null for the root

        virtual bool isState() const;        //!< indicate if its concrete type is State
        virtual bool isStartState() const;   //!< indicate if its concrete type is StartState
        virtual bool isEndState() const;     //!< indicate if its concrete type is EndState
        virtual bool isHistoryState() const; //!< indicate if its concrete type is HistoryState

        TransitionPtr addTransition(AbstractStatePtr pToState, const std::string &evtName); //!< add a transition - toState is not null,
                                                                                            //!< evtName is not empty
        TransitionPtr addInternalTransition(const std::string &evtName); //!< add an internal transition - evtName is not empty
        TransitionPtr addImplicitTransition(AbstractStatePtr pToState);  //!< add an implicit transition - toState is not null
        const TransitionMap &getTransitions() const;                     //!< return the transitions associated with the state
        void removeTransition(TransitionPtr pTransition);                //!< remove a transition

        AbstractStatePtr doTransition(EventPtr pEvent);                       //!< do the transition linked to the event
        virtual AbstractStatePtr doImplicitTransition(const std::any &value); //!< do the implicit transition linked to the event

        ITransitionDelegatePtr getDelegate() const;                           //!< return the current delegate
        virtual void setTransitionDelegate(ITransitionDelegatePtr pDelegate); //!< assign the delegate

    protected:
        AbstractState(StatePtr pParent, const std::string &name); //!< ctor

    private:
        StatePtr getLCAState(AbstractStatePtr pOther); // LCA => Least Common Ancestor
        void doEntryFromLCAState(StatePtr pLCAState, const std::any &value);
        void doExitUntilLCAState(StatePtr pLCAState, const std::any &value);

        StateWPtr m_pParent;
        std::string m_name;
        TransitionMap m_pTransitionMap;
        ITransitionDelegateWPtr m_pDelegate;
    };

} // namespace cho::osbase::statemachine

std::ostream &operator<<(std::ostream &os, const cho::osbase::statemachine::AbstractState &state);
