// \brief Declaration of the log output realization for files

#include "LogOutputFile.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace cho::osbase::data::impl {
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputFile, 0, LOGOUTPUT_FILE_FACTORY_NAME, std::filesystem::path)

    /*
     * \class LogOutputFile
     */
    LogOutputFile::LogOutputFile(const std::filesystem::path &path) try : m_ostream(path) {
    } catch (const std::filesystem::filesystem_error &) {
    }

    void LogOutputFile::log(std::string &&msg) {
        if (m_ostream.is_open()) {
            m_ostream << msg << std::endl;
        } else {
            m_noFileBuffer.push_back(std::forward<std::string>(msg));
        }
    }

    void LogOutputFile::flush(std::ostream &os) {
        for (auto const &msg : m_noFileBuffer) {
            os << msg << std::endl;
        }
    }
} // namespace cho::osbase::data::impl
