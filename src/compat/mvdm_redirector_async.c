#define MVDM_REDIRECTOR_WORKER_EXPORTS
#include "include/mvdm_redirector_async.h"

#include <stdlib.h>
#include <string.h>

#include "include/guest_location.h"
#include "include/host_context.h"
#include "include/mvdm_cpu_access.h"

typedef struct mvdm_redirector_async_state {
    compat_host_context *owner;
    uint32_t owner_epoch;
    mvdm_guest_location buffer;
    mvdm_guest_location bytes_transferred;
    mvdm_guest_location error_code;
    uint8_t *staging;
    WORD length;
    int is_read;
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

int mvdm_redirector_async_prepare(void **state_out,
    DWORD *guest_buffer_out, DWORD *anr_out, BOOL *type2_out, DWORD *semaphore_out,
    WORD segment, WORD offset, int is_read, int type2, LPBYTE *buffer_out,
    WORD *length_out)
{
    mvdm_guest_location request_location;
    mvdm_guest_location_lease request_lease;
    mvdm_guest_location_lease buffer_lease;
    mvdm_redirector_async_state *state;
    uint8_t const *bytes;
    uint32_t buffer_value, count_value, error_value;

    compat_host_context *owner;

    if (buffer_out != NULL) *buffer_out = NULL;
    if (length_out != NULL) *length_out = 0u;
    owner = compat_host_context_current();
    if (state_out == NULL || guest_buffer_out == NULL || anr_out == NULL ||
        type2_out == NULL || semaphore_out == NULL ||
        buffer_out == NULL || length_out == NULL ||
        !compat_host_context_valid(owner) ||
        owner->state != COMPAT_HOST_CONTEXT_ACTIVE ||
        !mvdm_guest_location_set_real_mode(&request_location, segment, offset) ||
        !mvdm_cpu_access_current()->acquire_location(&request_location,
            type2 ? 24u : 18u,
            GUEST_MEMORY_ACCESS_READ, &request_lease)) {
        return 0;
    }

    bytes = request_lease.bytes;
    state = (mvdm_redirector_async_state *)calloc(1u, sizeof(*state));
    if (state == NULL) {
        (void)mvdm_cpu_access_current()->release_location(&request_lease, 0);
        return 0;
    }
    state->owner = owner;
    state->owner_epoch = owner->epoch;
    state->length = read_u16(bytes + 4u);
    buffer_value = read_u32(bytes + 6u);
    state->is_read = is_read;
    count_value = read_u32(bytes);
    *guest_buffer_out = buffer_value;
    error_value = read_u32(bytes + 10u);
    *anr_out = read_u32(bytes + 14u);
    *type2_out = type2;
    *semaphore_out = type2 ? read_u32(bytes + 20u) : 0u;
    if (!mvdm_cpu_access_current()->release_location(&request_lease, 0) ||
        (state->length != 0u &&
            !location_from_value(&state->buffer, buffer_value)) ||
        !location_from_value(&state->bytes_transferred, count_value) ||
        !location_from_value(&state->error_code, error_value)) {
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
            if (!mvdm_cpu_access_current()->acquire_location(&state->buffer, state->length,
                GUEST_MEMORY_ACCESS_READ, &buffer_lease)) {
                free(state->staging);
                free(state);
                return 0;
            }
            memcpy(state->staging, buffer_lease.bytes, state->length);
            if (!mvdm_cpu_access_current()->release_location(&buffer_lease, 0)) {
                free(state->staging);
                free(state);
                return 0;
            }
        }
    }
    *state_out = state;
    *buffer_out = state->staging;
    *length_out = state->length;
    return 1;
}

int mvdm_redirector_async_complete(void *opaque_state,
    DWORD byte_count, DWORD error_code)
{
    mvdm_redirector_async_state *state;
    mvdm_guest_location_lease lease;
    uint8_t words[2];

    if (opaque_state == NULL) {
        return 0;
    }
    state = (mvdm_redirector_async_state *)opaque_state;
    if (compat_host_context_current() != state->owner ||
        !compat_host_context_valid(state->owner) ||
        state->owner->state != COMPAT_HOST_CONTEXT_ACTIVE ||
        state->owner->epoch != state->owner_epoch) {
        return 0;
    }
    /* Original ReadFile had already written the guest payload when completion
     * published error/count. Staging must preserve that order: a failed payload
     * commit must not make the request appear completed. This is not an atomic
     * multi-destination transaction; retain original error-before-count order. */
    if (state->is_read && byte_count != 0u) {
        if (byte_count > state->length || !mvdm_cpu_access_current()->acquire_location(
            &state->buffer, byte_count, GUEST_MEMORY_ACCESS_WRITE, &lease)) goto failed;
        memcpy(lease.bytes, state->staging, byte_count);
        if (!mvdm_cpu_access_current()->release_location(&lease, 1)) goto failed;
    }
    words[0] = (uint8_t)error_code;
    words[1] = (uint8_t)(error_code >> 8);
    if (!mvdm_cpu_access_current()->copy_to_guest(&state->error_code, words, 2u))
        goto failed;
    words[0] = (uint8_t)byte_count;
    words[1] = (uint8_t)(byte_count >> 8);
    if (!mvdm_cpu_access_current()->copy_to_guest(&state->bytes_transferred, words, 2u))
        goto failed;
    return 1;

failed:
    return 0;
}

void mvdm_redirector_async_release(void **opaque_state)
{
    mvdm_redirector_async_state *state;
    if (opaque_state == NULL || *opaque_state == NULL) return;
    state = (mvdm_redirector_async_state *)*opaque_state;
    free(state->staging);
    free(state);
    *opaque_state = NULL;
}
