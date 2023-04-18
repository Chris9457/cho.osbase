// \brief Declaration of the class StateTitle

#pragma once
#include "osStateMachine/AbstractState.h"
#include <QLabel>

namespace cho::osbase::statemachineviewer {
    class StateTitle : public QLabel {
        Q_OBJECT

    public:
        StateTitle(cho::osbase::statemachine::AbstractStatePtr pState);

        void setColor(const QColor &color);

    protected:
        void paintEvent(QPaintEvent *event) override;
        QColor m_color;
    };
} // namespace cho::osbase::statemachineviewer
