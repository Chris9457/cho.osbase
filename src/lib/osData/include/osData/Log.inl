// \brief Set of function helpers to log

#pragma once

template <typename T>
cho::osbase::data::Logger &operator<<(cho::osbase::data::Logger &logger, cho::osbase::data::LoggerMessage<T> &&msg) {
    return logger.addMessage(std::forward<cho::osbase::data::LoggerMessage<T>>(msg));
}

template <typename T>
cho::osbase::data::Logger &operator<<(cho::osbase::data::Logger &logger, T &&object) {
    if constexpr (std::is_arithmetic_v<T>) {
        return logger << cho::osbase::data::LoggerContent(std::to_string(object));
    } else {
        return logger << cho::osbase::data::LoggerMessage<T>(cho::osbase::data::LOGGER_KEY_OBJECT, std::forward<T>(object));
    }
}

template <typename T>
cho::osbase::data::Logger &operator<<(cho::osbase::data::Logger &logger, const T &object) {
    return logger << std::move(T(object));
}

template <typename T>
cho::osbase::data::Logger &operator<<(cho::osbase::data::Logger &logger, T &object) {
    return logger << std::move(T(object));
}
