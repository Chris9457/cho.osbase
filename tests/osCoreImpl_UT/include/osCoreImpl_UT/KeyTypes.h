// osBase package

#pragma once
#include <string>
#include <vector>

template <typename T>
struct KeyType;
template <>
struct KeyType<bool> {
    static const char *getKey() {
        return "keyBool";
    }
    static bool getValue() {
        return false;
    }
    static const char *getKeys() {
        return "keyBools";
    }
    static std::vector<bool> getValues() {
        return { false, true, false };
    }
};
template <>
struct KeyType<int> {
    static const char *getKey() {
        return "keyInt";
    }
    static const char *getKeys() {
        return "keyInts";
    }
    static int getValue() {
        return 32;
    }
    static std::vector<int> getValues() {
        return { 32, 54, 85 };
    }
};
template <>
struct KeyType<double> {
    static const char *getKey() {
        return "keyDouble";
    }
    static const char *getKeys() {
        return "keyDoubles";
    }
    static double getValue() {
        return 3.4;
    }
    static std::vector<double> getValues() {
        return { 3.4, 53.2, 45.6 };
    }
};
template <>
struct KeyType<std::wstring> {
    static const char *getKey() {
        return "keyString";
    }
    static const char *getKeys() {
        return "keyStrings";
    }
    static std::wstring getValue() {
        return L"t e s t";
    }
    static std::vector<std::wstring> getValues() {
        return { L"t e s t1", L"t e s t2", L"t e s t3" };
    }
};
