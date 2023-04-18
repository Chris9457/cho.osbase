// \brief Declaration of the class AbstractStateWidget

#include "AbstractStateWidget.h"
#include "StateWidgetRes.h"
#include "StateClientArea.h"
#include "StateWidget.h"
#include <deque>

namespace nsosbase = cho::osbase;
namespace nssm     = nsosbase::statemachine;

namespace cho::osbase::statemachineviewer {
    AbstractStateWidget::AbstractStateWidget(nssm::AbstractStatePtr pState) : m_pState(pState), m_color(STATE_DEACTIVATED_COLOR) {
    }

    nssm::AbstractStatePtr AbstractStateWidget::getState() const {
        return m_pState;
    }

    void AbstractStateWidget::activate(const ActivationMode mode) {
        switch (mode) {
        case ActivationMode::CurrentState:
            m_color = STATE_ACTIVATED_COLOR_CURRENTSTATE;
            break;
        case ActivationMode::FromState:
            m_color = STATE_ACTIVATED_COLOR_FROMSTATE;
            break;
        case ActivationMode::ToState:
            m_color = STATE_ACTIVATED_COLOR_TOSTATE;
            break;
        case ActivationMode::Failure:
            m_color = STATE_ACTIVATED_COLOR_FAILURE;
            break;
        default:
            m_color = STATE_DEACTIVATED_COLOR;
            break;
        }
    }

    void AbstractStateWidget::deactivate() {
        m_color = STATE_DEACTIVATED_COLOR;
    }

    void AbstractStateWidget::setToStateWidget(AbstractStateWidget *pToStateWidget) {
        auto const pClientArea = getLCAStateClientArea(pToStateWidget);
        if (pClientArea != nullptr) {
            pClientArea->setTransition(this, pToStateWidget);
        }
    }

    void AbstractStateWidget::resetTransition() {
    }

    const QColor &AbstractStateWidget::getColor() const {
        return m_color;
    }

    StateClientArea *AbstractStateWidget::getLCAStateClientArea(AbstractStateWidget *pToStateWidget) {
        if (pToStateWidget == nullptr) {
            return qobject_cast<StateClientArea *>(qobject_cast<StateWidget *>(this)->getClientArea());
        }

        if (this == pToStateWidget) {
            return qobject_cast<StateClientArea *>(parentWidget());
        }

        auto getClientAreas = [](const AbstractStateWidget *pWidget) -> std::deque<StateClientArea *> {
            std::deque<StateClientArea *> clientAreas;
            if (auto const pStateWidget = qobject_cast<const StateWidget *>(pWidget); pStateWidget != nullptr) {
                clientAreas.push_back(pStateWidget->getClientArea());
            }

            auto pClientArea = qobject_cast<StateClientArea *>(pWidget->parentWidget());
            if (pClientArea == nullptr) {
                auto const pStateWidget = qobject_cast<const StateWidget *>(pWidget);
                if (pWidget == nullptr) {
                    return {};
                }
                return { { pStateWidget->getClientArea() } };
            }

            while (pClientArea != nullptr) {
                clientAreas.push_back(pClientArea);
                pClientArea = qobject_cast<StateClientArea *>(pClientArea->parentWidget()->parentWidget());
            }
            return clientAreas;
        };

        auto fromClientArea = getClientAreas(this);
        auto toClientArea   = getClientAreas(pToStateWidget);
        while (fromClientArea.size() > toClientArea.size()) {
            fromClientArea.pop_front();
        }
        while (toClientArea.size() > fromClientArea.size()) {
            toClientArea.pop_front();
        }

        while (!fromClientArea.empty() && !toClientArea.empty() && fromClientArea.front() != toClientArea.front()) {
            fromClientArea.pop_front();
            toClientArea.pop_front();
        }

        if (fromClientArea.empty()) {
            return nullptr;
        }

        return fromClientArea.front();
    }
}; // namespace cho::osbase::statemachineviewer
