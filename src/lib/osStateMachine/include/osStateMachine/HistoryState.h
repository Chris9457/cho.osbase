// \brief Declaration of the class HistoryState

#pragma once
#include "AbstractState.h"

namespace NS_OSBASE::statemachine {

    class HistoryState;
    using HistoryStatePtr = std::shared_ptr<HistoryState>;

    /**
     * \brief this class represents an history state
     * \ingroup PACKAGE_OSSTATE
     */
    class HistoryState final : public AbstractState {
        friend class State;

    public:
        bool isHistoryState() const override;
        AbstractStatePtr doImplicitTransition(const std::any &) override;

        AbstractStatePtr getSavedState() const;      //!< return the saved state, null if not
        void setSavedState(AbstractStatePtr pState); //!< assign the saved state

    private:
        HistoryState(StatePtr pParent);
        AbstractStateWPtr m_pSavedState;
    };
} // namespace NS_OSBASE::statemachine
