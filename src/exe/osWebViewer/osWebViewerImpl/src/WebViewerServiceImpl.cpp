// \brief Declaration of the class WerbViewerServiceImpl

#include "osWebViewerImpl/WebViewerServiceImpl.h"
#include "osCore/DesignPattern/Observer.h"
#include "osData/Log.h"
#include <fstream>

using namespace std::chrono_literals;

namespace cho::osbase::webviewer {

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
        : m_pLogService(log::makeStub(getTaskLoop())), m_pLogServiceObserver(std::make_shared<LogServiceObserver>()) {
        auto const pStream  = core::makeJsonStream(std::ifstream(SETTING_FILE_NAME));
        auto const settings = pStream->getValue(webengine::Settings{});

        m_pWebEngine = webengine::makeWebEngine(settings);
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

    void WebViewerServiceImpl::doConnect(const std::string &url, const unsigned short port) {
        m_asyncImage.create();

        m_pLogService->attachAll(*m_pLogServiceObserver);
        m_pLogService->connect(url, port);
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
            pLogOutput->add(data::makeLogOutputDataExchange(m_pLogService->getInputLogUri()));
            data::TheLogger.setLogOutput(pLogOutput);

            oslog::info() << "WebViewerServiceImpl::onLogServiceConnected: " << bConnected << oslog::end();
        }
    }

} // namespace cho::osbase::webviewer
