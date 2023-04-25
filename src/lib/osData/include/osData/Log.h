// \brief Set of function helpers to log

#pragma once
#include "Logger.h"

/**
 * \defgroup PACKAGE_OSBASE_LOGS Logging
 *
 * \brief This module implements the logging feature
 *
 * \par Structured logs
 * The main approach for this logging feature is to provide for each log a set of key-values information.\n
 * The main advantage of these outputs is to allow an easy wau to expoit the logs by using a large existing set of tools that knows how to
 extract and manipulate the data.\n
 * The key-value format used is the JSon format.
 *
 * \par The singleton "TheLogger"
 * This singleton is the main class to manipulate logs.\n
 * Its main responsibilities are:
 *  - setting the output (predefined or custom)
 *  - build a log stream
 *
 *  \par Timestamps
 *  Each log contains a timestamp record - precision ~millisecond.\n
 *  - Key: "timestamp"
 *  - Format: date time (in ms).\n
 *
 *  \par Process
 *  Each log contains its process info.\n
 *  - Key: "process"
 *  - Value:
 *      - key: "name" - process name
 *      - key: "pid" - process id
 *      - key: "threadId" - thread id
 *
 *  \par Severity
 *  This record represent the level of severity of the log.\n
 *  - Key: "severity"
 *  - Available values: info (1), warning (2), error (4), debug (8), trace (16).\n
 *
 *  \par Channel
 *  This record add a "channel" information level of the log.\n
 *  The implementation acts as a mask based on 64 bits stringifyed in hexadecimal (ex: "0x100D80065F0000A4").\n
 *  Remark: a best practice is to use a "single bit" mask by using the bitwise operator << (1ull << channel, with channel < 64).\n
 *  - Key: "channel"
 *  - Value: hexadecimal string: "0xHHHHHHHHHHHHHHHH".\n
 *  Example:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   oslog::channel(1ul << 3) << "my log" << oslog::end();
   // output result: { "channel":"0x0000000000000008","content":"my log"}
   // an external tool analyser can apply a mask on the key "channel" to add or ignore this log
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 *  \par Content
 *  The content is added to the log when streaming a string (std::string, char *) or an arithmetic value (char, int, float...).
 *  - Key: "content"
 *  - Value: string content
 * Example:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   int count = 2;
   oslog::info() << "count info: " << count << oslog::end(); // log the content "count info: 2"
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Object
 * An object when adding a key-serilizable type other than strings and arithmethic values.\n
 * - Key: object
 * - Value: serialized reprensentation of the type.\n
 * Remark: the log can contains only ONE object, streaming several objects overrides the record with the last one.
 * Example:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
    struct TestObject {
        int value = 0;
        std::string strValue;
    };
    OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::data::ut::TestObject, value, strValue);

    oslog::info() << "my_object" << TestObject{2, "4"} << oslog::end(); // log contains "object":{"value":2,"strValue":"4"}
    oslog::info() << "my_object" << TestObject{1, "3"} << TestObject{2, "4"} << oslog::end(); // override, log contains
 "object":{"value":2,"strValue":"4"}
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par The log outputs
 * The log outputs are the concrete realizations to flush a log stream on the desire transport (file, websocket...)
 *
 * \par Log manipulations
 * The choosen way to build a log stream is by using the stream operator "<<" (such like for stl - ostreams).\n
 * The effective writing of the log is done by ending with "oslog::end()"
 *
 * \ingroup PACKAGE_OSDATA
 */

namespace oslog {
    /**
     * \addtogroup PACKAGE_OSBASE_LOGS
     * \{
     */

    /**
     * \brief used to ends (write) the log
     * \return TheLogger
     */
    NS_OSBASE::data::LoggerEnd end();

    /**
     * \brief used to start the log with the severity "info"
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &info();

    /**
     * \brief used to start the log with the severity "info" and the channel "channelValue"
     * \param channelValue value of the channel
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &info(const unsigned long long channelValue);

    /**
     * \brief used to start the log with the severity "warning"
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &warning();

    /**
     * \brief used to start the log with the severity "warning" and the channel "channelValue"
     * \param channelValue value of the channel
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &warning(const unsigned long long channelValue);

    /**
     * \brief used to start the log with the severity "error"
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &error();

    /**
     * \brief used to start the log with the severity "error" and the channel "channelValue"
     * \param channelValue value of the channel
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &error(const unsigned long long channelValue);

    /**
     * \brief used to start the log with the severity "debug"
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &debug();

    /**
     * \brief used to start the log with the severity "debug" and the channel "channelValue"
     * \param channelValue value of the channel
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &debug(const unsigned long long channelValue);

    /**
     * \brief used to start the log with the severity "trace"
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &trace();

    /**
     * \brief used to start the log with the severity "trace" and the channel "channelValue"
     * \param channelValue value of the channel
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &trace(const unsigned long long channelValue);

    /**
     * \brief add the channel
     * \param channelValue value of the channel
     * \return  TheLogger
     */
    NS_OSBASE::data::Logger &channel(const unsigned long long channelValue);

    /**
     * \brief add the process info
     * \return TheLogger
     */
    NS_OSBASE::data::Logger &process();

    /** \} */

} // namespace oslog

/** \cond */
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::Logger &);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerEnd &&msg);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerTimestamp &&msg);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, std::chrono::system_clock::time_point &&tp);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerProcess &&msg);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerSeverity &&msg);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerChannel &&msg);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerContent &&msg);

NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, std::string &&content);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, const std::string &content);
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, const char content[]);

template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerMessage<T> &&msg);
template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, T &&object);
template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, const T &object);
template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, T &object);
/** \endcond */

#include "Log.inl"
