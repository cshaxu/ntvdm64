/* Pure worker composition. run16 owns classification, command construction
 * and the inherited VDM environment; BaseSrv owns command availability.
 * Compile-only until the independent link selects authenticated BaseClient
 * transport. Never link this entry against the legacy local queue provider. */
#include "app/machine_shell.h"
#include "app/package_layout.h"
#include <windows.h>

int main(int argc,char **argv)
{
    session owner;
    app_machine_shell shell;
    uint32_t dispose_reason;
    int result=ERROR_INVALID_STATE;
    session_initialize(&owner,1u);
    app_machine_shell_initialize(&shell);
    if (!app_package_layout_set_process_media_roots(&owner) ||
        !app_package_layout_validate_command_configuration_root(&owner)) {
        result=ERROR_BAD_PATHNAME;
        goto finish;
    }
    if (!app_machine_shell_select_backend(&owner,SESSION_MACHINE_BACKEND_SOFTPC) ||
        !session_activate(&owner)) goto finish;
    if (app_machine_shell_open(&shell,&owner,1u,UINT64_C(1))!=APP_MACHINE_SHELL_OK) goto finish;
    /* No private argv shadow: original ntvdm.c/host_main see the launcher's
     * actual command line, with the original -w/-a/-i contract intact. */
    if (app_machine_shell_run(&shell,argc,argv,&result)!=APP_MACHINE_SHELL_OK)
        result=ERROR_PROCESS_ABORTED;
finish:
    if (!session_dispose_with_reason(&owner,&dispose_reason)) return ERROR_BUSY;
    return result;
}
