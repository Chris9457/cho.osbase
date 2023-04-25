// osBase package
#include "osApplication/Task.h"
#include "osData/Log.h"
#include "gtest/gtest.h"

using namespace NS_OSBASE::application;

namespace NS_OSBASE::application::ut {

    class Task_UT : public testing::Test {};

    namespace {
        struct S {
            int getInt(const int val) const {
                return val;
            }

            void setInt(const int val) {
                m_ret = val;
            }

            void setString(std::string str) {
                auto const s = str;
            }

            int m_ret;
        };

        static int s_myI = 0;
        static std::string s_myStr;

        void doVoid(int i, const std::string str) {
            s_myI   = i;
            s_myStr = str;
        };

        int doInt(int i, const std::string str) {
            s_myI   = i + 1;
            s_myStr = "_" + str;
            return i;
        }
    } // namespace

    TEST_F(Task_UT, checkDecoratedArgs) {
        auto pS = std::make_shared<S>();
        ITaskPtr pTask;
        {
            const std::string str = "toto";
            pTask                 = makeMethodTask(pS, &S::setString, str);
        }
        pTask->execute();
    }

    TEST_F(Task_UT, makerFunction) {

        auto const pFnTask = makeTask(&doVoid, 2, "toto");
        ASSERT_NE(nullptr, pFnTask);

        auto const pFnIntTask = makeTask(&doInt, 2, "toto");
        ASSERT_NE(nullptr, pFnIntTask);
    }

    TEST_F(Task_UT, executeFunction) {
        s_myI            = 0;
        s_myStr          = "";
        auto const pTask = makeTask(&doVoid, 1, "toto");
        pTask->execute();
        ASSERT_EQ(1, s_myI);
        ASSERT_EQ("toto", s_myStr);

        auto const pIntTask = makeTypedTask<int>(&doInt, 1, "toto");
        auto newInt         = pIntTask->execute();
        ASSERT_EQ(2, s_myI);
        ASSERT_EQ("_toto", s_myStr);
        constexpr bool isInt = std::is_same_v<int, decltype(newInt)>;
        ASSERT_TRUE(isInt);
        ASSERT_EQ(1, newInt);
    }

    TEST_F(Task_UT, makerLambda) {
        auto myVoidFunction = [](int i, const std::string str) { oslog::trace() << "i: " << i << ", str: " << str << oslog::end(); };
        auto const pTask    = makeTask(myVoidFunction, 1, "toto");
        ASSERT_NE(nullptr, pTask);

        auto myIntFunction = [](int i, const std::string str) {
            oslog::trace() << "i: " << i << ", str: " << str << oslog::end();
            return i;
        };
        auto const pIntTask = makeTypedTask<int>(myIntFunction, 1, "toto");
        ASSERT_NE(nullptr, pIntTask);
    }

    TEST_F(Task_UT, executeLambda) {
        int myI = 0;
        std::string myStr;

        auto myVoidFunction = [&myI, &myStr](int i, const std::string str) {
            myI   = i;
            myStr = str;
        };
        auto const pTask = makeTask(myVoidFunction, 1, "toto");
        pTask->execute();
        ASSERT_EQ(1, myI);
        ASSERT_EQ("toto", myStr);

        auto myIntFunction = [&myI, &myStr](int i, const std::string str) {
            myI   = i + 1;
            myStr = "_" + str;
            return i;
        };
        auto const pIntTask = makeTypedTask<int>(myIntFunction, 1, "toto");
        auto newInt         = pIntTask->execute();
        ASSERT_EQ(2, myI);
        ASSERT_EQ("_toto", myStr);
        const bool isInt = std::is_same_v<int, decltype(newInt)>;
        ASSERT_TRUE(isInt);
        ASSERT_EQ(1, newInt);
    }

    TEST_F(Task_UT, makerMethod) {
        auto const pS = std::make_shared<S>();

        auto const pIntMthTask = makeTypedMethodTask<int>(pS, &S::getInt, 1);
        ASSERT_NE(nullptr, pIntMthTask);

        auto const pMthTask = makeMethodTask(pS, &S::setInt, 1);
        ASSERT_NE(nullptr, pMthTask);
    }

    TEST_F(Task_UT, methodExecute) {
        auto pS = std::make_shared<S>();

        auto const pIntMthTask = makeTypedMethodTask<int>(pS, &S::getInt, 1);
        auto ret               = pIntMthTask->execute();
        ASSERT_EQ(1, ret);

        auto const pMthTask = makeMethodTask(pS, &S::setInt, 1);
        pMthTask->execute();
        ASSERT_EQ(1, pS->m_ret);

        pS.reset();
        ret = pIntMthTask->execute();
        ASSERT_EQ(int{}, ret);
    }

    TEST_F(Task_UT, enable) {
        auto const pTask = makeTask(&doVoid, 1, "toto");
        ASSERT_TRUE(pTask->isEnabled());

        s_myI   = 0;
        s_myStr = "";
        pTask->execute();
        ASSERT_EQ(1, s_myI);
        ASSERT_EQ("toto", s_myStr);

        pTask->setEnabled(false);
        ASSERT_FALSE(pTask->isEnabled());

        s_myI   = 0;
        s_myStr = "";
        pTask->execute();
        ASSERT_EQ(0, s_myI);
        ASSERT_EQ("", s_myStr);
    }
} // namespace NS_OSBASE::application::ut
