// \brief Declaration of the class WerbViewerServiceImpl

#include "osWebViewerImpl/WebVieverServiceImpl.h"
#include <fstream>

using namespace std::chrono_literals;
namespace nsosbase    = cho::osbase;
namespace nscore      = nsosbase::core;
namespace nswebengine = nsosbase::webengine;

namespace cho::osbase::webviewer {

    /*
     * \class WerbViewerServiceImpl
     */
    WebViewerServiceImpl::WebViewerServiceImpl() {
        auto const pStream  = nscore::makeJsonStream(std::ifstream(SETTING_FILE_NAME));
        auto const settings = pStream->getValue(nswebengine::Settings{});

        m_pWebEngine = nswebengine::makeWebEngine(settings);
        m_pWebEngine->runAsync();
    }

    WebViewerServiceImpl::~WebViewerServiceImpl() {
        m_pWebEngine->stop();
    }

    data::AsyncData<std::vector<char>> WebViewerServiceImpl::captureWindow([[maybe_unused]] ImageFormat format) {
        getTaskLoop()->push([this, format]() { m_asyncImage.set(m_pWebEngine->captureWindow(type_cast<std::string>(format))); });
        return m_asyncImage;
    }

    void WebViewerServiceImpl::doConnect([[maybe_unused]] const std::string &url, [[maybe_unused]] const unsigned short port) {
        m_asyncImage.create();
    }

    void WebViewerServiceImpl::doDisconnect() {
        m_asyncImage.reset();
    }

} // namespace cho::osbase::webviewer
