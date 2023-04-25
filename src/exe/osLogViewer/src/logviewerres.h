// \brief Declaration of the class LogViewer

#pragma once
#include <Windows.h>
#include <unordered_map>

namespace NS_OSBASE::logviewer {
    enum class ConsoleColor : unsigned short {
        Black       = 0,
        Darkblue    = FOREGROUND_BLUE,
        Darkgreen   = FOREGROUND_GREEN,
        Darkcyan    = FOREGROUND_GREEN | FOREGROUND_BLUE,
        Darkred     = FOREGROUND_RED,
        Darkmagenta = FOREGROUND_RED | FOREGROUND_BLUE,
        Darkyellow  = FOREGROUND_RED | FOREGROUND_GREEN,
        Darkgray    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        Gray        = FOREGROUND_INTENSITY,
        Blue        = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
        Green       = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
        Cyan        = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
        Red         = FOREGROUND_INTENSITY | FOREGROUND_RED,
        Magenta     = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
        Yellow      = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
        White       = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    };

    const std::unordered_map<std::string, ConsoleColor> consoleOutputColor{ { "black", ConsoleColor::Black },
        { "darkblue", ConsoleColor::Darkblue },
        { "darkgreen", ConsoleColor::Darkgreen },
        { "darkcyan", ConsoleColor::Darkcyan },
        { "darkred", ConsoleColor::Darkred },
        { "darkmagenta", ConsoleColor::Darkmagenta },
        { "darkyellow", ConsoleColor::Darkyellow },
        { "darkgray", ConsoleColor::Darkgray },
        { "gray", ConsoleColor::Gray },
        { "blue", ConsoleColor::Blue },
        { "green", ConsoleColor::Green },
        { "cyan", ConsoleColor::Cyan },
        { "red", ConsoleColor::Red },
        { "magenta", ConsoleColor::Magenta },
        { "yellow", ConsoleColor::Yellow },
        { "white", ConsoleColor::White } };

    enum class Operands {
        eq,         //!< equal
        ne,         //!< not equal
        lt,         //!< lesser
        le,         //!< lesser or equal
        gt,         //!< greater
        ge,         //!< greater or equal
        contain,    //!< contain string
        in,         //!< in string
        containKey, //!< json key contains direct sub key
        inKey,      //!< json key is contained by a parent key
    };

    const std::unordered_map<std::string, Operands> operands{
        { "eq", Operands::eq },
        { "ne", Operands::ne },
        { "lt", Operands::lt },
        { "le", Operands::le },
        { "gt", Operands::gt },
        { "ge", Operands::ge },
        { "contain", Operands::contain },
        { "in", Operands::in },
        { "containkey", Operands::containKey },
        { "inkey", Operands::inKey },
    };
} // namespace NS_OSBASE::logviewer
