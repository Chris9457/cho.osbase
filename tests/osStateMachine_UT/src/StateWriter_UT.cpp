// \brief Implementation tests of StateMachine
#include "osStateMachine/osStateMachine.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace cho::osbase::statemachine::ut {
    class StateWriter_UT : public testing::Test {
    protected:
        void SetUp() override {
            Test::SetUp();

            createStateChart();
            addTransition(s_root_start, s_s1, s_evt1);
            addTransition(s_s1_start, s_s1_1, s_evt2);
            addTransition(s_s1_1, s_s1_2, s_evt3);
            addTransition(s_s1_2, s_s1_end, s_evt4);
            addTransition(s_s1, s_s2_1, s_evt5);
            addTransition(s_s2_1, s_s2_2, s_evt6);
            addTransition(s_s2_2, s_s1, s_evt7);
        }

        StatePtr getRoot() const {
            return m_pRoot;
        }

    private:
        void createStateChart() {
            // |--------------------------------------------------|
            // |                      root                        |
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
            m_pRoot->setHistoryState();

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

            pState->setEntryAction([this, name](const std::any &) {});
            pState->setExitAction([this, name](const std::any &) {});
            return pState;
        }

        TransitionPtr addTransition(const std::string &fromStateName, const std::string &toStateName, const std::string &evtName) {
            auto const itFromState = std::find_if(StateWalker<WalkType::recursive>(m_pRoot).begin(),
                StateWalker<WalkType::recursive>().end(),
                [&fromStateName](auto const pState) { return pState->getName() == fromStateName; });

            if (itFromState == StateWalker<WalkType::recursive>().end()) {
                return nullptr;
            }

            auto const itToState = std::find_if(StateWalker<WalkType::recursive>(m_pRoot).begin(),
                StateWalker<WalkType::recursive>().end(),
                [&toStateName](auto const pState) { return pState->getName() == toStateName; });

            if (itFromState == StateWalker<WalkType::recursive>().end()) {
                return nullptr;
            }

            auto pTransition = itFromState->addTransition(*itToState, evtName);
            pTransition->setGuard([](const std::any &) { return true; });
            pTransition->setAction([this](const std::any &) {});

            return pTransition;
        }

        StatePtr m_pRoot;
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
    };

    TEST_F(StateWriter_UT, create) {
        auto const pWriter = makePlantUMLStateWriter();
        ASSERT_NE(nullptr, pWriter);
    }

    TEST_F(StateWriter_UT, writeState) {
        auto const pWriter = makePlantUMLStateWriter();
        pWriter->writeState(getRoot());

        // std::filesystem::path outputFile = std::filesystem::current_path() / "states.uml";
        // std::ofstream ofs(outputFile);
        // ofs << *pWriter;
    }
} // namespace cho::osbase::statemachine::ut
