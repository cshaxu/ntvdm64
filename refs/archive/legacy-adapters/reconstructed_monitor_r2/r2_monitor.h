#ifndef NTDOS64_R2_MONITOR_H
#define NTDOS64_R2_MONITOR_H

#include <stddef.h>
#include <stdint.h>

typedef enum r2_monitor_result {
    R2_MONITOR_BOP_DISPATCHED,
    R2_MONITOR_BOP_EXIT,
    R2_MONITOR_BOP_UNBOUND,
    R2_MONITOR_BUDGET_EXHAUSTED,
    R2_MONITOR_MEMORY_FAULT,
    R2_MONITOR_UNSUPPORTED_INSTRUCTION
} r2_monitor_result;

typedef int (*r2_monitor_bop_callback)(void *context, uint8_t selector);

typedef struct r2_monitor_state {
    uint8_t *ram;
    size_t ram_size;
    uint32_t eip;
    r2_monitor_bop_callback bop;
    void *bop_context;
} r2_monitor_state;

typedef struct r2_bios_table_adapter {
    void (**bios)(void);
} r2_bios_table_adapter;

r2_monitor_result r2_monitor_step_d6_bop(r2_monitor_state *state);
r2_monitor_result r2_monitor_step_bop(r2_monitor_state *state);
 r2_monitor_result r2_monitor_run(r2_monitor_state *state, size_t instruction_budget);
int r2_monitor_call_bios_table(void *context, uint8_t selector);

#endif
