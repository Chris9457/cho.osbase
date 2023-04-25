// \brief Declaration of the interface IStateWriter

#include "osStateMachine/IStateWriter.h"
#include "FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace NS_OSBASE::statemachine {

    /*
     * makers
     */
    IStateWriterPtr makePlantUMLStateWriter() {
        return core::TheFactoryManager.createInstance<IStateWriter>(STATE_WRITER_PLANTUML);
    }

    std::ostream &operator<<(std::ostream &os, const IStateWriter &stateWriter) {
        stateWriter.flush(os);
        return os;
    }
} // namespace NS_OSBASE::statemachine
