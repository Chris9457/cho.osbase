// \brief Declaration of the interface INetwork

#pragma once
#include "Uri.h"
#include "osCore/Exception/RuntimeException.h"
#include <memory>

namespace NS_OSBASE::data {

    /**
     * \brief Network exception
     */
    class NetworkException : public core::RuntimeException {
        using RuntimeException::RuntimeException;
    };

    class INetwork;
    using INetworkPtr = std::shared_ptr<INetwork>;

    /**
     * \brief This interface exposes the responsiblities of the network
     */
    class INetwork {
    public:
        virtual ~INetwork()              = default;
        virtual Uri::Host getLocalHost() = 0;
    };

    INetworkPtr makeNetwork(); //!< instantiate the concrete network
} // namespace NS_OSBASE::data
