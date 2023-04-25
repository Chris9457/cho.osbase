// \brief Declaration of the class StartState

#pragma once
#include "AbstractState.h"

namespace NS_OSBASE::statemachine {

    class StartState;
    using StartStatePtr = std::shared_ptr<StartState>;

    /**
     * \brief this class represents a start state
     * \ingroup PACKAGE_OSSTATE
     */
    class StartState final : public AbstractState {
        friend class State;

    public:
        bool isStartState() const override;
        std::string getFullName(const std::string &separator) const override;

    private:
        StartState(StatePtr pParent, const std::string &name);
    };
} // namespace NS_OSBASE::statemachine
