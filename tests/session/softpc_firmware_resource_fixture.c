#include "session/session.h"
#include "mvdm_softpc_firmware.h"

#include <string.h>

int main(int argc, char **argv)
{
    session instance;
    char path[SESSION_FIRMWARE_ROOT_BYTES + 32u];

    if (argc != 2) return 10;
    session_initialize(&instance, 1u);
    if (!session_valid(&instance) ||
        !session_set_firmware_root(&instance, argv[1]) ||
        !session_activate(&instance) || !session_thread_bind(&instance)) return 1;

    if (!mvdm_softpc_firmware_find_file("bios1.rom", path,
            (uint32_t)sizeof(path)) ||
        strstr(path, "bios1.rom") == NULL) return 2;
    if (mvdm_softpc_firmware_find_file("does-not-exist.rom", path,
            (uint32_t)sizeof(path)) || path[0] != '\0') return 3;
    if (mvdm_softpc_firmware_find_file("bios1.rom", path, 4u) ||
        path[0] != '\0') return 4;

    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 5;
    return 0;
}
