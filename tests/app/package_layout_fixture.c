#include "app/package_layout.h"

#include <string.h>

int main(void)
{
    session instance;
    const char *root;

    session_initialize(&instance, 1u);
    if (!app_package_layout_set_firmware_root(&instance,
            "C:\\installed\\ntvdm64.exe")) return 1;
    root = session_firmware_root(&instance);
    if (root == NULL || strcmp(root, "C:\\installed\\softpc\\roms") != 0)
        return 2;
    if (app_package_layout_set_firmware_root(&instance, "ntvdm64.exe") ||
        !session_valid(&instance)) return 3;
    if (!session_dispose(&instance)) return 4;
    return 0;
}
