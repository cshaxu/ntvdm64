#include "session/session.h"
#include "mvdm_softpc_firmware.h"

#include <string.h>

int main(int argc, char **argv)
{
    session instance;
    char path[SESSION_FIRMWARE_ROOT_BYTES + 32u];

    if (argc != 2) return 10;
    session_initialize(&instance, 1u);
    if (!session_set_dos_media_root(&instance, argv[1]) ||
        !session_activate(&instance) || !session_thread_bind(&instance)) return 1;
    if (!mvdm_softpc_dos_find_file("NTIO.SYS", path,
            (uint32_t)sizeof(path)) || strstr(path, "NTIO.SYS") == NULL)
        return 2;
    if (mvdm_softpc_dos_find_file("missing.sys", path,
            (uint32_t)sizeof(path)) || path[0] != '\0') return 3;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 4;
    return 0;
}
