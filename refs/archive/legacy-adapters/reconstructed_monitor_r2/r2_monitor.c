#include "r2_monitor.h"

static int r2_can_read(const r2_monitor_state *state, uint32_t address, size_t size)
{
    return state != NULL && state->ram != NULL && address <= state->ram_size &&
           size <= state->ram_size - address;
}

r2_monitor_result r2_monitor_step_d6_bop(r2_monitor_state *state)
{
    uint8_t selector;

    if (!r2_can_read(state, state->eip, 2)) {
        return R2_MONITOR_MEMORY_FAULT;
    }

    if (state->ram[state->eip] != 0xd6) {
        return R2_MONITOR_UNSUPPORTED_INSTRUCTION;
    }

    selector = state->ram[state->eip + 1];
    state->eip += 2;

    if (selector == 0xfe) {
        return R2_MONITOR_BOP_EXIT;
    }

    if (state->bop == NULL) {
        return R2_MONITOR_MEMORY_FAULT;
    }

    if (!state->bop(state->bop_context, selector)) {
        return R2_MONITOR_BOP_UNBOUND;
    }
    return R2_MONITOR_BOP_DISPATCHED;
}

r2_monitor_result r2_monitor_step_bop(r2_monitor_state *state)
{
    uint8_t selector;
    uint8_t modrm;
    uint32_t immediate;
    size_t extra_bytes;
    size_t index;

    if (!r2_can_read(state, state->eip, 1)) {
        return R2_MONITOR_MEMORY_FAULT;
    }

    if (state->ram[state->eip] == 0xd6) {
        return r2_monitor_step_d6_bop(state);
    }

    if (!r2_can_read(state, state->eip, 2)) {
        return R2_MONITOR_MEMORY_FAULT;
    }

    if (state->ram[state->eip] != 0xc4) {
        return R2_MONITOR_UNSUPPORTED_INSTRUCTION;
    }

    modrm = state->ram[state->eip + 1];
    if ((modrm & 0xfc) != 0xc4) {
        return R2_MONITOR_UNSUPPORTED_INSTRUCTION;
    }

    extra_bytes = modrm & 3;
    if (!r2_can_read(state, state->eip, 3 + extra_bytes)) {
        return R2_MONITOR_MEMORY_FAULT;
    }
    selector = state->ram[state->eip + 2];
    immediate = selector;
    for (index = 0; index < extra_bytes; ++index) {
        immediate |= (uint32_t)state->ram[state->eip + 3 + index] << (8 * (index + 1));
    }
    state->eip += (uint32_t)(3 + extra_bytes);
    if (immediate == 0xfe) {
        return R2_MONITOR_BOP_EXIT;
    }
    if ((immediate & 0xff) == 0xfe) {
        return R2_MONITOR_UNSUPPORTED_INSTRUCTION;
    }
    if (state->bop == NULL) {
        return R2_MONITOR_MEMORY_FAULT;
    }
    if (!state->bop(state->bop_context, selector)) {
        return R2_MONITOR_BOP_UNBOUND;
    }
    return R2_MONITOR_BOP_DISPATCHED;
}

int r2_monitor_call_bios_table(void *context, uint8_t selector)
{
    r2_bios_table_adapter *adapter = context;

    if (adapter != NULL && adapter->bios != NULL && adapter->bios[selector] != NULL) {
        adapter->bios[selector]();
        return 1;
    }
    return 0;
}

r2_monitor_result r2_monitor_run(r2_monitor_state *state, size_t instruction_budget)
{
    while (instruction_budget-- != 0) {
        if (!r2_can_read(state, state->eip, 1)) {
            return R2_MONITOR_MEMORY_FAULT;
        }
        if (state->ram[state->eip] == 0x90) {
            ++state->eip;
            continue;
        }
        return r2_monitor_step_bop(state);
    }
    return R2_MONITOR_BUDGET_EXHAUSTED;
}
