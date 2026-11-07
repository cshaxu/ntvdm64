#include "mvdm_vm_io.h"
#include "mvdm_vm_wake.h"
#include <string.h>

struct mvdm_vm_io {
    ntvdm_vm_text *text;
    mvdm_vm_continuation *continuation;
    char title[CONSOLE_IO_TITLE_BYTES];
    SRWLOCK input_lock;
    HANDLE ready;
    console_io_input keys[CONSOLE_IO_INPUT_CAPACITY];
    uint32_t head, count, input_mode;
};

DWORD mvdm_vm_io_create(ntvdm_vm_text *text, mvdm_vm_continuation *continuation,
    const char *title, mvdm_vm_io **output)
{
    mvdm_vm_io *io;
    size_t length;
    DWORD error;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output = NULL;
    if (!text || !continuation || !title) return ERROR_INVALID_PARAMETER;
    length = strnlen_s(title, CONSOLE_IO_TITLE_BYTES);
    if (length == CONSOLE_IO_TITLE_BYTES) return ERROR_INVALID_PARAMETER;
    io = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*io));
    if (!io) return ERROR_NOT_ENOUGH_MEMORY;
    io->ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!io->ready) {
        error = GetLastError(); HeapFree(GetProcessHeap(), 0, io); return error;
    }
    InitializeSRWLock(&io->input_lock);
    io->text = text; io->continuation = continuation;
    memcpy(io->title, title, length + 1);
    *output = io;
    return ERROR_SUCCESS;
}
HANDLE mvdm_vm_io_ready(mvdm_vm_io *io) { return io ? io->ready : NULL; }
DWORD mvdm_vm_io_return_input(mvdm_vm_io *io, uint32_t vm, uint64_t generation)
{
    console_io_input *keys;
    uint32_t i;
    DWORD error;
    if (!io) return ERROR_INVALID_PARAMETER;
    /* The caller joined all callbacks and rejected producers; no executor
     * lock is held while entering ICA. Preserve wrapped FIFO order. */
    if (!io->count) return ERROR_SUCCESS;
    keys = HeapAlloc(GetProcessHeap(), 0, io->count * sizeof(*keys));
    if (!keys) return ERROR_NOT_ENOUGH_MEMORY;
    for (i = 0; i < io->count; ++i)
        keys[i] = io->keys[(io->head + i) % CONSOLE_IO_INPUT_CAPACITY];
    error = mvdm_vm_wake_return_input(vm, generation, keys, io->count);
    if (!error) io->head = io->count = 0;
    HeapFree(GetProcessHeap(), 0, keys);
    return error;
}
void mvdm_vm_io_destroy(mvdm_vm_io *io)
{
    if (!io) return;
    CloseHandle(io->ready);
    HeapFree(GetProcessHeap(), 0, io);
}
DWORD mvdm_vm_io_keys(mvdm_vm_io *io, const KEY_EVENT_RECORD *keys, uint32_t count)
{
    uint32_t i;
    DWORD error = ERROR_SUCCESS;
    if (!io || !keys || !count || count > CONSOLE_IO_INPUT_CAPACITY) return ERROR_INVALID_PARAMETER;
    for (i = 0; i < count; ++i)
        if (!keys[i].wRepeatCount) return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&io->input_lock);
    {
        DWORD state = WaitForSingleObject(mvdm_vm_continuation_invalidated(io->continuation), 0);
        if (state != WAIT_TIMEOUT)
            error = state == WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
    }
    if (!error) {
        if (count > CONSOLE_IO_INPUT_CAPACITY - io->count) error = ERROR_NOT_ENOUGH_MEMORY;
        else if (!SetEvent(io->ready)) error = GetLastError();
        else {
            for (i = 0; i < count; ++i) {
                console_io_input wire = {0};
                wire.type = KEY_EVENT; wire.key_down = keys[i].bKeyDown != FALSE;
                wire.repeat = keys[i].wRepeatCount; wire.virtual_key = keys[i].wVirtualKeyCode;
                wire.scan = keys[i].wVirtualScanCode; wire.character = keys[i].uChar.UnicodeChar;
                wire.control = keys[i].dwControlKeyState;
                io->keys[(io->head + io->count + i) % CONSOLE_IO_INPUT_CAPACITY] = wire;
            }
            io->count += count;
        }
    }
    ReleaseSRWLockExclusive(&io->input_lock);
    return error;
}
DWORD mvdm_vm_io_key(mvdm_vm_io *io, const KEY_EVENT_RECORD *key)
{ return mvdm_vm_io_keys(io, key, 1); }
static DWORD input_pending(void *context, BOOL *pending)
{
    mvdm_vm_io *io = context;
    if (!pending) return ERROR_INVALID_PARAMETER;
    AcquireSRWLockShared(&io->input_lock);
    *pending = io->count != 0;
    ReleaseSRWLockShared(&io->input_lock);
    return ERROR_SUCCESS;
}
static DWORD read_keys(mvdm_vm_io *io, const console_io_request *request, console_io_reply *reply)
{
    uint32_t count, remaining, i;
    if (request->state.count > CONSOLE_IO_INPUT_CAPACITY || !reply->data)
        return ERROR_INVALID_PARAMETER;
    /* These copied guest events have no borrowed native Console key-state
     * receipt. Returnable reads use the protocol's zero-receipt form and
     * PREPEND_KEYS returns the exact records to this owned queue. */
    if (request->state.mode > CONSOLE_IO_INPUT_RETURNABLE || request->bytes ||
        (request->operation == CONSOLE_IO_PEEK_INPUT && request->state.mode))
        return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&io->input_lock);
    count = min(io->count, request->state.count);
    remaining = io->count - (request->operation == CONSOLE_IO_READ_INPUT ? count : 0);
    /* A failed readiness transition must not consume undelivered input. */
    if (!remaining && !ResetEvent(io->ready)) {
        DWORD error = GetLastError();
        ReleaseSRWLockExclusive(&io->input_lock); return error;
    }
    for (i = 0; i < count; ++i)
        memcpy(reply->data + i * sizeof(console_io_input),
            &io->keys[(io->head + i) % CONSOLE_IO_INPUT_CAPACITY], sizeof(console_io_input));
    if (request->operation == CONSOLE_IO_READ_INPUT) {
        io->head = (io->head + count) % CONSOLE_IO_INPUT_CAPACITY;
        io->count = remaining;
    }
    ReleaseSRWLockExclusive(&io->input_lock);
    reply->state.count = count; reply->bytes = count * sizeof(console_io_input);
    return ERROR_SUCCESS;
}
static DWORD prepend_keys(mvdm_vm_io *io, const console_io_request *request, console_io_reply *reply)
{
    uint32_t count = request->state.count, i;
    DWORD error = ERROR_SUCCESS;
    if (count > CONSOLE_IO_INPUT_CAPACITY || request->bytes != count * sizeof(console_io_input) ||
        (count && !request->data)) return ERROR_INVALID_PARAMETER;
    for (i = 0; i < count; ++i) {
        console_io_input key;
        memcpy(&key, request->data + i * sizeof(key), sizeof(key));
        if (key.type != KEY_EVENT || !key.repeat || key.repeat > UINT16_MAX ||
            key.virtual_key > UINT16_MAX || key.scan > UINT16_MAX || key.character > UINT16_MAX ||
            key.key_down > 1 || key.receipt_low || key.receipt_high)
            return ERROR_INVALID_DATA;
    }
    AcquireSRWLockExclusive(&io->input_lock);
    if (count > CONSOLE_IO_INPUT_CAPACITY - io->count) error = ERROR_NOT_ENOUGH_MEMORY;
    else if (count && !SetEvent(io->ready)) error = GetLastError();
    else {
        /* Validate and reserve the entire returned batch before changing the
         * queue; returned native input precedes newly arrived guest input. */
        io->head = (io->head + CONSOLE_IO_INPUT_CAPACITY - count) % CONSOLE_IO_INPUT_CAPACITY;
        for (i = 0; i < count; ++i)
            memcpy(&io->keys[(io->head + i) % CONSOLE_IO_INPUT_CAPACITY],
                request->data + i * sizeof(console_io_input), sizeof(console_io_input));
        io->count += count;
        reply->state.count = count;
    }
    ReleaseSRWLockExclusive(&io->input_lock);
    return error;
}
static DWORD dispatch(void *context, const ntvdm_vm_cancel *cancel,
    const console_io_request *request, console_io_reply *reply, frontend_base_result *result)
{
    mvdm_vm_io *io = context;
    DWORD error;
    HANDLE waits[4];
    if (!io || !cancel || !request || !reply || !result) return ERROR_INVALID_PARAMETER;
    waits[0] = cancel->stop; waits[1] = cancel->worker;
    waits[2] = cancel->retired; waits[3] = cancel->broker;
    result->reply = TRUE; result->terminal_error = 0;
    reply->bytes = 0; ZeroMemory(&reply->state, sizeof(reply->state));
    switch (request->operation) {
    case CONSOLE_IO_PUBLISH_TITLE_A:
        /* Channel-owned caption, also the next carrier's handoff title.
         * Do not change the worker's native Console or execution identity. */
        if (!request->data || !request->bytes || request->bytes > sizeof(io->title) ||
            request->data[request->bytes - 1] ||
            strnlen_s((const char *)request->data, request->bytes) != request->bytes - 1)
            error = ERROR_INVALID_PARAMETER;
        else {
            memcpy(io->title, request->data, request->bytes);
            error = ERROR_SUCCESS;
        }
        break;
    case CONSOLE_IO_GET_HANDOFF_TITLE_A:
        if (request->bytes || !reply->data) error = ERROR_INVALID_PARAMETER;
        else {
            reply->bytes = (uint32_t)strlen(io->title) + 1;
            memcpy(reply->data, io->title, reply->bytes); error = ERROR_SUCCESS;
        }
        break;
    case CONSOLE_IO_GET_MODE: case CONSOLE_IO_SET_MODE:
        if (request->bytes || request->state.input > 1) error = ERROR_INVALID_PARAMETER;
        else if (!request->state.input) {
            /* Guest cells have no native Console output processing enabled.
             * Never accept cooked/VT flags without their stream semantics. */
            reply->state.mode = 0;
            error = request->operation == CONSOLE_IO_GET_MODE || !request->state.mode ?
                ERROR_SUCCESS : ERROR_NOT_SUPPORTED;
        }
        else if (request->operation == CONSOLE_IO_GET_MODE) {
            reply->state.mode = io->input_mode; error = ERROR_SUCCESS;
        } else if (request->state.mode & ~(ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS))
            error = ERROR_NOT_SUPPORTED;
        else { io->input_mode = request->state.mode; error = ERROR_SUCCESS; }
        break;
    case CONSOLE_IO_READ_INPUT: case CONSOLE_IO_PEEK_INPUT:
        error = ntvdm_vm_text_pending(io->text) ? ERROR_BUSY : read_keys(io, request, reply); break;
    case CONSOLE_IO_PREPEND_KEYS:
        error = ntvdm_vm_text_pending(io->text) ? ERROR_BUSY : prepend_keys(io, request, reply); break;
    default:
        return mvdm_vm_application_dispatch(io->continuation, io->text, waits, request, reply);
    }
    reply->error = error; reply->result = !error;
    if (error) reply->bytes = 0;
    return ERROR_SUCCESS;
}
static void finish(void *context)
{
    mvdm_vm_io *io = context;
    ntvdm_vm_text_disconnect(io->text);
}
const ntvdm_vm_io mvdm_vm_io_operations = {dispatch, input_pending, finish};
