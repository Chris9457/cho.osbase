// \brief Declaration of the struct TransitionRecord

#pragma once
#include "osStateMachine/osStateMachine.h"
#include <chrono>
#include <set>

namespace cho::osbase::statemachineviewer {
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
        cho::osbase::statemachine::AbstractStatePtr pCurrentState;
        cho::osbase::statemachine::AbstractStatePtr pFromState;
        cho::osbase::statemachine::AbstractStatePtr pToState;
        ActionRecords actionRecords;
    };
    using TransitionRecords = std::vector<TransitionRecord>;

    struct KeyTransitionRecord {
        std::wstring processName;
        std::string smName;
        unsigned long long smId;
        cho::osbase::statemachine::StatePtr pRoot;

        bool operator<(const KeyTransitionRecord &other) const;
    };

    using SMRecords = std::map<KeyTransitionRecord, TransitionRecords>;
} // namespace cho::osbase::statemachineviewer
