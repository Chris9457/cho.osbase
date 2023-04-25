// \brief Declaration of the interface IWebEngine

#pragma once
#include "WebViewerEngineSettings.h"
#include "osData/Uri.h"
#include "osCore/Exception/RuntimeException.h"
#include <memory>
#include <future>

namespace NS_OSBASE::webengine {

    /**
     * \brief Exception thrown by web engines
     */
    class WebEngineException : public core::RuntimeException {
        using RuntimeException::RuntimeException;
    };

    class IWebViewerEngine;
    using IWebViewerEnginePtr = std::shared_ptr<IWebViewerEngine>;

    /**
     * \brief Contract for the web engine
     */
    class IWebViewerEngine {
    public:
        virtual ~IWebViewerEngine() = default;

        virtual void run()                          = 0;
        virtual std::shared_future<void> runAsync() = 0;
        virtual bool isRunning() const              = 0;
        virtual void stop()                         = 0;

        virtual Settings getSettings() const               = 0;
        virtual void setSettings(const Settings &settings) = 0;

        virtual void navigate(const data::Uri &uri)                                   = 0;
        virtual std::vector<char> captureWindow(const std::string &imageformat) const = 0;

        static inline const std::string FORMAT_BITMAP = "bmp";
        static inline const std::string FORMAT_JPEG   = "jpeg";
        static inline const std::string FORMAT_GIF    = "gif";
        static inline const std::string FORMAT_TIFF   = "tiff";
        static inline const std::string FORMAT_PNG    = "png";
    };

    IWebViewerEnginePtr makeWebViewerEngine();
    IWebViewerEnginePtr makeWebViewerEngine(const Settings &settings);
} // namespace NS_OSBASE::webengine
