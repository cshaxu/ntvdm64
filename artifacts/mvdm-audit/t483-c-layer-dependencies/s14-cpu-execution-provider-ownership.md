# CPU and execution provider ownership

## S14 P46 source review

Baseline 35f139db1. Complete body reads of six files below, followed by
selected original consumers/providers. This is implementation design evidence,
not a migration, a mirror restoration or new runtime acceptance.

| File under src/ntvdm-exe | Concrete disposition and proof |
| --- | --- |
| softpc/mvdm_softpc_descriptor_fields.c | retrieve_descr_fields is a C algorithm adapter, not B host mechanics. tape_io.c INT15 move-block calls it; c_bsic.c read_descriptor_linear already decodes through spr_read_dword and can fault. Candidate owner is that existing C descriptor unit. Preserve narrow output stores and fault behavior; do not invent a host descriptor table. |
| softpc/mvdm_softpc_effective_address.c | Selector-table lookup and real/v86 addressing belong C. c_bsic.c owns selector_outside_GDT_LDT and descriptor decoding. Candidate consolidation is that original CCPU mechanism, preserving the historical c_effective_addr/effective_addr entry shapes. This computes linear addresses, not a full data access check: it currently does not enforce segment access/limit or convert page faults to false. Do not silently strengthen its contract during relocation. |
| softpc/mvdm_keyboard_reset.c | Move finite IRQ cancellation into the C keyboard reset chain, subject to measured mirror delta. keyba.c captures user-originated output before emptying queues; nt_eoi.c host_DelayHwInterrupt cancels its quick event, then system/ica.c clears pending count/line. Both providers already belong C. Preserve captured pending discrimination and order; controller responses must not be canceled indiscriminately. |
| softpc/mvdm_softpc_execution.c | Split production verbose storage from fixture execution orchestration. Original support/main.c initializes verbose, so this file is not wholly test-only. run_until_return explicitly manages ACTIVE session admission, thread binding, guest-memory scope and termination escape and stays outer. Do not restore the entire retired xt.c or move the fixture state machine into B. Placement of the small production storage requires reconciliation with approved xt.c extraction. |
| softpc/mvdm_softpc_termination.c | Diagnostic formatting is independent host mechanics, but terminate_current_session closes original host resources then completes the product session via longjmp. Keep resource close C-owned and completion/escape outer-owned. sim32.c host_sas_init failure is an actual C caller; converting it to a returned initialization failure must be proven through its callers before removing the outward escape. A B forwarding callback would not solve ownership. |
| win32/ccpu_thread_exit_compat.c | Wrapper calls B opennt_prepare_thread_exit before C ccpu386exitthread. The B helper invokes installed lifecycle.finish, so moving this wrapper alone cannot eliminate outward implementation dependency. ntthread.c remains CPU stack/list release owner; product thread retirement must be placed at outer thread orchestration with equivalent ordering and no duplicate release. |

## Concrete checks and remaining proofs

The build generator redirects nt_thred.c's ccpu386exitthread call to the F
wrapper. c_bsic.c uses original GDT/LDT bounds and null-selector handling;
read_descriptor_linear retains guest paging/fault semantics. keyba.c calls the
reset helper after device queues are cleared; cancellation does not poll.
Session termination requires current, armed ACTIVE owner, sets COMPLETED and
registration_open=false before longjmp. These responsibilities cannot be
classified as a generic memory resource just because they share a session.

Recommended order after the complete inventory: descriptor/address owner,
keyboard cancellation, production storage separation, then initialization and
thread-exit orchestration. First three have concrete existing C carriers or
storage decisions; the last two require caller-return/cleanup proof. No new
mirror insertion is approved by this record. Before production edits, measure
non-error points/lines and obtain approval for net increases. Verify address
faults and legacy widths, reset pending-key behavior, startup failure and exact
thread cleanup ordering on final code. Existing tests have not been rerun.

Six complete-body reads cover seven currently open symbol rows. Original
provider snippets are supporting evidence, not whole-file audit counts.
