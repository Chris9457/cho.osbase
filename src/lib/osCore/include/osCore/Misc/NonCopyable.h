// \file  NonCopyable.h
// \brief Declaration of the class NonCopyable

#pragma once

namespace cho::osbase::core {

    /**
     * \brief Base class to declare non copyable classes
     * \ingroup PACKAGE_MISC
     */
    class NonCopyable {
    public:
        NonCopyable(const NonCopyable &) = delete;
        NonCopyable &operator=(const NonCopyable &) = delete;

    protected:
        NonCopyable()  = default;
        ~NonCopyable() = default;
    };
} // namespace cho::osbase::core