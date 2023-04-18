// \brief Private implementation of the class recursive_named_mutex

#pragma once
#include "osCore/Misc/RecursiveNamedMutex.h"

#include <Windows.h>
#include <string>

namespace cho::osbase::core {

    class recursive_named_mutex_impl {
    public:
        recursive_named_mutex_impl() = default;
        explicit recursive_named_mutex_impl(const std::string &name);
        ~recursive_named_mutex_impl();

        void lock();
        bool try_lock();
        void unlock();

        HANDLE native_handle() const;

    private:
        bool lock(const DWORD timeout);

        HANDLE m_handle = nullptr;
    };
} // namespace cho::osbase::core
