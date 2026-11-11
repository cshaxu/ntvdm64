/* SH-11: contained Win32 replacement for the published OpenNT *Oem boundary. */
#include <windows.h>
#include <string.h>
#include <wchar.h>
#include "oemuni.h"

#define RUNNER_OEM_PATH_CAPACITY 32768
#define RUNNER_OEM_ENV_SLOTS 32
#ifndef WC_NO_BEST_FIT_CHARS
#define WC_NO_BEST_FIT_CHARS 0x00000400
#endif
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

typedef struct {
    BOOL used;
    char name[128];
    char value[RUNNER_OEM_PATH_CAPACITY];
} RUNNER_OEM_ENV;

static WCHAR roots[26][RUNNER_OEM_PATH_CAPACITY];
static DWORD root_lengths[26];
static RUNNER_OEM_ENV environment[RUNNER_OEM_ENV_SLOTS];

static WCHAR *oem_to_wide(LPCSTR text) {
    int length;
    WCHAR *wide;
    if (text == NULL) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    length = MultiByteToWideChar(CP_OEMCP, MB_ERR_INVALID_CHARS, text, -1, NULL, 0);
    if (length == 0) return NULL;
    wide = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)length * sizeof(WCHAR));
    if (wide == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (MultiByteToWideChar(CP_OEMCP, MB_ERR_INVALID_CHARS, text, -1, wide, length) == 0) {
        HeapFree(GetProcessHeap(), 0, wide); return NULL;
    }
    return wide;
}

static BOOL wide_to_oem(LPCWSTR wide, LPSTR output, DWORD capacity) {
    int needed;
    BOOL used_default = FALSE;
    if (wide == NULL || output == NULL || capacity == 0) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    needed = WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS, wide, -1, output, (int)capacity, NULL, &used_default);
    if (needed == 0) return FALSE;
    if (used_default) { SetLastError(ERROR_NO_UNICODE_TRANSLATION); return FALSE; }
    return TRUE;
}

static BOOL has_root_prefix(LPCWSTR path, LPCWSTR root, DWORD root_length) {
    WCHAR next;
    if (root_length == 0 || _wcsnicmp(path, root, root_length) != 0) return FALSE;
    next = path[root_length];
    return next == L'\0' || next == L'\\';
}

static BOOL has_reparse_component(LPWSTR path, DWORD root_length) {
    WCHAR *cursor = path + root_length;
    WCHAR saved;
    DWORD attributes;
    while (*cursor) {
        if (*cursor == L'\\') {
            saved = *cursor; *cursor = L'\0';
            attributes = GetFileAttributesW(path);
            *cursor = saved;
            if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_REPARSE_POINT)) return TRUE;
        }
        ++cursor;
    }
    attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

/* Wrapper/session setup. The supplied DOS drive maps to one host directory. */
BOOL runner_oemuni_configure_drive_root(CHAR drive, LPCSTR path) {
    WCHAR *wide = oem_to_wide(path);
    DWORD length;
    DWORD index;
    WCHAR *root;
    if (drive >= 'a' && drive <= 'z') drive -= 'a' - 'A';
    if (drive < 'A' || drive > 'Z') { if (wide) HeapFree(GetProcessHeap(), 0, wide); SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    index = drive - 'A'; root = roots[index];
    if (wide == NULL) return FALSE;
    length = GetFullPathNameW(wide, RUNNER_OEM_PATH_CAPACITY, root, NULL);
    HeapFree(GetProcessHeap(), 0, wide);
    if (length == 0 || length >= RUNNER_OEM_PATH_CAPACITY ||
        (GetFileAttributesW(root) & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != FILE_ATTRIBUTE_DIRECTORY) {
        root_lengths[index] = 0; return FALSE;
    }
    while (length > 3 && root[length - 1] == L'\\') root[--length] = L'\0';
    root_lengths[index] = length;
    ZeroMemory(environment, sizeof(environment));
    return TRUE;
}

BOOL runner_oemuni_configure_root(LPCSTR path) { return runner_oemuni_configure_drive_root('C', path); }

VOID runner_oemuni_reset(VOID) {
    ZeroMemory(roots, sizeof(roots));
    ZeroMemory(root_lengths, sizeof(root_lengths));
    ZeroMemory(environment, sizeof(environment));
}

static WCHAR *contained_path(LPCSTR path) {
    WCHAR *input = oem_to_wide(path);
    WCHAR *candidate;
    WCHAR normalized[RUNNER_OEM_PATH_CAPACITY];
    WCHAR *suffix;
    WCHAR *root;
    DWORD root_length;
    DWORD drive = 2;
    DWORD length;
    if (input == NULL) return NULL;
    if (wcslen(input) >= 2 && input[1] == L':') {
        if (input[0] >= L'a' && input[0] <= L'z') input[0] -= L'a' - L'A';
        if (input[0] < L'A' || input[0] > L'Z') { HeapFree(GetProcessHeap(), 0, input); SetLastError(ERROR_ACCESS_DENIED); return NULL; }
        drive = input[0] - L'A'; suffix = input + 2;
        while (*suffix == L'\\' || *suffix == L'/') ++suffix;
    } else {
        suffix = input;
    }
    root = roots[drive]; root_length = root_lengths[drive];
    if (root_length == 0 || input[0] == L'\\' || input[0] == L'/') {
        HeapFree(GetProcessHeap(), 0, input); SetLastError(ERROR_ACCESS_DENIED); return NULL;
    }
    candidate = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, RUNNER_OEM_PATH_CAPACITY * sizeof(WCHAR));
    if (candidate == NULL) { HeapFree(GetProcessHeap(), 0, input); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (swprintf(candidate, RUNNER_OEM_PATH_CAPACITY, L"%ls\\%ls", root, suffix) < 0) {
        HeapFree(GetProcessHeap(), 0, input); HeapFree(GetProcessHeap(), 0, candidate); SetLastError(ERROR_FILENAME_EXCED_RANGE); return NULL;
    }
    HeapFree(GetProcessHeap(), 0, input);
    length = GetFullPathNameW(candidate, RUNNER_OEM_PATH_CAPACITY, normalized, NULL);
    if (length == 0 || length >= RUNNER_OEM_PATH_CAPACITY || !has_root_prefix(normalized, root, root_length) || has_reparse_component(normalized, root_length)) {
        HeapFree(GetProcessHeap(), 0, candidate); SetLastError(ERROR_ACCESS_DENIED); return NULL;
    }
    memcpy(candidate, normalized, ((SIZE_T)length + 1) * sizeof(WCHAR));
    return candidate;
}

static VOID copy_find_data(LPWIN32_FIND_DATAA output, const WIN32_FIND_DATAW *input) {
    memcpy(output, input, offsetof(WIN32_FIND_DATAA, cFileName));
    if (!wide_to_oem(input->cFileName, output->cFileName, MAX_PATH) ||
        !wide_to_oem(input->cAlternateFileName, output->cAlternateFileName, 14)) {
        output->cFileName[0] = '\0'; output->cAlternateFileName[0] = '\0';
    }
}

HANDLE WINAPI CreateFileOem(LPCSTR path, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES security, DWORD disposition, DWORD flags, HANDLE template_file) {
    WCHAR *wide = contained_path(path); HANDLE result;
    if (wide == NULL) return INVALID_HANDLE_VALUE;
    result = CreateFileW(wide, access == GENERIC_READ ? access | FILE_WRITE_ATTRIBUTES : access,
        share, security, disposition, flags, template_file);
    if (result == INVALID_HANDLE_VALUE && access == GENERIC_READ)
        result = CreateFileW(wide, access, share, security, disposition, flags, template_file);
    HeapFree(GetProcessHeap(), 0, wide); return result;
}

BOOL APIENTRY SetFileAttributesOem(LPSTR path, DWORD attributes) { WCHAR *wide = contained_path(path); BOOL result = wide != NULL && SetFileAttributesW(wide, attributes); if (wide) HeapFree(GetProcessHeap(), 0, wide); return result; }
DWORD APIENTRY GetFileAttributesOem(LPSTR path) { WCHAR *wide = contained_path(path); DWORD result = wide ? GetFileAttributesW(wide) : INVALID_FILE_ATTRIBUTES; if (wide) HeapFree(GetProcessHeap(), 0, wide); return result; }
BOOL APIENTRY DeleteFileOem(LPSTR path) { WCHAR *wide = contained_path(path); BOOL result = wide != NULL && DeleteFileW(wide); if (wide) HeapFree(GetProcessHeap(), 0, wide); return result; }
BOOL APIENTRY CreateDirectoryOem(LPSTR path, LPSECURITY_ATTRIBUTES security) { WCHAR *wide = contained_path(path); BOOL result = wide != NULL && CreateDirectoryW(wide, security); if (wide) HeapFree(GetProcessHeap(), 0, wide); return result; }
BOOL APIENTRY RemoveDirectoryOem(LPSTR path) { WCHAR *wide = contained_path(path); BOOL result = wide != NULL && RemoveDirectoryW(wide); if (wide) HeapFree(GetProcessHeap(), 0, wide); return result; }

BOOL APIENTRY MoveFileOem(LPSTR old_path, LPSTR new_path) {
    WCHAR *old_wide = contained_path(old_path), *new_wide = contained_path(new_path); BOOL result = FALSE;
    if (old_wide && new_wide) result = MoveFileW(old_wide, new_wide);
    if (old_wide) HeapFree(GetProcessHeap(), 0, old_wide); if (new_wide) HeapFree(GetProcessHeap(), 0, new_wide); return result;
}

BOOL APIENTRY MoveFileExOem(LPSTR old_path, LPSTR new_path, DWORD flags) {
    WCHAR *old_wide = contained_path(old_path), *new_wide = contained_path(new_path); BOOL result = FALSE;
    if (old_wide && new_wide) result = MoveFileExW(old_wide, new_wide, flags);
    if (old_wide) HeapFree(GetProcessHeap(), 0, old_wide); if (new_wide) HeapFree(GetProcessHeap(), 0, new_wide); return result;
}

HANDLE APIENTRY FindFirstFileOem(LPSTR path, LPWIN32_FIND_DATAA output) {
    WCHAR *wide = contained_path(path); WIN32_FIND_DATAW data; HANDLE handle;
    if (wide == NULL || output == NULL) { if (wide) HeapFree(GetProcessHeap(), 0, wide); SetLastError(ERROR_INVALID_PARAMETER); return INVALID_HANDLE_VALUE; }
    handle = FindFirstFileW(wide, &data); HeapFree(GetProcessHeap(), 0, wide);
    if (handle != INVALID_HANDLE_VALUE) copy_find_data(output, &data); return handle;
}

BOOL APIENTRY FindNextFileOem(HANDLE handle, LPWIN32_FIND_DATAA output) {
    WIN32_FIND_DATAW data;
    if (output == NULL) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!FindNextFileW(handle, &data)) return FALSE; copy_find_data(output, &data); return TRUE;
}

DWORD APIENTRY GetFullPathNameOem(LPCSTR path, DWORD capacity, LPSTR output, LPSTR *part) {
    WCHAR *wide = contained_path(path); DWORD result;
    if (wide == NULL) return 0;
    result = (DWORD)strlen(path);
    if (!wide_to_oem(wide, output, capacity)) { HeapFree(GetProcessHeap(), 0, wide); return 0; }
    if (part) { LPSTR slash = strrchr(output, '\\'); *part = slash ? slash + 1 : output; }
    HeapFree(GetProcessHeap(), 0, wide); return result;
}

DWORD APIENTRY GetCurrentDirectoryOem(DWORD capacity, LPSTR output) { UNREFERENCED_PARAMETER(capacity); UNREFERENCED_PARAMETER(output); SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return 0; }
BOOL APIENTRY SetCurrentDirectoryOem(LPSTR path) {
    WCHAR *wide = contained_path(path);
    DWORD attributes = wide ? GetFileAttributesW(wide) : INVALID_FILE_ATTRIBUTES;
    if (wide) HeapFree(GetProcessHeap(), 0, wide);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        if (attributes != INVALID_FILE_ATTRIBUTES) SetLastError(ERROR_DIRECTORY);
        return FALSE;
    }
    return TRUE;
}
UINT APIENTRY GetDriveTypeOem(LPSTR path) { WCHAR *wide = contained_path(path); UINT result = wide ? GetDriveTypeW(wide) : DRIVE_NO_ROOT_DIR; if (wide) HeapFree(GetProcessHeap(), 0, wide); return result; }

BOOL APIENTRY GetDiskFreeSpaceOem(LPSTR path, LPDWORD sectors, LPDWORD bytes, LPDWORD free_clusters, LPDWORD total_clusters) {
    WCHAR *wide = contained_path(path); BOOL result = wide != NULL && GetDiskFreeSpaceW(wide, sectors, bytes, free_clusters, total_clusters); if (wide) HeapFree(GetProcessHeap(), 0, wide); return result;
}

BOOL APIENTRY GetVolumeInformationOem(LPSTR path, LPSTR volume, DWORD volume_size, LPDWORD serial, LPDWORD component, LPDWORD flags, LPSTR fs, DWORD fs_size) {
    WCHAR *wide = contained_path(path); WCHAR volume_w[MAX_PATH], fs_w[MAX_PATH]; BOOL result;
    if (wide == NULL) return FALSE;
    result = GetVolumeInformationW(wide, volume ? volume_w : NULL, volume ? MAX_PATH : 0, serial, component, flags, fs ? fs_w : NULL, fs ? MAX_PATH : 0);
    HeapFree(GetProcessHeap(), 0, wide);
    if (!result) return FALSE;
    if ((volume && !wide_to_oem(volume_w, volume, volume_size)) || (fs && !wide_to_oem(fs_w, fs, fs_size))) return FALSE;
    return TRUE;
}

VOID APIENTRY OutputDebugStringOem(LPCSTR text) { WCHAR *wide = oem_to_wide(text); if (wide) { OutputDebugStringW(wide); HeapFree(GetProcessHeap(), 0, wide); } }
BOOL WINAPI GetComputerNameOem(LPSTR name, LPDWORD size) { UNREFERENCED_PARAMETER(name); UNREFERENCED_PARAMETER(size); SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
BOOL WINAPI RemoveFontResourceOem(LPSTR path) { UNREFERENCED_PARAMETER(path); SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }

static RUNNER_OEM_ENV *environment_slot(LPSTR name, BOOL create) {
    DWORD i; RUNNER_OEM_ENV *free_slot = NULL;
    for (i = 0; i < RUNNER_OEM_ENV_SLOTS; ++i) {
        if (environment[i].used && strcmp(environment[i].name, name) == 0) return &environment[i];
        if (!environment[i].used && free_slot == NULL) free_slot = &environment[i];
    }
    if (create && free_slot) { free_slot->used = TRUE; strncpy(free_slot->name, name, sizeof(free_slot->name) - 1); return free_slot; }
    return NULL;
}

BOOL WINAPI SetEnvironmentVariableOem(LPSTR name, LPSTR value) {
    RUNNER_OEM_ENV *slot;
    if (name == NULL || !*name) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    slot = environment_slot(name, value != NULL);
    if (value == NULL) { if (slot) ZeroMemory(slot, sizeof(*slot)); return TRUE; }
    if (slot == NULL || strlen(value) >= sizeof(slot->value)) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    strcpy(slot->value, value); return TRUE;
}

DWORD WINAPI GetEnvironmentVariableOem(LPSTR name, LPSTR output, DWORD capacity) {
    RUNNER_OEM_ENV *slot = environment_slot(name, FALSE); DWORD length;
    if (slot == NULL) { SetLastError(ERROR_ENVVAR_NOT_FOUND); return 0; }
    length = (DWORD)strlen(slot->value);
    if (capacity == 0 || output == NULL) return length + 1;
    if (capacity <= length) { if (capacity) output[0] = '\0'; return length + 1; }
    strcpy(output, slot->value); return length;
}
