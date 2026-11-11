#include <nt.h>

/* This fixture uses a deliberately impossible object name. It proves only
 * that the same-shaped OpenNT NT I/O facade preserves failure; it never opens
 * a volume, creates a file or changes a host resource. */
int main(void)
{
    static WCHAR impossible_name[] = L"\\DosDevices\\mvdm-no-such-storage-endpoint";
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES attributes;
    IO_STATUS_BLOCK status;
    HANDLE handle = INVALID_HANDLE_VALUE;
    NTSTATUS result;

    RtlInitUnicodeString(&name, impossible_name);
    InitializeObjectAttributes(&attributes, &name, OBJ_CASE_INSENSITIVE,
                               NULL, NULL);
    result = NtOpenFile(&handle, FILE_READ_ATTRIBUTES | SYNCHRONIZE,
                        &attributes, &status,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE);
    if (NT_SUCCESS(result)) {
        CloseHandle(handle);
        return 1;
    }
    return 0;
}
