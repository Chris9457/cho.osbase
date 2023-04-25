// \file  TaskLoopException.h
// \brief Declaration of the exceptions of the class TaskLoop

#pragma once
#include "osCore/Exception/RuntimeException.h"

namespace NS_OSBASE::application {

    /**
     * \brief   Generic exception for the class TaskLoop
     * \ingroup PACKAGE_TASK
     */
    class TaskLoopException final : public core::RuntimeException {
    public:
        /**
         * \brief ctor
         * \param   context Precise the context of the generic error (ex: is already running)
         */
        explicit TaskLoopException(const std::string &context);
    };
} // namespace NS_OSBASE::application