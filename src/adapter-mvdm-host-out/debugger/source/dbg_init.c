/*
 * Source-shaped startup slice for the original MVDM debugger package.
 *
 * `mvdm-host/dbg/dbg.c` owns the full NT4 debugger product.  Its DBGInit
 * body is independently reached by InitialiseDosEmulation(), while the
 * remaining DBG dispatch/event bodies require the private NT4 VDM debugger
 * transport.  This adapter preserves the DBGInit name, arguments, result
 * and debug-port observation without selecting that transport.
 */

#include <windows.h>
#include <winternl.h>

#include "adapter-mvdm-host-out/debugger/include/dbg_state.h"

#ifndef STATUS_NOT_IMPLEMENTED
#define STATUS_NOT_IMPLEMENTED ((NTSTATUS)0xC0000002L)
#endif

/* DIVERGENCE(ADAPTER-MVDM-DEBUGGER-001): the original body calls the linked
 * NT4 NtQueryInformationProcess import with ProcessDebugPort.  Modern SDKs
 * do not expose that historical declaration as a stable application-facing
 * contract.  Resolve the same NTDLL export at the host boundary; unavailable
 * retains the original failure direction (fDebugged = FALSE, TRUE result).
 * The numerical information class is the original ProcessDebugPort value. */
typedef NTSTATUS (NTAPI *MVDM_NT_QUERY_INFORMATION_PROCESS)(
    HANDLE process, ULONG information_class, PVOID information, ULONG length,
    PULONG return_length);

BOOL DBGInit(int argc, char *argv[])
{
    HANDLE process;
    HANDLE debug_port;
    NTSTATUS status;
    HMODULE ntdll;
    MVDM_NT_QUERY_INFORMATION_PROCESS query_information_process;

    (void)argc;
    (void)argv;

    process = GetCurrentProcess();
    debug_port = NULL;
    ntdll = GetModuleHandleW(L"ntdll.dll");
    query_information_process = ntdll == NULL ? NULL :
        (MVDM_NT_QUERY_INFORMATION_PROCESS)GetProcAddress(
            ntdll, "NtQueryInformationProcess");
    status = query_information_process == NULL ? STATUS_NOT_IMPLEMENTED :
        query_information_process(process, 7u, &debug_port,
            (ULONG)sizeof(debug_port), NULL);
    if (status >= 0) {
        mvdm_debugger_set_debugged(debug_port != NULL);
    } else {
        mvdm_debugger_set_debugged(FALSE);
    }
    return TRUE;
}
