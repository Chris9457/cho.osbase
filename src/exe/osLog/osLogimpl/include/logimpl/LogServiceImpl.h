// \brief Implementation of the service Log

#pragma once
#include "osLogSkeleton.h"
#include "osApplication/ServiceImpl.h"
#include "osData/Log.h"
#include "osCore/DesignPattern/Singleton.h"
#include "osCore/Serialization/CoreKeySerializer.h"

namespace NS_OSBASE::log {

    /**
     * \brief Settings of the service log
     */
    struct LogSettings {
        std::wstring outputFolder;
    };

    class LogServiceImpl : public ILogServiceSkeleton, public core::Singleton<LogServiceImpl> {
        friend Singleton<LogServiceImpl>;

    public:
        NS_OSBASE::data::AsyncData<std::string> getInputStream() override;
        NS_OSBASE::data::AsyncData<std::string> getOutputStream() override;

    protected:
        void doDisconnect() override;

    private:
        LogServiceImpl();

        void onLogReceived(std::string &&logStream);

        std::vector<data::AsyncData<std::string>> m_streams;
        data::LogOutputGroupPtr m_pLogOutputGroup;
        std::mutex m_mutex;
        LogSettings m_settings;
    };
#define TheLogServiceImpl LogServiceImpl::getInstance()
} // namespace NS_OSBASE::log
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::log::LogSettings, outputFolder);
