// \brief Declaration of the class LogOutputAsyncData

#include "LogOutputAsyncString.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace NS_OSBASE::data::impl {
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputAsyncString, 0, LOGOUTPUT_ASYNCSTRING_FACTORY_NAME, AsyncString);

    LogOutputAsyncString::LogOutputAsyncString(const AsyncData<std::string> &asyncLog) : m_asyncLog(asyncLog) {
        m_asyncLog.connect([this](const bool bConnected) { onConnected(bConnected); });
    }

    void LogOutputAsyncString::log(std::string &&msg) {
        std::lock_guard lock(m_mutLog);
        if (m_bConnected) {
            m_asyncLog.set(std::move(msg));
        } else {
            m_bufferedLogs.emplace_back(std::move(msg));
        }
    }

    void LogOutputAsyncString::flush(std::ostream &os) {
        std::lock_guard lock(m_mutLog);
        for (auto &&bufferedLog : m_bufferedLogs) {
            os << bufferedLog << std::endl;
        }
    }

    void LogOutputAsyncString::onConnected(const bool bConnected) {
        std::lock_guard lock(m_mutLog);
        m_bConnected = bConnected;
        if (bConnected) {
            for (auto &&bufferedLog : m_bufferedLogs) {
                m_asyncLog.set(std::move(bufferedLog));
            }
            m_bufferedLogs.clear();
        }
    }
} // namespace NS_OSBASE::data::impl
