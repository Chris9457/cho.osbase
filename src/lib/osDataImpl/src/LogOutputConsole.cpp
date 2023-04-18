// \brief Declaration of the log output realization for the console

#include "LogOutputConsole.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include <iostream>

namespace cho::osbase::data::impl {
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputConsole, 0, LOGOUTPUT_CONSOLE_FACTORY_NAME)

    /*
     * \class LogOutputConsole
     */
    void LogOutputConsole::log(std::string &&msg) {
        std::cout << msg << std::endl;
    }

    void LogOutputConsole::flush(std::ostream &) {
    }
} // namespace cho::osbase::data::impl
