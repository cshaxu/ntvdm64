#include "mvdm_vm_continuation.h"
#include "mvdm_vm_notification.h"
#include "common/protocol/console_io.h"
#include <string.h>

extern void host_ica_lock(void);
extern void host_ica_unlock(void);

typedef struct guest_application {
    uint64_t ticket;
    HANDLE ready;
    uint32_t size, error;
    BOOL acknowledged;
} guest_application;

struct mvdm_vm_continuation {
    struct mvdm_vm_continuation *next;
    uint64_t token;
    mvdm_vm_wake_identity identity;
    mvdm_vm_completion completion;
    unsigned references;
    BOOL completed, retired;
    HANDLE invalidated;
    guest_application *io;
};
static mvdm_vm_continuation *requests;
static uint64_t sequence;
static uint64_t io_sequence;

static void free_io(guest_application *io)
{
    if (!io) return;
    CloseHandle(io->ready);
    HeapFree(GetProcessHeap(), 0, io);
}

static mvdm_vm_continuation **find(uint64_t token)
{
    mvdm_vm_continuation **p;
    for (p = &requests; *p && (*p)->token != token; p = &(*p)->next) {}
    return p;
}
static void drop(mvdm_vm_continuation *request)
{
    if (!--request->references) {
        free_io(request->io);
        CloseHandle(request->invalidated);
        HeapFree(GetProcessHeap(), 0, request);
    }
}
static void detach(mvdm_vm_continuation **p)
{
    mvdm_vm_continuation *request = *p;
    *p = request->next;
    request->retired = TRUE;
    SetEvent(request->invalidated);
    if (request->io) SetEvent(request->io->ready);
    drop(request);
}

uint32_t mvdm_vm_continuation_begin(uint32_t vm, uint64_t *token,
    mvdm_vm_continuation **executor)
{
    mvdm_vm_continuation *request;
    uint64_t serial;
    DWORD error;
    if (executor) *executor = NULL;
    if (!token) return ERROR_INVALID_PARAMETER;
    *token = 0;
    if (!executor) return ERROR_INVALID_PARAMETER;
    request = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*request));
    if (!request) return ERROR_NOT_ENOUGH_MEMORY;
    request->invalidated = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!request->invalidated) {
        error = GetLastError();
        HeapFree(GetProcessHeap(), 0, request);
        return error;
    }
    host_ica_lock();
    error = mvdm_vm_notification_snapshot(&serial);
    if (!error) error = mvdm_vm_wake_lookup(vm, &request->identity);
    if (!error && sequence == UINT64_MAX) error = ERROR_ARITHMETIC_OVERFLOW;
    if (!error) {
        request->token = ++sequence;
        request->references = 2; /* registry and executor */
        request->next = requests;
        requests = request;
        *token = request->token;
        *executor = request;
    }
    host_ica_unlock();
    if (error) {
        CloseHandle(request->invalidated);
        HeapFree(GetProcessHeap(), 0, request);
    }
    return error;
}

HANDLE mvdm_vm_continuation_invalidated(mvdm_vm_continuation *request)
{
    return request ? request->invalidated : NULL;
}

uint64_t mvdm_vm_continuation_generation(const mvdm_vm_continuation *request)
{
    return request ? request->identity.generation : 0;
}

uint32_t mvdm_vm_continuation_complete(mvdm_vm_continuation *request,
    const mvdm_vm_completion *completion)
{
    DWORD error;
    uint64_t serial;
    if (!request || !completion || completion->reentry > 1)
        return ERROR_INVALID_PARAMETER;
    host_ica_lock();
    if (request->retired) error = ERROR_OPERATION_ABORTED;
    else if (request->completed) error = ERROR_INVALID_STATE;
    else if (request->io) error = ERROR_BUSY;
    else {
        request->completion = *completion;
        request->completed = TRUE;
        /* ICA also serializes VMM retirement and the existing wake queue.
         * Notification only sets a CPU flag; it cannot enter guest code here. */
        error = mvdm_vm_wake_publish(request->identity.vm,
            request->identity.generation, &serial);
    }
    host_ica_unlock();
    return error;
}

void mvdm_vm_continuation_release(mvdm_vm_continuation *request)
{
    if (!request) return;
    host_ica_lock();
    drop(request);
    host_ica_unlock();
}

void mvdm_vm_continuation_abandon(mvdm_vm_continuation *request)
{
    mvdm_vm_continuation **p;
    if (!request) return;
    host_ica_lock();
    p = find(request->token);
    if (*p == request) detach(p);
    host_ica_unlock();
}

uint32_t mvdm_vm_continuation_consume(uint64_t token, uint32_t vm,
    mvdm_vm_completion *completion)
{
    mvdm_vm_continuation **p;
    DWORD error = ERROR_NOT_FOUND;
    if (!completion) return ERROR_INVALID_PARAMETER;
    ZeroMemory(completion, sizeof(*completion));
    host_ica_lock();
    p = find(token);
    if (*p) {
        if ((*p)->identity.vm != vm) error = ERROR_ACCESS_DENIED;
        else if (!(*p)->completed) error = ERROR_IO_INCOMPLETE;
        else {
            *completion = (*p)->completion;
            detach(p);
            error = ERROR_SUCCESS;
        }
    }
    host_ica_unlock();
    return error;
}

void mvdm_vm_continuation_retire(uint32_t vm, uint64_t generation)
{
    mvdm_vm_continuation **p;
    host_ica_lock();
    for (p = &requests; *p;)
        if ((*p)->identity.vm == vm &&
            (*p)->identity.generation == generation) detach(p);
        else p = &(*p)->next;
    host_ica_unlock();
}

void mvdm_vm_continuation_reset(void)
{
    host_ica_lock();
    while (requests) detach(&requests);
    host_ica_unlock();
}

uint32_t mvdm_vm_continuation_post_io(mvdm_vm_continuation *request,
    const void *bytes, uint32_t size, uint64_t *ticket, HANDLE *ready)
{
    guest_application *io;
    DWORD error;
    uint64_t serial;
    if (ticket) *ticket = 0;
    if (ready) *ready = NULL;
    if (!request || !bytes || !size || size > CONSOLE_IO_MAX_DATA_BYTES ||
        !ticket || !ready) return ERROR_INVALID_PARAMETER;
    io = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*io) + size);
    if (!io) return ERROR_NOT_ENOUGH_MEMORY;
    io->ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!io->ready) {
        error = GetLastError();
        HeapFree(GetProcessHeap(), 0, io);
        return error;
    }
    io->size = size;
    memcpy(io + 1, bytes, size);
    host_ica_lock();
    if (request->retired) error = ERROR_OPERATION_ABORTED;
    else if (request->completed) error = ERROR_INVALID_STATE;
    else if (request->io) error = ERROR_BUSY;
    else if (io_sequence == UINT64_MAX) error = ERROR_ARITHMETIC_OVERFLOW;
    else {
        io->ticket = ++io_sequence;
        request->io = io;
        error = mvdm_vm_wake_publish(request->identity.vm,
            request->identity.generation, &serial);
        if (error) request->io = NULL;
        else { *ticket = io->ticket; *ready = io->ready; }
    }
    host_ica_unlock();
    if (error) free_io(io);
    return error;
}

uint32_t mvdm_vm_continuation_read_io(uint64_t token, uint32_t vm,
    uint64_t *ticket, void *bytes, uint32_t capacity, uint32_t *size)
{
    mvdm_vm_continuation *request;
    DWORD error = ERROR_NOT_FOUND;
    if (!ticket || !size || (capacity && !bytes)) return ERROR_INVALID_PARAMETER;
    *size = 0;
    host_ica_lock();
    request = *find(token);
    if (request) {
        guest_application *io = request->io;
        if (request->identity.vm != vm) error = ERROR_ACCESS_DENIED;
        else if (io && (!*ticket || *ticket == io->ticket)) {
            *ticket = io->ticket;
            *size = io->size;
            if (io->acknowledged) error = ERROR_INVALID_STATE;
            else if (capacity < io->size) error = ERROR_INSUFFICIENT_BUFFER;
            else { memcpy(bytes, io + 1, io->size); error = ERROR_SUCCESS; }
        }
    }
    host_ica_unlock();
    return error;
}

uint32_t mvdm_vm_continuation_ack_io(uint64_t token, uint32_t vm,
    uint64_t ticket, uint32_t error)
{
    mvdm_vm_continuation *request;
    DWORD status = ERROR_NOT_FOUND;
    if (!ticket) return ERROR_INVALID_PARAMETER;
    host_ica_lock();
    request = *find(token);
    if (request) {
        guest_application *io = request->io;
        if (request->identity.vm != vm) status = ERROR_ACCESS_DENIED;
        else if (io && ticket == io->ticket) {
            if (io->acknowledged) status = ERROR_INVALID_STATE;
            else {
                io->error = error;
                io->acknowledged = TRUE;
                status = SetEvent(io->ready) ? ERROR_SUCCESS : GetLastError();
            }
        }
    }
    host_ica_unlock();
    return status;
}

uint32_t mvdm_vm_continuation_finish_io(mvdm_vm_continuation *request,
    uint64_t ticket, uint32_t cancellation_error)
{
    guest_application *io = NULL;
    DWORD error = ERROR_NOT_FOUND;
    if (!request || !ticket) return ERROR_INVALID_PARAMETER;
    host_ica_lock();
    if (request->io && request->io->ticket == ticket) {
        io = request->io;
        if (request->retired) error = ERROR_OPERATION_ABORTED;
        else if (io->acknowledged) error = io->error;
        else error = cancellation_error ? cancellation_error : ERROR_IO_INCOMPLETE;
        if (error == ERROR_IO_INCOMPLETE && !io->acknowledged && !cancellation_error)
            io = NULL;
        else request->io = NULL;
    }
    host_ica_unlock();
    free_io(io);
    return error;
}
