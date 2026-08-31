#include "base_vdm_local.h"

#include "session/session.h"

#include <string.h>

static __declspec(thread) base_vdm_local *base_vdm_current;

static int base_vdm_local_thread_bind(void *context)
{
    base_vdm_local *record = (base_vdm_local *)context;
    session *owner = session_thread_current();
    if (record == NULL || !base_vdm_local_valid(record) ||
        record->owner != owner) return 0;
    if (base_vdm_current == record) return 1;
    if (base_vdm_current != NULL) return 0;
    base_vdm_current = record;
    return 1;
}

static void base_vdm_local_thread_unbind(void *context)
{
    base_vdm_local *record = (base_vdm_local *)context;
    if (base_vdm_current == record) base_vdm_current = NULL;
}

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
    record->first_vdm_available = 1u;
    InitializeCriticalSection(&record->lock);
    record->lock_initialized = 1u;
    record->wake_event = CreateEvent(NULL, TRUE, FALSE, NULL);
}

int base_vdm_local_valid(const base_vdm_local *record)
{
    return record != NULL && record->version == BASE_VDM_LOCAL_VERSION &&
        record->struct_bytes == sizeof(*record) && record->reserved0 == 0u &&
        record->command_bytes <= MAXIMUM_VDM_COMMAND_LENGTH &&
        record->application_bytes <= MAX_PATH &&
        record->environment_bytes <= MAXIMUM_VDM_ENVIORNMENT &&
        record->current_directory_bytes <= MAXIMUM_VDM_PATH_STRING &&
        record->lock_initialized == 1u && record->wake_event != NULL &&
        record->pending_request <= 1u &&
        (record->current_directories_bytes == 0u ||
         record->current_directories != NULL);
}

int base_vdm_local_publish(base_vdm_local *record, const base_vdm_command *command)
{
    int published = 0;
    if (!base_vdm_local_valid(record) || command == NULL ||
        command->struct_bytes != sizeof(*command) ||
        command->command_bytes == 0u ||
        !valid_bytes(command->command, command->command_bytes, MAXIMUM_VDM_COMMAND_LENGTH) ||
        !valid_bytes(command->application, command->application_bytes, MAX_PATH) ||
        !valid_bytes(command->environment, command->environment_bytes, MAXIMUM_VDM_ENVIORNMENT) ||
        /* CurDirectory is a host path carried to the original BaseClient
         * caller.  Its ABI buffer is MAXIMUM_VDM_PATH_STRING, not the
         * guest-visible MAXIMUM_VDM_CURRENT_DIR DOS directory limit. */
        !valid_bytes(command->current_directory, command->current_directory_bytes,
            MAXIMUM_VDM_PATH_STRING)) return 0;
    EnterCriticalSection(&record->lock);
    if (record->available != 0u) goto done;
    if (!copy_bytes(record->command, command->command, command->command_bytes) ||
        !copy_bytes(record->application, command->application, command->application_bytes) ||
        !copy_bytes(record->environment, command->environment, command->environment_bytes) ||
        !copy_bytes(record->current_directory, command->current_directory, command->current_directory_bytes)) goto done;
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
    if (!SetEvent(record->wake_event)) {
        record->available = 0u;
        goto done;
    }
    published = 1;
done:
    LeaveCriticalSection(&record->lock);
    return published;
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
    EnterCriticalSection(&record->lock);
    if (record->available == 0u) {
        clear_sizes(information);
        if ((state & RETURN_ON_NO_COMMAND) && (state & ASKING_FOR_SECOND_TIME)) {
            LeaveCriticalSection(&record->lock);
            return STATUS_NO_MEMORY;
        }
        record->pending_request = 1u;
        ResetEvent(record->wake_event);
        LeaveCriticalSection(&record->lock);
        return STATUS_PENDING;
    }
    if (state & ASKING_FOR_ENVIRONMENT) {
        if (!has_buffer(information->Enviornment, information->EnviornmentSize,
                record->environment_bytes)) {
            clear_sizes(information);
            information->EnviornmentSize = record->environment_bytes;
            LeaveCriticalSection(&record->lock);
            return STATUS_INVALID_PARAMETER;
        }
        (void)copy_bytes((uint8_t *)information->Enviornment, record->environment,
            record->environment_bytes);
        clear_sizes(information);
        information->EnviornmentSize = record->environment_bytes;
        LeaveCriticalSection(&record->lock);
        return STATUS_SUCCESS;
    }
    /* BaseSrv's normal command response makes CmdLine mandatory, but each
     * other capture buffer is optional.  In particular, original
     * cmdGetNextCmd obtains its first DOS environment from guest state and
     * calls with ASKING_FOR_FIRST_COMMAND plus Env == NULL/EnvLen == 0.
     * Do not turn that original client/server contract into a local
     * requirement for a host environment buffer. */
    if (!has_buffer(information->CmdLine, information->CmdSize, record->command_bytes) ||
        (information->AppName != NULL && !has_buffer(information->AppName,
            information->AppLen, record->application_bytes)) ||
        (information->Enviornment != NULL && !has_buffer(information->Enviornment,
            information->EnviornmentSize, record->environment_bytes)) ||
        (information->CurDirectory != NULL && !has_buffer(information->CurDirectory,
            information->CurDirectoryLen, record->current_directory_bytes))) {
        required_sizes(record, information);
        LeaveCriticalSection(&record->lock);
        return STATUS_INVALID_PARAMETER;
    }
    (void)copy_bytes((uint8_t *)information->CmdLine, record->command, record->command_bytes);
    if (information->AppName != NULL)
        (void)copy_bytes((uint8_t *)information->AppName, record->application,
            record->application_bytes);
    if (information->Enviornment != NULL)
        (void)copy_bytes((uint8_t *)information->Enviornment, record->environment,
            record->environment_bytes);
    if (information->CurDirectory != NULL)
        (void)copy_bytes((uint8_t *)information->CurDirectory,
            record->current_directory, record->current_directory_bytes);
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
    record->pending_request = 0u;
    ResetEvent(record->wake_event);
    LeaveCriticalSection(&record->lock);
    return STATUS_SUCCESS;
}

static void teardown(void *context)
{
    base_vdm_local *record = (base_vdm_local *)context;
    if (record == NULL) return;
    if (record->owner != NULL) (void)session_unregister_thread_hook(
        record->owner, base_vdm_local_thread_bind,
        base_vdm_local_thread_unbind, record);
    if (base_vdm_current == record) base_vdm_current = NULL;
    if (record->lock_initialized != 0u) EnterCriticalSection(&record->lock);
    if (record->current_directories != NULL) {
        HeapFree(GetProcessHeap(), 0u, record->current_directories);
        record->current_directories = NULL;
        record->current_directories_bytes = 0u;
    }
    if (record->wake_event != NULL) {
        SetEvent(record->wake_event);
        CloseHandle(record->wake_event);
        record->wake_event = NULL;
    }
    record->pending_request = 0u;
    record->owner = NULL;
    if (record->lock_initialized != 0u) {
        LeaveCriticalSection(&record->lock);
        DeleteCriticalSection(&record->lock);
        record->lock_initialized = 0u;
    }
}

int base_vdm_local_bind(base_vdm_local *record, session *owner)
{
    if (!base_vdm_local_valid(record) || record->owner != NULL || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE || base_vdm_current != NULL) return 0;
    record->owner = owner;
    if (!session_register_thread_hook(owner, base_vdm_local_thread_bind,
            base_vdm_local_thread_unbind, record) ||
        !session_register_teardown(owner, teardown, record)) {
        (void)session_unregister_thread_hook(owner, base_vdm_local_thread_bind,
            base_vdm_local_thread_unbind, record);
        record->owner = NULL;
        return 0;
    }
    base_vdm_current = record;
    return 1;
}

int base_vdm_local_unbind(base_vdm_local *record)
{
    if (!base_vdm_local_valid(record) || record->owner == NULL || base_vdm_current != record) return 0;
    teardown(record);
    return 1;
}

BOOL base_vdm_local_dispatch(PVDMINFO information)
{
    session *owner = session_thread_current();
    NTSTATUS status;
    if (information == NULL) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
    if (owner == NULL || !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE) {
        SetLastError(ERROR_NOT_READY); return FALSE;
    }
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
        status == STATUS_NO_MEMORY ? ERROR_NOT_ENOUGH_MEMORY :
        status == STATUS_PENDING ? ERROR_IO_PENDING : ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

/* The original BaseClient, not BaseSrv, waits on the server-returned DOS
 * wait object and retries with ASKING_FOR_SECOND_TIME.  The local record
 * retains that split: dispatch records pending state; this synchronous helper
 * waits without retaining the caller's VDMINFO pointer and updates the local
 * capture only after the producer signals it. */
int base_vdm_local_wait_for_command(PVDMINFO information)
{
    session *owner = session_thread_current();
    base_vdm_local *record = base_vdm_current;
    DWORD wait_status;
    if (information == NULL || owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE || record == NULL ||
        !base_vdm_local_valid(record) || record->owner != owner)
        return 0;
    wait_status = WaitForSingleObject(record->wake_event, INFINITE);
    if (wait_status != WAIT_OBJECT_0) {
        SetLastError(ERROR_GEN_FAILURE);
        return 0;
    }
    information->VDMState = (USHORT)(information->VDMState |
        ASKING_FOR_SECOND_TIME);
    return 1;
}

/* DIVERGENCE: BaseSrvIsFirstVDM originally owns a CSRSS-global flag. The
 * admitted product has one local session, so the identical query-and-clear
 * contract is kept in its bound Base VDM record. */
BOOL base_vdm_local_is_first(void)
{
    session *owner = session_thread_current();
    if (owner == NULL || !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        base_vdm_current == NULL || base_vdm_current->owner != owner) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    if (base_vdm_current->first_vdm_available == 0u) return FALSE;
    base_vdm_current->first_vdm_available = 0u;
    return TRUE;
}

/* DIVERGENCE: BaseSrvSetVDMCurDirs stores a captured MULTI_SZ on a CSRSS
 * console record. Keep that copied allocation in the bound one-session record
 * and retain the client API's zero-length no-op behavior. */
BOOL base_vdm_local_set_current_directories(ULONG byte_count,
    const CHAR *directories)
{
    session *owner = session_thread_current();
    uint8_t *copy;
    if (byte_count == 0u || directories == NULL) return TRUE;
    if (owner == NULL || !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        base_vdm_current == NULL || base_vdm_current->owner != owner) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    copy = (uint8_t *)HeapAlloc(GetProcessHeap(), 0u, byte_count);
    if (copy == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    memcpy(copy, directories, byte_count);
    if (base_vdm_current->current_directories != NULL)
        HeapFree(GetProcessHeap(), 0u, base_vdm_current->current_directories);
    base_vdm_current->current_directories = copy;
    base_vdm_current->current_directories_bytes = byte_count;
    return TRUE;
}

/* DIVERGENCE: BaseSrvGetVDMCurDirs returns the stored MULTI_SZ once and then
 * frees its console-record copy to avoid leaking a DOS directory into WOW.
 * The local record preserves that one-shot ownership rule. */
ULONG base_vdm_local_get_current_directories(ULONG byte_count,
    CHAR *directories)
{
    session *owner = session_thread_current();
    ULONG required;
    if (owner == NULL || !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        base_vdm_current == NULL || base_vdm_current->owner != owner) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0u;
    }
    required = base_vdm_current->current_directories_bytes;
    if (required == 0u) return 0u;
    if (directories == NULL || byte_count < required) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return required;
    }
    memcpy(directories, base_vdm_current->current_directories, required);
    HeapFree(GetProcessHeap(), 0u, base_vdm_current->current_directories);
    base_vdm_current->current_directories = NULL;
    base_vdm_current->current_directories_bytes = 0u;
    return required;
}

/* DIVERGENCE: BaseSrvExitDOSTask eventually tears down an NT4 console/CSR
 * record. A DOS caller in the one-session product completes only its own
 * session; it never exits the host process. WOW exits remain a later owner. */
VOID base_vdm_local_exit(BOOL wow_caller, ULONG wow_task)
{
    session *owner = session_thread_current();
    (void)wow_task;
    if (wow_caller || owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE || base_vdm_current == NULL ||
        base_vdm_current->owner != owner) return;
    session_complete(owner, 0u);
}
