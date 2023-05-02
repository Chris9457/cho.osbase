// \brief Declaration of the class StateMachineWindow

#include "StateMachineWindow.h"
#include "osStateMachine/StateMachineRecord.h"
#include "./ui_statemachinewindow.h"
#include "StateWidget.h"
#include "StartStateWidget.h"
#include "EndStateWidget.h"
#include "HistoryStateWidget.h"
#include "StateWidgetRes.h"
#include "osLog.h"
#include "ConnectionDialog.h"
#include "osStateMachine/osStateMachine.h"
#include "osStateMachine/StateMachineRes.h"
#include "osCoreImpl/CoreImpl.h"
#include "osData/IDataExchange.h"
#include "osApplication/ServiceException.h"
#include <filesystem>
#include <fstream>

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nsdata   = nsosbase::data;
namespace nsapp    = nsosbase::application;
namespace nssm     = nsosbase::statemachine;

namespace NS_OSBASE::statemachineviewer {
    /*
     * \class StateMachineWindow::LogReceiverDelegate
     */
    class StateMachineWindow::LogReceiverDelegate : public nsdata::IDataExchange::IDelegate {
    public:
        LogReceiverDelegate(StateMachineWindow &window) : m_window(window) {
        }

        void onConnected(const bool) override {
        }

        void onFailure(std::string &&) override {
        }

        void onDataReceived(nsdata::ByteBuffer &&buffer) override {
            auto const logLine = QString::fromStdString(type_cast<std::string>(std::forward<nsdata::ByteBuffer>(buffer)));
            QMetaObject::invokeMethod(&m_window, "parseLogLine", Qt::QueuedConnection, Q_ARG(QString, logLine));
        }

    private:
        StateMachineWindow &m_window;
    };

    /*
     * \class StateMachineWindow
     */
    StateMachineWindow::StateMachineWindow() : ui(new Ui::StateMachineWindow) {
        connectUI();
    }

    StateMachineWindow::StateMachineWindow(const NS_OSBASE::data::Uri &uri, const std::string &realm) : StateMachineWindow() {

        connectLog(uri, realm);

        ui->openAction->setEnabled(false);
        ui->connectAction->setEnabled(false);
    }

    StateMachineWindow::~StateMachineWindow() {
        if (m_pLogReceiver != nullptr) {
            m_pLogReceiver->close();
        }

        delete ui;
    }

    void StateMachineWindow::connectUI() {
        ui->setupUi(this);

        connect(ui->openAction, &QAction::triggered, this, &StateMachineWindow::onOpen);
        connect(ui->connectAction, &QAction::triggered, this, &StateMachineWindow::onConnect);

        // Tool bar
        ui->toolBar->addAction(ui->openAction);
        ui->toolBar->addAction(ui->connectAction);

        m_pProcessComboBox = new QComboBox;
        m_pProcessComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        ui->toolBar->addWidget(m_pProcessComboBox);

        using TCurrentIndexChanged = void (QComboBox::*)(int);
        connect(m_pProcessComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &StateMachineWindow::onProcessChanged);

        m_pSMComboBox = new QComboBox;
        m_pSMComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        ui->toolBar->addWidget(m_pSMComboBox);
        connect(m_pSMComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &StateMachineWindow::onStateMachineChanged);

        ui->stateMachineName->hide();

        ui->transitionSlider->setRange(0, 0);
        connect(ui->transitionSlider, &QSlider::valueChanged, this, &StateMachineWindow::onTransitionChanged);

        ui->playButton->hide();
        ui->rewindButton->setEnabled(false);
        ui->forwardButton->setEnabled(false);
        ui->previousButton->setEnabled(false);
        ui->nextButton->setEnabled(false);
        connect(ui->previousButton, &QPushButton::clicked, this, &StateMachineWindow::onPreviousTransition);
        connect(ui->nextButton, &QPushButton::clicked, this, &StateMachineWindow::onNextTransition);
        connect(ui->rewindButton, &QPushButton::clicked, this, &StateMachineWindow::onRewindTransition);
        connect(ui->forwardButton, &QPushButton::clicked, this, &StateMachineWindow::onForwardTransition);

        m_pTransitionDetailDock = new QDockWidget(TRANSITION_HEADER_DEFAULT_TITLE, ui->centralwidget);
        m_pTransitionDetailDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        m_pTransitionDetailTree = new QTreeWidget(m_pTransitionDetailDock);
        m_pTransitionDetailTree->setColumnCount(2);
        m_pTransitionDetailTree->setHeaderLabels(QStringList() << TRANSITION_HEADER_FIELDNAME << TRANSITION_HEADER_VALUE);
        m_pTransitionDetailDock->setWidget(m_pTransitionDetailTree);
        addDockWidget(Qt::RightDockWidgetArea, m_pTransitionDetailDock);

        connect(ui->transitionSlider, &QSlider::sliderMoved, [this](int value) {
            auto const itCurrentRecord = getCurrentTransitionRecordsIterator();
            if (itCurrentRecord == m_viewerRecords.cend()) {
                return;
            }
            auto const strTimeStamp = QString::fromStdString(nsdata::LoggerTimestamp(itCurrentRecord->second[value].timestamp).toString());

            auto const topLeft     = ui->transitionSlider->mapToGlobal(QPoint(0, 0));
            auto const bottomRight = ui->transitionSlider->mapToGlobal(ui->transitionSlider->rect().bottomRight());

            auto const left =
                topLeft.x() + value * (bottomRight.x() - topLeft.x()) / (ui->transitionSlider->maximum() - ui->transitionSlider->minimum());
            QToolTip::showText(QPoint(left, topLeft.y()), strTimeStamp);
        });
    }

    void StateMachineWindow::onOpen() {
        const QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()) {
            resetStateDiagram();
            loadFile(fileName);
        }
    }

    void StateMachineWindow::onConnect() {
        ConnectionDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            resetStateDiagram();
            connectLog(QString("ws://%1:%2").arg(dlg.getHost()).arg(dlg.getPort()).toStdString(), dlg.getRealm().toStdString());
        }
    }

    void StateMachineWindow::onProcessChanged(int index) {
        const QString processName = m_pProcessComboBox->itemText(index == -1 ? 0 : index);
        if (m_pProcessComboBox->currentText() != processName) {
            return;
        }
        m_pSMComboBox->clear();

        // Retrieve the list of sm by process
        QStringList smNames;
        for (auto &&[keyRecord, _] : m_viewerRecords) {
            if (keyRecord.processName == processName) {
                QString smName = "(0x" + QString::number(keyRecord.smId, 16) + ")";
                if (!keyRecord.smName.empty()) {
                    smName = QString::fromStdString(keyRecord.smName) + " " + smName;
                }
                smNames.push_back(smName);
            }
        }

        m_pSMComboBox->clear();
        m_pSMComboBox->addItems(smNames);
    }

    void StateMachineWindow::onStateMachineChanged(int) {
        reloadStateWidgets();
        auto const itCurrentRecord = getCurrentTransitionRecordsIterator();
        if (itCurrentRecord != m_viewerRecords.cend() && !itCurrentRecord->second.empty()) {
            auto const maxRange = static_cast<int>(itCurrentRecord->second.size()) - 1;
            ui->transitionSlider->setRange(0, maxRange);
            ui->transitionSlider->setValue(maxRange);
        } else {
            ui->transitionSlider->setRange(0, 0);
        }
    }

    void StateMachineWindow::onTransitionChanged(int index) {
        if (ui->transitionSlider->maximum() > 0) {
            const bool bCanNavigate = (ui->transitionSlider->maximum() - ui->transitionSlider->minimum()) > 1;
            auto const minRange     = ui->transitionSlider->minimum();
            auto const maxRange     = ui->transitionSlider->maximum();
            ui->previousButton->setEnabled(bCanNavigate && index > minRange);
            ui->nextButton->setEnabled(bCanNavigate && index < maxRange);
            ui->rewindButton->setEnabled(ui->previousButton->isEnabled());
            ui->forwardButton->setEnabled(ui->nextButton->isEnabled());
        }

        auto const itCurrentRecord = getCurrentTransitionRecordsIterator();
        if (itCurrentRecord == m_viewerRecords.cend()) {
            return;
        }

        auto const &transitionRecords = itCurrentRecord->second;
        if (transitionRecords.size() <= static_cast<size_t>(index)) {
            return;
        }

        auto const &transitionRecord = transitionRecords[static_cast<size_t>(index)];
        displayTransitionRecord(transitionRecord);
        activateTransitionStates(transitionRecord);
    }

    void StateMachineWindow::onPreviousTransition() {
        ui->transitionSlider->setValue(ui->transitionSlider->value() - 1);
    }

    void StateMachineWindow::onNextTransition() {
        ui->transitionSlider->setValue(ui->transitionSlider->value() + 1);
    }

    void StateMachineWindow::onRewindTransition() {
        ui->transitionSlider->setValue(0);
    }

    void StateMachineWindow::onForwardTransition() {
        ui->transitionSlider->setValue(ui->transitionSlider->maximum());
    }

    void StateMachineWindow::onPlayTransition() {
    }

    void StateMachineWindow::onPauseTransition() {
    }

    void StateMachineWindow::loadFile(const QString &fileName) {
        const std::filesystem::path path = fileName.toStdString();
        if (!exists(path)) {
            return;
        }

        std::string line;
        std::ifstream is(path);
        while (std::getline(is, line)) {
            auto const logLine = QString::fromStdString(line);
            parseLogLine(logLine);
        }

        onRewindTransition();
    }

    void StateMachineWindow::connectLog(const NS_OSBASE::data::Uri &uri, const std::string &realm) {
        auto const pTaskLoop   = std::make_shared<nsapp::TaskLoop>();
        auto const pLogService = log::makeStub(uri, realm, pTaskLoop);

        try {
            pLogService->connect();
            auto const guard  = nscore::make_scope_exit([&pLogService]() { pLogService->disconnect(); });
            auto const logUri = pLogService->getOutputLogUri();

            m_pLogReceiver         = nsdata::makeDataExchange(logUri.scheme);
            m_pLogReceiverDelegate = std::make_shared<LogReceiverDelegate>(*this);
            m_pLogReceiver->setDelegate(m_pLogReceiverDelegate);
            m_pLogReceiver->open(logUri);
        } catch (const nsapp::ServiceException &) {
            QMessageBox msgBox;
            msgBox.setText("Unable to connect to the log service");
            msgBox.exec();
        }
    }

    void StateMachineWindow::parseLogLine(QString logLine) {
        auto const pStream                = nscore::makeJsonStream(std::istringstream(logLine.toStdString()));
        const std::string keyStateMachine = nssm::LOGGER_KEY_STATEMACHINE;

        if (pStream->isKeyExist(keyStateMachine)) {
            auto const logTimestamp = pStream->getKeyValue(std::string(nsdata::LOGGER_KEY_TIMESTAMP), std::string{});
            auto const processInfo  = pStream->getKeyValue(std::string(nsdata::LOGGER_KEY_PROCESS), nsdata::LoggerProcess::Info{});
            auto const record       = pStream->getKeyValue(keyStateMachine, nssm::StateMachineRecord{});
            if (!record.currentState.empty()) {
                parseStateMachineRecord(logTimestamp, processInfo, record);
            }
        }
    }

    void StateMachineWindow::parseStateMachineRecord(
        const std::string &logTimestamp, const nsdata::LoggerProcess::Info &processInfo, const nssm::StateMachineRecord &smRecord) {
        KeyTransitionRecord keyViewerRecord;
        keyViewerRecord.processName = processInfo.name;
        if (smRecord.name.has_value()) {
            keyViewerRecord.smName = smRecord.name.value();
        }

        std::istringstream iss(smRecord.id);
        char dummy;
        iss >> dummy >> dummy >> std::hex >> keyViewerRecord.smId;

        nssm::StatePtr pRoot;
        auto itViewerRecord = m_viewerRecords.find(keyViewerRecord);
        if (itViewerRecord != m_viewerRecords.cend()) {
            pRoot          = itViewerRecord->first.pRoot;
            itViewerRecord = m_viewerRecords.insert({ keyViewerRecord, {} }).first;
        }

        auto pCurrentState = extractStateHierarchy(pRoot, smRecord.currentState);
        if (itViewerRecord == m_viewerRecords.cend()) {
            keyViewerRecord.pRoot = pRoot;
            itViewerRecord        = m_viewerRecords.insert({ keyViewerRecord, {} }).first;
        }

        auto transitionRecords = smRecord.transitionRecords;
        if (transitionRecords.empty()) {
            nssm::TransitionRecord transitionRecord;
            transitionRecord.name      = TRANSITION_HEADER_DEFAULT_TITLE;
            transitionRecord.timestamp = logTimestamp;
            transitionRecord.fromState = smRecord.currentState;
            transitionRecords.emplace_back(std::move(transitionRecord));
        }

        for (auto const &smTransitionRecord : transitionRecords) {
            TransitionRecord viewerTransitionRecord;

            viewerTransitionRecord.name =
                smTransitionRecord.name.has_value() ? smTransitionRecord.name.value() : TRANSITION_HEADER_DEFAULT_TITLE;

            viewerTransitionRecord.timestamp = nsdata::LoggerTimestamp::fromString(smTransitionRecord.timestamp).getTimePoint();

            if (smTransitionRecord.evtName.has_value()) {
                viewerTransitionRecord.evtName = smTransitionRecord.evtName.value();
            }
            if (smTransitionRecord.guardName.has_value()) {
                viewerTransitionRecord.guardName = smTransitionRecord.guardName.value();
            }

            viewerTransitionRecord.pCurrentState = pCurrentState;

            if (smTransitionRecord.fromState.has_value()) {
                viewerTransitionRecord.pFromState = extractStateHierarchy(pRoot, smTransitionRecord.fromState.value());
            }

            if (smTransitionRecord.toState.has_value()) {
                viewerTransitionRecord.pToState = extractStateHierarchy(pRoot, smTransitionRecord.toState.value());
                pCurrentState                   = viewerTransitionRecord.pToState;
            }

            if (smTransitionRecord.actionRecords.has_value()) {
                for (auto const &smActionRecord : smTransitionRecord.actionRecords.value()) {
                    ActionRecord viewerActionRecord;
                    viewerActionRecord.timestamp = nsdata::LoggerTimestamp::fromString(smActionRecord.timestamp).getTimePoint();
                    viewerActionRecord.name      = smActionRecord.name;
                    viewerTransitionRecord.actionRecords.push_back(viewerActionRecord);
                }
            }

            if (smRecord.failure.has_value()) {
                viewerTransitionRecord.failure = smRecord.failure.value();
            }

            itViewerRecord->second.push_back(viewerTransitionRecord);
        }

        updateToolbar(keyViewerRecord);
        updateStateWidgets();
    }

    nssm::AbstractStatePtr StateMachineWindow::extractStateHierarchy(nssm::StatePtr &pRoot, const std::string &fullName) {
        std::vector<std::string> stateNames;
        std::string parsed;
        std::istringstream iss(fullName);
        while (std::getline(iss, parsed, nssm::STATE_SEPARATOR[0])) {
            stateNames.push_back(parsed);
        }

        if (stateNames.size() <= 1) {
            return nullptr;
        }

        if (pRoot == nullptr) {
            pRoot = nssm::State::createRoot(stateNames[1]);
        } else if (pRoot->getName() != stateNames[1]) {
            return nullptr;
        }

        nssm::AbstractStatePtr pCurrentAbstractState = pRoot;
        for (size_t index = 2; index < stateNames.size(); ++index) {
            if (!pCurrentAbstractState->isState()) {
                return nullptr;
            }

            auto const pCurrentState = std::dynamic_pointer_cast<nssm::State>(pCurrentAbstractState);
            auto stateName           = stateNames[index];
            if (stateName.find(nssm::HISTORY_STATE_NAME) != std::string::npos && !pCurrentState->hasHistoryState()) {
                return pCurrentState->setHistoryState();
            }

            bool isStartState = false;
            bool isEndState   = false;
            if (auto const posStart = stateName.find(nssm::START_STATE_SUFFIX_NAME); posStart != std::string::npos) {
                isStartState = true;
                stateName    = stateName.substr(0, posStart);
            } else if (auto const posEnd = stateNames[index].find(nssm::END_STATE_SUFFIX_NAME); posEnd != std::string::npos) {
                isEndState = true;
                stateName  = stateName.substr(0, posEnd);
            }

            auto const itSubState = std::find_if(nssm::StateWalker<nssm::WalkType::direct>(pCurrentState).begin(),
                nssm::StateWalker<nssm::WalkType::direct>().end(),
                [&stateName](auto const pSubState) { return pSubState->getName() == stateName; });
            if (itSubState == nssm::StateWalker<nssm::WalkType::direct>().end()) {
                if (isStartState) {
                    return pCurrentState->setStartState(stateName);
                }

                if (isEndState) {
                    return pCurrentState->addEndState(stateName);
                }

                pCurrentAbstractState = nssm::State::create(pCurrentState, stateName);
            } else {
                pCurrentAbstractState = *itSubState;
            }
        }

        return pCurrentAbstractState;
    }

    void StateMachineWindow::updateToolbar(const KeyTransitionRecord &keyRecord) {
        // Populate the process names
        const QString processName = QString::fromStdWString(keyRecord.processName);
        const int indexProcess    = m_pProcessComboBox->findText(processName);
        if (indexProcess == -1) {
            m_pProcessComboBox->addItem(processName);
        }

        onProcessChanged(m_pProcessComboBox->currentIndex());
    }

    nssm::StatePtr StateMachineWindow::getRoot(nssm::AbstractStatePtr pState) const {
        if (pState == nullptr) {
            return nullptr;
        }

        auto pRoot = pState->isState() ? std::dynamic_pointer_cast<nssm::State>(pState) : pState->getParent();
        if (pRoot == nullptr) {
            return nullptr;
        }

        while (pRoot->getParent() != nullptr) {
            pRoot = pRoot->getParent();
        }

        return pRoot;
    }

    void StateMachineWindow::updateStateWidgets() const {
        auto const itRecord = getCurrentTransitionRecordsIterator();
        if (itRecord == m_viewerRecords.cend()) {
            return;
        }

        auto const pRoot = itRecord->first.pRoot;
        if (pRoot == nullptr) {
            return;
        }

        for (auto &&pState : nssm::StateWalker<nssm::WalkType::recursive>(pRoot)) {
            if (pState->isState()) {
                updateStateWidget(std::dynamic_pointer_cast<nssm::State>(pState));
            } else if (pState->isStartState()) {
                updateStartStateWidget(std::dynamic_pointer_cast<nssm::StartState>(pState));
            } else if (pState->isEndState()) {
                updateEndStateWidget(std::dynamic_pointer_cast<nssm::EndState>(pState));
            } else if (pState->isHistoryState()) {
                updateHistoryStateWidget(std::dynamic_pointer_cast<nssm::HistoryState>(pState));
            }
        }
    }

    void StateMachineWindow::updateStateWidget(NS_OSBASE::statemachine::StatePtr pState) const {
        auto const pStateWidget = findStateWidget(pState);
        if (pStateWidget != nullptr) {
            return;
        }

        createStateWidget(pState);
    }

    void StateMachineWindow::updateStartStateWidget(NS_OSBASE::statemachine::StartStatePtr pStartState) const {
        auto const pStartStateWidgets = getStateDiagramWidget()->findChildren<StartStateWidget *>();
        auto const itStartStateWidget = std::find_if(pStartStateWidgets.cbegin(),
            pStartStateWidgets.cend(),
            [&pStartState](auto const pStartStateWidget) { return pStartStateWidget->getState() == pStartState; });

        if (itStartStateWidget != pStartStateWidgets.cend()) {
            return;
        }

        createStateWidget(pStartState);
    }

    void StateMachineWindow::updateEndStateWidget(NS_OSBASE::statemachine::EndStatePtr pEndState) const {
        auto const pEndStateWidgets = getStateDiagramWidget()->findChildren<EndStateWidget *>();
        auto const itEndStateWidget = std::find_if(pEndStateWidgets.cbegin(),
            pEndStateWidgets.cend(),
            [&pEndState](auto const pEndStateWidget) { return pEndStateWidget->getState() == pEndState; });

        if (itEndStateWidget != pEndStateWidgets.cend()) {
            return;
        }

        createStateWidget(pEndState);
    }

    void StateMachineWindow::updateHistoryStateWidget(NS_OSBASE::statemachine::HistoryStatePtr pHistoryState) const {
        auto const pHistoryStateWidgets = getStateDiagramWidget()->findChildren<HistoryStateWidget *>();
        auto const itEndStateWidget =
            std::find_if(pHistoryStateWidgets.cbegin(), pHistoryStateWidgets.cend(), [&pHistoryState](auto const pHistoryStateWidget) {
                return pHistoryStateWidget->getState()->getParent() == pHistoryState->getParent();
            });

        if (itEndStateWidget != pHistoryStateWidgets.cend()) {
            return;
        }

        createStateWidget(pHistoryState);
    }

    void StateMachineWindow::createStateWidget(NS_OSBASE::statemachine::AbstractStatePtr pAbstractState) const {
        if (pAbstractState->getParent() == nullptr) {
            // for the root, create the main StateWidget
            auto const pRoot = std::dynamic_pointer_cast<nssm::State>(pAbstractState);
            if (pRoot == nullptr) {
                throw std::exception("but: the root must be a state");
            }

            auto pLayout = getStateDiagramWidget()->layout();
            if (pLayout == nullptr) {
                pLayout = new QVBoxLayout(getStateDiagramWidget());
            } else {
                // in case of existing state widgets, remove them
                auto const pStateWidgets = getStateDiagramWidget()->findChildren<StateWidget *>();
                if (!pStateWidgets.empty()) {
                    delete *pStateWidgets.cbegin();
                }
            }

            pLayout->addWidget(new StateWidget(pRoot));
            auto const itRecord = getCurrentTransitionRecordsIterator();
            if (itRecord != m_viewerRecords.cend() && !itRecord->first.smName.empty()) {
                ui->stateMachineName->setText(QString::fromStdString(itRecord->first.smName));
                ui->stateMachineName->show();
            } else {
                ui->stateMachineName->hide();
            }
            return;
        }

        auto const pParentStateWidget = findStateWidget(pAbstractState->getParent());
        if (pParentStateWidget == nullptr) {
            throw std::exception("bug: parent state widget must exists");
        }

        pParentStateWidget->addAbstractState(pAbstractState);
    }

    QWidget *StateMachineWindow::getStateDiagramWidget() const {
        return ui->stateDiagramWidget;
    }

    void StateMachineWindow::resetStateDiagram() {
        auto const pStateWidgets = getStateDiagramWidget()->findChildren<StateWidget *>();
        if (!pStateWidgets.empty()) {
            delete *pStateWidgets.cbegin();
        }

        m_viewerRecords.clear();
        m_pProcessComboBox->clear();
        m_pSMComboBox->clear();
    }

    StateWidget *StateMachineWindow::findStateWidget(NS_OSBASE::statemachine::StatePtr pState) const {
        auto const pStateWidgets = getStateDiagramWidget()->findChildren<StateWidget *>();
        auto const itStateWidget = std::find_if(pStateWidgets.cbegin(), pStateWidgets.cend(), [&pState](auto const pStateWidget) {
            return pStateWidget->getState() == pState;
        });

        if (itStateWidget != pStateWidgets.cend()) {
            return *itStateWidget;
        }

        return nullptr;
    }

    SMRecords::const_iterator StateMachineWindow::getCurrentTransitionRecordsIterator() const {
        auto const processName = m_pProcessComboBox->currentText();
        auto smName            = m_pSMComboBox->currentIndex() != -1 ? m_pSMComboBox->currentText() : m_pSMComboBox->itemText(0);
        auto const startIndex  = smName.indexOf('(');
        auto const endIndex    = smName.indexOf(')');
        smName                 = smName.mid(startIndex + 3, endIndex - startIndex - 2);

        std::stringstream iss(smName.toStdString());
        unsigned long long smId = smName.toULongLong(nullptr, 16);
        iss >> std::hex >> smId;

        return std::find_if(m_viewerRecords.cbegin(), m_viewerRecords.cend(), [&processName, &smId](auto const record) {
            return record.first.processName == processName && record.first.smId == smId;
        });
    }

    void StateMachineWindow::reloadStateWidgets() const {
        auto const itRecord = getCurrentTransitionRecordsIterator();
        if (itRecord == m_viewerRecords.cend()) {
            return;
        }

        auto const pStateWidgets = getStateDiagramWidget()->findChildren<StateWidget *>();
        if (!pStateWidgets.empty()) {
            delete *pStateWidgets.cbegin();
        }

        auto const pRoot = itRecord->first.pRoot;
        if (pRoot == nullptr) {
            return;
        }

        updateStateWidgets();
    }

    void StateMachineWindow::displayTransitionRecord(const TransitionRecord &transitionRecord) const {
        m_pTransitionDetailTree->clear();

        m_pTransitionDetailDock->setWindowTitle(QString::fromStdString(transitionRecord.name));

        QList<QTreeWidgetItem *> pItems;
        pItems.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
            QStringList(
                { TRANSITION_FIELD_TIMESTAMP, QString::fromStdString(nsdata::LoggerTimestamp(transitionRecord.timestamp).toString()) })));

        if (!transitionRecord.failure.empty()) {
            auto const pFailureItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                QStringList({ TRANSITION_FIELD_FAILURE, QString::fromStdString(transitionRecord.failure) }));
            const QBrush failureBrush(Qt::red);
            pFailureItem->setForeground(0, failureBrush);
            pFailureItem->setForeground(1, failureBrush);
            pItems.append(pFailureItem);
        }

        if (transitionRecord.pCurrentState != nullptr) {
            auto const pCurrentStateItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                QStringList({ TRANSITION_FIELD_CURRENTSTATE, QString::fromStdString(transitionRecord.pCurrentState->getName()) }));
            const QBrush currentStateBrush(STATE_ACTIVATED_COLOR_CURRENTSTATE);
            pCurrentStateItem->setForeground(0, currentStateBrush);
            pCurrentStateItem->setForeground(1, currentStateBrush);
            pItems.append(pCurrentStateItem);
        }

        if (!transitionRecord.evtName.empty()) {
            pItems.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                QStringList({ TRANSITION_FIELD_EVTNAME, QString::fromStdString(transitionRecord.evtName) })));
        }

        if (!transitionRecord.guardName.empty()) {
            pItems.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                QStringList({ TRANSITION_FIELD_GUARDNAME, QString::fromStdString(transitionRecord.guardName) })));
        }

        if (transitionRecord.pFromState != nullptr) {
            auto const pFromStateItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                QStringList({ TRANSITION_FIELD_FROMSTATE, QString::fromStdString(transitionRecord.pFromState->getName()) }));
            const QBrush fromStateBrush(STATE_ACTIVATED_COLOR_FROMSTATE);
            pFromStateItem->setForeground(0, fromStateBrush);
            pFromStateItem->setForeground(1, fromStateBrush);
            pItems.append(pFromStateItem);
        }

        if (transitionRecord.pToState != nullptr) {
            auto const pToStateItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                QStringList({ TRANSITION_FIELD_TOSTATE, QString::fromStdString(transitionRecord.pToState->getName()) }));
            const QBrush toStateBrush(
                transitionRecord.pFromState != transitionRecord.pToState ? STATE_ACTIVATED_COLOR_TOSTATE : STATE_ACTIVATED_COLOR_FROMSTATE);
            pToStateItem->setForeground(0, toStateBrush);
            pToStateItem->setForeground(1, toStateBrush);
            pItems.append(pToStateItem);
        }

        QList<QTreeWidgetItem *> pItemsToExpand;
        if (!transitionRecord.actionRecords.empty()) {
            auto const pActionsItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList({ TRANSITION_FIELD_ACTIONS }));
            pItems.append(pActionsItem);
            pItemsToExpand.append(pActionsItem);

            for (size_t index = 0; index < transitionRecord.actionRecords.size(); ++index) {
                auto const &actionRecord      = transitionRecord.actionRecords[index];
                auto const pActionIndexWidget = new QTreeWidgetItem(pActionsItem, QStringList({ "[" + QString::number(index) + "]" }));
                pActionIndexWidget->setExpanded(true);
                pItems.append(pActionIndexWidget);
                pItemsToExpand.append(pActionIndexWidget);

                pItems.append(new QTreeWidgetItem(pActionIndexWidget,
                    QStringList({ TRANSITION_FIELD_TIMESTAMP,
                        QString::fromStdString(nsdata::LoggerTimestamp(actionRecord.timestamp).toString()) })));
                pItems.append(new QTreeWidgetItem(
                    pActionIndexWidget, QStringList({ TRANSITION_FIELD_ACTIONNAME, QString::fromStdString(actionRecord.name) })));
            }
        }
        m_pTransitionDetailTree->insertTopLevelItems(0, pItems);
        for (auto &&pItemToExpand : pItemsToExpand) {
            pItemToExpand->setExpanded(true);
        }
    }

    void StateMachineWindow::activateTransitionStates(const TransitionRecord &transitionRecord) const {
        AbstractStateWidget *pFromStateWidget = nullptr;
        AbstractStateWidget *pToStateWidget   = nullptr;

        activateTransitionStates<StateWidget>(transitionRecord, pFromStateWidget, pToStateWidget);
        activateTransitionStates<StartStateWidget>(transitionRecord, pFromStateWidget, pToStateWidget);
        activateTransitionStates<EndStateWidget>(transitionRecord, pFromStateWidget, pToStateWidget);
        activateTransitionStates<HistoryStateWidget>(transitionRecord, pFromStateWidget, pToStateWidget);

        if (pFromStateWidget != nullptr) {
            pFromStateWidget->setToStateWidget(pToStateWidget);
        }

        getStateDiagramWidget()->update();
    }

    template <typename TState>
    void StateMachineWindow::activateTransitionStates(
        const TransitionRecord &transitionRecord, AbstractStateWidget *&pFromStateWidget, AbstractStateWidget *&pToStateWidget) const {
        auto const pStateWidgets = getStateDiagramWidget()->findChildren<TState *>();

        for (auto &&pStateWidget : pStateWidgets) {
            pStateWidget->resetTransition();
            if (pStateWidget->getState() == transitionRecord.pFromState && pStateWidget->getState() == transitionRecord.pToState) {
                pStateWidget->activate(AbstractStateWidget::ActivationMode::FromState);
                pFromStateWidget = pStateWidget;
                pToStateWidget   = pStateWidget;
            } else if (pStateWidget->getState() == transitionRecord.pFromState) {
                pStateWidget->activate(transitionRecord.failure.empty() ? AbstractStateWidget::ActivationMode::FromState
                                                                        : AbstractStateWidget::ActivationMode::Failure);
                pFromStateWidget = pStateWidget;
            } else if (pStateWidget->getState() == transitionRecord.pCurrentState) {
                pStateWidget->activate(AbstractStateWidget::ActivationMode::CurrentState);
            } else if (pStateWidget->getState() == transitionRecord.pToState) {
                pStateWidget->activate(AbstractStateWidget::ActivationMode::ToState);
                pToStateWidget = pStateWidget;
            } else {
                pStateWidget->deactivate();
            }
        }
    }

} // namespace NS_OSBASE::statemachineviewer
