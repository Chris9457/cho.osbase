// \brief Implementation tests of IDataExchange

#include "osData/Uri.h"
#include "gtest/gtest.h"

namespace cho::osbase::data::ut {

    class Uri_UT : public testing::Test {};

    TEST_F(Uri_UT, stringToUriOK) {
        { // full url
            const std::string strUrl = "https://user:xxx@example.com:8080/over/there?name=ferret#here";
            auto const uri           = type_cast<Uri>(strUrl);

            ASSERT_EQ("https", uri.scheme);
            ASSERT_TRUE(uri.authority);
            ASSERT_TRUE(uri.authority->userInfo);
            ASSERT_EQ("user:xxx", uri.authority->userInfo);
            ASSERT_EQ("example.com", uri.authority->host);
            ASSERT_TRUE(uri.authority->port);
            ASSERT_EQ(8080, uri.authority->port);
            ASSERT_TRUE(uri.path);
            ASSERT_EQ("/over/there", uri.path);
            ASSERT_TRUE(uri.query);
            ASSERT_EQ("name=ferret", uri.query);
            ASSERT_TRUE(uri.fragment);
            ASSERT_EQ("here", uri.fragment);
        }

        { // host with ip address
            const std::string strUrl = "https://127.0.0.1:8080/here";
            auto const uri           = type_cast<Uri>(strUrl);

            ASSERT_EQ("https", uri.scheme);
            ASSERT_TRUE(uri.authority);
            ASSERT_FALSE(uri.authority->userInfo);
            ASSERT_EQ("127.0.0.1", uri.authority->host);
            ASSERT_TRUE(uri.authority->port);
            ASSERT_EQ(8080, uri.authority->port);
            ASSERT_TRUE(uri.path);
            ASSERT_EQ("/here", uri.path);
            ASSERT_FALSE(uri.query);
            ASSERT_FALSE(uri.fragment);
        }

        { // sheme file
            const std::string strUrl = "file:C:/over/there?name=ferret";
            auto const uri           = type_cast<Uri>(strUrl);

            ASSERT_EQ("file", uri.scheme);
            ASSERT_FALSE(uri.authority);
            ASSERT_TRUE(uri.path);
            ASSERT_EQ("C:/over/there", uri.path);
            ASSERT_TRUE(uri.query);
            ASSERT_EQ("name=ferret", uri.query);
            ASSERT_FALSE(uri.fragment);
        }

        { // Authority missing but sheme file
            const std::string strUrl = "file:///over/there?name=ferret";
            auto const uri           = type_cast<Uri>(strUrl);

            ASSERT_EQ("file", uri.scheme);
            ASSERT_TRUE(uri.authority);
            ASSERT_FALSE(uri.authority->userInfo);
            ASSERT_TRUE(uri.authority->host.empty());
            ASSERT_FALSE(uri.authority->port);
            ASSERT_TRUE(uri.path);
            ASSERT_EQ("/over/there", uri.path);
            ASSERT_TRUE(uri.query);
            ASSERT_EQ("name=ferret", uri.query);
            ASSERT_FALSE(uri.fragment);
        }
    }

    TEST_F(Uri_UT, stringToUriKO) {
        { // bad scheme
            const std::string strUrl = "https//example.com/over/there?name=ferret";
            ASSERT_THROW(type_cast<Uri>(strUrl), BadUriException);
        }

        { // No authority && not file scheme
            const std::string strUrl = "https:///over/there?name=ferret";
            ASSERT_THROW(type_cast<Uri>(strUrl), BadUriException);
        }

        { // not file scheme and direct path
            const std::string strUrl = "https:/over/there?name=ferret";
            ASSERT_THROW(type_cast<Uri>(strUrl), BadUriException);
        }
    }

    TEST_F(Uri_UT, UriToString) {
        { // full url
            const Uri uri{ "https", Uri::Authority{ "user:xxx", "example.com", 8080 }, "/over/there", "name=ferret" };
            const std::string expectedStrUrl = "https://user:xxx@example.com:8080/over/there?name=ferret";
            ASSERT_EQ(expectedStrUrl, type_cast<std::string>(uri));
        }

        { // sheme + host
            const Uri uri{ "https", Uri::Authority{ {}, "example.com", {} }, {}, {} };
            const std::string expectedStrUrl = "https://example.com";
            ASSERT_EQ(expectedStrUrl, type_cast<std::string>(uri));
        }

        { // sheme + host + port
            const Uri uri{ "https", Uri::Authority{ {}, "example.com", 8080 }, {}, {} };
            const std::string expectedStrUrl = "https://example.com:8080";
            ASSERT_EQ(expectedStrUrl, type_cast<std::string>(uri));
        }
    }

    TEST_F(Uri_UT, pathToUri) {
        const std::filesystem::path path = "c:/temp.result.txt";
        auto const uri                   = type_cast<Uri>(path);

        ASSERT_EQ(Uri::schemeFile(), uri.scheme);
        ASSERT_TRUE(uri.path);
        ASSERT_EQ(path.u8string(), uri.path);
    }

    TEST_F(Uri_UT, uriToPathOK) {
        const Uri uri{ Uri::schemeFile(), {}, "c:/temp/result.txt", {} };
        const std::filesystem::path expectedPath{ "c:/temp/result.txt" };
        ASSERT_EQ(expectedPath, type_cast<std::filesystem::path>(uri));
    }

    TEST_F(Uri_UT, uriToPathKO) {
        { // bad scheme
            const Uri uri{ "http", {}, "c:/temp/result.txt", {} };
            ASSERT_THROW(type_cast<std::filesystem::path>(uri), BadUriException);
        }

        { // no path
            const Uri uri{ "file", {}, {}, {} };
            ASSERT_THROW(type_cast<std::filesystem::path>(uri), BadUriException);
        }
    }

    TEST_F(Uri_UT, isValidOK) {
        {
            const std::string strUrl = "https://user:xxx@example.com:8080/over/there?name=ferret#here";
            ASSERT_TRUE(Uri::isValid(strUrl));
        }

        { // host with ip address
            const std::string strUrl = "https://127.0.0.1:8080/here";
            ASSERT_TRUE(Uri::isValid(strUrl));
        }

        { // sheme file
            const std::string strUrl = "file:/over/there?name=ferret";
            ASSERT_TRUE(Uri::isValid(strUrl));
        }

        { // Authority missing but sheme file
            const std::string strUrl = "file:///over/there?name=ferret";
            ASSERT_TRUE(Uri::isValid(strUrl));
        }
    }

    TEST_F(Uri_UT, isValidKO) {
        { // bad scheme
            const std::string strUrl = "https//example.com/over/there?name=ferret";
            ASSERT_FALSE(Uri::isValid(strUrl));
        }

        { // No authority && not file scheme
            const std::string strUrl = "https:///over/there?name=ferret";
            ASSERT_FALSE(Uri::isValid(strUrl));
        }

        { // not file scheme and direct path
            const std::string strUrl = "https:/over/there?name=ferret";
            ASSERT_FALSE(Uri::isValid(strUrl));
        }
    }

    TEST_F(Uri_UT, operatorEqual) {
        auto const uri1  = type_cast<Uri>(std::string("https://user:xxx@example.com:8080/over/there?name=ferret#here"));
        auto const uri11 = uri1;
        auto const uri2  = type_cast<Uri>(std::string("http://user:xxx@example.com:8080/over/there?name=ferret#here"));

        ASSERT_TRUE(uri1 == uri11);
        ASSERT_EQ(uri1, uri11);
        ASSERT_FALSE(uri1 == uri2);
    }

    TEST_F(Uri_UT, operatorNotEqual) {
        auto const uri1  = type_cast<Uri>(std::string("https://user:xxx@example.com:8080/over/there?name=ferret#here"));
        auto const uri11 = uri1;
        auto const uri2  = type_cast<Uri>(std::string("http://user:xxx@example.com:8080/over/there?name=ferret#here"));

        ASSERT_FALSE(uri1 != uri11);
        ASSERT_TRUE(uri1 != uri2);
        ASSERT_NE(uri1, uri2);
    }

    TEST_F(Uri_UT, serialization_OK) {
        const std::string key   = "test";
        auto const factoryNames = core::getKeyStreamFamilies();
        ASSERT_LT(0, factoryNames.size());

        for (auto &&factoryName : factoryNames) {
            auto pStream   = core::makeKeyStream(factoryName);
            auto const uri = type_cast<Uri>(std::string("http://localhost:8080"));

            *pStream << core::makeKeyValue(key, uri);
            std::stringstream oss;
            oss << *pStream;

            auto keyValue = core::makeKeyValue<Uri>(key);
            *pStream >> keyValue;
            ASSERT_EQ(uri, keyValue.getValue());
        }
    }

    TEST_F(Uri_UT, serialization_KO) {
        const std::string key   = "test";
        auto const factoryNames = core::getKeyStreamFamilies();
        ASSERT_LT(0, factoryNames.size());

        for (auto &&factoryName : factoryNames) {
            auto pStream      = core::makeKeyStream(factoryName);
            auto const strUri = "http:://localhost:8080";

            *pStream << core::makeKeyValue(key, strUri);
            std::stringstream oss;
            oss << *pStream;

            auto keyValue = core::makeKeyValue<Uri>(key);
            *pStream >> keyValue;
            ASSERT_NE(strUri, type_cast<std::string>(keyValue.getValue()));
        }
    }

    TEST_F(Uri_UT, hash) {
        const std::string strUri = "http://127.0.0.1:8080";
        auto const uri           = type_cast<Uri>(strUri);

        ASSERT_EQ(std::hash<std::string>()(strUri), std::hash<Uri>()(uri));

        // check compilation
        std::unordered_map<Uri, std::string>();
    }

    TEST_F(Uri_UT, less) {
        const std::string strUri1 = "http://localhost:8080";
        const std::string strUri2 = "http://localhost:8081";
        auto const uri1           = type_cast<Uri>(strUri1);
        auto const uri2           = type_cast<Uri>(strUri2);

        ASSERT_EQ(std::less<>()(strUri1, strUri1), std::less<Uri>()(uri1, uri1));
        ASSERT_EQ(std::less<>()(strUri1, strUri2), std::less<Uri>()(uri1, uri2));
        ASSERT_EQ(std::less<>()(strUri2, strUri1), std::less<Uri>()(uri2, uri1));

        // check compilation
        std::map<Uri, std::string>();
    }

    TEST_F(Uri_UT, streamOperator) {
        const std::string strUri = "http://127.0.0.1:8080";
        auto const uri           = type_cast<Uri>(strUri);

        std::ostringstream os;
        os << uri;
        ASSERT_EQ(strUri, os.str());
    }

    TEST_F(Uri_UT, check_local_host) {
        ASSERT_TRUE(Uri::Host("127.0.0.1").isLocal());
        ASSERT_TRUE(Uri::Host("localhost").isLocal());
        ASSERT_FALSE(Uri::Host("127.0.0.2").isLocal());
    }

    TEST_F(Uri_UT, checkHost) {
        ASSERT_EQ("127.0.0.1", Uri::Host("localhost"));
        ASSERT_EQ("127.0.0.1", Uri::Host("127.0.0.1"));
        ASSERT_EQ("192.168.0.1", Uri::Host("192.168.0.1"));
    }
} // namespace cho::osbase::data::ut
