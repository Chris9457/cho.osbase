// \brief Declaration od the class State

#pragma once
#include "AbstractState.h"
#include <stack>
#include <functional>
#include <any>

namespace cho::osbase::statemachine {

    class StartState;
    using StartStatePtr = std::shared_ptr<StartState>;

    class EndState;
    using EndStatePtr = std::shared_ptr<EndState>;

    class HistoryState;
    using HistoryStatePtr = std::shared_ptr<HistoryState>;

    class State;
    using StatePtr  = std::shared_ptr<State>;
    using StateWPtr = std::weak_ptr<State>;

    class IStateWriter;
    using IStateWriterPtr = std::shared_ptr<IStateWriter>;

    /**
     * \brief This class represents a hierarchical state
     * \ingroup PACKAGE_OSSTATE
     */
    class State final : public AbstractState {
        class Iterator;
        class PrefixIterator;

    public:
        using iterator        = Iterator;                              //!< iterate the direct child (and not the other floors)
        using prefix_iterator = PrefixIterator;                        //!< iterate the whole hierarchy starting from this state
        using ActionType      = std::function<void(const std::any &)>; //!< alias for the prototype of the state actions

        static StatePtr create(StatePtr pParent, const std::string &name); //!< create a (sub-)state
        static StatePtr createRoot(const std::string &name);               //!< create the root state

        ~State() override; //!< dtor

        bool isState() const override;
        AbstractStatePtr doImplicitTransition(const std::any &value) override;
        void setTransitionDelegate(ITransitionDelegatePtr pDelegate) override;

        void setEntryAction(const ActionType &action);    //!< assign the entry action
        std::string getEntryActionName() const;           //!< return the name of the entry action - empty if no action is set
        void setEntryActionName(const std::string &name); //!< assign the name of the entry action
        void doEntry(const std::any &arg);                //!< do the entry action

        void setExitAction(const ActionType &action);    //!< assign the exit action
        std::string getExitActionName() const;           //!< return the name of the exit action - empty if no action is set
        void setExitActionName(const std::string &name); //!< assign the name of the entry action
        void doExit(const std::any &arg);                //!< do the exit action

        bool hasStartState() const;                                //!< indicate if the state has a start state
        StartStatePtr setStartState(const std::string &name = ""); //!< create the start state (can be only one)

        bool hasHistoryState() const;                     //!< indicate if the state has an history state
        HistoryStatePtr setHistoryState();                //!< create the history state (can be only one)
        EndStatePtr addEndState(const std::string &name); //!< add an end state

        void write(IStateWriterPtr pStateWriter); //!< export the state to a state writer (for known format such as PlantUML or Scxml)

    private:
        struct RangeIterator {
            AbstractStatePtrs::iterator itCurrent;
            AbstractStatePtrs::iterator itEnd;
        };

        class Iterator {
        public:
            explicit Iterator(StatePtr pState = nullptr);
            AbstractStatePtr operator*() const;
            AbstractStatePtr operator->() const;
            Iterator &operator++();
            bool operator!=(const Iterator &other) const;
            bool operator==(const Iterator &other) const;

        private:
            StatePtr m_pState;
            RangeIterator m_rangeIterator;

            static AbstractStatePtrs s_empty;
        };

        class PrefixIterator {
        public:
            explicit PrefixIterator(StatePtr pState = nullptr);
            AbstractStatePtr operator*() const;
            AbstractStatePtr operator->() const;
            PrefixIterator &operator++();
            bool operator!=(const PrefixIterator &other) const;
            bool operator==(const PrefixIterator &other) const;

        private:
            StatePtr m_pState;
            std::stack<Iterator> m_stackIterator;
        };
        State(StatePtr pParent, const std::string &name);

        void addChild(AbstractStatePtr pChildState);
        void removeChild(AbstractStatePtr pChildState);
        HistoryStatePtr getHistoryState() const;
        StartStatePtr getStartState() const;

        AbstractStatePtrs m_pChildren;
        ActionType m_entryAction;
        std::string m_entryActionName;
        ActionType m_exitAction;
        std::string m_exitActionName;
    };

} // namespace cho::osbase::statemachine
