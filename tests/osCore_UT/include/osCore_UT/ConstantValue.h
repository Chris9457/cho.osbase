// osBase package

/**
 * \file  constantvalue.h
 * \brief Declaration of classes ConstantValue
 */

#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <array>

namespace cho::osbase::core::ut {
    enum class TestEnum { field1 = 1, field2, field3 };

    inline std::ostream &operator<<(std::ostream &os, const TestEnum v) {
        return os << (v == TestEnum::field1 ? "field1" : v == TestEnum::field2 ? "field2" : v == TestEnum::field3 ? "field3" : "None");
    }

    template <typename T, int Rank>
    struct ConstantValue;

    // Declaration

    /*
     * bool
     */
    template <>
    struct ConstantValue<bool, 0> {
        static bool getValue() {
            return false;
        }
    };
    template <>
    struct ConstantValue<bool, 1> {
        static bool getValue() {
            return true;
        }
    };
    template <>
    struct ConstantValue<bool, 2> {
        static bool getValue() {
            return false;
        }
    };

    template <>
    struct ConstantValue<bool[], 0> {
        static std::vector<bool> getValue() {
            return { false, true, false, true };
        }
    };
    template <>
    struct ConstantValue<bool[], 1> {
        static std::vector<bool> getValue() {
            return { true, false, true, false };
        }
    };
    template <>
    struct ConstantValue<bool[], 2> {
        static std::vector<bool> getValue() {
            return { false, false, true, true };
        }
    };

    /*
     * char
     */
    template <>
    struct ConstantValue<char, 0> {
        static char getValue() {
            return 0;
        }
    };
    template <>
    struct ConstantValue<char, 1> {
        static char getValue() {
            return -2;
        }
    };
    template <>
    struct ConstantValue<char, 2> {
        static auto getValue() {
            return 5;
        }
    };

    template <>
    struct ConstantValue<char[], 0> {
        static std::vector<char> getValue() {
            return { 76, 5, 0, 1 };
        }
    };
    template <>
    struct ConstantValue<char[], 1> {
        static std::vector<char> getValue() {
            return { 77, 6, 1, 2 };
        }
    };
    template <>
    struct ConstantValue<char[], 2> {
        static std::vector<char> getValue() {
            return { 78, 7, 2, 0 };
        }
    };

    /*
     * short
     */
    template <>
    struct ConstantValue<short, 0> {
        static short getValue() {
            return 0;
        }
    };
    template <>
    struct ConstantValue<short, 1> {
        static short getValue() {
            return -2;
        }
    };
    template <>
    struct ConstantValue<short, 2> {
        static short getValue() {
            return 5;
        }
    };

    template <>
    struct ConstantValue<short[], 0> {
        static std::vector<short> getValue() {
            return { 76, 5, 0, 1 };
        }
    };
    template <>
    struct ConstantValue<short[], 1> {
        static std::vector<char> getValue() {
            return { 77, 6, 1, 2 };
        }
    };
    template <>
    struct ConstantValue<short[], 2> {
        static std::vector<int> getValue() {
            return { 78, 7, 2, 0 };
        }
    };

    /*
     * int
     */
    template <>
    struct ConstantValue<int, 0> {
        static int getValue() {
            return 0;
        }
    };
    template <>
    struct ConstantValue<int, 1> {
        static int getValue() {
            return -2;
        }
    };
    template <>
    struct ConstantValue<int, 2> {
        static int getValue() {
            return 5;
        }
    };

    template <>
    struct ConstantValue<int[], 0> {
        static std::vector<int> getValue() {
            return { 76, 5, 0, 1 };
        }
    };
    template <>
    struct ConstantValue<int[], 1> {
        static std::vector<int> getValue() {
            return { 77, 6, 1, 2 };
        }
    };
    template <>
    struct ConstantValue<int[], 2> {
        static std::vector<int> getValue() {
            return { 78, 7, 2, 0 };
        }
    };

    /*
     * double
     */
    template <>
    struct ConstantValue<double, 0> {
        static double getValue() {
            return 0.;
        }
    };
    template <>
    struct ConstantValue<double, 1> {
        static double getValue() {
            return 2.5;
        }
    };
    template <>
    struct ConstantValue<double, 2> {
        static double getValue() {
            return 5.4;
        }
    };

    template <>
    struct ConstantValue<double[], 0> {
        static std::vector<double> getValue() {
            return { 111.6, 87.3, 0.1, 53.1 };
        }
    };
    template <>
    struct ConstantValue<double[], 1> {
        static std::vector<double> getValue() {
            return { 112.6, 88.3, 0.2, 54.1 };
        }
    };
    template <>
    struct ConstantValue<double[], 2> {
        static std::vector<double> getValue() {
            return { 113.6, 88.4, 0.3, 55.1 };
        }
    };

    /*
     * float
     */
    template <int Rank>
    struct ConstantValue<float, Rank> {
        static float getValue() {
            return static_cast<float>(ConstantValue<double, Rank>::getValue());
        }
    };

    /*
     * size_t
     */
    template <int Rank>
    struct ConstantValue<size_t, Rank> {
        static size_t getValue() {
            return static_cast<size_t>(abs(ConstantValue<int, Rank>::getValue()));
        }
    };

    /*
     * std::string
     */
    template <>
    struct ConstantValue<std::string, 0> {
        static std::string getValue() {
            return std::string();
        }
    };
    template <>
    struct ConstantValue<std::string, 1> {
        static std::string getValue() {
            return std::string("tata");
        }
    };
    template <>
    struct ConstantValue<std::string, 2> {
        static std::string getValue() {
            return std::string("toto");
        }
    };

    template <>
    struct ConstantValue<std::string[], 0> {
        static std::vector<std::string> getValue() {
            return { std::string("ooap"), std::string("ususu"), std::string("r") };
        }
    };
    template <>
    struct ConstantValue<std::string[], 1> {
        static std::vector<std::string> getValue() {
            return { std::string("ususu"), std::string("r"), std::string("ooap") };
        }
    };
    template <>
    struct ConstantValue<std::string[], 2> {
        static std::vector<std::string> getValue() {
            return { std::string("r"), std::string("ooap"), std::string("ususu") };
        }
    };

    /*
     * std::wstring
     */
    template <>
    struct ConstantValue<std::wstring, 0> {
        static std::wstring getValue() {
            return std::wstring();
        }
    };
    template <>
    struct ConstantValue<std::wstring, 1> {
        static std::wstring getValue() {
            return std::wstring(L"tata");
        }
    };
    template <>
    struct ConstantValue<std::wstring, 2> {
        static std::wstring getValue() {
            return std::wstring(L"toto");
        }
    };

    template <>
    struct ConstantValue<std::wstring[], 0> {
        static std::vector<std::wstring> getValue() {
            return { std::wstring(L"ooap"), std::wstring(L"ususu"), std::wstring(L"r") };
        }
    };
    template <>
    struct ConstantValue<std::wstring[], 1> {
        static std::vector<std::wstring> getValue() {
            return { std::wstring(L"oap"), std::wstring(L"susu"), std::wstring(L"ra") };
        }
    };
    template <>
    struct ConstantValue<std::wstring[], 2> {
        static std::vector<std::wstring> getValue() {
            return { std::wstring(L"op"), std::wstring(L"usu"), std::wstring(L"raa") };
        }
    };

    /*
     * std::vector<int>
     */
    template <>
    struct ConstantValue<std::vector<int>, 0> {
        static std::vector<int> getValue() {
            return { 1, 2, 3 };
        }
    };
    template <>
    struct ConstantValue<std::vector<int>, 1> {
        static std::vector<int> getValue() {
            return { 4, 3, 2, 8 };
        }
    };
    template <>
    struct ConstantValue<std::vector<int>, 2> {
        static std::vector<int> getValue() {
            return { 18, 20, 12, 6, 7 };
        }
    };

    /*
     * std::array<int, 5>
     */
    template <>
    struct ConstantValue<std::array<int, 5>, 0> {
        static std::array<int, 5> getValue() {
            return { 1, 2, 3, 4, 5 };
        }
    };
    template <>
    struct ConstantValue<std::array<int, 5>, 1> {
        static std::array<int, 5> getValue() {
            return { 2, 3, 4, 5, 1 };
        }
    };
    template <>
    struct ConstantValue<std::array<int, 5>, 2> {
        static std::array<int, 5> getValue() {
            return { 3, 4, 5, 1, 2 };
        }
    };

    /*
     * TestEnum
     */
    template <>
    struct ConstantValue<TestEnum, 0> {
        static TestEnum getValue() {
            return TestEnum::field1;
        }
    };
    template <>
    struct ConstantValue<TestEnum, 1> {
        static TestEnum getValue() {
            return TestEnum::field2;
        }
    };
    template <>
    struct ConstantValue<TestEnum, 2> {
        static TestEnum getValue() {
            return TestEnum::field3;
        }
    };

    template <>
    struct ConstantValue<TestEnum[], 0> {
        static std::vector<TestEnum> getValue() {
            return { TestEnum::field1, TestEnum::field2, TestEnum::field3 };
        }
    };
    template <>
    struct ConstantValue<TestEnum[], 1> {
        static std::vector<TestEnum> getValue() {
            return { TestEnum::field2, TestEnum::field3, TestEnum::field2 };
        }
    };
    template <>
    struct ConstantValue<TestEnum[], 2> {
        static std::vector<TestEnum> getValue() {
            return { TestEnum::field3, TestEnum::field1, TestEnum::field1 };
        }
    };

    /*
     * unsigned short
     */
    template <>
    struct ConstantValue<unsigned short, 0> {
        static unsigned short getValue() {
            return 0;
        }
    };
    template <>
    struct ConstantValue<unsigned short, 1> {
        static unsigned short getValue() {
            return 2;
        }
    };
    template <>
    struct ConstantValue<unsigned short, 2> {
        static unsigned short getValue() {
            return 5;
        }
    };

    template <>
    struct ConstantValue<unsigned short[], 0> {
        static std::vector<unsigned short> getValue() {
            return { 111, 87, 0, 53 };
        }
    };
    template <>
    struct ConstantValue<unsigned short[], 1> {
        static std::vector<unsigned short> getValue() {
            return { 112, 88, 0, 54 };
        }
    };
    template <>
    struct ConstantValue<unsigned short[], 2> {
        static std::vector<unsigned short> getValue() {
            return { 113, 88, 0, 5 };
        }
    };

    /*
     * unsigned char
     */
    template <>
    struct ConstantValue<unsigned char, 0> {
        static unsigned char getValue() {
            return 0;
        }
    };
    template <>
    struct ConstantValue<unsigned char, 1> {
        static unsigned char getValue() {
            return 2;
        }
    };
    template <>
    struct ConstantValue<unsigned char, 2> {
        static unsigned char getValue() {
            return 5;
        }
    };

    template <>
    struct ConstantValue<unsigned char[], 0> {
        static std::vector<unsigned char> getValue() {
            return { 111, 87, 0, 53 };
        }
    };
    template <>
    struct ConstantValue<unsigned char[], 1> {
        static std::vector<unsigned char> getValue() {
            return { 112, 88, 0, 54 };
        }
    };
    template <>
    struct ConstantValue<unsigned char[], 2> {
        static std::vector<unsigned char> getValue() {
            return { 113, 88, 0, 5 };
        }
    };

    // Custom types
    // TestStruct
    struct TestStruct {
        std::string st;
        int i;
        double d;
        std::vector<int> v;

        bool operator==(const TestStruct &other) const {
            return st == other.st && i == other.i && d == other.d && v == other.v;
        }
    };

    template <>
    struct ConstantValue<TestStruct, 0> {
        static TestStruct getValue() {
            return { "oula", 42, 85.5, { 4, 8, 12 } };
        }
    };

    template <>
    struct ConstantValue<TestStruct, 1> {
        static TestStruct getValue() {
            return { "toto", 2, 3.5, { 1, 2 } };
        }
    };

    template <>
    struct ConstantValue<TestStruct[], 0> {
        static std::vector<TestStruct> getValue() {
            return { ConstantValue<TestStruct, 0>::getValue(), ConstantValue<TestStruct, 1>::getValue() };
        }
    };

    template <>
    struct ConstantValue<TestStruct[], 1> {
        static std::vector<TestStruct> getValue() {
            return { ConstantValue<TestStruct, 0>::getValue(),
                ConstantValue<TestStruct, 1>::getValue(),
                ConstantValue<TestStruct, 0>::getValue(),
                ConstantValue<TestStruct, 1>::getValue() };
        }
    };

    // ComplexTestStruct
    struct ComplexTestStruct {
        double d;
        std::vector<TestStruct> t;
        TestEnum e;
        std::string s;

        bool operator==(const ComplexTestStruct &other) const {
            return d == other.d && s == other.s && e == other.e && t == other.t;
        }
    };

    template <>
    struct ConstantValue<ComplexTestStruct, 0> {
        static ComplexTestStruct getValue() {
            return {
                3.5,
                { ConstantValue<TestStruct, 0>::getValue(), ConstantValue<TestStruct, 1>::getValue() },
                TestEnum::field3,
                "tutu",
            };
        }
    };

    template <>
    struct ConstantValue<ComplexTestStruct, 1> {
        static ComplexTestStruct getValue() {
            return {
                2.5,
                { ConstantValue<TestStruct, 1>::getValue(), ConstantValue<TestStruct, 0>::getValue() },
                TestEnum::field2,
                "titi",
            };
        }
    };

    template <>
    struct ConstantValue<ComplexTestStruct[], 0> {
        static std::vector<ComplexTestStruct> getValue() {
            return { ConstantValue<ComplexTestStruct, 0>::getValue(), ConstantValue<ComplexTestStruct, 1>::getValue() };
        }
    };

    template <>
    struct ConstantValue<ComplexTestStruct[], 1> {
        static std::vector<ComplexTestStruct> getValue() {
            return { ConstantValue<ComplexTestStruct, 0>::getValue(),
                ConstantValue<ComplexTestStruct, 1>::getValue(),
                ConstantValue<ComplexTestStruct, 0>::getValue(),
                ConstantValue<ComplexTestStruct, 1>::getValue() };
        }
    };

    template <>
    struct ConstantValue<std::vector<short>, 1> {
        static std::vector<short> getValue() {
            return {};
        }
    };
} // namespace cho::osbase::core::ut
