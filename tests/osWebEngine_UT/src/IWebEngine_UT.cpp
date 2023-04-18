#include "osWebEngine/IWebEngine.h"
#include <gtest/gtest.h>

using namespace std::chrono_literals;

namespace cho::osbase::webengine::ut {

    class IWebEngine_UT : public testing::Test {};

    TEST_F(IWebEngine_UT, makeWebEngine) {
        auto pWebEngine = makeWebEngine();
        ASSERT_NE(nullptr, pWebEngine);

        pWebEngine = makeWebEngine(Settings{});
        ASSERT_NE(nullptr, pWebEngine);
    }

    TEST_F(IWebEngine_UT, run_stop) {
        auto const pWebEngine = makeWebEngine();
        auto const futRun     = std::async([&pWebEngine]() { pWebEngine->run(); });

        ASSERT_EQ(std::future_status::timeout, futRun.wait_for(1s));

        pWebEngine->stop();
        ASSERT_EQ(std::future_status::ready, futRun.wait_for(1s));
    }

    TEST_F(IWebEngine_UT, runAsync_stop) {
        auto const pWebEngine = makeWebEngine();
        auto const futRun     = pWebEngine->runAsync();

        ASSERT_EQ(std::future_status::timeout, futRun.wait_for(1s));

        pWebEngine->stop();
        ASSERT_EQ(std::future_status::ready, futRun.wait_for(1s));
    }

    TEST_F(IWebEngine_UT, isRunning) {
        auto const pWebEngine = makeWebEngine();

        ASSERT_FALSE(pWebEngine->isRunning());
        auto const futRun = pWebEngine->runAsync();

        ASSERT_EQ(std::future_status::timeout, futRun.wait_for(1s));
        ASSERT_TRUE(pWebEngine->isRunning());

        pWebEngine->stop();
        ASSERT_EQ(std::future_status::ready, futRun.wait_for(1s));
        ASSERT_FALSE(pWebEngine->isRunning());
    }

    TEST_F(IWebEngine_UT, captureWindow_known_format) {
        auto const pWebEngine = makeWebEngine();
        auto const guard      = core::make_scope_exit([&pWebEngine]() { pWebEngine->stop(); });

        pWebEngine->runAsync();
        std::this_thread::sleep_for(1s);
        for (auto const format : { "bmp", "jpeg", "gif", "tiff", "png" }) {
            auto const image = pWebEngine->captureWindow(format);
            ASSERT_FALSE(image.empty());
        }
    }

    TEST_F(IWebEngine_UT, captureWindow_unknown_format) {
        auto const pWebEngine = makeWebEngine();
        auto const guard      = core::make_scope_exit([&pWebEngine]() { pWebEngine->stop(); });

        pWebEngine->runAsync();
        std::this_thread::sleep_for(1s);

        auto constexpr unknowFormat = "unknown";
        ASSERT_THROW(pWebEngine->captureWindow(unknowFormat), core::RuntimeException);
    }

    TEST_F(IWebEngine_UT, DISABLED_navigate) {
        auto const pWebEngine    = makeWebEngine();
        const std::string strUrl = "https://wwww.google.fr";
        auto const url           = type_cast<data::Uri>(strUrl);

        ASSERT_THROW(pWebEngine->navigate(url), WebEngineException);

        auto const futRun = pWebEngine->runAsync();
        ASSERT_EQ(std::future_status::timeout, futRun.wait_for(1s));

        ASSERT_NO_THROW(pWebEngine->navigate(url));
        pWebEngine->stop();
        ASSERT_EQ(std::future_status::ready, futRun.wait_for(1s));
    }
} // namespace cho::osbase::webengine::ut
