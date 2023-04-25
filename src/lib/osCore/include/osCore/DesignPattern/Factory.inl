// \file  Factory.inl
// \brief Implementation of classes Factory

namespace NS_OSBASE::core {

    /*
     * \class Factory<TBase, TConcrete, singleton>
     */
    template <class TBase, class TConcrete, bool singleton, typename... TArgs>
    TBase *Factory<TBase, TConcrete, singleton, TArgs...>::create(TArgs &&...args) {
        return new TConcrete(std::forward<TArgs>(args)...);
    }

    template <class TBase, class TConcrete, bool singleton, typename... TArgs>
    void Factory<TBase, TConcrete, singleton, TArgs...>::destroy(TBase *pInstance) {
        delete pInstance;
    }

    /*
     * \class Factory<TBase, TConcrete, true>
     */
    template <class TBase, class TConcrete>
    TBase *Factory<TBase, TConcrete, true>::create() {
        return &TConcrete::getInstance();
    }

    template <class TBase, class TConcrete>
    void Factory<TBase, TConcrete, true>::destroy(TBase *) {
        // Do nothing here because singletons are destroyed by the application
    }

} // namespace NS_OSBASE::core
