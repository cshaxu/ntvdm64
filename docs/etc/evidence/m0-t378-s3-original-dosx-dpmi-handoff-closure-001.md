# M0 T378 S3 — Original DOSX / DPMI handoff closure

## Question

Does the immutable original DOSX carrier retain a single original handoff to
the selected DPMI32 host package, including table, frame/stack and
source-shaped unavailable directions?

## Inputs

- T378 S1's immutable `DOSX.EXE` identity and T378 S2's closed prerequisite
  CONFIG/HIMEM/XMS cohort.
- Original `softpc.new/host/src/nt_bop.c` and all selected
  `mvdm-host/dpmi32` sources.
- Current formal CPU40/x86 product graph and the earlier T371 S3 source
  topology record.

## Procedure and observations

1. The original `MS_bop_3` body contains only its original direct call to
   `DpmiDispatch`.  No app, session or adapter selector recognizer sits in
   this handoff.
2. `DpmiDispatch` reads the original one-byte service index at `CS:IP` by
   `Sim32GetVDMPointer`, advances IP by one and dispatches only if the index
   is in range.  An invalid index retains the original debug/error direction;
   it is not changed into a synthetic protected-mode result.
3. The original 25-entry `DpmiDispatchTable` is intact and ordered from
   descriptor/protected-mode entry through DOSX initialization, XMEM,
   fault/IRET and unhandled-exception ownership.  In particular,
   `DpmiInitDosx` retains the source-shaped shared-data, real-mode stack,
   protected-mode selector and IRET-frame initialization sequence.
4. The formal graph's `original-mvdm-dpmi32.lib` directly contains the
   selected original `debug`, `dpmi32`, `dpmiint`, `dpmiselr`, `buffer`,
   `data`, `int21map`, `modesw`, `register`, `savestat`, `stack`, `vxd`,
   `xmem`, `dpmimemr` and `dpmimscr` objects.  The current
   `original-softpc-process.exe` formal target is linked without `/FORCE`.
5. Original `Sim32GetVDMPointer` remains an in-machine CPU40 pointer lifetime;
   the session mapping manager is used only where a host object crosses the
   existing XMEM/PM-stack binding boundary.  It does not alter DPMI selectors,
   BOP bytes or DOSX frame data.

## Boundary result

The following historical product mechanisms remain deliberately
source-shaped unavailable/later-owner boundaries: kernel VDM control,
VDD/VCD private services and private CSR paths.  They are not required to
prove that DOSX has one original DPMI owner, and S3 does not report their
success.

S3 is source-, binding- and formal-link closed.  It does **not** claim that
the fixed product observation has entered protected mode or emitted `53:xx`.
S4 owns only the complete declared workload/formal stage matrix; S5 will make
the single no-diagnostic observation and classify its first actual boundary.
