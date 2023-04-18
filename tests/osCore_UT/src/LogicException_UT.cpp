// \brief Unit tests of the class LogicException

#include "osCore/Exception/LogicException.h"
#include "gtest/gtest.h"

namespace nscore = cho::osbase::core;

namespace cho::osbase::core::ut {

    class LogicException_UT : public testing::Test {};

    const std::string logicExceptionMessage("Logic Exception Message");

    void functionThrowingALogicException() {
        throw nscore::LogicException(logicExceptionMessage);
    }

    void functionThrowingALogicExceptionWithString() {
        throw nscore::LogicException(logicExceptionMessage);
    }

    void functionThrowingALogicExceptionWhithCharPtr() {
        throw nscore::LogicException(logicExceptionMessage.c_str());
    }

    void functionThrowingALogicExceptionWithCopy() {
        nscore::LogicException e(logicExceptionMessage);
        throw nscore::LogicException(e);
    }

    TEST_F(LogicException_UT, test_generalexception_message) {
        ASSERT_THROW(functionThrowingALogicException(), nscore::LogicException);
        ASSERT_THROW(functionThrowingALogicExceptionWithString(), nscore::LogicException);
        ASSERT_THROW(functionThrowingALogicExceptionWhithCharPtr(), nscore::LogicException);
        ASSERT_THROW(functionThrowingALogicExceptionWithCopy(), nscore::LogicException);

        try {
            functionThrowingALogicException();
        } catch (const nscore::LogicException &e) {
            EXPECT_STREQ(e.what(), logicExceptionMessage.c_str()) << "Wrong message in logic exception";
        }

        try {
            functionThrowingALogicExceptionWithString();
        } catch (const nscore::LogicException &e) {
            EXPECT_STREQ(e.what(), logicExceptionMessage.c_str()) << "Wrong message in logic exception";
        }

        try {
            functionThrowingALogicExceptionWhithCharPtr();
        } catch (const nscore::LogicException &e) {
            EXPECT_STREQ(e.what(), logicExceptionMessage.c_str()) << "Wrong message in logic exception";
        }

        try {
            functionThrowingALogicExceptionWithCopy();
        } catch (const nscore::LogicException &e) {
            EXPECT_STREQ(e.what(), logicExceptionMessage.c_str()) << "Wrong message in logic exception";
        }
    }
} // namespace cho::osbase::core::ut
