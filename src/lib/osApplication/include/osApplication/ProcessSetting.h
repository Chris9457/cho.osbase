// \brief Declaration of the settings structure for the class Process

#pragma once
#include "osCore/Serialization/CoreKeySerializer.h"
#include <string>
#include <vector>

namespace cho::osbase::application {
    struct ProcessSetting {
        std::string serviceName;
        bool displayConsole            = false;
        bool autoRestart               = false;
        bool killIfExist               = true;
        unsigned int endProcessTimeout = 100; //!< time to wait in milliseconds after calling endProcess
    };
    using ProcessSettings = std::vector<ProcessSetting>;
} // namespace cho::osbase::application
OS_KEY_SERIALIZE_STRUCT(
    cho::osbase::application::ProcessSetting, serviceName, displayConsole, autoRestart, killIfExist, endProcessTimeout)
