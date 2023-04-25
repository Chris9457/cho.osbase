// \brief Declaration of the interface INetwork

#pragma once
#include "osData/INetwork.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace NS_OSBASE::data {

    /**
     * maker
     */
    INetworkPtr makeNetwork() {
        return core::TheFactoryManager.createInstance<INetwork>(NETWORK_FACTORY_NAME);
    }
} // namespace NS_OSBASE::data
