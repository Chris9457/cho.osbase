// \brief Declaration of the class HistoryStateWidget

#pragma once
#include "AbstractStateWidget.h"
#include "osStateMachine/HistoryState.h"

class QPainter;

namespace cho::osbase::statemachineviewer {
    class HistoryStateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        HistoryStateWidget(cho::osbase::statemachine::HistoryStatePtr pHistoryState);

    protected:
        void paintEvent(QPaintEvent *event) override;
    };
} // namespace cho::osbase::statemachineviewer
