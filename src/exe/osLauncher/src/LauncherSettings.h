// \brief Definition of the settings for the laucher

#pragma once
#include "osApplication//Process.h"
#include "osCore/Serialization/CoreKeySerializer.h"
#include "osData/IMessaging.h"
#include <string>
#include <vector>

namespace NS_OSBASE::launcher {

    struct LauncherSettings {
        data::Uri brokerUrl;
        std::string realm = data::IMessaging::DEFAULT_REALM;
        application::ProcessSettings settings;
    };
} // namespace NS_OSBASE::launcher

OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::launcher::LauncherSettings, brokerUrl, realm, settings)
