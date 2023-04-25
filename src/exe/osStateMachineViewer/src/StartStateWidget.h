// \brief Declaration of the class StartStateWidget

#pragma once
#include "AbstractStateWidget.h"
#include "osStateMachine/StartState.h"

class QPainter;

namespace NS_OSBASE::statemachineviewer {
    class StartStateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        StartStateWidget(NS_OSBASE::statemachine::StartStatePtr pStartState);

    protected:
        void paintEvent(QPaintEvent *event) override;
    };
} // namespace NS_OSBASE::statemachineviewer
