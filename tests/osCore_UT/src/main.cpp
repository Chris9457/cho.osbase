// \file  main.cpp
// \brief entrye point function

#include "osCoreImpl/CoreImpl.h"
#include "gtest/gtest.h"

OS_CORE_IMPL_LINK();

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
