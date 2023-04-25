// osBase package
#include "osApplication/TaskLoop.h"
#include "osCore/Exception/Exception.h"
#include "gtest/gtest.h"
#include <filesystem>

using namespace NS_OSBASE::application;

namespace NS_OSBASE::application::ut {

    class TaskLoop_UT : public testing::Test {
    protected:
        void TearDown() override {
            for (auto const &dumpFile : getDumpFiles()) {
                remove(dumpFile);
            }
        }

        std::vector<std::filesystem::path> getDumpFiles() const {
            std::vector<std::filesystem::path> dumpFiles;

            for (auto const &dirEntry : std::filesystem::directory_iterator{ core::TheStructuredExceptionHandler.getMinidumpFolder() }) {
                if (!dirEntry.is_directory()) {
                    const std::filesystem::path path = dirEntry;
                    if (path.filename().string().find(core::TheStructuredExceptionHandler.getDumpBaseFileName()) != std::string::npos) {
                        dumpFiles.push_back(path);
                    }
                }
            }

            return dumpFiles;
        }

        static void invokeCrash() {
            *(int *)0 = 0;
        }

        static constexpr std::chrono::milliseconds getTimeout(unsigned int timeout) {
            return std::chrono::milliseconds(timeout * TIMEOUT_FACTOR);
        }
    };

    namespace {
        struct S {
            S() {
                m_retInt    = 0;
                m_retDouble = 0.;
            }

            void setInt(const int val) {
                m_retInt = val;
            }

            void setDouble(const double val) {
                m_retDouble = val;
            }

            void incInt() {
                ++m_retInt;
            }

            static int m_retInt;
            static double m_retDouble;
        };
        int S::m_retInt{};
        double S::m_retDouble{};
    } // namespace

    TEST_F(TaskLoop_UT, run_stop) {
        TaskLoop loop;
        std::thread th([&loop] { loop.stop(); });
        loop.run();
        th.join();

        // Check no infinite loop
        SUCCEED();
    }

    TEST_F(TaskLoop_UT, runAsync_stop) {
        TaskLoop loop;
        auto const ret = loop.runAsync();
        loop.stop();
        ret.get();

        // Check no infinite loop
        SUCCEED();
    }

    TEST_F(TaskLoop_UT, runAsync_push_stop) {
        TaskLoop loop;
        auto const ret = loop.runAsync();
        loop.push([&loop]() { loop.stop(); });
        ret.get();

        // Check no deadloop
        SUCCEED();
    }

    TEST_F(TaskLoop_UT, run_once) {
        TaskLoop loop;
        std::condition_variable cvTask, cvExcept;
        std::mutex mut;

        auto run = [&]() {
            try {
                loop.run();
            } catch (const TaskLoopException &) {
                cvExcept.notify_one();
            }
        };

        auto const fut1 = std::async(run);
        loop.push([&]() { cvTask.notify_one(); });

        std::unique_lock<std::mutex> locker(mut);

        // Wait for the 1st task executed
        cvTask.wait(locker);
        SUCCEED();

        // 2nd try run : wait for the exception
        auto const fut2 = std::async(run);
        cvExcept.wait(locker);
        fut2.wait();
        SUCCEED();

        // 3rd try runAsyn : wait for the exception
        ASSERT_THROW(loop.runAsync(), TaskLoopException);

        loop.stop();
        fut1.wait();
    }

    TEST_F(TaskLoop_UT, runAsync_once) {
        TaskLoop loop;
        std::condition_variable cvTask, cvExcept;
        std::mutex mut;

        auto const fut1 = loop.runAsync();
        loop.push([&]() { cvTask.notify_one(); });

        std::unique_lock<std::mutex> locker(mut);

        // Wait for the 1st task executed
        cvTask.wait(locker);
        SUCCEED();

        ASSERT_THROW(loop.runAsync(), TaskLoopException);
        ASSERT_THROW(loop.run(), TaskLoopException);

        loop.stop();
    }

    TEST_F(TaskLoop_UT, run_RuntimeError_with_delegate) {
        bool bRuntimeErrorReached = false;
        class RuntimeErrorDelegate : public TaskLoop::IRuntimeErrorDelegate {
        public:
            RuntimeErrorDelegate(TaskLoop &loop, bool &bRuntimeErrorReached)
                : m_loop{ loop }, m_bRuntimeErrorReached{ bRuntimeErrorReached } {
            }
            void onRuntimeError(const std::string &strError) override {
                std::cout << strError << std::endl;
                m_bRuntimeErrorReached = true;
                m_loop.stop();
            }

        private:
            TaskLoop &m_loop;
            bool &m_bRuntimeErrorReached;
        };

        TaskLoop loop;
        auto const pRuntimeErrorDelegate = std::make_shared<RuntimeErrorDelegate>(loop, bRuntimeErrorReached);
        loop.setRuntimeErrorDelegate(pRuntimeErrorDelegate);

        loop.push(&TaskLoop_UT::invokeCrash);
        ASSERT_FALSE(bRuntimeErrorReached);
        ASSERT_NO_THROW(loop.run());
        ASSERT_TRUE(bRuntimeErrorReached);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(TaskLoop_UT, run_RuntimeError_without_delegate) {
        TaskLoop loop;

        loop.push(&TaskLoop_UT::invokeCrash);
        ASSERT_THROW(loop.run(), core::RuntimeException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(TaskLoop_UT, push) {
        TaskLoop loop;

        int i = 0, j = 0;
        loop.push([&i]() { i = 1; });
        loop.push([&j](int val) { j = val; }, 3);
        loop.push([&loop] { loop.stop(); });
        loop.run();

        // Check no infinite loop
        ASSERT_EQ(1, i);
        ASSERT_EQ(3, j);
    }

    TEST_F(TaskLoop_UT, pushSingleShot) {
        using namespace std::chrono_literals;

        TaskLoop loop;

        int i = 0;
        loop.pushSingleShot(getTimeout(1000), [&loop] { loop.stop(); });
        loop.pushSingleShot(
            getTimeout(10), [&i](int val) { i = val; }, 3);
        loop.pushSingleShot(
            getTimeout(1500), [&i](int val) { i = val; }, 4);
        loop.run();

        // Check no infinite loop
        ASSERT_EQ(3, i);
    }

    TEST_F(TaskLoop_UT, pushRepeated) {
        using namespace std::chrono_literals;

        TaskLoop loop;

        int i                    = 0;
        auto constexpr delayRep  = getTimeout(70);
        auto constexpr delayStop = getTimeout(1000);
        auto start               = TaskLoop::clock::now();
        loop.pushRepeated(delayRep, [&]() {
            auto const now      = TaskLoop::clock::now();
            auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            EXPECT_LE(abs(duration - delayRep), getTimeout(50)) << duration.count() << " " << delayRep.count();
            ++i;
            start = now;
        });
        loop.pushSingleShot(delayStop, [&loop] { loop.stop(); });
        loop.run();

        // Check no infinite loop
        ASSERT_EQ(delayStop / delayRep, i);
    }

    TEST_F(TaskLoop_UT, pushMethod) {
        TaskLoop loop;
        auto const pS = std::make_shared<S>();

        loop.pushMethod(pS, &S::setInt, 1);
        loop.pushMethod(pS, &S::setDouble, 2.);
        loop.push([&loop] { loop.stop(); });
        loop.run();

        // Check no infinite loop
        ASSERT_EQ(1, S::m_retInt);
        ASSERT_EQ(2., S::m_retDouble);
    }

    TEST_F(TaskLoop_UT, pushSingleShotMethod) {
        using namespace std::chrono_literals;
        TaskLoop loop;
        auto pS = std::make_shared<S>();

        loop.pushSingleShot(getTimeout(1000), [&loop] { loop.stop(); });
        loop.pushSingleShotMethod(getTimeout(10), pS, &S::setInt, 1);
        loop.pushSingleShot(getTimeout(50), [&pS]() { pS.reset(); });
        loop.pushSingleShotMethod(getTimeout(100), pS, &S::setDouble, 2.); // pS has been reset before, so no call
        loop.run();

        // Check no infinite loop
        ASSERT_EQ(nullptr, pS);
        ASSERT_EQ(1, S::m_retInt);
        ASSERT_NE(2., S::m_retDouble);
    }

    TEST_F(TaskLoop_UT, pushRepeatedMethod) {
        using namespace std::chrono_literals;
        TaskLoop loop;
        auto pS = std::make_shared<S>();

        auto constexpr delayRep  = getTimeout(70);
        auto constexpr delayStop = getTimeout(1000);
        auto start               = TaskLoop::clock::now();
        loop.pushRepeatedMethod(delayRep, pS, &S::incInt);
        loop.pushSingleShot(delayStop / 2, [&pS]() { pS.reset(); }); // At the half-time, reset pS --> no more call
        loop.pushSingleShot(delayStop, [&loop] { loop.stop(); });
        loop.run();

        // Check no infinite loop
        ASSERT_EQ(delayStop / delayRep / 2, S::m_retInt);
    }
} // namespace NS_OSBASE::application::ut
