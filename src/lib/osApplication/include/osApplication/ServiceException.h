/// \brief Declaration of the services' exceptions

#pragma once
#include "osCore/Exception/RuntimeException.h"

namespace NS_OSBASE::application {
    class ServiceException : public core::RuntimeException {
    public:
        using core::RuntimeException::RuntimeException;
    };
} // namespace NS_OSBASE::application
