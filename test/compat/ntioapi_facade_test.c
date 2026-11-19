#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>
#include <devioctl.h>
#include "compat/include/ntioapi.h"
#include "compat/include/dem_restart_hint.h"

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
    {
        DEM_RESTART_HINT hint;
        FILE_BOTH_DIR_INFORMATION entry = {0};
        PVOID next = NULL;
        DemCaptureRestartHint(&hint, NULL);
        if (hint.Length) return 2;
        entry.FileIndex = 7; entry.FileNameLength = sizeof(WCHAR);
        entry.FileName[0] = L'A';
        DemCaptureRestartHint(&hint, &entry);
        if (hint.Index != 7 || hint.Length != sizeof(WCHAR) || hint.Name[0] != L'A') return 3;
        result = DemFindRestartHint(&hint, INVALID_HANDLE_VALUE, &entry, sizeof(entry), &next);
        /* -1 is also the current-process pseudo handle. The kernel may report
         * type mismatch rather than invalid handle; preserve its exact result. */
        {
            typedef NTSTATUS (NTAPI *native_query)(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,
                PVOID,PVOID,ULONG,ULONG,BOOLEAN,PUNICODE_STRING,BOOLEAN);
            ULONG_PTR native_status[2];
            native_query query=(native_query)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryDirectoryFile");
            if (!query || NT_SUCCESS(result) || next != NULL ||
                result != query(INVALID_HANDLE_VALUE,NULL,NULL,NULL,native_status,
                    &entry,sizeof(entry),3,FALSE,NULL,TRUE)) return 4;
        }
    }
    result = NtQueryVolumeInformationFile(INVALID_HANDLE_VALUE, &status,
        &device, sizeof(device), FileFsDeviceInformation);
    return result == STATUS_NOT_IMPLEMENTED;
}
