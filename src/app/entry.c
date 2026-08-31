#include "app/machine_shell.h"
#include "app/launch_declaration.h"
#include "app/package_layout.h"

/* App owns these process-assembly outcomes.  They deliberately do not
 * describe an original SoftPC/guest result: a result returned by the
 * original entry remains untouched below.  Keeping the early outcomes
 * distinct lets the fixed non-debug launch container identify whether the
 * product reached original host startup without changing console ownership,
 * debugger state, or launch arguments. */
enum app_startup_status {
    APP_STARTUP_OPTIONS_REJECTED = 64,
    APP_STARTUP_MEDIA_REJECTED = 65,
    APP_STARTUP_MACHINE_REJECTED = 66,
    APP_STARTUP_SESSION_REJECTED = 67,
    APP_STARTUP_DECLARATION_REJECTED = 68,
    APP_STARTUP_COMMAND_REJECTED = 69,
    APP_STARTUP_SHELL_REJECTED = 70,
    APP_STARTUP_MACHINE_FAILURE = 71,
    APP_STARTUP_DISPOSE_FAILURE = 72
};

/* The application owns only process/session assembly.  Guest loading,
 * host initialization and CPU execution stay in the original SoftPC entry
 * selected by app_machine_shell_run(). */
int main(int argc, char **argv)
{
    app_machine_shell shell;
    app_launch_declaration declaration;
    session owner;
    int result = 1;

    session_initialize(&owner, 1u);
    app_machine_shell_initialize(&shell);
    app_launch_declaration_initialize(&declaration);
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv)) {
        result = APP_STARTUP_OPTIONS_REJECTED;
        goto finish;
    }
    if (!app_package_layout_set_process_media_roots(&owner)) {
        result = APP_STARTUP_MEDIA_REJECTED;
        goto finish;
    }
    if (!app_machine_shell_select_backend(&owner,
            SESSION_MACHINE_BACKEND_NONE)) {
        result = APP_STARTUP_MACHINE_REJECTED;
        goto finish;
    }
    if (!session_activate(&owner)) {
        result = APP_STARTUP_SESSION_REJECTED;
        goto finish;
    }
    /* The adapter owns the copied Base VDM/VDMINFO contract.  App only binds
     * the session-local declaration before the original entry asks whether it
     * is the first DOS VDM; it neither loads guest bytes nor supplies a guest
     * lifecycle. */
    if (!app_launch_declaration_bind(&declaration, &owner)) {
        result = APP_STARTUP_DECLARATION_REJECTED;
        goto finish;
    }
    if (declaration.ordinary_child != 0u &&
        !app_launch_declaration_publish(&declaration, &owner)) {
        result = APP_STARTUP_COMMAND_REJECTED;
        goto finish;
    }

    /* The current shell ABI validates nonzero capacity arguments but does
     * not consume them.  Original SoftPC owns machine initialization. */
    if (app_machine_shell_open(&shell, &owner, 1u, UINT64_C(1)) !=
            APP_MACHINE_SHELL_OK) {
        result = APP_STARTUP_SHELL_REJECTED;
        goto finish;
    }
    if (app_machine_shell_run(&shell, argc, argv, &result) !=
            APP_MACHINE_SHELL_OK) {
        result = APP_STARTUP_MACHINE_FAILURE;
    }

finish:
    if (!session_dispose(&owner)) return APP_STARTUP_DISPOSE_FAILURE;
    return result;
}
