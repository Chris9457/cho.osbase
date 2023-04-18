// \brief Declaration of the factory links

#pragma once
#include "IStateWriter.h"
#include "osCore/DesignPattern/AbstractFactory.h"

#define OS_STATEMACHINE_LINK()                                                                                                             \
    namespace cho::osbase::statemachine {                                                                                           \
        OS_LINK_FACTORY_N(IStateWriter, PlantUMLStateWriter, 0);                                                                           \
    }
