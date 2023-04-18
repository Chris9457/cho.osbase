// \file  SingletonShared.cpp
// \brief For unit tests

#include "Shared/SingletonShared.h"
#include "Static/SingletonStatic.h"

namespace cho::osbase::core::ut {

    void increment() {
        TheMySingleton.increment();
    }

} // namespace cho::osbase::core::ut