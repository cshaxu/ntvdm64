#include "softpc_memory_size_shim.h"

#include "adapter-softpc/mechanical_action.h"

struct runtime_softpc_memory_size_call {
    uint16_t ax;
    uint32_t active;
    uint32_t failed;
};

/* BOP handling is synchronous.  This is a per-thread shim context, never a
 * guest pointer or a Bochs object, and it cannot outlive the bridge call. */
static __declspec(thread) struct runtime_softpc_memory_size_call g_call;

int runtime_softpc_memory_size_begin(uint16_t *result_ax)
{
    if (result_ax == 0 || g_call.active != 0u) return 0;
    g_call.ax = 0u;
    g_call.failed = 0u;
    g_call.active = 1u;
    return 1;
}

int runtime_softpc_memory_size_end(uint16_t *result_ax)
{
    if (result_ax == 0 || g_call.active == 0u) return 0;
    *result_ax = g_call.ax;
    g_call.active = 0u;
    return g_call.failed == 0u;
}

void runtime_softpc_memory_size_sas_loadw(uint32_t address, word *value)
{
    uint8_t bytes[2];
    if (g_call.active == 0u || value == 0 ||
        address != RUNTIME_SOFTPC_MEMORY_VAR ||
        !runtime_machine_checked_ram_read(address, bytes, sizeof(bytes))) {
        g_call.failed = 1u;
        return;
    }
    *value = (word)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

void runtime_softpc_memory_size_setAX(word value)
{
    if (g_call.active == 0u) {
        g_call.failed = 1u;
        return;
    }
    g_call.ax = value;
}
