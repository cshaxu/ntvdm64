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
    if (root == NULL || strcmp(root, "C:\\installed\\mvdm\\softpc") != 0)
        return 2;
    root = session_mvdm_system_root(&instance);
    if (root == NULL || strcmp(root, "C:\\installed\\mvdm") != 0) return 3;
    if (app_package_layout_set_media_roots(&instance, "ntvdm64.exe") ||
        !session_valid(&instance)) return 3;
    if (!session_dispose(&instance)) return 5;
    return 0;
}
