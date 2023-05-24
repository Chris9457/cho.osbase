// \brief Unit tests of the module Serialization

#include "osCore/Serialization/Serializer.h"
#include "osCore_UT/ConstantValue.h"
#include <sstream>
#include "gtest/gtest.h"

OS_SERIALIZE_STRUCT(NS_OSBASE::core::ut::TestStruct, st, i, d, v)
OS_SERIALIZE_STRUCT(NS_OSBASE::core::ut::ComplexTestStruct, d, t, e, s)

namespace NS_OSBASE::core::ut {
    struct TestNoField {};
} // namespace NS_OSBASE::core::ut
OS_SERIALIZE_STRUCT(NS_OSBASE::core::ut::TestNoField)

namespace NS_OSBASE::core::ut {

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
        ComplexTestStruct,
        std::optional<int>,          // optional with value
        std::optional<std::string>>; // optional without value
    TYPED_TEST_SUITE(Serialization_UT, value_types);

    TYPED_TEST(Serialization_UT, single_testGetSetValueOK) {
        auto const in = makeBinaryValue(ConstantValue<TypeParam, 1>::getValue());
        auto out      = makeBinaryValue<TypeParam>();

        std::stringstream ss;
        ss << in;
        ss >> out;

        ASSERT_EQ(in, out);
    }
} // namespace NS_OSBASE::core::ut
