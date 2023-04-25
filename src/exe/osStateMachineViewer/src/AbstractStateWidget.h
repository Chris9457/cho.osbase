// \brief Declaration of the class AbstractStateWidget

#pragma once
#include "osStateMachine/AbstractState.h"
#include <QWidget>

namespace NS_OSBASE::statemachineviewer {

    class StateClientArea;

    class AbstractStateWidget : public QWidget {
    public:
        enum class ActivationMode { None, CurrentState, FromState, ToState, Failure };

        NS_OSBASE::statemachine::AbstractStatePtr getState() const;
        virtual void activate(const ActivationMode mode);
        virtual void deactivate();
        void setToStateWidget(AbstractStateWidget *pToStateWidget);
        virtual void resetTransition();

    protected:
        explicit AbstractStateWidget(NS_OSBASE::statemachine::AbstractStatePtr pState);

        const QColor &getColor() const;

    private:
        StateClientArea *getLCAStateClientArea(AbstractStateWidget *pToStateWidget);

        NS_OSBASE::statemachine::AbstractStatePtr m_pState;
        QColor m_color;
    };
} // namespace NS_OSBASE::statemachineviewer
