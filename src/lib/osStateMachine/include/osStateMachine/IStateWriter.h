// \brief Declaration of the interface IStateWriter

#pragma once
#include "State.h"
#include "StartState.h"
#include "EndState.h"
#include "HistoryState.h"
#include "Transition.h"

namespace cho::osbase::statemachine {

    class State;
    using StatePtr = std::shared_ptr<State>;

    class IStateWriter;
    using IStateWriterPtr = std::shared_ptr<IStateWriter>;

    /**
     * \brief Interface to export the states
     * \ingroup PACKAGE_OSSTATE
     */
    class IStateWriter {
    public:
        virtual ~IStateWriter() = default; //!< ctor

        virtual void writeState(StatePtr pState)   = 0; //!< write a state
        virtual void flush(std::ostream &os) const = 0; //!< flush the formating stream
    };

    IStateWriterPtr makePlantUMLStateWriter(); //!< instanciate a PlantUML writer

    std::ostream &operator<<(std::ostream &os, const IStateWriter &stateWriter); //!< stream operator for the state writer
} // namespace cho::osbase::statemachine
