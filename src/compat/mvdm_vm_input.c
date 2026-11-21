#include "include/mvdm_vm_input.h"
#include "include/mvdm_vm_wake.h"
#include <string.h>

DWORD mvdm_vm_input_begin(mvdm_vm_input *input, mvdm_vm_continuation *continuation)
{
    input->ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!input->ready) return GetLastError();
    InitializeSRWLock(&input->input_lock);
    input->continuation = continuation;
    return ERROR_SUCCESS;
}

DWORD mvdm_vm_input_return(mvdm_vm_input *io, uint32_t vm, uint64_t generation)
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
static DWORD enqueue_input(mvdm_vm_input *io, const KEY_EVENT_RECORD *keys,
    const console_io_input *mouse, uint32_t count)
{
    uint32_t i;
    DWORD error = ERROR_SUCCESS;
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
                console_io_input event = {0};
                if (mouse) event = *mouse;
                else {
                    event.type = KEY_EVENT; event.key_down = keys[i].bKeyDown != FALSE;
                    event.repeat = keys[i].wRepeatCount; event.virtual_key = keys[i].wVirtualKeyCode;
                    event.scan = keys[i].wVirtualScanCode; event.character = keys[i].uChar.UnicodeChar;
                    event.control = keys[i].dwControlKeyState;
                }
                io->keys[(io->head + io->count + i) % CONSOLE_IO_INPUT_CAPACITY] = event;
            }
            io->count += count;
        }
    }
    ReleaseSRWLockExclusive(&io->input_lock);
    return error;
}
DWORD mvdm_vm_input_keys(mvdm_vm_input *io, const KEY_EVENT_RECORD *keys, uint32_t count)
{
    uint32_t i;
    if (!io || !keys || !count || count > CONSOLE_IO_INPUT_CAPACITY) return ERROR_INVALID_PARAMETER;
    for (i = 0; i < count; ++i)
        if (!keys[i].wRepeatCount) return ERROR_INVALID_PARAMETER;
    return enqueue_input(io, keys, NULL, count);
}
DWORD mvdm_vm_input_mouse(mvdm_vm_input *io, const MOUSE_EVENT_RECORD *mouse)
{
    console_io_input event = {0};
    if (!io || !mouse || mouse->dwMousePosition.X < 0 || mouse->dwMousePosition.Y < 0 ||
        (mouse->dwEventFlags != 0 && mouse->dwEventFlags != MOUSE_MOVED))
        return ERROR_INVALID_PARAMETER;
    event.type = MOUSE_EVENT; event.x = mouse->dwMousePosition.X;
    event.y = mouse->dwMousePosition.Y; event.buttons = mouse->dwButtonState;
    event.control = mouse->dwControlKeyState; event.flags = mouse->dwEventFlags;
    return enqueue_input(io, NULL, &event, 1);
}
DWORD mvdm_vm_input_key(mvdm_vm_input *io, const KEY_EVENT_RECORD *key)
{ return mvdm_vm_input_keys(io, key, 1); }
DWORD mvdm_vm_input_pending(mvdm_vm_input *io, BOOL *pending)
{
    if (!pending) return ERROR_INVALID_PARAMETER;
    AcquireSRWLockShared(&io->input_lock);
    *pending = io->count != 0;
    ReleaseSRWLockShared(&io->input_lock);
    return ERROR_SUCCESS;
}
DWORD mvdm_vm_input_read(mvdm_vm_input *io, const console_io_request *request, console_io_reply *reply)
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
DWORD mvdm_vm_input_prepend(mvdm_vm_input *io, const console_io_request *request, console_io_reply *reply)
{
    uint32_t count = request->state.count, i;
    DWORD error = ERROR_SUCCESS;
    if (count > CONSOLE_IO_INPUT_CAPACITY || request->bytes != count * sizeof(console_io_input) ||
        (count && !request->data)) return ERROR_INVALID_PARAMETER;
    for (i = 0; i < count; ++i) {
        console_io_input key;
        memcpy(&key, request->data + i * sizeof(key), sizeof(key));
        if (key.receipt_low || key.receipt_high) return ERROR_INVALID_DATA;
        if (key.type == KEY_EVENT) {
            if (!key.repeat || key.repeat > UINT16_MAX || key.virtual_key > UINT16_MAX ||
                key.scan > UINT16_MAX || key.character > UINT16_MAX || key.key_down > 1)
                return ERROR_INVALID_DATA;
        } else if (key.type == MOUSE_EVENT) {
            if (key.x < 0 || key.y < 0 || key.x > INT16_MAX || key.y > INT16_MAX ||
                (key.flags != 0 && key.flags != MOUSE_MOVED) || key.repeat ||
                key.virtual_key || key.scan || key.character || key.key_down || key.menu || key.focus)
                return ERROR_INVALID_DATA;
        } else return ERROR_INVALID_DATA;
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
