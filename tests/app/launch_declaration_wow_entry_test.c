#include "app/launch_declaration.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_firmware.h"
#include "session/session.h"

#include <string.h>

static int verify_wow_arguments(void)
{
    app_launch_declaration declaration;
    session owner;
    char executable[] = "ntvdm32.exe";
    char *arguments[] = { executable };
    char **forwarded = NULL;
    int forwarded_count = 0;
    int result = 0;

    session_initialize(&owner, 61u);
    app_launch_declaration_initialize(&declaration);
    if (!session_set_mvdm_system_root(&owner, "O:\\ntvdm64") ||
        !app_launch_declaration_select_requested_image(&declaration,
            MVDM_IMAGE_WIN16)) goto done;
    if (!app_launch_declaration_prepare_softpc_arguments(&declaration, &owner,
            1, arguments, &forwarded_count, &forwarded)) goto done;
    result = forwarded_count == 5 && strcmp(forwarded[0], executable) == 0 &&
        strcmp(forwarded[1], "-f") == 0 && strcmp(forwarded[2], "-w") == 0 &&
        strcmp(forwarded[3], "-a") == 0;
    /* The original WOW parser finds the path after ` -a `; the injected
     * token must be a non-user, fully-qualified KRNL386 path. */
    result = result != 0 && strstr(forwarded[4], "\\KRNL386.EXE") != NULL &&
        forwarded[4][1] == ':';
    result = result != 0 && session_mvdm_wow_bootstrap_kernel(&owner) != NULL &&
        strcmp(session_mvdm_wow_bootstrap_kernel(&owner), forwarded[4]) == 0;
    if (!session_activate(&owner) || !session_thread_bind(&owner)) goto done;
    {
        char copied[MAX_PATH];
        result = result != 0 && mvdm_softpc_wow_bootstrap_kernel_copy(copied,
            (uint32_t)sizeof(copied)) && strcmp(copied, forwarded[4]) == 0;
    }
    if (!session_thread_unbind(&owner)) goto done;
done:
    app_launch_declaration_release_softpc_arguments(forwarded);
    (void)session_dispose(&owner);
    return result;
}

static int verify_dos_arguments(void)
{
    app_launch_declaration declaration;
    session owner;
    char executable[] = "ntvdm32.exe";
    char *arguments[] = { executable };
    char **forwarded = NULL;
    int forwarded_count = 0;
    int result = 0;

    session_initialize(&owner, 62u);
    app_launch_declaration_initialize(&declaration);
    if (!session_set_mvdm_system_root(&owner, "O:\\ntvdm64") ||
        !app_launch_declaration_select_requested_image(&declaration,
            MVDM_IMAGE_DOS)) goto done;
    if (!app_launch_declaration_prepare_softpc_arguments(&declaration, &owner,
            1, arguments, &forwarded_count, &forwarded)) goto done;
    result = forwarded_count == 2 && strcmp(forwarded[0], executable) == 0 &&
        strcmp(forwarded[1], "-f") == 0;
    result = result != 0 && session_mvdm_wow_bootstrap_kernel(&owner) == NULL;
    if (!session_activate(&owner) || !session_thread_bind(&owner)) goto done;
    {
        char copied[MAX_PATH];
        result = result != 0 && !mvdm_softpc_wow_bootstrap_kernel_copy(copied,
            (uint32_t)sizeof(copied));
    }
    if (!session_thread_unbind(&owner)) goto done;
done:
    app_launch_declaration_release_softpc_arguments(forwarded);
    (void)session_dispose(&owner);
    return result;
}

static int verify_public_worker_options_rejected(void)
{
    app_launch_declaration declaration;
    char executable[] = "ntvdm32.exe";
    char write[] = "WRITE.EXE";
    char command[] = "--command";
    char wow[] = "-w";
    char application[] = "-a";
    char observer[] = "-o";
    char *command_arguments[] = { executable, command, write };
    char *wow_arguments[] = { executable, wow, write };
    char *application_arguments[] = { executable, application, write };
    char *observer_arguments[] = { executable, observer, write };
    char *positional_arguments[] = { executable, write };
    int count;

    app_launch_declaration_initialize(&declaration);
    count = 3;
    if (app_launch_declaration_consume_options(&declaration, &count,
            command_arguments)) return 0;
    app_launch_declaration_initialize(&declaration);
    count = 3;
    if (app_launch_declaration_consume_options(&declaration, &count,
            wow_arguments)) return 0;
    app_launch_declaration_initialize(&declaration);
    count = 3;
    if (app_launch_declaration_consume_options(&declaration, &count,
            application_arguments)) return 0;
    app_launch_declaration_initialize(&declaration);
    count = 3;
    if (app_launch_declaration_consume_options(&declaration, &count,
            observer_arguments)) return 0;
    app_launch_declaration_initialize(&declaration);
    count = 2;
    return app_launch_declaration_consume_options(&declaration, &count,
        positional_arguments) && count == 1 && declaration.command_declared != 0u &&
        strcmp(declaration.requested_command, write) == 0;
}

int main(void)
{
    return verify_wow_arguments() && verify_dos_arguments() &&
        verify_public_worker_options_rejected() ? 0 : 1;
}
