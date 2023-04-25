// \file  SingletonStatic.cpp
// \brief For unit tests

#include "Static/SingletonStatic.h"

namespace NS_OSBASE::core {
    MySingleton::~MySingleton() {
    }

    int MySingleton::getCount() const {
        return m_count;
    }

    void MySingleton::increment() {
        ++m_count;
    }

} // namespace NS_OSBASE::core