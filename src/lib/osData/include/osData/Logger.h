// \brief Declaration of the class Logger

#pragma once
#include "ILogOutput.h"
#include "LogOutputGroup.h"
#include "osData/LoggerMessageRes.h"
#include "LoggerMessage.h"
#include "osCore/DesignPattern/Singleton.h"
#include "osCore/Serialization/KeyStream.h"
#include <mutex>

namespace NS_OSBASE::data {
    /**
     * \addtogroup PACKAGE_OSBASE_LOGS
     * \{
     */

    /**
     * \brief Main singleton allowing the log output
     */
    class Logger final : public core::Singleton<Logger> {
        friend core::Singleton<Logger>;

    public:
        void setLogOutput(ILogOutputPtr pLogOutput); //!< assign the output for the logger

        Severity_t getSeverityMask() const;          //!< return the current mask for the severity
        void setSeverityMask(const Severity_t mask); //!< assign the current severity mask

        unsigned long long getChannelMask() const;          //!< return the current channel mask
        void setChannelMask(const unsigned long long mask); //!< assign the current channel mask

        template <typename T>
        Logger &addMessage(LoggerMessage<T> &&msg); //!< add a new message (key-value)

        Logger &log();                     //!< log to the output the current construction
        bool injectLog(std::string &&log); //!< inject to the output the log

    private:
        Logger();
        ~Logger() override = default;

        static std::shared_ptr<core::KeyStream<std::string>> createKeyStream();
        static std::shared_ptr<core::KeyStream<std::string>> createKeyStream(std::string &&log);

        mutable std::recursive_mutex m_mutex;
        ILogOutputPtr m_pLogOutput;
        std::shared_ptr<core::KeyStream<std::string>> m_pStream;
        std::underlying_type_t<Severity> m_severityMask{ static_cast<std::underlying_type_t<Severity>>(-1) };
        std::underlying_type_t<Severity> m_currentSeverityMask{ static_cast<std::underlying_type_t<Severity>>(-1) };
        unsigned long long m_channelMask{ static_cast<unsigned long long>(-1) };
        unsigned long long m_currentChannelMask{ static_cast<unsigned long long>(-1) };
    };

#define TheLogger Logger::getInstance() //!< macro helper to invoke the singleton
    /** \} */
} // namespace NS_OSBASE::data

#include "Logger.inl"
