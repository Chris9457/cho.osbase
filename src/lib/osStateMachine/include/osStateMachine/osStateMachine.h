// \brief Main include of the package osStateMachine

#pragma once

/**
 * \defgroup PACKAGE_OSSTATEMACHINE State Machine
 *
 * \brief This package gathers classes and functions of to manage the hierarchical state machines
 *
 * \ingroup PACKAGE_OSBASE
 */

#include "AbstractState.h"
#include "State.h"
#include "StateWalker.h"
#include "StartState.h"
#include "EndState.h"
#include "HistoryState.h"
#include "Event.h"
#include "Transition.h"
#include "StateMachine.h"
#include "StateMachineException.h"
#include "IStateWriter.h"
#include "link.h"
