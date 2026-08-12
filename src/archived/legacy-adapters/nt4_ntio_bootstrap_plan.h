#ifndef NT4_NTIO_BOOTSTRAP_PLAN_H
#define NT4_NTIO_BOOTSTRAP_PLAN_H

#include <stddef.h>
#include <stdint.h>

#define NT4_NTIO_LOAD_PHYSICAL 0x700u
#define NT4_NTIO_LOAD_CS 0x0070u
#define NT4_NTIO_LOAD_IP 0x0000u
#define NT4_NTIO_VIRTUAL_INTERRUPT_PHYSICAL 0x714u
#define NT4_NTIO_VIRTUAL_INTERRUPT_SIZE 4u

typedef enum nt4_ntio_bootstrap_plan_result {
    NT4_NTIO_BOOTSTRAP_PLAN_OK = 0,
    NT4_NTIO_BOOTSTRAP_PLAN_INVALID_ARGUMENT,
    NT4_NTIO_BOOTSTRAP_PLAN_IMAGE_TOO_SHORT,
    NT4_NTIO_BOOTSTRAP_PLAN_RAM_RANGE_REJECTED,
    NT4_NTIO_BOOTSTRAP_PLAN_NO_MEMORY
} nt4_ntio_bootstrap_plan_result;

typedef struct nt4_ntio_bootstrap_plan {
    uint8_t *preload_bytes;
    size_t preload_byte_count;
    uint32_t preload_physical;
    uint16_t entry_cs;
    uint16_t entry_ip;
} nt4_ntio_bootstrap_plan;

/* out_plan must be zero initialized or released before reuse. On failure it
 * remains unchanged. The supplied four bytes are the already-established
 * machine reset state that the original NT4 loader preserves across its copy. */
nt4_ntio_bootstrap_plan_result nt4_ntio_bootstrap_plan_build(
    const uint8_t *ntio_bytes,
    size_t ntio_byte_count,
    const uint8_t virtual_interrupt_state[NT4_NTIO_VIRTUAL_INTERRUPT_SIZE],
    size_t ordinary_ram_byte_count,
    nt4_ntio_bootstrap_plan *out_plan);
void nt4_ntio_bootstrap_plan_release(nt4_ntio_bootstrap_plan *plan);

#endif
