// \brief Declaration of the class StateMachine

#pragma once
#include "State.h"
#include "Event.h"
#include "osApplication/TaskLoop.h"

/**
 * \defgroup PACKAGE_OSSTATEENGINE State machine engine
 *
 * \brief This package gathers classes and functions to manage the engine
 *
 * \ingroup PACKAGE_OSSTATEMACHINE
 */

namespace NS_OSBASE::statemachine {

    class StateMachine;
    using StateMachinePtr = std::shared_ptr<StateMachine>;

    /**
     * \brief This class represents a hierarchical state machine
     * \ingroup PACKAGE_OSSTATEENGINE
     */
    class StateMachine final : public std::enable_shared_from_this<StateMachine> {
    public:
        /**
         * \brief called to notify asynchronously the result of the transition
         */
        class ITransitionDelegate : core::NonCopyable {
        public:
            virtual ~ITransitionDelegate() = default; //!< dtor

            virtual void onTransitionSucceed(AbstractStatePtr pNewState) = 0; //!< called when a transition succed
            virtual void onTransitionError(AbstractStatePtr pNewState, const std::string &strError) = 0; //!< called when a transition fail
        };
        using ITransitionDelegatePtr  = std::shared_ptr<ITransitionDelegate>; //!< alias on a shared pointer on IDelegate
        using ITransitionDelegateWPtr = std::weak_ptr<ITransitionDelegate>;   //!< alias on a weakpointer on IDelegate

        static StateMachinePtr create(application::TaskLoopPtr pTaskLoop); //!< create an instance of the state machine
        virtual ~StateMachine();                                           //!< dtor

        const std::string &getName() const;    //!< return the name of the state machine
        void setName(const std::string &name); //!< assign the name of the state machine

        StatePtr getRootState() const;         //!< return the root state of the hierarchy
        StatePtr setRootState(StatePtr pRoot); //!< assign a root state

        void start();           //!< start the state machine
        void stop();            //!< stop the state machine
        bool isStarted() const; //!< indicate if the state machine is started

        void raiseEvent(EventPtr pEvent);                                                //!< raise the event
        void raiseEvent(const std::string &evtName, const std::any &value = std::any()); //!< create and raise the event

        AbstractStatePtr getCurrentState() const; //!< return the current state

        unsigned char getLogChannel() const;             //!< return the channel used for the log - by default the highest bit
        void setLogChannel(const unsigned char channel); //!< assign the channel used for the log

        void setDelegate(ITransitionDelegatePtr pDelegate); //!< assign the delegate

    private:
        class StateDelegate;
        using StateDelegatePtr = std::shared_ptr<StateDelegate>;

        StateMachine(application::TaskLoopPtr pTaskLoop);

        void doTransition(EventPtr pEvent);

        std::string m_name;
        application::TaskLoopPtr m_pTaskLoop;
        StatePtr m_pRoot;
        AbstractStateWPtr m_pCurrentState;
        std::mutex m_mutexEvent;
        ITransitionDelegateWPtr m_pTransitionDelegate;
        StateDelegatePtr m_pStateDelegate;
        unsigned char m_logChannel = static_cast<unsigned char>(sizeof(unsigned long long) * 8 - 1); // by default, highest weighted bit
    };

} // namespace NS_OSBASE::statemachine
