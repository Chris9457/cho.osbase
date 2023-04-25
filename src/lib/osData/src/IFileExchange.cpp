// \brief An abstract interface intended to  define the point to point data exchange between 2 classes

#include "osData/IFileExchange.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace nscore = NS_OSBASE::core;

namespace NS_OSBASE::data {

    namespace {
        const std::unordered_map<std::string, std::string> mapSchemeFileExchangeFactoryName{ { Uri::schemeFile(),
            IFILEEXCHANGE_LOCALFILE_FACTORY_NAME } };
    } // namespace

    IFileExchangePtr makeFileExchange(const std::string &scheme) {
        auto const itSchemeFactoryName = mapSchemeFileExchangeFactoryName.find(scheme);
        if (itSchemeFactoryName == mapSchemeFileExchangeFactoryName.cend()) {
            return nullptr;
        }

        return nscore::TheFactoryManager.createInstance<IFileExchange>(itSchemeFactoryName->second);
    }

} // namespace NS_OSBASE::data
