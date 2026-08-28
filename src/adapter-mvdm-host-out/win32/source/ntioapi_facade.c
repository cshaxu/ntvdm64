#include <nt.h>

#undef IO_STATUS_BLOCK
#undef PIO_STATUS_BLOCK

typedef NTSTATUS (NTAPI *PFN_NT_QUERY_DIRECTORY_FILE)(
    HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG,
    ADAPTER_FILE_INFORMATION_CLASS, BOOLEAN, PUNICODE_STRING, BOOLEAN);
typedef NTSTATUS (NTAPI *PFN_NT_QUERY_VOLUME_INFORMATION_FILE)(
    HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, ADAPTER_FS_INFORMATION_CLASS);
typedef NTSTATUS (NTAPI *PFN_NT_OPEN_SYMBOLIC_LINK_OBJECT)(
    PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
typedef NTSTATUS (NTAPI *PFN_NT_QUERY_SYMBOLIC_LINK_OBJECT)(
    HANDLE, PUNICODE_STRING, PULONG);
typedef NTSTATUS (NTAPI *PFN_NT_OPEN_FILE)(
    PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, ULONG, ULONG);
typedef NTSTATUS (NTAPI *PFN_NT_QUERY_OBJECT)(
    HANDLE, ULONG, PVOID, ULONG, PULONG);
typedef NTSTATUS (NTAPI *PFN_NT_QUERY_INFORMATION_FILE)(
    HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, ULONG);
typedef NTSTATUS (NTAPI *PFN_NT_FS_CONTROL_FILE)(
    HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, ULONG,
    PVOID, ULONG, PVOID, ULONG);
typedef NTSTATUS (NTAPI *PFN_NT_DEVICE_IO_CONTROL_FILE)(
    HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, ULONG,
    PVOID, ULONG, PVOID, ULONG);

static FARPROC ntioapi_lookup(const char *name)
{
    HMODULE module = GetModuleHandleW(L"ntdll.dll");
    return module == NULL ? NULL : GetProcAddress(module, name);
}

static void ntioapi_copy_status(POPENNT_IO_STATUS_BLOCK destination,
                                const IO_STATUS_BLOCK *source)
{
    if (destination != NULL && source != NULL) {
        destination->Status = source->Status;
        destination->Information = (ULONG)source->Information;
    }
}

NTSTATUS NTAPI opennt_NtQueryDirectoryFile(
    HANDLE file, HANDLE event, PIO_APC_ROUTINE apc, PVOID context,
    POPENNT_IO_STATUS_BLOCK status, PVOID information, ULONG length,
    ADAPTER_FILE_INFORMATION_CLASS information_class, BOOLEAN one_entry,
    PUNICODE_STRING name, BOOLEAN restart)
{
    IO_STATUS_BLOCK native_status;
    NTSTATUS result;
    PFN_NT_QUERY_DIRECTORY_FILE entry =
        (PFN_NT_QUERY_DIRECTORY_FILE)ntioapi_lookup("NtQueryDirectoryFile");
    if (entry == NULL) return STATUS_NOT_IMPLEMENTED;
    result = entry(file, event, apc, context, &native_status, information, length,
                   information_class, one_entry, name, restart);
    ntioapi_copy_status(status, &native_status);
    return result;
}

NTSTATUS NTAPI opennt_NtQueryVolumeInformationFile(
    HANDLE file, POPENNT_IO_STATUS_BLOCK status, PVOID information, ULONG length,
    ADAPTER_FS_INFORMATION_CLASS information_class)
{
    IO_STATUS_BLOCK native_status;
    NTSTATUS result;
    PFN_NT_QUERY_VOLUME_INFORMATION_FILE entry =
        (PFN_NT_QUERY_VOLUME_INFORMATION_FILE)ntioapi_lookup("NtQueryVolumeInformationFile");
    if (entry == NULL) return STATUS_NOT_IMPLEMENTED;
    result = entry(file, &native_status, information, length, information_class);
    ntioapi_copy_status(status, &native_status);
    return result;
}

NTSTATUS NTAPI opennt_NtOpenSymbolicLinkObject(
    PHANDLE link, ACCESS_MASK access, POBJECT_ATTRIBUTES attributes)
{
    PFN_NT_OPEN_SYMBOLIC_LINK_OBJECT entry =
        (PFN_NT_OPEN_SYMBOLIC_LINK_OBJECT)ntioapi_lookup("NtOpenSymbolicLinkObject");
    return entry == NULL ? STATUS_NOT_IMPLEMENTED : entry(link, access, attributes);
}

NTSTATUS NTAPI opennt_NtQuerySymbolicLinkObject(
    HANDLE link, PUNICODE_STRING target, PULONG returned_length)
{
    PFN_NT_QUERY_SYMBOLIC_LINK_OBJECT entry =
        (PFN_NT_QUERY_SYMBOLIC_LINK_OBJECT)ntioapi_lookup("NtQuerySymbolicLinkObject");
    return entry == NULL ? STATUS_NOT_IMPLEMENTED :
        entry(link, target, returned_length);
}

NTSTATUS NTAPI opennt_NtOpenFile(
    PHANDLE file, ACCESS_MASK access, POBJECT_ATTRIBUTES attributes,
    POPENNT_IO_STATUS_BLOCK status, ULONG share, ULONG options)
{
    IO_STATUS_BLOCK native_status;
    NTSTATUS result;
    PFN_NT_OPEN_FILE entry = (PFN_NT_OPEN_FILE)ntioapi_lookup("NtOpenFile");
    if (entry == NULL) return STATUS_NOT_IMPLEMENTED;
    result = entry(file, access, attributes, &native_status, share, options);
    ntioapi_copy_status(status, &native_status);
    return result;
}

NTSTATUS NTAPI opennt_NtQueryObject(
    HANDLE handle, ULONG information_class, PVOID information,
    ULONG information_length, PULONG return_length)
{
    PFN_NT_QUERY_OBJECT entry =
        (PFN_NT_QUERY_OBJECT)ntioapi_lookup("NtQueryObject");
    return entry == NULL ? STATUS_NOT_IMPLEMENTED :
        entry(handle, information_class, information, information_length,
              return_length);
}

NTSTATUS NTAPI opennt_NtQueryInformationFile(
    HANDLE file, POPENNT_IO_STATUS_BLOCK status, PVOID information,
    ULONG length, ADAPTER_FILE_INFORMATION_CLASS information_class)
{
    IO_STATUS_BLOCK native_status;
    NTSTATUS result;
    PFN_NT_QUERY_INFORMATION_FILE entry =
        (PFN_NT_QUERY_INFORMATION_FILE)ntioapi_lookup("NtQueryInformationFile");
    if (entry == NULL) return STATUS_NOT_IMPLEMENTED;
    result = entry(file, &native_status, information, length, (ULONG)information_class);
    ntioapi_copy_status(status, &native_status);
    return result;
}

NTSTATUS NTAPI opennt_NtFsControlFile(
    HANDLE file, HANDLE event, PIO_APC_ROUTINE apc, PVOID context,
    POPENNT_IO_STATUS_BLOCK status, ULONG code, PVOID input, ULONG input_length,
    PVOID output, ULONG output_length)
{
    IO_STATUS_BLOCK native_status;
    NTSTATUS result;
    PFN_NT_FS_CONTROL_FILE entry =
        (PFN_NT_FS_CONTROL_FILE)ntioapi_lookup("NtFsControlFile");
    if (entry == NULL) return STATUS_NOT_IMPLEMENTED;
    result = entry(file, event, apc, context, &native_status, code, input,
                   input_length, output, output_length);
    ntioapi_copy_status(status, &native_status);
    return result;
}

NTSTATUS NTAPI opennt_NtDeviceIoControlFile(
    HANDLE file, HANDLE event, PIO_APC_ROUTINE apc, PVOID context,
    POPENNT_IO_STATUS_BLOCK status, ULONG code, PVOID input, ULONG input_length,
    PVOID output, ULONG output_length)
{
    IO_STATUS_BLOCK native_status;
    NTSTATUS result;
    PFN_NT_DEVICE_IO_CONTROL_FILE entry =
        (PFN_NT_DEVICE_IO_CONTROL_FILE)ntioapi_lookup("NtDeviceIoControlFile");
    if (entry == NULL) return STATUS_NOT_IMPLEMENTED;
    result = entry(file, event, apc, context, &native_status, code, input,
                   input_length, output, output_length);
    ntioapi_copy_status(status, &native_status);
    return result;
}
