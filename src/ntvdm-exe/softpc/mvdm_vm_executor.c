#include "mvdm_vm_executor.h"
#include "mvdm_softpc_guest_memory.h"

typedef struct command_job {
    struct command_job *next;
    mvdm_vm_executor *owner;
    mvdm_vm_continuation *continuation;
    ntvdm_command_launch *launch;
    ntvdm_vm_text *text;
    mvdm_vm_io *io;
    uint64_t token;
    uint64_t generation;
    uint32_t vm;
    BOOL finishing;
} command_job;

struct mvdm_vm_executor {
    SRWLOCK lock;
    CONDITION_VARIABLE changed;
    ntvdm_vm_host *host;
    uint64_t payload;
    command_job *jobs;
    ULONG active;
    BOOL closing;
};

DWORD mvdm_vm_executor_create(ntvdm_vm_host *host, mvdm_vm_executor **output)
{
    mvdm_vm_executor *executor;
    DWORD error;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output = NULL;
    if (!host) return ERROR_INVALID_PARAMETER;
    executor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*executor));
    if (!executor) return ERROR_NOT_ENOUGH_MEMORY;
    InitializeSRWLock(&executor->lock);
    InitializeConditionVariable(&executor->changed);
    executor->host = host;
    error = ntvdm_vm_payload_begin(host, &executor->payload);
    if (error) { HeapFree(GetProcessHeap(), 0, executor); return error; }
    *output = executor;
    return ERROR_SUCCESS;
}

static VOID CALLBACK execute(PTP_CALLBACK_INSTANCE instance, void *context)
{
    command_job *job = context, **p;
    mvdm_vm_executor *owner = job->owner;
    common_frontend_location location = {0};
    mvdm_vm_completion completion = {0};
    DWORD error, status, exit_code = 0;
    (void)instance;
    /* VMM may reuse a numeric VM handle before its old native chain finishes
     * retirement. Serialize only that handle's frontend incarnation off CPU;
     * independent VMs do not wait for one another. Tokens order admissions. */
    AcquireSRWLockExclusive(&owner->lock);
    for (;;) {
        command_job *older;
        for (older = owner->jobs; older; older = older->next)
            if (older->vm == job->vm && older->token < job->token) break;
        if (!older) break;
        if (!SleepConditionVariableSRW(&owner->changed, &owner->lock, INFINITE, 0))
            ExitProcess(GetLastError());
    }
    ReleaseSRWLockExclusive(&owner->lock);
    status = WaitForSingleObject(mvdm_vm_continuation_invalidated(job->continuation), 0);
    completion.error = status == WAIT_TIMEOUT ? ERROR_SUCCESS :
        status == WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
    if (!completion.error)
        completion.error = ntvdm_vm_host_register(owner->host, owner->payload,
            job->vm, mvdm_vm_io_ready(job->io), &mvdm_vm_io_operations, job->io, &location);
    if (!completion.error)
        completion.error = mvdm_vm_command_execute(job->continuation, job->launch,
            owner->host, &location, &exit_code);
    completion.exit_code = exit_code;
    /* The command has ended, but retirement still owns live I/O callbacks.
     * Reject new producer events before that join; lifetime is not admission. */
    AcquireSRWLockExclusive(&owner->lock);
    job->finishing = TRUE;
    ReleaseSRWLockExclusive(&owner->lock);
    if (location.incarnation) {
        error = ntvdm_vm_host_retire(owner->host, &location);
        /* Failed retirement cannot free memory under unjoined I/O callbacks.
         * This is an unrecoverable local lifecycle failure, not success. */
        if (error && error != ERROR_NOT_FOUND) ExitProcess(error);
    }
    /* Remove input access before destroying its provider. Close waits for the
     * job until ALL resources and the completion producer are finished. */
    AcquireSRWLockExclusive(&owner->lock);
    for (p = &owner->jobs; *p != job; p = &(*p)->next) {}
    *p = job->next;
    mvdm_vm_io_destroy(job->io);
    job->io = NULL;
    ReleaseSRWLockExclusive(&owner->lock);
    ntvdm_command_launch_destroy(job->launch);
    ntvdm_vm_text_destroy(job->text);
    error = mvdm_vm_continuation_complete(job->continuation, &completion);
    if (error && error != ERROR_OPERATION_ABORTED) ExitProcess(error);
    mvdm_vm_continuation_release(job->continuation);
    AcquireSRWLockExclusive(&owner->lock);
    --owner->active;
    HeapFree(GetProcessHeap(), 0, job);
    WakeAllConditionVariable(&owner->changed);
    ReleaseSRWLockExclusive(&owner->lock);
}

DWORD mvdm_vm_executor_submit(mvdm_vm_executor *owner, uint32_t vm,
    ntvdm_command_launch *launch, ntvdm_vm_text *text, uint64_t *token)
{
    command_job *job;
    DWORD error;
    if (!token) return ERROR_INVALID_PARAMETER;
    *token = 0;
    if (!owner || !launch || !text) return ERROR_INVALID_PARAMETER;
    job = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*job));
    if (!job) return ERROR_NOT_ENOUGH_MEMORY;
    job->owner = owner; job->vm = vm; job->launch = launch; job->text = text;
    /* Never hold the executor lock across ICA operations. CPU admission may
     * already be inside ICA, so close/input must not invert that lock order. */
    error = mvdm_vm_continuation_begin(vm, &job->token, &job->continuation);
    if (!error) job->generation = mvdm_vm_continuation_generation(job->continuation);
    if (!error)
        error = mvdm_vm_io_create(text, job->continuation, "VM command", &job->io);
    if (!error) {
        AcquireSRWLockExclusive(&owner->lock);
        if (owner->closing) error = ERROR_OPERATION_ABORTED;
        else if (owner->active == MAXDWORD) error = ERROR_ARITHMETIC_OVERFLOW;
        else {
            command_job *other;
            for (other = owner->jobs; other; other = other->next)
                if (other->vm == vm && other->generation == job->generation) break;
            if (other) error = ERROR_BUSY;
            else {
                job->next = owner->jobs;
                owner->jobs = job;
                if (!TrySubmitThreadpoolCallback(execute, job, NULL)) {
                    error = GetLastError();
                    owner->jobs = job->next;
                } else { ++owner->active; *token = job->token; }
            }
        }
        ReleaseSRWLockExclusive(&owner->lock);
    }
    if (error) {
        mvdm_vm_io_destroy(job->io);
        mvdm_vm_continuation_abandon(job->continuation);
        mvdm_vm_continuation_release(job->continuation);
        HeapFree(GetProcessHeap(), 0, job);
    }
    return error;
}

DWORD mvdm_vm_executor_key(mvdm_vm_executor *owner, uint32_t vm, uint64_t token,
    const KEY_EVENT_RECORD *key)
{
    command_job *job;
    DWORD error = ERROR_NOT_FOUND;
    if (!owner || !vm || !key) return ERROR_INVALID_PARAMETER;
    AcquireSRWLockShared(&owner->lock);
    for (job = owner->jobs; job && job->token != token; job = job->next) {}
    if (owner->closing) error = ERROR_OPERATION_ABORTED;
    else if (job && job->vm == vm && job->io)
        error = job->finishing ? ERROR_OPERATION_ABORTED : mvdm_vm_io_key(job->io, key);
    ReleaseSRWLockShared(&owner->lock);
    return error;
}

DWORD mvdm_vm_executor_key_bop(mvdm_vm_executor *owner, uint32_t vm,
    uint32_t address, uint32_t extent)
{
    uint32_t descriptor[8];
    KEY_EVENT_RECORD key = {0};
    uint64_t token;
    if (!owner) return ERROR_NOT_READY;
    if (extent != sizeof(descriptor)) return ERROR_INVALID_PARAMETER;
    if (!mvdm_softpc_guest_memory_copy_from(address, (uint8_t *)descriptor,
        sizeof(descriptor))) return ERROR_INVALID_ADDRESS;
    if (descriptor[2] > 1 || !descriptor[3] || descriptor[3] > UINT16_MAX ||
        descriptor[4] > UINT16_MAX || descriptor[5] > UINT16_MAX ||
        descriptor[6] > UINT16_MAX) return ERROR_INVALID_DATA;
    token = ((uint64_t)descriptor[1] << 32) | descriptor[0];
    key.bKeyDown = (BOOL)descriptor[2];
    key.wRepeatCount = (WORD)descriptor[3];
    key.wVirtualKeyCode = (WORD)descriptor[4];
    key.wVirtualScanCode = (WORD)descriptor[5];
    key.uChar.UnicodeChar = (WCHAR)descriptor[6];
    key.dwControlKeyState = descriptor[7];
    return mvdm_vm_executor_key(owner, vm, token, &key);
}

void mvdm_vm_executor_close(mvdm_vm_executor *owner)
{
    if (!owner) return;
    AcquireSRWLockExclusive(&owner->lock);
    owner->closing = TRUE;
    ReleaseSRWLockExclusive(&owner->lock);
    mvdm_vm_continuation_reset();
    AcquireSRWLockExclusive(&owner->lock);
    while (owner->active)
        if (!SleepConditionVariableSRW(&owner->changed, &owner->lock, INFINITE, 0))
            ExitProcess(GetLastError());
    ReleaseSRWLockExclusive(&owner->lock);
}

void mvdm_vm_executor_destroy(mvdm_vm_executor *owner)
{
    if (!owner) return;
    mvdm_vm_executor_close(owner);
    /* The worker closes the host next; it owns the payload registry. */
    HeapFree(GetProcessHeap(), 0, owner);
}
