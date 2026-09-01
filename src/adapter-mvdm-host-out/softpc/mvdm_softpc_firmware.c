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
