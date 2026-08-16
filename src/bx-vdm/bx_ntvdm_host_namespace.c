#include "bx_ntvdm_host_namespace.h"

#include <string.h>
#include <wctype.h>
#include <winternl.h>

typedef struct bx_ntvdm_host_namespace_internal_entry_v1 {
    bx_ntvdm_host_namespace_entry_v1 projected;
    wchar_t host_name[MAX_PATH];
} bx_ntvdm_host_namespace_internal_entry_v1;

NTSYSAPI NTSTATUS NTAPI NtQueryDirectoryFile(HANDLE file, HANDLE event,
    PIO_APC_ROUTINE apc_routine, PVOID apc_context,
    PIO_STATUS_BLOCK status_block, PVOID file_information, ULONG length,
    FILE_INFORMATION_CLASS information_class, BOOLEAN return_single_entry,
    PUNICODE_STRING file_name, BOOLEAN restart_scan);
NTSYSAPI NTSTATUS NTAPI NtSetInformationFile(HANDLE file,
    PIO_STATUS_BLOCK status_block, PVOID file_information,
    ULONG length, FILE_INFORMATION_CLASS information_class);

/* FileIdBothDirectoryInformation (37) is a documented NT directory-query
 * information class, but its record is not declared by the user-mode SDK.
 * Keep the exact read-only wire layout local to the adapter: it is consumed
 * immediately and no native record, path, or handle crosses this module. */
#define BX_NTVDM_FILE_ID_BOTH_DIRECTORY_INFORMATION_CLASS \
    ((FILE_INFORMATION_CLASS)37)
/* FileRenameInformation is the stable NT file-information class 10. The
 * user-mode SDK exposes the payload but not this enum label. */
#define BX_NTVDM_FILE_RENAME_INFORMATION_CLASS ((FILE_INFORMATION_CLASS)10)
typedef struct bx_ntvdm_file_id_both_dir_information_v1 {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    LARGE_INTEGER FileId;
    WCHAR FileName[1];
} bx_ntvdm_file_id_both_dir_information_v1;

static uint32_t bx_ntvdm_host_namespace_bit(uint8_t index)
{
    return UINT32_C(1) << index;
}

static int bx_ntvdm_host_namespace_file_info(HANDLE handle,
    BY_HANDLE_FILE_INFORMATION *out)
{
    return handle != INVALID_HANDLE_VALUE && out != 0 &&
        GetFileInformationByHandle(handle, out) != 0;
}

static int bx_ntvdm_host_namespace_ascii_83(const wchar_t *source,
    wchar_t target[13])
{
    size_t index, base = 0u, extension = 0u;
    int saw_dot = 0;
    if (source == 0 || source[0] == L'\0') return 0;
    for (index = 0u; source[index] != L'\0'; ++index) {
        wchar_t value = towupper((wint_t)source[index]);
        if (value == L'.') {
            if (saw_dot || base == 0u) return 0;
            saw_dot = 1;
        } else {
            int allowed = (value >= L'A' && value <= L'Z') ||
                (value >= L'0' && value <= L'9') || value == L'$' ||
                value == L'%' || value == L'\'' || value == L'-' ||
                value == L'_' || value == L'@' || value == L'~' ||
                value == L'!' || value == L'#' || value == L'(' ||
                value == L')' || value == L'{' || value == L'}' ||
                value == L'^';
            if (!allowed) return 0;
            if (saw_dot) ++extension; else ++base;
            if (base > 8u || extension > 3u) return 0;
        }
        if (index >= 12u) return 0;
        target[index] = value;
    }
    if (base == 0u || (saw_dot && extension == 0u)) return 0;
    target[index] = L'\0';
    return 1;
}

static int bx_ntvdm_host_namespace_project_name(
    const bx_ntvdm_file_id_both_dir_information_v1 *info, wchar_t output[13])
{
    wchar_t source[MAX_PATH];
    size_t length;
    if (info == 0 || output == 0 ||
        info->FileNameLength == 0u ||
        info->FileNameLength / sizeof(wchar_t) >= MAX_PATH) return 0;
    if (info->ShortNameLength != 0) {
        length = (size_t)(unsigned char)info->ShortNameLength / sizeof(wchar_t);
        if (length == 0u || length >= 13u) return 0;
        memcpy(source, info->ShortName, length * sizeof(*source));
        source[length] = L'\0';
    } else {
        length = info->FileNameLength / sizeof(wchar_t);
        memcpy(source, info->FileName, length * sizeof(*source));
        source[length] = L'\0';
    }
    return bx_ntvdm_host_namespace_ascii_83(source, output);
}

static void bx_ntvdm_host_namespace_times(const FILETIME *utc,
    uint16_t *time_out, uint16_t *date_out)
{
    FILETIME local;
    WORD date, time;
    if (time_out == 0 || date_out == 0) return;
    *time_out = 0u; *date_out = 0u;
    if (utc != 0 && FileTimeToLocalFileTime(utc, &local) &&
        FileTimeToDosDateTime(&local, &date, &time)) {
        *time_out = time; *date_out = date;
    }
}

static int bx_ntvdm_host_namespace_collect(HANDLE directory,
    bx_ntvdm_host_namespace_internal_entry_v1 *items, uint32_t capacity,
    uint32_t *out_count)
{
    uint8_t *buffer;
    uint32_t count = 0u;
    int result = BX_NTVDM_HOST_NAMESPACE_V1_OK;
    BOOLEAN restart_scan = TRUE;
    if (directory == INVALID_HANDLE_VALUE || items == 0 || out_count == 0 ||
        capacity == 0u || capacity > BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES)
        return BX_NTVDM_HOST_NAMESPACE_V1_REJECTED;
    buffer = (uint8_t *)HeapAlloc(GetProcessHeap(), 0u, 65536u);
    if (buffer == 0) return BX_NTVDM_HOST_NAMESPACE_V1_REJECTED;
    for (;;) {
        bx_ntvdm_file_id_both_dir_information_v1 *info;
        IO_STATUS_BLOCK status_block;
        NTSTATUS status = NtQueryDirectoryFile(directory, 0, 0, 0,
            &status_block, buffer, 65536u,
            BX_NTVDM_FILE_ID_BOTH_DIRECTORY_INFORMATION_CLASS,
            FALSE, 0, restart_scan);
        restart_scan = FALSE;
        if (status < 0) {
            if (status == (NTSTATUS)0x80000006L) break;
            result = BX_NTVDM_HOST_NAMESPACE_V1_REJECTED;
            break;
        }
        for (info = (bx_ntvdm_file_id_both_dir_information_v1 *)buffer;;) {
            size_t name_length = info->FileNameLength / sizeof(wchar_t);
            uint32_t index;
            if (name_length == 1u && info->FileName[0] == L'.') goto next;
            if (name_length == 2u && info->FileName[0] == L'.' && info->FileName[1] == L'.') goto next;
            if (count == capacity) { result = BX_NTVDM_HOST_NAMESPACE_V1_CAPACITY; goto done; }
            if ((info->FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u ||
                !bx_ntvdm_host_namespace_project_name(info,
                    items[count].projected.dos_name)) goto next;
            for (index = 0u; index < count; ++index) {
                if (_wcsicmp(items[index].projected.dos_name,
                        items[count].projected.dos_name) == 0) {
                    result = BX_NTVDM_HOST_NAMESPACE_V1_COLLISION;
                    goto done;
                }
            }
            if (name_length >= MAX_PATH) { result = BX_NTVDM_HOST_NAMESPACE_V1_REJECTED; goto done; }
            memcpy(items[count].host_name, info->FileName, name_length * sizeof(wchar_t));
            items[count].host_name[name_length] = L'\0';
            items[count].projected.attributes = info->FileAttributes & 0x3fu;
            items[count].projected.byte_count = ((uint64_t)info->EndOfFile.HighPart << 32) |
                (uint32_t)info->EndOfFile.LowPart;
            bx_ntvdm_host_namespace_times((const FILETIME *)&info->LastWriteTime,
                &items[count].projected.dos_time, &items[count].projected.dos_date);
            ++count;
next:
            if (info->NextEntryOffset == 0u) break;
            info = (bx_ntvdm_file_id_both_dir_information_v1 *)
                ((uint8_t *)info + info->NextEntryOffset);
        }
    }
done:
    HeapFree(GetProcessHeap(), 0u, buffer);
    *out_count = count;
    return result;
}

static HANDLE bx_ntvdm_host_namespace_open_child(HANDLE parent,
    const wchar_t *name)
{
    UNICODE_STRING object_name;
    OBJECT_ATTRIBUTES attributes;
    IO_STATUS_BLOCK status_block;
    HANDLE child = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info;
    NTSTATUS status;
    size_t length;
    if (parent == INVALID_HANDLE_VALUE || name == 0 || (length = wcslen(name)) == 0u ||
        length > UINT16_MAX / sizeof(wchar_t)) return INVALID_HANDLE_VALUE;
    object_name.Buffer = (PWSTR)name;
    object_name.Length = (USHORT)(length * sizeof(wchar_t));
    object_name.MaximumLength = object_name.Length;
    InitializeObjectAttributes(&attributes, &object_name,
        OBJ_CASE_INSENSITIVE | OBJ_DONT_REPARSE, parent, 0);
    status = NtCreateFile(&child, GENERIC_READ | SYNCHRONIZE, &attributes,
        &status_block, 0, FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN,
        FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT |
        FILE_OPEN_REPARSE_POINT | FILE_OPEN_FOR_BACKUP_INTENT, 0, 0u);
    if (status < 0 || !bx_ntvdm_host_namespace_file_info(child, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) !=
            FILE_ATTRIBUTE_DIRECTORY) {
        if (child != INVALID_HANDLE_VALUE) CloseHandle(child);
        return INVALID_HANDLE_VALUE;
    }
    return child;
}

static int bx_ntvdm_host_namespace_file_relative(const wchar_t *relative)
{
    const wchar_t *component;
    if (relative == 0 || relative[0] == L'\0' || wcslen(relative) >= MAX_PATH)
        return 0;
    component = relative;
    for (;;) {
        const wchar_t *end = wcschr(component, L'\\');
        wchar_t canonical[13];
        size_t length = end == 0 ? wcslen(component) : (size_t)(end - component);
        if (length == 0u || length >= 13u) return 0;
        memcpy(canonical, component, length * sizeof(wchar_t));
        canonical[length] = L'\0';
        if (!bx_ntvdm_host_namespace_ascii_83(canonical, canonical)) return 0;
        if (end == 0) return 1;
        component = end + 1u;
        if (*component == L'\0') return 0;
    }
}

static ULONG bx_ntvdm_host_namespace_disposition(DWORD value)
{
    switch (value) {
    case CREATE_NEW: return FILE_CREATE;
    case CREATE_ALWAYS: return FILE_OVERWRITE_IF;
    case OPEN_EXISTING: return FILE_OPEN;
    case OPEN_ALWAYS: return FILE_OPEN_IF;
    case TRUNCATE_EXISTING: return FILE_OVERWRITE;
    default: return 0u;
    }
}

void bx_ntvdm_host_namespace_v1_release(bx_ntvdm_host_namespace_v1 *space)
{
    uint32_t index;
    if (space == 0) return;
    for (index = 0u; index < 26u; ++index) {
        if (space->roots[index] != INVALID_HANDLE_VALUE && space->roots[index] != 0)
            CloseHandle(space->roots[index]);
    }
    memset(space, 0, sizeof(*space));
}

int bx_ntvdm_host_namespace_v1_valid(const bx_ntvdm_host_namespace_v1 *space)
{
    if (space == 0 || space->magic != BX_NTVDM_HOST_NAMESPACE_V1_MAGIC ||
        space->version != BX_NTVDM_HOST_NAMESPACE_V1_VERSION ||
        !bx_ntvdm_host_drive_snapshot_v1_valid(&space->snapshot) ||
        (space->available_mask & ~space->snapshot.admitted_mask) != 0u) return 0;
    return 1;
}

int bx_ntvdm_host_namespace_v1_initialize(bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot)
{
    uint32_t index;
    if (space == 0 || !bx_ntvdm_host_drive_snapshot_v1_valid(snapshot)) return 0;
    memset(space, 0, sizeof(*space));
    for (index = 0u; index < 26u; ++index) space->roots[index] = INVALID_HANDLE_VALUE;
    space->magic = BX_NTVDM_HOST_NAMESPACE_V1_MAGIC;
    space->version = BX_NTVDM_HOST_NAMESPACE_V1_VERSION;
    memcpy(&space->snapshot, snapshot, sizeof(*snapshot));
    for (index = 0u; index < 26u; ++index) {
        wchar_t root[] = { (wchar_t)(L'A' + index), L':', L'\\', L'\0' };
        BY_HANDLE_FILE_INFORMATION info;
        HANDLE handle;
        if ((snapshot->admitted_mask & bx_ntvdm_host_namespace_bit((uint8_t)index)) == 0u) continue;
        /* A root-relative child open requires traverse on the retained root.
         * This remains a read/list capability; it grants no host mutation. */
        handle = CreateFileW(root, GENERIC_READ | SYNCHRONIZE,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, 0);
        if (!bx_ntvdm_host_namespace_file_info(handle, &info) ||
            (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) !=
            FILE_ATTRIBUTE_DIRECTORY) {
            if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
            continue;
        }
        space->roots[index] = handle;
        space->available_mask |= bx_ntvdm_host_namespace_bit((uint8_t)index);
    }
    return bx_ntvdm_host_namespace_v1_valid(space);
}

static int bx_ntvdm_host_namespace_v1_open_directory_ex(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, ACCESS_MASK desired_access,
    ULONG disposition, HANDLE *handle_out, DWORD *win32_error_out);

int bx_ntvdm_host_namespace_v1_enumerate(const bx_ntvdm_host_namespace_v1 *space,
    uint8_t drive_index, const wchar_t *relative_directory,
    bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_capacity,
    uint32_t *out_count)
{
    HANDLE current = INVALID_HANDLE_VALUE;
    bx_ntvdm_host_namespace_internal_entry_v1 *items;
    uint32_t count = 0u, index;
    DWORD open_error = ERROR_INVALID_PARAMETER;
    int result;
    if (out_count != 0) *out_count = 0u;
    if (!bx_ntvdm_host_namespace_v1_valid(space) || drive_index >= 26u ||
        relative_directory == 0 || entries == 0 || out_count == 0 || entry_capacity == 0u ||
        entry_capacity > BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES ||
        (space->available_mask & bx_ntvdm_host_namespace_bit(drive_index)) == 0u)
        return BX_NTVDM_HOST_NAMESPACE_V1_REJECTED;
    if (relative_directory[0] == L'\0') current = space->roots[drive_index];
    else if (!bx_ntvdm_host_namespace_v1_open_directory_ex(space, drive_index,
            relative_directory, FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES,
            FILE_OPEN, &current, &open_error)) {
        SetLastError(open_error);
        return BX_NTVDM_HOST_NAMESPACE_V1_REJECTED;
    }
    items = (bx_ntvdm_host_namespace_internal_entry_v1 *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        entry_capacity * sizeof(*items));
    if (items == 0) { if (current != space->roots[drive_index]) CloseHandle(current); return BX_NTVDM_HOST_NAMESPACE_V1_REJECTED; }
    result = bx_ntvdm_host_namespace_collect(current, items, entry_capacity, &count);
    if (current != space->roots[drive_index]) CloseHandle(current);
    if (result == BX_NTVDM_HOST_NAMESPACE_V1_OK) {
        for (index = 0u; index < count; ++index) {
            uint32_t other;
            for (other = index + 1u; other < count; ++other) {
                if (wcscmp(items[other].projected.dos_name, items[index].projected.dos_name) < 0) {
                    bx_ntvdm_host_namespace_internal_entry_v1 swap = items[index];
                    items[index] = items[other]; items[other] = swap;
                }
            }
            entries[index] = items[index].projected;
        }
        *out_count = count;
    }
    HeapFree(GetProcessHeap(), 0u, items);
    return result;
}

static int bx_ntvdm_host_namespace_v1_open_directory_ex(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, ACCESS_MASK desired_access,
    ULONG disposition, HANDLE *handle_out, DWORD *win32_error_out);

int bx_ntvdm_host_namespace_v1_directory_exists(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_directory)
{
    return bx_ntvdm_host_namespace_v1_directory_exists_ex(space, drive_index,
        relative_directory, 0);
}

int bx_ntvdm_host_namespace_v1_directory_exists_ex(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_directory, DWORD *win32_error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    DWORD error = ERROR_INVALID_PARAMETER;
    if (!bx_ntvdm_host_namespace_v1_valid(space) || drive_index >= 26u ||
        relative_directory == 0) {
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    /* The admitted root is a valid directory but is not a child path.  Keep
     * this explicit rather than passing an empty name to the NT relative-open
     * routine, whose contract deliberately rejects empty child components. */
    if (relative_directory[0] == L'\0') {
        int available = (space->available_mask & bx_ntvdm_host_namespace_bit(drive_index)) != 0u;
        if (win32_error_out != 0) *win32_error_out = available ? ERROR_SUCCESS : ERROR_PATH_NOT_FOUND;
        return available;
    }
    int result = bx_ntvdm_host_namespace_v1_open_directory_ex(space, drive_index,
        relative_directory, FILE_READ_ATTRIBUTES, FILE_OPEN, &handle, &error);
    if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    if (win32_error_out != 0) *win32_error_out = result ? ERROR_SUCCESS : error;
    return result;
}

int bx_ntvdm_host_namespace_v1_open_file(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, ACCESS_MASK desired_access,
    ULONG share_access, DWORD creation_disposition, HANDLE *handle_out)
{
    return bx_ntvdm_host_namespace_v1_open_file_ex(space, drive_index,
        relative_path, desired_access, share_access, creation_disposition,
        handle_out, 0);
}

int bx_ntvdm_host_namespace_v1_open_file_ex(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, ACCESS_MASK desired_access,
    ULONG share_access, DWORD creation_disposition, HANDLE *handle_out,
    DWORD *win32_error_out)
{
    UNICODE_STRING object_name;
    OBJECT_ATTRIBUTES attributes;
    IO_STATUS_BLOCK status_block;
    BY_HANDLE_FILE_INFORMATION info;
    HANDLE handle = INVALID_HANDLE_VALUE;
    ULONG disposition;
    NTSTATUS status;
    size_t length;
    if (handle_out != 0) *handle_out = INVALID_HANDLE_VALUE;
    if (win32_error_out != 0) *win32_error_out = ERROR_INVALID_PARAMETER;
    if (!bx_ntvdm_host_namespace_v1_valid(space) || handle_out == 0 ||
        drive_index >= 26u ||
        (space->available_mask & bx_ntvdm_host_namespace_bit(drive_index)) == 0u ||
        !bx_ntvdm_host_namespace_file_relative(relative_path) ||
        (disposition = bx_ntvdm_host_namespace_disposition(creation_disposition)) == 0u ||
        (length = wcslen(relative_path)) > UINT16_MAX / sizeof(wchar_t)) return 0;
    object_name.Buffer = (PWSTR)relative_path;
    object_name.Length = (USHORT)(length * sizeof(wchar_t));
    object_name.MaximumLength = object_name.Length;
    InitializeObjectAttributes(&attributes, &object_name,
        OBJ_CASE_INSENSITIVE | OBJ_DONT_REPARSE, space->roots[drive_index], 0);
    status = NtCreateFile(&handle, desired_access | SYNCHRONIZE, &attributes,
        &status_block, 0, FILE_ATTRIBUTE_NORMAL, share_access, disposition,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT |
            FILE_OPEN_REPARSE_POINT,
        0, 0u);
    if (status < 0 || !bx_ntvdm_host_namespace_file_info(handle, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
            FILE_ATTRIBUTE_REPARSE_POINT)) != 0u) {
        DWORD error = status < 0 ? (DWORD)RtlNtStatusToDosError(status) : GetLastError();
        if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
        if (win32_error_out != 0)
            *win32_error_out = error == ERROR_SUCCESS ? ERROR_INVALID_DATA : error;
        return 0;
    }
    *handle_out = handle;
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_host_namespace_v1_query_file_attributes(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, DWORD *attributes_out,
    DWORD *win32_error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info;
    DWORD error = ERROR_SUCCESS;
    if (attributes_out != 0) *attributes_out = 0u;
    if (!bx_ntvdm_host_namespace_v1_open_file_ex(space, drive_index,
            relative_path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ |
            FILE_SHARE_WRITE | FILE_SHARE_DELETE, OPEN_EXISTING, &handle,
            &error)) {
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    if (!GetFileInformationByHandle(handle, &info)) {
        error = GetLastError();
        CloseHandle(handle);
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    CloseHandle(handle);
    *attributes_out = info.dwFileAttributes;
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_host_namespace_v1_set_file_attributes(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, DWORD attributes, DWORD *win32_error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    FILE_BASIC_INFO info;
    DWORD error = ERROR_SUCCESS;
    if (attributes == 0u || (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0u) {
        if (win32_error_out != 0) *win32_error_out = ERROR_INVALID_PARAMETER;
        return 0;
    }
    if (!bx_ntvdm_host_namespace_v1_open_file_ex(space, drive_index,
            relative_path, FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            OPEN_EXISTING, &handle, &error)) {
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    memset(&info, 0, sizeof(info));
    info.FileAttributes = attributes;
    if (!SetFileInformationByHandle(handle, FileBasicInfo, &info, sizeof(info))) {
        error = GetLastError();
        CloseHandle(handle);
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    CloseHandle(handle);
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_host_namespace_v1_delete_file(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, DWORD *win32_error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    FILE_DISPOSITION_INFO disposition;
    DWORD error = ERROR_SUCCESS;
    if (!bx_ntvdm_host_namespace_v1_open_file_ex(space, drive_index,
            relative_path, DELETE | FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            OPEN_EXISTING, &handle, &error)) {
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    disposition.DeleteFile = TRUE;
    if (!SetFileInformationByHandle(handle, FileDispositionInfo, &disposition,
            sizeof(disposition))) {
        error = GetLastError();
        CloseHandle(handle);
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    CloseHandle(handle);
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return 1;
}

static int bx_ntvdm_host_namespace_v1_open_directory_ex(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, ACCESS_MASK desired_access,
    ULONG disposition, HANDLE *handle_out, DWORD *win32_error_out)
{
    UNICODE_STRING object_name;
    OBJECT_ATTRIBUTES attributes;
    IO_STATUS_BLOCK status_block;
    BY_HANDLE_FILE_INFORMATION info;
    HANDLE handle = INVALID_HANDLE_VALUE;
    NTSTATUS status;
    size_t length;
    if (handle_out != 0) *handle_out = INVALID_HANDLE_VALUE;
    if (win32_error_out != 0) *win32_error_out = ERROR_INVALID_PARAMETER;
    if (!bx_ntvdm_host_namespace_v1_valid(space) || handle_out == 0 ||
        drive_index >= 26u ||
        (space->available_mask & bx_ntvdm_host_namespace_bit(drive_index)) == 0u ||
        !bx_ntvdm_host_namespace_file_relative(relative_path) ||
        (length = wcslen(relative_path)) > UINT16_MAX / sizeof(wchar_t)) return 0;
    object_name.Buffer = (PWSTR)relative_path;
    object_name.Length = (USHORT)(length * sizeof(wchar_t));
    object_name.MaximumLength = object_name.Length;
    InitializeObjectAttributes(&attributes, &object_name,
        OBJ_CASE_INSENSITIVE | OBJ_DONT_REPARSE, space->roots[drive_index], 0);
    status = NtCreateFile(&handle, desired_access | SYNCHRONIZE, &attributes,
        &status_block, 0, FILE_ATTRIBUTE_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, disposition,
        FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT |
        FILE_OPEN_REPARSE_POINT, 0, 0u);
    if (status < 0 || !bx_ntvdm_host_namespace_file_info(handle, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
            FILE_ATTRIBUTE_REPARSE_POINT)) != FILE_ATTRIBUTE_DIRECTORY) {
        DWORD error = status < 0 ? (DWORD)RtlNtStatusToDosError(status) : GetLastError();
        if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
        if (win32_error_out != 0)
            *win32_error_out = error == ERROR_SUCCESS ? ERROR_INVALID_DATA : error;
        return 0;
    }
    *handle_out = handle;
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_host_namespace_v1_create_directory(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, DWORD *win32_error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    if (!bx_ntvdm_host_namespace_v1_open_directory_ex(space, drive_index,
            relative_path, FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES,
            FILE_CREATE, &handle, win32_error_out)) return 0;
    CloseHandle(handle);
    return 1;
}

int bx_ntvdm_host_namespace_v1_remove_directory(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, DWORD *win32_error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    FILE_DISPOSITION_INFO disposition;
    DWORD error = ERROR_SUCCESS;
    if (!bx_ntvdm_host_namespace_v1_open_directory_ex(space, drive_index,
            relative_path, DELETE | FILE_READ_ATTRIBUTES, FILE_OPEN, &handle,
            &error)) {
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    disposition.DeleteFile = TRUE;
    if (!SetFileInformationByHandle(handle, FileDispositionInfo, &disposition,
            sizeof(disposition))) {
        error = GetLastError();
        CloseHandle(handle);
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    CloseHandle(handle);
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_host_namespace_v1_rename_file(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t source_drive,
    const wchar_t *source_relative, uint8_t destination_drive,
    const wchar_t *destination_relative, DWORD *win32_error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    FILE_RENAME_INFO *rename_info;
    size_t name_length, bytes;
    DWORD error = ERROR_SUCCESS;
    if (win32_error_out != 0) *win32_error_out = ERROR_INVALID_PARAMETER;
    if (!bx_ntvdm_host_namespace_v1_valid(space) || source_drive >= 26u ||
        destination_drive >= 26u ||
        source_drive != destination_drive ||
        !bx_ntvdm_host_namespace_file_relative(destination_relative) ||
        (name_length = wcslen(destination_relative)) > UINT32_MAX / sizeof(wchar_t)) {
        if (win32_error_out != 0 && source_drive != destination_drive)
            *win32_error_out = ERROR_NOT_SAME_DEVICE;
        return 0;
    }
    if (!bx_ntvdm_host_namespace_v1_open_file_ex(space, source_drive,
            source_relative, DELETE | FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            OPEN_EXISTING, &handle, &error)) {
        if (win32_error_out != 0) *win32_error_out = error;
        return 0;
    }
    bytes = sizeof(*rename_info) + (name_length - 1u) * sizeof(wchar_t);
    rename_info = (FILE_RENAME_INFO *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        bytes);
    if (rename_info == 0) {
        CloseHandle(handle);
        if (win32_error_out != 0) *win32_error_out = ERROR_NOT_ENOUGH_MEMORY;
        return 0;
    }
    rename_info->ReplaceIfExists = FALSE;
    rename_info->RootDirectory = space->roots[destination_drive];
    rename_info->FileNameLength = (DWORD)(name_length * sizeof(wchar_t));
    memcpy(rename_info->FileName, destination_relative,
        name_length * sizeof(wchar_t));
    {
        IO_STATUS_BLOCK status_block;
        NTSTATUS status = NtSetInformationFile(handle, &status_block,
            rename_info, (ULONG)bytes,
            BX_NTVDM_FILE_RENAME_INFORMATION_CLASS);
        if (status >= 0) {
            HeapFree(GetProcessHeap(), 0u, rename_info);
            CloseHandle(handle);
            if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
            return 1;
        }
        error = (DWORD)RtlNtStatusToDosError(status);
    }
    HeapFree(GetProcessHeap(), 0u, rename_info);
    CloseHandle(handle);
    if (win32_error_out != 0) *win32_error_out = error;
    return 0;
}
