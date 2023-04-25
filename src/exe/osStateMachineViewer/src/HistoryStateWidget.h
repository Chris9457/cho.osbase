// \brief Declaration of the class HistoryStateWidget

#pragma once
#include "AbstractStateWidget.h"
#include "osStateMachine/HistoryState.h"

class QPainter;

namespace NS_OSBASE::statemachineviewer {
    class HistoryStateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        HistoryStateWidget(NS_OSBASE::statemachine::HistoryStatePtr pHistoryState);

    protected:
        void paintEvent(QPaintEvent *event) override;
    };
} // namespace NS_OSBASE::statemachineviewer
