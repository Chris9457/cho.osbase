// \brief Definition of the interface of the logs

#include "osData/ILogOutput.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace nscore = NS_OSBASE::core;

namespace NS_OSBASE::data {
    ILogOutputPtr makeLogOutputNull() {
        return nscore::TheFactoryManager.createInstance<ILogOutput>(LOGOUTPUT_NULL_FACTORY_NAME);
    }

    ILogOutputPtr makeLogOutputFile(const std::filesystem::path &path) {
        return nscore::TheFactoryManager.createInstance<ILogOutput>(LOGOUTPUT_FILE_FACTORY_NAME, path);
    }

    ILogOutputPtr makeLogOutputDataExchange(const Uri &uri) {
        return nscore::TheFactoryManager.createInstance<ILogOutput>(LOGOUTPUT_DATAEXCHANGE_FACTORY_NAME, uri);
    }

    ILogOutputPtr makeLogOutputDataExchange(IDataExchangePtr pDataExchange) {
        return nscore::TheFactoryManager.createInstance<ILogOutput>(LOGOUTPUT_DATAEXCHANGE_FACTORY_NAME, pDataExchange);
    }

    ILogOutputPtr makeLogOutputAsyncString(const AsyncString &asyncString) {
        return nscore::TheFactoryManager.createInstance<ILogOutput>(LOGOUTPUT_ASYNCSTRING_FACTORY_NAME, asyncString);
    }

    ILogOutputPtr makeLogOutputAsyncString(const Uri &uri) {
        return makeLogOutputAsyncString(AsyncData<std::string>(uri));
    }

    ILogOutputPtr makeLogOutputConsole() {
        return nscore::TheFactoryManager.createInstance<ILogOutput>(LOGOUTPUT_CONSOLE_FACTORY_NAME);
    }

    ILogOutputPtr makeLogOutputDebug() {
        return nscore::TheFactoryManager.createInstance<ILogOutput>(LOGOUTPUT_DEBUG_FACTORY_NAME);
    }

} // namespace NS_OSBASE::data
