// \brief Declaration of the resources used by the classes StateWidget(s)

#pragma once
#include <QColor>

namespace NS_OSBASE::statemachineviewer {
    constexpr int STATEWIDGET_RADIUS       = 15;
    constexpr int STATEWIDGET_TITLE_HEIGHT = 25;
    constexpr int STATEWIDGET_MIN_WIDTH    = 100;
    constexpr int STATEWIDGET_MARGIN       = 20;
    constexpr int STATEWIDGET_SPACING      = 20;

    constexpr int LEAFSTATEWIDGET_EDGE = 25;

    constexpr int STATEWIDGET_BORDER_WIDTH               = 2;
    constexpr double STATEWIDGET_BACKGROUND_TRANSPARENCY = 0.05; // 0. ==> transparent, 1. ==> opaque

    constexpr int STARTSTATEWIDGET_EDGE = LEAFSTATEWIDGET_EDGE;

    constexpr int ENDSTATEWIDGET_INNER_EDGE   = LEAFSTATEWIDGET_EDGE - 10;
    constexpr int ENDSTATEWIDGET_EDGE         = LEAFSTATEWIDGET_EDGE;
    constexpr int ENDSTATEWIDGET_BORDER_WIDTH = STATEWIDGET_BORDER_WIDTH;

    constexpr int HISTORYSTATEWIDGET_EDGE         = LEAFSTATEWIDGET_EDGE;
    constexpr char HISTORYSTATEWIDGET_LABEL[]     = "H";
    constexpr int HISTORYSTATEWIDGET_BORDER_WIDTH = STATEWIDGET_BORDER_WIDTH;

    constexpr int TRANSITION_LINE_WIDTH     = STATEWIDGET_BORDER_WIDTH;
    constexpr int TRANSITION_RECURSIVE_EDGE = LEAFSTATEWIDGET_EDGE;
    constexpr int TRANSITION_INNER_EDGE     = LEAFSTATEWIDGET_EDGE;

    constexpr char TRANSITION_HEADER_DEFAULT_TITLE[] = "Transition detail";
    constexpr char TRANSITION_HEADER_FIELDNAME[]     = "Field name";
    constexpr char TRANSITION_HEADER_VALUE[]         = "Value";
    constexpr char TRANSITION_FIELD_TIMESTAMP[]      = "Timestamp";
    constexpr char TRANSITION_FIELD_EVTNAME[]        = "Event name";
    constexpr char TRANSITION_FIELD_FAILURE[]        = "Failure";
    constexpr char TRANSITION_FIELD_GUARDNAME[]      = "Guard name";
    constexpr char TRANSITION_FIELD_CURRENTSTATE[]   = "Current state";
    constexpr char TRANSITION_FIELD_FROMSTATE[]      = "From state";
    constexpr char TRANSITION_FIELD_TOSTATE[]        = "To state";
    constexpr char TRANSITION_FIELD_ACTIONS[]        = "Actions";
    constexpr char TRANSITION_FIELD_ACTIONNAME[]     = "Name";

    const QColor STATE_ACTIVATED_COLOR_CURRENTSTATE = Qt::darkCyan;
    const QColor STATE_ACTIVATED_COLOR_FROMSTATE    = Qt::blue;
    const QColor STATE_ACTIVATED_COLOR_TOSTATE      = Qt::darkGreen;
    const QColor STATE_ACTIVATED_COLOR_FAILURE      = Qt::red;
    const QColor STATE_DEACTIVATED_COLOR            = Qt::black;

} // namespace NS_OSBASE::statemachineviewer
