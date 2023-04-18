// \brief Declaration of the exception of the state machine

#pragma once
#include "osCore/Exception/Exception.h"

namespace cho::osbase::statemachine {

    /**
     * \brief Generic exception for the state machines
     * \ingroup PACKAGE_OSSTATEENGINE
     */
    class StateMachineException : public core::RuntimeException {
        using core::RuntimeException::RuntimeException;
    };
} // namespace cho::osbase::statemachine
