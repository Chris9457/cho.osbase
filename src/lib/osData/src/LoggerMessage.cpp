// \brief Declaration of the basics key messages for the Logger

#include "osData/LoggerMessage.h"
#include "osData/LoggerMessageRes.h"
#include "osCore/Interprocess/ProcessPID.h"
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <thread>

namespace NS_OSBASE::data {

    /*
     * \class LoggerTimestamp
     */
    LoggerTimestamp::LoggerTimestamp(const clock::time_point &timePoint) : m_timePoint(timePoint) {
    }

    const std::chrono::system_clock::time_point LoggerTimestamp::getTimePoint() const {
        return m_timePoint;
    }

    std::string LoggerTimestamp::toString() const {
        std::stringstream os;
        auto const tt                = clock::to_time_t(m_timePoint);
        auto const tm                = *std::localtime(&tt);
        auto const fractionalSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(m_timePoint.time_since_epoch());

        os << std::put_time(&tm, LOGGER_TIMESTAMP_FORMAT) << "." << std::setw(3) << std::setfill('0') << (fractionalSeconds.count() % 1000);
        return os.str();
    }

    LoggerTimestamp LoggerTimestamp::fromString(const std::string &strTimeStamp) {
        // Extract the fraction
        std::string strDateTime = strTimeStamp;
        int fraction            = 0;
        auto const pos          = strTimeStamp.find('.');
        if (pos != std::string::npos) {
            strDateTime = strTimeStamp.substr(0, pos);
            fraction    = std::stoi(strTimeStamp.substr(pos + 1));
        }

        std::tm t = {};
        std::stringstream iss(strTimeStamp);
        iss >> std::get_time(&t, LOGGER_TIMESTAMP_FORMAT);

        // adjust offset UTC
        std::tm epoch     = {};
        epoch.tm_mday     = 2;
        epoch.tm_year     = 70;
        auto const offset = mktime(&epoch) - 60 * 60 * 24;

        auto tp = clock::from_time_t(mktime(&t) + offset);
        tp += std::chrono::milliseconds(fraction);
        return LoggerTimestamp(tp);
    }

    /*
     * \class LoggerSeverity
     */
    Severity_t operator|(const Severity lhs, const Severity &rhs) {
        return static_cast<Severity_t>(lhs) | static_cast<Severity_t>(rhs);
    }

    Severity_t operator|(const Severity lhs, const Severity_t &rhs) {
        return static_cast<Severity_t>(lhs) | rhs;
    }

    Severity_t operator|(const Severity_t lhs, const Severity &rhs) {
        return rhs | lhs;
    }

    Severity_t operator&(const Severity lhs, const Severity &rhs) {
        return static_cast<Severity_t>(lhs) & static_cast<Severity_t>(rhs);
    }

    Severity_t operator&(const Severity lhs, const Severity_t &rhs) {
        return static_cast<Severity_t>(lhs) & rhs;
    }

    Severity_t operator&(const Severity_t lhs, const Severity &rhs) {
        return rhs & lhs;
    }

    LoggerSeverity::LoggerSeverity(const Severity &severity) : m_severity(severity) {
    }

    Severity LoggerSeverity::getSeverity() const {
        return m_severity;
    }

    /*
     * \class LoggerChannel
     */
    LoggerChannel::LoggerChannel(const unsigned long long channel) : m_channel(channel) {
    }

    unsigned long long LoggerChannel::getChannel() const {
        return m_channel;
    }

    std::string LoggerChannel::toString() const {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::setw(sizeof(m_channel) * 2) << std::uppercase << std::setfill('0') << m_channel;
        return oss.str();
    }

    unsigned long long LoggerChannel::fromString(const std::string &str) {
        std::istringstream iss(str);
        unsigned long long channel = 0;
        char dummyChar             = 0;
        iss >> dummyChar >> dummyChar >> std::hex >> channel;
        return channel;
    }

    /*
     * \class LoggerContent
     */
    LoggerContent::LoggerContent(const std::string &content) : m_content(content) {
    }

    LoggerContent::LoggerContent(std::string &&content) : m_content(content) {
    }

    const std::string &LoggerContent::getContent() const {
        return m_content;
    }

    /*
     * \class LoggerProcess
     */
    LoggerProcess::Info LoggerProcess::getInfo() {
        Info info;
        std::ostringstream oss;
        oss << "0x" << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << core::getCurrentPID();
        info.pid  = oss.str();
        info.name = std::filesystem::path(core::getCurrentProcessName()).filename().wstring();

        oss.str("");
        oss << "0x" << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << std::this_thread::get_id();
        info.threadId = oss.str();
        return info;
    }
} // namespace NS_OSBASE::data
