// \brief Declaration of the class Logger

#pragma once

namespace NS_OSBASE::data {

    /*
     * \class Logger
     */
    template <typename T>
    Logger &Logger::addMessage(LoggerMessage<T> &&msg) {
        std::lock_guard lock(m_mutex); // scoped-lock mutex to make atomic addMessage
        if (m_pStream == nullptr) {
            m_mutex.lock(); // lock mutex until the call to logger end to avoid mix-logging in concurrency (multi-thread)
            m_currentSeverityMask = m_severityMask;
            m_currentChannelMask  = m_channelMask;
            m_pStream             = createKeyStream();
            if (m_pStream == nullptr) {
                return *this;
            }
        }

        if constexpr (std::is_same_v<T, Severity>) {
            m_currentSeverityMask = static_cast<std::underlying_type_t<Severity>>(msg.getValue());
        } else if constexpr (std::is_same_v<T, std::string>) {
            auto const cpyMsg = msg;
            if (cpyMsg.getKey() == LOGGER_KEY_CHANNEL) {
                m_currentChannelMask = LoggerChannel::fromString(cpyMsg.getValue());
            } else if (cpyMsg.getKey() == LOGGER_KEY_CONTENT) {
                auto const newContent = m_pStream->getKeyValue(LOGGER_KEY_CONTENT, std::string()) + cpyMsg.getValue();
                msg.setValue(newContent);
            }
        }

        if ((m_currentSeverityMask & m_severityMask) == 0 || (m_currentChannelMask & m_channelMask) == 0) {
            return *this;
        }

        if constexpr (std::is_void_v<T>) {
            if (m_pStream->getCurrentKey() == msg.getKey()) {
                m_pStream->closeKey();
            } else {
                m_pStream->openKey(msg.getKey());
            }
        } else {
            *m_pStream << msg;
        }

        return *this;
    }

} // namespace NS_OSBASE::data
