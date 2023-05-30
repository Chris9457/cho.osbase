// \brief Declaration of the class LogOutputAsyncData

#pragma once
#include "osData/ILogOutput.h"
#include "osData/AsyncData.h"

namespace NS_OSBASE::data::impl {

    /**
     * \brief concrete implementation for asyncdata
     */
    class LogOutputAsyncString : public ILogOutput {
    public:
        LogOutputAsyncString(const AsyncData<std::string> &asyncLog);

        void log(std::string &&msg) override;
        void flush(std::ostream &os) override;

    private:
        void onConnected(const bool bConnected);

        AsyncData<std::string> m_asyncLog;
        std::vector<std::string> m_bufferedLogs;
        bool m_bConnected = false;
        std::mutex m_mutLog;
    };
} // namespace NS_OSBASE::data::impl
