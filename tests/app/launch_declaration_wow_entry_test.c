#include "app/launch_declaration.h"
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
        !session_activate(&owner) ||
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
        !session_activate(&owner) ||
        !app_launch_declaration_select_requested_image(&declaration,
            MVDM_IMAGE_DOS)) goto done;
    if (!app_launch_declaration_prepare_softpc_arguments(&declaration, &owner,
            1, arguments, &forwarded_count, &forwarded)) goto done;
    result = forwarded_count == 2 && strcmp(forwarded[0], executable) == 0 &&
        strcmp(forwarded[1], "-f") == 0;
done:
    app_launch_declaration_release_softpc_arguments(forwarded);
    (void)session_dispose(&owner);
    return result;
}

int main(void)
{
    return verify_wow_arguments() && verify_dos_arguments() ? 0 : 1;
}
