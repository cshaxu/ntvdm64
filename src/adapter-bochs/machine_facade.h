#ifndef MACHINE_FACADE_H
#define MACHINE_FACADE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int machine_facade_v1_get_a20(uint32_t *enabled);
int machine_facade_v1_set_a20(uint32_t enabled);
int machine_facade_v1_memory_readable(uint64_t address, uint64_t byte_count);
int machine_facade_v1_memory_writable(uint64_t address, uint64_t byte_count);
int machine_facade_v1_memory_read(uint64_t address, uint64_t byte_count,
    uint8_t *bytes);
int machine_facade_v1_memory_write(uint64_t address, uint64_t byte_count,
    uint8_t *bytes);

#ifdef __cplusplus
}
#endif

#endif
