#include <assert.h>
#include <stdint.h>

#include "r2_monitor.h"

static int record_bop(void *context, uint8_t selector)
{
    *(uint8_t *)context = selector;
    return 1;
}

static uint8_t bios_selector;

static void record_bios_50(void)
{
    bios_selector = 0x50;
}

int main(void)
{
    uint8_t bytes[] = {0xd6, 0x50};
    uint8_t exit_bytes[] = {0xd6, 0xfe};
    uint8_t c4_bytes[] = {0xc4, 0xc4, 0x50};
    uint8_t c4_extended[] = {0xc4, 0xc5, 0x50, 0x12};
    uint8_t c4_fast_bop[] = {0xc4, 0xc5, 0xfe, 0x03};
    uint8_t nop_then_bop[] = {0x90, 0xd6, 0x50};
    uint8_t selector = 0;
    void (*bios[256])(void) = {0};
    r2_bios_table_adapter adapter;
    r2_monitor_state state = {bytes, sizeof(bytes), 0, record_bop, &selector};

    assert(r2_monitor_step_d6_bop(&state) == R2_MONITOR_BOP_DISPATCHED);
    assert(selector == 0x50 && state.eip == 2);
    state.ram = exit_bytes;
    state.eip = 0;
    assert(r2_monitor_step_d6_bop(&state) == R2_MONITOR_BOP_EXIT && state.eip == 2);
    state.ram_size = 1;
    state.eip = 0;
    assert(r2_monitor_step_d6_bop(&state) == R2_MONITOR_MEMORY_FAULT);

    state.ram = c4_bytes;
    state.ram_size = sizeof(c4_bytes);
    state.eip = 0;
    selector = 0;
    assert(r2_monitor_step_bop(&state) == R2_MONITOR_BOP_DISPATCHED);
    assert(selector == 0x50 && state.eip == 3);

    state.ram = c4_extended;
    state.ram_size = sizeof(c4_extended);
    state.eip = 0;
    selector = 0;
    assert(r2_monitor_step_bop(&state) == R2_MONITOR_BOP_DISPATCHED);
    assert(selector == 0x50 && state.eip == 4);

    state.ram = c4_fast_bop;
    state.ram_size = sizeof(c4_fast_bop);
    state.eip = 0;
    assert(r2_monitor_step_bop(&state) == R2_MONITOR_UNSUPPORTED_INSTRUCTION);
    assert(state.eip == 4);

    bios[0x50] = record_bios_50;
    adapter.bios = bios;
    state.ram = bytes;
    state.ram_size = sizeof(bytes);
    state.eip = 0;
    state.bop = r2_monitor_call_bios_table;
    state.bop_context = &adapter;
    bios_selector = 0;
    assert(r2_monitor_step_bop(&state) == R2_MONITOR_BOP_DISPATCHED);
    assert(bios_selector == 0x50);

    bios[0x50] = 0;
    state.eip = 0;
    assert(r2_monitor_step_bop(&state) == R2_MONITOR_BOP_UNBOUND);

    state.ram = nop_then_bop;
    state.ram_size = sizeof(nop_then_bop);
    state.eip = 0;
    bios[0x50] = record_bios_50;
    bios_selector = 0;
    assert(r2_monitor_run(&state, 2) == R2_MONITOR_BOP_DISPATCHED);
    assert(state.eip == 3 && bios_selector == 0x50);

    state.eip = 0;
    assert(r2_monitor_run(&state, 1) == R2_MONITOR_BUDGET_EXHAUSTED);
    assert(state.eip == 1);
    return 0;
}
