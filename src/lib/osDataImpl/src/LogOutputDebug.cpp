// \brief Declaration of the log output realization for the console

#include "LogOutputDebug.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include <Windows.h>

namespace cho::osbase::data::impl {
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputDebug, 0, LOGOUTPUT_DEBUG_FACTORY_NAME)

    /*
     * \class LogOutputConsole
     */
    void LogOutputDebug::log(std::string &&msg) {
        OutputDebugStringA((msg + "\n").c_str());
    }

    void LogOutputDebug::flush(std::ostream &) {
    }
} // namespace cho::osbase::data::impl
