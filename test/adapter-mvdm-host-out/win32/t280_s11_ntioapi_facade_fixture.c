#include <nt.h>
#include "dem.h"

_Static_assert(sizeof(IO_STATUS_BLOCK) == sizeof(NTSTATUS) + sizeof(ULONG),
    "IO_STATUS_BLOCK source layout");
_Static_assert(sizeof(((FILE_BOTH_DIR_INFORMATION *)0)->ShortName) == 12 * sizeof(WCHAR),
    "FILE_BOTH_DIR_INFORMATION source layout");
_Static_assert(sizeof(FILE_FS_DEVICE_INFORMATION) == 2 * sizeof(ULONG),
    "FILE_FS_DEVICE_INFORMATION source layout");

int main(void)
{
    IO_STATUS_BLOCK status;
    FILE_FS_DEVICE_INFORMATION device;
    NTSTATUS result;
    NTSTATUS (NTAPI *directory)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID,
        PIO_STATUS_BLOCK, PVOID, ULONG, ADAPTER_FILE_INFORMATION_CLASS,
        BOOLEAN, PUNICODE_STRING, BOOLEAN) = NtQueryDirectoryFile;
    NTSTATUS (NTAPI *volume)(HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG,
        ADAPTER_FS_INFORMATION_CLASS) = NtQueryVolumeInformationFile;
    if (directory == 0 || volume == 0) return 1;
    result = NtQueryVolumeInformationFile(INVALID_HANDLE_VALUE, &status,
        &device, sizeof(device), AdapterFileFsDeviceInformation);
    return result == STATUS_NOT_IMPLEMENTED;
}
