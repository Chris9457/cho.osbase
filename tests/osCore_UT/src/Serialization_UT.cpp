// \brief Unit tests of the module Serialization

#include "osCore/Serialization/Serializer.h"
#include "osCore_UT/ConstantValue.h"
#include <sstream>
#include "gtest/gtest.h"

OS_SERIALIZE_STRUCT(cho::osbase::core::ut::TestStruct, st, i, d, v)
OS_SERIALIZE_STRUCT(cho::osbase::core::ut::ComplexTestStruct, d, t, e, s)

namespace cho::osbase::core::ut {
    struct TestNoField {};
} // namespace cho::osbase::core::ut
OS_SERIALIZE_STRUCT(cho::osbase::core::ut::TestNoField)

namespace cho::osbase::core::ut {

    template <typename>
    class Serialization_UT : public testing::Test {};

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
        std::vector<short>, // empty vector
        std::array<int, 5>,
        TestEnum,
        TestStruct,
        ComplexTestStruct>;
    TYPED_TEST_SUITE(Serialization_UT, value_types);

    TYPED_TEST(Serialization_UT, single_testGetSetValueOK) {
        auto const in = makeBinaryValue(ConstantValue<TypeParam, 1>::getValue());
        auto out      = makeBinaryValue<TypeParam>();

        std::stringstream ss;
        ss << in;
        ss >> out;

        ASSERT_EQ(in, out);
    }
} // namespace cho::osbase::core::ut