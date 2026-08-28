#include "app/machine_shell.h"

#include "adapter-bochs/machine_lifecycle.h"

#include <string.h>

void app_machine_shell_initialize(app_machine_shell *shell)
{
    if (shell == NULL) return;
    memset(shell, 0, sizeof(*shell));
    shell->abi_version = APP_MACHINE_SHELL_VERSION;
    shell->struct_bytes = (uint32_t)sizeof(*shell);
}

int app_machine_shell_valid(const app_machine_shell *shell)
{
    return shell != NULL &&
        shell->abi_version == APP_MACHINE_SHELL_VERSION &&
        shell->struct_bytes == sizeof(*shell) && shell->reserved0 == 0u &&
        (shell->active == 0u || shell->active == 1u) &&
        ((shell->active == 0u && shell->owner == NULL) ||
         (shell->active == 1u && shell->owner != NULL));
}

int app_machine_shell_select_backend(session *owner, uint32_t requested_backend)
{
    uint32_t selected = requested_backend;
    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_READY) return 0;
    if (selected == SESSION_MACHINE_BACKEND_NONE)
        selected = APP_MACHINE_SHELL_DEFAULT_BACKEND;
    return session_select_machine_backend(owner, selected);
}

static void app_machine_shell_teardown(void *context)
{
    app_machine_shell *shell = (app_machine_shell *)context;
    (void)app_machine_shell_close(shell);
}

enum app_machine_shell_status app_machine_shell_open(app_machine_shell *shell,
    session *owner, uint32_t ips, uint64_t machine_memory_bytes)
{
    struct adapter_bochs_machine_lifecycle_configuration configuration;

    if (!app_machine_shell_valid(shell) || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        ips == 0u || machine_memory_bytes == 0u)
        return APP_MACHINE_SHELL_REJECTED_INPUT;
    if (shell->active != 0u)
        return APP_MACHINE_SHELL_REJECTED_STATE;
    if (session_machine_backend(owner) == SESSION_MACHINE_BACKEND_SOFTPC)
        /* Original SoftPC creation is S4 work.  Do not silently compose a
         * Bochs machine for a session which selected the original backend. */
        return APP_MACHINE_SHELL_BACKEND_UNAVAILABLE;
    if (session_machine_backend(owner) != SESSION_MACHINE_BACKEND_BOCHS)
        return APP_MACHINE_SHELL_REJECTED_STATE;

    memset(&configuration, 0, sizeof(configuration));
    configuration.version = ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION;
    configuration.ips = ips;
    configuration.guest_memory_bytes = machine_memory_bytes;
    configuration.host_memory_bytes = machine_memory_bytes;
    if (adapter_bochs_machine_lifecycle_create(&configuration) !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK)
        return APP_MACHINE_SHELL_MACHINE_FAILURE;

    shell->owner = owner;
    shell->active = 1u;
    if (!session_register_teardown(owner, app_machine_shell_teardown, shell)) {
        (void)adapter_bochs_machine_lifecycle_destroy();
        shell->owner = NULL;
        shell->active = 0u;
        return APP_MACHINE_SHELL_MACHINE_FAILURE;
    }
    return APP_MACHINE_SHELL_OK;
}

enum app_machine_shell_status app_machine_shell_close(app_machine_shell *shell)
{
    enum adapter_bochs_machine_lifecycle_status status;
    if (!app_machine_shell_valid(shell))
        return APP_MACHINE_SHELL_REJECTED_INPUT;
    if (shell->active == 0u)
        return APP_MACHINE_SHELL_REJECTED_STATE;
    status = adapter_bochs_machine_lifecycle_destroy();
    if (status != ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK)
        return APP_MACHINE_SHELL_MACHINE_FAILURE;
    shell->owner = NULL;
    shell->active = 0u;
    return APP_MACHINE_SHELL_OK;
}

int app_machine_shell_active(const app_machine_shell *shell)
{
    return app_machine_shell_valid(shell) && shell->active != 0u;
}
