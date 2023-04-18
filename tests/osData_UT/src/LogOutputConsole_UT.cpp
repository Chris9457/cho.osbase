// \brief Unit test for the logs

#include "osData/ILogOutput.h"
#include "gtest/gtest.h"
#include <fstream>
#include <future>

using namespace std::chrono_literals;

namespace cho::osbase::data::ut {

    class LogOutputConsole_UT : public testing::Test {};

    TEST_F(LogOutputConsole_UT, create) {
        auto const pLog = makeLogOutputConsole();
        ASSERT_NE(nullptr, pLog);
    }
} // namespace cho::osbase::data::ut
