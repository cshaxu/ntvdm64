#include <stdio.h>
#include <string.h>

#include "base_vdm_local.h"
#include "session/session.h"

static void reset_info(VDMINFO *information)
{
    memset(information, 0, sizeof(*information));
}

typedef struct publish_context {
    base_vdm_local *record;
    const base_vdm_command *command;
    uint32_t observed_pending;
} publish_context;

static DWORD WINAPI publish_after_wait(LPVOID parameter)
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

int main(void)
{
    static const uint8_t command[] = "C:\\DOS\\COMMAND.COM /C VER";
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
    static const CHAR directories[] = "=C:=C:\\DOS\0=D:=D:\\WORK\0\0";
    CHAR directory_copy[sizeof(directories)];
    publish_context producer;
    HANDLE producer_thread;
    DWORD producer_result = UINT32_MAX;

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
    if (!GetNextVDMCommand(NULL) || GetNextVDMCommand(NULL)) return 14;

    reset_info(&information);
    information.VDMState = ASKING_FOR_ENVIRONMENT;
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = sizeof(environment_buffer);
    if (!GetNextVDMCommand(&information) ||
        information.EnviornmentSize != sizeof(environment) ||
        memcmp(environment_buffer, environment, sizeof(environment)) != 0) return 3;

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
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = sizeof(environment_buffer);
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = sizeof(directory_buffer);
    if (!GetNextVDMCommand(&information) || information.VDMState != 0u ||
        information.iTask != 7u || information.dwCreationFlags != 19u ||
        information.ErrorCode != 23u || information.CodePage != 437u ||
        information.CurDrive != 2u || !information.fComingFromBat ||
        information.StdIn != NULL || information.StdOut != NULL || information.StdErr != NULL ||
        memcmp(command_buffer, command, sizeof(command)) != 0 ||
        memcmp(application_buffer, application, sizeof(application)) != 0 ||
        memcmp(environment_buffer, environment, sizeof(environment)) != 0 ||
        memcmp(directory_buffer, directory, sizeof(directory)) != 0) return 5;

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
    if (information.VDMState != 0u || information.iTask != payload.task ||
        source.pending_request != 0u || producer.observed_pending == 0u ||
        memcmp(command_buffer, command, sizeof(command)) != 0 ||
        WaitForSingleObject(producer_thread, INFINITE) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(producer_thread, &producer_result) ||
        producer_result != 0u) {
        fprintf(stderr, "pending Base VDM result mismatch: state=%u task=%lu pending=%lu observed=%lu producer=%lu\n",
            (unsigned)information.VDMState, (unsigned long)information.iTask,
            (unsigned long)source.pending_request,
            (unsigned long)producer.observed_pending,
            (unsigned long)producer_result);
        CloseHandle(producer_thread);
        return 23;
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
    reset_info(&information);
    information.VDMState = ASKING_FOR_WOW_BINARY;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
        return 10;
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
