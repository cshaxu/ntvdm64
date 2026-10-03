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
