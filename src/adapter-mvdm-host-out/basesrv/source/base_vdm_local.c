#include "base_vdm_local.h"

#include "session/session.h"

#include <string.h>

static __declspec(thread) base_vdm_local *base_vdm_current;

static int valid_bytes(const uint8_t *bytes, uint32_t count, uint32_t maximum)
{
    return count <= maximum && (count == 0u || bytes != NULL);
}

static int copy_bytes(uint8_t *destination, const uint8_t *source, uint32_t count)
{
    if (count == 0u) return 1;
    if (destination == NULL || source == NULL) return 0;
    memcpy(destination, source, count);
    return 1;
}

void base_vdm_local_initialize(base_vdm_local *record)
{
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->version = BASE_VDM_LOCAL_VERSION;
    record->struct_bytes = (uint32_t)sizeof(*record);
}

int base_vdm_local_valid(const base_vdm_local *record)
{
    return record != NULL && record->version == BASE_VDM_LOCAL_VERSION &&
        record->struct_bytes == sizeof(*record) && record->reserved0 == 0u &&
        record->command_bytes <= MAXIMUM_VDM_COMMAND_LENGTH &&
        record->application_bytes <= MAXIMUM_VDM_PATH_STRING &&
        record->environment_bytes <= MAXIMUM_VDM_ENVIORNMENT &&
        record->current_directory_bytes <= MAXIMUM_VDM_CURRENT_DIR;
}

int base_vdm_local_publish(base_vdm_local *record, const base_vdm_command *command)
{
    if (!base_vdm_local_valid(record) || command == NULL ||
        command->struct_bytes != sizeof(*command) || record->available != 0u ||
        command->command_bytes == 0u ||
        !valid_bytes(command->command, command->command_bytes, MAXIMUM_VDM_COMMAND_LENGTH) ||
        !valid_bytes(command->application, command->application_bytes, MAXIMUM_VDM_PATH_STRING) ||
        !valid_bytes(command->environment, command->environment_bytes, MAXIMUM_VDM_ENVIORNMENT) ||
        !valid_bytes(command->current_directory, command->current_directory_bytes, MAXIMUM_VDM_CURRENT_DIR)) return 0;
    if (!copy_bytes(record->command, command->command, command->command_bytes) ||
        !copy_bytes(record->application, command->application, command->application_bytes) ||
        !copy_bytes(record->environment, command->environment, command->environment_bytes) ||
        !copy_bytes(record->current_directory, command->current_directory, command->current_directory_bytes)) return 0;
    record->task = command->task;
    record->creation_flags = command->creation_flags;
    record->error_code = command->error_code;
    record->code_page = command->code_page;
    record->current_drive = command->current_drive;
    record->coming_from_bat = command->coming_from_bat;
    record->command_bytes = command->command_bytes;
    record->application_bytes = command->application_bytes;
    record->environment_bytes = command->environment_bytes;
    record->current_directory_bytes = command->current_directory_bytes;
    record->available = 1u;
    return 1;
}

static void clear_sizes(PVDMINFO information)
{
    information->CmdSize = information->AppLen = information->PifLen = 0u;
    information->EnviornmentSize = information->CurDirectoryLen = 0u;
    information->DesktopLen = information->TitleLen = information->ReservedLen = 0u;
}

static void required_sizes(const base_vdm_local *record, PVDMINFO information)
{
    information->CmdSize = record->command_bytes;
    information->AppLen = record->application_bytes;
    information->PifLen = 0u;
    information->EnviornmentSize = record->environment_bytes;
    information->CurDirectoryLen = record->current_directory_bytes;
    information->DesktopLen = information->TitleLen = information->ReservedLen = 0u;
}

static int has_buffer(const void *destination, uint32_t capacity, uint32_t required)
{
    return required == 0u || (destination != NULL && capacity >= required);
}

/* DIVERGENCE: source-derived DOS-only BaseSrvGetNextVDMCommand slice. The
 * original body requires CSRSS console/DOS records and duplicated handles.
 * This retains copy, capacity, environment and status order; WOW/PIF/child
 * and global server branches remain unavailable. */
static NTSTATUS get_next_dos(base_vdm_local *record, PVDMINFO information)
{
    uint16_t state = information->VDMState;
    if (state & (ASKING_FOR_WOW_BINARY | ASKING_FOR_SEPWOW_BINARY | ASKING_FOR_PIF))
        return STATUS_NOT_IMPLEMENTED;
    if (record->available == 0u) {
        clear_sizes(information);
        return ((state & RETURN_ON_NO_COMMAND) && (state & ASKING_FOR_SECOND_TIME))
            ? STATUS_NO_MEMORY : STATUS_NOT_IMPLEMENTED;
    }
    if (state & ASKING_FOR_ENVIRONMENT) {
        if (!has_buffer(information->Enviornment, information->EnviornmentSize,
                record->environment_bytes)) {
            clear_sizes(information);
            information->EnviornmentSize = record->environment_bytes;
            return STATUS_INVALID_PARAMETER;
        }
        (void)copy_bytes((uint8_t *)information->Enviornment, record->environment,
            record->environment_bytes);
        clear_sizes(information);
        information->EnviornmentSize = record->environment_bytes;
        return STATUS_SUCCESS;
    }
    if (!has_buffer(information->CmdLine, information->CmdSize, record->command_bytes) ||
        !has_buffer(information->AppName, information->AppLen, record->application_bytes) ||
        !has_buffer(information->Enviornment, information->EnviornmentSize, record->environment_bytes) ||
        !has_buffer(information->CurDirectory, information->CurDirectoryLen, record->current_directory_bytes)) {
        required_sizes(record, information);
        return STATUS_INVALID_PARAMETER;
    }
    (void)copy_bytes((uint8_t *)information->CmdLine, record->command, record->command_bytes);
    (void)copy_bytes((uint8_t *)information->AppName, record->application, record->application_bytes);
    (void)copy_bytes((uint8_t *)information->Enviornment, record->environment, record->environment_bytes);
    (void)copy_bytes((uint8_t *)information->CurDirectory, record->current_directory, record->current_directory_bytes);
    required_sizes(record, information);
    information->VDMState = 0u;
    information->CurDrive = record->current_drive;
    information->StdIn = information->StdOut = information->StdErr = NULL;
    information->iTask = record->task;
    information->CodePage = record->code_page;
    information->dwCreationFlags = record->creation_flags;
    information->ErrorCode = record->error_code;
    information->fComingFromBat = record->coming_from_bat;
    record->available = 0u;
    return STATUS_SUCCESS;
}

static void teardown(void *context)
{
    base_vdm_local *record = (base_vdm_local *)context;
    if (record == NULL) return;
    if (base_vdm_current == record) base_vdm_current = NULL;
    record->owner = NULL;
}

int base_vdm_local_bind(base_vdm_local *record, session *owner)
{
    if (!base_vdm_local_valid(record) || record->owner != NULL || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE || base_vdm_current != NULL) return 0;
    base_vdm_current = record;
    if (!session_register_teardown(owner, teardown, record)) { base_vdm_current = NULL; return 0; }
    record->owner = owner;
    return 1;
}

int base_vdm_local_unbind(base_vdm_local *record)
{
    if (!base_vdm_local_valid(record) || record->owner == NULL || base_vdm_current != record) return 0;
    teardown(record);
    return 1;
}

BOOL APIENTRY GetNextVDMCommand(PVDMINFO information)
{
    session *owner = session_thread_current();
    NTSTATUS status;
    if (information == NULL) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
    if (owner == NULL) { SetLastError(ERROR_NOT_READY); return FALSE; }
    if (base_vdm_current == NULL || base_vdm_current->owner != owner) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE;
    }
    if (information->VDMState == INCREMENT_REENTER_COUNT || information->VDMState == DECREMENT_REENTER_COUNT) {
        if (information->VDMState == INCREMENT_REENTER_COUNT && base_vdm_current->reentry_count != UINT32_MAX) {
            ++base_vdm_current->reentry_count; return TRUE;
        }
        if (information->VDMState == DECREMENT_REENTER_COUNT && base_vdm_current->reentry_count != 0u) {
            --base_vdm_current->reentry_count; return TRUE;
        }
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    status = get_next_dos(base_vdm_current, information);
    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError(status == STATUS_INVALID_PARAMETER ? ERROR_INVALID_PARAMETER :
        status == STATUS_NO_MEMORY ? ERROR_NOT_ENOUGH_MEMORY : ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
