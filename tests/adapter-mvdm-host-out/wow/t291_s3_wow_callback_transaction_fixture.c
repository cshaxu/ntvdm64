#include "mvdm_wow_callback_transaction.h"

#include <string.h>

#include "adapter-mvdm-host-out/monitor/include/mvdm_wow_task_frame.h"
#include "session/session.h"

typedef struct fixture_state {
    uint32_t calls;
    uint32_t expected_stack;
} fixture_state;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    (void)context;
    (void)address;
    memset(bytes, 0, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t byte_count)
{
    (void)context;
    (void)address;
    (void)bytes;
    (void)byte_count;
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
        transaction->parameters[0] != 0x12u || transaction->parameters[2] != 0x56u)
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
    state.expected_stack = 0x12340020u;
    session_initialize(&instance, 291u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, NULL, read_memory, write_memory) ||
        !session_thread_bind(&instance) ||
        !session_register_control_route(&instance,
            MVDM_WOW_CALLBACK_CONTROL_OPERATION, dispatch_callback, &state) ||
        !mvdm_wow_task_frame_bind(0x12340080u, 0x12340080u, 0u, 0x0042u) ||
        !mvdm_wow_callback_transaction_invoke(7u, 0xabcd1234u, parameters,
            sizeof(parameters), &value) || value != 0x24681357u ||
        state.calls != 1u ||
        mvdm_wow_callback_transaction_invoke(7u, 0u, parameters,
            MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM + 1u, &value)) return 1;
    mvdm_wow_task_frame_clear();
    if (!session_thread_unbind(&instance)) return 2;
    session_guest_memory_end(&instance);
    return session_dispose(&instance) ? 0 : 3;
}
