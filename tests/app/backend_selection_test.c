#include "app/machine_shell.h"
#include "adapter-bochs/machine_lifecycle.h"

static uint32_t created;

enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_create(
    const struct adapter_bochs_machine_lifecycle_configuration *configuration)
{
    if (configuration == 0 || created != 0u) {
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INPUT;
    }
    created = 1u;
    return ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK;
}

enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_destroy(void)
{
    if (created == 0u) return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INACTIVE;
    created = 0u;
    return ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK;
}

int main(void)
{
    session bochs;
    session default_softpc;
    app_machine_shell shell;

    session_initialize(&bochs, 401u);
    if (!app_machine_shell_select_backend(&bochs,
            SESSION_MACHINE_BACKEND_BOCHS) || !session_activate(&bochs))
        return 1;
    app_machine_shell_initialize(&shell);
    if (app_machine_shell_open(&shell, &bochs, 1000000u,
            UINT64_C(0x100000)) != APP_MACHINE_SHELL_OK || created != 1u ||
        !session_dispose(&bochs) || created != 0u)
        return 2;

    session_initialize(&default_softpc, 402u);
    if (!app_machine_shell_select_backend(&default_softpc,
            SESSION_MACHINE_BACKEND_NONE) ||
        session_machine_backend(&default_softpc) != APP_MACHINE_SHELL_DEFAULT_BACKEND ||
        !session_activate(&default_softpc))
        return 3;
    app_machine_shell_initialize(&shell);
    if (app_machine_shell_open(&shell, &default_softpc, 1000000u,
            UINT64_C(0x100000)) != APP_MACHINE_SHELL_BACKEND_UNAVAILABLE ||
        created != 0u || !session_dispose(&default_softpc))
        return 4;
    return 0;
}
