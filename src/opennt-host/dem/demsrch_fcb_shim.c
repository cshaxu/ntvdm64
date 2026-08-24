/* Neutral host-composition seam for the directly imported OpenNT
 * base/mvdm/dos/dem/demsrch.c and demfcb.c owner package.  It supplies only
 * the historical OEM Win32 entrypoints and the common scoped CCPU/SAS call
 * replacement; pathname matching, FCB state and all search ordering remain
 * in the imported translation units. */

#include "demsrch_fcb_shim.h"

#include <stdlib.h>
#include <string.h>

void demFindFirst(void);
void demFindFirstFCB(void);
void demFindNext(void);
void demFindNextFCB(void);
void demTerminatePDB(void);
void demCloseFCB(void);
void demCreateFCB(void);
void demDate16(void);
void demDeleteFCB(void);
void demFCBIO(void);
void demGetFileInfo(void);
void demOpenFCB(void);
void demRenameFCB(void);

/* Original nt_msscs.c carries this exact lifecycle spelling.  Do not import
 * demdasd's unrelated macro domain merely to name the one FDISK teardown. */
void FdiskTerminatePDB(USHORT pdb);

/* OpenNT's standalone source expected this process-global lifecycle marker
 * from its historical VDM host.  A newly composed CLI session starts in the
 * same first-call state; session wiring will own later transitions. */
BOOL IsFirstCall = TRUE;

static LPSTR ansi_copy(LPCSTR oem)
{
    size_t bytes;
    LPSTR ansi;

    if (oem == NULL) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    bytes = strlen(oem) + 1u;
    if (bytes > MAX_PATH + 1u) { SetLastError(ERROR_BUFFER_OVERFLOW); return NULL; }
    ansi = (LPSTR)malloc(bytes);
    if (ansi == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (!OemToCharBuffA(oem, ansi, (DWORD)bytes)) { free(ansi); return NULL; }
    return ansi;
}

HANDLE FindFirstFileOem(LPSTR name, LPWIN32_FIND_DATAA data)
{
    LPSTR ansi = ansi_copy(name);
    HANDLE find;
    if (ansi == NULL) return INVALID_HANDLE_VALUE;
    find = FindFirstFileA(ansi, data);
    free(ansi);
    return find;
}

BOOL FindNextFileOem(HANDLE find, LPWIN32_FIND_DATAA data)
{
    return FindNextFileA(find, data);
}

/* DIVERGENCE(HOST-DIV-010): nt_msscs.c:VDDTerminateUserHook walks the private VDD user-hook
 * list. The CLI does not recreate the VDD callback broker; do not report a
 * fabricated callback completion. */
void VDDTerminateUserHook(USHORT pdb) { (void)pdb; }
void HostTerminatePDB(USHORT pdb)
{
    /* nt_msscs.c orders FloppyTerminatePDB(PDB), then FdiskTerminatePDB(PDB).
     * The floppy half remains the explicit FDC/DMA/CMOS unavailable owner;
     * retain the directly composable FDISK half in its original second slot. */
    FdiskTerminatePDB(pdb);
}

/* RtlProcessHeap was an OpenNT-era exported helper.  The current public
 * process heap has the same role for the imported RtlAllocate/FreeHeap calls.
 * NtVdmControl is a removed invasive-host capability: returning the native
 * unavailable status preserves demsrch.c's ordinary slow-reset fallback. */
PVOID NTAPI RtlProcessHeap(VOID) { return GetProcessHeap(); }
NTSTATUS NtVdmControl(VDMSERVICECLASS service, PVOID data)
{
    (void)service;
    (void)data;
    return STATUS_NOT_IMPLEMENTED;
}

int bx_ntvdm_demsrch_fcb_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*body)(void) = NULL;

    if (call == NULL) return 0;
    switch (call->service) {
    case 0x07u: body = demDeleteFCB; break;
    case 0x09u: body = demFindFirst; break;
    case 0x0au: body = demFindFirstFCB; break;
    case 0x0bu: body = demFindNext; break;
    case 0x0cu: body = demFindNextFCB; break;
    case 0x20u: body = demRenameFCB; break;
    case 0x2cu: body = demCreateFCB; break;
    case 0x2du: body = demOpenFCB; break;
    case 0x2eu: body = demCloseFCB; break;
    case 0x2fu: body = demFCBIO; break;
    case 0x30u: body = demDate16; break;
    case 0x31u: body = demGetFileInfo; break;
    case 0x3cu: body = demTerminatePDB; break;
    default: return 0;
    }
    return bx_ntvdm_demhndl_invoke_body(call, body);
}
