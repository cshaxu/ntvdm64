#include "mvdm_wow_callback_transaction.h"

#include <string.h>

#include "adapter-mvdm-host-out/monitor/include/mvdm_wow_task_frame.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_protected_span.h"
#include "session/session.h"

/* The callback-frame fixture intentionally exercises the real-mode source
 * branch. Protected span resolution is covered by the dedicated P1 fixture. */
uint32_t getPE(void)
{
    return 0u;
}

int mvdm_protected_span_resolve(uint32_t access, uint16_t selector,
    uint32_t offset, uint32_t byte_count, uint32_t *linear_out)
{
    (void)access;
    (void)selector;
    (void)offset;
    (void)byte_count;
    (void)linear_out;
    return 0;
}

typedef struct fixture_state {
    uint32_t calls;
    uint32_t expected_stack;
    uint8_t memory[0x20000];
} fixture_state;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_state *state = (fixture_state *)context;
    if (state == NULL || address > sizeof(state->memory) ||
        byte_count > sizeof(state->memory) - address) return 0;
    memcpy(bytes, state->memory + address, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t byte_count)
{
    fixture_state *state = (fixture_state *)context;
    if (state == NULL || address > sizeof(state->memory) ||
        byte_count > sizeof(state->memory) - address) return 0;
    memcpy(state->memory + address, bytes, byte_count);
    return 1;
}

static int32_t dispatch_callback(void *context, uint32_t operation,
    void *request)
{
    fixture_state *state = (fixture_state *)context;
    mvdm_wow_callback_transaction *transaction =
        (mvdm_wow_callback_transaction *)request;
    if (state == NULL || transaction == NULL ||
        operation != MVDM_WOW_CALLBACK_CONTROL_OPERATION ||
        transaction->struct_bytes != sizeof(*transaction) ||
        transaction->callback_stack != state->expected_stack ||
        transaction->task16 != 0x0042u || transaction->parameter_bytes != 3u ||
        transaction->parameters[0] != 0x12u || transaction->parameters[2] != 0x56u ||
        transaction->guest_frame.w_tdb != 0x0042u ||
        transaction->guest_frame.w_local_bp != 0xbeefu ||
        transaction->guest_frame.vpfn_proc != 0xabcd1234u ||
        transaction->guest_frame.vp_stack != 0x12340080u ||
        transaction->guest_frame.w_ax != 0x1234u)
        return -1;
    ++state->calls;
    transaction->ax = 0x1357u;
    transaction->dx = 0x2468u;
    return 0;
}

int main(void)
{
    session instance;
    fixture_state state;
    uint8_t parameters[] = { 0x12u, 0x34u, 0x56u };
    uint32_t value;

    memset(&state, 0, sizeof(state));
    state.expected_stack = 0x1234005au;
    state.memory[0x123c0u] = 0x42u;
    state.memory[0x123c1u] = 0x00u;
    state.memory[0x123c4u] = 0xefu;
    state.memory[0x123c5u] = 0xbeu;
    session_initialize(&instance, 291u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &state, read_memory, write_memory) ||
        !session_thread_bind(&instance) ||
        !session_register_control_route(&instance,
            MVDM_WOW_CALLBACK_CONTROL_OPERATION, dispatch_callback, &state) ||
        !mvdm_wow_task_frame_bind(0x12340080u, 0x12340080u, 0u, 0x0042u) ||
        !mvdm_wow_callback_transaction_invoke(7u, 0xabcd1234u, parameters,
            sizeof(parameters), &value) || value != 0x24681357u ||
        state.calls != 1u || state.memory[0x1239au] != 0x42u ||
        state.memory[0x1239au + 2u] != 7u ||
        state.memory[0x1239au + 30u] != 0x57u ||
        state.memory[0x1239au + 31u] != 0x13u ||
        state.memory[0x1239au + 32u] != 0x68u ||
        state.memory[0x1239au + 33u] != 0x24u ||
        mvdm_wow_callback_transaction_invoke(7u, 0u, parameters,
            MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM + 1u, &value)) return 1;
    mvdm_wow_task_frame_clear();
    if (!session_thread_unbind(&instance)) return 2;
    session_guest_memory_end(&instance);
    return session_dispose(&instance) ? 0 : 3;
}
