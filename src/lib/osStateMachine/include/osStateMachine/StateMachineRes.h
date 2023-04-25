// \brief Declaration of the resources used by the module StateMachine

#pragma once

namespace NS_OSBASE::statemachine {
    constexpr char ENTRY_ACTION_DEFAULT_NAME[]      = "doEntry";
    constexpr char EXIT_ACTION_DEFAULT_NAME[]       = "doExit";
    constexpr char GUARD_DEFAULT_NAME[]             = "checkGuard";
    constexpr char ACTION_TRANSITION_DEFAULT_NAME[] = "doActionTransition";

    constexpr char STATEMACHINE_DEFAULT_NAME[] = "State machine";
    constexpr char HISTORY_STATE_NAME[]        = "(H)";
    constexpr char START_STATE_SUFFIX_NAME[]   = "(S)";
    constexpr char END_STATE_SUFFIX_NAME[]     = "(E)";
    constexpr char LOGGER_KEY_STATEMACHINE[]   = "statemachine";
} // namespace NS_OSBASE::statemachine
