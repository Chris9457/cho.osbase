// \brief entry point function
#include "osWebEngine/osWebEngine.h"
#include <gtest/gtest.h>

OS_WEBENGINE_LINK();

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
