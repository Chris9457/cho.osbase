// \brief Declaration of the log output realization for null (no) output

#pragma once
#include "osData/ILogOutput.h"

namespace NS_OSBASE::data::impl {

    /**
     * \brief Realization of the interface ILogOutput
     */
    class LogOutputNull final : public ILogOutput {
    public:
        void log(std::string &&) override;
        void flush(std::ostream &) override;
    };
} // namespace NS_OSBASE::data::impl
