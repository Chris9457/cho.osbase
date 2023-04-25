// \brief Declaration of the tests for the WebViewer service

#include "WebViewerRes.h"
#include "osWebViewer.h"
#include <gtest/gtest.h>

namespace NS_OSBASE::webviewer::ut {

    class WebViewer_UT : public testing::Test {
    protected:
        void SetUp() override {
            m_pStub = makeStub();
            m_pStub->connect(brokerUrl, brokerPort);
        }

        void TearDown() override {
            m_pStub->disconnect();
        }

        auto getStub() const {
            return m_pStub;
        }

    private:
        IWebViewerServicePtr m_pStub;
    };

    TEST_F(WebViewer_UT, captureWindow) {

        for (auto const imageFormat : { ImageFormat::bmp, ImageFormat::gif, ImageFormat::jpeg, ImageFormat::png, ImageFormat::tiff }) {
            auto const image = getStub()->captureWindow(imageFormat).get();
            ASSERT_FALSE(image.empty());
        }
    }
} // namespace NS_OSBASE::webviewer::ut
