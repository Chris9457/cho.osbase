// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include <gtest/gtest.h>

namespace cho::osbase::statemachine::ut {
    class EndState_UT : public testing::Test {};

    TEST_F(EndState_UT, create) {
        auto const pRoot     = State::createRoot("root");
        auto const pEndState = pRoot->addEndState("End");
        ASSERT_NE(nullptr, pEndState);
    }

    TEST_F(EndState_UT, getParent) {
        auto const pRoot     = State::createRoot("root");
        auto const pEndState = pRoot->addEndState("End");
        ASSERT_EQ(pRoot, pEndState->getParent());
    }

    TEST_F(EndState_UT, getName) {
        auto const pRoot     = State::createRoot("root");
        auto const pEndState = pRoot->addEndState("End");
        ASSERT_EQ("End", pEndState->getName());
    }

} // namespace cho::osbase::statemachine::ut
