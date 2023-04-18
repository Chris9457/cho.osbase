// \biref Declaration of a named mutex

#include "osCore/Misc/RecursiveNamedMutex.h"
#include "RecursiveNamedMutexImpl.h"

namespace cho::osbase::core {
    recursive_named_mutex::recursive_named_mutex() : m_pImpl(std::make_shared<recursive_named_mutex_impl>()) {
    }

    recursive_named_mutex::recursive_named_mutex(const std::string &name) : m_pImpl(std::make_shared<recursive_named_mutex_impl>(name)) {
    }

    void recursive_named_mutex::lock() {
        m_pImpl->lock();
    }

    bool recursive_named_mutex::try_lock() {
        return m_pImpl->try_lock();
    }

    void recursive_named_mutex::unlock() {
        m_pImpl->unlock();
    }

    void *recursive_named_mutex::native_handle() const {
        return m_pImpl->native_handle();
    }
} // namespace cho::osbase::core
