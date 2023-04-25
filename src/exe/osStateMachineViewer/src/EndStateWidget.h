// \brief Declaration of the class EndStateWidget

#pragma once
#include "AbstractStateWidget.h"
#include "osStateMachine/EndState.h"

class QPainter;

namespace NS_OSBASE::statemachineviewer {

    class EndStateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        EndStateWidget(NS_OSBASE::statemachine::EndStatePtr pEndState);

    protected:
        void paintEvent(QPaintEvent *event) override;
    };
} // namespace NS_OSBASE::statemachineviewer
