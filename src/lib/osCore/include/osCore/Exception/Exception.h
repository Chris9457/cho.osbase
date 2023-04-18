// \file  Exception.h
// \brief Main header for the exception package

#pragma once

/**
 * \defgroup PACKAGE_EXCEPTION Exception
 *
 * \brief This package gathers classes and functions related to exception
 *
 * \par Crash or Structured Exception handling
 * The crash handling or Structured Exception Handling (aka SEH) is managed by:
 *  - the singleton StructuredExceptionHandler
 *  - the exception StructuredException
 *  - the set of functions: oscheck::throwIf...
 *  .
 *
 * \par Checking if a function crash
 * For functions with no return: <b>throwIfCrash</b>:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 void foo(int, const std::string&) { ... }

 void main() {
    try {
        oscheck::throwIfCrash(foo, 1, "c");
    } catch (const cho::osbase::core::StructuredException& e) {
        std::cout << e.what() << std::endl;
    }
 }
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * For functions with return: <b>throwIfCrashOrReturn</b>:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 int foo(int, const std::string&) { ... }

 void main() {
    try {
        auto const v = oscheck::throwIfCrashOrReturn<int>(foo, 1, "c");
        ...
    } catch (const cho::osbase::core::StructuredException& e) {
        std::cout << e.what() << std::endl;
    }
 }
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * \par Checking pre-conditions
 * Pre-condition are checked with the functions: \n
 *  oscheck::throwIf<comparison> \n
 * with comparison:
 *  - True => throw if true
 *  - False => throw if false
 *  - EQ => throw if equality
 *  - NE => throw if non-equality
 *  - NotLT => throw if not lesser than (strict)
 *  - NotLE => throw if not lesser than or equal
 *  - NotGT => throw if not greater than (strict)
 *  - NotGE => throw if not greater than or equal.
 * All these functions throw the exception cho::osbase::core::StructuredException in case of crash (SEH).\n
 * \n
 * Examples:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 throwIfTrue(isPushed());
 throwIfFalse(isReleased());
 throwIfNE(state, PUSHED);
 throwIfEQ(state, RELEASED);
 throwIfNotLT(threshold, 5);
 throwIfNotLE(threshold, 4);
 throwIfNotGT(floor, 2);
 throwIfNotGE(floor, 1);
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * \par Minidump
 * Each time a Structured Exception (SEH) is caught, a minidump file is generated:
 *  - at the folder defined in the singleton StructuredExceptionHandler (get/set MinidumpFolder) => by default, the folder is the folder of
 the executable
 *  - with the base name defined in the singleton StructuredExceptionHandler (get/set DumpBaseFileName) => by default, the base name is
 "dump_"
 *  - the dump file name is suffixed by the date time, format: YYmmdd_HHMMSS
 *  - the extension is ".dmp"
 *  .
 * This file is a binary file that can be read by Visual Studio. Example:\n
 * - dump_20220609_151336.dmp
 *
 * \ingroup PACKAGE_OSCORE
 */

#include "LogicException.h"
#include "RuntimeException.h"
#include "StructuredExceptionHandler.h"
#include "Check.h"
