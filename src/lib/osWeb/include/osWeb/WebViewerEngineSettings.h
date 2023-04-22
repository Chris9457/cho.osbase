// \brief Declaration of the settings for the web engine

#pragma once
#include "osData/Uri.h"
#include "osCore/Serialization/CoreKeySerializer.h"

namespace cho::osbase::webengine {

    enum class DisplayMode { Windowed, FullScreen };

    struct WindowSettings {
        DisplayMode displayMode;
        std::wstring title;
    };

    /**
     * \brief Settings used for the web engine
     */
    struct BrowserSettings {
        data::Uri startUrl                  = type_cast<data::Uri>(std::string("https://www.google.fr"));
        bool areDefaultContextMenusEnabled  = false;
        bool areDefaultScriptDialogsEnabled = true;
        bool areDevToolsEnabled             = false;
        bool areHostObjectsAllowed          = false;
        bool isBuiltInErrorPageEnabled      = false;
        bool isScriptEnabled                = true;
        bool isStatusBarEnabledEnabled      = false;
        bool isWebMessageEnabled            = false;
        bool isZoomControlEnabled           = false;
    };

    struct Settings {
        WindowSettings window;
        BrowserSettings browser;
    };
} // namespace cho::osbase::webengine
OS_KEY_SERIALIZE_STRUCT(cho::osbase::webengine::WindowSettings, displayMode, title);
OS_KEY_SERIALIZE_STRUCT(cho::osbase::webengine::BrowserSettings,
    startUrl,
    areDefaultContextMenusEnabled,
    areDefaultScriptDialogsEnabled,
    areDevToolsEnabled,
    areHostObjectsAllowed,
    isBuiltInErrorPageEnabled,
    isScriptEnabled,
    isStatusBarEnabledEnabled,
    isWebMessageEnabled,
    isZoomControlEnabled);
OS_KEY_SERIALIZE_STRUCT(cho::osbase::webengine::Settings, window, browser);
