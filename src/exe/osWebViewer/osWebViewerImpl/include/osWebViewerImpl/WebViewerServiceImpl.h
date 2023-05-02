// \brief Declaration of the class WerbViewerServiceImpl

#pragma once
#include "osWebViewerSkeleton.h"
#include "osLog.h"
#include "osWeb/IWebViewerEngine.h"
#include "osCore/DesignPattern/Singleton.h"

namespace NS_OSBASE::webviewer {

    /**
     * \brief implementation of the WebViewer service
     */
    class WebViewerServiceImpl : public IWebViewerServiceSkeleton, public core::Singleton<WebViewerServiceImpl> {
        friend core::Singleton<WebViewerServiceImpl>;

    public:
        data::AsyncData<std::vector<char>> captureWindow(ImageFormat format) override;

    protected:
        void doConnect() override;
        void doDisconnect() override;

    private:
        class LogServiceObserver;
        using LogServiceObsreverPtr = std::shared_ptr<LogServiceObserver>;

        WebViewerServiceImpl();
        ~WebViewerServiceImpl() override;

        void onLogServiceConnected(const bool bConnected) const;

        data::AsyncData<std::vector<char>> m_asyncImage;
        webengine::IWebViewerEnginePtr m_pWebEngine;
        log::ILogServicePtr m_pLogService;
        LogServiceObsreverPtr m_pLogServiceObserver;
    };
#define TheWebViewerServiceImpl WebViewerServiceImpl::getInstance()
} // namespace NS_OSBASE::webviewer
