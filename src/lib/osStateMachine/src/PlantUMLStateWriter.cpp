// \brief Declaration of the class PlantUMLStateWriter

#include "PlantUMLStateWriter.h"
#include "FactoryNames.h"
#include "PlantUMLStateWriterRes.h"
#include "osStateMachine/StateWalker.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include <algorithm>

namespace NS_OSBASE::statemachine {

    OS_REGISTER_FACTORY_N(IStateWriter, PlantUMLStateWriter, 0, STATE_WRITER_PLANTUML);

    namespace {
        std::string makePadding(const int rep) {
            std::ostringstream os;
            for (size_t i = 0; i < rep; ++i) {
                os << PLANTUML_PADDING;
            }
            return os.str();
        }
    } // namespace

    /*
     * \class PlantUMLStateWriter
     */
    void PlantUMLStateWriter::writeState(StatePtr pState) {
        writeState(pState, 0);

        if (!m_legendLines.empty()) {
            std::sort(m_legendLines.begin(), m_legendLines.end());

            m_buffer << PLANTUML_START_LEGEND << std::endl;
            for (auto const &legendLine : m_legendLines) {
                m_buffer << PLANTUML_PADDING << legendLine << std::endl;
            }
            m_buffer << PLANTUML_END_LEGEND << std::endl;
        }
    }

    void PlantUMLStateWriter::flush(std::ostream &os) const {
        os << PLANTUML_HEADER << std::endl;
        os << m_buffer.str();
        os << PLANTUML_FOOTER << std::endl;
    }

    void PlantUMLStateWriter::writeState(StatePtr pState, const int padding) {
        auto const strPadding = makePadding(padding);

        m_buffer << strPadding << PLANTUML_STATE << " " << pState->getName() << " " << PLANTUML_STARTBLOCK << std::endl;

        // write all substate
        for (auto const &pSubState : StateWalker<WalkType::direct>(pState)) {
            if (pSubState->isState()) {
                writeState(std::dynamic_pointer_cast<State>(pSubState), padding + 1);
            } else if (pSubState->isStartState()) {
                for (auto &&[evtName, pTransition] : pSubState->getTransitions()) {
                    writeTransition(evtName, pSubState, pTransition, padding + 1);
                }
            }
        }

        m_buffer << strPadding << PLANTUML_ENDBLOCK << std::endl;

        // write history state
        writeTransitionToHistoryState(std::dynamic_pointer_cast<State>(pState), padding);

        // write transitions
        for (auto &&[evtName, pTransition] : pState->getTransitions()) {
            writeTransition(evtName, pState, pTransition, padding);
        }

        m_buffer << std::endl;
    }

    void PlantUMLStateWriter::writeTransitionToHistoryState(StatePtr pState, const int padding) {
        auto const itHistoryState = std::find_if(StateWalker<WalkType::direct>(std::dynamic_pointer_cast<State>(pState)).begin(),
            StateWalker<WalkType::direct>().end(),
            [](auto const pSubState) { return pSubState->isHistoryState(); });

        if (itHistoryState != StateWalker<WalkType::direct>().end()) {
            m_buffer << makePadding(padding) << pState->getName() << PLANTUML_TRANSITION_DOWNARROW << pState->getName()
                     << PLANTUML_HISTORYSTATE << std::endl;
        }
    }

    void PlantUMLStateWriter::writeTransition(
        const std::string &evtName, AbstractStatePtr pFromState, TransitionPtr pTransition, const int padding) {
        const std::string strPadding    = makePadding(padding);
        const std::string fromStateName = pFromState->isState() ? pFromState->getName() : PLANTUML_STARTSTATE; // name or [*]
        auto const strTransition =
            [&evtName, guardName = pTransition->getGuardName(), actionName = pTransition->getActionName()]() -> std::string {
            // evtName [guardName] / actionName
            if (evtName.empty() && guardName.empty() && actionName.empty()) {
                return "";
            }

            std::ostringstream oss;
            if (!evtName.empty()) {
                oss << evtName;
            }
            if (!guardName.empty()) {
                oss << " " << PLANTUML_STARTGUARD << guardName << PLANTUML_ENDGUARD;
            }
            if (!actionName.empty()) {
                oss << PLANTUML_ACTIONDELIMITER << actionName;
            }

            return oss.str();
        }();

        auto const pToState = pTransition->getToState();
        std::string transitionText;
        if (!pTransition->getName().empty()) {
            transitionText = PLANTUML_TRANSITION_DELIMITER + pTransition->getName();
            if (!strTransition.empty()) {
                m_legendLines.emplace_back(pTransition->getName() + PLANTUML_TRANSITION_DELIMITER + strTransition);
            }
        } else if (!strTransition.empty()) {
            transitionText = PLANTUML_TRANSITION_DELIMITER + strTransition;
        }

        if (pToState == nullptr) {
            // internal transition
            if (!strTransition.empty()) {
                m_buffer << strPadding << fromStateName << transitionText << std::endl;
            }

        } else {
            // external transition
            auto const toStateName = pToState->isState() ? pToState->getName() : PLANTUML_ENDSTATE; // [*], name or name[H]
            m_buffer << strPadding << fromStateName << PLANTUML_TRANSITION_DOWNARROW << toStateName << transitionText << std::endl;
        }
    }
} // namespace NS_OSBASE::statemachine
