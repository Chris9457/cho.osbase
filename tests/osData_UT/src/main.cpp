// \brief entry point function

#include "osCoreImpl/CoreImpl.h"
#include "osData/osData.h"
#include "osDataImpl/osDataImpl.h"
#include <gtest/gtest.h>

OS_CORE_IMPL_LINK()
OS_DATA_IMPL_LINK()

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
