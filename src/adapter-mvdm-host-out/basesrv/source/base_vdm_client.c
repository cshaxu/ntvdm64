#include "base_vdm_local.h"

BOOL APIENTRY GetNextVDMCommand(PVDMINFO information)
{
    VDMINFO capture;
    VDMINFO request;
    USHORT state;

    /* DIVERGENCE: the original client uses a CSR capture buffer and private
     * Base DLL transport.  The local profile retains its caller-owned request
     * snapshot and result-copy boundary, then invokes the same-shaped local
     * BaseSrv dispatcher.  No caller pointer survives this call. */
    if (information == NULL) return base_vdm_local_is_first();

    request = *information;
    capture = request;
    state = information->VDMState;
    capture.VDMState = state;
retry:
    if (!base_vdm_local_dispatch(&capture)) {
        if (GetLastError() == ERROR_IO_PENDING &&
            base_vdm_local_wait_for_command(&capture)) {
            /* BaseClient retries the server request after its wait object is
             * signalled.  The server's no-command result has no caller-owned
             * buffers to retain, so reconstruct the captured request rather
             * than accidentally submitting the cleared required-size fields. */
            state = capture.VDMState;
            capture = request;
            capture.VDMState = state;
            /* `vdm.c` clears ExitCode before the client reissues the
             * server request.  A completed wait is a command-acquisition
             * retry, not a second parent-return report. */
            capture.ErrorCode = 0u;
            goto retry;
        }
        information->CmdSize = capture.CmdSize;
        information->AppLen = capture.AppLen;
        information->PifLen = capture.PifLen;
        information->EnviornmentSize = capture.EnviornmentSize;
        information->CurDirectoryLen = capture.CurDirectoryLen;
        information->DesktopLen = capture.DesktopLen;
        information->TitleLen = capture.TitleLen;
        information->ReservedLen = capture.ReservedLen;
        return FALSE;
    }

    *information = capture;
    return TRUE;
}

VOID APIENTRY ExitVDM(BOOL wow_caller, ULONG wow_task)
{
    base_vdm_local_exit(wow_caller, wow_task);
}

BOOL APIENTRY SetVDMCurrentDirectories(ULONG byte_count, CHAR *directories)
{
    return base_vdm_local_set_current_directories(byte_count, directories);
}

ULONG APIENTRY GetVDMCurrentDirectories(ULONG byte_count, CHAR *directories)
{
    return base_vdm_local_get_current_directories(byte_count, directories);
}
