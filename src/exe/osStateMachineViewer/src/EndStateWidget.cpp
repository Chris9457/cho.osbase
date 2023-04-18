// \brief Declaration of the class EndStateWidget

#include "EndStateWidget.h"
#include "StateWidgetRes.h"
#include "osCore/Misc/Scope.h"
#include <QtWidgets>

namespace nsosbase = cho::osbase;
namespace nscore   = nsosbase::core;
namespace nssm     = nsosbase::statemachine;

namespace cho::osbase::statemachineviewer {
    EndStateWidget::EndStateWidget(nssm::EndStatePtr pEndState) : AbstractStateWidget(pEndState) {
        setMinimumSize(LEAFSTATEWIDGET_EDGE, LEAFSTATEWIDGET_EDGE);
        setMaximumSize(LEAFSTATEWIDGET_EDGE, LEAFSTATEWIDGET_EDGE);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void EndStateWidget::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);
        QPainter painter;
        painter.begin(this);
        auto const guard = nscore::make_scope_exit([&painter]() { painter.end(); });

        const QRect rect(ENDSTATEWIDGET_BORDER_WIDTH - 1,
            ENDSTATEWIDGET_BORDER_WIDTH,
            LEAFSTATEWIDGET_EDGE - ENDSTATEWIDGET_BORDER_WIDTH - 1,
            LEAFSTATEWIDGET_EDGE - ENDSTATEWIDGET_BORDER_WIDTH - 1);
        constexpr int innerMargin = (LEAFSTATEWIDGET_EDGE - ENDSTATEWIDGET_INNER_EDGE) / 2;
        const QRect innerRect(rect - QMargins(innerMargin, innerMargin, innerMargin, innerMargin));

        const QPen pen(QBrush(getColor()), ENDSTATEWIDGET_BORDER_WIDTH);
        painter.setPen(pen);

        painter.setBrush(getColor());
        painter.drawEllipse(innerRect);

        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(rect);
    }
} // namespace cho::osbase::statemachineviewer
