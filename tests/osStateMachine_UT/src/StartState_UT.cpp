// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include <gtest/gtest.h>

namespace NS_OSBASE::statemachine::ut {
    class StartState_UT : public testing::Test {};

    TEST_F(StartState_UT, create) {
        auto const pRoot       = State::createRoot("root");
        auto const pStartState = pRoot->setStartState();
        ASSERT_NE(nullptr, pStartState);
    }

    TEST_F(StartState_UT, getParent) {
        auto const pRoot       = State::createRoot("root");
        auto const pStartState = pRoot->setStartState();
        ASSERT_EQ(pRoot, pStartState->getParent());
    }

    TEST_F(StartState_UT, getName) {
        auto const pRoot       = State::createRoot("root");
        auto const pStartState = pRoot->setStartState("start");
        ASSERT_EQ("start", pStartState->getName());
    }

} // namespace NS_OSBASE::statemachine::ut
