// \brief Declaration of the log output realization for the console

#pragma once
#include "osData/ILogOutput.h"

namespace NS_OSBASE::data::impl {

    /**
     * \brief Realization of the interface ILogOutput
     */
    class LogOutputConsole final : public ILogOutput {
    public:
        void log(std::string &&msg) override;
        void flush(std::ostream &os) override;
    };
} // namespace NS_OSBASE::data::impl
