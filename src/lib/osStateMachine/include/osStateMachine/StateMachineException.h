// \brief Declaration of the exception of the state machine

#pragma once
#include "osCore/Exception/Exception.h"

namespace NS_OSBASE::statemachine {

    /**
     * \brief Generic exception for the state machines
     * \ingroup PACKAGE_OSSTATEENGINE
     */
    class StateMachineException : public core::RuntimeException {
        using core::RuntimeException::RuntimeException;
    };
} // namespace NS_OSBASE::statemachine
