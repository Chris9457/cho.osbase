// \brief Header for the IBroker class

#include "osData/IBroker.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace nscore = NS_OSBASE::core;

namespace NS_OSBASE::data {

    IBrokerPtr makeBroker() {
        return nscore::TheFactoryManager.createInstance<IBroker>(WAMPCCBROCKER_FACTORY_NAME);
    }

} // namespace NS_OSBASE::data
