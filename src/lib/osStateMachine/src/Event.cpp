// \brief Declaration of the class Event

#include "osStateMachine/Event.h"

namespace NS_OSBASE::statemachine {
    EventPtr Event::create(const std::string &name, const std::any &value) {
        return EventPtr(new Event(name, value));
    }

    const std::string &Event::getName() const {
        return m_name;
    }

    const std::any &Event::getValue() const {
        return m_value;
    }

    Event::Event(const std::string &name, const std::any &value) : m_name(name), m_value(value) {
    }
} // namespace NS_OSBASE::statemachine
