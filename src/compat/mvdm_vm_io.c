#include "include/mvdm_vm_io.h"
#include <string.h>

struct mvdm_vm_io {
    ntvdm_vm_text *text;
    mvdm_vm_continuation *continuation;
    char title[CONSOLE_IO_TITLE_BYTES]; /* presentation/handoff caption */
    char *console_title; /* full API title; caption publication cannot truncate it */
    mvdm_vm_input input;
    uint32_t input_mode;
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
    io->console_title = HeapAlloc(GetProcessHeap(), 0, length + 1);
    if (!io->console_title) { HeapFree(GetProcessHeap(), 0, io); return ERROR_NOT_ENOUGH_MEMORY; }
    memcpy(io->console_title, title, length + 1);
    error = mvdm_vm_input_begin(&io->input, continuation);
    if (error) {
        HeapFree(GetProcessHeap(), 0, io->console_title);
        HeapFree(GetProcessHeap(), 0, io); return error;
    }
    io->text = text; io->continuation = continuation;
    memcpy(io->title, title, length + 1);
    *output = io;
    return ERROR_SUCCESS;
}
HANDLE mvdm_vm_io_ready(mvdm_vm_io *io) { return io ? io->input.ready : NULL; }
mvdm_vm_input *mvdm_vm_io_input(mvdm_vm_io *io) { return &io->input; }

void mvdm_vm_io_destroy(mvdm_vm_io *io)
{
    if (!io) return;
    CloseHandle(io->input.ready);
    HeapFree(GetProcessHeap(), 0, io->console_title);
    HeapFree(GetProcessHeap(), 0, io);
}
DWORD mvdm_vm_io_dispatch(mvdm_vm_io *io, const HANDLE cancellation[4],
    const console_io_request *request, console_io_reply *reply)
{
    DWORD error;
    if (!io || !cancellation || !request || !reply) return ERROR_INVALID_PARAMETER;
    reply->bytes = 0; ZeroMemory(&reply->state, sizeof(reply->state));
    switch (request->operation) {
    case CONSOLE_IO_SET_TITLE_A: {
        char *copy;
        if (!request->data || !request->bytes || request->bytes > CONSOLE_IO_TILE_BYTES ||
            memchr(request->data, 0, request->bytes) != request->data + request->bytes - 1) {
            error = ERROR_INVALID_PARAMETER; break;
        }
        copy = HeapAlloc(GetProcessHeap(), 0, request->bytes);
        if (!copy) { error = ERROR_NOT_ENOUGH_MEMORY; break; }
        memcpy(copy, request->data, request->bytes);
        HeapFree(GetProcessHeap(), 0, io->console_title);
        io->console_title = copy;
        error = ERROR_SUCCESS;
        break;
    }
    case CONSOLE_IO_GET_TITLE_A: {
        uint32_t capacity = request->state.count, length, count;
        if (request->bytes || capacity > CONSOLE_IO_TILE_BYTES || (capacity && !reply->data)) {
            error = ERROR_INVALID_PARAMETER; break;
        }
        length = (uint32_t)strlen(io->console_title);
        count = length < capacity ? length : 0;
        /* Match the selected native ANSI API: insufficient storage receives
         * an empty string; an empty title leaves the caller buffer untouched. */
        if (capacity && length) {
            memcpy(reply->data, io->console_title, count);
            reply->data[count] = 0;
            reply->bytes = count + 1;
        }
        reply->state.count = count;
        error = ERROR_SUCCESS;
        break;
    }
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
        error = ntvdm_vm_text_pending(io->text) ? ERROR_BUSY : mvdm_vm_input_read(&io->input, request, reply); break;
    case CONSOLE_IO_PREPEND_KEYS:
        error = ntvdm_vm_text_pending(io->text) ? ERROR_BUSY : mvdm_vm_input_prepend(&io->input, request, reply); break;
    default:
        return mvdm_vm_application_dispatch(io->continuation, io->text, cancellation, request, reply);
    }
    reply->error = error; reply->result = !error;
    if (error) reply->bytes = 0;
    return ERROR_SUCCESS;
}

void mvdm_vm_io_finish(mvdm_vm_io *io)
{
    ntvdm_vm_text_disconnect(io->text);
}
