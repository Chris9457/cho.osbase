// \file  RegisterFactoryGuard.cpp
// \brief Implementation of the class RegisterFactoryGuard

#include "osCore/DesignPattern/RegisterFactoryGuard.h"

namespace NS_OSBASE::core {

    /*
     * \class RegisterFactoryGuard
     */
    RegisterFactoryGuard::RegisterFactoryGuard(GenericAbstractFactoryPtr<void> pFactory) noexcept : m_pFactory(pFactory) {
    }

    RegisterFactoryGuard::RegisterFactoryGuard(RegisterFactoryGuard &&rhs) noexcept : m_pFactory(rhs.m_pFactory.lock()) {
        rhs.m_pFactory.reset();
    }

    RegisterFactoryGuard::~RegisterFactoryGuard() {
        auto pFactory = m_pFactory.lock();
        if (pFactory != nullptr)
            TheFactoryManager.unregisterFactory(pFactory);
    }

    bool RegisterFactoryGuard::link() noexcept {
        return true;
    }

} // namespace NS_OSBASE::core
