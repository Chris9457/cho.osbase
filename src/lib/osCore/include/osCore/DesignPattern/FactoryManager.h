// \file  osfactorymanager.h
// \brief Declaration of the class FactoryManager

#pragma once
#include "Factory.h"
#include "Singleton.h"
#include <functional>
#include <map>
#include <vector>

namespace NS_OSBASE::core {

    /** \cond */
    template <typename T>
    struct factory_param {
        using type = typename std::decay<T>::type;
    };
    /** \endcond */

    /**
     * \brief Singleton manager registration factories
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    class FactoryManager : public Singleton<FactoryManager> {
        friend class Singleton<FactoryManager>;

    public:
        //!< Register the factory with its concrete name
        template <class TBase>
        void registerFactory(GenericAbstractFactoryPtr<TBase> pFactory, const std::string &concreteName);

        void unregisterFactory(GenericAbstractFactoryPtr<void> pFactory); //!< Unregister the factory

        std::vector<std::string> getFactoryNames() const; //!< Returns names of all factories

        template <class TBase>
        std::vector<std::string> getFactoryNames() const; //!< Returns names of all factories which base class is TBase

        template <class TBase, typename... TArgs>
        std::shared_ptr<TBase> createInstance(const std::string &concreteName, TArgs &&...args); //!< Create an instance of the concrete
                                                                                                 //!< class named "concreteName" and its
                                                                                                 //!< base class is TBase

    private:
        using NamedFactories  = std::multimap<std::string, GenericAbstractFactoryPtr<void>>;
        using FactoryFamilies = std::map<std::string, NamedFactories>;

        FactoryManager();
        ~FactoryManager();

        std::vector<std::string> getFactoryNames(const std::string &familyName) const;
        template <class TBase, typename... TArgs>
        std::shared_ptr<AbstractFactory<TBase, TArgs...>> getAbstractFactory(const std::string &concreteName) const;

        FactoryFamilies m_factoryFamilies;
    };
#define TheFactoryManager FactoryManager::getInstance() //!< Macro used to access to the singleton FactoryManager

} // namespace NS_OSBASE::core

/** \cond */
#include "FactoryManager.inl"
/** \endcond */
