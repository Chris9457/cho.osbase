// \brief Set of function helpers to log

#pragma once

template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, NS_OSBASE::data::LoggerMessage<T> &&msg) {
    return logger.addMessage(std::forward<NS_OSBASE::data::LoggerMessage<T>>(msg));
}

template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, T &&object) {
    if constexpr (std::is_arithmetic_v<T>) {
        return logger << NS_OSBASE::data::LoggerContent(std::to_string(object));
    } else {
        return logger << NS_OSBASE::data::LoggerMessage<T>(NS_OSBASE::data::LOGGER_KEY_OBJECT, std::forward<T>(object));
    }
}

template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, const T &object) {
    return logger << std::move(T(object));
}

template <typename T>
NS_OSBASE::data::Logger &operator<<(NS_OSBASE::data::Logger &logger, T &object) {
    return logger << std::move(T(object));
}
