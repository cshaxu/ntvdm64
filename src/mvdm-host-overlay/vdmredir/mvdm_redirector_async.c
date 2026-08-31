#include "mvdm_redirector_async.h"

#include <stdlib.h>
#include <string.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_guest_location.h"
#include "mvdm-host/inc/vrnmpipe.h"

typedef struct mvdm_redirector_async_state {
    mvdm_guest_location buffer;
    mvdm_guest_location bytes_transferred;
    mvdm_guest_location error_code;
    uint8_t *staging;
    WORD length;
    int is_read;
} mvdm_redirector_async_state;

static volatile LONG redirector_worker_stop_requested;

void mvdm_redirector_async_worker_begin(void)
{
    InterlockedExchange(&redirector_worker_stop_requested, FALSE);
}

void mvdm_redirector_async_worker_request_stop(void)
{
    InterlockedExchange(&redirector_worker_stop_requested, TRUE);
}

int mvdm_redirector_async_worker_stop_requested(void)
{
    return InterlockedCompareExchange(&redirector_worker_stop_requested,
        FALSE, FALSE) != FALSE;
}

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

    if (buffer_out != NULL) *buffer_out = NULL;
    if (length_out != NULL) *length_out = 0u;
    if (request == NULL || buffer_out == NULL || length_out == NULL ||
        request->PrivateAsyncState != NULL ||
        !mvdm_guest_location_set_real_mode(&request_location, segment, offset) ||
        !mvdm_guest_location_acquire(&request_location, 24u,
            GUEST_MEMORY_ACCESS_READ, &request_lease)) return 0;

    bytes = request_lease.bytes;
    state = (mvdm_redirector_async_state *)calloc(1u, sizeof(*state));
    if (state == NULL) {
        (void)mvdm_guest_location_release(&request_lease, 0);
        return 0;
    }
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
    return 1;
}

int mvdm_redirector_async_complete(PDOS_ASYNC_NAMED_PIPE_INFO request,
    DWORD byte_count, DWORD error_code)
{
    mvdm_redirector_async_state *state;
    mvdm_guest_location_lease lease;
    uint8_t words[2];

    if (request == NULL || request->PrivateAsyncState == NULL) return 0;
    state = (mvdm_redirector_async_state *)request->PrivateAsyncState;
    words[0] = (uint8_t)error_code;
    words[1] = (uint8_t)(error_code >> 8);
    if (!mvdm_guest_location_copy_to_guest(&state->error_code, words, 2u))
        return 0;
    words[0] = (uint8_t)byte_count;
    words[1] = (uint8_t)(byte_count >> 8);
    if (!mvdm_guest_location_copy_to_guest(&state->bytes_transferred, words, 2u))
        return 0;
    if (state->is_read && byte_count != 0u) {
        if (byte_count > state->length || !mvdm_guest_location_acquire(
            &state->buffer, byte_count, GUEST_MEMORY_ACCESS_WRITE, &lease)) return 0;
        memcpy(lease.bytes, state->staging, byte_count);
        if (!mvdm_guest_location_release(&lease, 1)) return 0;
    }
    return 1;
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
