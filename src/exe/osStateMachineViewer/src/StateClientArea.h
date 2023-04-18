// \brief Declaration of the class StateClientArea

#pragma once
#include "osStateMachine/AbstractState.h"
#include <QtWidgets>

namespace cho::osbase::statemachineviewer {

    class AbstractStateWidget;

    class StateClientArea : public QWidget {
        Q_OBJECT

    public:
        StateClientArea(cho::osbase::statemachine::AbstractStatePtr pState);

        void addSubState(cho::osbase::statemachine::AbstractStatePtr pSubState);
        void setTransition(AbstractStateWidget *pFromAbstractStateWidget, AbstractStateWidget *pToAbstractStateWidget);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        void drawTransition(QPainter &painter) const;
        void drawRecursiveTransition(QPainter &painter) const;
        void drawInnerTransition(QPainter &painter) const;

        QBoxLayout *m_pLayout;
        QVBoxLayout *m_pHistoryLayout;
        AbstractStateWidget *m_pFromStateWidget;
        AbstractStateWidget *m_pToStateWidget;
    };
} // namespace cho::osbase::statemachineviewer
