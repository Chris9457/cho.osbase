// \brief Declaration of the class EndState

#include "osStateMachine/EndState.h"
#include "osStateMachine/State.h"
#include "osStateMachine/StateMachineRes.h"

namespace NS_OSBASE::statemachine {

    /*
     * \class EndState
     */
    bool EndState::isEndState() const {
        return true;
    }

    std::string EndState::getFullName(const std::string &separator) const {
        return AbstractState::getFullName(separator) + END_STATE_SUFFIX_NAME;
    }

    AbstractStatePtr EndState::doImplicitTransition(const std::any &value) {
        if (auto const pDelegate = getDelegate(); pDelegate != nullptr) {
            pDelegate->onBeginTransition(Event::create("", value), shared_from_this(), nullptr);
            pDelegate->onEndTransition(getParent());
        }
        return getParent();
    }

    EndState::EndState(StatePtr pParent, const std::string &name) : AbstractState(pParent, name) {
    }

} // namespace NS_OSBASE::statemachine
