// \brief Declaration of the class Network

#pragma once
#include "osData/INetwork.h"

namespace NS_OSBASE::data::impl {

    /**
     * \brief Concrete implementation of the interface INetwork
     */
    class Network : public INetwork {
    public:
        Uri::Host getLocalHost() override;
    };
} // namespace NS_OSBASE::data::impl
