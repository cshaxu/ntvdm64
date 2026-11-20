#ifndef WOW_USER_NATIVE_EXTENSION_H
#define WOW_USER_NATIVE_EXTENSION_H
#include <windows.h>
/* Native leaves only: original wkgthunk keeps its argument resolution,
 * conversion and x87 lifetime. Never apply these aliases to their provider. */
HMODULE WINAPI wow_extension_load_library_a(LPCSTR, HANDLE, DWORD);
BOOL WINAPI wow_extension_free_library(HMODULE);
FARPROC WINAPI wow_extension_get_proc_address(HMODULE, LPCSTR);
/* Exact selected wownt32.h export signatures; no original private layout. */
DWORD WINAPI WOWCallback16(DWORD, DWORD);
BOOL WINAPI WOWCallback16Ex(DWORD, DWORD, DWORD, PVOID, PDWORD);
VOID WINAPI WOWYield16(VOID);
VOID WINAPI WOWDirectedYield16(WORD);
#ifdef WOW_NATIVE_EXTENSION_BINDING
#define LoadLibraryExA wow_extension_load_library_a
#define FreeLibrary wow_extension_free_library
#define GetProcAddress wow_extension_get_proc_address
#endif
#endif
