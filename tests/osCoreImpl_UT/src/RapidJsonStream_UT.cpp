// \brief Declaration of the unit tests for JSon stream
#include "osCore/Serialization/KeyStream.h"
#include "osCoreImpl_UT/KeyTypes.h"
#include "osCore_UT/ConstantValue.h"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>

namespace cho::osbase::core::impl::ut {

    class RapidJSON_UT : public testing::Test {};
    template <typename T>
    class RapidJSONTyped_UT : public testing::Test {};

    using value_types = testing::Types<bool, int, double, std::wstring>;
    TYPED_TEST_SUITE(RapidJSONTyped_UT, value_types);

    const char *jsonValuesPath    = "resources/jsonvalues.json";
    const char *jsonBadValuesPath = "resources/jsonbadvalues.json";
    const char *jsonKeyLevelPath  = "resources/jsonkeylevel.json";
    const char *jsonMultiKeyPath  = "resources/jsonmultikey.json";

    const char *jsonKeyLevel = "				\
	{										\
		\"values\":{						\
			\"level1_1\":{					\
				\"keyBool\":false,			\
				\"keyInt\" : 32				\
			},								\
			\"level1_2\" : {				\
				\"keyDouble\":3.4,			\
				\"level2_1\" : {			\
					\"keyString\":\"test\"	\
				}							\
			}								\
		}									\
	}										\
";

    void printDir(const std::string &path) {
        for (auto &&entry : std::filesystem::directory_iterator(path)) {
            const std::string subPath = entry.path().relative_path().string();
            std::cout << subPath << std::endl;
            if (entry.is_directory())
                printDir(subPath);
        }
    }

    TEST_F(RapidJSON_UT, create) {
        printDir("resources");
        auto const pStream = makeJsonStream(std::ifstream(jsonValuesPath));
        ASSERT_NE(nullptr, pStream);
    }

    TEST_F(RapidJSON_UT, openKey_OK_Stream) {
        auto const pStream = makeJsonStream(std::istringstream(jsonKeyLevel));

        ASSERT_FALSE(pStream->openKey("values").isNull());
        ASSERT_FALSE(pStream->openKey("level1_1").isNull());
        ASSERT_TRUE(pStream->openKey("level1_1").isNull());
    }

    TEST_F(RapidJSON_UT, openKey_OK_File) {
        auto const pStream = makeJsonStream(std::ifstream(jsonKeyLevelPath));

        ASSERT_FALSE(pStream->openKey("values").isNull());
        ASSERT_FALSE(pStream->openKey("level1_1").isNull());
        ASSERT_TRUE(pStream->openKey("level1_1").isNull());

        // check with a path
        ASSERT_FALSE(pStream->openKeys({ "values", "level1_3[2]" }).isNull());
        ASSERT_FALSE(pStream->openKey("keyString").isNull());
        ASSERT_EQ(L"array3", pStream->getValue(std::wstring()));
    }

    TEST_F(RapidJSON_UT, openKey_KO) {
        auto const pStream = makeJsonStream();
        ASSERT_TRUE(pStream->openKey("level1_1").isNull());
    }

    TEST_F(RapidJSON_UT, createKey) {
        auto const pStream = makeJsonStream(std::ifstream(jsonKeyLevelPath));

        ASSERT_FALSE(pStream->openKey("values").isNull());
        ASSERT_FALSE(pStream->createKey("level1_1").isNull()); // Existing key
        ASSERT_FALSE(pStream->createKey("level1_1").isNull()); // Non existing key
        ASSERT_TRUE(pStream->closeKey());
        ASSERT_FALSE(pStream->openKey("level1_1").isNull()); // Existing key

        // check with a path
        ASSERT_FALSE(pStream->createKeys({ "values", "level1_3[2]" }).isNull()); // Existing key
        ASSERT_FALSE(pStream->openKey("keyString").isNull());
        ASSERT_EQ(L"array3", pStream->getValue(std::wstring()));

        ASSERT_FALSE(pStream->createKeys({ "values", "level1_3[5]" }).isNull()); // Non existing key
    }

    TEST_F(RapidJSON_UT, closeKey_OK) {
        auto const pStream = makeJsonStream(std::ifstream(jsonKeyLevelPath));

        pStream->openKey("values");
        pStream->openKey("level1_2");
        pStream->openKey("level2_1");
        ASSERT_TRUE(pStream->closeKey());
        ASSERT_EQ("level1_2", pStream->getCurrentKey().getKey());

        ASSERT_TRUE(pStream->closeKey());
        ASSERT_TRUE(pStream->closeKey());
        ASSERT_FALSE(pStream->closeKey());
    }

    TEST_F(RapidJSON_UT, closeKey_KO) {
        auto const pStream = makeJsonStream();
        ASSERT_FALSE(pStream->closeKey());
    }

    TEST_F(RapidJSON_UT, getCurrentKey) {
        auto pStream = makeJsonStream(std::ifstream(jsonKeyLevelPath));

        ASSERT_TRUE(pStream->getCurrentKey().getKey().empty());

        pStream->openKey("values");
        pStream->openKey("level1_2");
        ASSERT_EQ("level1_2", pStream->getCurrentKey().getKey());

        pStream->openKey("level2_1");
        ASSERT_EQ("level2_1", pStream->getCurrentKey().getKey());

        pStream->closeKey();
        ASSERT_EQ("level1_2", pStream->getCurrentKey().getKey());

        pStream = makeJsonStream();
        ASSERT_FALSE(pStream->getCurrentKey().isNull());
    }

    TEST_F(RapidJSON_UT, firstKey_known_key) {
        auto const pStream = makeJsonStream(std::ifstream(jsonKeyLevelPath));
        const std::string key("level1_1");
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

    TEST_F(RapidJSON_UT, firstKey_unknown_key) {
        auto const pStream = makeJsonStream(std::ifstream(jsonKeyLevelPath));
        const std::string key("level1_1");
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

    TEST_F(RapidJSON_UT, firstKey_KO) {
        auto const pStream = makeJsonStream();
        ASSERT_TRUE(pStream->firstKey(makeKey(std::string())).isNull());
    }

    TEST_F(RapidJSON_UT, nextKey_unknown_key) {
        auto const pStream = makeJsonStream(std::ifstream(jsonKeyLevelPath));
        const std::string key1("level1_1");
        const std::string key2("level1_2");
        const double doubleDefaultValue  = core::ut::ConstantValue<double, 0>::getValue();
        const double expectedDoubleValue = KeyType<double>::getValue();

        pStream->openKey("values");
        pStream->firstKey(makeKey(key1));
        ASSERT_FALSE(pStream->nextKey(makeKey(std::string())).isNull());
        ASSERT_EQ(key2, pStream->getCurrentKey().getKey());
        ASSERT_EQ(expectedDoubleValue, pStream->getKeyValue(KeyType<double>::getKey(), doubleDefaultValue));
    }

    TEST_F(RapidJSON_UT, emptyVector) {
        const std::string key = "array";
        auto const pStream    = makeJsonStream();

        pStream->setKeyValue(key, std::vector<int>{});
        auto const out = pStream->getKeyValue(key, std::vector<int>{});

        ASSERT_TRUE(out.empty());
    }

    TYPED_TEST(RapidJSONTyped_UT, getValue_OK) {
        auto const pStream           = makeJsonStream(std::ifstream(jsonValuesPath));
        const TypeParam defaultValue = core::ut::ConstantValue<TypeParam, 0>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(KeyType<TypeParam>::getValue(), pStream->getKeyValue(KeyType<TypeParam>::getKey(), defaultValue));
    }

    TEST_F(RapidJSON_UT, checkDoubleAsInt) {
        auto const pStream        = makeJsonStream(std::ifstream(jsonValuesPath));
        const double defaultValue = core::ut::ConstantValue<double, 0>::getValue();

        pStream->openKey("values");
        const double expectedValue = 3.;
        const std::string key      = "keyDoubleAsInt";
        ASSERT_EQ(expectedValue, pStream->getKeyValue(key, defaultValue));
    }

    TYPED_TEST(RapidJSONTyped_UT, getValue_KO) {
        auto const pStream           = makeJsonStream(std::ifstream(jsonBadValuesPath));
        const TypeParam defaultValue = core::ut::ConstantValue<TypeParam, 1>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(defaultValue, pStream->getKeyValue(KeyType<TypeParam>::getKey(), defaultValue));
    }

    TYPED_TEST(RapidJSONTyped_UT, setValue_existingTag) {
        auto const pStream           = makeJsonStream(std::ifstream(jsonValuesPath));
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

    TYPED_TEST(RapidJSONTyped_UT, setValue_nonExistingTag) {
        auto const pStream           = makeJsonStream(std::ifstream(jsonBadValuesPath));
        const TypeParam defaultValue = core::ut::ConstantValue<TypeParam, 1>::getValue();
        const TypeParam value        = core::ut::ConstantValue<TypeParam, 0>::getValue();
        pStream->openKey("values");

        const auto key = KeyType<TypeParam>::getKey();
        ASSERT_EQ(defaultValue, pStream->getKeyValue(key, defaultValue));
        ASSERT_NE(value, pStream->getKeyValue(key, defaultValue));

        ASSERT_TRUE(pStream->setKeyValue(key, value));
        ASSERT_NE(defaultValue, pStream->getKeyValue(key, defaultValue));
        ASSERT_EQ(value, pStream->getKeyValue(key, defaultValue));
    }

    TYPED_TEST(RapidJSONTyped_UT, getValues_OK) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeJsonStream(std::ifstream(jsonValuesPath));
        const TypeParams defaultValue = core::ut::ConstantValue<TypeParam[], 0>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(KeyType<TypeParam>::getValues(), pStream->getKeyValue(KeyType<TypeParam>::getKeys(), defaultValue));
    }

    TYPED_TEST(RapidJSONTyped_UT, getValues_KO) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeJsonStream(std::ifstream(jsonBadValuesPath));
        const TypeParams defaultValue = core::ut::ConstantValue<TypeParam[], 1>::getValue();

        pStream->openKey("values");
        ASSERT_EQ(defaultValue, pStream->getKeyValue(KeyType<TypeParam>::getKeys(), defaultValue));
    }

    TYPED_TEST(RapidJSONTyped_UT, setValues_existingTag) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeJsonStream(std::ifstream(jsonValuesPath));
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

    TYPED_TEST(RapidJSONTyped_UT, setValues_nonExistingTag) {
        using TypeParams              = std::vector<TypeParam>;
        auto const pStream            = makeJsonStream(std::ifstream(jsonBadValuesPath));
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
} // namespace cho::osbase::core::impl::ut
