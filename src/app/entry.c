#include "app/machine_shell.h"
#include "app/launch_declaration.h"
#include "app/package_layout.h"

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
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv))
        goto finish;
    if (!app_package_layout_set_process_media_roots(&owner)) goto finish;
    if (!app_machine_shell_select_backend(&owner,
            SESSION_MACHINE_BACKEND_NONE)) goto finish;
    if (!session_activate(&owner)) goto finish;
    /* The adapter owns the copied Base VDM/VDMINFO contract.  App only binds
     * the session-local declaration before the original entry asks whether it
     * is the first DOS VDM; it neither loads guest bytes nor supplies a guest
     * lifecycle. */
    if (!app_launch_declaration_bind(&declaration, &owner)) goto finish;
    if (declaration.ordinary_child != 0u &&
        !app_launch_declaration_publish(&declaration, &owner)) goto finish;

    /* The current shell ABI validates nonzero capacity arguments but does
     * not consume them.  Original SoftPC owns machine initialization. */
    if (app_machine_shell_open(&shell, &owner, 1u, UINT64_C(1)) !=
            APP_MACHINE_SHELL_OK) goto finish;
    if (app_machine_shell_run(&shell, argc, argv, &result) !=
            APP_MACHINE_SHELL_OK) {
        result = 1;
    }

finish:
    if (!session_dispose(&owner)) return 1;
    return result;
}
