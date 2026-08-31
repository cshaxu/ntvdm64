# M0 T318 S2 P33 — CPU40 first-fetch and dispatch owner audit

## Purpose

Close the original CPU40 `c_cpu_simulate -> ccpu(FALSE)` owner cluster before
running the fixed r30 container again.  This is a source audit, not an
in-product observer, BOP probe, or a replacement CPU path.

## Reached first-fetch contract

The selected `CPU_40_STYLE` build makes `cpu_simulate` an alias for the
original `c_cpu_simulate`.  The sequence is therefore:

1. `host_start_cpu` in `softpc.new/host/src/nt_cpu.c` calls that alias.
2. `c_cpu_simulate` obtains the current thread's original simulation frame
   with `ccpu386SimulatePtr`, saves it with `setjmp`, and calls `ccpu(FALSE)`.
3. `ccpu` installs its current-thread exception return frame, then enters
   `NEXT_INST`.
4. `SETUP_HOST_IP` calls original `usr_chk_byte(CS.base + EIP, PG_R)`.  With
   paging disabled during the NTIO real-mode handoff, `usr_chk_byte` returns
   that numeric linear/physical address unchanged.
5. The original CCPU fast path takes `&CCPU_M[address]`.  `CCPU_M` is the
   contiguous `Start_of_M_area` allocation established by `sas_init` and
   `InitIntelMemory`; it is neither a host-object surrogate nor a mapping ID.
6. The opcode decoder reads the byte, then returns to the same `NEXT_INST`
   cycle after original per-instruction event processing.

`c_GetPhyAdd` remains the source-shaped slow/physical pointer provider for
SAS and for page-aware accesses.  Its mapping-manager hook precedes the
unchanged contiguous-RAM return solely for registered external physical-page
bindings.  The direct first-fetch macro does not use that hook.

## Direct prerequisite disposition

- **CCPU thread frames:** `c_cpu_init` calls `ccpu386InitThreadStuff` before
  `InitialiseDosEmulation` and `host_start_cpu`.  The thread provider allocates
  TLS state for the main thread; both `ccpu386SimulatePtr` and
  `ccpu386ThrdExptnPtr` retain explicit original diagnostic/null failure
  paths.  The immutable r30 observation contains none of those diagnostics,
  so this audit does not attribute the escaped access violation to them.
- **Exceptions:** `ccpu` installs the original exception `setjmp` frame before
  `NEXT_INST`.  The outer process filter from P31 only reports an exception
  that escaped this CCPU boundary; it does not prove which CPU expression
  faulted.  No exception handler, register contract, or guest state change is
  admitted here.
- **Timer and hardware events:** `c_cpu_interrupt` only sets the original
  interrupt-map bits.  The dispatcher consumes `CPU_TIMER_TICK` through
  `host_timer_event` at the existing instruction boundary (and in the original
  HLT loop).  This happens after a completed instruction or a HLT wait; it is
  not required to form the first NTIO fetch.  Device/PIT work remains a later
  owner cluster unless fixed-container evidence reaches it.
- **C-VID/GDP:** C-VID setup was completed before
  `InitialiseDosEmulation`.  Its generated vector table is not dereferenced by
  the ordinary RAM first-fetch macro.  Video-memory access remains a separate
  source-defined controller/C-VID cluster, not a reason to alter first fetch.
- **Historical host register vectors:** `load_sw_cpu_access_functions` has no
  selected direct startup call.  CPU40's `cpu.h` maps `cpu_init` and
  `cpu_simulate` directly to CCPU providers; the historical vector is for
  consumers that explicitly use the old register facade.  It is therefore not
  a justified speculative initialization edit for this crash.

## Result and next action

No source defect is proven, and no source modification is admitted.  The next
permitted action is exactly one fresh run of the immutable r30 product in the
unchanged formal real-console container.  It may establish only whether the
same product still exits, times out, or produces an existing formal marker;
it must not add a debugger, BOP recorder, changed console owner, changed media
or firmware, or a different timeout.  If it still exits `0xC0000005`, the
next static owner unit is the original host console/display initialization
cluster already before `InitialiseDosEmulation`, rather than a guest/BOP fix.
