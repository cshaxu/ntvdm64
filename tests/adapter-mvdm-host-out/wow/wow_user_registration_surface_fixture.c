/*
 * Isolated observation of the legacy USER32 WOW registration surface.
 *
 * This program deliberately has no product linkage and supplies zeroed input
 * callbacks.  The historical registrar stores the input callbacks but does
 * not call them while registering.  Running it in its own process therefore
 * records the contemporary USER32 contract without changing an NTVDM worker.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "opennt-abi/host-compat/include/nt.h"
/* wowuserp.h normally receives this exact private callback typedef through
 * the original precompiled USER declarations.  The fixture needs only the
 * registration record, so retain the ABI spelling without importing the
 * conflicting full NT4 winuserp declaration universe. */
typedef VOID (WINAPI *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

typedef DWORD (WINAPI *wow_user_register_proc)(APFNWOWHANDLERSIN,
    APFNWOWHANDLERSOUT);

int __cdecl main(void)
{
    HMODULE user;
    wow_user_register_proc register_handlers;
    PFNWOWHANDLERSIN input;
    PFNWOWHANDLERSOUT output;
    DWORD shared;
    const DWORD *slots;
    DWORD index;
    DWORD populated = 0;

    ZeroMemory(&input, sizeof(input));
    ZeroMemory(&output, sizeof(output));
    user = LoadLibraryW(L"user32.dll");
    if (!user) {
        printf("WOW_USER_REGISTRATION_SURFACE load-error=%lu\n", GetLastError());
        return 2;
    }
    register_handlers = (wow_user_register_proc)GetProcAddress(user,
        "UserRegisterWowHandlers");
    if (!register_handlers) {
        printf("WOW_USER_REGISTRATION_SURFACE export-error=%lu\n", GetLastError());
        FreeLibrary(user);
        return 3;
    }
    shared = register_handlers(&input, &output);
    slots = (const DWORD *)((const BYTE *)&output + sizeof(output.dwBldInfo));
    for (index = 0; index != 20; ++index) {
        if (slots[index] != 0) ++populated;
    }
    printf("WOW_USER_REGISTRATION_SURFACE shared=%08lX build=%08lX slots=%lu\n",
        (unsigned long)shared, (unsigned long)output.dwBldInfo,
        (unsigned long)populated);
    FreeLibrary(user);
    return 0;
}
