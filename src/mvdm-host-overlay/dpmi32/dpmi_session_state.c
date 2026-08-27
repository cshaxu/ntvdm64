#include "dpmi_session_state.h"

#include <stdlib.h>
#include <string.h>

#include "session/session.h"

typedef struct mvdm_dpmi_session_state {
    session *owner;
    mvdm_dpmi_session_state_snapshot snapshot;
    struct mvdm_dpmi_session_state *next;
} mvdm_dpmi_session_state;

static mvdm_dpmi_session_state *mvdm_dpmi_session_states;

static uint16_t get16(const uint8_t *bytes, uint32_t offset)
{
    return (uint16_t)((uint16_t)bytes[offset] |
        ((uint16_t)bytes[offset + 1u] << 8));
}

static uint32_t get32(const uint8_t *bytes, uint32_t offset)
{
    return (uint32_t)bytes[offset] | ((uint32_t)bytes[offset + 1u] << 8) |
        ((uint32_t)bytes[offset + 2u] << 16) |
        ((uint32_t)bytes[offset + 3u] << 24);
}

static void mvdm_dpmi_session_cleanup(void *context)
{
    session *owner = (session *)context;
    mvdm_dpmi_session_state **cursor = &mvdm_dpmi_session_states;
    while (*cursor != NULL) {
        mvdm_dpmi_session_state *state = *cursor;
        if (state->owner == owner) {
            *cursor = state->next;
            free(state);
        } else {
            cursor = &state->next;
        }
    }
}

static mvdm_dpmi_session_state *current_state(int create)
{
    session *owner = session_thread_current();
    mvdm_dpmi_session_state *state;
    if (owner == NULL) return NULL;
    for (state = mvdm_dpmi_session_states; state != NULL; state = state->next) {
        if (state->owner == owner) return state;
    }
    if (!create || !session_register_teardown(owner, mvdm_dpmi_session_cleanup,
        owner)) return NULL;
    state = (mvdm_dpmi_session_state *)calloc(1u, sizeof(*state));
    if (state == NULL) return NULL;
    state->owner = owner;
    state->next = mvdm_dpmi_session_states;
    mvdm_dpmi_session_states = state;
    return state;
}

int mvdm_dpmi_session_record_dosx(const uint8_t *bytes, uint32_t byte_count)
{
    mvdm_dpmi_session_state *state = current_state(1);
    if (state == NULL || bytes == NULL || byte_count < 58u) return 0;
    state->snapshot.dosx_stack_segment = get16(bytes, 0u);
    state->snapshot.small_xlat_buffer = get32(bytes, 2u);
    state->snapshot.large_xlat_buffer = get32(bytes, 6u);
    state->snapshot.dosx_stack_frame = get32(bytes, 10u);
    state->snapshot.dosx_stack_frame_size = get16(bytes, 14u);
    state->snapshot.rm_bop_fe = get32(bytes, 16u);
    state->snapshot.dosx_rm_code_segment = get16(bytes, 20u);
    state->snapshot.dosx_dta_buffer = get32(bytes, 22u);
    state->snapshot.dosx_pm_data_selector = get16(bytes, 26u);
    state->snapshot.dosx_rm_code_selector = get16(bytes, 28u);
    state->snapshot.dosx_segment_to_selector = get32(bytes, 30u);
    state->snapshot.dosx_fault_iret = get32(bytes, 34u);
    state->snapshot.dosx_fault_iretd = get32(bytes, 38u);
    state->snapshot.dosx_int_iret = get32(bytes, 42u);
    state->snapshot.dosx_int_iretd = get32(bytes, 46u);
    state->snapshot.dosx_iret = get32(bytes, 50u);
    state->snapshot.dosx_iretd = get32(bytes, 54u);
    return 1;
}

int mvdm_dpmi_session_record_app(uint16_t flags, uint32_t dta,
    uint16_t dta_offset, uint16_t dta_selector, uint16_t psp_selector)
{
    mvdm_dpmi_session_state *state = current_state(1);
    if (state == NULL) return 0;
    state->snapshot.current_app_flags = flags;
    state->snapshot.current_dta = dta;
    state->snapshot.current_dta_offset = dta_offset;
    state->snapshot.current_dta_selector = dta_selector;
    state->snapshot.current_psp_selector = psp_selector;
    return 1;
}

int mvdm_dpmi_session_record_selector_table(uint16_t selector)
{
    mvdm_dpmi_session_state *state = current_state(1);
    if (state == NULL) return 0;
    state->snapshot.selector_table = (uint32_t)selector << 16;
    return 1;
}

int mvdm_dpmi_session_snapshot(mvdm_dpmi_session_state_snapshot *snapshot_out)
{
    mvdm_dpmi_session_state *state = current_state(0);
    if (snapshot_out == NULL || state == NULL) return 0;
    *snapshot_out = state->snapshot;
    return 1;
}
