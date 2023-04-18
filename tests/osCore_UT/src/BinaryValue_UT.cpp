// \brief Unit test of the class BinaryValue

#include "osCore/Serialization/BinaryValue.h"
#include "osCore_UT/ConstantValue.h"
#include "gtest/gtest.h"

namespace cho::osbase::core::ut {
    template <typename T>
    class BinaryValue_UT : public testing::Test {};

    using value_types = testing::Types<bool,
        char,
        short,
        unsigned short,
        int,
        float,
        double,
        std::string,
        std::wstring,
        std::vector<int>,
        TestEnum,
        TestStruct,
        ComplexTestStruct>;
    TYPED_TEST_SUITE(BinaryValue_UT, value_types);

    TYPED_TEST(BinaryValue_UT, ctor) {
        { // default
            BinaryValue<TypeParam> bv;
            ASSERT_EQ(TypeParam{}, bv.get());
        }

        { // copy value
            auto const v = ConstantValue<TypeParam, 1>::getValue();
            BinaryValue<TypeParam> bv(v);
            ASSERT_EQ(v, bv.get());
        }

        { // move value
            auto const expectedValue = ConstantValue<TypeParam, 1>::getValue();
            auto value               = expectedValue;
            BinaryValue<TypeParam> bv(std::move(value));
            ASSERT_EQ(expectedValue, bv.get());
        }
    }

    TYPED_TEST(BinaryValue_UT, getset) {
        auto const expectedValue = ConstantValue<TypeParam, 1>::getValue();

        { // get non const
            BinaryValue<TypeParam> bv;
            bv.get() = expectedValue;
            ASSERT_EQ(expectedValue, bv.get());
        }

        { // get const
            BinaryValue<TypeParam> bv(expectedValue);
            ASSERT_EQ(expectedValue, bv.get());
        }

        { // set copy
            BinaryValue<TypeParam> bv;
            bv.set(expectedValue);
            ASSERT_EQ(expectedValue, bv.get());
        }

        { // set move
            BinaryValue<TypeParam> bv;
            auto value = expectedValue;
            bv.set(std::move(value));
            ASSERT_EQ(expectedValue, bv.get());
        }
    }

    TYPED_TEST(BinaryValue_UT, comparisonEqual) {
        auto const bv1 = BinaryValue<TypeParam>{ ConstantValue<TypeParam, 0>::getValue() };
        auto const bv2 = BinaryValue<TypeParam>{ ConstantValue<TypeParam, 1>::getValue() };

        ASSERT_TRUE(bv1 == bv1);
        ASSERT_FALSE(bv1 == bv2);
    }

    TYPED_TEST(BinaryValue_UT, comparisonNotEqual) {
        auto const bv1 = BinaryValue<TypeParam>{ ConstantValue<TypeParam, 0>::getValue() };
        auto const bv2 = BinaryValue<TypeParam>{ ConstantValue<TypeParam, 1>::getValue() };

        ASSERT_FALSE(bv1 != bv1);
        ASSERT_TRUE(bv1 != bv2);
    }
} // namespace cho::osbase::core::ut
