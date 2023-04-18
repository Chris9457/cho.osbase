// \brief Declaration od the class State

#include "osStateMachine/StateWalker.h"

namespace cho::osbase::statemachine {
    /**
     * \class StateWalker<direct>
     */
    StateWalker<WalkType::direct>::StateWalker(StatePtr pState) : m_pState(pState) {
    }

    State::iterator StateWalker<WalkType::direct>::begin() const {
        return State::iterator(m_pState);
    }

    State::iterator StateWalker<WalkType::direct>::end() const {
        return State::iterator();
    }

    /**
     * \class StateWalker<recursive>
     */
    StateWalker<WalkType::recursive>::StateWalker(StatePtr pState) : m_pState(pState) {
    }

    State::prefix_iterator StateWalker<WalkType::recursive>::begin() const {
        return State::prefix_iterator(m_pState);
    }

    State::prefix_iterator StateWalker<WalkType::recursive>::end() const {
        return State::prefix_iterator();
    }
} // namespace cho::osbase::statemachine
