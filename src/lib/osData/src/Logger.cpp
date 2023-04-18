// \brief Declaration of the class Logger

#include "osData/Logger.h"
#include <sstream>

namespace cho::osbase::data {

    namespace {} // namespace

    /*
     * \class Logger
     */

    Logger::Logger() {
        setChannelMask(~OS_LOG_OSBASE_CHANNELS);
        setSeverityMask(static_cast<Severity_t>(Severity::error));
    }

    Logger &Logger::log() {
        std::lock_guard lock(m_mutex); // scoped-lock mutex to make atomic log
        if (m_pStream == nullptr) {
            return *this;
        }

        auto const guard = core::make_scope_exit([this]() {
            m_currentChannelMask  = m_channelMask;
            m_currentSeverityMask = m_severityMask;
            m_pStream.reset();
            m_mutex.unlock(); // unlock mutex to unblock waiting concurrencing thread
        });

        if (m_pLogOutput != nullptr && (m_currentChannelMask & m_channelMask) != 0 && (m_currentSeverityMask & m_severityMask) != 0) {
            std::ostringstream os;

            os << *m_pStream;
            m_pLogOutput->log(os.str());
        }

        return *this;
    }

    void Logger::setLogOutput(ILogOutputPtr pLogOutput) {
        std::lock_guard lock(m_mutex);
        m_pLogOutput = pLogOutput;
    }

    std::underlying_type_t<Severity> Logger::getSeverityMask() const {
        return m_severityMask;
    }

    void Logger::setSeverityMask(const Severity_t mask) {
        m_severityMask = mask;
    }

    unsigned long long Logger::getChannelMask() const {
        return m_channelMask;
    }

    void Logger::setChannelMask(const unsigned long long mask) {
        m_channelMask = mask;
    }

    bool Logger::injectLog(std::string &&log) {
        std::lock_guard lock(m_mutex);
        if (m_pStream != nullptr) {
            return false;
        }

        m_pStream = createKeyStream(std::forward<std::string>(log));
        m_mutex.lock(); // lock again here to engage a entire log procedure (unlocked in log())
        this->log();
        return true;
    }

    std::shared_ptr<core::KeyStream<std::string>> Logger::createKeyStream() {
        return core::makeJsonStream();
    }

    std::shared_ptr<core::KeyStream<std::string>> Logger::createKeyStream(std::string &&log) {
        return core::makeJsonStream(std::stringstream(std::forward<std::string>(log)));
    }
} // namespace cho::osbase::data
