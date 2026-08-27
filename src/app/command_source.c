#include "app/command_source.h"

#include "vdm_command.h"
#include "session/session.h"

#include <string.h>

static int app_command_bytes_valid(const uint8_t *bytes, uint32_t byte_count,
    uint32_t maximum)
{
    return byte_count <= maximum && (byte_count == 0u || bytes != NULL);
}

static int app_command_copy(uint8_t *destination, const uint8_t *source,
    uint32_t byte_count)
{
    if (byte_count == 0u) return 1;
    if (destination == NULL || source == NULL) return 0;
    memcpy(destination, source, byte_count);
    return 1;
}

void app_command_source_initialize(app_command_source *source)
{
    if (source == NULL) return;
    memset(source, 0, sizeof(*source));
    source->version = APP_COMMAND_SOURCE_VERSION;
    source->struct_bytes = (uint32_t)sizeof(*source);
}

int app_command_source_valid(const app_command_source *source)
{
    return source != NULL && source->version == APP_COMMAND_SOURCE_VERSION &&
        source->struct_bytes == sizeof(*source) && source->reserved0 == 0u &&
        source->command_bytes <= MAXIMUM_VDM_COMMAND_LENGTH &&
        source->application_bytes <= MAXIMUM_VDM_PATH_STRING &&
        source->environment_bytes <= MAXIMUM_VDM_ENVIORNMENT &&
        source->current_directory_bytes <= MAXIMUM_VDM_CURRENT_DIR;
}

int app_command_source_publish(app_command_source *source,
    const app_command_payload *payload)
{
    if (!app_command_source_valid(source) || payload == NULL ||
        payload->struct_bytes != sizeof(*payload) || source->available != 0u ||
        payload->command_bytes == 0u ||
        !app_command_bytes_valid(payload->command, payload->command_bytes,
            MAXIMUM_VDM_COMMAND_LENGTH) ||
        !app_command_bytes_valid(payload->application, payload->application_bytes,
            MAXIMUM_VDM_PATH_STRING) ||
        !app_command_bytes_valid(payload->environment, payload->environment_bytes,
            MAXIMUM_VDM_ENVIORNMENT) ||
        !app_command_bytes_valid(payload->current_directory,
            payload->current_directory_bytes, MAXIMUM_VDM_CURRENT_DIR)) return 0;

    if (!app_command_copy(source->command, payload->command, payload->command_bytes) ||
        !app_command_copy(source->application, payload->application,
            payload->application_bytes) ||
        !app_command_copy(source->environment, payload->environment,
            payload->environment_bytes) ||
        !app_command_copy(source->current_directory, payload->current_directory,
            payload->current_directory_bytes)) return 0;
    source->task = payload->task;
    source->creation_flags = payload->creation_flags;
    source->error_code = payload->error_code;
    source->code_page = payload->code_page;
    source->current_drive = payload->current_drive;
    source->coming_from_bat = payload->coming_from_bat;
    source->command_bytes = payload->command_bytes;
    source->application_bytes = payload->application_bytes;
    source->environment_bytes = payload->environment_bytes;
    source->current_directory_bytes = payload->current_directory_bytes;
    source->available = 1u;
    return 1;
}

static void app_command_source_clear_sizes(PVDMINFO information)
{
    information->CmdSize = 0u;
    information->AppLen = 0u;
    information->PifLen = 0u;
    information->EnviornmentSize = 0u;
    information->CurDirectoryLen = 0u;
    information->DesktopLen = 0u;
    information->TitleLen = 0u;
    information->ReservedLen = 0u;
}

static void app_command_source_required_sizes(const app_command_source *source,
    PVDMINFO information)
{
    information->CmdSize = source->command_bytes;
    information->AppLen = source->application_bytes;
    information->PifLen = 0u;
    information->EnviornmentSize = source->environment_bytes;
    information->CurDirectoryLen = source->current_directory_bytes;
    information->DesktopLen = 0u;
    information->TitleLen = 0u;
    information->ReservedLen = 0u;
}

static int app_command_source_buffer_valid(const void *destination,
    uint32_t capacity, uint32_t required)
{
    return required == 0u || (destination != NULL && capacity >= required);
}

static NTSTATUS app_command_source_reentry(app_command_source *source,
    PVDMINFO information)
{
    if (information->VDMState == INCREMENT_REENTER_COUNT) {
        if (source->reentry_count == UINT32_MAX) return STATUS_INVALID_PARAMETER;
        ++source->reentry_count;
        return STATUS_SUCCESS;
    }
    if (information->VDMState == DECREMENT_REENTER_COUNT) {
        if (source->reentry_count == 0u) return STATUS_INVALID_PARAMETER;
        --source->reentry_count;
        return STATUS_SUCCESS;
    }
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS app_command_source_copy(app_command_source *source,
    PVDMINFO information)
{
    uint16_t requested_state = information->VDMState;

    if ((requested_state & (ASKING_FOR_WOW_BINARY |
            ASKING_FOR_SEPWOW_BINARY | ASKING_FOR_PIF)) != 0u)
        return STATUS_NOT_IMPLEMENTED;
    if (source->available == 0u) {
        app_command_source_clear_sizes(information);
        if ((requested_state & RETURN_ON_NO_COMMAND) != 0u &&
            (requested_state & ASKING_FOR_SECOND_TIME) != 0u)
            return STATUS_NO_MEMORY;
        return STATUS_NOT_IMPLEMENTED;
    }
    if ((requested_state & ASKING_FOR_ENVIRONMENT) != 0u) {
        if (!app_command_source_buffer_valid(information->Enviornment,
                information->EnviornmentSize, source->environment_bytes)) {
            app_command_source_clear_sizes(information);
            information->EnviornmentSize = source->environment_bytes;
            return STATUS_INVALID_PARAMETER;
        }
        (void)app_command_copy((uint8_t *)information->Enviornment,
            source->environment, source->environment_bytes);
        app_command_source_clear_sizes(information);
        information->EnviornmentSize = source->environment_bytes;
        return STATUS_SUCCESS;
    }
    if (!app_command_source_buffer_valid(information->CmdLine,
            information->CmdSize, source->command_bytes) ||
        !app_command_source_buffer_valid(information->AppName,
            information->AppLen, source->application_bytes) ||
        !app_command_source_buffer_valid(information->Enviornment,
            information->EnviornmentSize, source->environment_bytes) ||
        !app_command_source_buffer_valid(information->CurDirectory,
            information->CurDirectoryLen, source->current_directory_bytes)) {
        app_command_source_required_sizes(source, information);
        return STATUS_INVALID_PARAMETER;
    }

    (void)app_command_copy((uint8_t *)information->CmdLine, source->command,
        source->command_bytes);
    (void)app_command_copy((uint8_t *)information->AppName, source->application,
        source->application_bytes);
    (void)app_command_copy((uint8_t *)information->Enviornment, source->environment,
        source->environment_bytes);
    (void)app_command_copy((uint8_t *)information->CurDirectory,
        source->current_directory, source->current_directory_bytes);
    app_command_source_required_sizes(source, information);
    information->VDMState = 0u;
    information->CurDrive = source->current_drive;
    information->StdIn = NULL;
    information->StdOut = NULL;
    information->StdErr = NULL;
    information->iTask = source->task;
    information->CodePage = source->code_page;
    information->dwCreationFlags = source->creation_flags;
    information->ErrorCode = source->error_code;
    information->fComingFromBat = source->coming_from_bat;
    source->available = 0u;
    return STATUS_SUCCESS;
}

static int32_t app_command_source_dispatch(void *context, uint32_t operation,
    void *request_bytes)
{
    app_command_source *source = (app_command_source *)context;
    adapter_vdm_monitor_command_request *request =
        (adapter_vdm_monitor_command_request *)request_bytes;
    if (!app_command_source_valid(source) ||
        operation != ADAPTER_VDM_MONITOR_GET_NEXT_COMMAND || request == NULL ||
        request->version != ADAPTER_VDM_MONITOR_COMMAND_REQUEST_VERSION ||
        request->struct_bytes != sizeof(*request) || request->information == NULL)
        return (int32_t)STATUS_INVALID_PARAMETER;
    if (request->information->VDMState == INCREMENT_REENTER_COUNT ||
        request->information->VDMState == DECREMENT_REENTER_COUNT)
        return (int32_t)app_command_source_reentry(source, request->information);
    return (int32_t)app_command_source_copy(source, request->information);
}

static void app_command_source_teardown(void *context)
{
    app_command_source *source = (app_command_source *)context;
    if (source == NULL) return;
    source->owner = NULL;
}

int app_command_source_bind(app_command_source *source, session *owner)
{
    if (!app_command_source_valid(source) || source->owner != NULL || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE) return 0;
    if (!session_register_control_route(owner,
        ADAPTER_VDM_MONITOR_GET_NEXT_COMMAND, app_command_source_dispatch, source))
        return 0;
    if (!session_register_teardown(owner, app_command_source_teardown, source)) {
        (void)session_unregister_control_route(owner,
            ADAPTER_VDM_MONITOR_GET_NEXT_COMMAND, app_command_source_dispatch, source);
        return 0;
    }
    source->owner = owner;
    return 1;
}

int app_command_source_unbind(app_command_source *source)
{
    if (!app_command_source_valid(source) || source->owner == NULL) return 0;
    if (!session_unregister_control_route(source->owner,
        ADAPTER_VDM_MONITOR_GET_NEXT_COMMAND, app_command_source_dispatch, source)) return 0;
    source->owner = NULL;
    return 1;
}
