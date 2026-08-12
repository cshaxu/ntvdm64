#ifndef BX_NTVDM_CPU_STATE_ABI_H
#define BX_NTVDM_CPU_STATE_ABI_H

#include <stdint.h>

#define BX_NTVDM_CPU_STATE_ABI_MAGIC 0x42584353u
#define BX_NTVDM_CPU_STATE_ABI_VERSION 1u

enum bx_ntvdm_cpu_execution_mode {
    BX_NTVDM_CPU_EXECUTION_REAL = 1u,
    BX_NTVDM_CPU_EXECUTION_PROTECTED = 2u,
    BX_NTVDM_CPU_EXECUTION_V8086 = 3u
};

/* Copied x86 state at one declared instruction boundary. Selectors are values
 * only: descriptor caches, page tables, mappings and host pointers remain
 * native-backend owned. */
typedef struct bx_ntvdm_cpu_state_v1 {
    uint32_t magic, abi_version, struct_bytes, execution_mode;
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
    uint16_t cs, ds, es, ss, fs, gs;
    uint32_t reserved0;
} bx_ntvdm_cpu_state_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_cpu_state_v1_initialize(bx_ntvdm_cpu_state_v1 *state,
    uint32_t execution_mode);
int bx_ntvdm_cpu_state_v1_valid(const bx_ntvdm_cpu_state_v1 *state);
#ifdef __cplusplus
}
#endif

#endif
