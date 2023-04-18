// \brief Declaration of the class PlantUMLStateWriter

#pragma once
#include "osStateMachine/IStateWriter.h"
#include <sstream>

namespace cho::osbase::statemachine {

    class PlantUMLStateWriter : public IStateWriter {
    public:
        void writeState(StatePtr pState) override;
        void flush(std::ostream &os) const override;

    private:
        void writeState(StatePtr pState, const int padding);
        void writeTransitionToHistoryState(StatePtr pState, const int padding);
        void writeTransition(const std::string &evtName, AbstractStatePtr pFromState, TransitionPtr pTransition, const int padding);
        std::ostringstream m_buffer;
        std::vector<std::string> m_legendLines;
    };
} // namespace cho::osbase::statemachine
