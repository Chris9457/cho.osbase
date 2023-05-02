// \brief Test of processes

#include "BaseService_UT.h"
#include "TestProcess.h"
#include "osApplication_UT/TestProcessServiceImpl.h"

namespace nsdata = NS_OSBASE::data;

namespace NS_OSBASE::application::ut {

    template <typename TMth>
    struct is_const_mth;

    template <class TClass, typename TRet, typename... TArgs>
    struct is_const_mth<TRet (TClass::*)(TArgs...)> : std::false_type {};

    template <class TClass, typename TRet, typename... TArgs>
    struct is_const_mth<TRet (TClass::*)(TArgs...) const> : std::true_type {};

    class ServiceProcess_UT : public TService_UT<process::TestProcessService, TestProcessImpl> {
    protected:
        ServiceProcess_UT()
            : TService_UT<process::TestProcessService, TestProcessImpl>(
                  []() { return process::makeStub(std::string{ "ws://" + getBrokerUrl() + ":" + std::to_string(getBrokerPort()) }, ""); }) {
        }
    };

    TEST_F(ServiceProcess_UT, callNoRetNoArg) {
        ASSERT_NO_THROW(getStub()->noReturnNoArg());
    }

    TEST_F(ServiceProcess_UT, callRetNoArg) {
        ASSERT_EQ('A', getStub()->retCharNoArg());
        ASSERT_EQ(0xff, getStub()->retUnsignedCharNoArg());
        ASSERT_EQ(-1, getStub()->retShortNoArg());
        ASSERT_EQ(3, getStub()->retUnsignedShortNoArg());
        ASSERT_EQ(-10, getStub()->retIntNoArg());
        ASSERT_EQ(114u, getStub()->retUnsignedIntNoArg());
        ASSERT_EQ(-42ll, getStub()->retLongNoArg());
        ASSERT_EQ(18ull, getStub()->retUnsignedLongNoArg());
        ASSERT_EQ(-0.5f, getStub()->retShortFloatingNoArg());
        ASSERT_EQ(-0.7, getStub()->retFloatingNoArg());
        ASSERT_EQ("toto", getStub()->retStringNoArg());
        ASSERT_EQ(type_cast<data::Uri>(std::string("ws://localhost:50210")), getStub()->retUriNoArg());
        ASSERT_EQ(true, getStub()->retCustomTypeNoArg().f1);

        const std::array<int, 4> expectedArray = { 0, 1, 2, 3 };
        ASSERT_EQ(expectedArray, getStub()->retArrayFixNoArg());

        const std::vector<std::string> expectedVector = { "titi", "toto" };
        ASSERT_EQ(expectedVector, getStub()->retArrayNotFixNoArg());

        auto asyncString                             = getStub()->retAsyncNoArg();
        const std::vector<std::string> expectedAsync = { "titi", "toto" };
        ASSERT_EQ(expectedAsync, asyncString.get());
    }

    TEST_F(ServiceProcess_UT, callRetArgs) {
        using args     = internal::deduce_tuple_from_callback<decltype(&process::TestProcess::retArgs)>::arg_type;
        auto const ret = getStub()->retArgs(
            'A', 0xff, -18, 30, -96, 78u, -45ll, 78ull, 10.4f, 0.6, "toto", true, type_cast<data::Uri>(std::string("ws://localhost:8080")));

        auto constexpr isChar = std::is_same_v<char, std::tuple_element_t<0, args>>;
        ASSERT_TRUE(isChar);
        ASSERT_EQ('A', ret._char);

        auto constexpr isUnsignedChar = std::is_same_v<unsigned char, std::tuple_element_t<1, args>>;
        ASSERT_TRUE(isUnsignedChar);
        ASSERT_EQ(0xff, ret._unsigned_char);

        auto constexpr isShort = std::is_same_v<short, std::tuple_element_t<2, args>>;
        ASSERT_TRUE(isShort);
        ASSERT_EQ(-18, ret._short_integer);

        auto constexpr isUnsignedShort = std::is_same_v<unsigned short, std::tuple_element_t<3, args>>;
        ASSERT_TRUE(isUnsignedShort);
        ASSERT_EQ(30, ret._unsigned_short_integer);

        auto constexpr isInt = std::is_same_v<int, std::tuple_element_t<4, args>>;
        ASSERT_TRUE(isInt);
        ASSERT_EQ(-96, ret._integer);

        auto constexpr isUnsignedInt = std::is_same_v<unsigned int, std::tuple_element_t<5, args>>;
        ASSERT_TRUE(isUnsignedInt);
        ASSERT_EQ(78u, ret._unsigned_integer);

        auto constexpr isLongLong = std::is_same_v<long long, std::tuple_element_t<6, args>>;
        ASSERT_TRUE(isLongLong);
        ASSERT_EQ(-45ll, ret._long_integer);

        auto constexpr isUnsignedLongLong = std::is_same_v<unsigned long long, std::tuple_element_t<7, args>>;
        ASSERT_TRUE(isUnsignedLongLong);
        ASSERT_EQ(78ull, ret._unsigned_long_integer);

        auto constexpr isFloat = std::is_same_v<float, std::tuple_element_t<8, args>>;
        ASSERT_TRUE(isFloat);
        ASSERT_EQ(10.4f, ret._short_floating);

        auto constexpr isDouble = std::is_same_v<double, std::tuple_element_t<9, args>>;
        ASSERT_TRUE(isDouble);
        ASSERT_EQ(0.6, ret._floating);

        auto constexpr isString = std::is_same_v<std::string, std::tuple_element_t<10, args>>;
        ASSERT_TRUE(isString);
        ASSERT_EQ("toto", ret._string);

        auto constexpr isBool = std::is_same_v<bool, std::tuple_element_t<11, args>>;
        ASSERT_TRUE(isBool);
        ASSERT_EQ(true, ret._boolean);

        auto constexpr isUri = std::is_same_v<data::Uri, std::tuple_element_t<12, args>>;
        ASSERT_TRUE(isUri);
        ASSERT_EQ(type_cast<data::Uri>(std::string("ws://localhost:8080")), ret._uri);
    }

    TEST_F(ServiceProcess_UT, callRetAsyncArg) {
        const process::ResultAsync expectedResult = { { true, false, false }, { "toto", "titi" } };
        process::AsyncStruct asyncStruct;

        asyncStruct.bools.create();
        asyncStruct.strings.create();
        asyncStruct.strings.set(expectedResult.strings);
        asyncStruct.bools.set(expectedResult.bools);

        auto ret = getStub()->retAsyncArg(asyncStruct);
        ASSERT_EQ(expectedResult.bools, ret.bools);
        ASSERT_EQ(expectedResult.strings, ret.strings);
        asyncStruct.bools.reset();
        asyncStruct.strings.reset();

        std::this_thread::sleep_for(getTimeout(100));
    }

    TEST_F(ServiceProcess_UT, checkConst) {
        ASSERT_TRUE(is_const_mth<decltype(&process::TestProcess::retNoArgConst)>::value);
        ASSERT_FALSE(is_const_mth<decltype(&process::TestProcess::retAsyncArg)>::value);
    }

} // namespace NS_OSBASE::application::ut
