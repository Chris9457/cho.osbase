// \brief Declaration of the "check" functions

#include "osCore/Exception/Check.h"

namespace oscheck {

    namespace {
        void crash() {
            *(int *)0 = 0;
        }
    } // namespace

    void throwIfFalse(const bool exp) {
        if (!exp) {
            throwIfCrash(crash);
        }
    }

    void throwIfTrue(const bool exp) {
        if (exp) {
            throwIfCrash(crash);
        }
    }
} // namespace oscheck
