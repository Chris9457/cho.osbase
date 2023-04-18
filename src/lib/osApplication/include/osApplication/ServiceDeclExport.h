// \brief Declaration of the macros import / export for shared libraries of the services

#pragma once

#ifdef SERVICES_DLL_EXPORT
#define SERVICES_DLL_PREFIX __declspec(dllexport)
#else
#define SERVICES_DLL_PREFIX __declspec(dllimport)
#endif
