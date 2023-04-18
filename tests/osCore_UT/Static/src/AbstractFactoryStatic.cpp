// \file  AbstractFactoryStatic.cpp
// \brief For unit tests

#include "Static/AbstractFactoryStatic.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace cho::osbase::core::ut {

    OS_REGISTER_FACTORY_N(BaseProduct, ConcreteProductStatic, 0, "Class ConcreteProduct Static", int);

    ConcreteProductStatic::ConcreteProductStatic(const int i) : m_i(i) {
    }

    int ConcreteProductStatic::getInt() const {
        return m_i;
    }

    void ConcreteProductStatic::setInt(const int i) {
        m_i = i;
    }
    std::string ConcreteProductStatic::getStr() const {
        return m_str;
    }

    void ConcreteProductStatic::setStr(const std::string &str) {
        m_str = str;
    }

} // namespace cho::osbase::core::ut