// \brief Declaration of the class StateMachineWindow

#pragma once
#include "TransitionRecord.h"
#include "osStateMachine/State.h"
#include "osStateMachine/StateMachineRecord.h"
#include "osData/Log.h"
#include "osData/IDataExchange.h"
#include <QtWidgets>
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui {
    class StateMachineWindow;
}
QT_END_NAMESPACE

namespace NS_OSBASE::statemachineviewer {

    class AbstractStateWidget;
    class StateWidget;
    class StartStateWidget;
    class EndStateWidget;
    class HistoryStateWidget;

    class StateMachineWindow : public QMainWindow {
        Q_OBJECT

    public:
        StateMachineWindow();
        StateMachineWindow(const std::string &brokerHost, const unsigned short brokerPort);
        ~StateMachineWindow() override;

    private:
        class LogReceiverDelegate;
        using LogReceiverDelegatePtr = std::shared_ptr<LogReceiverDelegate>;

        void connectUI();
        void onOpen();
        void onConnect();
        void onProcessChanged(int index);
        void onStateMachineChanged(int index);
        void onTransitionChanged(int index);
        void onPreviousTransition();
        void onNextTransition();
        void onRewindTransition();
        void onForwardTransition();
        void onPlayTransition();
        void onPauseTransition();

        void loadFile(const QString &fileName);
        void connectLog(const std::string &host, const unsigned short port);
        Q_INVOKABLE void parseLogLine(QString logLine);
        void parseStateMachineRecord(const std::string &logTimestamp,
            const NS_OSBASE::data::LoggerProcess::Info &processInfo,
            const NS_OSBASE::statemachine::StateMachineRecord &record);
        NS_OSBASE::statemachine::AbstractStatePtr extractStateHierarchy(
            NS_OSBASE::statemachine::StatePtr &pRoot, const std::string &fullName);

        void updateToolbar(const KeyTransitionRecord &keyRecord);

        NS_OSBASE::statemachine::StatePtr getRoot(NS_OSBASE::statemachine::AbstractStatePtr pState) const;
        SMRecords::const_iterator getCurrentTransitionRecordsIterator() const;

        void reloadStateWidgets() const;
        void updateStateWidgets() const;
        void updateStateWidget(NS_OSBASE::statemachine::StatePtr pState) const;
        void updateStartStateWidget(NS_OSBASE::statemachine::StartStatePtr pStartState) const;
        void updateEndStateWidget(NS_OSBASE::statemachine::EndStatePtr pEndState) const;
        void updateHistoryStateWidget(NS_OSBASE::statemachine::HistoryStatePtr pHistoryState) const;
        void createStateWidget(NS_OSBASE::statemachine::AbstractStatePtr pAbstractState) const;
        StateWidget *findStateWidget(NS_OSBASE::statemachine::StatePtr pState) const;

        void displayTransitionRecord(const TransitionRecord &transitionRecord) const;
        void activateTransitionStates(const TransitionRecord &transitionRecord) const;
        template <typename TState>
        void activateTransitionStates(
            const TransitionRecord &transitionRecord, AbstractStateWidget *&pFromStateWidget, AbstractStateWidget *&pToStateWidget) const;

        QWidget *getStateDiagramWidget() const;
        void resetStateDiagram();

        Ui::StateMachineWindow *ui;
        SMRecords m_viewerRecords;
        QComboBox *m_pProcessComboBox        = nullptr;
        QComboBox *m_pSMComboBox             = nullptr;
        QTreeWidget *m_pTransitionDetailTree = nullptr;
        QDockWidget *m_pTransitionDetailDock = nullptr;
        NS_OSBASE::data::IDataExchangePtr m_pLogReceiver;
        LogReceiverDelegatePtr m_pLogReceiverDelegate;
    };
} // namespace NS_OSBASE::statemachineviewer
