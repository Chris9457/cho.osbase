// \file  SingletonShared.cpp
// \brief For unit tests

#include "Shared/SingletonShared.h"
#include "Static/SingletonStatic.h"

namespace NS_OSBASE::core::ut {

    void increment() {
        TheMySingleton.increment();
    }

} // namespace NS_OSBASE::core::ut