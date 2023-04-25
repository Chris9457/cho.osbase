// \brief Declaration of the basics key messages for the Logger

#pragma once
#include "osCore/Serialization/CoreKeySerializer.h"
#include <string>
#include <chrono>

namespace NS_OSBASE::data {
    /**
     * \addtogroup PACKAGE_OSBASE_LOGS
     * \{
     */

    /**
     * \brief Class that invokes the log
     */
    class LoggerEnd final {};

    /**
     * \brief Timestamp of the log
     */
    class LoggerTimestamp final {
    public:
        using clock = std::chrono::system_clock; //!< alias for the used clock

        explicit LoggerTimestamp(const clock::time_point &timePoint = clock::now()); //!< ctor

        const clock::time_point getTimePoint() const; //!< return the related time point

        std::string toString() const;                                       //!< return the formated date time (use LOGGER_TIMESTAMP_FORMAT)
        static LoggerTimestamp fromString(const std::string &strTimeStamp); //!< return the LoggerTimestamp from the formated string

    private:
        clock::time_point m_timePoint;
    };

    /**
     * \brief Severity applied to the current log
     */
    enum class Severity { info = 1 << 0, warning = 1 << 1, error = 1 << 2, debug = 1 << 3, trace = 1 << 4 };
    using Severity_t = std::underlying_type_t<Severity>; //!< alias for the underlying type of Severity (useful for masks)

    /** \cond */
    Severity_t operator|(const Severity lhs, const Severity &rhs);
    Severity_t operator|(const Severity lhs, const Severity_t &rhs);
    Severity_t operator|(const Severity_t lhs, const Severity &rhs);

    Severity_t operator&(const Severity lhs, const Severity &rhs);
    Severity_t operator&(const Severity lhs, const Severity_t &rhs);
    Severity_t operator&(const Severity_t lhs, const Severity &rhs);
    /** \endcond */

    /**
     * \brief Class that set the severity of the log
     */
    class LoggerSeverity final {
    public:
        explicit LoggerSeverity(const Severity &severity); //!< ctor
        Severity getSeverity() const;                      //!< return the serverity

    private:
        Severity m_severity = Severity::info;
    };

    /**
     * \brief Class that set the channel of the log
     */
    class LoggerChannel final {
    public:
        explicit LoggerChannel(const unsigned long long channel); //!< ctor

        unsigned long long getChannel() const;                        //!< return the channel
        std::string toString() const;                                 //!< return the channel stringifyed (0XHHHHHHHHHHHHHHHH)
        static unsigned long long fromString(const std::string &str); //!< parse the stringifyed channel

    private:
        unsigned long long m_channel = 0xFFFFFFFFFFFFFFFF;
    };

    /**
     * \brief Class that set a content
     */
    class LoggerContent final {
    public:
        explicit LoggerContent(const std::string &content); //!< ctor
        LoggerContent(std::string &&content);               //!< move ctor

        const std::string &getContent() const; //!< return the content

    private:
        std::string m_content;
    };

    /**
     * \brief Class that set the process id
     */
    class LoggerProcess final {
    public:
        /**
         * \brief gathers the process info
         */
        struct Info {
            std::wstring name;    //!< name of the process (exe file name, without the path)
            std::string pid;      //!< process id
            std::string threadId; //!< current thread id (in the process)
        };

        static Info getInfo(); //!< return the process information
    };

    /**
     * \brief Generic message
     */
    template <typename T>
    using LoggerMessage = core::KeyValue<std::string, T>; //!< alias for generic messages for logs
    /** \} */

} // namespace NS_OSBASE::data
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::data::LoggerProcess::Info, name, pid, threadId);
