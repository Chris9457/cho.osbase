// \brief Declaration of the log output realization for null (no) output

#include "LogOutputNull.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include <iostream>

namespace cho::osbase::data::impl {
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputNull, 0, LOGOUTPUT_NULL_FACTORY_NAME)

    /*
     * \class LogOutputNull
     */
    void LogOutputNull::log(std::string &&) {
    }

    void LogOutputNull::flush(std::ostream &) {
    }
} // namespace cho::osbase::data::impl
