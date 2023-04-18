// \brief Declaration of the class StartStateWidget

#pragma once
#include "AbstractStateWidget.h"
#include "osStateMachine/StartState.h"

class QPainter;

namespace cho::osbase::statemachineviewer {
    class StartStateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        StartStateWidget(cho::osbase::statemachine::StartStatePtr pStartState);

    protected:
        void paintEvent(QPaintEvent *event) override;
    };
} // namespace cho::osbase::statemachineviewer
