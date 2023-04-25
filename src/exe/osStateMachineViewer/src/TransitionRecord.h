// \brief Declaration of the struct TransitionRecord

#pragma once
#include "osStateMachine/osStateMachine.h"
#include <chrono>
#include <set>

namespace NS_OSBASE::statemachineviewer {
    using time_point = std::chrono::system_clock::time_point;

    struct ActionRecord {
        time_point timestamp;
        std::string name;
    };
    using ActionRecords = std::vector<ActionRecord>;

    struct TransitionRecord {
        std::string name;
        time_point timestamp;
        std::string evtName;
        std::string failure;
        std::string guardName;
        NS_OSBASE::statemachine::AbstractStatePtr pCurrentState;
        NS_OSBASE::statemachine::AbstractStatePtr pFromState;
        NS_OSBASE::statemachine::AbstractStatePtr pToState;
        ActionRecords actionRecords;
    };
    using TransitionRecords = std::vector<TransitionRecord>;

    struct KeyTransitionRecord {
        std::wstring processName;
        std::string smName;
        unsigned long long smId;
        NS_OSBASE::statemachine::StatePtr pRoot;

        bool operator<(const KeyTransitionRecord &other) const;
    };

    using SMRecords = std::map<KeyTransitionRecord, TransitionRecords>;
} // namespace NS_OSBASE::statemachineviewer
