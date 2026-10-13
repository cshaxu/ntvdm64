#include "api.h"

DWORD __cdecl Ntw16sGetAbiVersion(void)
{
    return NTW16S_ABI_VERSION;
}

BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,LPVOID reserved)
{
    /* Static TLS is used by the core; do not DisableThreadLibraryCalls.
     * Explicit same-thread install/remove owns all GUI work outside loader lock. */
    (void)module;(void)reason;(void)reserved;
    return TRUE;
}
