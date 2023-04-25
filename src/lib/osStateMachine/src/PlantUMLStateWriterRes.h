// \brief Declaration of the resources used in the class PlantUMLStateWriter

#pragma once

namespace NS_OSBASE::statemachine {
    constexpr char PLANTUML_HEADER[]                = "@startuml";
    constexpr char PLANTUML_FOOTER[]                = "@enduml";
    constexpr char PLANTUML_STATE[]                 = "state";
    constexpr char PLANTUML_STARTBLOCK[]            = "{";
    constexpr char PLANTUML_ENDBLOCK[]              = "}";
    constexpr char PLANTUML_STARTSTATE[]            = "[*]";
    constexpr char PLANTUML_ENDSTATE[]              = "[*]";
    constexpr char PLANTUML_HISTORYSTATE[]          = "[H]";
    constexpr char PLANTUML_TRANSITION_DELIMITER[]  = " : ";
    constexpr char PLANTUML_STARTGUARD[]            = "[";
    constexpr char PLANTUML_ENDGUARD[]              = "]";
    constexpr char PLANTUML_ACTIONDELIMITER[]       = " / ";
    constexpr char PLANTUML_TRANSITION_DOWNARROW[]  = " --> ";
    constexpr char PLANTUML_TRANSITION_RIGHTARROW[] = " -> ";
    constexpr char PLANTUML_PADDING[]               = "    ";
    constexpr char PLANTUML_START_LEGEND[]          = "legend";
    constexpr char PLANTUML_END_LEGEND[]            = "end legend";

} // namespace NS_OSBASE::statemachine
