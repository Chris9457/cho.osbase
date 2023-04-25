// \brief Unit test of the class KeyValue

#include "osCore/Serialization/KeyValue.h"
#include "osCore_UT/ConstantValue.h"
#include "gtest/gtest.h"

namespace NS_OSBASE::core::ut {

    class KeyValue_UT : public testing::Test {};
    template <typename T>
    class KeyValueTyped_UT : public testing::Test {};
    template <typename T>
    class KeyValueArrayTyped_UT : public testing::Test {};
    class KeyValueTuple_UT : public testing::Test {};

    using value_types = testing::Types<bool, int, float, double, std::wstring>;
    TYPED_TEST_SUITE(KeyValueTyped_UT, value_types);
    TYPED_TEST_SUITE(KeyValueArrayTyped_UT, value_types);

    TEST_F(KeyValue_UT, ctor) {
        const std::string keyName = "key";
        KeyValue<std::string, void> key(keyName);

        ASSERT_EQ(keyName, key.getKey());
    }

    TEST_F(KeyValue_UT, setKey) {
        const std::string keyName    = "key";
        const std::string newKeyName = "key";
        KeyValue<std::string, void> key(keyName);

        key.setKey(newKeyName);
        ASSERT_EQ(newKeyName, key.getKey());
    }

    TEST_F(KeyValue_UT, equal) {
        const std::string keyNameL = "keyL";
        const std::string keyNameR = "keyR";
        KeyValue<std::string, void> keyL(keyNameL);
        KeyValue<std::string, void> keyR(keyNameR);

        ASSERT_TRUE(keyL == keyL);
        ASSERT_FALSE(keyL == keyR);
    }

    TEST_F(KeyValue_UT, notEqual) {
        const std::string keyNameL = "keyL";
        const std::string keyNameR = "keyR";
        KeyValue<std::string, void> keyL(keyNameL);
        KeyValue<std::string, void> keyR(keyNameR);

        ASSERT_FALSE(keyL != keyL);
        ASSERT_TRUE(keyL != keyR);
    }

    TEST_F(KeyValue_UT, getBasicType) {
        const std::string keyName = "key";
        KeyValue<std::string, void> key(keyName);

        ASSERT_EQ(BasicType::Void, key.getBasicType());
    }

    TEST_F(KeyValue_UT, null) {
        const std::string keyName = "key";
        KeyValue<std::string, void> keyValue(keyName);
        const auto &nullValue = KeyValue<std::string, void>::null();
        auto copiedNullValue  = KeyValue<std::string, void>::null();

        ASSERT_FALSE(keyValue.isNull());
        ASSERT_TRUE(nullValue.isNull());
        ASSERT_TRUE(copiedNullValue.isNull());
    }

    TEST_F(KeyValue_UT, makeStringKeyValue) {
        const std::string keyName = "key";

        auto const kv = makeKeyValue(keyName, "toto");
        ASSERT_EQ(std::string("toto"), kv.getValue());

        auto const wkv = makeKeyValue(keyName, L"toto");
        ASSERT_EQ(std::wstring(L"toto"), wkv.getValue());
    }

    TYPED_TEST(KeyValueTyped_UT, ctor) {
        const std::string keyName = "key";
        const auto value          = ConstantValue<TypeParam, 0>::getValue();
        KeyValue<std::string, TypeParam> key(keyName, value);

        ASSERT_EQ(value, key.getValue());
    }

    TYPED_TEST(KeyValueTyped_UT, setValue) {
        const std::string keyName = "key";
        const auto value          = ConstantValue<TypeParam, 0>::getValue();
        const auto newValue       = ConstantValue<TypeParam, 1>::getValue();
        KeyValue<std::string, TypeParam> key(keyName, value);

        key.setValue(newValue);
        ASSERT_EQ(newValue, key.getValue());
    }

    TYPED_TEST(KeyValueTyped_UT, equal) {
        const std::string keyNameL = "keyL";
        const std::string keyNameR = "keyR";
        const auto valueL          = ConstantValue<TypeParam, 0>::getValue();
        const auto valueR          = ConstantValue<TypeParam, 1>::getValue();
        KeyValue<std::string, TypeParam> keyL1(keyNameL, valueL);
        KeyValue<std::string, TypeParam> keyL2(keyNameL, valueR);
        KeyValue<std::string, TypeParam> keyR1(keyNameR, valueL);
        KeyValue<std::string, TypeParam> keyR2(keyNameR, valueR);

        ASSERT_TRUE(keyL1 == keyL1);  // Same key, same value
        ASSERT_FALSE(keyL1 == keyL2); // Same key, other value
        ASSERT_FALSE(keyL1 == keyR1); // Other key, same value
        ASSERT_FALSE(keyL1 == keyR2); // Other key, otherValue
    }

    TYPED_TEST(KeyValueTyped_UT, notEqual) {
        const std::string keyNameL = "keyL";
        const std::string keyNameR = "keyR";
        const auto valueL          = ConstantValue<TypeParam, 0>::getValue();
        const auto valueR          = ConstantValue<TypeParam, 1>::getValue();
        KeyValue<std::string, TypeParam> keyL1(keyNameL, valueL);
        KeyValue<std::string, TypeParam> keyL2(keyNameL, valueR);
        KeyValue<std::string, TypeParam> keyR1(keyNameR, valueL);
        KeyValue<std::string, TypeParam> keyR2(keyNameR, valueR);

        ASSERT_FALSE(keyL1 != keyL1); // Same key, same value
        ASSERT_TRUE(keyL1 != keyL2);  // Same key, other value
        ASSERT_TRUE(keyL1 != keyR1);  // Other key, same value
        ASSERT_TRUE(keyL1 != keyR2);  // Other key, otherValue
    }

    TYPED_TEST(KeyValueTyped_UT, getBasicType) {
        const std::string keyName = "key";
        const auto value          = ConstantValue<TypeParam, 0>::getValue();
        KeyValue<std::string, TypeParam> keyValue(keyName, value);

        ASSERT_EQ(BasicTypeT<TypeParam>::value, keyValue.getBasicType());
    }

    TYPED_TEST(KeyValueTyped_UT, null) {
        const std::string keyName = "key";
        const auto value          = ConstantValue<TypeParam, 0>::getValue();
        KeyValue<std::string, TypeParam> keyValue(keyName, value);
        const auto &nullValue = KeyValue<std::string, TypeParam>::null();
        auto copiedNullValue  = KeyValue<std::string, TypeParam>::null();

        ASSERT_FALSE(keyValue.isNull());
        ASSERT_TRUE(nullValue.isNull());
        ASSERT_TRUE(copiedNullValue.isNull());
    }

    TYPED_TEST(KeyValueTyped_UT, makeKeyValue) {
        const std::string keyName = "key";
        const auto value          = ConstantValue<TypeParam, 0>::getValue();
        KeyValue<std::string, TypeParam> keyValue(keyName, value);

        ASSERT_EQ(makeKeyValue(keyName, value), keyValue);
    }

    TYPED_TEST(KeyValueTyped_UT, makeDefaultKeyValue) {
        const std::string keyName = "key";
        const auto value          = TypeParam{};
        KeyValue<std::string, TypeParam> keyValue(keyName, value);

        auto const defaultKeyValue = makeKeyValue<TypeParam>(keyName);
        ASSERT_EQ(defaultKeyValue, keyValue);
    }

    TYPED_TEST(KeyValueArrayTyped_UT, ctor) {
        const std::string keyName     = "key";
        std::vector<TypeParam> values = {
            ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue(), ConstantValue<TypeParam, 2>::getValue()
        };
        KeyValue<std::string, TypeParam[]> keyValue(keyName, values);

        ASSERT_EQ(values.size(), keyValue.size());
        for (size_t index = 0; index < values.size(); ++index)
            ASSERT_EQ(values[index], keyValue.getValue(index));
    }

    TYPED_TEST(KeyValueArrayTyped_UT, setValue) {
        const std::string keyName     = "key";
        std::vector<TypeParam> values = {
            ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue(), ConstantValue<TypeParam, 2>::getValue()
        };
        KeyValue<std::string, TypeParam[]> keyValue(keyName, values);

        for (size_t index = 0; index < values.size() - 1; ++index) {
            const auto newValue = values[index + 1];
            keyValue.setValue(index, newValue);
            ASSERT_EQ(newValue, keyValue.getValue(index));
        }
    }

    TYPED_TEST(KeyValueArrayTyped_UT, addValue) {
        const std::string keyName     = "key";
        std::vector<TypeParam> values = {
            ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue(), ConstantValue<TypeParam, 2>::getValue()
        };
        KeyValue<std::string, TypeParam[]> keyValue(keyName, values);

        const auto newValue = ConstantValue<TypeParam, 0>::getValue();
        keyValue.addValue(newValue);
        ASSERT_EQ(values.size() + 1, keyValue.size());
        ASSERT_EQ(newValue, keyValue.getValue(keyValue.size() - 1));
    }

    TYPED_TEST(KeyValueArrayTyped_UT, equal) {
        const std::string keyNameL    = "keyL";
        const std::string keyNameR    = "keyR";
        std::vector<TypeParam> valueL = {
            ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue(), ConstantValue<TypeParam, 2>::getValue()
        };
        std::vector<TypeParam> valueR = {
            ConstantValue<TypeParam, 2>::getValue(), ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue()
        };
        KeyValue<std::string, TypeParam[]> keyL1(keyNameL, valueL);
        KeyValue<std::string, TypeParam[]> keyL2(keyNameL, valueR);
        KeyValue<std::string, TypeParam[]> keyR1(keyNameR, valueL);
        KeyValue<std::string, TypeParam[]> keyR2(keyNameR, valueR);

        ASSERT_TRUE(keyL1 == keyL1);  // Same key, same value
        ASSERT_FALSE(keyL1 == keyL2); // Same key, other value
        ASSERT_FALSE(keyL1 == keyR1); // Other key, same value
        ASSERT_FALSE(keyL1 == keyR2); // Other key, otherValue
    }

    TYPED_TEST(KeyValueArrayTyped_UT, notEqual) {
        const std::string keyNameL    = "keyL";
        const std::string keyNameR    = "keyR";
        std::vector<TypeParam> valueL = {
            ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue(), ConstantValue<TypeParam, 2>::getValue()
        };
        std::vector<TypeParam> valueR = {
            ConstantValue<TypeParam, 2>::getValue(), ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue()
        };
        KeyValue<std::string, TypeParam[]> keyL1(keyNameL, valueL);
        KeyValue<std::string, TypeParam[]> keyL2(keyNameL, valueR);
        KeyValue<std::string, TypeParam[]> keyR1(keyNameR, valueL);
        KeyValue<std::string, TypeParam[]> keyR2(keyNameR, valueR);

        ASSERT_FALSE(keyL1 != keyL1); // Same key, same value
        ASSERT_TRUE(keyL1 != keyL2);  // Same key, other value
        ASSERT_TRUE(keyL1 != keyR1);  // Other key, same value
        ASSERT_TRUE(keyL1 != keyR2);  // Other key, otherValue
    }

    TYPED_TEST(KeyValueArrayTyped_UT, makeKeyValue) {
        const std::string keyName     = "key";
        std::vector<TypeParam> values = {
            ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue(), ConstantValue<TypeParam, 2>::getValue()
        };
        KeyValue<std::string, TypeParam[]> keyValue(keyName, values);

        ASSERT_EQ(makeKeyValue(keyName, values), keyValue);
    }

    TYPED_TEST(KeyValueArrayTyped_UT, makeDefaultKeyValue) {
        const std::string keyName = "key";
        const auto values         = std::vector<TypeParam>{};
        KeyValue<std::string, TypeParam[]> keyValue(keyName, values);

        auto const defaultKeyValue = makeKeyValue<TypeParam[]>(keyName);
        ASSERT_EQ(defaultKeyValue, keyValue);
    }

    TYPED_TEST(KeyValueArrayTyped_UT, operatorSubscript) {
        const std::string keyName     = "key";
        std::vector<TypeParam> values = {
            ConstantValue<TypeParam, 0>::getValue(), ConstantValue<TypeParam, 1>::getValue(), ConstantValue<TypeParam, 2>::getValue()
        };
        KeyValue<std::string, TypeParam[]> keyValue(keyName, values);

        for (size_t index = 0; index < values.size() - 1; ++index) {
            ASSERT_EQ(keyValue.getValue(index), keyValue[index]);

            const auto newValue = values[index + 1];
            keyValue[index]     = newValue;
            ASSERT_EQ(newValue, keyValue.getValue(index));
        }
    }

    TEST_F(KeyValueTuple_UT, makeKeyValue) {
        const std::string key = "key";
        // 1 arg
        {
            auto expectedResult = 2;
            auto keyValue       = makeKeyValue(key, 2);

            constexpr bool bSameType = std::is_same_v<decltype(expectedResult), decltype(keyValue)::ValueType>;
            ASSERT_TRUE(bSameType);
            ASSERT_EQ(expectedResult, keyValue.getValue());
        }

        // 2 args
        {
            auto expectedResult = std::make_tuple(2, std::string("toto"));
            auto keyValue       = makeKeyValue(key, 2, std::string("toto"));

            constexpr bool bSameType = std::is_same_v<decltype(expectedResult), decltype(keyValue)::ValueType>;
            ASSERT_TRUE(bSameType);
            ASSERT_EQ(expectedResult, keyValue.getValue());
        }

        // 3 args
        {
            auto expectedResult = std::make_tuple(2, std::string("toto"), 5.2);
            auto keyValue       = makeKeyValue(key, 2, std::string("toto"), 5.2);

            constexpr bool bSameType = std::is_same_v<decltype(expectedResult), decltype(keyValue)::ValueType>;
            ASSERT_TRUE(bSameType);
            ASSERT_EQ(expectedResult, keyValue.getValue());
        }
    }
} // namespace NS_OSBASE::core::ut
