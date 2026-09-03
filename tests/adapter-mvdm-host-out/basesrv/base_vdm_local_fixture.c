#include <stdio.h>
#include <string.h>

#include "base_vdm_local.h"
#include "app/launch_declaration.h"
#include "session/session.h"
#include "thread_start_compat.h"

static void reset_info(VDMINFO *information)
{
    memset(information, 0, sizeof(*information));
}

typedef struct publish_context {
    base_vdm_local *record;
    const base_vdm_command *command;
    uint32_t observed_pending;
} publish_context;

/* thread_start_compat deliberately preserves the original SoftPC cdecl
 * CreateThread callback contract on x86.  This fixture producer enters that
 * same boundary, so it must not use a WINAPI callback. */
static DWORD __cdecl publish_after_wait(LPVOID parameter)
{
    publish_context *context = (publish_context *)parameter;
    unsigned int attempt;
    for (attempt = 0u; attempt != 100u; ++attempt) {
        EnterCriticalSection(&context->record->lock);
        context->observed_pending = context->record->pending_request;
        LeaveCriticalSection(&context->record->lock);
        if (context->observed_pending != 0u) break;
        Sleep(1u);
    }
    if (!base_vdm_local_publish(context->record, context->command)) return 1u;
    return context->observed_pending != 0u ? 0u : 2u;
}

static volatile LONG worker_reentry_result;

static VOID __cdecl worker_reentry(void)
{
    VDMINFO information;
    reset_info(&information);
    information.VDMState = INCREMENT_REENTER_COUNT;
    if (!GetNextVDMCommand(&information)) {
        worker_reentry_result = 1;
        return;
    }
    information.VDMState = DECREMENT_REENTER_COUNT;
    worker_reentry_result = GetNextVDMCommand(&information) ? 0 : 2;
}

static int verify_long_package_launch_declaration(void)
{
    static char program[] = "fixture";
    static char command_option[] = "--command";
    static char command_text[] = "T365TEST.COM";
    char *argv[] = { program, command_option, command_text, NULL };
    const char *root =
        "O:\\repos.hobby\\ntvdm64\\build\\M0-T318\\S2\\runtime-r37-command-ingress-after-complete-host-path-fix\\mvdm";
    const char *application =
        "O:\\repos.hobby\\ntvdm64\\build\\M0-T318\\S2\\runtime-r37-command-ingress-after-complete-host-path-fix\\mvdm\\system32\\COMMAND.COM";
    int argc = 3;
    session instance;
    app_launch_declaration declaration;

    session_initialize(&instance, 2u);
    app_launch_declaration_initialize(&declaration);
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv)) return 25;
    if (!session_set_mvdm_system_root(&instance, root)) return 26;
    if (!session_activate(&instance)) return 27;
    if (!app_launch_declaration_bind(&declaration, &instance)) return 28;
    if (!app_launch_declaration_publish(&declaration, &instance)) return 29;
    if (declaration.base_vdm.available != 1u) return 30;
    if (declaration.base_vdm.current_directory_bytes != strlen(root) + 1u) return 31;
    if (declaration.base_vdm.application_bytes != strlen(application) + 1u) return 32;
    if (declaration.base_vdm.command_owner != BASE_VDM_COMMAND_DOS) return 36;
    if (strcmp((const char *)declaration.base_vdm.command,
            "/C T365TEST.COM\r\n") != 0) return 41;
    if (declaration.base_vdm.command_bytes < 3u ||
        declaration.base_vdm.command[declaration.base_vdm.command_bytes - 3u] != '\r' ||
        declaration.base_vdm.command[declaration.base_vdm.command_bytes - 2u] != '\n' ||
        declaration.base_vdm.command[declaration.base_vdm.command_bytes - 1u] != '\0') return 35;
    if (!base_vdm_local_unbind(&declaration.base_vdm)) return 33;
    if (!session_dispose(&instance)) return 34;
    return 0;
}

static int verify_rejected_launch_declarations(void)
{
    static char program[] = "fixture";
    static char option[] = "--command";
    static char empty[] = "";
    static char line_break[] = "EXIT\n";
    static char first[] = "VER";
    static char second[] = "EXIT";
    app_launch_declaration declaration;
    int argc;
    char *missing[] = { program, option, NULL };
    char *empty_value[] = { program, option, empty, NULL };
    char *multiline_value[] = { program, option, line_break, NULL };
    char *duplicate[] = { program, option, first, option, second, NULL };

    app_launch_declaration_initialize(&declaration);
    argc = 2;
    if (app_launch_declaration_consume_options(&declaration, &argc, missing))
        return 37;
    app_launch_declaration_initialize(&declaration);
    argc = 3;
    if (app_launch_declaration_consume_options(&declaration, &argc, empty_value))
        return 38;
    app_launch_declaration_initialize(&declaration);
    argc = 3;
    if (app_launch_declaration_consume_options(&declaration, &argc, multiline_value))
        return 39;
    app_launch_declaration_initialize(&declaration);
    argc = 5;
    if (app_launch_declaration_consume_options(&declaration, &argc, duplicate))
        return 40;
    return 0;
}

static int verify_positional_launch_declaration(void)
{
    static char program[] = "fixture";
    static char stream_option[] = "-o";
    static char echo[] = "echo";
    static char message[] = "hello world";
    char *argv[] = { program, stream_option, echo, message, NULL };
    char **softpc_argv = NULL;
    int argc = 4;
    int softpc_argc = 0;
    app_launch_declaration declaration;

    app_launch_declaration_initialize(&declaration);
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv)) return 42;
    if (argc != 2 || strcmp(argv[1], "-o") != 0 ||
        !declaration.command_declared ||
        strcmp(declaration.requested_command, "echo \"hello world\"") != 0)
        return 43;
    if (!app_launch_declaration_prepare_softpc_arguments(argc, argv,
            &softpc_argc, &softpc_argv)) return 44;
    if (softpc_argc != 3 || strcmp(softpc_argv[1], "-o") != 0 ||
        strcmp(softpc_argv[2], "-f") != 0) {
        app_launch_declaration_release_softpc_arguments(softpc_argv);
        return 45;
    }
    app_launch_declaration_release_softpc_arguments(softpc_argv);
    return 0;
}

static int verify_empty_launch_declaration(void)
{
    static char program[] = "fixture";
    char *argv[] = { program, NULL };
    char **softpc_argv = NULL;
    int argc = 1;
    int softpc_argc = 0;
    app_launch_declaration declaration;

    app_launch_declaration_initialize(&declaration);
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv) ||
        argc != 1 || declaration.command_declared != 0u) return 46;
    if (!app_launch_declaration_prepare_softpc_arguments(argc, argv,
            &softpc_argc, &softpc_argv)) return 47;
    if (softpc_argc != 2 || strcmp(softpc_argv[1], "-f") != 0) {
        app_launch_declaration_release_softpc_arguments(softpc_argv);
        return 48;
    }
    app_launch_declaration_release_softpc_arguments(softpc_argv);
    return 0;
}

static int verify_interactive_initial_launch_record(void)
{
    static char program[] = "fixture";
    char *argv[] = { program, NULL };
    const char *root = "C:\\MVDM";
    const char *application = "C:\\MVDM\\system32\\COMMAND.COM";
    int argc = 1;
    session instance;
    app_launch_declaration declaration;

    session_initialize(&instance, 9u);
    app_launch_declaration_initialize(&declaration);
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv) ||
        declaration.command_declared != 0u) return 53;
    if (!session_set_mvdm_system_root(&instance, root) ||
        !session_activate(&instance) ||
        !app_launch_declaration_bind(&declaration, &instance) ||
        !app_launch_declaration_publish(&declaration, &instance)) return 54;
    if (declaration.base_vdm.terminal_on_command_exhaustion != 0u ||
        declaration.base_vdm.command_owner != BASE_VDM_COMMAND_DOS ||
        declaration.base_vdm.command_bytes != 3u ||
        memcmp(declaration.base_vdm.command, "\r\n\0", 3u) != 0 ||
        declaration.base_vdm.application_bytes != strlen(application) + 1u ||
        strcmp((const char *)declaration.base_vdm.application, application) != 0) {
        (void)base_vdm_local_unbind(&declaration.base_vdm);
        (void)session_dispose(&instance);
        return 55;
    }
    if (!base_vdm_local_unbind(&declaration.base_vdm) ||
        !session_dispose(&instance)) return 56;
    return 0;
}

static int verify_explicit_command_child_record(void)
{
    static char program[] = "fixture";
    static char command_name[] = "command.com";
    char *argv[] = { program, command_name, NULL };
    const char *root = "C:\\MVDM";
    int argc = 2;
    session instance;
    app_launch_declaration declaration;

    session_initialize(&instance, 10u);
    app_launch_declaration_initialize(&declaration);
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv) ||
        argc != 1 || declaration.command_declared != 1u ||
        strcmp(declaration.requested_command, "command.com") != 0 ||
        !session_set_mvdm_system_root(&instance, root) ||
        !session_activate(&instance) ||
        !app_launch_declaration_bind(&declaration, &instance) ||
        !app_launch_declaration_publish(&declaration, &instance)) return 57;
    /* `/C` is consumed by the first resident COMMAND.COM.  The second
     * COMMAND.COM is its original executable child, so app must publish no
     * second record and no special child-tail format. */
    if (declaration.base_vdm.terminal_on_command_exhaustion != 1u ||
        declaration.base_vdm.command_owner != BASE_VDM_COMMAND_DOS ||
        strcmp((const char *)declaration.base_vdm.command,
            "/C command.com\r\n") != 0 ||
        declaration.base_vdm.command_bytes != sizeof("/C command.com\r\n")) {
        (void)base_vdm_local_unbind(&declaration.base_vdm);
        (void)session_dispose(&instance);
        return 58;
    }
    if (!base_vdm_local_unbind(&declaration.base_vdm) ||
        !session_dispose(&instance)) return 59;
    return 0;
}

static int verify_explicit_softpc_and_command_options(void)
{
    static char program[] = "fixture";
    static char foreground_option[] = "-f";
    static char stream_option[] = "-o";
    static char command_option[] = "--command";
    static char command_text[] = "VER";
    char *argv[] = { program, foreground_option, stream_option,
        command_option, command_text, NULL };
    char **softpc_argv = NULL;
    int argc = 5;
    int softpc_argc = 0;
    app_launch_declaration declaration;

    app_launch_declaration_initialize(&declaration);
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv))
        return 49;
    if (argc != 3 || strcmp(argv[1], "-f") != 0 ||
        strcmp(argv[2], "-o") != 0 || !declaration.command_declared ||
        strcmp(declaration.requested_command, "VER") != 0) return 50;
    if (!app_launch_declaration_prepare_softpc_arguments(argc, argv,
            &softpc_argc, &softpc_argv)) return 51;
    if (softpc_argc != 3 || strcmp(softpc_argv[1], "-f") != 0 ||
        strcmp(softpc_argv[2], "-o") != 0) {
        app_launch_declaration_release_softpc_arguments(softpc_argv);
        return 52;
    }
    app_launch_declaration_release_softpc_arguments(softpc_argv);
    return 0;
}

int main(void)
{
    static const uint8_t command[] = "C:\\DOS\\COMMAND.COM /C VER\r\n";
    static const uint8_t application[] = "C:\\DOS\\COMMAND.COM";
    static const uint8_t environment[] = "COMSPEC=C:\\DOS\\COMMAND.COM\0PATH=C:\\DOS\0\0";
    static const uint8_t directory[] = "C:\\DOS\0";
    base_vdm_command payload;
    base_vdm_local source;
    session instance;
    VDMINFO information;
    uint8_t command_buffer[MAXIMUM_VDM_COMMAND_LENGTH];
    uint8_t application_buffer[MAXIMUM_VDM_PATH_STRING];
    uint8_t environment_buffer[MAXIMUM_VDM_ENVIORNMENT];
    uint8_t directory_buffer[MAXIMUM_VDM_CURRENT_DIR];
    uint8_t reserved_buffer[64];
    static const CHAR directories[] = "=C:=C:\\DOS\0=D:=D:\\WORK\0\0";
    CHAR directory_copy[sizeof(directories)];
    publish_context producer;
    HANDLE producer_thread;
    HANDLE reentry_thread;
    DWORD producer_result = UINT32_MAX;
    int launch_declaration_result;

    launch_declaration_result = verify_long_package_launch_declaration();
    if (launch_declaration_result != 0) return launch_declaration_result;
    launch_declaration_result = verify_rejected_launch_declarations();
    if (launch_declaration_result != 0) return launch_declaration_result;
    launch_declaration_result = verify_positional_launch_declaration();
    if (launch_declaration_result != 0) return launch_declaration_result;
    launch_declaration_result = verify_empty_launch_declaration();
    if (launch_declaration_result != 0) return launch_declaration_result;
    launch_declaration_result = verify_interactive_initial_launch_record();
    if (launch_declaration_result != 0) return launch_declaration_result;
    launch_declaration_result = verify_explicit_command_child_record();
    if (launch_declaration_result != 0) return launch_declaration_result;
    launch_declaration_result = verify_explicit_softpc_and_command_options();
    if (launch_declaration_result != 0) return launch_declaration_result;
    reset_info(&information);
    if (GetNextVDMCommand(NULL) || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
        return 1;
    session_initialize(&instance, 1u);
    base_vdm_local_initialize(&source);
    memset(&payload, 0, sizeof(payload));
    payload.struct_bytes = sizeof(payload);
    payload.task = 7u;
    payload.creation_flags = 19u;
    payload.error_code = 23u;
    payload.code_page = 437u;
    payload.current_drive = 2u;
    payload.coming_from_bat = TRUE;
    payload.command = command;
    payload.command_bytes = (uint16_t)sizeof(command);
    payload.application = application;
    payload.application_bytes = (uint16_t)sizeof(application);
    payload.environment = environment;
    payload.environment_bytes = (uint32_t)sizeof(environment);
    payload.current_directory = directory;
    payload.current_directory_bytes = (uint16_t)sizeof(directory);
    if (!base_vdm_local_publish(&source, &payload) ||
        !session_activate(&instance) || !base_vdm_local_bind(&source, &instance) ||
        !session_thread_bind(&instance)) return 2;
    worker_reentry_result = 3;
    reentry_thread = opennt_create_void_cdecl_thread(NULL, 0u,
        worker_reentry, NULL, 0u, NULL);
    if (reentry_thread == NULL ||
        WaitForSingleObject(reentry_thread, 5000u) != WAIT_OBJECT_0 ||
        worker_reentry_result != 0 || source.reentry_count != 0u) return 24;
    CloseHandle(reentry_thread);
    if (!GetNextVDMCommand(NULL) || GetNextVDMCommand(NULL)) return 14;

    reset_info(&information);
    information.VDMState = ASKING_FOR_ENVIRONMENT;
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = sizeof(environment_buffer);
    if (!GetNextVDMCommand(&information) ||
        information.EnviornmentSize != sizeof(environment) ||
        memcmp(environment_buffer, environment, sizeof(environment)) != 0) return 3;

    /* The original SoftPC bootstrap queries PIF/title/current-directory
     * before it obtains the first DOS command.  BaseSrvFillPifInfo answers
     * from the queued DOS record without consuming that record. */
    reset_info(&information);
    information.VDMState = ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY;
    information.PifFile = command_buffer;
    information.PifLen = sizeof(command_buffer);
    information.Title = application_buffer;
    information.TitleLen = sizeof(application_buffer);
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = sizeof(directory_buffer);
    information.Reserved = reserved_buffer;
    information.ReservedLen = sizeof(reserved_buffer);
    if (!GetNextVDMCommand(&information) || information.PifLen != 0u ||
        information.ReservedLen != 0u ||
        information.TitleLen != sizeof(application) ||
        information.CurDirectoryLen != sizeof(directory) ||
        command_buffer[0] != '\0' || reserved_buffer[0] != '\0' ||
        memcmp(application_buffer, application, sizeof(application)) != 0 ||
        memcmp(directory_buffer, directory, sizeof(directory)) != 0 ||
        source.available != 1u ||
        source.dos_record_state != BASE_VDM_DOS_RECORD_TO_TAKE_A_COMMAND)
        return 42;

    reset_info(&information);
    information.CmdLine = command_buffer;
    information.CmdSize = 1u;
    information.AppName = application_buffer;
    information.AppLen = 1u;
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = 1u;
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = 1u;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_INVALID_PARAMETER ||
        information.CmdSize != sizeof(command) || information.AppLen != sizeof(application) ||
        information.EnviornmentSize != sizeof(environment) ||
        information.CurDirectoryLen != sizeof(directory)) return 4;

    reset_info(&information);
    information.CmdLine = command_buffer;
    information.CmdSize = sizeof(command_buffer);
    information.AppName = application_buffer;
    information.AppLen = sizeof(application_buffer);
    /* Original cmdGetNextCmd's ASKING_FOR_FIRST_COMMAND does not capture a
     * host environment: COMMAND.COM already supplied its guest environment.
     * BaseSrv returns the command record without requiring this optional
     * buffer. */
    information.Enviornment = NULL;
    information.EnviornmentSize = 0u;
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = sizeof(directory_buffer);
    if (!GetNextVDMCommand(&information) || information.VDMState != 0u ||
        information.iTask != 7u || information.dwCreationFlags != 19u ||
        information.ErrorCode != 23u || information.CodePage != 437u ||
        information.CurDrive != 2u || !information.fComingFromBat ||
        information.StdIn != NULL || information.StdOut != NULL || information.StdErr != NULL ||
        memcmp(command_buffer, command, sizeof(command)) != 0 ||
        memcmp(application_buffer, application, sizeof(application)) != 0 ||
        memcmp(directory_buffer, directory, sizeof(directory)) != 0) return 5;

    /* The second client request is the original no-command terminal probe.
     * A one-session record has no external parent process to wake, so this
     * source-shaped check verifies its empty `RETURN_ON_NO_COMMAND` result
     * without inventing a command record. */
    reset_info(&information);
    information.VDMState = RETURN_ON_NO_COMMAND | ASKING_FOR_SECOND_TIME;
    information.ErrorCode = 91u;
    if (GetNextVDMCommand(&information) ||
        GetLastError() != ERROR_NOT_ENOUGH_MEMORY ||
        information.CmdSize != 0u || information.EnviornmentSize != 0u ||
        source.dos_record_state !=
            BASE_VDM_DOS_RECORD_BUSY || source.pending_request != 0u) return 41;

    /* Original BaseClient waits when BaseSrv has no DOS record, then retries
     * after the server's producer signal with ASKING_FOR_SECOND_TIME.  The
     * publisher is deliberately a separate host thread: no caller pointer,
     * guest pointer or native handle enters VDMINFO. */
    producer.record = &source;
    producer.command = &payload;
    producer.observed_pending = 0u;
    producer_thread = CreateThread(NULL, 0u, publish_after_wait, &producer,
        0u, NULL);
    if (producer_thread == NULL) return 21;
    reset_info(&information);
    information.CmdLine = command_buffer;
    information.CmdSize = sizeof(command_buffer);
    information.AppName = application_buffer;
    information.AppLen = sizeof(application_buffer);
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = sizeof(environment_buffer);
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = sizeof(directory_buffer);
    if (!GetNextVDMCommand(&information)) {
        fputs("pending Base VDM request did not complete\n", stderr);
        CloseHandle(producer_thread);
        return 22;
    }
    /* Join before inspecting producer-owned state.  The original client-side
     * retry only observes the completed server signal; it does not race the
     * publisher's bookkeeping. */
    if (WaitForSingleObject(producer_thread, INFINITE) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(producer_thread, &producer_result)) {
        fputs("pending Base VDM producer did not terminate\n", stderr);
        CloseHandle(producer_thread);
        return 23;
    }
    EnterCriticalSection(&source.lock);
    {
        uint32_t pending_request = source.pending_request;
        uint32_t observed_pending = producer.observed_pending;
        LeaveCriticalSection(&source.lock);
        int command_matches = memcmp(command_buffer, command, sizeof(command)) == 0;
        if (information.VDMState != 0u || information.iTask != payload.task ||
            pending_request != 0u || observed_pending == 0u ||
            !command_matches || producer_result != 0u) {
            fprintf(stderr, "pending Base VDM result mismatch: state=%u task=%lu pending=%lu observed=%lu command=%d producer=%lu\n",
                (unsigned)information.VDMState, (unsigned long)information.iTask,
                (unsigned long)pending_request, (unsigned long)observed_pending,
                command_matches, (unsigned long)producer_result);
            CloseHandle(producer_thread);
            return 23;
        }
    }
    CloseHandle(producer_thread);

    reset_info(&information);
    information.VDMState = RETURN_ON_NO_COMMAND | ASKING_FOR_SECOND_TIME;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_NOT_ENOUGH_MEMORY ||
        information.CmdSize != 0u || information.EnviornmentSize != 0u) return 6;
    reset_info(&information);
    information.VDMState = INCREMENT_REENTER_COUNT;
    if (!GetNextVDMCommand(&information)) return 7;
    information.VDMState = DECREMENT_REENTER_COUNT;
    if (!GetNextVDMCommand(&information)) return 8;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_INVALID_PARAMETER)
        return 9;
    /* BaseSrv has distinct DOS and WOW records.  A WOW request cannot
     * consume a DOS record and, unlike a DOS request, returns a successful
     * empty result when its own queue is empty. */
    reset_info(&information);
    information.VDMState = ASKING_FOR_WOW_BINARY;
    if (!GetNextVDMCommand(&information) || information.CmdSize != 0u ||
        information.AppLen != 0u || information.EnviornmentSize != 0u)
        return 10;
    payload.command_owner = BASE_VDM_COMMAND_WOW;
    if (!base_vdm_local_publish(&source, &payload)) return 25;
    reset_info(&information);
    information.VDMState = ASKING_FOR_WOW_BINARY;
    information.CmdLine = command_buffer;
    information.CmdSize = sizeof(command_buffer);
    information.AppName = application_buffer;
    information.AppLen = sizeof(application_buffer);
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = sizeof(environment_buffer);
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = sizeof(directory_buffer);
    if (!GetNextVDMCommand(&information) || information.VDMState != 0u ||
        information.iTask != payload.task ||
        memcmp(command_buffer, command, sizeof(command)) != 0)
        return 26;
    reset_info(&information);
    information.VDMState = ASKING_FOR_SEPWOW_BINARY;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
        return 27;
    if (!SetVDMCurrentDirectories((ULONG)sizeof(directories), (CHAR *)directories))
        return 15;
    if (GetVDMCurrentDirectories(1u, directory_copy) != sizeof(directories) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 16;
    if (GetVDMCurrentDirectories((ULONG)sizeof(directory_copy), directory_copy) !=
        sizeof(directories) || memcmp(directory_copy, directories, sizeof(directories)) != 0)
        return 17;
    if (GetVDMCurrentDirectories((ULONG)sizeof(directory_copy), directory_copy) != 0u)
        return 18;
    ExitVDM(FALSE, 0u);
    if (instance.state != SESSION_STATE_COMPLETED) return 19;
    reset_info(&information);
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_NOT_READY)
        return 20;
    if (!base_vdm_local_unbind(&source)) return 11;
    reset_info(&information);
    if (GetNextVDMCommand(&information) ||
        GetLastError() != ERROR_NOT_READY) return 12;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 13;
    puts("PASS: local Base VDM broker contract");
    return 0;
}
