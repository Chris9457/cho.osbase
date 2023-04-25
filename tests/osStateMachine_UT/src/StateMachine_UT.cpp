// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include "osCore/Misc/Scope.h"
#include "osData/Log.h"
#include <gtest/gtest.h>
#include <optional>
#include <fstream>
#include <filesystem>

using namespace std::chrono_literals;

namespace NS_OSBASE::statemachine::ut {
    class StateMachine_UT : public testing::Test {
        class StateMachineDelegate : public StateMachine::ITransitionDelegate {
        public:
            StateMachineDelegate(StateMachine_UT &stateMachine_UT) : m_stateMachineUt(stateMachine_UT) {
            }

            void onTransitionSucceed(AbstractStatePtr pNewState) override {
                m_stateMachineUt.onTransitionSucceed(pNewState);
            }
            void onTransitionError(AbstractStatePtr pNewState, const std::string &strError) override {
                m_stateMachineUt.onTransitionError(pNewState, strError);
            }

        private:
            StateMachine_UT &m_stateMachineUt;
        };

    protected:
        void SetUp() override {
            Test::SetUp();

            m_pTaskLoop       = std::make_shared<application::TaskLoop>();
            m_futTaskLoopStop = m_pTaskLoop->runAsync();

            createStateChart();
            addTransition(s_root_start, s_s1, s_evt1);
            addTransition(s_s1_start, s_s1_1, s_evt2);
            addTransition(s_s1_1, s_s1_2, s_evt3);
            addTransition(s_s1_2, s_s1_end, s_evt4);
            auto const pTransitionWithoutGuard = addTransition(s_s1, s_s2_1, s_evt5);
            pTransitionWithoutGuard->setGuard(nullptr);
            addTransition(s_s2_1, s_s2_2, s_evt6);
            addTransition(s_s2_2, s_s1, s_evt7);
            addTransition(s_s1, "", s_evt7);
            addTransition(s_s1, s_s1, s_evt6);

            // Add a transition failure
            auto const pTransitionFailure = addTransition(s_s1, s_s2, s_evt1);
            pTransitionFailure->setAction([](const std::any &) { throw StateMachineException("failure"); });

            m_pStateMachine = StateMachine::create(getTaskLoop());
            m_pStateMachine->setName("Test State Machine");
            m_pStateMachine->setRootState(m_pRoot);

            m_pStateMachineDelegate = std::make_shared<StateMachineDelegate>(*this);
            m_pStateMachine->setDelegate(m_pStateMachineDelegate);

            auto const pLogOutputGroup = data::makeLogOutputGroup();
            pLogOutputGroup->add(data::makeLogOutputDebug());
            pLogOutputGroup->add(data::makeLogOutputFile(std::filesystem::current_path() / "statemachine.log"));
            data::TheLogger.setLogOutput(pLogOutputGroup);
            data::TheLogger.setChannelMask(data::TheLogger.getChannelMask() | (1ull << m_pStateMachine->getLogChannel()));
            data::TheLogger.setSeverityMask(data::TheLogger.getSeverityMask() | data::Severity::info);

            auto const pWriter = makePlantUMLStateWriter();
            m_pRoot->write(pWriter);

            std::ofstream ofs(std::filesystem::current_path() / "statediagram.uml");
            ofs << *pWriter;
        }

        void TearDown() override {
            if (m_pStateMachine->isStarted()) {
                m_pStateMachine->stop();
            }

            if (m_pTaskLoop->isRunning()) {
                m_pTaskLoop->stop();
                m_futTaskLoopStop.wait();
            }
        }

        StateMachinePtr getStateMachine() {
            return m_pStateMachine;
        }

        application::TaskLoopPtr getTaskLoop() const {
            return m_pTaskLoop;
        }

#define STATE_TIMEOUT 100ms

        std::optional<int> waitForStateAction(
            const std::string &stateName, const bool bEntry, const std::chrono::milliseconds &timeout = STATE_TIMEOUT) {
            auto const itStatePromise = m_mapStatePromises.find(stateName);
            if (itStatePromise == m_mapStatePromises.cend()) {
                return {};
            }

            auto const guard = core::make_scope_exit([&itStatePromise, &bEntry]() {
                if (bEntry) {
                    std::promise<int>().swap(itStatePromise->second.entry);
                } else {
                    std::promise<int>().swap(itStatePromise->second.exit);
                }
            });

            auto actionFut = bEntry ? itStatePromise->second.entry.get_future() : itStatePromise->second.exit.get_future();
            if (actionFut.wait_for(timeout) == std::future_status::timeout) {
                return {};
            }

            return actionFut.get();
        }

        std::optional<int> waitForTransitionAction(const std::string &evtName, const std::chrono::milliseconds &timeout = STATE_TIMEOUT) {
            auto const itTransitionPromise = m_mapTransitionPromises.find(evtName);
            if (itTransitionPromise == m_mapTransitionPromises.cend()) {
                return {};
            }

            auto const guard = core::make_scope_exit([&itTransitionPromise]() { std::promise<int>().swap(itTransitionPromise->second); });

            auto actionFut = itTransitionPromise->second.get_future();
            if (actionFut.wait_for(timeout) == std::future_status::timeout) {
                return {};
            }

            return actionFut.get();
        }

        std::optional<AbstractStatePtr> waitForTransition(const std::chrono::milliseconds &timeout = STATE_TIMEOUT) {
            auto const guard = core::make_scope_exit([this]() { std::promise<AbstractStatePtr>().swap(m_promiseNewState); });
            if (auto newStateFut = m_promiseNewState.get_future(); newStateFut.wait_for(timeout) == std::future_status::ready) {
                return newStateFut.get();
            }
            return {};
        }

        bool waitForTransitionError(const std::chrono::milliseconds &timeout = STATE_TIMEOUT) {
            auto const guard = core::make_scope_exit([this]() { std::promise<void>().swap(m_promiseError); });
            if (auto const errorFut = m_promiseError.get_future(); errorFut.wait_for(timeout) == std::future_status::ready) {
                return true;
            }
            return false;
        }

        AbstractStatePtr findState(const std::string &stateName) const {
            auto const itState = std::find_if(StateWalker<WalkType::recursive>(m_pRoot).begin(),
                StateWalker<WalkType::recursive>().end(),
                [&stateName](auto const pState) { return pState->getName() == stateName; });

            if (itState != StateWalker<WalkType::recursive>().end()) {
                return *itState;
            }

            return nullptr;
        }

        inline static const std::string s_root       = "root";
        inline static const std::string s_root_start = "root_start";

        inline static const std::string s_s1       = "S1";
        inline static const std::string s_s1_start = "S1_start";
        inline static const std::string s_s1_1     = "S1_1";
        inline static const std::string s_s1_2     = "S1_2";
        inline static const std::string s_s1_end   = "S1_end";

        inline static const std::string s_s2   = "S2";
        inline static const std::string s_s2_1 = "S2_1";
        inline static const std::string s_s2_2 = "S2_2";

        inline static constexpr int s_threshold = 5;

        inline static const std::string s_evt1 = "evt1";
        inline static const std::string s_evt2 = "evt2";
        inline static const std::string s_evt3 = "evt3";
        inline static const std::string s_evt4 = "evt4";
        inline static const std::string s_evt5 = "evt5";
        inline static const std::string s_evt6 = "evt6";
        inline static const std::string s_evt7 = "evt7";

    private:
        struct StatePromise {
            std::promise<int> entry;
            std::promise<int> exit;
        };

        void createStateChart() {
            // |--------------------------------------------------|
            // |                      root                        |
            // |                                                  |
            // |        O : root_start                            |
            // |                                                  |
            // | |-------------------|      |-------------------| |
            // | |        S1         |      |         S2        | |
            // | |               (H) |      |                   | |
            // | |                   |      |                   | |
            // | |        O : start  |      |                   | |
            // | |                   |      |                   | |
            // | | |---------------| |      | |---------------| | |
            // | | |     S1_1      | |      | |     S2_1      | | |
            // | | |---------------| |      | |---------------| | |
            // | |                   |      |                   | |
            // | | |---------------| |      | |---------------| | |
            // | | |     S1_2      | |      | |     S2_2      | | |
            // | | |---------------| |      | |---------------| | |
            // | |                   |      |                   | |
            // | |        (O) : end  |      |                   | |
            // | |-------------------|      |-------------------| |
            // |                                                  |
            // |--------------------------------------------------|

            m_pRoot = State::createRoot(s_root);
            m_pRoot->setStartState(s_root_start);

            auto const pS1       = createState(m_pRoot, s_s1);
            auto const pS1_H     = pS1->setHistoryState();
            auto const pS1_Start = pS1->setStartState(s_s1_start);
            auto const pS1_1     = createState(pS1, s_s1_1);
            auto const pS1_2     = createState(pS1, s_s1_2);
            auto const pS1_End   = pS1->addEndState(s_s1_end);

            auto const pS2   = createState(m_pRoot, s_s2);
            auto const pS2_1 = createState(pS2, s_s2_1);
            auto const pS2_2 = createState(pS2, s_s2_2);
        }

        StatePtr createState(StatePtr pParent, const std::string &name) {
            auto const pState = State::create(pParent, name);
            m_mapStatePromises.insert({ name, StatePromise{} });

            pState->setEntryAction([this, name](const std::any &value) { setPromiseState(name, std::any_cast<int>(value), true); });
            pState->setExitAction([this, name](const std::any &value) { setPromiseState(name, std::any_cast<int>(value), false); });
            return pState;
        }

        void setPromiseState(const std::string &name, const int value, const bool bEntry) {
            auto const itStatePromise = m_mapStatePromises.find(name);
            if (itStatePromise == m_mapStatePromises.cend()) {
                return;
            }

            if (bEntry) {
                itStatePromise->second.entry.set_value(value);
            } else {
                itStatePromise->second.exit.set_value(value);
            }
        }

        TransitionPtr addTransition(
            const std::string &fromStateName, const std::string &toStateName, const std::string &evtName, const bool bAddGuard = true) {
            auto const itFromState = std::find_if(StateWalker<WalkType::recursive>(m_pRoot).begin(),
                StateWalker<WalkType::recursive>().end(),
                [&fromStateName](auto const pState) { return pState->getName() == fromStateName; });

            if (itFromState == StateWalker<WalkType::recursive>().end()) {
                return nullptr;
            }

            auto const itToState = std::find_if(StateWalker<WalkType::recursive>(m_pRoot).begin(),
                StateWalker<WalkType::recursive>().end(),
                [&toStateName](auto const pState) { return pState->getName() == toStateName; });
            auto const pToState  = itToState == StateWalker<WalkType::recursive>().end() ? nullptr : *itToState;

            if (itFromState == StateWalker<WalkType::recursive>().end()) {
                return nullptr;
            }

            auto pTransition =
                pToState != nullptr ? itFromState->addTransition(pToState, evtName) : itFromState->addInternalTransition(evtName);
            m_mapTransitionPromises.insert({ evtName, std::promise<int>{} });
            if (bAddGuard) {
                pTransition->setGuard([](const std::any &value) {
                    if (value.has_value()) {
                        return std::any_cast<int>(value) < s_threshold;
                    }

                    return true;
                });
            }
            pTransition->setAction([this, evtName](const std::any &value) {
                if (!value.has_value()) {
                    return;
                }

                m_mapTransitionPromises[evtName].set_value(std::any_cast<int>(value));
            });

            return pTransition;
        }

        void onTransitionSucceed(AbstractStatePtr pNewState) {
            m_promiseNewState.set_value(pNewState);
        }

        void onTransitionError(AbstractStatePtr pNewState, const std::string &strError) {
            std::cout << "Transition failure - " << strError << std::endl;
            m_promiseError.set_value();
        }

        application::TaskLoopPtr m_pTaskLoop;
        std::shared_future<void> m_futTaskLoopStop;
        StatePtr m_pRoot;
        std::unordered_map<std::string, StatePromise> m_mapStatePromises;
        std::unordered_map<std::string, std::promise<int>> m_mapTransitionPromises;
        StateMachinePtr m_pStateMachine;
        StateMachine::ITransitionDelegatePtr m_pStateMachineDelegate;
        std::promise<AbstractStatePtr> m_promiseNewState;
        std::promise<void> m_promiseError;
    };

    TEST_F(StateMachine_UT, create_OK) {
        auto pSM = StateMachine::create(getTaskLoop());
        ASSERT_NE(nullptr, pSM);
    }

    TEST_F(StateMachine_UT, create_KO) {
        ASSERT_THROW(StateMachine::create(nullptr), StateMachineException);
    }

    TEST_F(StateMachine_UT, setRoot_OK) {
        auto pSM = StateMachine::create(getTaskLoop());

        auto const pRoot = State::createRoot("root");
        ASSERT_EQ(pRoot, pSM->setRootState(pRoot));
    }

    TEST_F(StateMachine_UT, setRoot_KO) {
        auto pSM = StateMachine::create(getTaskLoop());
        ASSERT_THROW(pSM->setRootState(StatePtr()), StateMachineException);
    }

    TEST_F(StateMachine_UT, start_OK) {
        auto const pTaskLoop = std::make_shared<application::TaskLoop>();
        auto pSM             = StateMachine::create(getTaskLoop());

        pSM->setRootState(State::createRoot("root"));
        ASSERT_NO_THROW(pSM->start());
    }

    TEST_F(StateMachine_UT, start_KO) {
        auto pSM = StateMachine::create(getTaskLoop());

        ASSERT_THROW(pSM->start(), StateMachineException); // no root state assigned

        getTaskLoop()->stop();
        pSM->setRootState(State::createRoot("root"));

        ASSERT_THROW(pSM->start(), StateMachineException); // taskloop not running
    }

    TEST_F(StateMachine_UT, stop_OK) {
        auto pSM = StateMachine::create(getTaskLoop());
        pSM->setRootState(State::createRoot("root"));
        pSM->start();
        ASSERT_NO_THROW(pSM->stop());
    }

    TEST_F(StateMachine_UT, stop_KO) {
        auto pSM = StateMachine::create(getTaskLoop());
        ASSERT_THROW(pSM->stop(), StateMachineException);
    }

    TEST_F(StateMachine_UT, anime) {
        auto const pSM = getStateMachine();

        getStateMachine()->start();
        ASSERT_EQ(findState(s_root_start), waitForTransition());

        auto const guard = core::make_scope_exit([&pSM]() { pSM->stop(); });

        // check guard (done once)
        pSM->raiseEvent(s_evt1, s_threshold + 1);
        ASSERT_EQ(findState(s_root_start), waitForTransition());
        ASSERT_FALSE(waitForStateAction(s_s1, true).has_value());

        // root -evt1-> S1
        constexpr int value = s_threshold - 1;
        pSM->raiseEvent(s_evt1, value);
        ASSERT_EQ(value, waitForTransitionAction(s_evt1));
        ASSERT_EQ(value, waitForStateAction(s_s1, true));
        ASSERT_EQ(findState(s_s1_start), waitForTransition());

        // S1_start -evt2-> S1_1
        pSM->raiseEvent(s_evt2, value);
        ASSERT_EQ(value, waitForTransitionAction(s_evt2));
        ASSERT_EQ(value, waitForStateAction(s_s1_1, true));
        ASSERT_EQ(findState(s_s1_1), waitForTransition());

        // S1_1 -evt3-> S1_2
        pSM->raiseEvent(s_evt3, value);
        ASSERT_EQ(value, waitForStateAction(s_s1_1, false));
        ASSERT_EQ(value, waitForTransitionAction(s_evt3));
        ASSERT_EQ(value, waitForStateAction(s_s1_2, true));
        ASSERT_EQ(findState(s_s1_2), waitForTransition());

        // S1_1_2 -evt5-> S2_1
        pSM->raiseEvent(s_evt5, value);
        ASSERT_FALSE(waitForStateAction(s_s1_2, true).has_value());
        ASSERT_EQ(value, waitForStateAction(s_s1, false));
        ASSERT_EQ(value, waitForTransitionAction(s_evt5));
        ASSERT_EQ(value, waitForStateAction(s_s2, true));
        ASSERT_EQ(value, waitForStateAction(s_s2_1, true));
        ASSERT_EQ(findState(s_s2_1), waitForTransition());

        // S2_1 -evt6-> S2_2
        pSM->raiseEvent(s_evt6, value);
        ASSERT_EQ(value, waitForStateAction(s_s2_1, false));
        ASSERT_EQ(value, waitForTransitionAction(s_evt6));
        ASSERT_EQ(value, waitForStateAction(s_s2_2, true));
        ASSERT_EQ(findState(s_s2_2), waitForTransition());

        // S2_2 -evt7-> S1
        pSM->raiseEvent(s_evt7, value);
        ASSERT_EQ(value, waitForStateAction(s_s2_2, false));
        ASSERT_EQ(value, waitForStateAction(s_s2, false));
        ASSERT_EQ(value, waitForTransitionAction(s_evt7));
        ASSERT_EQ(value, waitForStateAction(s_s1, true));
        ASSERT_EQ(findState(s_s1_2), waitForTransition());

        // S1_2 -evt4-> S1_end
        pSM->raiseEvent(s_evt4, value);
        ASSERT_EQ(value, waitForStateAction(s_s1_2, false));
        ASSERT_EQ(value, waitForTransitionAction(s_evt4));
        ASSERT_EQ(findState(s_s1), waitForTransition());

        // S1 -evt7-> S1 (internal)
        pSM->raiseEvent(s_evt7, value);
        ASSERT_EQ(value, waitForTransitionAction(s_evt7));
        ASSERT_FALSE(waitForStateAction(s_s1, false).has_value());
        ASSERT_FALSE(waitForStateAction(s_s1, true).has_value());
        ASSERT_EQ(findState(s_s1), waitForTransition());

        // S1 -evt1-> S1 (failure, throw exception
        pSM->raiseEvent(s_evt1, value);
        ASSERT_TRUE(waitForTransitionError());
        ASSERT_EQ(value, waitForStateAction(s_s1, false));
        ASSERT_FALSE(waitForStateAction(s_s1, true).has_value());

        // re-try a internal transition after failure
        // S1 -evt7-> S1 (internal)
        pSM->raiseEvent(s_evt7, value);
        ASSERT_EQ(value, waitForTransitionAction(s_evt7));
        ASSERT_FALSE(waitForStateAction(s_s1, false).has_value());
        ASSERT_FALSE(waitForStateAction(s_s1, true).has_value());
        ASSERT_EQ(findState(s_s1), waitForTransition());

        // re-try a recursive transition after failure
        // S1 -evt7-> S1 (internal)
        pSM->raiseEvent(s_evt6, value);
        ASSERT_EQ(value, waitForStateAction(s_s1, false));
        ASSERT_EQ(value, waitForTransitionAction(s_evt6));
        ASSERT_EQ(value, waitForStateAction(s_s1, true));
        ASSERT_EQ(findState(s_s1_start), waitForTransition());
    }

} // namespace NS_OSBASE::statemachine::ut
