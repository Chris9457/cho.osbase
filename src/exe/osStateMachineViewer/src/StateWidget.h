// \brief Declaration of the class StateWidget

#pragma once
#include "AbstractStateWidget.h"

class QPainter;

namespace cho::osbase::statemachineviewer {
    class StateTitle;
    class StateClientArea;

    class StateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        StateWidget(cho::osbase::statemachine::StatePtr pState);

        void activate(const ActivationMode mode) override;
        void deactivate() override;

        void addAbstractState(cho::osbase::statemachine::AbstractStatePtr pState);
        void resetTransition() override;
        StateClientArea *getClientArea() const;

    protected:
        void paintEvent(QPaintEvent *event) override;

        StateClientArea *m_pClientArea;
        StateTitle *m_pTitle;
    };
} // namespace cho::osbase::statemachineviewer
