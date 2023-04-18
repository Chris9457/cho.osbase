// \file  TaskLoopException.h
// \brief Implementationof the exceptions of the class TaskLoop

#include "osApplication/TaskLoopException.h"

namespace cho::osbase::application {
    /*
     * \class TaskLoopException
     */
    TaskLoopException ::TaskLoopException(const std::string &context) : RuntimeException("TaskLoop error: " + context) {
    }
} // namespace cho::osbase::application
