// \brief Declaration of the class WerbViewerServiceImpl

#pragma once
#include "osWebViewerSkeleton.h"
#include "osWebEngine/IWebEngine.h"

namespace cho::osbase::webviewer {

    /**
     * \brief implementation of the WebViewer service
     */
    class WebViewerServiceImpl : public IWebViewerServiceSkeleton, public core::Singleton<WebViewerServiceImpl> {
        friend core::Singleton<WebViewerServiceImpl>;

    public:
        data::AsyncData<std::vector<char>> captureWindow(ImageFormat format) override;

    private:
        WebViewerServiceImpl();
        ~WebViewerServiceImpl() override;

    protected:
        void doConnect(const std::string &url, const unsigned short port) override;
        void doDisconnect() override;

    private:
        data::AsyncData<std::vector<char>> m_asyncImage;
        webengine::IWebEnginePtr m_pWebEngine;
    };
#define TheWebViewerServiceImpl WebViewerServiceImpl::getInstance()
} // namespace cho::osbase::webviewer
