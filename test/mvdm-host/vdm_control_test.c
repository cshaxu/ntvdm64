/* Original caller declaration against the standalone compatibility provider. */
#include <nt.h>
#include <vdm.h>
#include <stdio.h>

/* XP vdmtib.inc: offsets consumed by the original x86 WOW assembly. */
typedef char tib_interrupt_table_offset[(FIELD_OFFSET(VDM_TIB, VdmInterruptTable) == 0x004) ? 1 : -1];
typedef char tib_fault_table_offset[(FIELD_OFFSET(VDM_TIB, VdmFaultTable) == 0x008) ? 1 : -1];
typedef char tib_monitor_offset[(FIELD_OFFSET(VDM_TIB, MonitorContext) == 0x00c) ? 1 : -1];
typedef char tib_guest_offset[(FIELD_OFFSET(VDM_TIB, VdmContext) == 0x2d8) ? 1 : -1];
typedef char tib_event_offset[(FIELD_OFFSET(VDM_TIB, EventInfo) == 0x5a4) ? 1 : -1];
typedef char tib_dpmi_offset[(FIELD_OFFSET(VDM_TIB, DpmiInfo) == 0x634) ? 1 : -1];

typedef char tib_full_size[(sizeof(VDM_TIB) == 0x674) ? 1 : -1];

typedef char service_abi_width[(sizeof(VDMSERVICECLASS) == 4) ? 1 : -1];

typedef char cli_request_abi_width[(sizeof(VDM_PM_CLI_DATA) == sizeof(ULONG)) ? 1 : -1];

int main(void)
{
    int service;
    ULONG data = 0x12345678;
    for (service = VdmStartExecution; service <= VdmPMCliControl; ++service) {
        SetLastError(ERROR_ACCESS_DENIED);
        if (NtVdmControl((VDMSERVICECLASS)service, &data) != STATUS_NOT_IMPLEMENTED ||
            data != 0x12345678 || GetLastError() != ERROR_ACCESS_DENIED) return 1;
    }
    if (NtVdmControl(VdmQueryDir, NULL) != STATUS_NOT_IMPLEMENTED ||
        NtVdmControl(VdmQueryDir, (PVOID)1) != STATUS_NOT_IMPLEMENTED) return 2;
    {
        VDM_PM_CLI_DATA cli;
        for (cli.Control = PM_CLI_CONTROL_DISABLE;
             cli.Control <= PM_CLI_CONTROL_CLEAR; ++cli.Control) {
            ULONG before = cli.Control;
            SetLastError(ERROR_ACCESS_DENIED);
            if (NtVdmControl(VdmPMCliControl, &cli) != STATUS_NOT_IMPLEMENTED ||
                cli.Control != before || GetLastError() != ERROR_ACCESS_DENIED) return 3;
        }
    }
    puts("PASS original NtVdmControl ABI, unavailable services and untouched data");
    return 0;
}
