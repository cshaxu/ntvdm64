#include "app/machine_shell.h"

int main()
{
    session owner;
    session softpc_owner;
    app_machine_shell shell;

    session_initialize(&owner, 1u);
    if (!app_machine_shell_select_backend(&owner, SESSION_MACHINE_BACKEND_BOCHS) ||
        app_machine_shell_select_backend(&owner, SESSION_MACHINE_BACKEND_SOFTPC))
        return 1;
    if (!session_activate(&owner)) return 1;
    app_machine_shell_initialize(&shell);
    if (app_machine_shell_open(&shell, &owner, 1000000,
        UINT64_C(0x100000)) != APP_MACHINE_SHELL_OK ||
        !app_machine_shell_active(&shell))
        return 2;
    if (app_machine_shell_select_backend(&owner, SESSION_MACHINE_BACKEND_SOFTPC))
        return 3;
    if (!session_dispose(&owner) || app_machine_shell_active(&shell))
        return 4;

    session_initialize(&softpc_owner, 2u);
    if (!app_machine_shell_select_backend(&softpc_owner,
            SESSION_MACHINE_BACKEND_NONE) ||
        session_machine_backend(&softpc_owner) != APP_MACHINE_SHELL_DEFAULT_BACKEND ||
        !session_activate(&softpc_owner))
        return 5;
    app_machine_shell_initialize(&shell);
    if (app_machine_shell_open(&shell, &softpc_owner, 1000000,
            UINT64_C(0x100000)) != APP_MACHINE_SHELL_BACKEND_UNAVAILABLE ||
        app_machine_shell_active(&shell) || !session_dispose(&softpc_owner))
        return 6;
    return 0;
}
