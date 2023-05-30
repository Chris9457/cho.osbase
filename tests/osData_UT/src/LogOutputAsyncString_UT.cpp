// \brief Unit tests for LogOutputAsyncData

#include "osData/Log.h"
#include "gtest/gtest.h"

using namespace std::chrono_literals;

namespace NS_OSBASE::data::ut {

    class LogOutputAsyncString_UT : public testing::Test {};

    TEST_F(LogOutputAsyncString_UT, maker) {
        AsyncString asyncString;
        auto const guard = core::make_scope_exit([&asyncString] { asyncString.reset(); });
        asyncString.create();

        auto pLogOutput = makeLogOutputAsyncString(asyncString);
        ASSERT_NE(nullptr, pLogOutput);

        pLogOutput = makeLogOutputAsyncString(asyncString.getUriOfCreator());
        ASSERT_NE(nullptr, pLogOutput);
    }

    TEST_F(LogOutputAsyncString_UT, log) {
        AsyncString asyncString;
        auto const guard = core::make_scope_exit([&asyncString] { asyncString.reset(); });
        asyncString.create();

        auto const pLogOutput    = makeLogOutputAsyncString(asyncString);
        const std::string strLog = "my log";

        pLogOutput->log(std::string(strLog));

        asyncString.waitConnection();
        auto const receivedLog = asyncString.getFor(1s);

        ASSERT_EQ(receivedLog, strLog);
    }

    TEST_F(LogOutputAsyncString_UT, flush) {
        AsyncString asyncString;
        auto const guard = core::make_scope_exit([&asyncString] { asyncString.reset(); });
        asyncString.create();
        auto const uri = asyncString.getUriOfCreator();
        asyncString.reset();

        auto const pLogOutput    = makeLogOutputAsyncString(uri);
        const std::string strLog = "my log";
        pLogOutput->log(std::string(strLog));

        std::stringstream oss;
        asyncString.reset();
        pLogOutput->flush(oss);
        ASSERT_EQ(strLog + '\n', oss.str());
    }
} // namespace NS_OSBASE::data::ut
