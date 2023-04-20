// \brief Declaration of the class EdgeWebEngine

#include "WebView2Engine.h"
#include "FactoryNames.h"
#include "CaptureWindowManager.h"
#include "osData/INetwork.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Misc/ScopeValue.h"
#include <tchar.h>
#include <wrl.h>
#include <iostream>
#include <fstream>

using namespace Microsoft::WRL;

namespace cho::osbase::webengine {
    OS_REGISTER_FACTORY_N(IWebViewerEngine, WebView2Engine, 0, WEBENGINE_WEBVIEW2)
    OS_REGISTER_FACTORY_N(IWebViewerEngine, WebView2Engine, 1, WEBENGINE_WEBVIEW2, Settings)

    namespace {
        constexpr TCHAR windowClassName[] = _T("osBaseWebEngine");
        size_t instanceCount              = 0;

    } // namespace

    /*
     * \class WebView2WebEngine
     */
    std::recursive_mutex WebView2Engine::s_EdgeWebEngineInstanceLock;
    std::unordered_map<HWND, WebView2Engine *> WebView2Engine::s_mapHwndEdgeWebEngine;

    WebView2Engine::WebView2Engine() : WebView2Engine(Settings{}) {
    }

    WebView2Engine::WebView2Engine(const Settings &settings)
        : m_settings(settings), m_className(windowClassName + std::to_wstring(instanceCount++)) {
        if (m_settings.browser.startUrl.authority.has_value() && m_settings.browser.startUrl.authority.value().host.isLocal()) {
            auto const pNetwork                                = data::makeNetwork();
            m_settings.browser.startUrl.authority.value().host = pNetwork->getLocalHost();
        }
    }

    WebView2Engine::~WebView2Engine() {
        if (isRunning()) {
            stop();
        }
    }

    /*
     * \class EdgeWebEngine
     */
    void WebView2Engine::run() {
        if (isRunning()) {
            throw WebEngineException("web engine is already running");
        }

        auto const guard = core::make_scope_value(m_bRunning, true);
        createWindow();
        createWebView();
        registerEdgeWebEngine(this);
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        destroyWebView();
        destroyWindow();
        unregisterEdgeWebEngine(this);
    }

    std::shared_future<void> WebView2Engine::runAsync() {
        if (isRunning()) {
            throw WebEngineException("web engine is already running");
        }

        m_futRunAsync = std::async([this]() { run(); });
        return m_futRunAsync;
    }

    bool WebView2Engine::isRunning() const {
        return m_bRunning;
    }

    void WebView2Engine::stop() {
        if (isRunning()) {
            PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        }

        if (m_futRunAsync.valid()) {
            m_futRunAsync.wait();
        }
    }

    void WebView2Engine::navigate(const data::Uri &uri) {
        if (!isRunning()) {
            throw WebEngineException("navigation forbidden while not running");
        }

        if (m_pWebView == nullptr) {
            throw WebEngineException("navigation failed - web view 2 not instantiated");
        }

        if (m_pWebView->Navigate(type_cast<std::wstring>(uri).c_str()) == E_INVALIDARG) {
            throw WebEngineException("navigate to a bad url: " + type_cast<std::string>(uri));
        }
    }

    std::vector<char> WebView2Engine::captureWindow(const std::string &imageFormat) const {
        return TheCaptureWindowManager.captureWindow(m_hWnd, imageFormat);
    }

    Settings WebView2Engine::getSettings() const {
        return m_settings;
    }

    void WebView2Engine::setSettings(const Settings &settings) {
        m_settings = settings;
        if (m_pWebView) {
            wil::com_ptr<ICoreWebView2Settings> pSettings;
            m_pWebView->get_Settings(&pSettings);
            if (m_pWebView->get_Settings(&pSettings) != S_OK) {
                throw WebEngineException("unable to acquire the web engine settings");
            }

            pSettings->put_AreDefaultContextMenusEnabled(settings.browser.areDefaultContextMenusEnabled);
            pSettings->put_AreDefaultScriptDialogsEnabled(settings.browser.areDefaultScriptDialogsEnabled);
            pSettings->put_AreDevToolsEnabled(settings.browser.areDevToolsEnabled);
            pSettings->put_IsBuiltInErrorPageEnabled(settings.browser.isBuiltInErrorPageEnabled);
            pSettings->put_IsScriptEnabled(settings.browser.isScriptEnabled);
            pSettings->put_IsStatusBarEnabled(settings.browser.isStatusBarEnabledEnabled);
            pSettings->put_IsWebMessageEnabled(settings.browser.isWebMessageEnabled);
            pSettings->put_IsZoomControlEnabled(settings.browser.isZoomControlEnabled);
        }
    }

    void WebView2Engine::createWindow() {
        static auto const hInstance = ::GetModuleHandle(nullptr);
        WNDCLASSEX wcex{ 0 };

        wcex.cbSize        = sizeof(WNDCLASSEX);
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = &WebView2Engine::onWindowMessage;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = 0;
        wcex.hInstance     = hInstance;
        wcex.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
        wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName  = nullptr;
        wcex.lpszClassName = m_className.c_str();
        wcex.hIconSm       = LoadIcon(wcex.hInstance, IDI_APPLICATION);

        if (!RegisterClassEx(&wcex)) {
            throw WebEngineException("unable to register the window class");
        }

        const DWORD dwStyle = m_settings.window.displayMode == DisplayMode::Windowed ? WS_OVERLAPPEDWINDOW : WS_POPUP;
        m_hWnd              = CreateWindow(m_className.c_str(),
            m_settings.window.title.c_str(),
            dwStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            1200,
            900,
            NULL,
            NULL,
            hInstance,
            NULL);
        if (m_hWnd == nullptr) {
            throw WebEngineException("unable to create the window: " + std::to_string(::GetLastError()));
        }

        const int nCmdShow = m_settings.window.displayMode == DisplayMode::Windowed ? SW_SHOWNORMAL : SW_SHOWMAXIMIZED;
        ShowWindow(m_hWnd, nCmdShow);
        UpdateWindow(m_hWnd);
    }

    void WebView2Engine::destroyWindow() {
        if (m_hWnd != nullptr && m_hWnd != INVALID_HANDLE_VALUE) {
            ::DestroyWindow(m_hWnd);
            m_hWnd = nullptr;
            UnregisterClass(m_className.c_str(), GetModuleHandle(nullptr));
        }
    }

    void WebView2Engine::createWebView() {
        CreateCoreWebView2EnvironmentWithOptions(nullptr,
            nullptr,
            nullptr,
            Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>([this](HRESULT result,
                                                                                     ICoreWebView2Environment *env) -> HRESULT {
                result = S_OK;
                if (env == nullptr) {
                    std::cout << "WebView2WebEngine::createWebView: error - ICoreWebView2Environment null" << std::endl;
                    return S_FALSE;
                }
                env->CreateCoreWebView2Controller(m_hWnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller *pController) -> HRESULT {
                            result = S_OK;
                            if (pController == nullptr) {
                                return result;
                            }

                            m_pWebViewController = pController;
                            m_pWebViewController->get_CoreWebView2(&m_pWebView);

                            if (m_pWebView == nullptr) {
                                return result;
                            }

                            setSettings(m_settings);
                            resize();
                            navigate(m_settings.browser.startUrl);
                            return result;
                        })
                        .Get());
                return result;
            }).Get());
    }

    void WebView2Engine::destroyWebView() {
        if (m_pWebViewController != nullptr) {
            m_pWebViewController->Close();
            m_pWebView = nullptr;
        }
    }

    HWND WebView2Engine::getHWnd() const {
        return m_hWnd;
    }

    void WebView2Engine::resize() {
        if (m_pWebViewController != nullptr) {
            RECT bounds;
            GetClientRect(m_hWnd, &bounds);
            m_pWebViewController->put_Bounds(bounds);
        }
    }

    LRESULT WebView2Engine::onWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

        switch (message) {
        case WM_SIZE: {
            std::lock_guard lock(s_EdgeWebEngineInstanceLock);
            auto const pWebEngine = getWebEngine(hWnd);
            if (pWebEngine != nullptr) {
                pWebEngine->resize();
            }
        } break;
        case WM_DESTROY: {
            std::lock_guard lock(s_EdgeWebEngineInstanceLock);
            auto const pWebEngine = getWebEngine(hWnd);
            if (pWebEngine != nullptr) {
                pWebEngine->m_hWnd = nullptr;
            }
            PostQuitMessage(0);
        } break;
        default:
            break;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void WebView2Engine::registerEdgeWebEngine(WebView2Engine *pEdgeWebEngine) {
        std::lock_guard lock(s_EdgeWebEngineInstanceLock);
        if (pEdgeWebEngine == nullptr) {
            return;
        }

        if (s_mapHwndEdgeWebEngine.find(pEdgeWebEngine->getHWnd()) != s_mapHwndEdgeWebEngine.cend()) {
            return;
        }

        s_mapHwndEdgeWebEngine.insert({ pEdgeWebEngine->getHWnd(), pEdgeWebEngine });
    }

    void WebView2Engine::unregisterEdgeWebEngine(WebView2Engine *pEdgeWebEngine) {
        std::lock_guard lock(s_EdgeWebEngineInstanceLock);
        if (pEdgeWebEngine == nullptr) {
            return;
        }

        auto const itHwndEdgeWebEngine = s_mapHwndEdgeWebEngine.find(pEdgeWebEngine->getHWnd());
        if (itHwndEdgeWebEngine != s_mapHwndEdgeWebEngine.cend()) {
            s_mapHwndEdgeWebEngine.erase(itHwndEdgeWebEngine);
        }
    }

    WebView2Engine *WebView2Engine::getWebEngine(const HWND hWnd) {
        std::lock_guard lock(s_EdgeWebEngineInstanceLock);
        auto const itHwndEdgeWebEngine = s_mapHwndEdgeWebEngine.find(hWnd);
        if (itHwndEdgeWebEngine != s_mapHwndEdgeWebEngine.cend()) {
            return itHwndEdgeWebEngine->second;
        }

        return nullptr;
    }
} // namespace cho::osbase::webengine
