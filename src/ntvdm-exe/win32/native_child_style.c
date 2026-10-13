#include <windows.h>
#include "nthook32-dll/hook.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client_launch_style.h"

/* Original nt_reset.c startup classification; never inferred from a task ID.
 * Separate WOW receives its actual Check/Get policy through the DOS record,
 * whereas shared WOW uses the original shared record/receipt query. */
extern BOOL fSeparateWow;
DWORD WINAPI NtvdmSeparateWowStyle(DWORD *style)
{
    if(!style)return ERROR_INVALID_PARAMETER;
    if(!fSeparateWow)return ERROR_NOT_FOUND;
    return OpenNtBaseClientSeparateWowStyle(style);
}

/* WOW borrows a suspended native child. This worker-local seam uses the same
 * installer as COMMAND/NTVWM, without binding a worker-wide appearance policy
 * or granting the GUI task any text frontend capability. */
DWORD WINAPI NtvdmInstallChildStyle(HANDLE child,DWORD style)
{
    nthook_context context;
    DWORD error;
    if(!WINDOW_STYLE_VALID(style))return ERROR_INVALID_PARAMETER;
    error=nthook_context_paths(&context);
    if(error)return error;
    context.window_style=style;
    return nthook_install(child,&context,NATIVE_HOOK_INTERCEPT);
}
