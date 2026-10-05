/*
 * Focused CPU/host fixtures deliberately link their own test main instead of
 * the product entry.  These are asserted-unreached forms of the original
 * BaseClient process APIs: a fixture traversal into one terminates with a
 * defined failure instead of selecting a second worker or inventing service
 * policy.  `nt_mem` also queries the original process environment during SAS
 * setup, where absent configuration is the source-default behavior.
 */
#include <windows.h>
#include <vdmapi.h>
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

/* CPU-only fixtures cannot reach a broker or frontend. Keep new adapter
 * imports fail-closed, just like ExitVDM below; do not connect real products. */
static DWORD unexpected_broker(void)
{
    ExitProcess(ERROR_CALL_NOT_IMPLEMENTED);
    return ERROR_CALL_NOT_IMPLEMENTED;
}
DWORD OpenNtBaseClientConnectCurrent(void) { return unexpected_broker(); }
DWORD OpenNtBaseClientWatchBroker(void) { return unexpected_broker(); }
void OpenNtBaseClientDisconnectCurrent(void) { (void)unexpected_broker(); }
DWORD OpenNtBaseClientWorkerIoTransition(DWORD action)
{ (void)action; return unexpected_broker(); }
DWORD OpenNtBaseClientWorkerFrontendCapability(HANDLE *value)
{ (void)value; return unexpected_broker(); }
DWORD OpenNtBaseClientAcquireConsoleContext(HANDLE frontend, HANDLE *value)
{ (void)frontend; (void)value; return unexpected_broker(); }
DWORD OpenNtBaseClientWorkerShutdownEvent(HANDLE *value)
{ (void)value; return unexpected_broker(); }
DWORD OpenNtBaseClientWorkerIoReleaseEvent(HANDLE *value)
{ (void)value; return unexpected_broker(); }
DWORD OpenNtBaseClientWaitFrontend(HANDLE *pipe, HANDLE *frontend,
    DWORD *generation, HANDLE *ready)
{ (void)pipe; (void)frontend; (void)generation; (void)ready; return unexpected_broker(); }
void OpenNtBaseClientSetCommandBinding(DWORD (*ready)(void *), void *context)
{ (void)ready; (void)context; (void)unexpected_broker(); }

VOID APIENTRY ExitVDM(BOOL is_wow, ULONG wow_task)
{
    (void)is_wow;
    (void)wow_task;
    ExitProcess(ERROR_CALL_NOT_IMPLEMENTED);
}

BOOL APIENTRY GetNextVDMCommand(PVDMINFO info)
{
    (void)info;
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL APIENTRY SetVDMCurrentDirectories(ULONG bytes, LPSTR directories)
{
    (void)bytes;
    (void)directories;
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

char * __cdecl mvdm_host_getenv(const char *name)
{
    (void)name;
    return NULL;
}
