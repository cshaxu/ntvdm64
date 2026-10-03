/* Test-only translation of the actual service implementation. Explicitly
 * selected instead of its archive member by this one in-process fixture.
 * Expose the same private copied queue/take operations, without adding a
 * production API, transport, substitute policy or duplicate implementation. */
#include "../../src/ntsrv-exe/opennt/source/base_service.c"

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
