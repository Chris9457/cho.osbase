// \brief Unit test for the logs

#include "osData/ILogOutput.h"
#include "gtest/gtest.h"
#include <fstream>
#include <future>

using namespace std::chrono_literals;

namespace cho::osbase::data::ut {

    class LogOutputFile_UT : public testing::Test {};

    TEST_F(LogOutputFile_UT, create) {
        auto const path  = std::filesystem::current_path() / "test.log";
        auto const guard = core::make_scope_exit([&path]() {
            if (exists(path)) {
                remove(path);
            }
        });

        auto const pLog = makeLogOutputFile(path);
        ASSERT_NE(nullptr, pLog);
    }

    TEST_F(LogOutputFile_UT, log) {
        auto const path  = std::filesystem::current_path() / "test.log";
        auto const guard = core::make_scope_exit([&path]() {
            if (exists(path)) {
                remove(path);
            }
        });

        auto const msg  = "test";
        auto const pLog = makeLogOutputFile(path);
        pLog->log(msg);

        std::ifstream is(path);
        std::string line;
        is >> line;
        ASSERT_EQ(msg, line);
    }

    TEST_F(LogOutputFile_UT, flush) {
        auto const pLog = makeLogOutputFile(std::filesystem::current_path()); // bad path

        auto const msg = "test";
        pLog->log(msg);
        std::stringstream os;
        pLog->flush(os);

        std::string flushedMsg;
        std::getline(os, flushedMsg);
        ASSERT_EQ(flushedMsg, msg);
    }
} // namespace cho::osbase::data::ut
