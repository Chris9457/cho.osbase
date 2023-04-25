// \brief Declaration of the interface IWebEngine

#include "osWeb/IWebViewerEngine.h"
#include "FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace NS_OSBASE::webengine {

    /*
     * \Maker
     */
    IWebViewerEnginePtr makeWebViewerEngine() {
        return core::TheFactoryManager.createInstance<IWebViewerEngine>(WEBENGINE_WEBVIEW2);
    }

    IWebViewerEnginePtr makeWebViewerEngine(const Settings &settings) {
        return core::TheFactoryManager.createInstance<IWebViewerEngine>(WEBENGINE_WEBVIEW2, settings);
    }
} // namespace NS_OSBASE::webengine
