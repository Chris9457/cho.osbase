// \file  AbstractFactoryShared.cpp
// \brief For unit tests

#include "Shared/AbstractFactoryShared.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace cho::osbase::core::ut {
    OS_REGISTER_FACTORY(BaseProduct, ConcreteProductShared, "Class ConcreteProduct Shared", int);

    ConcreteProductShared::ConcreteProductShared(int i) : m_i(i) {
    }

    int ConcreteProductShared::getInt() const {
        return m_i;
    }

    void ConcreteProductShared::setInt(const int i) {
        m_i = i;
    }
    std::string ConcreteProductShared::getStr() const {
        return m_str;
    }

    void ConcreteProductShared::setStr(const std::string &str) {
        m_str = str;
    }
} // namespace cho::osbase::core::ut