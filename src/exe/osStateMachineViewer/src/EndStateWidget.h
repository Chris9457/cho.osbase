// \brief Declaration of the class EndStateWidget

#pragma once
#include "AbstractStateWidget.h"
#include "osStateMachine/EndState.h"

class QPainter;

namespace cho::osbase::statemachineviewer {

    class EndStateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        EndStateWidget(cho::osbase::statemachine::EndStatePtr pEndState);

    protected:
        void paintEvent(QPaintEvent *event) override;
    };
} // namespace cho::osbase::statemachineviewer
