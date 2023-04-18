// \file  SingletonStatic.cpp
// \brief For unit tests

#include "Static/SingletonStatic.h"

namespace cho::osbase::core {
    MySingleton::~MySingleton() {
    }

    int MySingleton::getCount() const {
        return m_count;
    }

    void MySingleton::increment() {
        ++m_count;
    }

} // namespace cho::osbase::core