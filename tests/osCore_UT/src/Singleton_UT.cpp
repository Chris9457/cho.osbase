// \brief Unit tests of the design pattern Singleton

#include "Shared/SingletonShared.h"
#include "Static/SingletonStatic.h"
#include "gtest/gtest.h"
#include <thread>

namespace NS_OSBASE::core::ut {

    class Singleton_UT : public testing::Test {};

    int count = 0;
    class TemporizedSingleton : public Singleton<TemporizedSingleton> {
        friend class Singleton<TemporizedSingleton>;

    public:
        void touch() const {
        }
        int getCount() const {
            return m_count;
        }

    private:
        TemporizedSingleton() : m_count(++count) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
        }

        int m_count = 0;
    };
#define TheTemporizedSingleton TemporizedSingleton::getInstance()

    TEST_F(Singleton_UT, check_multi_binary_singleton) {
        // This test aim to call the singleton into the dll
        ASSERT_EQ(0, TheMySingleton.getCount());

        TheMySingleton.increment();
        ASSERT_EQ(1, TheMySingleton.getCount());

        increment();
        ASSERT_EQ(2, TheMySingleton.getCount());
    }

    TEST_F(Singleton_UT, check_thread_safe) {
        std::thread th1([]() { TheTemporizedSingleton.touch(); });
        std::thread th2([]() { TheTemporizedSingleton.touch(); });
        th1.join();
        th2.join();
        ASSERT_EQ(1, TheTemporizedSingleton.getCount());
    }
} // namespace NS_OSBASE::core::ut
