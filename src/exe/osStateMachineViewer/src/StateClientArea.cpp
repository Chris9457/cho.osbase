// \brief Declaration of the class StateClientArea

#include "StateClientArea.h"
#include "StateWidget.h"
#include "StateWidgetRes.h"
#include "StartStateWidget.h"
#include "EndStateWidget.h"
#include "HistoryStateWidget.h"
#include "osStateMachine/State.h"
#include "osCore/Misc/Scope.h"

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nssm     = nsosbase::statemachine;

namespace NS_OSBASE::statemachineviewer {
    StateClientArea::StateClientArea(nssm::AbstractStatePtr pState) : m_pHistoryLayout(new QVBoxLayout(this)) {
        const bool bHorizontalLayout = pState->getDepth() % 2 == 0;

        if (bHorizontalLayout) {
            m_pLayout = new QHBoxLayout();
        } else {
            m_pLayout = new QVBoxLayout();
        }
        m_pHistoryLayout->addLayout(m_pLayout);
        m_pLayout->setSpacing(STATEWIDGET_SPACING);
        m_pLayout->setMargin(STATEWIDGET_MARGIN);
    }

    void StateClientArea::addSubState(nssm::AbstractStatePtr pSubState) {
        if (pSubState->isStartState()) {
            auto const pStartState = new StartStateWidget(std::dynamic_pointer_cast<nssm::StartState>(pSubState));
            m_pLayout->addWidget(pStartState, 0, Qt::AlignHCenter);
        } else if (pSubState->isEndState()) {
            auto const pEndState = new EndStateWidget(std::dynamic_pointer_cast<nssm::EndState>(pSubState));
            m_pLayout->addWidget(pEndState, 0, Qt::AlignHCenter);
        } else if (pSubState->isHistoryState()) {
            auto const pHistoryState = new HistoryStateWidget(std::dynamic_pointer_cast<nssm::HistoryState>(pSubState));
            m_pHistoryLayout->insertWidget(0, pHistoryState, 1, Qt::AlignRight);
        } else {
            auto const pSubStateWidget = new StateWidget(std::dynamic_pointer_cast<nssm::State>(pSubState));
            m_pLayout->addWidget(pSubStateWidget);
        }
    }

    void StateClientArea::setTransition(AbstractStateWidget *pFromAbstractStateWidget, AbstractStateWidget *pToAbstractStateWidget) {
        m_pFromStateWidget = pFromAbstractStateWidget;
        m_pToStateWidget   = pToAbstractStateWidget;
    }

    void StateClientArea::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);

        if (m_pFromStateWidget == nullptr) {
            return;
        }

        QPainter painter;
        painter.begin(this);
        auto const guard = nscore::make_scope_exit([&painter]() { painter.end(); });

        const QPen pen(QBrush(STATE_ACTIVATED_COLOR_FROMSTATE), TRANSITION_LINE_WIDTH);
        painter.setPen(pen);

        if (m_pToStateWidget == nullptr) {
            drawInnerTransition(painter);
        } else if (m_pFromStateWidget == m_pToStateWidget) {
            drawRecursiveTransition(painter);
        } else {
            drawTransition(painter);
        }
    }

    void StateClientArea::drawTransition(QPainter &painter) const {
        auto const fromRect =
            QRect(m_pFromStateWidget == parentWidget() ? mapFromParent(QPoint(0, 0)) : m_pFromStateWidget->mapTo(this, QPoint(0, 0)),
                m_pFromStateWidget->size());
        auto const toRect =
            QRect(m_pToStateWidget == parentWidget() ? mapFromParent(QPoint(0, 0)) : m_pToStateWidget->mapTo(this, QPoint(0, 0)),
                m_pToStateWidget->size());

        QRegion region(rect());
        QPoint startPoint, endPoint;

        if (fromRect.intersected(toRect) == toRect) {
            region -= toRect;

            endPoint = QPoint(toRect.center());

            // find nearest border
            auto const dLeft   = endPoint.x() - fromRect.left();
            auto const dTop    = endPoint.y() - fromRect.top();
            auto const dRight  = fromRect.right() - endPoint.x();
            auto const dBottom = fromRect.bottom() - endPoint.y();
            auto const dMin    = std::min(std::min(dLeft, dRight), std::min(dTop, dBottom));

            startPoint = dMin == dLeft   ? QPoint(fromRect.left(), endPoint.y())
                         : dMin == dTop  ? QPoint(endPoint.x(), fromRect.top())
                         : dMin == dLeft ? QPoint(fromRect.right(), endPoint.y())
                                         : QPoint(endPoint.x(), fromRect.bottom());
        } else if (toRect.intersected(fromRect) == fromRect) {
            region -= fromRect;

            startPoint = QPoint(fromRect.center());

            // find nearest border
            auto const dLeft   = startPoint.x() - toRect.left();
            auto const dTop    = startPoint.y() - toRect.top();
            auto const dRight  = toRect.right() - startPoint.x();
            auto const dBottom = toRect.bottom() - startPoint.y();
            auto const dMin    = std::min(std::min(dLeft, dRight), std::min(dTop, dBottom));

            endPoint = dMin == dLeft   ? QPoint(toRect.left(), startPoint.y())
                       : dMin == dTop  ? QPoint(startPoint.x(), toRect.top())
                       : dMin == dLeft ? QPoint(toRect.right(), startPoint.y())
                                       : QPoint(startPoint.x(), toRect.bottom());
        } else {
            region -= fromRect;
            region -= toRect;
            startPoint = QPoint(fromRect.center());
            endPoint   = QPoint(toRect.center());
        }

        painter.setClipRegion(region);
        painter.drawLine(startPoint, endPoint);
    }

    void StateClientArea::drawRecursiveTransition(QPainter &painter) const {
        auto const topRight = m_pFromStateWidget == parentWidget() ? mapFromParent(parentWidget()->rect().topRight())
                                                                   : m_pFromStateWidget->mapTo(this, m_pFromStateWidget->rect().topRight());
        const QRect rectArc(topRight - QPoint(TRANSITION_RECURSIVE_EDGE / 2, TRANSITION_RECURSIVE_EDGE / 2),
            QSize(TRANSITION_RECURSIVE_EDGE, TRANSITION_RECURSIVE_EDGE));
        painter.drawArc(rectArc, -90 * 16, 270 * 16);

        constexpr int arrowEdge = TRANSITION_RECURSIVE_EDGE / 4;
        const QPoint arrowEnd   = rectArc.topLeft() + QPoint(0, TRANSITION_RECURSIVE_EDGE / 2);
        painter.drawLine(QPoint(-arrowEdge / 2 + arrowEnd.x(), -arrowEdge + arrowEnd.y()), arrowEnd);
        painter.drawLine(QPoint(arrowEdge + arrowEnd.x(), -arrowEdge + arrowEnd.y()), arrowEnd);
    }

    void StateClientArea::drawInnerTransition(QPainter &painter) const {
        auto const pStateWidget = qobject_cast<StateWidget *>(m_pFromStateWidget);
        if (pStateWidget == nullptr) {
            return;
        }

        auto const pClientArea = pStateWidget->getClientArea();
        auto const margin      = m_pHistoryLayout->margin();
        auto const topLeft     = pClientArea == this ? QPoint(margin, margin) : pClientArea->mapTo(this, QPoint(margin, margin));

        const QRect rectArc(topLeft, QSize(TRANSITION_INNER_EDGE, TRANSITION_INNER_EDGE));
        painter.drawArc(rectArc, -45 * 16, -315 * 16);

        constexpr int arrowEdge = TRANSITION_RECURSIVE_EDGE / 4;
        const QPoint arrowEnd   = rectArc.topRight() + QPoint(0, TRANSITION_RECURSIVE_EDGE / 2);
        painter.drawLine(QPoint(-arrowEdge + arrowEnd.x(), -arrowEdge + arrowEnd.y()), arrowEnd);
        painter.drawLine(QPoint(arrowEdge / 2 + arrowEnd.x(), -arrowEdge + arrowEnd.y()), arrowEnd);
    }
} // namespace NS_OSBASE::statemachineviewer
