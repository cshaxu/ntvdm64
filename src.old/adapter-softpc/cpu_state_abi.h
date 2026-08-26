#ifndef RUNTIME_CPU_STATE_ABI_H
#define RUNTIME_CPU_STATE_ABI_H

#include <stdint.h>

#define RUNTIME_CPU_STATE_ABI_MAGIC 0x42584353u
#define RUNTIME_CPU_STATE_ABI_VERSION 1u

enum runtime_cpu_execution_mode {
    RUNTIME_CPU_EXECUTION_REAL = 1u,
    RUNTIME_CPU_EXECUTION_PROTECTED = 2u,
    RUNTIME_CPU_EXECUTION_V8086 = 3u
};

/* Copied x86 state at one declared instruction boundary. Selectors are values
 * only: descriptor caches, page tables, mappings and host pointers remain
 * native-backend owned. */
typedef struct runtime_cpu_state {
    uint32_t magic, abi_version, struct_bytes, execution_mode;
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
    uint16_t cs, ds, es, ss, fs, gs;
    uint32_t reserved0;
} runtime_cpu_state;

#ifdef __cplusplus
extern "C" {
#endif
void runtime_cpu_state_initialize(runtime_cpu_state *state,
    uint32_t execution_mode);
int runtime_cpu_state_valid(const runtime_cpu_state *state);
#ifdef __cplusplus
}
#endif

#endif
