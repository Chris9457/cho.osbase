// \file  TaskLoopException.h
// \brief Implementationof the exceptions of the class TaskLoop

#include "osApplication/TaskLoopException.h"

namespace NS_OSBASE::application {
    /*
     * \class TaskLoopException
     */
    TaskLoopException ::TaskLoopException(const std::string &context) : RuntimeException("TaskLoop error: " + context) {
    }
} // namespace NS_OSBASE::application
