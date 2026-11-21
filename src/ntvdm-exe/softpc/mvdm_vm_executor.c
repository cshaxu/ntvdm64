#include "mvdm_vm_executor.h"

typedef struct command_job {
    mvdm_vm_input_job input_job;
    mvdm_vm_executor *owner;
    mvdm_vm_continuation *continuation;
    ntvdm_command_launch *launch;
    ntvdm_vm_text *text;
    mvdm_vm_io *io;
} command_job;

struct mvdm_vm_executor {
    mvdm_vm_rundown rundown;
    ntvdm_vm_host *host;
    uint64_t payload;
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
    InitializeSRWLock(&executor->rundown.lock);
    InitializeConditionVariable(&executor->rundown.changed);
    executor->host = host;
    error = ntvdm_vm_payload_begin(host, &executor->payload);
    if (error) { HeapFree(GetProcessHeap(), 0, executor); return error; }
    *output = executor;
    return ERROR_SUCCESS;
}

static VOID CALLBACK execute(PTP_CALLBACK_INSTANCE instance, void *context)
{
    command_job *job = context;
    mvdm_vm_input_job **p;
    mvdm_vm_executor *owner = job->owner;
    common_frontend_location location = {0};
    mvdm_vm_completion completion = {0};
    DWORD error, status, exit_code = 0;
    (void)instance;
    /* VMM may reuse a numeric VM handle before its old native chain finishes
     * retirement. Serialize only that handle's frontend incarnation off CPU;
     * independent VMs do not wait for one another. Tokens order admissions. */
    AcquireSRWLockExclusive(&owner->rundown.lock);
    for (;;) {
        mvdm_vm_input_job *older;
        for (older = owner->rundown.jobs; older; older = older->next)
            if (older->vm == job->input_job.vm && older->token < job->input_job.token) break;
        if (!older) break;
        if (!SleepConditionVariableSRW(&owner->rundown.changed, &owner->rundown.lock, INFINITE, 0))
            ExitProcess(GetLastError());
    }
    ReleaseSRWLockExclusive(&owner->rundown.lock);
    status = WaitForSingleObject(mvdm_vm_continuation_invalidated(job->continuation), 0);
    completion.error = status == WAIT_TIMEOUT ? ERROR_SUCCESS :
        status == WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
    if (!completion.error)
        completion.error = ntvdm_vm_host_register(owner->host, owner->payload,
            job->input_job.vm, mvdm_vm_io_ready(job->io), &mvdm_vm_io_operations, job->io, &location);
    if (!completion.error)
        completion.error = mvdm_vm_command_execute(job->continuation, job->launch,
            owner->host, &location, &exit_code);
    completion.exit_code = exit_code;
    /* The command has ended, but retirement still owns live I/O callbacks.
     * Reject new producer events before that join; lifetime is not admission. */
    AcquireSRWLockExclusive(&owner->rundown.lock);
    job->input_job.finishing = TRUE;
    ReleaseSRWLockExclusive(&owner->rundown.lock);
    if (location.incarnation) {
        error = ntvdm_vm_host_retire(owner->host, &location);
        /* Failed retirement cannot free memory under unjoined I/O callbacks.
         * This is an unrecoverable local lifecycle failure, not success. */
        if (error && error != ERROR_NOT_FOUND) ExitProcess(error);
    }
    error = mvdm_vm_input_return(job->input_job.input, job->input_job.vm, job->input_job.generation);
    if (error && !completion.error)
        completion.error = error == ERROR_NOT_FOUND ? ERROR_OPERATION_ABORTED : error;
    /* Remove input access before destroying its provider. Close waits for the
     * job until ALL resources and the completion producer are finished. */
    AcquireSRWLockExclusive(&owner->rundown.lock);
    for (p = &owner->rundown.jobs; *p != &job->input_job; p = &(*p)->next) {}
    *p = job->input_job.next;
    mvdm_vm_io_destroy(job->io);
    job->io = NULL;
    ReleaseSRWLockExclusive(&owner->rundown.lock);
    ntvdm_command_launch_destroy(job->launch);
    ntvdm_vm_text_destroy(job->text);
    error = mvdm_vm_continuation_complete(job->continuation, &completion);
    if (error && error != ERROR_OPERATION_ABORTED) ExitProcess(error);
    mvdm_vm_continuation_release(job->continuation);
    AcquireSRWLockExclusive(&owner->rundown.lock);
    --owner->rundown.active;
    HeapFree(GetProcessHeap(), 0, job);
    WakeAllConditionVariable(&owner->rundown.changed);
    ReleaseSRWLockExclusive(&owner->rundown.lock);
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
    job->owner = owner; job->input_job.vm = vm; job->launch = launch; job->text = text;
    /* Never hold the executor lock across ICA operations. CPU admission may
     * already be inside ICA, so close/input must not invert that lock order. */
    error = mvdm_vm_continuation_begin(vm, &job->input_job.token, &job->continuation);
    if (!error) job->input_job.generation = mvdm_vm_continuation_generation(job->continuation);
    if (!error)
        error = mvdm_vm_io_create(text, job->continuation, "VM command", &job->io);
    if (!error) job->input_job.input = mvdm_vm_io_input(job->io);
    if (!error) {
        AcquireSRWLockExclusive(&owner->rundown.lock);
        if (owner->rundown.closing) error = ERROR_OPERATION_ABORTED;
        else if (owner->rundown.active == MAXDWORD) error = ERROR_ARITHMETIC_OVERFLOW;
        else {
            mvdm_vm_input_job *other;
            for (other = owner->rundown.jobs; other; other = other->next)
                if (other->vm == vm && other->generation == job->input_job.generation) break;
            if (other) error = ERROR_BUSY;
            else {
                job->input_job.next = owner->rundown.jobs;
                owner->rundown.jobs = &job->input_job;
                if (!TrySubmitThreadpoolCallback(execute, job, NULL)) {
                    error = GetLastError();
                    owner->rundown.jobs = job->input_job.next;
                } else { ++owner->rundown.active; *token = job->input_job.token; }
            }
        }
        ReleaseSRWLockExclusive(&owner->rundown.lock);
    }
    if (error) {
        mvdm_vm_io_destroy(job->io);
        mvdm_vm_continuation_abandon(job->continuation);
        mvdm_vm_continuation_release(job->continuation);
        HeapFree(GetProcessHeap(), 0, job);
    }
    return error;
}

mvdm_vm_rundown *mvdm_vm_executor_rundown(mvdm_vm_executor *owner)
{
    return owner ? &owner->rundown : NULL;
}

void mvdm_vm_executor_close(mvdm_vm_executor *owner)
{
    if (owner) {
        DWORD error = mvdm_vm_rundown_close(&owner->rundown, TRUE);
        if (error) ExitProcess(error);
    }
}

void mvdm_vm_executor_destroy(mvdm_vm_executor *owner)
{
    if (!owner) return;
    mvdm_vm_executor_close(owner);
    /* The worker closes the host next; it owns the payload registry. */
    HeapFree(GetProcessHeap(), 0, owner);
}
