#include "contained_dem_file_tokens.h"

#include <string.h>
#include <wchar.h>

#define NTDOS64_DEM_PATH_CAPACITY 32768

static volatile LONG ntdos64_next_session_id;

static uint16_t next_session_id(void) {
    LONG value = InterlockedIncrement(&ntdos64_next_session_id) & 0xffff;
    return (uint16_t)(value == 0 ? 1 : value);
}

static BOOL root_prefix(LPCWSTR path, LPCWSTR root, DWORD root_length) {
    WCHAR next;
    if (root_length == 0 || _wcsnicmp(path, root, root_length) != 0) return FALSE;
    next = path[root_length];
    return next == L'\0' || next == L'\\';
}

static BOOL reparse_component(LPWSTR path, DWORD root_length) {
    WCHAR *cursor = path + root_length;
    WCHAR saved;
    DWORD attributes;

    while (*cursor) {
        if (*cursor == L'\\') {
            saved = *cursor;
            *cursor = L'\0';
            attributes = GetFileAttributesW(path);
            *cursor = saved;
            if (attributes != INVALID_FILE_ATTRIBUTES &&
                (attributes & FILE_ATTRIBUTE_REPARSE_POINT)) return TRUE;
        }
        ++cursor;
    }
    attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

static BOOL contained_path(const NTDOS64_DEM_FILE_SESSION *session,
    LPCWSTR relative_path, WCHAR output[NTDOS64_DEM_PATH_CAPACITY]) {
    WCHAR candidate[NTDOS64_DEM_PATH_CAPACITY];
    DWORD length;

    if (session == NULL || relative_path == NULL || !*relative_path ||
        relative_path[0] == L'\\' || relative_path[0] == L'/' ||
        wcschr(relative_path, L':') != NULL) {
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    if (swprintf(candidate, NTDOS64_DEM_PATH_CAPACITY, L"%ls\\%ls",
        session->root, relative_path) < 0) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    length = GetFullPathNameW(candidate, NTDOS64_DEM_PATH_CAPACITY, output, NULL);
    if (length == 0 || length >= NTDOS64_DEM_PATH_CAPACITY ||
        !root_prefix(output, session->root, session->root_length) ||
        reparse_component(output, session->root_length)) {
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    return TRUE;
}

static uint32_t make_token(const NTDOS64_DEM_FILE_SESSION *session,
    uint8_t slot, uint8_t generation) {
    return ((uint32_t)session->session_id << 16) |
        ((uint32_t)generation << 8) | (uint32_t)(slot + 1);
}

static NTDOS64_DEM_FILE_SLOT *find_slot(NTDOS64_DEM_FILE_SESSION *session,
    uint32_t token) {
    uint8_t slot = (uint8_t)(token & 0xff);
    uint8_t generation = (uint8_t)((token >> 8) & 0xff);
    uint16_t session_id = (uint16_t)(token >> 16);

    if (session == NULL || token == 0 || session_id != session->session_id ||
        slot == 0 || slot > NTDOS64_DEM_FILE_MAX_SLOTS) {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
    }
    --slot;
    if (!session->slots[slot].open || session->slots[slot].generation != generation) {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
    }
    return &session->slots[slot];
}

BOOL ntdos64_dem_file_session_init(NTDOS64_DEM_FILE_SESSION *session, LPCWSTR root) {
    DWORD attributes;
    DWORD length;

    if (session == NULL || root == NULL || !*root) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    ZeroMemory(session, sizeof(*session));
    length = GetFullPathNameW(root, NTDOS64_DEM_PATH_CAPACITY, session->root, NULL);
    attributes = length == 0 || length >= NTDOS64_DEM_PATH_CAPACITY ?
        INVALID_FILE_ATTRIBUTES : GetFileAttributesW(session->root);
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) !=
        FILE_ATTRIBUTE_DIRECTORY) {
        ZeroMemory(session, sizeof(*session));
        SetLastError(ERROR_DIRECTORY);
        return FALSE;
    }
    while (length > 3 && session->root[length - 1] == L'\\')
        session->root[--length] = L'\0';
    session->root_length = length;
    session->session_id = next_session_id();
    return TRUE;
}

VOID ntdos64_dem_file_session_teardown(NTDOS64_DEM_FILE_SESSION *session) {
    DWORD index;
    if (session == NULL) return;
    for (index = 0; index < NTDOS64_DEM_FILE_MAX_SLOTS; ++index) {
        if (session->slots[index].open) CloseHandle(session->slots[index].handle);
    }
    ZeroMemory(session, sizeof(*session));
}

BOOL ntdos64_dem_file_open_read(NTDOS64_DEM_FILE_SESSION *session, LPCWSTR relative_path,
    uint32_t *token, uint32_t *size) {
    WCHAR path[NTDOS64_DEM_PATH_CAPACITY];
    LARGE_INTEGER length;
    HANDLE handle;
    DWORD index;
    NTDOS64_DEM_FILE_SLOT *slot = NULL;

    if (session == NULL || token == NULL || size == NULL || !contained_path(session, relative_path, path))
        return FALSE;
    for (index = 0; index < NTDOS64_DEM_FILE_MAX_SLOTS; ++index) {
        if (!session->slots[index].open) { slot = &session->slots[index]; break; }
    }
    if (slot == NULL) { SetLastError(ERROR_TOO_MANY_OPEN_FILES); return FALSE; }
    handle = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) return FALSE;
    if (!GetFileSizeEx(handle, &length) || length.QuadPart < 0 ||
        (uint64_t)length.QuadPart > UINT32_MAX) {
        if (GetLastError() == ERROR_SUCCESS) SetLastError(ERROR_FILE_TOO_LARGE);
        CloseHandle(handle);
        return FALSE;
    }
    ++slot->generation;
    if (slot->generation == 0) ++slot->generation;
    slot->handle = handle;
    slot->open = TRUE;
    *token = make_token(session, (uint8_t)index, slot->generation);
    *size = (uint32_t)length.QuadPart;
    return TRUE;
}

BOOL ntdos64_dem_file_read(NTDOS64_DEM_FILE_SESSION *session, uint32_t token,
    uint64_t offset, BOOL use_current_offset, void *buffer, uint32_t requested,
    uint32_t *actual) {
    NTDOS64_DEM_FILE_SLOT *slot = find_slot(session, token);
    LARGE_INTEGER position;

    if (slot == NULL || actual == NULL || (requested != 0 && buffer == NULL)) {
        if (slot != NULL) SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (!use_current_offset) {
        position.QuadPart = (LONGLONG)offset;
        if (!SetFilePointerEx(slot->handle, position, NULL, FILE_BEGIN)) return FALSE;
    }
    return ReadFile(slot->handle, buffer, requested, (LPDWORD)actual, NULL);
}

BOOL ntdos64_dem_file_seek(NTDOS64_DEM_FILE_SESSION *session, uint32_t token,
    int64_t offset, DWORD origin, uint32_t *actual) {
    NTDOS64_DEM_FILE_SLOT *slot = find_slot(session, token);
    LARGE_INTEGER distance;
    LARGE_INTEGER position;

    if (slot == NULL || actual == NULL || origin > FILE_END) {
        if (slot != NULL) SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    distance.QuadPart = offset;
    if (!SetFilePointerEx(slot->handle, distance, &position, origin) ||
        position.QuadPart < 0 || (uint64_t)position.QuadPart > UINT32_MAX) {
        if (GetLastError() == ERROR_SUCCESS) SetLastError(ERROR_NEGATIVE_SEEK);
        return FALSE;
    }
    *actual = (uint32_t)position.QuadPart;
    return TRUE;
}

BOOL ntdos64_dem_file_close(NTDOS64_DEM_FILE_SESSION *session, uint32_t token,
    BOOL set_final_offset, uint32_t final_offset) {
    NTDOS64_DEM_FILE_SLOT *slot = find_slot(session, token);
    LARGE_INTEGER position;
    HANDLE handle;

    if (slot == NULL) return FALSE;
    if (set_final_offset) {
        position.QuadPart = final_offset;
        if (!SetFilePointerEx(slot->handle, position, NULL, FILE_BEGIN)) return FALSE;
    }
    handle = slot->handle;
    slot->open = FALSE;
    slot->handle = NULL;
    return CloseHandle(handle);
}
