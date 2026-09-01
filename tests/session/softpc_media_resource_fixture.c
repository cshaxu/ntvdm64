#include "session/session.h"
#include "mvdm_softpc_firmware.h"

#include <string.h>

int main(int argc, char **argv)
{
    session instance;
    char path[SESSION_FIRMWARE_ROOT_BYTES + 32u];
    char small_path[4];

    if (argc != 2) return 10;
    session_initialize(&instance, 1u);
    if (!session_set_mvdm_system_root(&instance, argv[1]) ||
        !session_activate(&instance) || !session_thread_bind(&instance)) return 1;
    if (!mvdm_softpc_system_copy_root(path, (uint32_t)sizeof(path)) ||
        strcmp(path, argv[1]) != 0) return 5;
    if (mvdm_softpc_system_copy_root(small_path, (uint32_t)sizeof(small_path)) ||
        small_path[0] != '\0') return 6;
    if (!mvdm_softpc_system_find_file("NTIO.SYS", path,
            (uint32_t)sizeof(path)) || strstr(path, "NTIO.SYS") == NULL)
        return 2;
    if (mvdm_softpc_system_find_file("missing.sys", path,
            (uint32_t)sizeof(path)) || path[0] != '\0') return 3;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 4;
    return 0;
}
