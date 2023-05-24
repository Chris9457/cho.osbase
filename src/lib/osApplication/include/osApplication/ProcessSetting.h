// \brief Declaration of the settings structure for the class Process

#pragma once
#include "osCore/Serialization/CoreKeySerializer.h"
#include <string>
#include <vector>

namespace NS_OSBASE::application {
    struct ProcessSetting {
        std::string serviceName;
        bool displayConsole            = false;
        bool autoRestart               = false;
        bool killIfExist               = true;
        unsigned int endProcessTimeout = 10000; //!< time to wait in milliseconds after calling endProcess
    };
    using ProcessSettings = std::vector<ProcessSetting>;
} // namespace NS_OSBASE::application
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::application::ProcessSetting, serviceName, displayConsole, autoRestart, killIfExist, endProcessTimeout)
