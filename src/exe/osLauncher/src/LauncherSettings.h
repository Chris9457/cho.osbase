// \brief Definition of the settings for the laucher

#pragma once
#include "osApplication//Process.h"
#include "osCore/Serialization/CoreKeySerializer.h"
#include <string>
#include <vector>

namespace cho::osbase::launcher {

    struct LauncherSettings {
        std::string brokerUrl;
        unsigned short brokerPort = 8080;
        application::ProcessSettings settings;
    };
} // namespace cho::osbase::launcher

OS_KEY_SERIALIZE_STRUCT(cho::osbase::launcher::LauncherSettings, brokerUrl, brokerPort, settings)
