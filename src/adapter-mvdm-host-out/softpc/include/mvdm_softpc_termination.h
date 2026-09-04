#ifndef MVDM_SOFTPC_TERMINATION_H
#define MVDM_SOFTPC_TERMINATION_H

#include <stdint.h>

struct _EXCEPTION_POINTERS;

/* Same-shaped replacement for the selected NT4 product-shell terminal path.
 * It never terminates the application process: a bound session receives the
 * original VDM exit code and escapes to its app-owned execution boundary. */
int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code);

/* Default-off diagnostic context for the existing selected termination
 * boundary.  The string is a static source-entry label only; it neither
 * changes the completion code nor escapes the current host thread. */
void mvdm_softpc_set_termination_origin(const char *origin);

/* Diagnostic-only companion to the original top-level VDM exception filter.
 * It records the original exception identity without changing the filter's
 * result, selected termination path, or guest/CPU state. */
void mvdm_softpc_record_unhandled_exception(
    const struct _EXCEPTION_POINTERS *exception_info);

/* Fixed-container diagnostic only.  It observes the original top-level main
 * return after host_main/SEH have already selected the result. */
void mvdm_softpc_record_main_return(int result);

/* Fixed-container diagnostic only. `selector` and `service` were already
 * decoded by original MVDM code. The four guest words are copied at that
 * ingress and are observational only; this function neither routes nor
 * changes guest state. */
void mvdm_softpc_record_bop_dispatch(unsigned int selector,
                                     unsigned int service,
                                     unsigned int guest_cs,
                                     unsigned int guest_ip,
                                     unsigned int guest_ds,
                                     unsigned int guest_dx);

/* Fixed-container diagnostic only.  Called after the original BOP handler
 * and its original service-byte IP advance; copies already-live CPU words. */
void mvdm_softpc_record_bop_return(unsigned int selector,
                                   unsigned int service,
                                   unsigned int guest_cs,
                                   unsigned int guest_ip,
                                   unsigned int guest_ax,
                                   unsigned int guest_cf,
                                   unsigned int guest_if);

/* Default-off fixed-container observation of the original CCPU BOP FE
 * unwind.  The CPU has already decoded and advanced past the instruction;
 * this copies only its live CS:IP immediately before the unchanged original
 * c_cpu_unsimulate transition. */
void mvdm_softpc_record_cpu_unsimulate(unsigned int guest_cs,
                                       unsigned int guest_ip);

/* Default-off fixed-container observation at the completed original CPU40
 * invocation boundary.  It copies only already-live CS:IP after CCPU has
 * selected its ordinary return path. */
void mvdm_softpc_record_cpu_simulate_return(unsigned int guest_cs,
                                            unsigned int guest_ip);

/* Default-off fixed-container observation at the original host text-output
 * boundary.  `count` is the already selected byte count; no guest text or
 * buffer pointer crosses the observation boundary. */
void mvdm_softpc_record_stream_io_update(const uint8_t *buffer,
    unsigned int count);
void mvdm_softpc_record_stream_io_result(unsigned int count,
    unsigned int wrote, unsigned int bytes_written, unsigned long error_code);

/* Default-off fixed-container observation of the original Console-to-8042
 * keyboard route.  Arguments are source-owned scalar state only. */
void mvdm_softpc_record_console_key(unsigned int scan_code,
                                    unsigned int key_down);
void mvdm_softpc_record_keyboard_offer(unsigned int scan_code,
                                       unsigned int accepted,
                                       unsigned int scanning_stopped,
                                       unsigned int keyboard_disabled,
                                       unsigned int queue_depth);
/* Default-off witness of original keyboard interrupt gating. `stage` is
 * source-owned: 1 is KbdResume and 2 is KbdIntDelay. */
void mvdm_softpc_record_keyboard_gate(unsigned int stage,
                                      unsigned int eoi_pending,
                                      unsigned int interrupts_enabled,
                                      unsigned int output_full,
                                      unsigned int keyboard_disabled);
/* Default-off scalar-only witnesses for the existing master PIC IRQ1 and
 * CPU40 hardware-interrupt handoff. */
void mvdm_softpc_record_keyboard_ica_irq(unsigned int adapter,
                                         unsigned int line);
void mvdm_softpc_record_keyboard_ica_already_high(unsigned int adapter,
                                                   unsigned int previous_line,
                                                   unsigned int requested_line);
void mvdm_softpc_record_keyboard_ica_request(unsigned int irr,
                                              unsigned int isr,
                                              unsigned int imr,
                                              unsigned int scan_result);
void mvdm_softpc_record_cpu_hw_interrupt(void);
/* Default-off CPU40 observation when an already-pending hardware interrupt
 * cannot yet be acknowledged because the guest IF flag is clear. */
void mvdm_softpc_record_cpu_hw_interrupt_deferred(unsigned int interrupts_enabled,
    unsigned int guest_cs, unsigned int guest_ip);
/* Default-off observation after the unchanged CPU40 interrupt acknowledge.
 * `vector` is the original PIC result; this function cannot alter the CPU,
 * PIC, BIOS or guest state. */
void mvdm_softpc_record_cpu_hw_interrupt_service(unsigned int vector);
/* Default-off observation at the original BIOS keyboard `AH == 2` waitio
 * edge.  It does not queue, read, or alter a key. */
void mvdm_softpc_record_keyboard_waitio(void);
/* Default-off latched observation at original BIOS keyboard `AH == 0/1`
 * polling edges.  It does not queue, read, or alter a key. */
void mvdm_softpc_record_keyboard_poll(void);
/* Default-off observation after the original PIC EOI selection.  It copies
 * only the original adapter/line result and cannot alter PIC, CPU, BIOS or
 * guest state. */
void mvdm_softpc_record_ica_eoi(unsigned int adapter, int line,
                                unsigned int cpu_interrupt_pending);
void mvdm_softpc_record_host_eoi_hook(int irq_line, int call_count,
                                      unsigned int registered);
void mvdm_softpc_record_keyboard_port_read(unsigned int value,
                                           unsigned int output_full);
void mvdm_softpc_record_keyboard_eoi_state(unsigned int bios_owns_hardware,
                                            unsigned int bios_buffer_space);
void mvdm_softpc_record_keyboard_delay(unsigned int stage,
                                       unsigned int delay_us,
                                       unsigned int delay_mask);
/* Default-off scalar observation of the source-owned 6805-to-8042 output
 * pump.  It is deliberately selector- and guest-data-blind. */
void mvdm_softpc_record_keyboard_pump(unsigned int stage,
                                      unsigned int eoi_pending,
                                      unsigned int output_full,
                                      unsigned int pending_8042,
                                      unsigned int queue_depth,
                                      unsigned int keyboard_disabled,
                                      unsigned int waiting_for_upcode);

/* Fixed-container diagnostic only.  `stage` identifies an already-selected
 * original COMMAND call boundary; it does not route or alter that call. */
void mvdm_softpc_record_command_call(unsigned int service,
                                    unsigned int stage,
                                    unsigned int guest_ax,
                                    unsigned int guest_cf);

/* Default-off, fixed-container continuation observation for the original
 * COMMAND GetNextCmd table entry.  All arguments are copied scalars at the
 * existing table boundary; no command bytes, record pointer or guest/native
 * address crosses into the observer. */
void mvdm_softpc_record_command_continuation(unsigned int stage,
    unsigned int guest_cs, unsigned int guest_ip, unsigned int guest_ax,
    unsigned int guest_bx, unsigned int guest_cf, unsigned int first_call,
    unsigned int repeat_call, uint32_t dos_record_state);

/* Default-off, scalar-only observation of the original cmdGetNextCmd
 * BaseClient request/result.  It distinguishes a command-record completion
 * from a command-returned DOS error without reading command bytes, retaining
 * a VDMINFO pointer, or changing the request. */
void mvdm_softpc_record_command_vdm_result(unsigned int stage,
    unsigned int error_code, unsigned int vdm_state, unsigned int succeeded,
    unsigned int first_call, unsigned int repeat_call);

/* Default-off, scalar-only observation of the original guest CMDINFO return
 * code when COMMAND re-enters cmdGetNextCmd. */
void mvdm_softpc_record_command_guest_return(unsigned int return_code,
    unsigned int first_call, unsigned int repeat_call);

/* Default-off, fixed-container observation of the original COMMAND
 * GetInitEnvironment table entry.  The caller supplies only already-live
 * scalar registers; the captured report path was removed from inherited host
 * environment before original COMMAND can copy it into guest memory. */
void mvdm_softpc_record_command_environment(unsigned int stage,
    unsigned int guest_es, unsigned int guest_bx, unsigned int guest_ax,
    unsigned int guest_cf, unsigned int guest_ds, unsigned int guest_ss,
    unsigned int guest_sp);

/* Default-off, selected-image observation of original COMMAND's resident
 * transient-entry table.  It reads the three original 16:16 entries through
 * short session leases after the existing environment table call; it never
 * retains a guest alias or changes guest/CPU/BOP state. */
void mvdm_softpc_record_command_stub_table(uint16_t guest_cs);

/* Default-off COMMAND environment-return observation.  It copies only a
 * bounded instruction window at the already-advanced BOP return location. */
void mvdm_softpc_record_command_environment_return_code(unsigned int guest_cs,
    unsigned int guest_ip);

/* App captures optional continuation and stream-I/O report paths before
 * original MVDM reads its inherited environment, then removes those
 * diagnostic variables. The retained paths are adapter-private and are never
 * made guest-visible. */
void mvdm_softpc_capture_command_continuation_report_path(void);

/* Default-off, fixed-container DEM observation.  The caller passes only the
 * original numeric DS:SI form; this helper takes and releases its own bounded
 * session lease before recording copied text and scalar outcome. */
void mvdm_softpc_record_dem_open(uint16_t guest_ds, uint16_t guest_si,
    unsigned int phase, unsigned int status, unsigned int guest_ax,
    unsigned int guest_cf);

void mvdm_softpc_record_dem_create(uint16_t guest_ds, uint16_t guest_si,
    unsigned int phase, unsigned int status, unsigned int guest_ax,
    unsigned int guest_cf);

/* Default-off original DEM read observation. It receives only already-live
 * scalar register/result values; no guest buffer, host handle or token escapes
 * the original handler. */
void mvdm_softpc_record_dem_read(uint16_t guest_ds, uint16_t guest_dx,
    uint16_t requested_bytes, uint16_t file_offset_high,
    uint16_t file_offset_low, uint16_t completed_bytes,
    unsigned int phase, unsigned int guest_ax, unsigned int guest_cf);

void mvdm_softpc_record_dem_write(uint16_t guest_ds, uint16_t guest_dx,
    uint16_t requested_bytes, uint16_t file_offset_high,
    uint16_t file_offset_low, unsigned int phase, unsigned int guest_ax,
    unsigned int guest_cf);

/* Default-off original DEM file-pointer observation. It receives only the
 * already-live DOS scalar contract; no host handle, guest pointer or payload
 * is retained by the observer. */
void mvdm_softpc_record_dem_seek(uint16_t requested_high,
    uint16_t requested_low, unsigned int origin, unsigned int phase,
    uint16_t result_high, uint16_t result_low, unsigned int guest_ax,
    unsigned int guest_cf);

/* Default-off, fixed-container CONFIG completion observation.  It copies the
 * exact selected NTDOS map's pass byte and 64-byte shell filename through
 * short read leases, then releases them before the original notification BOP
 * returns.  It neither routes nor modifies CONFIG, UMB, DEM, CPU or guest
 * state. */
void mvdm_softpc_record_config_done(uint16_t guest_cs);

/* Default-off, selected-image CPU40 SAS-write observation.  The original
 * caller supplies its already-calculated guest linear address, write width and
 * scalar value after the original store completed.  The helper records only a
 * bounded configured span; it neither translates, retains, nor writes guest
 * memory. */
void mvdm_softpc_record_sas_store(uint32_t guest_linear_address,
    uint32_t byte_count, uint32_t value);

/* Default-off CCPU SAS observation.  The original caller has already decided
 * that the normal RAM calculation is required.  This copies scalar lifecycle
 * state only; it neither retains nor exposes a guest/native pointer, changes
 * the mapping decision, or touches the memory being read. */
void mvdm_softpc_record_direct_ram_access(uint32_t requested_physical_address,
    uint32_t direct_physical_address, uint32_t wrap_mask,
    uint32_t m_area_length, uintptr_t m_area_base);

#endif
