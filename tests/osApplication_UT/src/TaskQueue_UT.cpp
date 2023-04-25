// osBase package
#include "osApplication/TaskQueue.h"
#include "osData/Log.h"
#include "gtest/gtest.h"

using namespace NS_OSBASE::application;

namespace NS_OSBASE::application::ut {

    class TaskQueue_UT : public testing::Test {
    protected:
        static constexpr std::chrono::milliseconds getTimeout(unsigned int timeout) {
            return std::chrono::milliseconds(timeout * TIMEOUT_FACTOR);
        }
    };

    namespace {
        struct S {
            S() {
                m_ret = 0;
            }

            void setInt(const int val) {
                m_ret = val;
            }

            static int m_ret;
        };
        int S::m_ret{};
    } // namespace

    TEST_F(TaskQueue_UT, pushTask_pullTask) {
        TaskQueue tq;
        bool bTaskExecuted1 = false, bTaskExecuted2 = false;

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushTask([&bTaskExecuted1]() { bTaskExecuted1 = true; });
        tq.pushTask([&bTaskExecuted2]() { bTaskExecuted2 = true; });
        ASSERT_TRUE(tq.isRemainingTasks());

        auto pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_TRUE(bTaskExecuted1);
        ASSERT_FALSE(bTaskExecuted2);

        pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_TRUE(bTaskExecuted2);

        pTask = tq.pullTask();
        ASSERT_EQ(nullptr, pTask);
    }

    TEST_F(TaskQueue_UT, pushImmediatTask) {
        TaskQueue tq;
        bool bTaskExecuted1 = false, bTaskExecuted2 = false;

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushTask([&bTaskExecuted1]() { bTaskExecuted1 = true; });
        tq.pushImmediateTask([&bTaskExecuted2]() { bTaskExecuted2 = true; });
        ASSERT_TRUE(tq.isRemainingTasks());

        auto pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_FALSE(bTaskExecuted1);
        ASSERT_TRUE(bTaskExecuted2);

        pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_TRUE(bTaskExecuted1);
    }

    TEST_F(TaskQueue_UT, pushSingleShotTask) {
        using namespace std::chrono_literals;
        TaskQueue tq;

        auto constexpr delay = getTimeout(50);
        auto const start     = TaskQueue::clock::now();
        bool bTaskExecuted1 = false, bTaskExecuted2 = false, bTaskExecuted3 = false;

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushSingleShotTask(delay, [&bTaskExecuted1]() { bTaskExecuted1 = true; });
        tq.pushSingleShotTask(delay * 2, [&bTaskExecuted2]() { bTaskExecuted2 = true; });
        tq.pushSingleShotTask(delay / 2, [&bTaskExecuted3]() { bTaskExecuted3 = true; });
        ASSERT_TRUE(tq.isRemainingTasks());

        auto timespan = 0ms;

        // first task
        auto pTask = tq.waitForTask();
        ASSERT_TRUE(tq.isRemainingTasks());

        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_FALSE(bTaskExecuted1);
        ASSERT_FALSE(bTaskExecuted2);
        ASSERT_TRUE(bTaskExecuted3);
        bTaskExecuted3 = false;
        timespan       = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
        ASSERT_GE(timespan, delay / 2);
        ASSERT_LE(timespan, delay);

        // second task
        pTask = tq.waitForTask();
        ASSERT_TRUE(tq.isRemainingTasks());

        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_TRUE(bTaskExecuted1);
        ASSERT_FALSE(bTaskExecuted2);
        ASSERT_FALSE(bTaskExecuted3);
        bTaskExecuted1 = false;
        timespan       = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
        ASSERT_GE(timespan, delay);
        ASSERT_LE(timespan, delay * 2);

        // third task
        pTask = tq.waitForTask();
        ASSERT_FALSE(tq.isRemainingTasks());

        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_FALSE(bTaskExecuted1);
        ASSERT_TRUE(bTaskExecuted2);
        ASSERT_FALSE(bTaskExecuted3);
        bTaskExecuted2 = false;
        timespan       = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
        ASSERT_GE(timespan, delay * 2);

        oslog::trace() << "timespan: " << timespan.count() << oslog::end();
    }

    TEST_F(TaskQueue_UT, pushRepeatedTask) {
        using namespace std::chrono_literals;
        TaskQueue tq;

        auto constexpr delay = getTimeout(50);
        bool bTaskExecuted   = false;

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushRepeatedTask(delay, [&bTaskExecuted]() { bTaskExecuted = true; });
        ASSERT_TRUE(tq.isRemainingTasks());

        // Stop repeating for the last
        for (int i = 0; i < 5; ++i) {
            auto const start = TaskQueue::clock::now();
            auto pTask       = tq.waitForTask();

            if (i == 4) {
                ASSERT_FALSE(tq.isRemainingTasks());
                break;
            } else {
                ASSERT_TRUE(tq.isRemainingTasks());
            }
            ASSERT_NE(nullptr, pTask);

            ASSERT_FALSE(bTaskExecuted);
            pTask->execute();
            ASSERT_TRUE(bTaskExecuted);
            bTaskExecuted = false;

            auto const timespan = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
            ASSERT_LE(abs(timespan - delay), getTimeout(20));
            oslog::trace() << "timespan: " << timespan.count() << oslog::end();

            if (i == 3) {
                pTask->setEnabled(false);
            }
        }
    }

    TEST_F(TaskQueue_UT, pushMethodTask_pullTask) {
        TaskQueue tq;
        auto pS = std::make_shared<S>();

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushMethodTask(pS, &S::setInt, 2);
        tq.pushMethodTask(pS, &S::setInt, 3);
        ASSERT_TRUE(tq.isRemainingTasks());

        auto pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_EQ(S::m_ret, 2);

        // Check pS is not more called (reset)
        pS.reset();
        pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_EQ(S::m_ret, 2);

        pTask = tq.pullTask();
        ASSERT_EQ(nullptr, pTask);
    }

    TEST_F(TaskQueue_UT, pushImmediatMethodTask) {
        TaskQueue tq;
        auto pS = std::make_shared<S>();

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushMethodTask(pS, &S::setInt, 2);
        tq.pushImmediateMethodTask(pS, &S::setInt, 3);
        ASSERT_TRUE(tq.isRemainingTasks());

        auto pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_EQ(S::m_ret, 3);

        // Check pS is not more called (reset)
        pS.reset();
        pTask = tq.pullTask();
        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_EQ(S::m_ret, 3);

        pTask = tq.pullTask();
        ASSERT_EQ(nullptr, pTask);
    }

    TEST_F(TaskQueue_UT, pushSingleShotMethodTask) {
        using namespace std::chrono_literals;
        TaskQueue tq;
        auto pS = std::make_shared<S>();

        auto constexpr delay = getTimeout(50);
        auto const start     = TaskQueue::clock::now();

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushSingleShotMethodTask(delay, pS, &S::setInt, 1);
        tq.pushSingleShotMethodTask(delay * 2, pS, &S::setInt, 2);
        tq.pushSingleShotMethodTask(delay / 2, pS, &S::setInt, 3);
        ASSERT_TRUE(tq.isRemainingTasks());

        auto timespan = 0ms;

        // first task
        auto pTask = tq.waitForTask();
        ASSERT_TRUE(tq.isRemainingTasks());

        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_EQ(3, S::m_ret);
        timespan = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
        ASSERT_GE(timespan, delay / 2);
        ASSERT_LE(timespan, delay);

        // second task
        pTask = tq.waitForTask();
        ASSERT_TRUE(tq.isRemainingTasks());

        ASSERT_NE(nullptr, pTask);
        pTask->execute();
        ASSERT_EQ(1, S::m_ret);
        timespan = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
        ASSERT_GE(timespan, delay);
        ASSERT_LE(timespan, delay * 2);

        // third task
        pTask = tq.waitForTask();
        ASSERT_FALSE(tq.isRemainingTasks());

        ASSERT_NE(nullptr, pTask);
        pS.reset();
        pTask->execute(); // check pS is not more called
        ASSERT_EQ(1, S::m_ret);
        timespan = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
        ASSERT_GE(timespan, delay * 2);

        oslog::trace() << "timespan: " << timespan.count() << oslog::end();
    }

    TEST_F(TaskQueue_UT, pushRepeatedMethodTask) {
        using namespace std::chrono_literals;
        TaskQueue tq;
        auto const pS = std::make_shared<S>();

        auto constexpr delay = getTimeout(50);

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushRepeatedMethodTask(delay, pS, &S::setInt, 1);
        ASSERT_TRUE(tq.isRemainingTasks());

        for (int i = 0; i < 5; ++i) {
            auto const start = TaskQueue::clock::now();
            auto pTask       = tq.waitForTask();

            ASSERT_TRUE(tq.isRemainingTasks());
            ASSERT_NE(nullptr, pTask);

            ASSERT_NE(1, S::m_ret);
            pTask->execute();
            ASSERT_EQ(1, S::m_ret);
            S::m_ret = 0;

            auto const timespan = std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start);
            ASSERT_LE(abs(timespan - delay), getTimeout(20));
            oslog::trace() << "timespan: " << timespan.count() << oslog::end();
        }
    }

    TEST_F(TaskQueue_UT, waitForTask) {
        using namespace std::chrono_literals;
        TaskQueue tq;

        auto constexpr delay = getTimeout(50);
        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushRepeatedTask(delay, []() {});

        auto pTask = tq.waitForTask(/* infinite */);
        ASSERT_NE(nullptr, pTask);

        pTask = tq.waitForTask(delay / 2);
        ASSERT_EQ(nullptr, pTask);

        pTask = tq.waitForTask(delay * 2);
        ASSERT_NE(nullptr, pTask);
    }

    TEST_F(TaskQueue_UT, waitUntilTask) {
        using namespace std::chrono_literals;
        TaskQueue tq;

        auto constexpr delay = getTimeout(50);
        int timespan         = 0;

        ASSERT_FALSE(tq.isRemainingTasks());

        tq.pushRepeatedTask(delay, []() {});

        auto start = TaskQueue::clock::now();
        auto pTask = tq.waitUntilTask(start + std::chrono::milliseconds(delay * 2));
        ASSERT_NE(nullptr, pTask);
        timespan = (int)std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start).count();
        oslog::trace() << "timespan: " << timespan << oslog::end();

        start = TaskQueue::clock::now();
        pTask = tq.waitUntilTask(start + std::chrono::milliseconds(delay / 2));
        ASSERT_EQ(nullptr, pTask);
        timespan = (int)std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start).count();
        oslog::trace() << "timespan: " << timespan << oslog::end();

        start = TaskQueue::clock::now();
        pTask = tq.waitUntilTask(start + std::chrono::milliseconds(delay * 2));
        ASSERT_NE(nullptr, pTask);
        timespan = (int)std::chrono::duration_cast<std::chrono::milliseconds>(TaskQueue::clock::now() - start).count();
        oslog::trace() << "timespan: " << timespan << oslog::end();
    }
} // namespace NS_OSBASE::application::ut
