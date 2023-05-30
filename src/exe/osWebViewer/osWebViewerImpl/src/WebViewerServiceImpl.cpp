// \brief Declaration of the class WerbViewerServiceImpl

#include "osWebViewerImpl/WebViewerServiceImpl.h"
#include "osCore/DesignPattern/Observer.h"
#include "osData/Log.h"
#include "osApplication/ServiceConfiguration.h"
#include <fstream>

using namespace std::chrono_literals;

namespace NS_OSBASE::webviewer {

    /*
     * \class WebViewerServiceImpl::LogServiceObserver
     */
    class WebViewerServiceImpl::LogServiceObserver : public core::Observer<application::ServiceConnectionMsg> {
    public:
        void update(const core::Observable &, const application::ServiceConnectionMsg &msg) override {
            TheWebViewerServiceImpl.onLogServiceConnected(msg.isConnected());
        }
    };

    /*
     * \class WerbViewerServiceImpl
     */
    WebViewerServiceImpl::WebViewerServiceImpl()
        : IWebViewerServiceSkeleton(application::TheServiceConfiguration.getBrokerUri(), application::TheServiceConfiguration.getRealm()),
          m_pLogService(log::makeStub(getBrokerUri(), getRealm(), getTaskLoop())),
          m_pLogServiceObserver(std::make_shared<LogServiceObserver>()) {
        auto const pStream  = core::makeJsonStream(std::ifstream(SETTING_FILE_NAME));
        auto const settings = pStream->getValue(webengine::Settings{});

        m_pWebEngine = webengine::makeWebViewerEngine(settings);
        m_pWebEngine->runAsync();
    }

    WebViewerServiceImpl::~WebViewerServiceImpl() {
        m_pWebEngine->stop();
    }

    data::AsyncData<std::vector<char>> WebViewerServiceImpl::captureWindow(ImageFormat format) {
        oslog::info() << "capture image format: " << type_cast<std::string>(format) << oslog::end();
        getTaskLoop()->push([this, format]() { m_asyncImage.set(m_pWebEngine->captureWindow(type_cast<std::string>(format))); });
        return m_asyncImage;
    }

    void WebViewerServiceImpl::doConnect() {
        m_asyncImage.create();

        m_pLogService->attachAll(*m_pLogServiceObserver);
        m_pLogService->connect();
    }

    void WebViewerServiceImpl::doDisconnect() {
        m_pLogService->disconnect();
        m_pLogService->detachAll(*m_pLogServiceObserver);

        m_asyncImage.reset();
    }

    void WebViewerServiceImpl::onLogServiceConnected(const bool bConnected) const {
        if (bConnected) {
            auto const pLogOutput = data::makeLogOutputGroup();
            pLogOutput->add(data::makeLogOutputConsole());
            pLogOutput->add(data::makeLogOutputAsyncString(m_pLogService->getInputStream()));
            data::TheLogger.setLogOutput(pLogOutput);

            oslog::info() << "WebViewerServiceImpl::onLogServiceConnected: " << bConnected << oslog::end();
        }
    }

} // namespace NS_OSBASE::webviewer
