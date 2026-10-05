#include "intercept.h"
#include "detours/detours.h"
nthook_context nthook_process_context={0};
extern "C" void WINAPI NthookAnchor(void) {}
/* Diagnostic fixture reads only local payload state; never a broker API. */
extern "C" DWORD WINAPI NthookContextFlags(void)
{
    return nthook_process_context.mode |
        (nthook_process_context.frontend ? 0x100u : 0u);
}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID)
{
    if(reason==DLL_PROCESS_ATTACH) {
        BOOL found=FALSE;
        DisableThreadLibraryCalls(instance);
        if(!DetourRestoreAfterWith() ||
           nthook_context_read(&nthook_process_context,&found) || !found ||
           nthook_process_context.mode!=NATIVE_HOOK_INTERCEPT)return FALSE;
        return nthook_attach()==NO_ERROR;
    }
    /* Process teardown is not a task completion source. No waits or RPC. */
    return TRUE;
}
