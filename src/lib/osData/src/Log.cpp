// \brief Set of function helpers to log

#include "osData/Log.h"

namespace nsosbase = cho::osbase;
namespace nsdata   = nsosbase::data;

namespace oslog {
    namespace {
        nsdata::Logger &prefix() {
            return nsdata::TheLogger << std::chrono::system_clock::now() << process();
        }
    } // namespace

    nsdata::LoggerEnd end() {
        return {};
    }

    nsdata::Logger &info() {
        return prefix() << nsdata::LoggerSeverity(nsdata::Severity::info);
    }

    nsdata::Logger &info(const unsigned long long channelValue) {
        return info() << channel(channelValue);
    }

    nsdata::Logger &warning() {
        return prefix() << nsdata::LoggerSeverity(nsdata::Severity::warning);
    }

    nsdata::Logger &warning(const unsigned long long channelValue) {
        return warning() << channel(channelValue);
    }

    nsdata::Logger &error() {
        return prefix() << nsdata::LoggerSeverity(nsdata::Severity::error);
    }

    nsdata::Logger &error(const unsigned long long channelValue) {
        return error() << channel(channelValue);
    }

    nsdata::Logger &debug() {
        return prefix() << nsdata::LoggerSeverity(nsdata::Severity::debug);
    }

    nsdata::Logger &debug(const unsigned long long channelValue) {
        return debug() << channel(channelValue);
    }

    nsdata::Logger &trace() {
        return prefix() << nsdata::LoggerSeverity(nsdata::Severity::trace);
    }

    nsdata::Logger &trace(const unsigned long long channelValue) {
        return trace() << channel(channelValue);
    }

    nsdata::Logger &channel(const unsigned long long channelValue) {
        return nsdata::TheLogger << nsdata::LoggerChannel(channelValue);
    }

    nsdata::Logger &process() {
        return nsdata::TheLogger << nsdata::LoggerProcess();
    }

} // namespace oslog

nsdata::Logger &operator<<(nsdata::Logger &logger, nsdata::Logger &) {
    return logger;
}

nsdata::Logger &operator<<(nsdata::Logger &logger, nsdata::LoggerEnd &&) {
    return logger.log();
}

nsdata::Logger &operator<<(nsdata::Logger &logger, nsdata::LoggerTimestamp &&msg) {
    return logger.addMessage(nsdata::LoggerMessage<std::string>(nsdata::LOGGER_KEY_TIMESTAMP, msg.toString()));
}

cho::osbase::data::Logger &operator<<(cho::osbase::data::Logger &logger, std::chrono::system_clock::time_point &&tp) {
    return logger << nsdata::LoggerTimestamp(tp);
}

nsdata::Logger &operator<<(nsdata::Logger &logger, nsdata::LoggerProcess &&msg) {
    return logger.addMessage(nsdata::LoggerMessage<nsdata::LoggerProcess::Info>(nsdata::LOGGER_KEY_PROCESS, msg.getInfo()));
}

nsdata::Logger &operator<<(nsdata::Logger &logger, nsdata::LoggerSeverity &&msg) {
    return logger.addMessage(nsdata::LoggerMessage<nsdata::Severity>(nsdata::LOGGER_KEY_SEVERITY, msg.getSeverity()));
}

nsdata::Logger &operator<<(nsdata::Logger &logger, nsdata::LoggerChannel &&msg) {
    return logger.addMessage(nsdata::LoggerMessage<std::string>(nsdata::LOGGER_KEY_CHANNEL, msg.toString()));
}

nsdata::Logger &operator<<(nsdata::Logger &logger, nsdata::LoggerContent &&msg) {
    return logger.addMessage(nsdata::LoggerMessage<std::string>(nsdata::LOGGER_KEY_CONTENT, msg.getContent()));
}

nsdata::Logger &operator<<(nsdata::Logger &logger, std::string &&content) {
    return logger << nsdata::LoggerContent(std::forward<std::string>(content));
}

nsdata::Logger &operator<<(nsdata::Logger &logger, const std::string &content) {
    return logger << nsdata::LoggerContent(content);
}

nsdata::Logger &operator<<(nsdata::Logger &logger, const char content[]) {
    return logger << nsdata::LoggerContent(content);
}
