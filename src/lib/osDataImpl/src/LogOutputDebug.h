// \brief Declaration of the log output realization for the console

#pragma once
#include "osData/ILogOutput.h"

namespace cho::osbase::data::impl {

    /**
     * \brief Realization of the interface ILogOutput
     */
    class LogOutputDebug final : public ILogOutput {
    public:
        void log(std::string &&msg) override;
        void flush(std::ostream &os) override;
    };
} // namespace cho::osbase::data::impl
