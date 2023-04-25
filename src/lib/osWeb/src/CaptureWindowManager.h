// \brief Declaration of the class CaptureWindowManager

#pragma once
#include "osCore/DesignPattern/Singleton.h"
#include <Windows.h>
#include <vector>

namespace NS_OSBASE::webengine {

    /**
     * \brief Provide methods to capture a window in the good image format
     */
    class CaptureWindowManager : public core::Singleton<CaptureWindowManager> {
        friend core::Singleton<CaptureWindowManager>;

    public:
        std::vector<char> captureWindow(HWND hWnd, const std::string imageFormat) const;

    private:
        CaptureWindowManager();
        ~CaptureWindowManager() override;

        HBITMAP captureScreen(HWND hScreenWnd, const RECT &rect) const;
        BITMAPINFOHEADER createBitmapHeader(const int width, const int height) const;
        std::vector<char> saveToMemory(HBITMAP *hbitmap, const std::string &dataFormat) const;
        CLSID getEncoderClsid(const std::string &mimeFormat) const;

        ULONG_PTR m_gdiplusToken{};
    };
#define TheCaptureWindowManager CaptureWindowManager::getInstance()
} // namespace NS_OSBASE::webengine
