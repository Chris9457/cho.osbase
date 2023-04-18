// \file  AbstractFactoryShared.h
// \brief For unit tests
#pragma once

#include "osCore_UT/BaseProduct.h"

namespace cho::osbase::core::ut {
    class ConcreteProductShared : public BaseProduct {
    public:
        ConcreteProductShared(int i);

        int getInt() const override;
        void setInt(const int i) override;

        std::string getStr() const override;
        void setStr(const std::string &str) override;

    private:
        int m_i = 0;
        std::string m_str;
    };
} // namespace cho::osbase::core::ut