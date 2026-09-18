#define MVDM_REDIRECTOR_WORKER_EXPORTS
#include "mvdm_redirector_async.h"

#include <stdlib.h>
#include <string.h>

#include "ntvdm-exe/softpc/include/mvdm_guest_location.h"
#include "ntvdm-exe/session/session.h"
#include "mvdm/inc/vrnmpipe.h"

/* Default-off proof for the one standalone ownership seam in this file.
 * It records no guest bytes, addresses, handles or command data. */
void mvdm_redirector_async_trace(char const *stage)
{
    char path[MAX_PATH];
    HANDLE file;
    DWORD path_bytes;
    DWORD written;

    path_bytes = GetEnvironmentVariableA("MVDM_REDIR_ASYNC_TRACE_PATH",
        path, (DWORD)sizeof(path));
    if (stage == NULL || path_bytes == 0u || path_bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    (void)WriteFile(file, stage, (DWORD)strlen(stage), &written, NULL);
    (void)WriteFile(file, "\r\n", 2u, &written, NULL);
    CloseHandle(file);
}

typedef struct mvdm_redirector_async_state {
    /* VrpAsyncNmPipeThread is an original DLL-owned worker thread.  It has no
     * inherited TLS binding, whereas these bounded locations belong to the
     * one NTVDM worker session that accepted the request. */
    session *owner;
    uint32_t owner_epoch;
    mvdm_guest_location buffer;
    mvdm_guest_location bytes_transferred;
    mvdm_guest_location error_code;
    uint8_t *staging;
    WORD length;
    int is_read;
    int completion_bound;
} mvdm_redirector_async_state;

static uint16_t read_u16(uint8_t const *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

static uint32_t read_u32(uint8_t const *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

static int location_from_value(mvdm_guest_location *location, uint32_t value)
{
    return value != 0u && mvdm_guest_location_from_far_value(location, value);
}

int mvdm_redirector_async_prepare(PDOS_ASYNC_NAMED_PIPE_INFO request,
    WORD segment, WORD offset, DWORD request_type, LPBYTE *buffer_out,
    WORD *length_out)
{
    mvdm_guest_location request_location;
    mvdm_guest_location_lease request_lease;
    mvdm_guest_location_lease buffer_lease;
    mvdm_redirector_async_state *state;
    uint8_t const *bytes;
    uint32_t buffer_value;

    session *owner;

    if (buffer_out != NULL) *buffer_out = NULL;
    if (length_out != NULL) *length_out = 0u;
    owner = session_thread_current();
    mvdm_redirector_async_trace("prepare-enter");
    if (request == NULL || buffer_out == NULL || length_out == NULL ||
        request->PrivateAsyncState != NULL ||
        owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE ||
        !mvdm_guest_location_set_real_mode(&request_location, segment, offset) ||
        !mvdm_guest_location_acquire(&request_location, 24u,
            GUEST_MEMORY_ACCESS_READ, &request_lease)) {
        mvdm_redirector_async_trace("prepare-rejected");
        return 0;
    }

    bytes = request_lease.bytes;
    state = (mvdm_redirector_async_state *)calloc(1u, sizeof(*state));
    if (state == NULL) {
        (void)mvdm_guest_location_release(&request_lease, 0);
        return 0;
    }
    state->owner = owner;
    state->owner_epoch = owner->epoch;
    state->length = read_u16(bytes + 4u);
    buffer_value = read_u32(bytes + 6u);
    state->is_read = request_type == 0x86u || request_type == 0x90u;
    request->pBytesTransferred = read_u32(bytes);
    request->Buffer = buffer_value;
    request->pErrorCode = read_u32(bytes + 10u);
    request->ANR = read_u32(bytes + 14u);
    request->Type2 = request_type == 0x90u || request_type == 0x91u;
    request->Semaphore = request->Type2 ? read_u32(bytes + 20u) : 0u;
    if (!mvdm_guest_location_release(&request_lease, 0) ||
        (state->length != 0u &&
            !location_from_value(&state->buffer, buffer_value)) ||
        !location_from_value(&state->bytes_transferred, request->pBytesTransferred) ||
        !location_from_value(&state->error_code, request->pErrorCode)) {
        free(state);
        return 0;
    }
    if (state->length != 0u) {
        state->staging = (uint8_t *)malloc(state->length);
        if (state->staging == NULL) {
            free(state);
            return 0;
        }
        if (!state->is_read) {
            if (!mvdm_guest_location_acquire(&state->buffer, state->length,
                GUEST_MEMORY_ACCESS_READ, &buffer_lease)) {
                free(state->staging);
                free(state);
                return 0;
            }
            memcpy(state->staging, buffer_lease.bytes, state->length);
            if (!mvdm_guest_location_release(&buffer_lease, 0)) {
                free(state->staging);
                free(state);
                return 0;
            }
        }
    }
    request->PrivateAsyncState = state;
    *buffer_out = state->staging;
    *length_out = state->length;
    mvdm_redirector_async_trace("prepare-ok");
    return 1;
}

int mvdm_redirector_async_completion_begin(PDOS_ASYNC_NAMED_PIPE_INFO request)
{
    mvdm_redirector_async_state *state;

    if (request == NULL || request->PrivateAsyncState == NULL) {
        return 0;
    }
    state = (mvdm_redirector_async_state *)request->PrivateAsyncState;
    if (state->owner == NULL || !session_valid(state->owner) ||
        state->owner->state != SESSION_STATE_ACTIVE ||
        state->owner->epoch != state->owner_epoch) {
        return 0;
    }
    if (session_thread_current() == state->owner) return 1;
    if (session_thread_current() != NULL ||
        !session_thread_bind_owned_source(state->owner,
            SESSION_THREAD_BINDING_ORIGINAL_WORKER,
            "VDMREDIR async completion")) return 0;
    state->completion_bound = 1;
    mvdm_redirector_async_trace("completion-bound");
    return 1;
}

void mvdm_redirector_async_completion_end(PDOS_ASYNC_NAMED_PIPE_INFO request)
{
    mvdm_redirector_async_state *state;

    if (request == NULL || request->PrivateAsyncState == NULL) return;
    state = (mvdm_redirector_async_state *)request->PrivateAsyncState;
    if (state->completion_bound) {
        state->completion_bound = 0;
        (void)session_thread_unbind(state->owner);
        mvdm_redirector_async_trace("completion-unbound");
    }
}

int mvdm_redirector_async_complete(PDOS_ASYNC_NAMED_PIPE_INFO request,
    DWORD byte_count, DWORD error_code)
{
    mvdm_redirector_async_state *state;
    mvdm_guest_location_lease lease;
    uint8_t words[2];

    mvdm_redirector_async_trace("complete-enter");
    if (request == NULL || request->PrivateAsyncState == NULL) {
        mvdm_redirector_async_trace("complete-no-state");
        return 0;
    }
    state = (mvdm_redirector_async_state *)request->PrivateAsyncState;
    if (session_thread_current() != state->owner) {
        mvdm_redirector_async_trace("complete-not-bound");
        return 0;
    }
    words[0] = (uint8_t)error_code;
    words[1] = (uint8_t)(error_code >> 8);
    if (!mvdm_guest_location_copy_to_guest(&state->error_code, words, 2u))
        goto failed;
    words[0] = (uint8_t)byte_count;
    words[1] = (uint8_t)(byte_count >> 8);
    if (!mvdm_guest_location_copy_to_guest(&state->bytes_transferred, words, 2u))
        goto failed;
    if (state->is_read && byte_count != 0u) {
        if (byte_count > state->length || !mvdm_guest_location_acquire(
            &state->buffer, byte_count, GUEST_MEMORY_ACCESS_WRITE, &lease)) goto failed;
        memcpy(lease.bytes, state->staging, byte_count);
        if (!mvdm_guest_location_release(&lease, 1)) goto failed;
    }
    mvdm_redirector_async_trace("complete-ok");
    return 1;

failed:
    mvdm_redirector_async_trace("complete-copy-failed");
    return 0;
}

void mvdm_redirector_async_release(PDOS_ASYNC_NAMED_PIPE_INFO request)
{
    mvdm_redirector_async_state *state;
    if (request == NULL || request->PrivateAsyncState == NULL) return;
    state = (mvdm_redirector_async_state *)request->PrivateAsyncState;
    free(state->staging);
    free(state);
    request->PrivateAsyncState = NULL;
}
