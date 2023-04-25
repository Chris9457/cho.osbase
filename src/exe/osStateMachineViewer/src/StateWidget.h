// \brief Declaration of the class StateWidget

#pragma once
#include "AbstractStateWidget.h"

class QPainter;

namespace NS_OSBASE::statemachineviewer {
    class StateTitle;
    class StateClientArea;

    class StateWidget : public AbstractStateWidget {
        Q_OBJECT

    public:
        StateWidget(NS_OSBASE::statemachine::StatePtr pState);

        void activate(const ActivationMode mode) override;
        void deactivate() override;

        void addAbstractState(NS_OSBASE::statemachine::AbstractStatePtr pState);
        void resetTransition() override;
        StateClientArea *getClientArea() const;

    protected:
        void paintEvent(QPaintEvent *event) override;

        StateClientArea *m_pClientArea;
        StateTitle *m_pTitle;
    };
} // namespace NS_OSBASE::statemachineviewer
