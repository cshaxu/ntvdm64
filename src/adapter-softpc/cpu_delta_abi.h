#ifndef RUNTIME_CPU_DELTA_ABI_H
#define RUNTIME_CPU_DELTA_ABI_H

#include <stdint.h>

#define RUNTIME_CPU_DELTA_ABI_MAGIC 0x42584344u
#define RUNTIME_CPU_DELTA_ABI_VERSION 1u
#define RUNTIME_CPU_DELTA_V1_GPR16_COUNT 8u
#define RUNTIME_CPU_DELTA_V1_SEGMENT_COUNT 6u
#define RUNTIME_CPU_DELTA_V1_GPR16_MASK 0xffu

/* A deliberately narrow post-boundary result: v1 can replace only low
 * sixteen-bit GPR values. It never carries a CPU pointer and cannot alter
 * mode, segments, flags, memory or devices. */
typedef struct runtime_cpu_delta_v1 {
    uint32_t magic, abi_version, struct_bytes, gpr16_write_mask;
    uint16_t gpr16_values[RUNTIME_CPU_DELTA_V1_GPR16_COUNT];
    /* Mechanical real/protected-mode segment updates.  This does not carry
     * selector meaning; the CPU applies the typed value by its native loader. */
    uint32_t segment_write_mask;
    uint16_t segment_values[RUNTIME_CPU_DELTA_V1_SEGMENT_COUNT];
    uint32_t reserved0;
} runtime_cpu_delta_v1;

#ifdef __cplusplus
extern "C" {
#endif
void runtime_cpu_delta_v1_initialize(runtime_cpu_delta_v1 *delta);
int runtime_cpu_delta_v1_valid(const runtime_cpu_delta_v1 *delta);
int runtime_cpu_delta_v1_set_gpr16(runtime_cpu_delta_v1 *delta,
    uint32_t register_index, uint16_t value);
int runtime_cpu_delta_v1_set_segment(runtime_cpu_delta_v1 *delta,
    uint32_t segment_index, uint16_t value);
#ifdef __cplusplus
}
#endif

#endif
