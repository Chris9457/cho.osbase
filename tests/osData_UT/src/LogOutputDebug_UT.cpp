// \brief Unit test for the logs

#include "osData/ILogOutput.h"
#include "gtest/gtest.h"
#include <fstream>
#include <future>

using namespace std::chrono_literals;

namespace NS_OSBASE::data::ut {

    class LogOutputDebug_UT : public testing::Test {};

    TEST_F(LogOutputDebug_UT, create) {
        auto const pLog = makeLogOutputDebug();
        ASSERT_NE(nullptr, pLog);
    }
} // namespace NS_OSBASE::data::ut
