#ifndef WOW_NATIVE_EXTENSION_PROVIDER_H
#define WOW_NATIVE_EXTENSION_PROVIDER_H
#include <windows.h>
union _PARM16;
struct _VDMFRAME;
/* Selected x86 /Gz original ABI; opaque guest-frame pointers are never dereferenced by B. */
typedef struct wow_native_extension_provider {
    BOOL (WINAPI *kernel_resource)(INT);
    BOOL (WINAPI *callback)(INT, union _PARM16 *, DWORD, DWORD *);
    DWORD (WINAPI *call_proc)(struct _VDMFRAME *);
    ULONG (WINAPI *call_mm)(struct _VDMFRAME *);
    VOID (WINAPI *yield)(void);
    VOID (WINAPI *directed_yield)(WORD);
} wow_native_extension_provider;
BOOL WINAPI wow_native_extension_bind(const wow_native_extension_provider *);
#endif
