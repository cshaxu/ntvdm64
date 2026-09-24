#include "mvdm_softpc_firmware.h"

#include <string.h>
#include <windows.h>

#include "ntvdm-exe/session/session.h"

static int mvdm_softpc_media_find_file(const char *root, const char *name,
    char *path_out, uint32_t path_out_bytes)
{
    size_t root_length;
    size_t name_length;
    size_t separator_length;

    if (path_out != NULL && path_out_bytes != 0u) path_out[0] = '\0';
    if (root == NULL || name == NULL || name[0] == '\0' ||
        path_out == NULL || path_out_bytes == 0u) return 0;
    root_length = strlen(root);
    name_length = strlen(name);
    separator_length = root_length != 0u && root[root_length - 1u] != '\\' &&
        root[root_length - 1u] != '/' ? 1u : 0u;
    if (root_length + separator_length + name_length + 1u > path_out_bytes)
        return 0;

    memcpy(path_out, root, root_length);
    if (separator_length != 0u) path_out[root_length++] = '\\';
    memcpy(path_out + root_length, name, name_length + 1u);
    if (GetFileAttributesA(path_out) == INVALID_FILE_ATTRIBUTES) {
        path_out[0] = '\0';
        return 0;
    }
    return 1;
}

typedef struct _MVDM_SOFTPC_PROFILE_SHADOW {
    session *owner;
    HANDLE handle;
    char path[MAX_PATH];
} MVDM_SOFTPC_PROFILE_SHADOW;

static MVDM_SOFTPC_PROFILE_SHADOW profile_shadow;

static void mvdm_softpc_profile_shadow_dispose(void *context)
{
    MVDM_SOFTPC_PROFILE_SHADOW *shadow =
        (MVDM_SOFTPC_PROFILE_SHADOW *)context;

    if (shadow == NULL) return;
    if (shadow->handle != NULL && shadow->handle != INVALID_HANDLE_VALUE)
        CloseHandle(shadow->handle);
    if (shadow->path[0] != '\0') (void)DeleteFileA(shadow->path);
    ZeroMemory(shadow, sizeof(*shadow));
}

int mvdm_softpc_firmware_find_file(const char *name, char *path_out,
    uint32_t path_out_bytes)
{
    session *instance = session_thread_current();
    return mvdm_softpc_media_find_file(instance != NULL ?
        session_firmware_root(instance) : NULL, name, path_out,
        path_out_bytes);
}

long mvdm_softpc_firmware_read_embedded_rom(const char *name, void *bytes_out,
    uint32_t bytes_out_capacity)
{
    const char *resource_name;
    HMODULE module;
    HRSRC resource;
    HGLOBAL loaded;
    DWORD size;
    const void *bytes;

    if (name == NULL || bytes_out == NULL) return 0;
    if (strcmp(name, "bios1.rom") == 0) resource_name = "SOFTPC_BIOS1";
    else if (strcmp(name, "bios4.rom") == 0) resource_name = "SOFTPC_BIOS4";
    else if (strcmp(name, "v7vga.rom") == 0) resource_name = "SOFTPC_V7VGA";
    else return 0;
    module = GetModuleHandleA(NULL);
    resource = module == NULL ? NULL : FindResourceA(module, resource_name,
        RT_RCDATA);
    if (resource == NULL) return 0;
    size = SizeofResource(module, resource);
    if (size == 0u || size > bytes_out_capacity) return 0;
    loaded = LoadResource(module, resource);
    bytes = loaded == NULL ? NULL : LockResource(loaded);
    if (bytes == NULL) return 0;
    memcpy(bytes_out, bytes, size);
    return (long)size;
}

int mvdm_softpc_system_find_file(const char *name, char *path_out,
    uint32_t path_out_bytes)
{
    session *instance = session_thread_current();
    return mvdm_softpc_media_find_file(instance != NULL ?
        session_mvdm_system_root(instance) : NULL, name, path_out,
        path_out_bytes);
}

int mvdm_softpc_profile_shadow_system_ini(char *path_out,
    uint32_t path_out_bytes)
{
    session *owner = session_thread_current();
    char source[MAX_PATH];
    char directory[MAX_PATH];
    DWORD directory_chars;
    size_t path_bytes;

    if (path_out != NULL && path_out_bytes != 0u) path_out[0] = '\0';
    if (owner == NULL || path_out == NULL || path_out_bytes == 0u) return 0;
    if (profile_shadow.owner == owner && profile_shadow.path[0] != '\0') {
        path_bytes = strlen(profile_shadow.path) + 1u;
        if (path_bytes > path_out_bytes) return 0;
        memcpy(path_out, profile_shadow.path, path_bytes);
        return 1;
    }
    if (profile_shadow.owner != NULL ||
        !mvdm_softpc_system_find_file("system.ini", source, sizeof(source)))
        return 0;
    directory_chars = GetTempPathA(sizeof(directory), directory);
    if (directory_chars == 0u || directory_chars >= sizeof(directory) ||
        !GetTempFileNameA(directory, "nvm", 0u, profile_shadow.path) ||
        !CopyFileA(source, profile_shadow.path, FALSE) ||
        (profile_shadow.handle = CreateFileA(profile_shadow.path, GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
            NULL)) == INVALID_HANDLE_VALUE) {
        if (profile_shadow.path[0] != '\0') DeleteFileA(profile_shadow.path);
        ZeroMemory(&profile_shadow, sizeof(profile_shadow));
        return 0;
    }
    profile_shadow.owner = owner;
    if (!session_register_teardown(owner, mvdm_softpc_profile_shadow_dispose,
            &profile_shadow)) {
        mvdm_softpc_profile_shadow_dispose(&profile_shadow);
        return 0;
    }
    path_bytes = strlen(profile_shadow.path) + 1u;
    if (path_bytes > path_out_bytes) return 0;
    memcpy(path_out, profile_shadow.path, path_bytes);
    return 1;
}

int mvdm_softpc_system_copy_root(char *path_out, uint32_t path_out_bytes)
{
    session *instance = session_thread_current();
    const char *root = instance != NULL ? session_mvdm_system_root(instance) : NULL;
    size_t root_bytes;

    if (path_out != NULL && path_out_bytes != 0u) path_out[0] = '\0';
    if (root == NULL || path_out == NULL || path_out_bytes == 0u) return 0;
    root_bytes = strlen(root) + 1u;
    if (root_bytes > path_out_bytes) return 0;
    memcpy(path_out, root, root_bytes);
    return 1;
}

int mvdm_softpc_system_copy_system_directory(char *path_out,
    uint32_t path_out_bytes)
{
    session *instance = session_thread_current();
    const char *root = instance != NULL ? session_mvdm_system_root(instance) : NULL;
    static const char child[] = "system32";
    size_t root_bytes;
    size_t separator_bytes;
    DWORD attributes;

    if (path_out != NULL && path_out_bytes != 0u) path_out[0] = '\0';
    if (root == NULL || root[0] == '\0' || path_out == NULL ||
        path_out_bytes == 0u) return 0;
    root_bytes = strlen(root);
    separator_bytes = root[root_bytes - 1u] != '\\' &&
        root[root_bytes - 1u] != '/' ? 1u : 0u;
    if (root_bytes + separator_bytes + sizeof(child) > path_out_bytes)
        return 0;
    memcpy(path_out, root, root_bytes);
    if (separator_bytes != 0u) path_out[root_bytes++] = '\\';
    memcpy(path_out + root_bytes, child, sizeof(child));
    attributes = GetFileAttributesA(path_out);
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0u) {
        path_out[0] = '\0';
        return 0;
    }
    return 1;
}

static uint32_t mvdm_softpc_copy_ntvdm_directory(int system_directory,
    char *path_out, uint32_t path_out_chars)
{
    char path[MAX_PATH];
    size_t path_chars;

    if (path_out != NULL && path_out_chars != 0u) path_out[0] = '\0';
    if (!(system_directory ? mvdm_softpc_system_copy_system_directory(path,
            (uint32_t)sizeof(path)) : mvdm_softpc_system_copy_root(path,
            (uint32_t)sizeof(path)))) return 0;
    path_chars = strlen(path);
    if (path_out == NULL || path_out_chars == 0u ||
        path_chars >= path_out_chars) return (uint32_t)(path_chars + 1u);
    memcpy(path_out, path, path_chars + 1u);
    return (uint32_t)path_chars;
}

uint32_t GetNtvdmWindowsDirectoryA(char *path_out, uint32_t path_out_chars)
{
    return mvdm_softpc_copy_ntvdm_directory(0, path_out, path_out_chars);
}

uint32_t GetNtvdmSystemDirectoryA(char *path_out, uint32_t path_out_chars)
{
    return mvdm_softpc_copy_ntvdm_directory(1, path_out, path_out_chars);
}

static uint32_t mvdm_softpc_copy_ntvdm_directory_wide(int system_directory,
    wchar_t *path_out, uint32_t path_out_chars)
{
    char path[MAX_PATH];
    int path_chars;

    if (path_out != NULL && path_out_chars != 0u) path_out[0] = L'\0';
    if (!mvdm_softpc_copy_ntvdm_directory(system_directory, path,
            (uint32_t)sizeof(path))) return 0;
    path_chars = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
    if (path_chars <= 0) return 0;
    if (path_out == NULL || path_out_chars == 0u ||
        (uint32_t)path_chars > path_out_chars) return (uint32_t)path_chars;
    if (!MultiByteToWideChar(CP_ACP, 0, path, -1, path_out,
            (int)path_out_chars)) return 0;
    return (uint32_t)(path_chars - 1);
}

uint32_t GetNtvdmWindowsDirectoryW(wchar_t *path_out,
    uint32_t path_out_chars)
{
    return mvdm_softpc_copy_ntvdm_directory_wide(0, path_out,
        path_out_chars);
}

uint32_t GetNtvdmSystemDirectoryW(wchar_t *path_out,
    uint32_t path_out_chars)
{
    return mvdm_softpc_copy_ntvdm_directory_wide(1, path_out,
        path_out_chars);
}

int mvdm_softpc_config_path_to_ansi(char *path, uint32_t capacity)
{
    WCHAR unicode[MAX_PATH + 12];
    int chars;

    /* GetPIFConfigFiles feeds ANSI CreateFile callers in cmdconf/nt_msscs.
     * Preserve the original OEM expansion; cross encoding only here. */
    if (path == NULL || capacity == 0 || capacity > MAX_PATH + 12 ||
        memchr(path, '\0', capacity) == NULL) return 0;
    chars = MultiByteToWideChar(CP_OEMCP, 0, path, -1, unicode,
        sizeof(unicode) / sizeof(unicode[0]));
    if (!chars) return 0;
    return WideCharToMultiByte(CP_ACP, 0, unicode, chars, path,
        (int)capacity, NULL, NULL) != 0;
}

void mvdm_softpc_prepare_system_file_compatibility(void)
{
    /* The historical routine has no observable result other than the two
     * installation-only host writes.  Do not fabricate their success by
     * mutating the admitted real host drive. */
}
