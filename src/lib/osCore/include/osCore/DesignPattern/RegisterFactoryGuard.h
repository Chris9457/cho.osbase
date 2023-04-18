// \file  RegisterFactoryGuard.h
// \brief Declaration of the class RegisterFactoryGuard

#pragma once
#include "FactoryManager.h"
#include "osCore/Misc/NonCopyable.h"

namespace cho::osbase::core {

    /**
     * \brief	This class is a RAII implementation of the auto unregistering
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    class RegisterFactoryGuard : private NonCopyable {
    public:
        /**
         * \brief	Create a registering guard
         *
         * \param[in]	concreteName	Name of the concrete factory
         * \remark		This method creates the associated concrete factory and register it to the factory manager
         */
        template <class TBase, class TConcrete, typename... TArgs>
        static RegisterFactoryGuard create(const std::string &concreteName);

        template <class TBase, class TConcrete, typename... TArgs>
        static RegisterFactoryGuard create(const char *concreteName); //!< \copydoc RegisterFactoryGuard::create(const std::string&)

        RegisterFactoryGuard(RegisterFactoryGuard &&rhs) noexcept; //!< Move ctor
        ~RegisterFactoryGuard();                                   //!< Dtor - Unregister the factory

        bool link() noexcept; //!< This method is used only to force the link of factories

    private:
        RegisterFactoryGuard(GenericAbstractFactoryPtr<void> pFactory) noexcept;

        GenericAbstractFactoryWeakPtr<void> m_pFactory;
    };

} // namespace cho::osbase::core

/** \cond */
#include "RegisterFactoryGuard.inl"
/** \endcond */
