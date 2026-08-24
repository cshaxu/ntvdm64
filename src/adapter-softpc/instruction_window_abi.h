#ifndef RUNTIME_INSTRUCTION_WINDOW_ABI_H
#define RUNTIME_INSTRUCTION_WINDOW_ABI_H

#include <stdint.h>

#define RUNTIME_INSTRUCTION_WINDOW_ABI_MAGIC 0x42584957u
#define RUNTIME_INSTRUCTION_WINDOW_ABI_VERSION 1u
#define RUNTIME_INSTRUCTION_WINDOW_V1_MAX_BYTES 15u

/* A copied, bounded observation at a faulting instruction address. It never
 * contains a guest address, host pointer, or decoded instruction object. */
typedef struct runtime_instruction_window_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t valid_bytes;
    uint8_t bytes[RUNTIME_INSTRUCTION_WINDOW_V1_MAX_BYTES];
    uint8_t reserved0;
} runtime_instruction_window_v1;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_instruction_window_v1_capture(
    runtime_instruction_window_v1 *window, const uint8_t *bytes,
    uint32_t valid_bytes);
int runtime_instruction_window_v1_valid(
    const runtime_instruction_window_v1 *window);

#ifdef __cplusplus
}
#endif

#endif
