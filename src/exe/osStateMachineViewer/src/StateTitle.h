// \brief Declaration of the class StateTitle

#pragma once
#include "osStateMachine/AbstractState.h"
#include <QLabel>

namespace NS_OSBASE::statemachineviewer {
    class StateTitle : public QLabel {
        Q_OBJECT

    public:
        StateTitle(NS_OSBASE::statemachine::AbstractStatePtr pState);

        void setColor(const QColor &color);

    protected:
        void paintEvent(QPaintEvent *event) override;
        QColor m_color;
    };
} // namespace NS_OSBASE::statemachineviewer
