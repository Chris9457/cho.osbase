// \file  BaseProduct.h
// \brief For unit tests

#pragma once
#include <string>

namespace cho::osbase::core {

    class BaseProduct {
    public:
        virtual ~BaseProduct()           = default;
        virtual int getInt() const       = 0;
        virtual void setInt(const int i) = 0;

        virtual std::string getStr() const          = 0;
        virtual void setStr(const std::string &str) = 0;
    };
} // namespace cho::osbase::core
