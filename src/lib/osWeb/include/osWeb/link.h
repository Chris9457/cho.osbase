// \brief Declaration of the factory link of the module osWebEngine

#pragma once
#include "IWebViewerEngine.h"
#include "osCore/DesignPattern/AbstractFactory.h"

#define OS_WEBENGINE_LINK()                                                                                                                \
    namespace NS_OSBASE::webengine {                                                                                                     \
        OS_LINK_FACTORY_N(IWebViewerEngine, WebView2Engine, 0);                                                                            \
        OS_LINK_FACTORY_N(IWebViewerEngine, WebView2Engine, 1);                                                                            \
    }
