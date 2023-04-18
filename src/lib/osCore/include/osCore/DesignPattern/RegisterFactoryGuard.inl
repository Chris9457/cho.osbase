// \file  RegisterFactoryGuard.inl
// \brief Implementation of the class RegisterFactoryGuard

namespace cho::osbase::core {

    /*
     * \class RegisterFactoryGuard
     */
    template <class TBase, class TConcrete, typename... TArgs>
    RegisterFactoryGuard RegisterFactoryGuard::create(const std::string &concreteName) {
        using FactoryType =
            Factory<TBase, TConcrete, std::is_base_of<Singleton<TConcrete>, TConcrete>::value, typename factory_param<TArgs>::type...>;
        auto pFactory = std::make_shared<FactoryType>(FactoryType());

        TheFactoryManager.registerFactory(std::static_pointer_cast<GenericAbstractFactory<TBase>>(pFactory), concreteName);
        return RegisterFactoryGuard(pFactory);
    }

    template <class TBase, class TConcrete, typename... TArgs>
    RegisterFactoryGuard RegisterFactoryGuard::create(const char *concreteName) {
        return RegisterFactoryGuard::create<TBase, TConcrete, TArgs...>(std::string(concreteName));
    }

} // namespace cho::osbase::core
