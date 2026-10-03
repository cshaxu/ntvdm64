/* Test-only access to the actual archive's private copied queue/take seam.
 * Do not embed a second service translation. All service modules are selected
 * from the same production archive; no substitute policy or public API. */
#include <service_internal.h>

DWORD fixture_queue_native_command(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE capability,DWORD bytes,const BYTE *payload)
{
    return service_queue_native_command(connection,pid,generation,capability,L"fixture.exe",bytes,payload);
}

DWORD fixture_take_native_command(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,DWORD capacity,BYTE *payload,DWORD *bytes,HANDLE *sender,
    HANDLE *execution,HANDLE *frontend,DWORD *request)
{
    DWORD caller_generation=0;
    return service_take_native_command(connection,pid,generation,capacity,payload,bytes,
        sender,execution,frontend,request,&caller_generation);
}

/* Fault injection changes only the local reference rights of the actual
 * notification object. The real production publisher/decision runs under
 * its existing lock; the owned handle is restored before leaving the seam. */
DWORD fixture_frontend_notification_denied(OPENNT_BASE_CONNECTION *root,
    DWORD pid,DWORD generation,DWORD nonce,BOOL decision,BOOL *closing)
{
    HANDLE original,readonly=NULL;
    DWORD error;
    if(!root || !closing)return ERROR_INVALID_PARAMETER;
    *closing=FALSE;
    EnterCriticalSection(&root->service->lock);
    original=root->frontend_capability;
    if(!DuplicateHandle(GetCurrentProcess(),original,GetCurrentProcess(),
        &readonly,SYNCHRONIZE,FALSE,0))error=GetLastError();
    else {
        root->frontend_capability=readonly;
        error=decision ? OpenNtBaseServiceFrontendJoinDecision(root,pid,generation,nonce,FALSE) :
            service_refresh_frontend_work(root);
        *closing=root->frontend_closing;
        root->frontend_capability=original;
        CloseHandle(readonly);
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}
