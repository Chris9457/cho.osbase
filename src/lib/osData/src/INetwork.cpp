// \brief Declaration of the interface INetwork

#pragma once
#include "osData/INetwork.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace cho::osbase::data {

    /**
     * maker
     */
    INetworkPtr makeNetwork() {
        return core::TheFactoryManager.createInstance<INetwork>(NETWORK_FACTORY_NAME);
    }
} // namespace cho::osbase::data
