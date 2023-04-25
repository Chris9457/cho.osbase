// \brief Definition of the settings for the laucher

#pragma once
#include "osApplication//Process.h"
#include "osCore/Serialization/CoreKeySerializer.h"
#include <string>
#include <vector>

namespace NS_OSBASE::launcher {

    struct LauncherSettings {
        std::string brokerUrl;
        unsigned short brokerPort = 8080;
        application::ProcessSettings settings;
    };
} // namespace NS_OSBASE::launcher

OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::launcher::LauncherSettings, brokerUrl, brokerPort, settings)
