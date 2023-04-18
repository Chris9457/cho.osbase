// \brief Implementation of the service Log

#pragma once
#include "osLogSkeleton.h"
#include "osApplication/ServiceImpl.h"
#include "osData/Log.h"
#include "osCore/DesignPattern/Singleton.h"
#include "osCore/Serialization/CoreKeySerializer.h"

namespace cho::osbase::log {

    /**
     * \brief Settings of the service log
     */
    struct LogSettings {
        std::wstring outputFolder;
    };

    class LogServiceImpl : public ILogServiceSkeleton, public core::Singleton<LogServiceImpl> {
        friend Singleton<LogServiceImpl>;

    public:
        data::Uri getInputLogUri() override;
        data::Uri getOutputLogUri() override;

    protected:
        void doDisconnect() override;

    private:
        using DataExchangePtrs = std::unordered_map<data::IDataExchangePtr, data::IDataExchange::IDelegatePtr>;

        class DataExchangeDelegate;
        using DataExchangeDelegatePtr = std::shared_ptr<DataExchangeDelegate>;

        LogServiceImpl();

        void onLogReceived(const std::string &logStream);

        std::vector<data::IDataExchangePtr> m_pDataExchanges;
        data::IDataExchange::IDelegatePtr m_pDataExchangeDelegate;
        data::LogOutputGroupPtr m_pLogOutputGroup;
        std::mutex m_mutex;
        LogSettings m_settings;
    };
#define TheLogServiceImpl LogServiceImpl::getInstance()
} // namespace cho::osbase::log
OS_KEY_SERIALIZE_STRUCT(cho::osbase::log::LogSettings, outputFolder);
