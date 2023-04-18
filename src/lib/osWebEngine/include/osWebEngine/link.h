// \brief Declaration of the factory link of the module osWebEngine

#pragma once
#include "IWebEngine.h"
#include "osCore/DesignPattern/AbstractFactory.h"

#define OS_WEBENGINE_LINK()                                                                                                                \
    namespace cho::osbase::webengine {                                                                                              \
        OS_LINK_FACTORY_N(IWebEngine, WebView2WebEngine, 0);                                                                               \
        OS_LINK_FACTORY_N(IWebEngine, WebView2WebEngine, 1);                                                                               \
    }
