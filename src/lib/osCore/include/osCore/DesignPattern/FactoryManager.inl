// \file  FactoryManager.inl
// \brief Implementation of the class FactoryManager

namespace cho::osbase::core {

    /*
     * \class FactoryManager
     */
    template <class TBase>
    void FactoryManager::registerFactory(GenericAbstractFactoryPtr<TBase> pFactory, const std::string &concreteName) {
        const std::string familyName(typeid(TBase).name());
        FactoryFamilies::iterator itFactoryFamily = m_factoryFamilies.find(familyName);

        if (itFactoryFamily == m_factoryFamilies.end())
            itFactoryFamily = m_factoryFamilies.insert(FactoryFamilies::value_type(familyName, NamedFactories())).first;

        itFactoryFamily->second.insert(NamedFactories::value_type(concreteName, pFactory));
    }

    template <class TBase>
    std::vector<std::string> FactoryManager::getFactoryNames() const {
        const std::string familyName = typeid(TBase).name();
        return getFactoryNames(familyName);
    }

    template <class TBase, typename... TArgs>
    std::shared_ptr<TBase> FactoryManager::createInstance(const std::string &concreteName, TArgs &&...args) {
        auto pAbstractFactory = getAbstractFactory<TBase, typename factory_param<TArgs>::type...>(concreteName);
        if (pAbstractFactory != nullptr) {
            using namespace std::placeholders;
            return std::shared_ptr<TBase>(pAbstractFactory->create(std::forward<typename factory_param<TArgs>::type>(
                                              static_cast<typename factory_param<TArgs>::type>(args))...),
                std::bind(&AbstractFactory<TBase, typename factory_param<TArgs>::type...>::destroy, pAbstractFactory, _1));
        }

        return nullptr;
    }

    template <class TBase, typename... TArgs>
    std::shared_ptr<AbstractFactory<TBase, TArgs...>> FactoryManager::getAbstractFactory(const std::string &concreteName) const {
        std::shared_ptr<AbstractFactory<TBase, TArgs...>> pAbstractFactory;
        const std::string familyName                    = typeid(TBase).name();
        FactoryFamilies::const_iterator itFactoryFamily = m_factoryFamilies.find(familyName);

        if (itFactoryFamily != m_factoryFamilies.end()) {
            for (auto namedFactory : itFactoryFamily->second) {
                if (namedFactory.first == concreteName) {
                    pAbstractFactory = std::dynamic_pointer_cast<AbstractFactory<TBase, TArgs...>>(namedFactory.second);
                    if (pAbstractFactory != nullptr)
                        break;
                }
            }
        }

        return pAbstractFactory;
    }

} // namespace cho::osbase::core
