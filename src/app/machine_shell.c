#include "app/machine_shell.h"
#include "mvdm_softpc_execution.h"

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
    if (!app_machine_shell_valid(shell) || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        ips == 0u || machine_memory_bytes == 0u)
        return APP_MACHINE_SHELL_REJECTED_INPUT;
    if (shell->active != 0u)
        return APP_MACHINE_SHELL_REJECTED_STATE;
    if (session_machine_backend(owner) != SESSION_MACHINE_BACKEND_SOFTPC)
        return APP_MACHINE_SHELL_REJECTED_STATE;

    /* CCPU40's original host_cpu_init() is intentionally empty.  The shell
     * therefore only arms the session; app_machine_shell_run() enters the
     * original ntvdm.c -> host_main lifecycle rather than pretending this is
     * CPU initialization. */
    (void)ips;
    (void)machine_memory_bytes;

    shell->owner = owner;
    shell->active = 1u;
    if (!session_register_teardown(owner, app_machine_shell_teardown, shell)) {
        shell->owner = NULL;
        shell->active = 0u;
        return APP_MACHINE_SHELL_MACHINE_FAILURE;
    }
    return APP_MACHINE_SHELL_OK;
}

enum app_machine_shell_status app_machine_shell_run(app_machine_shell *shell,
    int argc, char **argv, int *result_out)
{
    if (!app_machine_shell_valid(shell) || shell->active == 0u ||
        shell->owner == NULL || result_out == NULL)
        return APP_MACHINE_SHELL_REJECTED_INPUT;
    if (!mvdm_softpc_execution_run_original_entry(shell->owner, argc, argv,
            result_out))
        return APP_MACHINE_SHELL_MACHINE_FAILURE;
    return APP_MACHINE_SHELL_OK;
}

enum app_machine_shell_status app_machine_shell_close(app_machine_shell *shell)
{
    if (!app_machine_shell_valid(shell))
        return APP_MACHINE_SHELL_REJECTED_INPUT;
    if (shell->active == 0u)
        return APP_MACHINE_SHELL_REJECTED_STATE;
    shell->owner = NULL;
    shell->active = 0u;
    return APP_MACHINE_SHELL_OK;
}

int app_machine_shell_active(const app_machine_shell *shell)
{
    return app_machine_shell_valid(shell) && shell->active != 0u;
}
