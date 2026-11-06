#include "mvdm_vm_continuation.h"
#include "mvdm_softpc_guest_memory.h"
#include "common/protocol/console_io.h"
#include "ntvdm-exe/frontend/vm_text.h"
#include <string.h>

DWORD mvdm_vm_application_publish(mvdm_vm_continuation *request,
    const void *bytes, uint32_t size, const HANDLE cancellation[4])
{
    HANDLE waits[5], ready;
    DWORD count = 0, i, j, status, error;
    uint64_t ticket;
    for (i = 0; cancellation && i < 4; ++i) {
        if (!cancellation[i]) continue;
        if (cancellation[i] == INVALID_HANDLE_VALUE) return ERROR_INVALID_HANDLE;
        for (j = 0; j < count && waits[j] != cancellation[i]; ++j) {}
        if (j == count) waits[count++] = cancellation[i];
    }
    if (count) {
        status = WaitForMultipleObjects(count, waits, FALSE, 0);
        if (status == WAIT_FAILED) return GetLastError();
        if (status != WAIT_TIMEOUT) return ERROR_OPERATION_ABORTED;
    }
    error = mvdm_vm_continuation_post_io(request, bytes, size, &ticket, &ready);
    if (error) return error;
    waits[count] = ready;
    status = WaitForMultipleObjects(count + 1, waits, FALSE, INFINITE);
    error = status == WAIT_OBJECT_0 + count ? ERROR_SUCCESS :
        status == WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
    return mvdm_vm_continuation_finish_io(request, ticket, error);
}

/* Existing BOP transport; copy a bounded descriptor, never retain VMM memory.
 * token[2], ticket[2], buffer address, capacity/extent, guest status. */
DWORD mvdm_vm_application_bop(uint32_t operation, uint32_t vm,
    uint32_t address, uint32_t extent)
{
    uint32_t descriptor[7], size;
    uint64_t token, ticket;
    DWORD error;
    void *bytes = NULL;
    if (extent != sizeof(descriptor) || !mvdm_softpc_guest_memory_copy_from(
        address, (uint8_t *)descriptor, sizeof(descriptor))) return ERROR_INVALID_ADDRESS;
    memcpy(&token, descriptor, 8); memcpy(&ticket, descriptor + 2, 8);
    if (operation == 14)
        return mvdm_vm_continuation_ack_io(token, vm, ticket, descriptor[6]);
    if (descriptor[5] > CONSOLE_IO_MAX_DATA_BYTES) return ERROR_INVALID_DATA;
    if (descriptor[4] && descriptor[5]) {
        bytes = HeapAlloc(GetProcessHeap(), 0, descriptor[5]);
        if (!bytes) return ERROR_NOT_ENOUGH_MEMORY;
    }
    error = mvdm_vm_continuation_read_io(token, vm, &ticket, bytes,
        bytes ? descriptor[5] : 0, &size);
    if (!error && !mvdm_softpc_guest_memory_copy_to(descriptor[4], bytes, size))
        error = ERROR_INVALID_ADDRESS;
    if (bytes) HeapFree(GetProcessHeap(), 0, bytes);
    memcpy(descriptor + 2, &ticket, 8); descriptor[5] = size;
    if (!mvdm_softpc_guest_memory_copy_to(address, (uint8_t *)descriptor,
        sizeof(descriptor))) error = ERROR_INVALID_ADDRESS;
    return error;
}


DWORD mvdm_vm_application_commit(mvdm_vm_continuation *request,
    ntvdm_vm_text *text, const HANDLE cancellation[4])
{
    void *bytes = NULL;
    uint32_t size = 0;
    DWORD error = ntvdm_vm_text_copy_application(text, &bytes, &size);
    if (!error)
        error = mvdm_vm_application_publish(request, bytes, size, cancellation);
    if (bytes) HeapFree(GetProcessHeap(), 0, bytes);
    /* Preserve the originating failure even when no transaction was open. */
    {
        DWORD committed = ntvdm_vm_text_finish(text, error);
        return error ? error : committed;
    }
}

DWORD mvdm_vm_application_dispatch(mvdm_vm_continuation *continuation,
    ntvdm_vm_text *text, const HANDLE cancellation[4],
    const console_io_request *request, console_io_reply *reply)
{
    DWORD error;
    BOOL mutation = FALSE;
    if (!continuation || !text || !request || !reply) return ERROR_INVALID_PARAMETER;
    if (request->bytes && !request->data) return ERROR_INVALID_DATA;
    reply->bytes = 0;
    reply->result = FALSE;
    reply->error = ERROR_SUCCESS;
    ZeroMemory(&reply->state, sizeof(reply->state));
    if (ntvdm_vm_text_pending(text) == 2 &&
        request->operation != CONSOLE_IO_VIDEO_DATA &&
        request->operation != CONSOLE_IO_PUBLICATION_ABORT) {
        error = ERROR_BUSY; goto done;
    }
    switch (request->operation) {
    case CONSOLE_IO_ATTRIBUTE: case CONSOLE_IO_WINDOW_RECT:
    case CONSOLE_IO_FILL_CHARACTER: case CONSOLE_IO_FILL_ATTRIBUTE:
    case CONSOLE_IO_CURSOR_POSITION: case CONSOLE_IO_CURSOR_INFO:
    case CONSOLE_IO_WRITE_CELLS_A: case CONSOLE_IO_WRITE_CELLS_W:
    case CONSOLE_IO_VIDEO_BEGIN:
        mutation = TRUE; break;
    }
    if (mutation && !ntvdm_vm_text_pending(text)) {
        error = ntvdm_vm_text_begin(text, TRUE);
        if (error) goto done;
    }
    if (request->operation == CONSOLE_IO_PUBLICATION_END) {
        if (request->bytes) return ERROR_INVALID_DATA;
        error = mvdm_vm_application_commit(continuation, text, cancellation);
    } else error = ntvdm_vm_text_dispatch(text, request, reply);
    if (ntvdm_vm_text_pending(text) == 2) {
        if (error) (void)ntvdm_vm_text_finish(text, error);
        else if (ntvdm_vm_text_ready(text))
            error = mvdm_vm_application_commit(continuation, text, cancellation);
    }
done:
    reply->error = error;
    reply->result = !error;
    if (error) reply->bytes = 0;
    return ERROR_SUCCESS;
}
