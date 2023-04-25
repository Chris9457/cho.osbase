// \brief Declaration of the class StateWidget

#include "StateWidget.h"
#include "StateWidgetRes.h"
#include "StateTitle.h"
#include "StateClientArea.h"
#include "osStateMachine/StateWalker.h"
#include "osCore/Misc/Scope.h"
#include <QtWidgets>
#include <iostream>

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nssm     = nsosbase::statemachine;

namespace NS_OSBASE::statemachineviewer {
    StateWidget::StateWidget(nssm::StatePtr pState) : AbstractStateWidget(pState) {
        auto *pLayout = new QVBoxLayout(this);

        pLayout->setMargin(0);
        pLayout->setSpacing(0);

        m_pTitle      = new StateTitle(pState);
        m_pClientArea = new StateClientArea(pState);
        pLayout->addWidget(m_pTitle);
        pLayout->addWidget(m_pClientArea);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void StateWidget::activate(const ActivationMode mode) {
        AbstractStateWidget::activate(mode);
        m_pTitle->setColor(getColor());
    }

    void StateWidget::deactivate() {
        AbstractStateWidget::deactivate();
        m_pTitle->setColor(getColor());
    }

    void StateWidget::addAbstractState(NS_OSBASE::statemachine::AbstractStatePtr pState) {
        m_pClientArea->addSubState(pState);
    }

    void StateWidget::resetTransition() {
        m_pClientArea->setTransition(nullptr, nullptr);
    }

    StateClientArea *StateWidget::getClientArea() const {
        return m_pClientArea;
    }

    void StateWidget::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);

        QPainter painter;
        painter.begin(this);
        auto const guard = nscore::make_scope_exit([&painter]() { painter.end(); });
        painter.setPen(getColor());
        auto const rect =
            QRect(0, 0, std::max(event->rect().width(), this->rect().width()), std::max(event->rect().height(), this->rect().height()));

        QColor color = getColor();
        color.setAlphaF(STATEWIDGET_BACKGROUND_TRANSPARENCY);
        painter.setBrush(color);

        const QPen pen(QBrush(getColor()), STATEWIDGET_BORDER_WIDTH);
        painter.setPen(pen);

        painter.drawRoundedRect(rect - QMargins(STATEWIDGET_BORDER_WIDTH - 1,
                                           STATEWIDGET_BORDER_WIDTH - 1,
                                           STATEWIDGET_BORDER_WIDTH - 1,
                                           STATEWIDGET_BORDER_WIDTH - 1),
            STATEWIDGET_RADIUS,
            STATEWIDGET_RADIUS);
    }
} // namespace NS_OSBASE::statemachineviewer
