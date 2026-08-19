#ifndef BX_NTVDM_BOP_SHIM_DEMERROR_LOCK_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEMERROR_LOCK_SHIM_H

/* Compatibility surface for directly imported OpenNT demerror.c/demlock.c.
 * Diagnostics determine each missing historic binding before it is admitted. */
#include "demdasd_ioctl_shim.h"

/* demerror.c is the original owner of demClientError.  Earlier imports used
 * the temporary demhndl fallback macro; remove it here so demlock.c binds to
 * the directly imported owner instead of creating a duplicate symbol. */
#undef demClientError
#undef GetVDMAddr

/* Exact fixed guest layouts from src/opennt/base/mvdm/inc/dossvc.h and
 * src/opennt/base/mvdm/dos/dem/dem.h. */
#pragma pack(push, 1)
typedef struct vhe_s { char vhe_fbInt24, vhe_HrdErrCode, vhe_bDriveNum; } VHE, *PVHE;
typedef struct _SYSDEV { ULONG sdevNext; char sdevIgnore[6]; UCHAR sdevDevName[8]; } SYSDEV, *PSYSDEV;
typedef struct _SAVEDEMWORLD { USHORT ax,bx,cx,dx,ds,es,si,di,bp; ULONG iSVC; } SAVEDEMWORLD;
#pragma pack(pop)

extern ULONG CurrentISVC;
typedef VOID (*PFNSVC)(VOID);
extern PFNSVC apfnSVC[];
VOID demClientError(HANDLE file, CHAR drive);
ULONG demClientErrorEx(HANDLE file, CHAR drive, BOOL set_registers);
VOID demSaveHardErrInfo(VOID);
VOID demRestoreHardErrInfo(VOID);
#define GetVDMAddr(segment, offset) bx_ntvdm_demerror_get_vdm_addr(segment, offset)
#define GETULONG(hi, lo) ((DWORD)((((DWORD)(USHORT)(hi)) << 16) | (USHORT)(lo)))
/* Retry restores DS/ES only around its original handler call.  The current
 * typed result ABI has no selector delta, so a Direct retry retains the
 * copied selector and must decline unsupported cross-segment changes. */
#define setDS(value) ((void)(value))
#define SVC_DEMLASTSVC 0x49u

/* Declared by the original NT native runtime; modern SDK headers do not
 * expose all of these historical helpers through winternl.h. */
NTSTATUS bx_ntvdm_demerror_append_unicode(PUNICODE_STRING, PCWSTR);
NTSTATUS bx_ntvdm_demerror_open_symbolic_link(PHANDLE, ULONG,
    POBJECT_ATTRIBUTES);
NTSTATUS bx_ntvdm_demerror_query_symbolic_link(HANDLE, PUNICODE_STRING,
    PULONG);
BOOLEAN bx_ntvdm_demerror_equal_unicode(const UNICODE_STRING *,
    const UNICODE_STRING *, BOOLEAN);
#define RtlAppendUnicodeToString bx_ntvdm_demerror_append_unicode
#define NtOpenSymbolicLinkObject bx_ntvdm_demerror_open_symbolic_link
#define NtQuerySymbolicLinkObject bx_ntvdm_demerror_query_symbolic_link
#define RtlEqualUnicodeString bx_ntvdm_demerror_equal_unicode
int bx_ntvdm_demerror_lock_invoke(bx_ntvdm_demhndl_call *call);
LPVOID bx_ntvdm_demerror_get_vdm_addr(USHORT segment, USHORT offset);
void bx_ntvdm_demerror_flush_hard_error(void);

#endif
