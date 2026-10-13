#ifndef NTW16S_API_H
#define NTW16S_API_H
#include <windows.h>
#if defined(NTW16S_STATIC)
#define NTW16S_API
#elif defined(NTW16S_BUILD)
#define NTW16S_API __declspec(dllexport)
#else
#define NTW16S_API __declspec(dllimport)
#endif
#define NTW16S_ABI_VERSION 1u
/* Local C ABI, same native width. Never pass CRT allocations across it.
 * Calls and removal run on the HWND's GUI thread. The caller must retain its
 * module reference until all windows/scopes/fonts are released and no calls
 * or callbacks remain active. DllMain performs no USER/GDI operations. */
NTW16S_API DWORD __cdecl Ntw16sGetAbiVersion(void);
#endif
