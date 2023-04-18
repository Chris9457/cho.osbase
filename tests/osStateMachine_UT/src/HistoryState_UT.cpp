// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include <gtest/gtest.h>

namespace cho::osbase::statemachine::ut {
    class HistoryState_UT : public testing::Test {};

    TEST_F(HistoryState_UT, create) {
        auto const pRoot         = State::createRoot("root");
        auto const pHistoryState = pRoot->setHistoryState();
        ASSERT_NE(nullptr, pHistoryState);
    }

    TEST_F(HistoryState_UT, getParent) {
        auto const pRoot         = State::createRoot("root");
        auto const pHistoryState = pRoot->setHistoryState();
        ASSERT_EQ(pRoot, pHistoryState->getParent());
    }

    TEST_F(HistoryState_UT, savedState) {
        auto const pRoot         = State::createRoot("root");
        auto const pHistoryState = pRoot->setHistoryState();
        auto const pS1           = State::create(pRoot, "S1");
        auto const pS1_1         = State::create(pS1, "S1_1");

        ASSERT_EQ(nullptr, pHistoryState->getSavedState());

        pHistoryState->setSavedState(pS1_1);
        ASSERT_EQ(pS1_1, pHistoryState->getSavedState());
    }
} // namespace cho::osbase::statemachine::ut
