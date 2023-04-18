// \brief Declaration of the class HistoryState

#include "osStateMachine/HistoryState.h"
#include "osStateMachine/State.h"
#include "osStateMachine/StateMachineRes.h"

namespace cho::osbase::statemachine {

    /*
     * \class HistoryState
     */
    bool HistoryState::isHistoryState() const {
        return true;
    }

    AbstractStatePtr HistoryState::getSavedState() const {
        return m_pSavedState.lock();
    }

    void HistoryState::setSavedState(AbstractStatePtr pState) {
        m_pSavedState = pState;
    }

    AbstractStatePtr HistoryState::doImplicitTransition(const std::any &) {
        if (auto pSavedState = m_pSavedState.lock(); pSavedState != nullptr) {
            return pSavedState;
        }
        return nullptr;
    }

    HistoryState::HistoryState(StatePtr pParent) : AbstractState(pParent, HISTORY_STATE_NAME) {
    }

} // namespace cho::osbase::statemachine
