// \file  ProcessPID.cpp
// \brief Implementation of the functions related to the process

#include "osCore/Interprocess/ProcessPID.h"
#include <windows.h>

namespace NS_OSBASE::core {
    /*
     * getCurrentPID
     */
    unsigned long getCurrentPID() {
        return ::GetCurrentProcessId();
    }

    std::string getCurrentProcessName() {
        char processName[MAX_PATH];
        ::GetModuleFileNameA(nullptr, processName, sizeof(processName));
        return processName;
    }

} // namespace NS_OSBASE::core
