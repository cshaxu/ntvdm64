#ifndef NTDOS64_OPENNT_BOP_VDMREDIR_VRMSLOT_COMPAT_H
#define NTDOS64_OPENNT_BOP_VDMREDIR_VRMSLOT_COMPAT_H

/*
 * Compatibility include for directly retained VDMREDIR mailslot bodies.
 * OpenNT's vdmredir.h supplied these CCPU register macros from the NT4
 * product shell.  The source-shaped adapter-softpc facade retains the same
 * call form against a bounded copied call frame.
 */
#include <windows.h>

#include "adapter-softpc/opennt_ccpu_sas_facade.h"

#define SET_ERROR(err) { setAX((WORD)(err)); setCF(1); }
/* DIVERGENCE(BOP-DIV-059): OpenNT's record carries a raw process HANDLE;
 * the standalone composition carries only its opaque manager token.  Retain
 * the original close call shape while the existing CCPU/SAS facade releases
 * the matching mapped handle. */
#define VrpCloseMailslotHandle(token, handle) \
    ((void)bx_ntvdm_ccpu_sas_get_handle(0u, (token)), \
     bx_ntvdm_ccpu_sas_close_handle((handle)))

typedef unsigned short SELECTOR;
typedef struct {
    unsigned short Offset;
    SELECTOR Selector;
} ADDRESS16;

typedef struct _VR_MAILSLOT_INFO *PVR_MAILSLOT_INFO;
typedef struct _VR_MAILSLOT_INFO {
    PVR_MAILSLOT_INFO Next;
    WORD DosPdb;
    WORD Handle16;
    HANDLE Handle32;
    ADDRESS16 BufferAddress;
    SELECTOR Selector;
    DWORD MessageSize;
    DWORD NameLength;
    CHAR Name[2];
} VR_MAILSLOT_INFO;

typedef int (*bx_ntvdm_vrmslot_release_fn)(void *state, uint32_t token,
    DWORD *error_out);

PVR_MAILSLOT_INFO VrpAllocateMailslotStructure(DWORD name_length);
void VrpFreeMailslotStructure(PVR_MAILSLOT_INFO record);
void VrpLinkMailslotStructure(PVR_MAILSLOT_INFO record);
PVR_MAILSLOT_INFO VrpUnlinkMailslotStructure(WORD handle16);
PVR_MAILSLOT_INFO VrpMapMailslotHandle16(WORD handle16);
PVR_MAILSLOT_INFO VrpMapMailslotName(LPSTR name);
void VrpRemoveProcessMailslots(WORD dos_pdb);
void VrpRemoveProcessMailslotsWithRelease(WORD dos_pdb, void *state,
    bx_ntvdm_vrmslot_release_fn release);
void VrpResetMailslots(void *state, bx_ntvdm_vrmslot_release_fn release);

void VrPeekMailslot(void);
void VrTerminateMailslots(WORD DosPdb);
void bx_ntvdm_vrmslot_terminate_bop_body(void);

#endif
