// \brief Declaration of the class EdgeWebEngine

#pragma once
#include "osWeb/IWebViewerEngine.h"
#include <wil/com.h>
#include <WebView2.h>
#include <Windows.h>
#include <mutex>

namespace cho::osbase::webengine {

    /*
     * \brief Implementation for the Microsoft Edge Web Engine
     */
    class WebView2Engine : public IWebViewerEngine, public std::enable_shared_from_this<IWebViewerEngine> {
    public:
        WebView2Engine();
        WebView2Engine(const Settings &settings);
        ~WebView2Engine() override;

        void run() override;
        std::shared_future<void> runAsync() override;
        bool isRunning() const override;
        void stop() override;
        Settings getSettings() const override;
        void setSettings(const Settings &settings) override;
        void navigate(const data::Uri &uri) override;
        std::vector<char> captureWindow(const std::string &imageFormat) const override;

    private:
        void createWindow();
        void destroyWindow();
        void createWebView();
        void destroyWebView();
        HWND getHWnd() const;
        void resize();

        static LRESULT CALLBACK onWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        static void registerEdgeWebEngine(WebView2Engine *pEdgeWebEngine);
        static void unregisterEdgeWebEngine(WebView2Engine *pEdgeWebEngine);
        static WebView2Engine *getWebEngine(const HWND hWnd);

        Settings m_settings;
        std::shared_future<void> m_futRunAsync;
        bool m_bRunning = false;
        HWND m_hWnd     = nullptr;
        ATOM m_registeredClass{};
        wil::com_ptr<ICoreWebView2Controller> m_pWebViewController;
        wil::com_ptr<ICoreWebView2> m_pWebView;
        std::wstring m_className;

        static std::recursive_mutex s_EdgeWebEngineInstanceLock;
        static std::unordered_map<HWND, WebView2Engine *> s_mapHwndEdgeWebEngine;
    };
} // namespace cho::osbase::webengine
