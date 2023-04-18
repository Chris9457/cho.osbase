// \brief Declaration of the class WerbViewerServiceImpl

#pragma once
#include "osWebViewerSkeleton.h"
#include "osLog.h"
#include "osWebEngine/IWebEngine.h"
#include "osCore/DesignPattern/Singleton.h"

namespace cho::osbase::webviewer {

    /**
     * \brief implementation of the WebViewer service
     */
    class WebViewerServiceImpl : public IWebViewerServiceSkeleton, public core::Singleton<WebViewerServiceImpl> {
        friend core::Singleton<WebViewerServiceImpl>;

    public:
        data::AsyncData<std::vector<char>> captureWindow(ImageFormat format) override;

    protected:
        void doConnect(const std::string &url, const unsigned short port) override;
        void doDisconnect() override;

    private:
        class LogServiceObserver;
        using LogServiceObsreverPtr = std::shared_ptr<LogServiceObserver>;

        WebViewerServiceImpl();
        ~WebViewerServiceImpl() override;

        void onLogServiceConnected(const bool bConnected) const;

        data::AsyncData<std::vector<char>> m_asyncImage;
        webengine::IWebEnginePtr m_pWebEngine;
        log::ILogServicePtr m_pLogService;
        LogServiceObsreverPtr m_pLogServiceObserver;
    };
#define TheWebViewerServiceImpl WebViewerServiceImpl::getInstance()
} // namespace cho::osbase::webviewer
