// \brief Unit test for the LoggerMessage

#include "osData/Log.h"
#include "gtest/gtest.h"

namespace NS_OSBASE::data::ut {

    class LoggerMessage_UT : public testing::Test {};

    TEST_F(LoggerMessage_UT, DISABLED_timestamp) {
        // this test is wrong because the date time logged is not complete (missing DST, TZ)
        // so the rebuilt date time can defer depending on the default parameter
        const LoggerTimestamp ts;

        // check by string comparison
        auto const strTimestamp            = ts.toString();
        auto const strTransformedTimeStamp = LoggerTimestamp::fromString(strTimestamp).toString();
        ASSERT_EQ(strTransformedTimeStamp, strTimestamp);

        // check by time_point comparison
        auto const tp     = LoggerTimestamp::fromString(strTimestamp).getTimePoint();
        std::string strTp = LoggerTimestamp(tp).toString();
        auto const d      = std::chrono::duration_cast<std::chrono::milliseconds>(ts.getTimePoint() - tp);
        ASSERT_EQ(0, d.count());
    }
} // namespace NS_OSBASE::data::ut
