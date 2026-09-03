#include "base_vdm_local.h"

#include "session/session.h"

#include <stdio.h>
#include <string.h>

static __declspec(thread) base_vdm_local *base_vdm_current;

/* Default-off fixed-container witness for the source-shaped BaseVDM request
 * boundary.  It deliberately records only scalar queue/request state: no
 * VDMINFO buffer, guest address, host handle, or pointer crosses this
 * diagnostic boundary. */
static void base_vdm_local_record_request(const base_vdm_local *record,
    USHORT state, NTSTATUS status)
{
    char path[MAX_PATH];
    char line[192];
    DWORD path_bytes;
    HANDLE file;
    DWORD written;
    int formatted;

    path_bytes = GetEnvironmentVariableA("MVDM_BASE_VDM_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (path_bytes == 0u || path_bytes >= sizeof(path) || record == NULL)
        return;
    formatted = snprintf(line, sizeof(line),
        "MVDM-BASEVDM state=%04X available=%lu owner=%lu dos-state=%lu pending=%lu status=%08lX\\r\\n",
        (unsigned int)state, (unsigned long)record->available,
        (unsigned long)record->command_owner,
        (unsigned long)record->dos_record_state,
        (unsigned long)record->pending_request, (unsigned long)status);
    if (formatted <= 0 || (size_t)formatted >= sizeof(line)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    (void)WriteFile(file, line, (DWORD)formatted, &written, NULL);
    CloseHandle(file);
}

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
        record->reserved1 == 0u &&
        (record->command_owner == BASE_VDM_COMMAND_DOS ||
         record->command_owner == BASE_VDM_COMMAND_WOW) &&
        record->command_bytes <= MAXIMUM_VDM_COMMAND_LENGTH &&
        record->application_bytes <= MAX_PATH &&
        record->environment_bytes <= MAXIMUM_VDM_ENVIORNMENT &&
        record->current_directory_bytes <= MAX_PATH + 1u &&
        record->lock_initialized == 1u && record->wake_event != NULL &&
        record->pending_request <= 1u &&
        record->native_child_launch_pending <= 1u &&
        record->dos_record_state <= BASE_VDM_DOS_RECORD_HAS_RETURNED_ERROR_CODE &&
        (record->current_directories_bytes == 0u ||
         record->current_directories != NULL);
}

int base_vdm_local_publish(base_vdm_local *record, const base_vdm_command *command)
{
    int published = 0;
    if (!base_vdm_local_valid(record) || command == NULL ||
        command->struct_bytes != sizeof(*command) || command->reserved0 != 0u ||
        command->reserved1 != 0u ||
        (command->command_owner != BASE_VDM_COMMAND_DOS &&
         command->command_owner != BASE_VDM_COMMAND_WOW) ||
        command->command_bytes == 0u ||
        !valid_bytes(command->command, command->command_bytes, MAXIMUM_VDM_COMMAND_LENGTH) ||
        !valid_bytes(command->application, command->application_bytes, MAX_PATH) ||
        !valid_bytes(command->environment, command->environment_bytes, MAXIMUM_VDM_ENVIORNMENT) ||
        /* CurDirectory is a host path carried to the original BaseClient
         * caller.  Original COMMAND advertises MAX_PATH + 1 bytes, not the
         * guest-visible MAXIMUM_VDM_CURRENT_DIR DOS directory limit. */
        !valid_bytes(command->current_directory, command->current_directory_bytes,
            MAX_PATH + 1u)) return 0;
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
    record->command_owner = command->command_owner;
    record->command_bytes = command->command_bytes;
    record->application_bytes = command->application_bytes;
    record->environment_bytes = command->environment_bytes;
    record->current_directory_bytes = command->current_directory_bytes;
    record->available = 1u;
    record->dos_record_state = command->command_owner == BASE_VDM_COMMAND_DOS ?
        BASE_VDM_DOS_RECORD_TO_TAKE_A_COMMAND : BASE_VDM_DOS_RECORD_EMPTY;
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

static int is_wow_request(USHORT state)
{
    return (state & ASKING_FOR_WOW_BINARY) != 0u;
}

/* DIVERGENCE(ADAPTER-BASESRV-010): source-shaped one-session counterpart of
 * BaseSrvFillPifInfo in base/win32/server/srvvdm.c.  The original server
 * serves this query from the DOS record without consuming that record.  This
 * product has no CSRSS console-record list, but its copied record retains the
 * same host-side PIF/title/current-directory fields.  There is no PIF payload
 * in the admitted launch declaration, so PifFile and Reserved are reported as
 * empty; Title follows the original AppName fallback. */
static NTSTATUS fill_pif_info(const base_vdm_local *record,
    PVDMINFO information)
{
    uint32_t title_bytes = record->application_bytes;
    uint32_t directory_bytes = record->current_directory_bytes;

    if (information->PifLen != 0u && information->PifFile == NULL)
        return STATUS_INVALID_PARAMETER;
    if (information->TitleLen != 0u && information->Title == NULL)
        return STATUS_INVALID_PARAMETER;
    if (information->CurDirectoryLen != 0u && information->CurDirectory == NULL)
        return STATUS_INVALID_PARAMETER;
    if (information->ReservedLen != 0u && information->Reserved == NULL)
        return STATUS_INVALID_PARAMETER;
    if ((information->TitleLen != 0u && title_bytes > information->TitleLen) ||
        (information->CurDirectoryLen != 0u &&
            directory_bytes > information->CurDirectoryLen))
        return STATUS_INVALID_PARAMETER;

    if (information->PifLen != 0u) ((CHAR *)information->PifFile)[0] = '\0';
    if (information->ReservedLen != 0u) ((CHAR *)information->Reserved)[0] = '\0';
    if (information->TitleLen != 0u) {
        if (title_bytes != 0u)
            (void)copy_bytes((uint8_t *)information->Title,
                record->application, title_bytes);
        else
            ((CHAR *)information->Title)[0] = '\0';
    }
    if (information->CurDirectoryLen != 0u) {
        if (directory_bytes != 0u)
            (void)copy_bytes((uint8_t *)information->CurDirectory,
                record->current_directory, directory_bytes);
        else
            ((CHAR *)information->CurDirectory)[0] = '\0';
    }

    information->PifLen = 0u;
    information->TitleLen = (USHORT)title_bytes;
    information->CurDirectoryLen = (USHORT)directory_bytes;
    information->ReservedLen = 0u;
    return STATUS_SUCCESS;
}

/* DIVERGENCE: source-derived single-session BaseSrvGetNextVDMCommand slice.
 * `srvvdm.c` distinguishes DOS console records from the separate WOW record:
 * an empty WOW queue returns successful zero lengths without blocking, while
 * DOS waits for its own record.  CSRSS queues/duplicated handles remain
 * unavailable, but this copied record preserves that source-visible choice,
 * capacity checks and copy order without accepting the other queue's item. */
static NTSTATUS get_next_command(base_vdm_local *record, PVDMINFO information)
{
    uint16_t state = information->VDMState;
    int wow_request = is_wow_request(state);
    if ((state & ASKING_FOR_SEPWOW_BINARY) && !(state & ASKING_FOR_PIF))
        return STATUS_NOT_IMPLEMENTED;
    EnterCriticalSection(&record->lock);
    if (state & ASKING_FOR_PIF) {
        NTSTATUS status;
        if (record->available == 0u ||
            (wow_request && record->command_owner != BASE_VDM_COMMAND_WOW) ||
            (!wow_request && record->command_owner != BASE_VDM_COMMAND_DOS)) {
            LeaveCriticalSection(&record->lock);
            return STATUS_INVALID_PARAMETER;
        }
        status = fill_pif_info(record, information);
        LeaveCriticalSection(&record->lock);
        return status;
    }
    if (record->available == 0u ||
        (wow_request && record->command_owner != BASE_VDM_COMMAND_WOW) ||
        (!wow_request && record->command_owner != BASE_VDM_COMMAND_DOS)) {
        clear_sizes(information);
        /* Original BaseSrv never blocks WOWEXEC when its WOW queue is
         * empty; it reports an empty successful response instead. */
        if (wow_request) {
            LeaveCriticalSection(&record->lock);
            return STATUS_SUCCESS;
        }
        if ((state & RETURN_ON_NO_COMMAND) &&
            (record->dos_record_state == BASE_VDM_DOS_RECORD_BUSY ||
             record->dos_record_state == BASE_VDM_DOS_RECORD_HAS_RETURNED_ERROR_CODE)) {
            if ((state & NO_PARENT_TO_WAKE) != 0u &&
                (state & ASKING_FOR_SECOND_TIME) == 0u) {
                /* `srvvdm.c` returns a wait object for the first
                 * NO_PARENT_TO_WAKE request.  BaseClient waits, then retries
                 * with ASKING_FOR_SECOND_TIME before it observes the empty
                 * RETURN_ON_NO_COMMAND result.  The one-session seam has no
                 * duplicated CSRSS handle, but preserves that observable
                 * request/retry order through its private events. */
                record->pending_request = 1u;
                ResetEvent(record->wake_event);
                LeaveCriticalSection(&record->lock);
                return STATUS_PENDING;
            }
            if ((state & ASKING_FOR_SECOND_TIME) != 0u) {
                /* On the retried request, `srvvdm.c` does not rewrite the
                 * busy DOS record.  It falls through to its no-record
                 * RETURN_ON_NO_COMMAND terminal below. */
                LeaveCriticalSection(&record->lock);
                return STATUS_NO_MEMORY;
            }
            /* DIVERGENCE: the original BaseSrv marks a VDM_BUSY child as
             * returned, wakes its external parent record, then its
             * BaseClient retry observes no next command.  This one-session
             * composition has no external parent or duplicated wait handle.
             * Preserve the same caller-visible terminal result directly:
             * no command buffers, failure return, and no retained request.
             * Do not turn this into a synthetic guest command or a generic
             * scheduler wake. */
            record->error_code = information->ErrorCode;
            record->dos_record_state =
                BASE_VDM_DOS_RECORD_HAS_RETURNED_ERROR_CODE;
            record->pending_request = 0u;
            ResetEvent(record->wake_event);
            LeaveCriticalSection(&record->lock);
            return STATUS_NO_MEMORY;
        }
        if ((state & RETURN_ON_NO_COMMAND) && (state & ASKING_FOR_SECOND_TIME)) {
            LeaveCriticalSection(&record->lock);
            return STATUS_NO_MEMORY;
        }
        /* DIVERGENCE(ADAPTER-BASESRV-007): BaseSrv normally leaves an
         * ordinary DOS VDM waiting for another console command.  The app's
         * explicitly declared one-shot CLI command has no such producer.
         * Reach this only after the copied DOS record became BUSY and original
         * COMMAND made its next BaseVDM request; end the bound session at that
         * source-visible command boundary instead of inventing a guest input
         * record or changing COMMAND. */
        if (!wow_request && record->terminal_on_command_exhaustion != 0u &&
            record->dos_record_state == BASE_VDM_DOS_RECORD_BUSY) {
            LeaveCriticalSection(&record->lock);
            if (session_terminate_current(0u)) return STATUS_NO_MEMORY;
            SetLastError(ERROR_INVALID_STATE);
            return STATUS_INVALID_PARAMETER;
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
    if (!wow_request)
        record->dos_record_state = BASE_VDM_DOS_RECORD_BUSY;
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
    record->native_child_launch_pending = 0u;
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
        EnterCriticalSection(&base_vdm_current->lock);
        if (information->VDMState == INCREMENT_REENTER_COUNT && base_vdm_current->reentry_count != UINT32_MAX) {
            base_vdm_current->native_child_launch_pending = 0u;
            ++base_vdm_current->reentry_count;
            LeaveCriticalSection(&base_vdm_current->lock);
            return TRUE;
        }
        if (information->VDMState == DECREMENT_REENTER_COUNT && base_vdm_current->reentry_count != 0u) {
            --base_vdm_current->reentry_count;
            if (base_vdm_current->reentry_count == 0u &&
                base_vdm_current->native_child_launch_pending == 0u)
                SetEvent(base_vdm_current->wake_event);
            LeaveCriticalSection(&base_vdm_current->lock);
            return TRUE;
        }
        LeaveCriticalSection(&base_vdm_current->lock);
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    status = get_next_command(base_vdm_current, information);
    base_vdm_local_record_request(base_vdm_current, information->VDMState,
        status);
    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError(status == STATUS_INVALID_PARAMETER ? ERROR_INVALID_PARAMETER :
        status == STATUS_NO_MEMORY ? ERROR_NOT_ENOUGH_MEMORY :
        status == STATUS_PENDING ? ERROR_IO_PENDING : ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

int base_vdm_local_set_terminal_on_command_exhaustion(base_vdm_local *record,
    int enabled)
{
    if (!base_vdm_local_valid(record) || record->owner == NULL ||
        record->available != 0u || record->dos_record_state !=
            BASE_VDM_DOS_RECORD_EMPTY)
        return 0;
    record->terminal_on_command_exhaustion = enabled != 0 ? 1u : 0u;
    return 1;
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

int base_vdm_local_native_child_begin(void)
{
    session *owner = session_thread_current();
    base_vdm_local *record = base_vdm_current;

    if (owner == NULL || !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        record == NULL || !base_vdm_local_valid(record) || record->owner != owner)
        return 0;
    EnterCriticalSection(&record->lock);
    if (record->native_child_launch_pending != 0u || record->reentry_count != 0u) {
        LeaveCriticalSection(&record->lock);
        return 0;
    }
    record->native_child_launch_pending = 1u;
    ResetEvent(record->wake_event);
    LeaveCriticalSection(&record->lock);
    return 1;
}

void base_vdm_local_native_child_cancel(void)
{
    session *owner = session_thread_current();
    base_vdm_local *record = base_vdm_current;

    if (owner == NULL || record == NULL || !base_vdm_local_valid(record) ||
        record->owner != owner) return;
    EnterCriticalSection(&record->lock);
    record->native_child_launch_pending = 0u;
    if (record->reentry_count == 0u) SetEvent(record->wake_event);
    LeaveCriticalSection(&record->lock);
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

uint32_t base_vdm_local_observe_dos_record_state(void)
{
    session *owner = session_thread_current();
    base_vdm_local *record = base_vdm_current;
    uint32_t state;

    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE || record == NULL ||
        record->owner != owner || record->lock_initialized != 1u)
        return UINT32_MAX;
    EnterCriticalSection(&record->lock);
    state = record->dos_record_state;
    LeaveCriticalSection(&record->lock);
    return state;
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
