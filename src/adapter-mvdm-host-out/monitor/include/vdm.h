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

NTSTATUS NtVdmControl(VDMSERVICECLASS Service, PVOID ServiceData);

#endif
