#ifndef MACHINE_FACADE_H
#define MACHINE_FACADE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*machine_facade_timer_callback)(void *opaque);
typedef int (*machine_facade_opaque_callback)(void *context,
    const void *event, unsigned event_bytes, void *outcome, unsigned outcome_bytes);

int machine_facade_bind_opaque_callback(
    machine_facade_opaque_callback callback, void *context);
void machine_facade_unbind_opaque_callback(void);

int machine_facade_get_a20(uint32_t *enabled);
int machine_facade_set_a20(uint32_t enabled);
int machine_facade_memory_readable(uint64_t address, uint64_t byte_count);
int machine_facade_memory_writable(uint64_t address, uint64_t byte_count);
int machine_facade_memory_read(uint64_t address, uint64_t byte_count,
    uint8_t *bytes);
int machine_facade_memory_write(uint64_t address, uint64_t byte_count,
    const uint8_t *bytes);
enum machine_facade_protected_range_status {
    MACHINE_FACADE_PROTECTED_RANGE_OK = 0,
    MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MODE,
    MACHINE_FACADE_PROTECTED_RANGE_REJECTED_ACCESS,
    MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MEMORY
};
int machine_facade_execute_protected_range(uint32_t kind,
    uint32_t segment, uint32_t offset, uint32_t byte_count, uint8_t *bytes);
int machine_facade_machine_begin(uint64_t guest_bytes,
    uint64_t host_bytes);
int machine_facade_machine_cleanup(void);
int machine_facade_machine_active(void);
int machine_facade_set_realmode_segment_limit_compatibility(uint32_t enabled);
int machine_facade_compose_headless_8042(void);
void machine_facade_initialize_timing(uint32_t ips);
void machine_facade_apply_real_mode_entry(uint16_t cs, uint32_t eip);
int machine_facade_copy_real_mode_entry(uint16_t *cs, uint32_t *eip);
int machine_facade_copy_real_mode_state(uint16_t *cs, uint16_t *ss,
    uint16_t *sp, uint32_t *eip);
void machine_facade_request_cpu_stop(void);
void machine_facade_cpu_loop(void);
int machine_facade_register_timer(void *opaque,
    machine_facade_timer_callback callback, uint64_t ticks,
    uint32_t continuous, uint32_t active, uint32_t *timer_id);
void machine_facade_deactivate_timer(uint32_t timer_id);
void machine_facade_unregister_timer(uint32_t timer_id);

#ifdef __cplusplus
}
#endif

#endif
