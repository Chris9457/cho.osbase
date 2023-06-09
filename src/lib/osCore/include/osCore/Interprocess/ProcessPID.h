// \file  ProcessPID.h
// \brief Declaration of the functions related to the process

#pragma once
#include <string>

namespace NS_OSBASE::core {

    /**
     * \brief Return the identifier of th process running the application
     * \ingroup PACKAGE_INTERPROCESS
     */

    unsigned long getCurrentPID(); //!< Return the current process ID

    std::string getCurrentProcessName(); //!< Return the current process name

} // namespace NS_OSBASE::core
