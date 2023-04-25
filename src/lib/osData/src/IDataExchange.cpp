// \brief An abstract interface intended to  define the point to point data exchange between 2 classes

#include "osData/IDataExchange.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace nscore = NS_OSBASE::core;

namespace NS_OSBASE::data {
    namespace {
        const std::unordered_map<std::string, std::string> mapSchemeDataExchangeFactoryName{ { Uri::schemeWebsocket(),
            IDATAEXCHANGE_WEBSOCKET_FACTORY_NAME } };
    } // namespace

    IDataExchangePtr makeDataExchange(const std::string &scheme) {
        auto const itSchemeFactoryName = mapSchemeDataExchangeFactoryName.find(scheme);
        if (itSchemeFactoryName == mapSchemeDataExchangeFactoryName.cend()) {
            return nullptr;
        }

        return nscore::TheFactoryManager.createInstance<IDataExchange>(itSchemeFactoryName->second);
    }

} // namespace NS_OSBASE::data
