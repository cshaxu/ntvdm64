/* Test-only unavailable-medium contract for the VCD public device provider.
 * It compiles the product provider directly, replaces just QueryDosDeviceA
 * with an empty public namespace, and proves that no synthetic SERIALCOMM key
 * is opened.  This fixture is not linked into any deployed executable. */

#include <windows.h>
#include <stdio.h>

DWORD WINAPI s45_empty_query_dos_device(LPCSTR device, LPSTR targets,
                                        DWORD capacity)
{
    (void)device;
    if (capacity<2u) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    targets[0]=0;
    targets[1]=0;
    return 2;
}

#define QueryDosDeviceA s45_empty_query_dos_device
#include "../../src/ntvdm-exe/softpc/mvdm_shadow_registry.c"

int main(void)
{
    HKEY key=(HKEY)(ULONG_PTR)1;
    LONG status;

    if (!ntvdm_shadow_registry_initialize()) return 1;
    status=ntvdm_shadow_reg_open_key_ex(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DEVICEMAP\\SERIALCOMM",0,KEY_QUERY_VALUE,&key);
    ntvdm_shadow_registry_shutdown();
    if (status!=ERROR_FILE_NOT_FOUND || key!=NULL) return 2;
    puts("S45_VCD_PUBLIC_SERIAL_ABSENT_OK");
    return 0;
}
