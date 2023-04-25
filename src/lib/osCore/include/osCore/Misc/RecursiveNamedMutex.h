// \biref Declaration of a named mutex

#pragma once
#include <memory>
#include <string>

namespace NS_OSBASE::core {

    class recursive_named_mutex_impl;
    using recursive_named_mutex_impl_ptr = std::shared_ptr<recursive_named_mutex_impl>;

    /**
     * \brief This class represent a recursive names mutex
     * \remark The notation used is voluntarily align with those of the stl to keep the same wording
     * \ingroup PACKAGE_MISC
     */
    class recursive_named_mutex {
    public:
        recursive_named_mutex();                                 //!< ctor
        explicit recursive_named_mutex(const std::string &name); //!< ctor with name

        void lock();     //!< lock th mutex
        bool try_lock(); //!< try to lock the mutex - return immediatelly, the result depends if the putex is alreaduy locked
        void unlock();   //!< unlock the mutex

        void *native_handle() const; //!< return the native handle of the mutex (depends on the OS implementation)

    private:
        recursive_named_mutex_impl_ptr m_pImpl;
    };
} // namespace NS_OSBASE::core
