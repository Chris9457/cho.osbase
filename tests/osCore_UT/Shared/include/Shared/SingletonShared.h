// \file  SingletonShared.h
// \brief For unit tests
#pragma once

#ifdef DLL_EXPORT
#define LIB_PUBLISH __declspec(dllexport)
#else
#define LIB_PUBLISH __declspec(dllimport)
#endif

namespace NS_OSBASE::core::ut {

    LIB_PUBLISH void increment();

}