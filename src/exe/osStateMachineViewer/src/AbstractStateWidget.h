// \brief Declaration of the class AbstractStateWidget

#pragma once
#include "osStateMachine/AbstractState.h"
#include <QWidget>

namespace cho::osbase::statemachineviewer {

    class StateClientArea;

    class AbstractStateWidget : public QWidget {
    public:
        enum class ActivationMode { None, CurrentState, FromState, ToState, Failure };

        cho::osbase::statemachine::AbstractStatePtr getState() const;
        virtual void activate(const ActivationMode mode);
        virtual void deactivate();
        void setToStateWidget(AbstractStateWidget *pToStateWidget);
        virtual void resetTransition();

    protected:
        explicit AbstractStateWidget(cho::osbase::statemachine::AbstractStatePtr pState);

        const QColor &getColor() const;

    private:
        StateClientArea *getLCAStateClientArea(AbstractStateWidget *pToStateWidget);

        cho::osbase::statemachine::AbstractStatePtr m_pState;
        QColor m_color;
    };
} // namespace cho::osbase::statemachineviewer
