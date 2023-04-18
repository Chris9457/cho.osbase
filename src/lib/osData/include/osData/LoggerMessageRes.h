// \brief Declaration of the public resources used for the logger key messages

#pragma once

namespace cho::osbase::data {

    // Tags
    constexpr char LOGGER_TIMESTAMP_FORMAT[] = "%Y-%m-%d %H:%M:%S"; // format: "YYYY-mm-dd HH:MM:SS." - milliseconds are append after
    constexpr char LOGGER_KEY_TIMESTAMP[]    = "timestamp";
    constexpr char LOGGER_KEY_SEVERITY[]     = "severity";
    constexpr char LOGGER_KEY_CHANNEL[]      = "channel";
    constexpr char LOGGER_KEY_CONTENT[]      = "content";
    constexpr char LOGGER_KEY_OBJECT[]       = "object";
    constexpr char LOGGER_KEY_PROCESS[]      = "process";

    // osBase channels
    constexpr auto OS_LOG_CHANNEL_DATA        = 1ull << 0;
    constexpr auto OS_LOG_CHANNEL_APPLICATION = 1ull << 1;

    constexpr auto OS_LOG_OSBASE_CHANNELS = OS_LOG_CHANNEL_DATA | OS_LOG_CHANNEL_APPLICATION;

} // namespace cho::osbase::data
