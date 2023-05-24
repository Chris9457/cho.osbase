// osBase package
#include "osCore/Serialization/KeyStream.h"
#include "osCoreImpl_UT/KeyTypes.h"
#include "osCore_UT/ConstantValue.h"
#include "gtest/gtest.h"
#include <fstream>

namespace NS_OSBASE::core::impl::ut {

    class RapidXML_UT : public testing::Test {};
    template <typename T>
    class RapidXMLTyped_UT : public testing::Test {};

    using value_types = testing::Types<bool, int, double, std::wstring>;
    TYPED_TEST_SUITE(RapidXMLTyped_UT, value_types);

    const char *xmlValuesPath    = "resources/xmlvalues.xml";
    const char *xmlBadValuesPath = "resources/xmlbadvalues.xml";
    const char *xmlKeyLevelPath  = "resources/xmlkeylevel.xml";
    const char *xmlMultiKeyPath  = "resources/xmlmultikey.xml";

    const char *xmlKeyLevel = "							\
<?xml version = \"1.0\" encoding = \"UTF-8\" ?>		\
<values>											\
	<level1_1>										\
		<keyBool>0</keyBool>						\
		<keyInt>32</keyInt>							\
	</level1_1>										\
	<level1_2>										\
		<keyDouble>3.4</keyDouble>					\
		<level2_1>									\
			<keyString>test</keyString>				\
		</level2_1>									\
	</level1_2>										\
</values>											\
";

    TEST_F(RapidXML_UT, create) {
        std::ifstream ifs(xmlValuesPath);
        auto const pStream = makeXmlStream(std::ifstream(xmlValuesPath));
        ASSERT_NE(nullptr, pStream);
    }

    TEST_F(RapidXML_UT, openKey_OK_Stream) {
        auto const pStream = makeXmlStream(std::istringstream(xmlKeyLevel));

        ASSERT_FALSE(pStream->openKey("values").isNull());
        ASSERT_FALSE(pStream->openKey("level1_1").isNull());
        ASSERT_TRUE(pStream->openKey("level1_1").isNull());
    }

    TEST_F(RapidXML_UT, openKey_OK_File) {
        auto const pStream = makeXmlStream(std::ifstream(xmlKeyLevelPath));

        ASSERT_FALSE(pStream->openKey("values").isNull());
        ASSERT_FALSE(pStream->openKey("level1_1").isNull());
        ASSERT_TRUE(pStream->openKey("level1_1").isNull());

        // check with a path
        ASSERT_FALSE(pStream->openKeys({ "values", "level1_3[2]" }).isNull());
        ASSERT_FALSE(pStream->openKey("keyString").isNull());
        ASSERT_EQ(L"array3", pStream->getValue(std::wstring()));
    }

    TEST_F(RapidXML_UT, openKey_KO) {
        auto const pStream = makeXmlStream();
        ASSERT_TRUE(pStream->openKey("level1_1").isNull());
    }

    TEST_F(RapidXML_UT, createKey) {
        auto const pStream = makeXmlStream(std::ifstream(xmlKeyLevelPath));

        ASSERT_FALSE(pStream->openKey("values").isNull());
        ASSERT_FALSE(pStream->createKey("level1_1").isNull()); // Existing key
        ASSERT_FALSE(pStream->createKey("level1_1").isNull()); // Non existing key

        // check with a path
        ASSERT_FALSE(pStream->createKeys({ "values", "level1_3[2]" }).isNull()); // Existing key
        ASSERT_FALSE(pStream->openKey("keyString").isNull());
        ASSERT_EQ(L"array3", pStream->getValue(std::wstring()));

        ASSERT_FALSE(pStream->createKeys({ "values", "level1_3[5]" }).isNull()); // Non existing key
    }

    TEST_F(RapidXML_UT, closeKey_OK) {
        auto const pStream = makeXmlStream(std::ifstream(xmlKeyLevelPath));

        pStream->openKey("values");
        pStream->openKey("level1_2");
        pStream->openKey("level2_1");
        ASSERT_TRUE(pStream->closeKey());
        ASSERT_EQ("level1_2", pStream->getCurrentKey().getKey());

        ASSERT_TRUE(pStream->closeKey());
        ASSERT_TRUE(pStream->closeKey());
        ASSERT_FALSE(pStream->closeKey());
    }

    TEST_F(RapidXML_UT, closeKey_KO) {
        auto const pStream = makeXmlStream();
        ASSERT_FALSE(pStream->closeKey());
    }

    TEST_F(RapidXML_UT, getCurrentKey) {
        auto pStream = makeXmlStream(std::ifstream(xmlKeyLevelPath));

        ASSERT_TRUE(pStream->getCurrentKey().getKey().empty());

        pStream->openKey("values");
        pStream->openKey("level1_2");
        ASSERT_EQ("level1_2", pStream->getCurrentKey().getKey());

        pStream->openKey("level2_1");
        ASSERT_EQ("level2_1", pStream->getCurrentKey().getKey());

        pStream->closeKey();
        ASSERT_EQ("level1_2", pStream->getCurrentKey().getKey());

        pStream = makeXmlStream();
        ASSERT_FALSE(pStream->getCurrentKey().isNull());
    }

    TEST_F(RapidXML_UT, firstKey_known_key) {
        auto const pStream = makeXmlStream(std::ifstream(xmlMultiKeyPath));
        const std::string key("level");
        const bool boolDefaultValue  = core::ut::ConstantValue<bool, 0>::getValue();
        const bool expectedBoolValue = KeyType<bool>::getValue();
        const int intDefaultValue    = core::ut::ConstantValue<int, 0>::getValue();
        const int expectedIntValue   = KeyType<int>::getValue();

        pStream->openKey("values");
        ASSERT_FALSE(pStream->firstKey(makeKey(key)).isNull());
        ASSERT_EQ(key, pStream->getCurrentKey().getKey());
        ASSERT_EQ(expectedBoolValue, pStream->getKeyValue(KeyType<bool>::getKey(), boolDefaultValue));
        ASSERT_EQ(expectedIntValue, pStream->getKeyValue(KeyType<int>::getKey(), intDefaultValue));
    }

    TEST_F(RapidXML_UT, firstKey_unknown_key) {
        auto const pStream = makeXmlStream(std::ifstream(xmlMultiKeyPath));
        const std::string key("level");
        const bool boolDefaultValue  = core::ut::ConstantValue<bool, 0>::getValue();
        const bool expectedBoolValue = KeyType<bool>::getValue();
        const int intDefaultValue    = core::ut::ConstantValue<int, 0>::getValue();
        const int expectedIntValue   = KeyType<int>::getValue();

        pStream->openKey("values");
        ASSERT_FALSE(pStream->firstKey(makeKey(std::string())).isNull());
        ASSERT_EQ(key, pStream->getCurrentKey().getKey());
        ASSERT_EQ(expectedBoolValue, pStream->getKeyValue(KeyType<bool>::getKey(), boolDefaultValue));
        ASSERT_EQ(expectedIntValue, pStream->getKeyValue(KeyType<int>::getKey(), intDefaultValue));
    }

    TEST_F(RapidXML_UT, firstKey_KO) {
        auto const pStream = makeXmlStream();
        ASSERT_TRUE(pStream->firstKey(makeKey(std::string())).isNull());
    }

    TEST_F(RapidXML_UT, nextKey_known_key) {
        auto const pStream = makeXmlStream(std::ifstream(xmlMultiKeyPath));
        const std::string key("level");
        const double doubleDefaultValue  = core::ut::ConstantValue<double, 0>::getValue();
        const double expectedDoubleValue = KeyType<double>::getValue();

        pStream->openKey("values");
        pStream->firstKey(makeKey(key));
        ASSERT_FALSE(pStream->nextKey(makeKey(key)).isNull());
        ASSERT_EQ(key, pStream->getCurrentKey().getKey());
        ASSERT_EQ(expectedDoubleValue, pStream->getKeyValue(KeyType<double>::getKey(), doubleDefaultValue));
    }

    TEST_F(RapidXML_UT, nextKey_unknown_key) {
        auto const pStream = makeXmlStream(std::ifstream(xmlMultiKeyPath));
        const std::string key("level");
        const double doubleDefaultValue  = core::ut::ConstantValue<double, 0>::getValue();
        const double expectedDoubleValue = KeyType<double>::getValue();

        pStream->openKey("values");
        pStream->firstKey(makeKey(key));
        ASSERT_FALSE(pStream->nextKey(makeKey(std::string())).isNull());
        ASSERT_EQ(key, pStream->getCurrentKey().getKey());
        ASSERT_EQ(expectedDoubleValue, pStream->getKeyValue(KeyType<double>::getKey(), doubleDefaultValue));
    }

    TEST_F(RapidXML_UT, emptyVector) {
        const std::string key = "array";
        auto const pStream    = makeXmlStream();

        pStream->setKeyValue(key, std::vector<int>{});
        auto const out = pStream->getKeyValue(key, std::vector<int>{});

        ASSERT_TRUE(out.empty());
    }

    TYPED_TEST(RapidXMLTyped_UT, getValue_OK) {
        auto const pStream           = makeXmlStream(std::ifstream(xmlValuesPath));
        const TypeParam defaultValue = core::ut::ConstantValue<TypeParam, 0>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(KeyType<TypeParam>::getValue(), pStream->getKeyValue(KeyType<TypeParam>::getKey(), defaultValue));
    }

    TYPED_TEST(RapidXMLTyped_UT, getValue_KO) {
        auto const pStream           = makeXmlStream(std::ifstream(xmlBadValuesPath));
        const TypeParam defaultValue = core::ut::ConstantValue<TypeParam, 1>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(defaultValue, pStream->getKeyValue(KeyType<TypeParam>::getKey(), defaultValue));
    }

    TEST_F(RapidXML_UT, getValue_void) {
        auto const pStream = makeXmlStream(std::ifstream(xmlValuesPath));

        pStream->openKey("values");
        const std::string key = "keyVoid";
        ASSERT_TRUE(pStream->getKeyValue(key));
    }

    TEST_F(RapidXML_UT, getValue_void_KO) {
        auto const pStream = makeXmlStream(std::ifstream(xmlBadValuesPath));

        pStream->openKey("values");
        const std::string key = "keyVoid";
        ASSERT_FALSE(pStream->getKeyValue(key));
    }

    TYPED_TEST(RapidXMLTyped_UT, setValue_existingTag) {
        auto const pStream           = makeXmlStream(std::ifstream(xmlValuesPath));
        const TypeParam defaultValue = core::ut::ConstantValue<TypeParam, 0>::getValue();
        const TypeParam value        = core::ut::ConstantValue<TypeParam, 1>::getValue();
        pStream->openKey("values");

        const auto key = KeyType<TypeParam>::getKey();
        ASSERT_EQ(KeyType<TypeParam>::getValue(), pStream->getKeyValue(key, defaultValue));
        ASSERT_NE(value, pStream->getKeyValue(key, defaultValue));

        ASSERT_TRUE(pStream->setKeyValue(key, value));
        ASSERT_NE(KeyType<TypeParam>::getValue(), pStream->getKeyValue(key, defaultValue));
        ASSERT_EQ(value, pStream->getKeyValue(key, defaultValue));
    }

    TYPED_TEST(RapidXMLTyped_UT, setValue_nonExistingTag) {
        auto const pStream           = makeXmlStream(std::ifstream(xmlBadValuesPath));
        const TypeParam defaultValue = core::ut::ConstantValue<TypeParam, 1>::getValue();
        const TypeParam value        = core::ut::ConstantValue<TypeParam, 0>::getValue();

        const auto key = KeyType<TypeParam>::getKey();
        ASSERT_EQ(defaultValue, pStream->getKeyValue(key, defaultValue));
        ASSERT_NE(value, pStream->getKeyValue(key, defaultValue));

        ASSERT_TRUE(pStream->setKeyValue(key, value));
        ASSERT_NE(defaultValue, pStream->getKeyValue(key, defaultValue));
        ASSERT_EQ(value, pStream->getKeyValue(key, defaultValue));
    }

    TYPED_TEST(RapidXMLTyped_UT, getValues_OK) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeXmlStream(std::ifstream(xmlValuesPath));
        const TypeParams defaultValue = core::ut::ConstantValue<TypeParam[], 0>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(KeyType<TypeParam>::getValues(), pStream->getKeyValue(KeyType<TypeParam>::getKeys(), defaultValue));
    }

    TYPED_TEST(RapidXMLTyped_UT, getValues_KO) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeXmlStream(std::ifstream(xmlBadValuesPath));
        const TypeParams defaultValue = core::ut::ConstantValue<TypeParam[], 1>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(defaultValue, pStream->getKeyValue(KeyType<TypeParam>::getKeys(), defaultValue));
    }

    TYPED_TEST(RapidXMLTyped_UT, setValues_existingTag) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeXmlStream(std::ifstream(xmlValuesPath));
        const TypeParams defaultValue = core::ut::ConstantValue<TypeParam[], 0>::getValue();
        const TypeParams value        = core::ut::ConstantValue<TypeParam[], 1>::getValue();
        pStream->openKey("values");

        const auto key = KeyType<TypeParam>::getKeys();
        ASSERT_EQ(KeyType<TypeParam>::getValues(), pStream->getKeyValue(key, defaultValue));
        ASSERT_NE(value, pStream->getKeyValue(key, defaultValue));

        ASSERT_TRUE(pStream->setKeyValue(key, value));
        ASSERT_NE(KeyType<TypeParam>::getValues(), pStream->getKeyValue(key, defaultValue));
        ASSERT_EQ(value, pStream->getKeyValue(key, defaultValue));
    }

    TYPED_TEST(RapidXMLTyped_UT, setValues_nonExistingTag) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeXmlStream(std::ifstream(xmlBadValuesPath));
        const TypeParams defaultValue = core::ut::ConstantValue<TypeParam[], 1>::getValue();
        const TypeParams value        = core::ut::ConstantValue<TypeParam[], 0>::getValue();
        pStream->openKey("values");

        const auto key = KeyType<TypeParam>::getKeys();
        ASSERT_EQ(defaultValue, pStream->getKeyValue(key, defaultValue));
        ASSERT_NE(value, pStream->getKeyValue(key, defaultValue));

        ASSERT_TRUE(pStream->setKeyValue(key, value));
        ASSERT_NE(defaultValue, pStream->getKeyValue(key, defaultValue));
        ASSERT_EQ(value, pStream->getKeyValue(key, defaultValue));
    }
} // namespace NS_OSBASE::core::impl::ut
