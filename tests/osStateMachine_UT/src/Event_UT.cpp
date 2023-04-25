// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include <gtest/gtest.h>

namespace NS_OSBASE::statemachine::ut {
    class Event_UT : public testing::Test {};

    TEST_F(Event_UT, create) {
        auto pEvent = Event::create("evt");
        ASSERT_NE(nullptr, pEvent);

        pEvent = Event::create("evt", 2);
        ASSERT_NE(nullptr, pEvent);
    }

    TEST_F(Event_UT, getName) {
        const std::string name = "evt";
        auto const pEvent      = Event::create(name);
        ASSERT_EQ(name, pEvent->getName());
    }

    TEST_F(Event_UT, getValue) {
        auto const value = 56.;
        ;
        auto const pEvent = Event::create("evt", value);
        ASSERT_EQ(value, std::any_cast<decltype(value)>(pEvent->getValue()));
    }

} // namespace NS_OSBASE::statemachine::ut
