// \brief Declaration of the class CaptureWindowManager

#include "CaptureWindowManager.h"
#include "osCore/Misc/Scope.h"
#include "osCore/Serialization/Converters.h"
#include "osCore/Exception/RuntimeException.h"
#include <gdiplus.h>

namespace cho::osbase::webengine {

    /*
     * \class CaptureWindowManager
     */
    CaptureWindowManager::CaptureWindowManager() {
        const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
    }

    CaptureWindowManager::~CaptureWindowManager() {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }

    std::vector<char> CaptureWindowManager::captureWindow(HWND hWnd, const std::string imageFormat) const {
        POINT topLeftClient{ 0, 0 };
        if (!ClientToScreen(hWnd, &topLeftClient)) {
            return {};
        }
        RECT clientRect;
        if (!GetClientRect(hWnd, &clientRect)) {
            return {};
        }

        clientRect.left += topLeftClient.x;
        clientRect.right += topLeftClient.x;
        clientRect.top += topLeftClient.y;
        clientRect.bottom += topLeftClient.y;

        const HWND hScreenWnd = GetDesktopWindow();
        HBITMAP hBmp          = captureScreen(hScreenWnd, clientRect);

        return saveToMemory(&hBmp, imageFormat);
    }

    HBITMAP CaptureWindowManager::captureScreen(HWND hScreenWnd, const RECT &rect) const {
        // get handles to a device context (DC)
        const HDC hwindowDC           = GetDC(hScreenWnd);
        const HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
        auto const guard              = core::make_scope_exit([&hwindowCompatibleDC, &hwindowDC, &hScreenWnd]() {
            DeleteDC(hwindowCompatibleDC);
            ReleaseDC(hScreenWnd, hwindowDC);
        });

        SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

        const int width  = rect.right - rect.left;
        const int height = rect.bottom - rect.top;

        const HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
        BITMAPINFOHEADER bi    = createBitmapHeader(width, height);

        SelectObject(hwindowCompatibleDC, hbwindow);

        const DWORD dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;
        const HANDLE hDIB     = GlobalAlloc(GHND, dwBmpSize);
        char *lpbitmap        = static_cast<char *>(GlobalLock(hDIB));

        StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, rect.left, rect.top, width, height, SRCCOPY);
        GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, lpbitmap, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS);

        return hbwindow;
    }

    std::vector<char> CaptureWindowManager::saveToMemory(HBITMAP *hbitmap, const std::string &dataFormat) const {
        Gdiplus::Bitmap bmp(*hbitmap, nullptr);
        IStream *istream = nullptr;
        CreateStreamOnHGlobal(nullptr, TRUE, &istream);

        CLSID clsid = getEncoderClsid("image/" + dataFormat);

        Gdiplus::Status status = bmp.Save(istream, &clsid, NULL);
        if (status != Gdiplus::Status::Ok) {
            throw core::RuntimeException("unable to save the bitmap");
        }

        HGLOBAL hg = nullptr;
        GetHGlobalFromStream(istream, &hg);
        auto const pImage = static_cast<const char *>(GlobalLock(hg));

        auto const guard = core::make_scope_exit([&hg, &istream]() {
            GlobalUnlock(hg);
            istream->Release();
        });

        const int bufsize = static_cast<int>(GlobalSize(hg));
        return { pImage, pImage + bufsize };
    }

    BITMAPINFOHEADER CaptureWindowManager::createBitmapHeader(const int width, const int height) const {
        BITMAPINFOHEADER bi;

        // create a bitmap
        bi.biSize          = sizeof(BITMAPINFOHEADER);
        bi.biWidth         = width;
        bi.biHeight        = -height; // this is the line that makes it draw upside down or not
        bi.biPlanes        = 1;
        bi.biBitCount      = 32;
        bi.biCompression   = BI_RGB;
        bi.biSizeImage     = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed       = 0;
        bi.biClrImportant  = 0;

        return bi;
    }

    CLSID CaptureWindowManager::getEncoderClsid(const std::string &mimeFormat) const {
        CLSID clsid{};
        UINT num  = 0;
        UINT size = 0;

        Gdiplus::ImageCodecInfo *pImageCodecInfo = nullptr;
        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0) {
            throw core::RuntimeException("no encoder available");
        }

        pImageCodecInfo = static_cast<Gdiplus::ImageCodecInfo *>(malloc(size));
        if (pImageCodecInfo == nullptr) {
            throw core::RuntimeException("unable to get the encoder info");
        }

        auto const guard = core::make_scope_exit([pImageCodecInfo]() { free(pImageCodecInfo); });
        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j) {
            auto mimeType = type_cast<std::string>(std::wstring(pImageCodecInfo[j].MimeType));
            if (mimeType == mimeFormat) {
                clsid = pImageCodecInfo[j].Clsid;
                return clsid;
            }
        }

        throw core::RuntimeException("unable to get the encoder for the format " + mimeFormat);
    }
} // namespace cho::osbase::webengine
