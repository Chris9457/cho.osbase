// \brief Unit test of the class design pattern AbstractFactory

#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore_UT/BaseProduct.h"
#include "gtest/gtest.h"
#include <memory>

namespace NS_OSBASE::core::ut {

    class AbstractFactory_UT : public testing::Test {};

    class ConcreteProduct1 : public BaseProduct {
    public:
        ConcreteProduct1(int i) : m_i(i) {
        }
        ConcreteProduct1(const std::string &str) : m_str(str) {
        }

        int getInt() const override {
            return m_i;
        }

        void setInt(const int i) override {
            m_i = i;
        }
        std::string getStr() const override {
            return m_str;
        }

        void setStr(const std::string &str) override {
            m_str = str;
        }

    private:
        int m_i = 0;
        std::string m_str;
    };
    OS_REGISTER_FACTORY(BaseProduct, ConcreteProduct1, "Class ConcreteProduct1", int);
    OS_REGISTER_FACTORY(BaseProduct, ConcreteProduct1, "Class ConcreteProduct1", std::string);

    TEST_F(AbstractFactory_UT, createWithCtors) {
        namespace nscore = NS_OSBASE::core;

        auto const factoryNames = nscore::TheFactoryManager.getFactoryNames<nscore::BaseProduct>();
        const bool bConcreteProductFound =
            std::find(factoryNames.cbegin(), factoryNames.cend(), "Class ConcreteProduct1") != factoryNames.cend();

        ASSERT_TRUE(bConcreteProductFound);

        auto pConcreteProduct1Int = nscore::TheFactoryManager.createInstance<nscore::BaseProduct>("Class ConcreteProduct1", 11);
        ASSERT_NE(nullptr, pConcreteProduct1Int);
        ASSERT_EQ(11, pConcreteProduct1Int->getInt());
        ASSERT_TRUE(pConcreteProduct1Int->getStr().empty());

        const std::string constStr("tata");
        auto pConcreteProduct1Str = nscore::TheFactoryManager.createInstance<nscore::BaseProduct>("Class ConcreteProduct1", constStr);
        ASSERT_NE(nullptr, pConcreteProduct1Str);
        ASSERT_EQ(0, pConcreteProduct1Str->getInt());
        ASSERT_EQ(constStr, pConcreteProduct1Str->getStr());
    }

    class ConcreteProduct2 : public BaseProduct, public Singleton<ConcreteProduct2> {
        friend class Singleton<ConcreteProduct2>;

    public:
        int getInt() const override {
            return m_i;
        }

        void setInt(const int i) override {
            m_i = i;
        }
        std::string getStr() const override {
            return m_str;
        }

        void setStr(const std::string &str) override {
            m_str = str;
        }

    private:
        ConcreteProduct2()  = default;
        ~ConcreteProduct2() = default;

        int m_i = 0;
        std::string m_str;
    };
    OS_REGISTER_FACTORY(BaseProduct, ConcreteProduct2, "Class ConcreteProduct2");

    TEST_F(AbstractFactory_UT, createWithSingleton) {
        namespace nscore = NS_OSBASE::core;

        auto const factoryNames = nscore::TheFactoryManager.getFactoryNames<nscore::BaseProduct>();
        const bool bConcreteProductFound =
            std::find(factoryNames.cbegin(), factoryNames.cend(), "Class ConcreteProduct2") != factoryNames.cend();

        ASSERT_TRUE(bConcreteProductFound);

        auto pConcreteProduct2 = nscore::TheFactoryManager.createInstance<nscore::BaseProduct>("Class ConcreteProduct2");
        ASSERT_NE(nullptr, pConcreteProduct2);
        ASSERT_EQ(0, pConcreteProduct2->getInt());
        ASSERT_TRUE(pConcreteProduct2->getStr().empty());

        pConcreteProduct2->setInt(1);
        pConcreteProduct2->setStr("toto");

        auto pConcreteProduct2Bis = nscore::TheFactoryManager.createInstance<nscore::BaseProduct>("Class ConcreteProduct2");
        ASSERT_NE(nullptr, pConcreteProduct2Bis);
        ASSERT_EQ(1, pConcreteProduct2Bis->getInt());
        ASSERT_EQ("toto", pConcreteProduct2Bis->getStr());
    }

    TEST_F(AbstractFactory_UT, createInSharedLib) {
        namespace nscore = NS_OSBASE::core;

        auto const factoryNames = nscore::TheFactoryManager.getFactoryNames<nscore::BaseProduct>();
        const bool bConcreteProductFound =
            std::find(factoryNames.cbegin(), factoryNames.cend(), "Class ConcreteProduct Shared") != factoryNames.cend();

        ASSERT_TRUE(bConcreteProductFound);

        auto pConcreteProductShared = nscore::TheFactoryManager.createInstance<nscore::BaseProduct>("Class ConcreteProduct Shared", 123);
        ASSERT_NE(nullptr, pConcreteProductShared);
        ASSERT_EQ(123, pConcreteProductShared->getInt());
        ASSERT_TRUE(pConcreteProductShared->getStr().empty());
    }

    OS_LINK_FACTORY_N(BaseProduct, ConcreteProductStatic, 0);

    TEST_F(AbstractFactory_UT, createInStaticLib) {
        namespace nscore = NS_OSBASE::core;

        auto const factoryNames = nscore::TheFactoryManager.getFactoryNames<nscore::BaseProduct>();
        const bool bConcreteProductFound =
            std::find(factoryNames.cbegin(), factoryNames.cend(), "Class ConcreteProduct Static") != factoryNames.cend();

        ASSERT_TRUE(bConcreteProductFound);

        auto pConcreteProductStaticLinked =
            nscore::TheFactoryManager.createInstance<nscore::BaseProduct>("Class ConcreteProduct Static", 11);
        ASSERT_NE(nullptr, pConcreteProductStaticLinked);
        ASSERT_EQ(11, pConcreteProductStaticLinked->getInt());
        ASSERT_TRUE(pConcreteProductStaticLinked->getStr().empty());
    }
} // namespace NS_OSBASE::core::ut
