// \brief Declaration of the class EndState

#pragma once
#include "AbstractState.h"

namespace cho::osbase::statemachine {

    class EndState;
    using EndStatePtr = std::shared_ptr<EndState>;

    /**
     * \brief this class represents an end state
     * \ingroup PACKAGE_OSSTATE
     */
    class EndState final : public AbstractState {
        friend class State;

    public:
        bool isEndState() const override;
        std::string getFullName(const std::string &separator) const override;
        AbstractStatePtr doImplicitTransition(const std::any &) override;

    private:
        EndState(StatePtr pParent, const std::string &name);
    };
} // namespace cho::osbase::statemachine
