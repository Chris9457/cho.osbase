// \brief Declaration of the class Event

#pragma once
#include <memory>
#include <any>
#include <string>

namespace cho::osbase::statemachine {

    class Event;
    using EventPtr = std::shared_ptr<Event>;

    /**
     * \brief This class represents an event
     * \ingroup PACKAGE_OSSTATE
     */
    class Event final {
    public:
        static EventPtr create(const std::string &name, const std::any &value = std::any()); //!< intanciate an event

        const std::string &getName() const; //!< return the name of the event
        const std::any &getValue() const;   //!< return the value of the event

    private:
        Event(const std::string &name, const std::any &value);

        std::string m_name;
        std::any m_value;
    };
} // namespace cho::osbase::statemachine
