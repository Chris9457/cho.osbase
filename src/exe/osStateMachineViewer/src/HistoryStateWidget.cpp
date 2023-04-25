// \brief Declaration of the class HistoryStateWidget

#include "HistoryStateWidget.h"
#include "StateWidgetRes.h"
#include "osCore/Misc/Scope.h"
#include <QtWidgets>

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nssm     = nsosbase::statemachine;

namespace NS_OSBASE::statemachineviewer {
    HistoryStateWidget::HistoryStateWidget(nssm::HistoryStatePtr pHistoryState) : AbstractStateWidget(pHistoryState) {
        setMinimumSize(HISTORYSTATEWIDGET_EDGE, HISTORYSTATEWIDGET_EDGE);
        setMaximumSize(HISTORYSTATEWIDGET_EDGE, HISTORYSTATEWIDGET_EDGE);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void HistoryStateWidget::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);
        QPainter painter;
        painter.begin(this);
        auto const guard = nscore::make_scope_exit([&painter]() { painter.end(); });

        constexpr QRect rect(HISTORYSTATEWIDGET_BORDER_WIDTH - 1,
            HISTORYSTATEWIDGET_BORDER_WIDTH - 1,
            LEAFSTATEWIDGET_EDGE - HISTORYSTATEWIDGET_BORDER_WIDTH - 1,
            LEAFSTATEWIDGET_EDGE - HISTORYSTATEWIDGET_BORDER_WIDTH - 1);
        const QPen pen(QBrush(getColor()), HISTORYSTATEWIDGET_BORDER_WIDTH);
        painter.setPen(pen);

        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(rect);
        painter.drawText(rect + QMargins(0, 0, 1, 1), Qt::AlignCenter, HISTORYSTATEWIDGET_LABEL);
    }
} // namespace NS_OSBASE::statemachineviewer
