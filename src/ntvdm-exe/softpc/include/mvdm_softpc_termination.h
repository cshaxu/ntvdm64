#ifndef MVDM_SOFTPC_TERMINATION_H
#define MVDM_SOFTPC_TERMINATION_H

#include <stdint.h>

/* Same-shaped replacement for the selected NT4 product-shell terminal path.
 * It never terminates the application process: a bound session receives the
 * original VDM exit code and escapes to its app-owned execution boundary. */
int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code);

/* Default-off test attribution for an existing original TerminateVDM path. */
void mvdm_softpc_report_termination(char const *stage,
    void const *return_address);
void mvdm_softpc_report_virtual_memory_failure(unsigned long status);
void mvdm_softpc_report_wow_bop_state(unsigned long cr0, unsigned long cr3,
    unsigned short cs, unsigned short ip, unsigned long ds_base,
    unsigned long ds_limit);
/* Default-off post-dispatch observation.  It is emitted only after the
 * original W32Dispatch returns to its BOP owner. */
void mvdm_softpc_report_wow_dispatch_return_state(unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip);
void mvdm_softpc_report_wow_stage(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip);
/* Default-off witness for the original WOW_x86 fast-selector precondition.
 * It records the already-computed callback/flat-array carrier only. */
void mvdm_softpc_report_wow_fast_callback_binding(char const *stage,
    unsigned long address);
void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit);
/* Default-off descriptor-publication witness for the bounded WOW LDT alias
 * investigation.  The caller supplies records it has already published; the
 * witness does not inspect or modify guest state. */
void mvdm_softpc_report_dpmi_descriptor(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high);
/* Default-off witness for the original DOSX PassTableAddress publication.
 * It records only already-selected table carrier addresses, before and after
 * the CCPU binding has run; it does not copy, select, or alter either table. */
void mvdm_softpc_report_dpmi_table_address(char const *stage,
    unsigned long source_address, unsigned long gdt_shadow_address,
    unsigned long ldt_shadow_address);
/* Default-off observation of an already completed CPU40 descriptor
 * publication.  The selector filter shares the callback trace's explicit
 * target selector; no descriptor is read or changed for this witness. */
void mvdm_softpc_report_cpu40_descriptor_publish(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high);
/* Default-off witness for the original 53:00 descriptor-array conversion.
 * It captures only the BOP's existing register inputs and converted address,
 * before the original function reads the first descriptor. */
void mvdm_softpc_report_dpmi_set_descriptor(unsigned short es,
    unsigned short bx, unsigned short ax, unsigned short cx,
    unsigned long descriptor_pointer, unsigned long protected_mode);
/* Default-off witness at the entry to the original descriptor publisher.
 * Unlike the completed-conversion witness above, this preserves visibility
 * of a malformed selector that the original body rejects before converting
 * ES:BX. */
void mvdm_softpc_report_dpmi_set_descriptor_entry(unsigned short ax,
    unsigned short bx, unsigned short cx);
/* Default-off dispatch witness for the original DPMI BOP boundary.  It
 * records the already-decoded minor function and register inputs only. */
void mvdm_softpc_report_dpmi_bop(unsigned long index, unsigned short ax,
    unsigned short bx, unsigned short cx);
/* Default-off witness for DPMI BOP 53:03.  CPU40's original RISC provider
 * rejects FastBop; this reports the already-selected output registers only. */
void mvdm_softpc_report_dpmi_fast_bop_result(unsigned short bx,
    unsigned short dx, unsigned short es);
/* Default-off witness for the reached original INSIGNIA.386 control BOP.
 * It records pre-existing register scalars before virtual_device_trap
 * selects its source-defined control action. */
void mvdm_softpc_report_virtual_device_control(unsigned long eax,
    unsigned short bx, unsigned short cx, unsigned short dx);
/* Default-off witness for an already-entered software interrupt.  It is
 * intentionally observation-only while the WOW DPMI publication boundary is
 * being identified. */
void mvdm_softpc_report_dpmi_swint(unsigned long interrupt_number,
    unsigned short ax, unsigned short bx, unsigned short cx,
    unsigned short caller_cs, unsigned long caller_eip);
void mvdm_softpc_report_dpmi_swint_target(unsigned long interrupt_number,
    unsigned short handler_cs, unsigned long handler_eip,
    unsigned short handler_flags, unsigned char const *code,
    unsigned long code_size);
void mvdm_softpc_report_wow_allocsel_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short bx,
    unsigned short cx, unsigned short dx, unsigned short bp,
    unsigned long eflags);
/* Default-off execution witness for DOSX's original 04F2h WOW descriptor
 * service.  It observes the guest registers at instruction boundaries only;
 * the service remains the original DOSX implementation. */
void mvdm_softpc_report_wow_setdescriptor_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short bx,
    unsigned short cx, unsigned short dx, unsigned short bp,
    unsigned long eflags);
/* Default-off entry/return witness for KRNL386's original get_sel allocator.
 * It identifies whether the 16-bit loader actually allocates the selector it
 * later publishes as a procedure pointer; it does not participate in that
 * allocation. */
void mvdm_softpc_report_wow_getsel_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short cx,
    unsigned short si, unsigned short ds, unsigned long eflags);
/* Default-off instruction-window witness for the original KRNL386
 * LongPtrAddWOW entry.  This exposes the original reference selector only
 * after its own stack shuffles have placed it in DX; it does not inspect the
 * guest stack or modify the loader. */
void mvdm_softpc_report_wow_longptradd_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short bx,
    unsigned short cx, unsigned short dx, unsigned short ss,
    unsigned long esp);
/* Default-off observation at the return edge of the original 16-bit DPMI
 * interrupt frame.  It records the already-restored service result only for
 * the caller-selected WOW private service; it does not alter the frame. */
void mvdm_softpc_report_dpmi_iret16(unsigned short frame_bits,
    unsigned short service_ax, unsigned short result_ax,
    unsigned short result_bx, unsigned short continuation_cs,
    unsigned long continuation_eip, unsigned long eflags);
void mvdm_softpc_report_dpmi_iret16_state(unsigned short frame_bits,
    unsigned short ax, unsigned short bx, unsigned short cx,
    unsigned short dx, unsigned short si, unsigned short di,
    unsigned short bp, unsigned short ds, unsigned short es,
    unsigned short ss, unsigned long sp, unsigned short return_ip,
    unsigned short return_cs, unsigned short return_flags);
/* Default-off CCPU code-segment load witness for the WOW callback frontier.
 * The original CCPU owner supplies its already-decoded cache values; this
 * adapter only emits them when the test explicitly selects a selector. */
void mvdm_softpc_report_wow_code_segment_load(unsigned short selector,
    unsigned long base, unsigned long limit, unsigned long access_rights);
/* Default-off witness immediately before the original protected-mode far-call
 * validator.  It distinguishes a missing guest call from a rejected target;
 * it neither validates nor changes the decoded pointer. */
void mvdm_softpc_report_wow_far_call_attempt(unsigned short selector,
    unsigned long offset, unsigned short caller_cs, unsigned long caller_eip,
    unsigned short ldt_selector, unsigned long ldt_base,
    unsigned long ldt_limit);
/* Default-off witness after the original descriptor location is accepted.
 * It exports only values that the original CCPU validator already decoded. */
void mvdm_softpc_report_wow_far_call_validation(unsigned short selector,
    unsigned long descriptor_address, unsigned long access_rights,
    unsigned long current_privilege);
/* Default-off comparison of the original DOSX source LDT and the separate
 * CPU40 LDT carrier at an already-validated far-call target.  It only copies
 * the two descriptor records for diagnosis; neither table is modified. */
void mvdm_softpc_report_wow_descriptor_domains(unsigned short selector,
    unsigned long descriptor_address, unsigned long ldt_base,
    unsigned long ldt_limit, unsigned short ldt_selector,
    unsigned long descriptor_low, unsigned long descriptor_high);
/* Default-off proof of the actual CCPU page-table translation for an already
 * validated WOW far-call target.  It observes the source descriptor and does
 * not alter descriptors, paging, the TLB, or instruction execution. */
void mvdm_softpc_report_wow_fetch_translation(unsigned short selector,
    unsigned long offset, unsigned long linear, unsigned long physical,
    unsigned long mapped);
/* Default-off witness for the original WOW_x86 INT 2Ah fast LDT publication
 * protocol.  It records the pre-existing selector and two descriptor words
 * before ordinary CCPU software-interrupt dispatch continues. */
void mvdm_softpc_report_wow_int2a_ldt(unsigned short selector,
    unsigned long descriptor_low, unsigned long descriptor_high);
void mvdm_softpc_report_wow_int2a(unsigned long eax, unsigned long ebx,
    unsigned long ecx, unsigned long edx, unsigned long ebp);
/* Default-off witness for CCPU's existing protected-mode exception hook.
 * It reports only whether the already-installed original DPMI handler took
 * the exception; it neither installs a handler nor changes its result. */
void mvdm_softpc_report_wow_exception_hook(unsigned long exception_number,
    unsigned long error_code, unsigned long handler_installed,
    unsigned long handled);
/* Default-off witness for the original DPMI fault hand-off.  The handler and
 * register values are sampled after the original stack/frame construction;
 * this adapter never selects a handler or changes the transfer. */
void mvdm_softpc_report_dpmi_fault_dispatch(unsigned long exception_number,
    unsigned long error_code, unsigned short handler_cs, unsigned long handler_eip,
    unsigned short resulting_cs, unsigned long resulting_eip,
    unsigned short resulting_ss, unsigned long resulting_esp,
    unsigned long frame32, unsigned short source_cs, unsigned long source_eip);
/* Default-off witness for the original DPMI locked-stack frame.  The caller
 * copies words which the original code has already constructed; this helper
 * neither dereferences guest memory nor changes the fault return protocol. */
void mvdm_softpc_report_dpmi_fault_stack(char const *stage,
    unsigned short saved_ss, unsigned long saved_esp,
    unsigned short active_ss, unsigned long active_esp,
    unsigned short const *words, unsigned long word_count);
/* Default-off witness for writes to the original DOSX descriptor source entry
 * selected by the existing WOW callback trace filter.  It observes only the
 * store presented by CCPU; it never publishes, translates, or changes it. */
void mvdm_softpc_report_wow_source_descriptor_store(unsigned long address,
    unsigned long value, unsigned long width, unsigned short cs,
    unsigned long eip, unsigned short ss, unsigned long esp);

void mvdm_softpc_report_nt_transition(unsigned short cs, unsigned long ip,
    unsigned long flags, unsigned short ss, unsigned long sp);

#endif
