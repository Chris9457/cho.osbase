// \brief Header for the IBroker class

#include "osData/IBroker.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace nscore = cho::osbase::core;

namespace cho::osbase::data {

    IBrokerPtr makeBroker() {
        return nscore::TheFactoryManager.createInstance<IBroker>(WAMPCCBROCKER_FACTORY_NAME);
    }

} // namespace cho::osbase::data
