#include "app/package_layout.h"

#include <string.h>

int main(int argc, char **argv)
{
    session instance;
    const char *root;

    session_initialize(&instance, 1u);
    if (argc == 2) {
        if (!app_package_layout_set_media_roots(&instance, argv[1]) ||
            !app_package_layout_validate_command_configuration_root(&instance) ||
            !session_dispose(&instance)) return 10;
        return 0;
    }
    if (argc != 1) return 11;
    if (!app_package_layout_set_media_roots(&instance,
            "C:\\installed\\ntvdm64.exe")) return 1;
    root = session_firmware_root(&instance);
    if (root == NULL || strcmp(root, "C:\\installed\\mvdm\\softpc") != 0)
        return 2;
    root = session_mvdm_system_root(&instance);
    if (root == NULL || strcmp(root, "C:\\installed\\mvdm") != 0) return 3;
    /* The invented root has no filesystem entry and must fail the distinct
     * original-COMMAND admission check without changing root derivation. */
    if (app_package_layout_validate_command_configuration_root(&instance))
        return 4;
    if (app_package_layout_set_media_roots(&instance, "ntvdm64.exe") ||
        !session_valid(&instance)) return 5;
    if (!session_dispose(&instance)) return 6;
    return 0;
}
