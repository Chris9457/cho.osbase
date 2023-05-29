// \brief Implementation of the service Monitoring

#include "logimpl/LogServiceImpl.h"
#include "osData/IDataExchange.h"
#include "osData/Log.h"
#include "osApplication/ServiceConfiguration.h"
#include <fstream>

namespace NS_OSBASE::log {

    /*
     * \class LogServiceImpl
     */
    LogServiceImpl::LogServiceImpl()
        : ILogServiceSkeleton(application::TheServiceConfiguration.getBrokerUri(), application::TheServiceConfiguration.getRealm()),
          m_pLogOutputGroup(data::makeLogOutputGroup()) {

        auto const pStream = core::makeJsonStream(std::ifstream(SETTING_FILE_NAME));
        m_settings         = pStream->getValue(LogSettings{});

        if (m_settings.outputFolder.empty() || (m_settings.outputFolder[m_settings.outputFolder.size() - 1] != L'/' &&
                                                   m_settings.outputFolder[m_settings.outputFolder.size() - 1] != L'\\')) {
            m_settings.outputFolder += L'/';
        }
        std::filesystem::create_directories(m_settings.outputFolder);

        m_pLogOutputGroup->add(data::makeLogOutputConsole());

        auto const t  = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto const tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "log_%Y_%m_%d_%H_%M_%S.log");
        m_pLogOutputGroup->add(data::makeLogOutputFile(std::filesystem::path(m_settings.outputFolder) / oss.str()));
        data::TheLogger.setLogOutput(m_pLogOutputGroup);
    };

    data::AsyncData<std::string> LogServiceImpl::getInputStream() {
        std::lock_guard lock(m_mutex);
        data::AsyncData<std::string> asyncInput;
        auto const guard = core::make_scope_exit([this, &asyncInput] { m_streams.emplace_back(std::move(asyncInput)); });
        asyncInput.create();
        return asyncInput;
    }

    data::AsyncData<std::string> LogServiceImpl::getOutputStream() {
        std::lock_guard lock(m_mutex);
        data::AsyncData<std::string> asyncOutput;
        auto const guard = core::make_scope_exit([this, &asyncOutput] { m_streams.emplace_back(std::move(asyncOutput)); });
        asyncOutput.create();
        m_pLogOutputGroup->add(data::makeLogOutputAsyncData(asyncOutput));
        return asyncOutput;
    }

    void LogServiceImpl::doDisconnect() {
        std::lock_guard lock(m_mutex);
        for (auto &&stream : m_streams) {
            stream.reset();
        }
        ILogServiceSkeleton::doDisconnect();
    }

    void LogServiceImpl::onLogReceived(std::string &&logStream) {
        std::lock_guard lock(m_mutex);
        data::TheLogger.injectLog(std::move(logStream));
    }

} // namespace NS_OSBASE::log
