// \file  LogicException.h
// \brief Declaration of the class LogicException

#pragma once

#include <stdexcept>

namespace NS_OSBASE::core {

    /**
     * \brief Thrown when an expected error occurs
     * \ingroup PACKAGE_EXCEPTION
     */
    class LogicException : public std::logic_error {
        using std::logic_error::logic_error;
    };
} // namespace NS_OSBASE::core
