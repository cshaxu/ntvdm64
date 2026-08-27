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

/* Reached exact-layout records from OpenNT's internal vdm.h. DPMI source
 * stores registration records here before its historical monitor/emulator
 * path consumes them. The carrier is session-thread storage, not a kernel
 * VDM object and not an instruction-execution API. */
#pragma pack(push, 1)
typedef struct _Vdm_InterruptHandler {
    USHORT CsSelector;
    USHORT Flags;
    ULONG Eip;
} VDM_INTERRUPTHANDLER, *PVDM_INTERRUPTHANDLER;
#pragma pack(pop)

typedef struct _Vdm_FaultHandler {
    USHORT CsSelector;
    USHORT SsSelector;
    ULONG Eip;
    ULONG Esp;
    ULONG Flags;
} VDM_FAULTHANDLER, *PVDM_FAULTHANDLER;

typedef struct _VDMSET_INT21_HANDLER_DATA {
    ULONG Selector;
    ULONG Offset;
    BOOLEAN Gate32;
} VDMSET_INT21_HANDLER_DATA, *PVDMSET_INT21_HANDLER_DATA;

typedef struct _Vdm_Tib {
    /* DIVERGENCE(ADAPTER-MONITOR-004): The selected dpmi32 source revision
     * names PmStackInfo whereas the nearby later declaration calls the same
     * reached layout DpmiInfo. Retain the source spelling and exact field
     * layout; neither name exposes a native pointer. */
    VDM_PMSTACKINFO PmStackInfo;
    VDM_INTERRUPTHANDLER VdmInterruptHandlers[256];
    VDM_FAULTHANDLER VdmFaultHandlers[32];
    CONTEXT VdmContext;
} VDM_TIB, *PVDM_TIB;

extern __declspec(thread) VDM_TIB VdmTib;
extern __declspec(thread) ULONG mvdm_monitor_ntvdm_state;

#define pNtVDMState (&mvdm_monitor_ntvdm_state)
#define VDM_32BIT_APP 0x00000100UL
#define VDM_INT_INT_GATE 0x00000001UL
#define VDM_INT_32 0x00000002UL
#define EFLAGS_TF_MASK 0x00000100UL

/* Bind the source-shaped NtCurrentTeb()->Vdm lookup to this thread's carrier.
 * The caller must already be inside an owned session worker. */
int mvdm_monitor_bind_current_thread(void);

NTSTATUS NtVdmControl(VDMSERVICECLASS Service, PVOID ServiceData);

#endif
