#include <windows.h>
#include <WinHvPlatform.h>
#include <stdio.h>

int wmain(void)
{
    WHV_CAPABILITY capability = {0};
    UINT32 written = 0u;
    const HRESULT result = WHvGetCapability(WHvCapabilityCodeHypervisorPresent,
        &capability, sizeof(capability.HypervisorPresent), &written);

    if (FAILED(result) || written != sizeof(capability.HypervisorPresent)) {
        fwprintf(stderr, L"whpx-probe: WHvGetCapability failed: 0x%08lX bytes=%lu\n",
            (unsigned long)result, (unsigned long)written);
        return 3;
    }
    wprintf(L"whpx-probe: hypervisor-present=%d\n", capability.HypervisorPresent);
    return capability.HypervisorPresent ? 0 : 2;
}
