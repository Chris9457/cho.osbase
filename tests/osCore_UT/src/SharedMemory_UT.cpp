// \brief Unit tests of the class SharedMemory

#include "osCore/Interprocess/sharedmemory.h"
#include "gtest/gtest.h"
#include <future>

namespace NS_OSBASE::core::ut {
    class SharedMemory_UT : public testing::Test {
    protected:
        static bool testLockProc(SharedMemory<void> *pSharedMemory) {
            pSharedMemory->lock();
            return true;
        }

        bool testLock(SharedMemory<void> &sharedMemory) {
            std::future<bool> lockStatus = std::async(&testLockProc, &sharedMemory);

            if (lockStatus.wait_for(std::chrono::seconds(1)) == std::future_status::timeout) {
                sharedMemory.unlock();
                return false;
            }

            return true;
        }
    };

    TEST_F(SharedMemory_UT, createOK) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory(name);

        EXPECT_TRUE(sharedMemory.create());
    }

    TEST_F(SharedMemory_UT, createKO) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory1(name);
        SharedMemory<int> sharedMemory2(name);

        ASSERT_TRUE(sharedMemory1.create());
        ASSERT_FALSE(sharedMemory2.create());
    }

    TEST_F(SharedMemory_UT, openOK) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory1(name);
        SharedMemory<int> sharedMemory2(name);

        ASSERT_TRUE(sharedMemory1.create());
        ASSERT_TRUE(sharedMemory2.open());
    }

    TEST_F(SharedMemory_UT, openKO) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory(name);

        ASSERT_FALSE(sharedMemory.open());
    }

    TEST_F(SharedMemory_UT, getSet) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory1(name);
        SharedMemory<int> sharedMemory2(name);

        ASSERT_TRUE(sharedMemory1.create());
        ASSERT_TRUE(sharedMemory2.open());

        const int value = 45;
        sharedMemory1.set(value);
        ASSERT_EQ(sharedMemory2.get(), value);
    }

    TEST_F(SharedMemory_UT, lock) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory1(name);
        SharedMemory<int> sharedMemory2(name);

        sharedMemory1.lock();
        const bool bExpectedLock = testLock(sharedMemory2);
        ASSERT_FALSE(bExpectedLock);
    }

    TEST_F(SharedMemory_UT, unlock) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory1(name);
        SharedMemory<int> sharedMemory2(name);

        sharedMemory1.lock();
        sharedMemory1.unlock();
        ASSERT_TRUE(testLock(sharedMemory2));
    }

    TEST_F(SharedMemory_UT, locker) {
        const std::string name = "test shared memory";
        SharedMemory<int> sharedMemory1(name);
        SharedMemory<int> sharedMemory2(name);

        {
            SharedMemoryLocker _(sharedMemory1);
            const bool bExpectedLock = testLock(sharedMemory2);
            ASSERT_FALSE(bExpectedLock);
        }

        ASSERT_TRUE(testLock(sharedMemory2));
    }
} // namespace NS_OSBASE::core::ut
