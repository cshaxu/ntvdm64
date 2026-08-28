#include "base_vdm_local.h"

BOOL APIENTRY GetNextVDMCommand(PVDMINFO information)
{
    VDMINFO capture;
    USHORT state;

    /* DIVERGENCE: the original client uses a CSR capture buffer and private
     * Base DLL transport.  The local profile retains its caller-owned request
     * snapshot and result-copy boundary, then invokes the same-shaped local
     * BaseSrv dispatcher.  No caller pointer survives this call. */
    if (information == NULL) return base_vdm_local_dispatch(NULL);

    capture = *information;
    state = information->VDMState;
    capture.VDMState = state;
    if (!base_vdm_local_dispatch(&capture)) {
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
