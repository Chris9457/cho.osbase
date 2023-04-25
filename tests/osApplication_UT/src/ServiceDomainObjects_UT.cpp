// \brief Test of domain objects

#include "BaseService_UT.h"
#include "TestDomainObjects.h"
#include "osApplication/ServiceException.h"

namespace nsut       = NS_OSBASE::application::ut;
namespace nsdo       = nsut::domain_objects;
namespace nsdoimport = nsut::testdomainobjectsimport;

namespace NS_OSBASE::application::ut {

    class ServiceDomainObjects_UT : public BaseService_UT {};

    TEST_F(ServiceDomainObjects_UT, checkBasicTypes) {
        auto constexpr expectedCharType = std::is_same_v<char, decltype(nsdo::TestAllBasicTypes::_char)>;
        ASSERT_TRUE(expectedCharType);

        auto constexpr expectedUnsignedCharType = std::is_same_v<unsigned char, decltype(nsdo::TestAllBasicTypes::_unsigned_char)>;
        ASSERT_TRUE(expectedUnsignedCharType);

        auto constexpr expectedShortType = std::is_same_v<short, decltype(nsdo::TestAllBasicTypes::_short_integer)>;
        ASSERT_TRUE(expectedShortType);

        auto constexpr expectedUnsignedShortType =
            std::is_same_v<unsigned short, decltype(nsdo::TestAllBasicTypes::_unsigned_short_integer)>;
        ASSERT_TRUE(expectedUnsignedShortType);

        auto constexpr expectedIntType = std::is_same_v<int, decltype(nsdo::TestAllBasicTypes::_integer)>;
        ASSERT_TRUE(expectedIntType);

        auto constexpr expectedUnsignedIntType = std::is_same_v<unsigned int, decltype(nsdo::TestAllBasicTypes::_unsigned_integer)>;
        ASSERT_TRUE(expectedUnsignedIntType);

        auto constexpr expectedlonglongType = std::is_same_v<long long, decltype(nsdo::TestAllBasicTypes::_long_integer)>;
        ASSERT_TRUE(expectedlonglongType);

        auto constexpr expectedUnsignedLongLongType =
            std::is_same_v<unsigned long long, decltype(nsdo::TestAllBasicTypes::_unsigned_long_integer)>;
        ASSERT_TRUE(expectedUnsignedLongLongType);

        auto constexpr expectedFloatType = std::is_same_v<float, decltype(nsdo::TestAllBasicTypes::_short_floating)>;
        ASSERT_TRUE(expectedFloatType);

        auto constexpr expectedDoubleType = std::is_same_v<double, decltype(nsdo::TestAllBasicTypes::_floating)>;
        ASSERT_TRUE(expectedDoubleType);

        auto constexpr expectedStringType = std::is_same_v<std::string, decltype(nsdo::TestAllBasicTypes::_string)>;
        ASSERT_TRUE(expectedStringType);

        auto constexpr expectedBoolType = std::is_same_v<bool, decltype(nsdo::TestAllBasicTypes::_boolean)>;
        ASSERT_TRUE(expectedBoolType);

        auto constexpr expectedUriType = std::is_same_v<data::Uri, decltype(nsdo::TestAllBasicTypes::_uri)>;
        ASSERT_TRUE(expectedUriType);
    }

    TEST_F(ServiceDomainObjects_UT, checkEnum) {
        auto constexpr testEnum1 = nsdo::TestEnum::value1;
        ASSERT_EQ(static_cast<nsdo::TestEnum>(12), testEnum1);

        auto constexpr testEnum2 = nsdo::TestEnum::value2;
        ASSERT_EQ(static_cast<nsdo::TestEnum>(54), testEnum2);
    }

    TEST_F(ServiceDomainObjects_UT, checkEnumConversion) {
        ASSERT_EQ("value1", type_cast<std::string>(nsdo::TestEnum::value1));
        ASSERT_EQ("value2", type_cast<std::string>(nsdo::TestEnum::value2));
        ASSERT_THROW(type_cast<std::string>(static_cast<nsdo::TestEnum>(0)), application::ServiceException);

        // check for imports
        ASSERT_EQ("field1", type_cast<std::string>(nsdoimport::EnumForConvert::field1));
        ASSERT_EQ("field2", type_cast<std::string>(nsdoimport::EnumForConvert::field2));
    }

    TEST_F(ServiceDomainObjects_UT, checkStruct) {
        nsdo::TestStruct testStruct;

        ASSERT_EQ(5, testStruct.basic);
        auto constexpr expectedBasicType = std::is_same_v<char, decltype(testStruct.basic)>;
        ASSERT_TRUE(expectedBasicType);
        ASSERT_EQ(5, testStruct.basic);

        const std::vector<bool> expectedNotFixedCollection = { true, false };
        auto constexpr expectedNotFixedCollectionType      = std::is_same_v<std::vector<bool>, decltype(testStruct.notFixedCollection)>;
        ASSERT_TRUE(expectedNotFixedCollectionType);
        ASSERT_EQ(expectedNotFixedCollection, testStruct.notFixedCollection);

        const std::array<int, 4> expectedFixedCollection = { 0, 1, 2, 3 };
        auto constexpr expectedFixedCollectionType       = std::is_same_v<std::array<int, 4>, decltype(testStruct.fixedCollection)>;
        ASSERT_TRUE(expectedFixedCollectionType);
        ASSERT_EQ(expectedFixedCollection, testStruct.fixedCollection);

        const std::vector<double> asyncValues = { 0.1, 0.6, 5.6 };
        testStruct.asyncData                  = data::makeAsyncData<std::vector<double>>();
        testStruct.asyncData.set(asyncValues);
        auto clientAsyncData = testStruct.asyncData;
        ASSERT_TRUE(clientAsyncData.tryConnectFor(getTimeout(100)));
        ASSERT_EQ(asyncValues, clientAsyncData.get());

        const std::array<std::string, 2> asyncPagedValues = { "toto", "titi" };
        testStruct.asyncPagedData                         = data::makeAsyncPagedData<std::array<std::string, 2>>();
        testStruct.asyncPagedData.set(asyncPagedValues);
        auto clientAsyncPagedData = testStruct.asyncPagedData;
        ASSERT_TRUE(clientAsyncPagedData.tryConnectFor(getTimeout(100)));
        ASSERT_EQ(asyncPagedValues, clientAsyncPagedData.get());

        // Verify import
        auto const expectedImportType = std::is_same_v<testdomainobjectsimport::Vectf4, decltype(testStruct.vectf4)>;
        ASSERT_TRUE(expectedImportType);
    }

    TEST_F(ServiceDomainObjects_UT, checkAlias) {
        auto constexpr expectedBasicType = std::is_same_v<unsigned short, nsdo::TestAliasBasic>;
        ASSERT_TRUE(expectedBasicType);

        auto constexpr expectedNotFixedCollexctionType =
            std::is_same_v<std::vector<nsdo::TestAliasBasic>, nsdo::TestAliasCollectionNotFixed>;
        ASSERT_TRUE(expectedNotFixedCollexctionType);

        auto constexpr expectedFixedCollexctionType = std::is_same_v<std::array<nsdo::TestAliasBasic, 5>, nsdo::TestAliasCollectionFixed>;
        ASSERT_TRUE(expectedFixedCollexctionType);

        auto constexpr expectedAsyncDataType = std::is_same_v<data::AsyncData<nsdo::TestAliasCollectionNotFixed>, nsdo::TestAliasAsyncData>;
        ASSERT_TRUE(expectedAsyncDataType);

        auto constexpr expectedAsyncPagedDataType =
            std::is_same_v<data::AsyncPagedData<nsdo::TestAliasCollectionFixed>, nsdo::TestAliasAsyncPagedData>;
        ASSERT_TRUE(expectedAsyncPagedDataType);
    }

} // namespace NS_OSBASE::application::ut
