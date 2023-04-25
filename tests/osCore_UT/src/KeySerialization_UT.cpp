// \brief Unit test of the module Key Serialization

#include "osCore/Misc/TypeCast.h"
#include "osCore/Serialization/CoreKeySerializer.h"
#include "osCore_UT/ConstantValue.h"
#include "gtest/gtest.h"

using namespace NS_OSBASE;

struct CustomType {
    int field1;
    double field2;
    bool field3;
    std::wstring field4;

    bool operator==(const CustomType &rhs) const {
        return field1 == rhs.field1 && field2 == rhs.field2 && field3 == rhs.field3 && field4 == rhs.field4;
    }

    bool operator!=(const CustomType &rhs) const {
        return !(*this == rhs);
    }
};
OS_KEY_SERIALIZE_STRUCT(CustomType, field1, field2, field3, field4);

std::ostream &operator<<(std::ostream &os, const CustomType &c) {
    return os << "field1: " << c.field1 << ", "
              << "field2: " << c.field2 << ", "
              << "field3: " << c.field3 << ", "
              << "field4: " << type_cast<std::string>(c.field4);
}

namespace NS_OSBASE::core::ut {

    template <>
    struct ConstantValue<CustomType, 0> {
        static CustomType getValue() {
            return CustomType({ 1, 1.2, true, L"type1" });
        }
    };
    template <>
    struct ConstantValue<CustomType, 1> {
        static CustomType getValue() {
            return CustomType({ 2, 2.1, false, L"type2" });
        }
    };

    template <>
    struct ConstantValue<CustomType[], 0> {
        static std::vector<CustomType> getValue() {
            return {
                CustomType({ 1, 1.2, true, L"type1" }), CustomType({ 3, 7.6, false, L"type2" }), CustomType({ 10, 9.2, true, L"type3" })
            };
        }
    };
    template <>
    struct ConstantValue<CustomType[], 1> {
        static std::vector<CustomType> getValue() {
            return { CustomType({ 10, 10.2, false, L"type10" }),
                CustomType({ 30, 70.6, true, L"type20" }),
                CustomType({ 100, 90.2, false, L"type30" }) };
        }
    };

    template <>
    struct ConstantValue<std::array<int, 2>, 0> {
        static std::array<int, 2> getValue() {
            return { 1, 2 };
        }
    };

    template <>
    struct ConstantValue<std::array<int, 2>, 1> {
        static std::array<int, 2> getValue() {
            return {
                3,
            };
        }
    };

    template <>
    struct ConstantValue<std::array<int, 2>[], 0> {
        static std::vector<std::array<int, 2>> getValue() {
            return { { 1, 2 }, { 3, 4 } };
        }
    };

    template <>
    struct ConstantValue<std::array<int, 2>[], 1> {
        static std::vector<std::array<int, 2>> getValue() {
            return { { 3, 4 }, { 1, 2 } };
        }
    };

    template <typename T>
    class KeySerialization_UT : public testing::Test {};

    using value_types = testing::
        Types<bool, int, double, std::wstring, std::string, CustomType, TestEnum, std::array<int, 2>, unsigned short, unsigned char>;
    TYPED_TEST_SUITE(KeySerialization_UT, value_types);

    class KeyStreamTuple_UT : public testing::Test {};

    TYPED_TEST(KeySerialization_UT, single_testGetSetValueOK) {
        const std::string key   = "test";
        auto const factoryNames = core::getKeyStreamFamilies();
        ASSERT_LT(0, factoryNames.size());

        for (auto &&factoryName : factoryNames) {
            std::cout << "Concrete keystream: " << factoryName << std::endl;
            auto pStream      = core::makeKeyStream(factoryName);
            auto const value1 = ConstantValue<TypeParam, 0>::getValue();

            *pStream << core::makeKeyValue(key, value1);

            auto keyValue = core::makeKeyValue<TypeParam>(key);
            *pStream >> keyValue;
            EXPECT_EQ(value1, keyValue.getValue());
        }
    }

    TYPED_TEST(KeySerialization_UT, single_testGetSetValueKO) {
        const std::string key        = "test";
        const std::string unknownKey = "unknown";
        auto const factoryNames      = core::getKeyStreamFamilies();
        ASSERT_LT(0, factoryNames.size());

        for (auto &&factoryName : factoryNames) {
            std::cout << "Concrete keystream: " << factoryName << std::endl;
            auto pStream      = core::makeKeyStream(factoryName);
            auto const value1 = ConstantValue<TypeParam, 0>::getValue();
            auto const value2 = ConstantValue<TypeParam, 1>::getValue();

            *pStream << core::makeKeyValue(key, value1);

            auto unknownKeyValue = core::makeKeyValue(unknownKey, value2);
            *pStream >> unknownKeyValue;
            EXPECT_NE(value1, unknownKeyValue.getValue());
        }
    }

    TYPED_TEST(KeySerialization_UT, multiple_testGetSetValueOK) {
        const std::string key   = "test";
        auto const factoryNames = core::getKeyStreamFamilies();
        ASSERT_LT(0, factoryNames.size());

        for (auto &&factoryName : factoryNames) {
            std::cout << "Concrete keystream: " << factoryName << std::endl;
            auto pStream      = core::makeKeyStream(factoryName);
            auto const value1 = ConstantValue<TypeParam[], 0>::getValue();
            auto const value2 = ConstantValue<TypeParam[], 1>::getValue();

            *pStream << core::makeKeyValue(key, value1);

            auto keyValue = core::makeKeyValue<TypeParam[]>(key);
            *pStream >> keyValue;
            EXPECT_EQ(value1, keyValue.getValue());
        }
    }

    TEST_F(KeyStreamTuple_UT, testGetSetValueOK) {
        const std::string key   = "test";
        auto const factoryNames = core::getKeyStreamFamilies();
        ASSERT_LT(0, factoryNames.size());

        for (auto &&factoryName : factoryNames) {
            std::cout << "Concrete keystream: " << factoryName << std::endl;
            auto pStream = core::makeKeyStream(factoryName);

            auto const keyValueIn = core::makeKeyValue(key, std::string("toto"), 2., ConstantValue<CustomType, 0>::getValue());
            auto keyValueOut      = core::makeKeyValue<decltype(keyValueIn)::ValueType>(key);

            *pStream << keyValueIn;
            *pStream >> keyValueOut;

            EXPECT_EQ(keyValueIn.getValue(), keyValueOut.getValue());
        }
    }

    struct MySubCustomData {
        std::string strField;
        bool operator==(const MySubCustomData &other) const {
            return strField == other.strField;
        }
    };
    struct MyCustomData {
        int intField = 0;
        MySubCustomData subCustomDataField;
        bool operator==(const MyCustomData &other) const {
            return intField == other.intField && subCustomDataField == other.subCustomDataField;
        }
    };

    void serialize(core::KeyStream<std::string> &keyStream, const std::string &key, const MyCustomData &myCustomData) {
        keyStream << core::makeKeyValue(key, myCustomData);
    }

    MyCustomData deserialize(core::KeyStream<std::string> &keyStream, const std::string &key) {
        auto keyValue = core::makeKeyValue<MyCustomData>(key);
        keyStream >> keyValue;
        return keyValue.getValue();
    }

    TEST(KeyStream__UT, customData) {
        const MyCustomData myCustomData{ 2, { "toto" } };

        for (auto &&keyStreamFamily : core::getKeyStreamFamilies()) {
            auto const pKeyStream = core::makeKeyStream(keyStreamFamily);
            std::string keyName   = "customData";

            serialize(*pKeyStream, keyName, myCustomData);
            std::ostringstream oss;
            oss << *pKeyStream;
            std::string str = oss.str();

            MyCustomData otherCustomData = deserialize(*pKeyStream, keyName);

            if (!(otherCustomData == myCustomData))
                throw std::exception();
            ASSERT_EQ(otherCustomData, myCustomData);
        }
    }
} // namespace NS_OSBASE::core::ut
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::core::ut::MySubCustomData, strField);
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::core::ut::MyCustomData, intField, subCustomDataField);
