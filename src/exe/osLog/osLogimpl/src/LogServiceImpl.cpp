// \brief Implementation of the service Monitoring

#include "logimpl/LogServiceImpl.h"
#include "osData/IDataExchange.h"
#include "osData/Log.h"
#include "osApplication/ServiceConfiguration.h"
#include <fstream>

namespace NS_OSBASE::log {

    /*
     * \class LogServiceImpl::DataExchangeDelegate
     */
    class LogServiceImpl::DataExchangeDelegate : public data::IDataExchange::IDelegate {
    public:
        void onConnected(const bool) override {
        }

        void onFailure(std::string &&) override {
        }

        void onDataReceived(data::ByteBuffer &&buffer) override {
            TheLogServiceImpl.getTaskLoop()->push(
                [logStream = type_cast<std::string>(std::move(buffer))]() { TheLogServiceImpl.onLogReceived(logStream); });
        }
    };

    /*
     * \class LogServiceImpl
     */
    LogServiceImpl::LogServiceImpl()
        : ILogServiceSkeleton(application::TheServiceConfiguration.getBrokerUri(), application::TheServiceConfiguration.getRealm()),
          m_pDataExchangeDelegate(std::make_shared<DataExchangeDelegate>()),
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

    data::Uri LogServiceImpl::getInputLogUri() {
        std::lock_guard lock(m_mutex);
        auto const pInputDataExchange = data::makeDataExchange();
        pInputDataExchange->setDelegate(m_pDataExchangeDelegate);
        m_pDataExchanges.push_back(pInputDataExchange);
        pInputDataExchange->create();
        return pInputDataExchange->getUriOfCreator();
    }

    data::Uri LogServiceImpl::getOutputLogUri() {
        std::lock_guard lock(m_mutex);
        auto const pOutputDataExchange = data::makeDataExchange();
        pOutputDataExchange->create();
        m_pDataExchanges.push_back(pOutputDataExchange);

        m_pLogOutputGroup->add(data::makeLogOutputDataExchange(pOutputDataExchange));
        return pOutputDataExchange->getUriOfCreator();
    }

    void LogServiceImpl::doDisconnect() {
        std::lock_guard lock(m_mutex);
        for (auto &&pDataExchange : m_pDataExchanges) {
            pDataExchange->destroy();
        }
        ILogServiceSkeleton::doDisconnect();
    }

    void LogServiceImpl::onLogReceived(const std::string &logStream) {
        std::lock_guard lock(m_mutex);
        data::TheLogger.injectLog(std::string(logStream));
    }

} // namespace NS_OSBASE::log
