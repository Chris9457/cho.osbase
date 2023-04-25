// \brief Declaration of the class StartStateWidget

#include "StartStateWidget.h"
#include "StateWidgetRes.h"
#include "osCore/Misc/Scope.h"
#include <QtWidgets>

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nssm     = nsosbase::statemachine;

namespace NS_OSBASE::statemachineviewer {
    StartStateWidget::StartStateWidget(nssm::StartStatePtr pStartState) : AbstractStateWidget(pStartState) {
        setMinimumSize(LEAFSTATEWIDGET_EDGE, LEAFSTATEWIDGET_EDGE);
        setMaximumSize(LEAFSTATEWIDGET_EDGE, LEAFSTATEWIDGET_EDGE);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void StartStateWidget::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);
        QPainter painter;
        painter.begin(this);
        auto const guard = nscore::make_scope_exit([&painter]() { painter.end(); });

        constexpr QRect rect(0, 0, LEAFSTATEWIDGET_EDGE - 1, LEAFSTATEWIDGET_EDGE - 1);
        painter.setBrush(getColor());
        painter.setPen(getColor());
        painter.drawEllipse(rect);
    }
} // namespace NS_OSBASE::statemachineviewer
