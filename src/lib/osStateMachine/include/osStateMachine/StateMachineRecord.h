// \brief Declaration of a sreializable record of the state machine

#pragma once
#include "osCore/Serialization/CoreKeySerializer.h"

namespace cho::osbase::statemachine {

    /**
     * \brief Structire used to record an action
     * \ingroup PACKAGE_OSSTATEENGINE
     */
    struct ActionRecord {
        std::string timestamp; //!< instant of the record
        std::string name;      //!< name of the action
    };
    using ActionRecords = std::vector<ActionRecord>;

    /**
     * \brief Structure used to log transition record
     * \ingroup PACKAGE_OSSTATEENGINE
     */
    struct TransitionRecord {
        std::string timestamp;                      //!< instant of the record
        std::optional<std::string> name;            //!< transition name
        std::optional<std::string> evtName;         //!< event name
        std::optional<std::string> guardName;       //!< used (with succes) guard name
        std::optional<std::string> fromState;       //!< full name from state
        std::optional<std::string> toState;         //!< full name to state
        std::optional<ActionRecords> actionRecords; //!< action list
    };
    using TransitionRecords = std::vector<TransitionRecord>; //!< alias for the collection of transition records

    /**
     * \brief Structure use to log a state machine record
     * \ingroup PACKAGE_OSSTATEENGINE
     */
    struct StateMachineRecord {
        std::optional<std::string> name;     //!< name of the state machine
        std::string id;                      //!< state machine unique identifier
        std::optional<std::string> failure;  //!< reason of the transition failure
        std::string currentState;            //!< current state before the transition
        TransitionRecords transitionRecords; //!< lits of transitions during this record
    };
} // namespace cho::osbase::statemachine
OS_KEY_SERIALIZE_STRUCT(cho::osbase::statemachine::ActionRecord, timestamp, name);
OS_KEY_SERIALIZE_STRUCT(
    cho::osbase::statemachine::TransitionRecord, timestamp, name, evtName, guardName, fromState, toState, actionRecords);
OS_KEY_SERIALIZE_STRUCT(cho::osbase::statemachine::StateMachineRecord, name, id, failure, currentState, transitionRecords);
