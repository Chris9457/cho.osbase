// \brief Declaration of the interface IWebEngine

#include "osWebEngine/IWebEngine.h"
#include "FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace cho::osbase::webengine {

    /*
     * \Maker
     */
    IWebEnginePtr makeWebEngine() {
        return core::TheFactoryManager.createInstance<IWebEngine>(WEBENGINE_WEBVIEW2);
    }

    IWebEnginePtr makeWebEngine(const Settings &settings) {
        return core::TheFactoryManager.createInstance<IWebEngine>(WEBENGINE_WEBVIEW2, settings);
    }
} // namespace cho::osbase::webengine
