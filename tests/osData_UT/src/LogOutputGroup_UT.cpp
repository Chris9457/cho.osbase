// \brief Unit test for the logs

#include "osData/LogOutputGroup.h"
#include "gtest/gtest.h"
#include <fstream>
#include <future>

using namespace std::chrono_literals;

namespace NS_OSBASE::data::ut {

    class LogOutputGroup_UT : public testing::Test {};

    TEST_F(LogOutputGroup_UT, create) {
        auto const pLogGroup = makeLogOutputGroup();
        ASSERT_NE(nullptr, pLogGroup);
    }

    TEST_F(LogOutputGroup_UT, add) {
        auto const pLogGroup = makeLogOutputGroup();

        pLogGroup->add(makeLogOutputConsole());
        pLogGroup->add(makeLogOutputDebug());
        ASSERT_EQ(2, pLogGroup->getCount());

        pLogGroup->add(nullptr);
        ASSERT_EQ(2, pLogGroup->getCount());
    }

    TEST_F(LogOutputGroup_UT, remove) {
        auto const pLogGroup = makeLogOutputGroup();

        auto const pLogOutputConsole = makeLogOutputConsole();
        auto const pLogOutputDebug   = makeLogOutputDebug();
        pLogGroup->add(pLogOutputConsole);
        pLogGroup->add(pLogOutputDebug);

        pLogGroup->remove(pLogOutputConsole);
        ASSERT_EQ(1, pLogGroup->getCount());

        pLogGroup->remove(pLogOutputConsole);
        ASSERT_EQ(1, pLogGroup->getCount());

        pLogGroup->remove(pLogOutputDebug);
        ASSERT_EQ(0, pLogGroup->getCount());
    }

    TEST_F(LogOutputGroup_UT, clear) {
        auto const pLogGroup = makeLogOutputGroup();

        pLogGroup->clear();
        ASSERT_EQ(0, pLogGroup->getCount());

        pLogGroup->add(makeLogOutputConsole());
        pLogGroup->add(makeLogOutputDebug());
        pLogGroup->clear();
        ASSERT_EQ(0, pLogGroup->getCount());
    }

    TEST_F(LogOutputGroup_UT, log) {
        auto const path  = std::filesystem::current_path() / "test.log";
        auto const guard = core::make_scope_exit([&path]() {
            if (exists(path)) {
                remove(path);
            }
        });

        auto const msg       = "my_test";
        auto const pLogGroup = makeLogOutputGroup();

        pLogGroup->add(makeLogOutputConsole());
        pLogGroup->add(makeLogOutputDebug());
        pLogGroup->add(makeLogOutputFile(path));

        pLogGroup->log(msg);
        std::ifstream is(path);
        std::string line;
        is >> line;
        ASSERT_EQ(msg, line);
    }

    TEST_F(LogOutputGroup_UT, for_range_loop) {
        auto const pLogGroup         = makeLogOutputGroup();
        auto const pLogOutputConsole = makeLogOutputConsole();
        auto const pLogOutputDebug   = makeLogOutputDebug();
        std::set<ILogOutputPtr> pLogOutputs;
        pLogOutputs.insert(pLogOutputConsole);
        pLogOutputs.insert(pLogOutputDebug);

        pLogGroup->add(pLogOutputConsole);
        pLogGroup->add(pLogOutputDebug);

        for (auto const &pLogOutput : *pLogGroup) {
            ASSERT_NE(pLogOutputs.cend(), pLogOutputs.find(pLogOutput));
        }
    }
} // namespace NS_OSBASE::data::ut
