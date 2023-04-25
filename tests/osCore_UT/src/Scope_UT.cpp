// \brief Unit test of the the Scope tools

#include "osCore/Misc/Scope.h"
#include "osCore/Misc/ScopeValue.h"
#include "gtest/gtest.h"

namespace NS_OSBASE::core::ut {
    class ScopeExit_UT : public testing::Test {};
    class ScopeValue_UT : public testing::Test {};

    TEST_F(ScopeExit_UT, make_scope_exit) {
        bool bExited = false;
        {
            auto se = make_scope_exit([&bExited]() { bExited = !bExited; });
        }
        ASSERT_EQ(true, bExited);
    }

    TEST_F(ScopeExit_UT, ctor_move) {
        bool bExited = false;
        {
            auto se = make_scope_exit([&bExited]() { bExited = !bExited; });
            { auto seMove(std::move(se)); }
            ASSERT_EQ(true, bExited);
        }
        ASSERT_EQ(true, bExited);
    }

    TEST_F(ScopeExit_UT, release) {
        bool bExited = false;
        {
            auto se = make_scope_exit([&bExited]() { bExited = !bExited; });
            se.release();
        }
        ASSERT_EQ(false, bExited);
    }

    TEST_F(ScopeValue_UT, make_scope_value_without_newvalue) {
        int value = 1;
        {
            auto sv = make_scope_value(value);
            value   = 0;
            ASSERT_EQ(0, value);
        }
        ASSERT_EQ(1, value);
    }

    TEST_F(ScopeValue_UT, make_scope_value_with_newvalue) {
        int value = 1;
        {
            auto sv = make_scope_value(value, 0);
            ASSERT_EQ(0, value);
        }
        ASSERT_EQ(1, value);
    }

    TEST_F(ScopeValue_UT, ctor_move) {
        int value = 1;
        {
            auto sv = make_scope_value(value, 0);
            { auto svMove = std::move(sv); }
            ASSERT_EQ(1, value);
        }
        ASSERT_EQ(1, value);
    }
} // namespace NS_OSBASE::core::ut
