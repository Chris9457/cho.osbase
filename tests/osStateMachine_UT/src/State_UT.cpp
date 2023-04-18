// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "osStateMachine/AbstractState.h"

namespace cho::osbase::statemachine::ut {

    class State_UT : public testing::Test {
    protected:
        void SetUp() override {
            Test::SetUp();

            createStateChart();
            addT1();
            addT2();
            addT3();
            addT4();
            addT5();
            addT6_T7();
        }

        void createStateChart() {
            // root
            //  - S1
            //  - S2
            //      - S2_1
            //          - H
            //          - S2_1_1
            //          - S2_1_2
            //      - S2_2
            //  - S3
            //      - S3_start
            //      - S3_1
            //      - S3_end
            m_pRoot                  = State::createRoot(s_root);
            auto const pS1           = State::create(m_pRoot, s_s1);
            auto const pS2           = State::create(m_pRoot, s_s2);
            auto const pS3           = State::create(m_pRoot, s_s3);
            auto const pS2_1         = State::create(pS2, s_s2_1);
            auto const pS2_2         = State::create(pS2, s_s2_2);
            auto const pS2_1_History = pS2_1->setHistoryState();
            auto const pS2_1_1       = State::create(pS2_1, s_s2_1_1);
            auto const pS2_1_2       = State::create(pS2_1, s_s2_1_2);
            auto const pStartState   = pS3->setStartState(s_s3_start);
            auto const pS3_1         = State::create(pS3, s_s3_1);
            auto const pEndState     = pS3->addEndState(s_s3_end);

            auto exitS1 = [this](const std::any &value) { v_S1 += std::any_cast<int>(value); };
            pS1->setExitAction(exitS1);

            auto entryS2 = [this](const std::any &value) {
                if (value.has_value()) {
                    v_S2 += std::any_cast<int>(value);
                }
            };
            auto exitS2 = [this](const std::any &value) {
                if (value.has_value()) {
                    v_S2 += std::any_cast<int>(value);
                }
            };
            pS2->setEntryAction(entryS2);
            pS2->setExitAction(exitS2);

            auto exitS2_1_1 = [this](const std::any &value) {
                if (value.has_value()) {
                    v_S2_1_1 += std::any_cast<int>(value);
                }
            };
            pS2_1_1->setExitAction(exitS2_1_1);

            auto exitS2_1 = [this](const std::any &value) {
                if (value.has_value()) {
                    v_S2_1 += std::any_cast<int>(value);
                }
            };
            pS2_1->setExitAction(exitS2_1);

            auto entryS3 = [this](const std::any &value) { v_S3 += std::any_cast<int>(value); };
            pS3->setEntryAction(entryS3);

            auto entryS3_1 = [this](const std::any &value) {
                if (value.has_value()) {
                    v_S3_1 += std::any_cast<int>(value);
                }
            };
            auto exitS3_1 = [this](const std::any &value) { v_S3_1 += std::any_cast<int>(value); };
            pS3_1->setEntryAction(entryS3_1);
            pS3_1->setExitAction(exitS3_1);

            pStartState->addImplicitTransition(pS3_1);
        }

        void addT1() {
            // T1: S1 => S2
            auto const pS1 = findAbstractState(s_s1);
            auto const pS2 = findAbstractState(s_s2);
            auto guard     = [](const std::any &value) { return std::any_cast<int>(value) < 2; };
            auto actionT   = [this](const std::any &value) { v_T1 += std::any_cast<int>(value); };

            auto const pTransition = pS1->addTransition(pS2, evt1);
            pTransition->setGuard(guard);
            pTransition->setAction(actionT);
            pTransition->setName("T1");
        }

        void addT2() {
            // T2: S2_1_1 => S3_1
            auto const pS2_1_1 = findAbstractState(s_s2_1_1);
            auto const pS3_1   = findAbstractState(s_s3_1);
            auto guard         = [](const std::any &value) { return std::any_cast<int>(value) < 2; };
            auto actionT       = [this](const std::any &value) { v_T2 += std::any_cast<int>(value); };

            auto const pTransition = pS2_1_1->addTransition(pS3_1, evt2);
            pTransition->setGuard(guard);
            pTransition->setAction(actionT);
        }

        void addT3() {
            // T3: S2_1 => S3
            auto const pS2_1 = findAbstractState(s_s2_1);
            auto const pS3   = findAbstractState(s_s3);
            auto guard       = [](const std::any &value) { return std::any_cast<int>(value) < 2; };
            auto actionT     = [this](const std::any &value) { v_T3 += std::any_cast<int>(value); };

            auto const pTransition = pS2_1->addTransition(pS3, evt3);
            pTransition->setGuard(guard);
            pTransition->setAction(actionT);
        }

        void addT4() {
            // T4: S3_1 => S3_end
            auto const pS3_1   = findAbstractState(s_s3_1);
            auto const pS3_end = findAbstractState(s_s3_end);
            auto guard         = [](const std::any &value) { return std::any_cast<int>(value) < 2; };
            auto actionT       = [this](const std::any &value) { v_T4 += std::any_cast<int>(value); };

            auto const pTransition = pS3_1->addTransition(pS3_end, evt4);
            pTransition->setGuard(guard);
            pTransition->setAction(actionT);
        }

        void addT5() {
            // T5: internal S2
            auto const pS2 = findAbstractState(s_s2);
            auto guard     = [](const std::any &value) { return std::any_cast<int>(value) < 2; };
            auto actionT   = [this](const std::any &value) { v_T5 += std::any_cast<int>(value); };

            auto const pTransition = pS2->addInternalTransition(evt5);
            pTransition->setGuard(guard);
            pTransition->setAction(actionT);
        }

        void addT6_T7() {
            {
                // T6: S2_1_1 => S2_1_2
                auto const pS2_1_1     = findAbstractState(s_s2_1_1);
                auto const pS2_1_2     = findAbstractState(s_s2_1_2);
                auto const pTransition = pS2_1_1->addTransition(pS2_1_2, evt6);
            }
            {
                // T7: S2_1 => S2_1
                auto const pS2_1       = findAbstractState(s_s2_1);
                auto const pTransition = pS2_1->addTransition(pS2_1, evt7);
            }
        }

        StatePtr findState(const std::string &name) {
            return std::dynamic_pointer_cast<State>(findAbstractState(name));
        }

        AbstractStatePtr findAbstractState(const std::string &name) {
            auto const itState = std::find_if(StateWalker<WalkType::recursive>(m_pRoot).begin(),
                StateWalker<WalkType::recursive>(m_pRoot).end(),
                [&name](auto const pState) { return pState->getName() == name; });

            if (itState != StateWalker<WalkType::recursive>(m_pRoot).end()) {
                return (*itState);
            }

            return nullptr;
        }

        inline static const std::string s_root     = "root";
        inline static const std::string s_s1       = "S1";
        inline static const std::string s_s2       = "S2";
        inline static const std::string s_s3       = "S3";
        inline static const std::string s_s2_1     = "S2_1";
        inline static const std::string s_s2_2     = "S2_2";
        inline static const std::string s_s2_1_1   = "S2_1_1";
        inline static const std::string s_s2_1_2   = "S2_1_2";
        inline static const std::string s_s3_start = "S3_start";
        inline static const std::string s_s3_1     = "S3_1";
        inline static const std::string s_s3_end   = "S3_end";

        inline static const std::string evt1 = "evt1";
        inline static const std::string evt2 = "evt2";
        inline static const std::string evt3 = "evt3";
        inline static const std::string evt4 = "evt4";
        inline static const std::string evt5 = "evt5";
        inline static const std::string evt6 = "evt6";
        inline static const std::string evt7 = "evt7";

        int v_root   = 0;
        int v_S1     = 0;
        int v_S2     = 0;
        int v_S3     = 0;
        int v_S2_1   = 0;
        int v_S2_2   = 0;
        int v_S2_1_1 = 0;
        int v_S2_1_2 = 0;
        int v_S3_1   = 0;

        int v_T1 = 0;
        int v_T2 = 0;
        int v_T3 = 0;
        int v_T4 = 0;
        int v_T5 = 0;

    private:
        StatePtr m_pRoot;
    };

    class StateDelegate : public AbstractState::ITransitionDelegate {
    public:
        void onEntryState(StatePtr pState) override {
            if (m_transitions.empty()) {
                return;
            }
            m_transitions.back().entities.emplace_back(pState);
        }

        void onExitState(StatePtr pState) override {
            if (m_transitions.empty()) {
                return;
            }
            m_transitions.back().entities.emplace_back(pState);
        }

        void onActionTransition(TransitionPtr pTransition) override {
            if (m_transitions.empty()) {
                return;
            }
            m_transitions.back().entities.emplace_back(pTransition);
        }

        auto getTransitions() const {
            return m_transitions;
        }

        void reset() {
            m_transitions.clear();
        }

        void onBeginTransition(EventPtr pEvent, AbstractStatePtr, TransitionPtr) override {
            m_transitions.emplace_back();
            m_transitions.back().pEvent = pEvent;
        }

        void onEndTransition(AbstractStatePtr) override {
        }

    private:
        struct Transition {
            std::vector<std::any> entities;
            EventPtr pEvent;
        };
        std::vector<Transition> m_transitions;
    };

    TEST_F(State_UT, createRoot) {
        const std::string rootName = "root";
        auto const pRoot           = State::createRoot(rootName);
        ASSERT_NE(nullptr, pRoot);
        ASSERT_EQ(rootName, pRoot->getName());
    }

    TEST_F(State_UT, create) {
        const std::string rootName = "root";
        const std::string name     = "s";
        auto const pRootState      = State::createRoot(rootName);
        auto const pState          = State::create(pRootState, name);
        ASSERT_NE(nullptr, pState);
        ASSERT_EQ(name, pState->getName());
        ASSERT_EQ(pRootState, pState->getParent());
    }

    TEST_F(State_UT, getFullName) {
        {
            // default separator
            auto const expectedFullName =
                STATE_SEPARATOR + s_root + STATE_SEPARATOR + s_s2 + STATE_SEPARATOR + s_s2_1 + STATE_SEPARATOR + s_s2_1_1;
            ASSERT_EQ(expectedFullName, findState(s_s2_1_1)->getFullName());
        }

        {
            // custom separator
            constexpr char CUSTOM_SEP[] = "|";
            auto const expectedFullName = CUSTOM_SEP + s_root + CUSTOM_SEP + s_s2 + CUSTOM_SEP + s_s2_1 + CUSTOM_SEP + s_s2_1_1;
            ASSERT_EQ(expectedFullName, findState(s_s2_1_1)->getFullName(CUSTOM_SEP));
        }
    }

    TEST_F(State_UT, getDepth) {
        const std::string rootName = "root";
        const std::string name     = "s";
        auto const pRootState      = State::createRoot(rootName);
        ASSERT_EQ(0, pRootState->getDepth());

        auto pState = State::create(pRootState, name);
        ASSERT_EQ(1, pState->getDepth());

        pState = State::create(pState, name);
        ASSERT_EQ(2, pState->getDepth());

        pState = State::create(pState, name);
        ASSERT_EQ(3, pState->getDepth());
    }

    TEST_F(State_UT, isState) {
        auto const pRoot = findState(s_root);
        ASSERT_TRUE(pRoot->isState());
        ASSERT_FALSE(pRoot->isStartState());
        ASSERT_FALSE(pRoot->isEndState());
        ASSERT_FALSE(pRoot->isHistoryState());
    }

    TEST_F(State_UT, iterator) {
        const std::string rootName = "root";
        const std::string name     = "s";
        auto const pRootState      = State::createRoot(rootName);
        for (size_t i = 0; i < 10; ++i) {
            auto pState = State::create(pRootState, name + std::to_string(i));
        }

        size_t index = 0;
        for (State::iterator it(pRootState), itEnd; it != itEnd; ++it) {
            ASSERT_EQ(name + std::to_string(index), (*it)->getName());
            ++index;
        }
    }

    TEST_F(State_UT, prefix_iterator) {
        auto it = State::prefix_iterator(findState(s_root));
        ASSERT_EQ(findAbstractState(s_root), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s1), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s2), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s2_1), *it);
        ++it;
        ASSERT_TRUE((*it)->isHistoryState());
        ++it;
        ASSERT_EQ(findAbstractState(s_s2_1_1), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s2_1_2), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s2_2), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s3), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s3_start), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s3_1), *it);
        ++it;
        ASSERT_EQ(findAbstractState(s_s3_end), *it);
        ++it;
        ASSERT_FALSE(it != State::prefix_iterator());
    }

    TEST_F(State_UT, prefix_iterator_root_only) {
        auto const pRoot = State::createRoot("root");
        auto it          = State::prefix_iterator(pRoot);

        ASSERT_TRUE(it != State::prefix_iterator());
        ASSERT_EQ(pRoot, *it);
        ++it;
        ASSERT_FALSE(it != State::prefix_iterator());
    }

    TEST_F(State_UT, for_range_loop_direct) {
        size_t index  = 0;
        std::vector v = { s_s1, s_s2, s_s3 };
        for (auto &&pState : StateWalker<WalkType::direct>(findState(s_root))) {
            ASSERT_EQ(v[index], pState->getName());
            ++index;
        }
    }

    TEST_F(State_UT, for_range_loop_recursive) {
        size_t index  = 0;
        std::vector v = { s_root, s_s1, s_s2, s_s2_1, std::string(), s_s2_1_1, s_s2_1_2, s_s2_2, s_s3, s_s3_start, s_s3_1, s_s3_end };
        for (auto &&pState : StateWalker<WalkType::recursive>(findState(s_root))) {
            if (index == 4) {
                ASSERT_TRUE(pState->isHistoryState());
            } else {
                ASSERT_EQ(v[index], pState->getName());
            }
            ++index;
        }
    }

    TEST_F(State_UT, entryAction) {
        double value     = 1.;
        auto entryAction = [&value](const std::any &arg) {
            auto const v = std::any_cast<double>(arg);
            value += v;
        };

        auto const pS1 = findState(s_s1);
        pS1->doEntry(3.);
        ASSERT_EQ(1., value);

        pS1->setEntryAction(entryAction);
        pS1->doEntry(3.);
        ASSERT_EQ(4., value);
    }

    TEST_F(State_UT, exitAction) {
        int value       = 1;
        auto exitAction = [&value](const std::any &arg) {
            auto const v = std::any_cast<int>(arg);
            value += v;
        };

        auto const pS1 = findState(s_s1);
        pS1->doExit(3);
        ASSERT_EQ(1, value);

        pS1->setExitAction(exitAction);

        pS1->doExit(3);
        ASSERT_EQ(4, value);
    }

    TEST_F(State_UT, startState) {
        auto const pS1 = findState(s_s1);
        ASSERT_FALSE(pS1->hasStartState());

        auto const pStartState1 = pS1->setStartState("start1");
        ASSERT_TRUE(pS1->hasStartState());

        ASSERT_NE(nullptr, pStartState1);
        auto const pStartState2 = pS1->setStartState("start2");
        ASSERT_NE(pStartState1, pStartState2);
        ASSERT_TRUE(pS1->hasStartState());
    }

    TEST_F(State_UT, historyState) {
        auto const pS1 = findState(s_s1);
        ASSERT_FALSE(pS1->hasHistoryState());

        auto const pHistoryState1 = pS1->setHistoryState();
        ASSERT_TRUE(pS1->hasHistoryState());

        ASSERT_NE(nullptr, pHistoryState1);
        auto const pHistoryState2 = pS1->setHistoryState();
        ASSERT_NE(pHistoryState1, pHistoryState2);
        ASSERT_TRUE(pS1->hasHistoryState());
    }

    TEST_F(State_UT, endState) {
        auto const pS1       = findState(s_s1);
        auto const pEndState = pS1->addEndState("end1");

        ASSERT_NE(nullptr, pEndState);
    }

    TEST_F(State_UT, addTransition_same_level) {
        auto const pS1 = findState(s_s1);
        auto const pS2 = findState(s_s2);

        ASSERT_EQ(0, v_S1);
        ASSERT_EQ(0, v_T1);
        ASSERT_EQ(0, v_S2);

        // Guard ok
        auto pToState = pS1->doTransition(Event::create(evt1, 1));
        ASSERT_EQ(pS2, pToState);
        ASSERT_EQ(1, v_S1);
        ASSERT_EQ(1, v_T1);
        ASSERT_EQ(1, v_S2);

        // Guard ko
        pToState = pS1->doTransition(Event::create(evt1, 2));
        ASSERT_EQ(nullptr, pToState);
        ASSERT_EQ(1, v_S1);
        ASSERT_EQ(1, v_T1);
        ASSERT_EQ(1, v_S2);
    }

    TEST_F(State_UT, addTransition_different_level) {
        auto const pS2_1_1 = findState(s_s2_1_1);
        auto const pS3_1   = findState(s_s3_1);

        ASSERT_EQ(0, v_S2_1_1);
        ASSERT_EQ(0, v_S2_1);
        ASSERT_EQ(0, v_S2);
        ASSERT_EQ(0, v_T2);
        ASSERT_EQ(0, v_S3);
        ASSERT_EQ(0, v_S3_1);

        // Guard ok
        auto pToState = pS2_1_1->doTransition(Event::create(evt2, 1));
        ASSERT_EQ(pS3_1, pToState);
        ASSERT_EQ(1, v_S2_1_1);
        ASSERT_EQ(1, v_S2_1);
        ASSERT_EQ(1, v_S2);
        ASSERT_EQ(1, v_T2);
        ASSERT_EQ(1, v_S3);
        ASSERT_EQ(1, v_S3_1);

        // Guard ko
        pToState = pS2_1_1->doTransition(Event::create(evt2, 2));
        ASSERT_EQ(nullptr, pToState);
        ASSERT_EQ(1, v_S2_1_1);
        ASSERT_EQ(1, v_S2_1);
        ASSERT_EQ(1, v_S2);
        ASSERT_EQ(1, v_T2);
        ASSERT_EQ(1, v_S3);
        ASSERT_EQ(1, v_S3_1);
    }

    TEST_F(State_UT, addImplicitTransition) {
        auto const pS2_1_1 = findState(s_s2_1_1);
        auto const pS3_1   = findState(s_s3_1);

        ASSERT_EQ(0, v_S2_1_1);
        ASSERT_EQ(0, v_S2_1);
        ASSERT_EQ(0, v_S2);
        ASSERT_EQ(0, v_T3);
        ASSERT_EQ(0, v_S3);
        ASSERT_EQ(0, v_S3_1);

        // Guard ok
        auto pToState = pS2_1_1->doTransition(Event::create(evt3, 1));
        ASSERT_EQ(pS3_1, pToState);
        ASSERT_EQ(0, v_S2_1_1);
        ASSERT_EQ(1, v_S2_1);
        ASSERT_EQ(1, v_S2);
        ASSERT_EQ(1, v_T3);
        ASSERT_EQ(1, v_S3);
        ASSERT_EQ(1, v_S3_1);

        // Guard ko
        pToState = pS2_1_1->doTransition(Event::create(evt3, 2));
        ASSERT_EQ(nullptr, pToState);
        ASSERT_EQ(0, v_S2_1_1);
        ASSERT_EQ(1, v_S2_1);
        ASSERT_EQ(1, v_S2);
        ASSERT_EQ(1, v_T3);
        ASSERT_EQ(1, v_S3);
        ASSERT_EQ(1, v_S3_1);
    }

    TEST_F(State_UT, addTransition_end) {
        auto const pS3_1 = findAbstractState(s_s3_1);
        auto const pS3   = findAbstractState(s_s3);

        ASSERT_EQ(0, v_S3_1);
        ASSERT_EQ(0, v_T4);
        ASSERT_EQ(0, v_S3);

        // Guard ok
        auto pToState = pS3_1->doTransition(Event::create(evt4, 1));
        ASSERT_EQ(pS3, pToState);
        ASSERT_EQ(1, v_S3_1);
        ASSERT_EQ(1, v_T4);
        ASSERT_EQ(0, v_S3);

        // Guard ko
        pToState = pS3_1->doTransition(Event::create(evt4, 2));
        ASSERT_EQ(nullptr, pToState);
        ASSERT_EQ(1, v_S3_1);
        ASSERT_EQ(1, v_T4);
        ASSERT_EQ(0, v_S3);
    }

    TEST_F(State_UT, addInternalTransition_end) {
        auto const pS2_1_1 = findAbstractState(s_s2_1_1);

        ASSERT_EQ(0, v_S2_1_1);
        ASSERT_EQ(0, v_S2_1);
        ASSERT_EQ(0, v_S2);
        ASSERT_EQ(0, v_T5);

        // Guard ok
        auto pToState = pS2_1_1->doTransition(Event::create(evt5, 1));
        ASSERT_EQ(nullptr, pToState); // no change
        ASSERT_EQ(0, v_S2_1_1);
        ASSERT_EQ(0, v_S2_1);
        ASSERT_EQ(0, v_S2);
        ASSERT_EQ(1, v_T5); // action of transition done

        // Guard ko
        pToState = pS2_1_1->doTransition(Event::create(evt5, 2));
        ASSERT_EQ(nullptr, pToState);
        ASSERT_EQ(0, v_S2_1_1);
        ASSERT_EQ(0, v_S2_1);
        ASSERT_EQ(0, v_S2);
        ASSERT_EQ(1, v_T5); // action of transition done
    }

    TEST_F(State_UT, addTransition_history) {
        auto const pS2_1_1 = findAbstractState(s_s2_1_1);
        auto const pS2_1_2 = findAbstractState(s_s2_1_2);
        auto const pS2_1   = findAbstractState(s_s2_1);

        auto pToState = pS2_1_1->doTransition(Event::create(evt6));
        ASSERT_EQ(pS2_1_2, pToState); // no change

        pToState = pS2_1->doTransition(Event::create(evt7));
        ASSERT_EQ(pS2_1_2, pToState);
    }

    TEST_F(State_UT, write_OK) {
        auto const pStateWriter = makePlantUMLStateWriter();

        ASSERT_NO_THROW(findState(s_root)->write(pStateWriter));
        std::ofstream ofs(std::filesystem::current_path() / "State_UT.uml");
        ofs << *pStateWriter;
    }

    TEST_F(State_UT, write_KO) {
        ASSERT_THROW(findState(s_root)->write(nullptr), StateMachineException);
    }

    TEST_F(State_UT, setDelegate) {
        auto const pDelegate = std::make_shared<StateDelegate>();
        auto const pRoot     = findState(s_root);
        pRoot->setTransitionDelegate(pDelegate);
        for (auto &&pState : StateWalker<WalkType::recursive>(pRoot)) {
            ASSERT_EQ(pDelegate, pState->getDelegate());
            for (auto &&[evtName, pTransition] : pState->getTransitions()) {
                ASSERT_EQ(pDelegate, pTransition->getDelegate());
            }
        }
    }

    TEST_F(State_UT, checkDelegate) {
        auto const pDelegate = std::make_shared<StateDelegate>();
        findState(s_root)->setTransitionDelegate(pDelegate);

        // S2_1_1 -evt2-> S3_1
        auto pEvent = Event::create(evt2, 1);
        findState(s_s2_1_1)->doTransition(pEvent);
        auto transitions = pDelegate->getTransitions();
        ASSERT_EQ(1, transitions.size());
        ASSERT_EQ(pEvent, transitions[0].pEvent);
        ASSERT_EQ(6, transitions[0].entities.size());
        ASSERT_EQ(findState(s_s2_1_1), std::any_cast<StatePtr>(transitions[0].entities[0]));
        ASSERT_EQ(findState(s_s2_1), std::any_cast<StatePtr>(transitions[0].entities[1]));
        ASSERT_EQ(findState(s_s2), std::any_cast<StatePtr>(transitions[0].entities[2]));

        ASSERT_EQ(typeid(TransitionPtr), transitions[0].entities[3].type());

        ASSERT_EQ(findState(s_s3), std::any_cast<StatePtr>(transitions[0].entities[4]));
        ASSERT_EQ(findState(s_s3_1), std::any_cast<StatePtr>(transitions[0].entities[5]));

        pDelegate->reset();

        // S2_1 -evt3-> S3
        pEvent = Event::create(evt3, 1);
        findState(s_s2_1)->doTransition(pEvent);
        transitions = pDelegate->getTransitions();
        ASSERT_EQ(3, transitions.size());
        ASSERT_EQ(pEvent, transitions[0].pEvent);
        ASSERT_EQ(4, transitions[0].entities.size());
        ASSERT_EQ(findState(s_s2_1), std::any_cast<StatePtr>(transitions[0].entities[0]));
        ASSERT_EQ(findState(s_s2), std::any_cast<StatePtr>(transitions[0].entities[1]));

        ASSERT_EQ(typeid(TransitionPtr), transitions[0].entities[2].type());

        ASSERT_EQ(findState(s_s3), std::any_cast<StatePtr>(transitions[0].entities[3]));

        ASSERT_EQ(0, transitions[1].entities.size());

        ASSERT_EQ(1, transitions[2].entities.size());
        ASSERT_NE(nullptr, transitions[2].pEvent);
        ASSERT_EQ(findState(s_s3_1), std::any_cast<StatePtr>(transitions[2].entities[0]));
    }

} // namespace cho::osbase::statemachine::ut
