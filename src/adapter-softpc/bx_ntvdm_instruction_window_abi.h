#ifndef BX_NTVDM_INSTRUCTION_WINDOW_ABI_H
#define BX_NTVDM_INSTRUCTION_WINDOW_ABI_H

#include <stdint.h>

#define BX_NTVDM_INSTRUCTION_WINDOW_ABI_MAGIC 0x42584957u
#define BX_NTVDM_INSTRUCTION_WINDOW_ABI_VERSION 1u
#define BX_NTVDM_INSTRUCTION_WINDOW_V1_MAX_BYTES 15u

/* A copied, bounded observation at a faulting instruction address. It never
 * contains a guest address, host pointer, or decoded instruction object. */
typedef struct bx_ntvdm_instruction_window_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t valid_bytes;
    uint8_t bytes[BX_NTVDM_INSTRUCTION_WINDOW_V1_MAX_BYTES];
    uint8_t reserved0;
} bx_ntvdm_instruction_window_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_instruction_window_v1_capture(
    bx_ntvdm_instruction_window_v1 *window, const uint8_t *bytes,
    uint32_t valid_bytes);
int bx_ntvdm_instruction_window_v1_valid(
    const bx_ntvdm_instruction_window_v1 *window);

#ifdef __cplusplus
}
#endif

#endif
