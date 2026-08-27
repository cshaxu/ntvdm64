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

/* Selector-blind protected-machine records.  They intentionally carry only
 * copied architectural scalars.  They do not identify a DPMI service, an
 * OpenNT provider, a guest object, or a Bochs object. */
#define MACHINE_FACADE_PROTECTED_FRAME_VERSION 1u
#define MACHINE_FACADE_PROTECTED_SEGMENT_VERSION 1u
#define MACHINE_FACADE_PROTECTED_EFLAGS_WRITE_MASK 0x003f7fd5u

enum machine_facade_execution_mode {
    MACHINE_FACADE_EXECUTION_MODE_REAL = 1u,
    MACHINE_FACADE_EXECUTION_MODE_PROTECTED = 2u,
    MACHINE_FACADE_EXECUTION_MODE_V8086 = 3u
};

enum machine_facade_protected_frame_status {
    MACHINE_FACADE_PROTECTED_FRAME_OK = 0,
    MACHINE_FACADE_PROTECTED_FRAME_REJECTED_INPUT,
    MACHINE_FACADE_PROTECTED_FRAME_REJECTED_MODE,
    MACHINE_FACADE_PROTECTED_FRAME_REJECTED_STALE,
    MACHINE_FACADE_PROTECTED_FRAME_REJECTED_CHANGE
};

/* The order of general registers is explicit instead of relying on Bochs'
 * private register-index order.  A caller may only submit a candidate based
 * on a freshly copied frame while adapter-bochs owns a returned CPU loop; it
 * cannot alter CR0 or a segment selector. */
struct machine_facade_protected_frame {
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t execution_mode;
    uint32_t cr0;
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp;
    uint32_t eip, eflags;
    uint16_t cs, ds, es, ss, fs, gs;
    uint32_t reserved0;
};

/* A copied active segment descriptor.  It is inspection-only in S3: LDT/IDT
 * mutation remains a later, source-proven owner decision. */
struct machine_facade_protected_segment {
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t slot;
    uint32_t base;
    uint32_t limit;
    uint32_t access;
    uint16_t selector;
    uint16_t reserved0;
};

void machine_facade_protected_frame_clear(
    struct machine_facade_protected_frame *frame);
int machine_facade_protected_frame_valid(
    const struct machine_facade_protected_frame *frame);
int machine_facade_copy_protected_frame(
    struct machine_facade_protected_frame *frame);
int machine_facade_commit_protected_frame(
    const struct machine_facade_protected_frame *expected,
    const struct machine_facade_protected_frame *candidate);
int machine_facade_copy_protected_segment(uint32_t slot,
    struct machine_facade_protected_segment *segment);
int machine_facade_protected_span_transfer(uint32_t kind,
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
/* Source-shaped 16-bit monitor register primitives.  They expose no BOP,
 * OpenNT, DOS or VDM semantics; adapter-mvdm-host-out/softpc owns that compatibility ABI. */
int machine_facade_copy_ax16(uint16_t *value);
int machine_facade_copy_bx16(uint16_t *value);
int machine_facade_copy_cx16(uint16_t *value);
int machine_facade_copy_dx16(uint16_t *value);
int machine_facade_copy_bp16(uint16_t *value);
int machine_facade_copy_sp16(uint16_t *value);
int machine_facade_copy_di16(uint16_t *value);
int machine_facade_copy_ip16(uint16_t *value);
int machine_facade_copy_cs16(uint16_t *value);
int machine_facade_copy_ss16(uint16_t *value);
int machine_facade_copy_si16(uint16_t *value);
int machine_facade_copy_ds16(uint16_t *value);
int machine_facade_copy_es16(uint16_t *value);
int machine_facade_copy_fs16(uint16_t *value);
int machine_facade_copy_gs16(uint16_t *value);
int machine_facade_copy_msw16(uint16_t *value);
int machine_facade_copy_eax32(uint32_t *value);
int machine_facade_copy_ebx32(uint32_t *value);
int machine_facade_copy_ecx32(uint32_t *value);
int machine_facade_copy_edx32(uint32_t *value);
int machine_facade_copy_esi32(uint32_t *value);
int machine_facade_copy_edi32(uint32_t *value);
int machine_facade_copy_ebp32(uint32_t *value);
int machine_facade_copy_esp32(uint32_t *value);
int machine_facade_copy_eip32(uint32_t *value);
int machine_facade_copy_al8(uint8_t *value);
int machine_facade_set_al8(uint8_t value);
int machine_facade_set_bl8(uint8_t value);
int machine_facade_set_ax16(uint16_t value);
int machine_facade_set_bx16(uint16_t value);
int machine_facade_set_dx16(uint16_t value);
int machine_facade_set_cx16(uint16_t value);
int machine_facade_set_bp16(uint16_t value);
int machine_facade_set_sp16(uint16_t value);
int machine_facade_set_si16(uint16_t value);
int machine_facade_set_di16(uint16_t value);
int machine_facade_set_ip16(uint16_t value);
int machine_facade_set_eax32(uint32_t value);
int machine_facade_set_ebx32(uint32_t value);
int machine_facade_set_ecx32(uint32_t value);
int machine_facade_set_edx32(uint32_t value);
int machine_facade_set_esi32(uint32_t value);
int machine_facade_set_edi32(uint32_t value);
int machine_facade_set_ebp32(uint32_t value);
int machine_facade_set_esp32(uint32_t value);
int machine_facade_set_eip32(uint32_t value);
int machine_facade_set_carry(uint32_t value);
void machine_facade_request_cpu_stop(void);
/* Clears only a previous adapter-owned finite-run stop request.  This does
 * not alter CPU registers, guest memory, devices, or pending interrupts. */
int machine_facade_prepare_cpu_resume(void);
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
