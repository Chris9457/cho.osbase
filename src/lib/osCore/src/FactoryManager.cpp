// \file  FactoryManager.cpp
// \brief Implementation of the class FactoryManager

#include "osCore/DesignPattern/FactoryManager.h"
#include <algorithm>
#include <iterator>

namespace cho::osbase::core {

    /*
     * \class FactoryManager
     */
    FactoryManager::FactoryManager()  = default;
    FactoryManager::~FactoryManager() = default;

    void FactoryManager::unregisterFactory(GenericAbstractFactoryPtr<void> pFactory) {
        FactoryFamilies::iterator itFactoryFamily      = m_factoryFamilies.begin();
        FactoryFamilies::iterator itFactoryFamilyEnd   = m_factoryFamilies.end();
        FactoryFamilies::iterator itFactoryFamilyFound = itFactoryFamilyEnd;

        for (; (itFactoryFamily != itFactoryFamilyEnd) && (itFactoryFamilyFound == itFactoryFamilyEnd); ++itFactoryFamily) {
            NamedFactories::iterator itNamedFactory      = itFactoryFamily->second.begin();
            NamedFactories::iterator itNamedFactoryEnd   = itFactoryFamily->second.end();
            NamedFactories::iterator itNamedFactoryFound = itNamedFactoryEnd;

            for (; (itNamedFactory != itNamedFactoryEnd) && (itNamedFactoryFound == itNamedFactoryEnd); ++itNamedFactory) {
                if (itNamedFactory->second.get() == pFactory.get()) {
                    itNamedFactoryFound  = itNamedFactory;
                    itFactoryFamilyFound = itFactoryFamily;
                }
            }

            if (itNamedFactoryFound != itNamedFactoryEnd)
                itFactoryFamily->second.erase(itNamedFactoryFound);
        }

        if ((itFactoryFamilyFound != itFactoryFamilyEnd) && (itFactoryFamilyFound->second.empty()))
            m_factoryFamilies.erase(itFactoryFamilyFound);
    }

    std::vector<std::string> FactoryManager::getFactoryNames() const {
        std::vector<std::string> factoryNames;
        for (const auto &factoryFamily : m_factoryFamilies) {
            auto subFactoryNames = getFactoryNames(factoryFamily.first);
            std::copy(subFactoryNames.cbegin(), subFactoryNames.cend(), std::back_inserter(factoryNames));
        }

        return factoryNames;
    }

    std::vector<std::string> FactoryManager::getFactoryNames(const std::string &familyName) const {
        std::vector<std::string> factoryNames;
        FactoryFamilies::const_iterator itFactotyFamily = m_factoryFamilies.find(familyName);

        if (itFactotyFamily != m_factoryFamilies.end()) {
            for (auto &&namedFactory : itFactotyFamily->second) {
                if (std::find(factoryNames.cbegin(), factoryNames.cend(), namedFactory.first) == factoryNames.cend())
                    factoryNames.push_back(namedFactory.first);
            }
        }

        return factoryNames;
    }

} // namespace cho::osbase::core
