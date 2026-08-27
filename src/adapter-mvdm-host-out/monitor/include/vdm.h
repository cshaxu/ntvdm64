/*
 * Reached same-shaped subset of the historical user-mode VDM control ABI.
 * The complete NT4 kernel VDM product surface is intentionally not imported.
 */
#ifndef ADAPTER_VDM_MONITOR_VDM_H
#define ADAPTER_VDM_MONITOR_VDM_H

#include <nt.h>

typedef enum _VdmServiceClass {
    VdmStartExecution,
    VdmQueueInterrupt,
    VdmDelayInterrupt,
    VdmInitialize,
    VdmFeatures,
    VdmSetInt21Handler,
    VdmQueryDir,
    VdmPrinterDirectIoOpen,
    VdmPrinterDirectIoClose,
    VdmPrinterInitialize,
    VdmSetLdtEntries,
    VdmSetProcessLdtInfo,
    VdmAdlibEmulation,
    VdmPMCliControl,
    VdmQueryVdmProcess
} VDMSERVICECLASS, *PVDMSERVICECLASS;

typedef struct _VdmQueryDirInfo {
    HANDLE FileHandle;
    PVOID FileInformation;
    ULONG Length;
    PUNICODE_STRING FileName;
    ULONG FileIndex;
} VDMQUERYDIRINFO, *PVDMQUERYDIRINFO;

/* Reached, same-shaped user-mode subset of the historical per-VDM TIB.
 * The original DPMI sources use only PmStackInfo and pNtVDMState in this
 * recovery stage.  Storage is thread-local and belongs to the bound session;
 * it is not the removed NT kernel VDM control block. */
typedef struct _VdmPmStackInfo {
    USHORT LockCount;
    USHORT Flags;
    USHORT SsSelector;
    USHORT SaveSsSelector;
    ULONG SaveEsp;
    ULONG SaveEip;
    ULONG DosxIntIret;
    ULONG DosxIntIretD;
    ULONG DosxFaultIret;
    ULONG DosxFaultIretD;
    ULONG DosxRmReflector;
} VDM_PMSTACKINFO, *PVDM_PMSTACKINFO;

typedef struct _Vdm_Tib {
    VDM_PMSTACKINFO PmStackInfo;
} VDM_TIB, *PVDM_TIB;

extern __declspec(thread) VDM_TIB VdmTib;
extern __declspec(thread) ULONG mvdm_monitor_ntvdm_state;

#define pNtVDMState (&mvdm_monitor_ntvdm_state)
#define VDM_32BIT_APP 0x00000100UL

NTSTATUS NtVdmControl(VDMSERVICECLASS Service, PVOID ServiceData);

#endif
