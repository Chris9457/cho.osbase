// \brief entry point function
#include "osStateMachine/osStateMachine.h"
#include "osDataImpl/osDataImpl.h"
#include "osCoreImpl/CoreImpl.h"
#include <gtest/gtest.h>

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();
OS_STATEMACHINE_LINK();

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
