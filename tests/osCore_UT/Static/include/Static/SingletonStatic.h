// \file  SingletonStatic.h
// \brief For unit tests

#pragma once
#include "osCore/DesignPattern/Singleton.h"

namespace NS_OSBASE::core {

    class MySingleton : public Singleton<MySingleton> {
        friend Singleton<MySingleton>;

    public:
        int getCount() const;
        void increment();

    private:
        MySingleton() = default;
        ~MySingleton();

        int m_count = 0;
    };
#define TheMySingleton MySingleton::getInstance()
} // namespace NS_OSBASE::core