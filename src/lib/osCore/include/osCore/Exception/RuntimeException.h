// \file  RuntimeException.h
// \brief Declaration of the class RuntimeException

#pragma once

#include <stdexcept>

namespace cho::osbase::core {

    /**
     * \brief Thrown when an unexpected error occurs (bug but no crash)
     * \ingroup PACKAGE_EXCEPTION
     */
    class RuntimeException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
} // namespace cho::osbase::core
