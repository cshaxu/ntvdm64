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
