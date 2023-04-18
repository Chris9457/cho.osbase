// \brief Unit tests of the class RuntimeException

#include "osCore/Exception/RuntimeException.h"
#include "gtest/gtest.h"

namespace cho::osbase::core::ut {

    class RuntimeException_UT : public testing::Test {};

    const std::string runtimeExceptionMessage("Runtime Exception Message");

    void functionThrowingARuntimeException() {
        throw RuntimeException(runtimeExceptionMessage);
    }

    void functionThrowingARuntimeExceptionWithString() {
        throw RuntimeException(runtimeExceptionMessage);
    }

    void functionThrowingARuntimeExceptionWhithCharPtr() {
        throw RuntimeException(runtimeExceptionMessage.c_str());
    }

    void functionThrowingARuntimeExceptionWithCopy() {
        RuntimeException e(runtimeExceptionMessage);
        throw RuntimeException(e);
    }

    TEST_F(RuntimeException_UT, test_generalexception_message) {
        ASSERT_THROW(functionThrowingARuntimeException(), RuntimeException);
        ASSERT_THROW(functionThrowingARuntimeExceptionWithString(), RuntimeException);
        ASSERT_THROW(functionThrowingARuntimeExceptionWhithCharPtr(), RuntimeException);
        ASSERT_THROW(functionThrowingARuntimeExceptionWithCopy(), RuntimeException);

        try {
            functionThrowingARuntimeException();
        } catch (const RuntimeException &e) {
            EXPECT_STREQ(e.what(), runtimeExceptionMessage.c_str()) << "Wrong message in runtime exception";
        }

        try {
            functionThrowingARuntimeExceptionWithString();
        } catch (const RuntimeException &e) {
            EXPECT_STREQ(e.what(), runtimeExceptionMessage.c_str()) << "Wrong message in runtime exception";
        }

        try {
            functionThrowingARuntimeExceptionWhithCharPtr();
        } catch (const RuntimeException &e) {
            EXPECT_STREQ(e.what(), runtimeExceptionMessage.c_str()) << "Wrong message in runtime exception";
        }

        try {
            functionThrowingARuntimeExceptionWithCopy();
        } catch (const RuntimeException &e) {
            EXPECT_STREQ(e.what(), runtimeExceptionMessage.c_str()) << "Wrong message in runtime exception";
        }
    }
} // namespace cho::osbase::core::ut
