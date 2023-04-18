// \brief Declaration of the class StartState

#include "osStateMachine/StartState.h"
#include "osStateMachine/StateMachineRes.h"

namespace cho::osbase::statemachine {

    /*
     * \class StartState
     */
    bool StartState::isStartState() const {
        return true;
    }

    std::string StartState::getFullName(const std::string &separator) const {
        return AbstractState::getFullName(separator) + START_STATE_SUFFIX_NAME;
    }

    StartState::StartState(StatePtr pParent, const std::string &name) : AbstractState(pParent, name) {
    }

} // namespace cho::osbase::statemachine
