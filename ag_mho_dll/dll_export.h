#ifndef MHO_DLL_EXPORT_H
#define MHO_DLL_EXPORT_H

#ifdef __MINGW32__
#define MHO_API_EXPORT extern "C" __attribute__((dllexport))
#else
#define MHO_API_EXPORT extern "C" __declspec(dllexport)
#endif

MHO_API_EXPORT void dummy();

#endif
