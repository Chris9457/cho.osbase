// \brief Declaration of the class LogOutputAsyncData

#include "LogOutputAsyncData.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace NS_OSBASE::data::impl {
    using AsyncString = AsyncData<std::string>;
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputAsyncData, 0, LOGOUTPUT_ASYNCDATA_FACTORY_NAME, AsyncString);

    LogOutputAsyncData::LogOutputAsyncData(const AsyncData<std::string> &asyncLog) : m_asyncLog(asyncLog) {
        m_asyncLog.connect([this](const bool bConnected) { onConnected(bConnected); });
    }

    void LogOutputAsyncData::log(std::string &&msg) {
        std::lock_guard lock(m_mutLog);
        if (m_bConnected) {
            m_asyncLog.set(std::move(msg));
        } else {
            m_bufferedLogs.emplace_back(std::move(msg));
        }
    }

    void LogOutputAsyncData::flush(std::ostream &os) {
        std::lock_guard lock(m_mutLog);
        for (auto &&bufferedLog : m_bufferedLogs) {
            os << bufferedLog << std::endl;
        }
    }

    void LogOutputAsyncData::onConnected(const bool bConnected) {
        std::lock_guard lock(m_mutLog);
        m_bConnected = bConnected;
        if (bConnected) {
            for (auto &&bufferedLog : m_bufferedLogs) {
                log(std::move(bufferedLog));
            }
            m_bufferedLogs.clear();
        }
    }
} // namespace NS_OSBASE::data::impl
