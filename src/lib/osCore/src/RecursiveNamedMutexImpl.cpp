// \brief Private implementation of the class recursive_named_mutex

#include "RecursiveNamedMutexImpl.h"

namespace cho::osbase::core {
    recursive_named_mutex_impl::recursive_named_mutex_impl(const std::string &name) : m_handle(CreateMutexA(nullptr, false, name.c_str())) {
    }

    recursive_named_mutex_impl::~recursive_named_mutex_impl() {
        if (m_handle != nullptr) {
            CloseHandle(m_handle);
        }
    }

    void recursive_named_mutex_impl::lock() {
        lock(INFINITE);
    }

    bool recursive_named_mutex_impl::try_lock() {
        return lock(0);
    }

    void recursive_named_mutex_impl::unlock() {
        if (m_handle != nullptr) {
            ReleaseMutex(m_handle);
        }
    }

    bool recursive_named_mutex_impl::lock(const DWORD timeout) {
        return m_handle != nullptr ? WaitForSingleObject(m_handle, timeout) == WAIT_OBJECT_0 : false;
    }

    HANDLE recursive_named_mutex_impl::native_handle() const {
        return m_handle;
    }

} // namespace cho::osbase::core
