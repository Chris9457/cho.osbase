// \brief Declaration of the (base) settings of a service

#pragma once
#include "osCore/Serialization/CoreKeySerializer.h"
#include "osData/Uri.h"

namespace NS_OSBASE::application {

    struct ServiceSettingsInput {
        bool bStop = false;
        data::Uri brokerUrl;
        std::string realm;
    };

    struct ServiceSettings {
        std::optional<ServiceSettingsInput> serviceInput;
    };
} // namespace NS_OSBASE::application
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::application::ServiceSettingsInput, bStop, brokerUrl, realm)
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::application::ServiceSettings, serviceInput)
