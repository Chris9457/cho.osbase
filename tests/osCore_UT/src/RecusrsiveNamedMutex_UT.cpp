// \brief Unit tests of the class recursive_named_mutex

#include "osCore/Misc/RecursiveNamedMutex.h"
#include "gtest/gtest.h"
#include <mutex>

namespace cho::osbase::core::ut {
    class RecursiveNamesMutex_UT : public testing::Test {};

    TEST_F(RecursiveNamesMutex_UT, ctor) {
        ASSERT_EQ(nullptr, recursive_named_mutex().native_handle());
        ASSERT_NE(nullptr, recursive_named_mutex("test").native_handle());
    }

    TEST_F(RecursiveNamesMutex_UT, lock_unlock) {
        const std::string name = "test";
        recursive_named_mutex mutex1(name);
        recursive_named_mutex mutex2(name);

        // multi lock authorized id=n the same thread
        mutex1.lock();
        mutex2.lock();
        SUCCEED();

        bool val = false;
        std::mutex mut;
        std::condition_variable cv;
        std::unique_lock lock(mut);
        std::thread th([&name, &cv, &val]() {
            recursive_named_mutex mutTh(name);
            mutTh.lock();
            val = true;
            cv.notify_one();
            mutTh.unlock();
        });

        ASSERT_FALSE(val);
        mutex1.unlock();
        mutex2.unlock();
        cv.wait(lock);
        ASSERT_TRUE(val);
        th.join();
    }

    TEST_F(RecursiveNamesMutex_UT, try_lock) {
        const std::string name = "test";
        recursive_named_mutex mutex1(name);
        recursive_named_mutex mutex2(name);

        // multi lock authorized id=n the same thread
        ASSERT_TRUE(mutex1.try_lock());
        ASSERT_TRUE(mutex2.try_lock());

        bool val = false;
        std::mutex mut;
        std::condition_variable cv;
        std::unique_lock lock(mut);
        std::thread th([&name, &cv, &val]() {
            recursive_named_mutex mutTh(name);
            if (mutTh.try_lock()) {
                val = true;
                mutTh.unlock();
            }
            cv.notify_one();
        });

        ASSERT_FALSE(val);
        cv.wait(lock);
        ASSERT_FALSE(val);
        th.join();
    }
} // namespace cho::osbase::core::ut
