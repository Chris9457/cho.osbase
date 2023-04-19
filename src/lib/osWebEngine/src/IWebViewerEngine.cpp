// \brief Declaration of the interface IWebEngine

#include "osWebEngine/IWebViewerEngine.h"
#include "FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace cho::osbase::webengine {

    /*
     * \Maker
     */
    IWebViewerEnginePtr makeWebViewerEngine() {
        return core::TheFactoryManager.createInstance<IWebViewerEngine>(WEBENGINE_WEBVIEW2);
    }

    IWebViewerEnginePtr makeWebViewerEngine(const Settings &settings) {
        return core::TheFactoryManager.createInstance<IWebViewerEngine>(WEBENGINE_WEBVIEW2, settings);
    }
} // namespace cho::osbase::webengine
