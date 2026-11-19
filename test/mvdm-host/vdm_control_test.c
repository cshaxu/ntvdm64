/* Original caller declaration against the standalone compatibility provider. */
#include <nt.h>
#include <vdm.h>
#include <stdio.h>

typedef char service_abi_width[(sizeof(VDMSERVICECLASS) == 4) ? 1 : -1];

int main(void)
{
    int service;
    ULONG data = 0x12345678;
    for (service = VdmStartExecution; service <= VdmQueryVdmProcess; ++service) {
        SetLastError(ERROR_ACCESS_DENIED);
        if (NtVdmControl((VDMSERVICECLASS)service, &data) != STATUS_NOT_IMPLEMENTED ||
            data != 0x12345678 || GetLastError() != ERROR_ACCESS_DENIED) return 1;
    }
    if (NtVdmControl(VdmQueryDir, NULL) != STATUS_NOT_IMPLEMENTED ||
        NtVdmControl(VdmQueryDir, (PVOID)1) != STATUS_NOT_IMPLEMENTED) return 2;
    puts("PASS original NtVdmControl ABI, unavailable services and untouched data");
    return 0;
}
