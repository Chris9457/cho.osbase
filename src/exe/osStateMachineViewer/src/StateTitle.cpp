// \brief Declaration of the class StateTitle

#include "StateTitle.h"
#include "StateWidgetRes.h"
#include "osCore/Misc/Scope.h"
#include <QtWidgets>

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nssm     = nsosbase::statemachine;

namespace NS_OSBASE::statemachineviewer {
    StateTitle::StateTitle(nssm::AbstractStatePtr pState)
        : QLabel(QString::fromStdString(pState->getName())), m_color(STATE_DEACTIVATED_COLOR) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setAlignment(Qt::AlignCenter);
        setMinimumHeight(STATEWIDGET_TITLE_HEIGHT);
        setMinimumWidth(STATEWIDGET_MIN_WIDTH);
    }

    void StateTitle::setColor(const QColor &color) {
        m_color = color;
        setStyleSheet(QString("color:%1").arg(m_color.name()));
    }

    void StateTitle::paintEvent(QPaintEvent *event) {
        QLabel::paintEvent(event);

        QPainter painter;
        painter.begin(this);
        auto const guard = nscore::make_scope_exit([&painter]() { painter.end(); });

        const QPen pen(QBrush(m_color), STATEWIDGET_BORDER_WIDTH);
        painter.setPen(pen);

        auto const rect =
            QRect(0, 0, std::max(event->rect().width(), this->rect().width()), std::max(event->rect().height(), this->rect().height()));
        painter.drawLine(STATEWIDGET_BORDER_WIDTH,
            rect.bottom() - STATEWIDGET_BORDER_WIDTH + 1,
            rect.right() - STATEWIDGET_BORDER_WIDTH + 1,
            rect.bottom() - STATEWIDGET_BORDER_WIDTH + 1);
    }
} // namespace NS_OSBASE::statemachineviewer
