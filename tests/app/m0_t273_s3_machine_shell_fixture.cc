#include "app/machine_shell.h"

int main()
{
    session owner;
    app_machine_shell shell;

    session_initialize(&owner, 1u);
    if (!session_activate(&owner)) return 1;
    app_machine_shell_initialize(&shell);
    if (app_machine_shell_open(&shell, &owner, 1000000,
        UINT64_C(0x100000)) != APP_MACHINE_SHELL_OK ||
        !app_machine_shell_active(&shell))
        return 2;
    if (!session_dispose(&owner) || app_machine_shell_active(&shell))
        return 3;
    return 0;
}
