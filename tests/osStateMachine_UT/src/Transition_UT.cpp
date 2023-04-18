// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include <gtest/gtest.h>

namespace cho::osbase::statemachine::ut {
    class Transition_UT : public testing::Test {};

    TEST_F(Transition_UT, create) {
        auto const pTransition = Transition::create();
        ASSERT_NE(nullptr, pTransition);
    }

    TEST_F(Transition_UT, getToState) {
        auto const pToState    = State::createRoot("to");
        auto const pTransition = Transition::create(pToState);

        ASSERT_EQ(pToState, pTransition->getToState());
    }

    TEST_F(Transition_UT, setGuard) {
        auto const pToState    = State::createRoot("to");
        auto const pTransition = Transition::create(pToState);

        constexpr int expectedValue = 3;
        auto guard = [&expectedValue](const std::any &value) { return std::any_cast<decltype(expectedValue)>(value) == expectedValue; };

        ASSERT_TRUE(pTransition->checkGuard(expectedValue + 1));

        pTransition->setGuard(guard);
        ASSERT_TRUE(pTransition->checkGuard(expectedValue));
        ASSERT_FALSE(pTransition->checkGuard(expectedValue + 1));
    }

    TEST_F(Transition_UT, setAction) {
        auto const pToState    = State::createRoot("to");
        auto const pTransition = Transition::create(pToState);

        double value = 1.;
        auto action  = [&value](const std::any &arg) {
            auto const v = std::any_cast<double>(arg);
            value += v;
        };

        pTransition->doAction(3.);
        ASSERT_EQ(1., value);

        pTransition->setAction(action);
        pTransition->doAction(3.);
        ASSERT_EQ(4., value);
    }

    TEST_F(Transition_UT, name) {
        auto const pTransition = Transition::create();

        ASSERT_TRUE(pTransition->getName().empty());

        const std::string name = "T1";
        pTransition->setName(name);
        ASSERT_EQ(name, pTransition->getName());
    }

} // namespace cho::osbase::statemachine::ut
