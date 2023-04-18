// \brief Declaration of the log output realization for files

#pragma once
#include "osData/ILogOutput.h"
#include <filesystem>
#include <fstream>

namespace cho::osbase::data::impl {

    /**
     * \brief Realization of the interface ILogOutput
     */
    class LogOutputFile final : public ILogOutput {
    public:
        LogOutputFile(const std::filesystem::path &path);
        void log(std::string &&msg) override;
        void flush(std::ostream &os) override;

    private:
        std::ofstream m_ostream;
        std::vector<std::string> m_noFileBuffer;
    };
} // namespace cho::osbase::data::impl
