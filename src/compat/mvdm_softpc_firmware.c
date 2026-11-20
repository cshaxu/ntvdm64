#include "include/mvdm_softpc_firmware.h"

#include <string.h>
#include <windows.h>

#include "include/host_context.h"

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
    compat_host_context *owner;
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

int mvdm_softpc_system_find_file(const char *name, char *path_out,
    uint32_t path_out_bytes)
{
    compat_host_context *instance = compat_host_context_current();
    char directory[COMPAT_MVDM_SYSTEM_ROOT_BYTES + 16u];
    if (path_out != NULL && path_out_bytes != 0u) path_out[0] = '\0';
    if (instance == NULL || name == NULL) return 0;
    /* Original setup places SYSTEM.INI at Windows root, but NTIO and the
     * default CONFIG.NT/AUTOEXEC.NT in GetSystemDirectory's system32.
     * This binding never searches CWD/PATH or falls back to a flat copy. */
    if (!_stricmp(name, "system.ini"))
        return mvdm_softpc_media_find_file(instance->mvdm_system_root,
            name, path_out, path_out_bytes);
    if (!NtvdmGetSystemDirectoryA(directory, sizeof(directory))) return 0;
    return mvdm_softpc_media_find_file(directory, name, path_out,
        path_out_bytes);
}

int mvdm_softpc_profile_shadow_system_ini(char *path_out,
    uint32_t path_out_bytes)
{
    compat_host_context *owner = compat_host_context_current();
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
    if (!compat_host_context_register(owner, mvdm_softpc_profile_shadow_dispose,
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
    compat_host_context *instance = compat_host_context_current();
    const char *root = instance != NULL ? instance->mvdm_system_root : NULL;
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
    compat_host_context *instance = compat_host_context_current();
    const char *root = instance != NULL ? instance->mvdm_system_root : NULL;
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
