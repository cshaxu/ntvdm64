#include "app/package_layout.h"

#include <string.h>

int main(void)
{
    session instance;
    const char *root;

    session_initialize(&instance, 1u);
    if (!app_package_layout_set_media_roots(&instance,
            "C:\\installed\\ntvdm64.exe")) return 1;
    root = session_firmware_root(&instance);
    if (root == NULL || strcmp(root, "C:\\installed\\softpc\\roms") != 0)
        return 2;
    root = session_dos_media_root(&instance);
    if (root == NULL || strcmp(root, "C:\\installed\\dos") != 0) return 3;
    root = session_win16_media_root(&instance);
    if (root == NULL || strcmp(root, "C:\\installed\\win16") != 0) return 4;
    if (app_package_layout_set_media_roots(&instance, "ntvdm64.exe") ||
        !session_valid(&instance)) return 3;
    if (!session_dispose(&instance)) return 5;
    return 0;
}
