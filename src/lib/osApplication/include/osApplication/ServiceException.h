/// \brief Declaration of the services' exceptions

#pragma once
#include "osCore/Exception/RuntimeException.h"

namespace cho::osbase::application {
    class ServiceException : public core::RuntimeException {
    public:
        using core::RuntimeException::RuntimeException;
    };
} // namespace cho::osbase::application
