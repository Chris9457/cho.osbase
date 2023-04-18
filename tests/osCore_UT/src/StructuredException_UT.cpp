// \brief Unit test for the class StructuredException

#include "osCore/Exception/StructuredExceptionHandler.h"
#include "osCore/Exception/Check.h"
#include "gtest/gtest.h"

namespace cho::osbase::core::ut {
    class StructuredException_UT : public testing::Test {
    protected:
        void TearDown() override {
            for (auto const &dumpFile : getDumpFiles()) {
                remove(dumpFile);
            }
        }

        std::vector<std::filesystem::path> getDumpFiles() const {
            std::vector<std::filesystem::path> dumpFiles;

            for (auto const &dirEntry : std::filesystem::directory_iterator{ TheStructuredExceptionHandler.getMinidumpFolder() }) {
                if (!dirEntry.is_directory()) {
                    const std::filesystem::path path = dirEntry;
                    if (path.filename().string().find(TheStructuredExceptionHandler.getDumpBaseFileName()) != std::string::npos) {
                        dumpFiles.push_back(path);
                    }
                }
            }

            return dumpFiles;
        }

        static void invokeCrash(int, const std::string) {
            *((int *)0) = 0;
        }

        static int invokeCrashInt(int, const std::string) {
            invokeCrash(1, "c");
            return 0;
        }
    };

    TEST_F(StructuredException_UT, miniDumpFolder) {
        auto const path = std::filesystem::current_path() / "crashdump";
        if (exists(path)) {
            remove(path);
        }

        TheStructuredExceptionHandler.setMinidumpFolder(path);
        ASSERT_EQ(path, TheStructuredExceptionHandler.getMinidumpFolder());
        ASSERT_TRUE(exists(path));

        // Check that the generated dump file is located to the right path
        ASSERT_THROW(oscheck::throwIfCrash(StructuredException_UT::invokeCrash, 1, "c"), StructuredException);
        for (auto const &dirEntry : std::filesystem::directory_iterator{ TheStructuredExceptionHandler.getMinidumpFolder() }) {
            if (!dirEntry.is_directory()) {
                const std::filesystem::path pathDumpFile = dirEntry;
                if (pathDumpFile.filename().string().find(TheStructuredExceptionHandler.getDumpBaseFileName()) == 0) {
                    SUCCEED();
                    return;
                }
            }
        }
        FAIL();
    }

    TEST_F(StructuredException_UT, baseDumpFileName) {
        const std::string baseFileName = "testCrash";

        TheStructuredExceptionHandler.setDumpBaseFileName(baseFileName);
        ASSERT_EQ(baseFileName, TheStructuredExceptionHandler.getDumpBaseFileName());

        // Check that the generated dump file has the right base name
        ASSERT_THROW(oscheck::throwIfCrash(StructuredException_UT::invokeCrash, 1, "c"), StructuredException);
        for (auto const &dirEntry : std::filesystem::directory_iterator{ TheStructuredExceptionHandler.getMinidumpFolder() }) {
            if (!dirEntry.is_directory()) {
                const std::filesystem::path path = dirEntry;
                if (path.filename().string().find(TheStructuredExceptionHandler.getDumpBaseFileName()) == 0) {
                    SUCCEED();
                    return;
                }
            }
        }
        FAIL();
    }

    TEST_F(StructuredException_UT, throwIfCrash_OK) {
        ASSERT_THROW(oscheck::throwIfCrash(StructuredException_UT::invokeCrash, 1, "c"), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfCrash_KO) {
        ASSERT_NO_THROW(oscheck::throwIfCrash([]() {}));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfCrashOrReturn_OK) {
        ASSERT_THROW(oscheck::throwIfCrashOrReturn<int>(StructuredException_UT::invokeCrashInt, 1, "c"), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfCrashOrReturn_KO) {
        try {
            auto const v = oscheck::throwIfCrashOrReturn<int>([]() { return 1; });
            ASSERT_EQ(1, v);
        } catch (const StructuredException &) {
            FAIL();
        }
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfFalse_OK) {
        ASSERT_THROW(oscheck::throwIfFalse(false), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfFalse_KO) {
        ASSERT_NO_THROW(oscheck::throwIfFalse(true));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfTrue_OK) {
        ASSERT_THROW(oscheck::throwIfTrue(true), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfTrue_KO) {
        ASSERT_NO_THROW(oscheck::throwIfTrue(false));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNE_OK) {
        ASSERT_THROW(oscheck::throwIfNE(1, 2), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNE_KO) {
        ASSERT_NO_THROW(oscheck::throwIfNE(1, 1));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfEQ_OK) {
        ASSERT_THROW(oscheck::throwIfEQ(1, 1), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfEQ_KO) {
        ASSERT_NO_THROW(oscheck::throwIfEQ(1, 2));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotLT_OK) {
        ASSERT_THROW(oscheck::throwIfNotLT(1, 0), StructuredException);
        ASSERT_THROW(oscheck::throwIfNotLT(0, 0), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotLT_KO) {
        ASSERT_NO_THROW(oscheck::throwIfNotLT(1, 2));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotLE_OK) {
        ASSERT_THROW(oscheck::throwIfNotLE(1, 0), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotLE_KO) {
        ASSERT_NO_THROW(oscheck::throwIfNotLE(1, 2));
        ASSERT_NO_THROW(oscheck::throwIfNotLE(1, 1));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotGT_OK) {
        ASSERT_THROW(oscheck::throwIfNotGT(0, 1), StructuredException);
        ASSERT_THROW(oscheck::throwIfNotGT(0, 0), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotGT_KO) {
        ASSERT_NO_THROW(oscheck::throwIfNotGT(2, 1));
        ASSERT_TRUE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotGE_OK) {
        ASSERT_THROW(oscheck::throwIfNotGE(0, 1), StructuredException);
        ASSERT_FALSE(getDumpFiles().empty());
    }

    TEST_F(StructuredException_UT, throwIfNotGE_KO) {
        ASSERT_NO_THROW(oscheck::throwIfNotGE(2, 1));
        ASSERT_NO_THROW(oscheck::throwIfNotGE(1, 1));
        ASSERT_TRUE(getDumpFiles().empty());
    }

} // namespace cho::osbase::core::ut
