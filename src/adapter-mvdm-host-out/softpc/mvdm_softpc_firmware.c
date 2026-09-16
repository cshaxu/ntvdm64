#include "mvdm_softpc_firmware.h"

#include <string.h>
#include <windows.h>

#include "session/session.h"

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

void mvdm_softpc_prepare_system_file_compatibility(void)
{
    /* The historical routine has no observable result other than the two
     * installation-only host writes.  Do not fabricate their success by
     * mutating the admitted real host drive. */
}
