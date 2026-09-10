# M0 T313 S5 P5 — Bounded execution and reached-path purity reread

## Result

The selected Win32/x86 CCPU40 route has the bounded execution and controlled
terminal result required by S5.  It is not a DOS boot claim.

- The bounded witness initializes the selected original SAS/CCPU/FPU state,
  enters the unchanged original `nt_cpu.c:host_start_cpu()` wrapper, executes
  the original RAM/FPU instruction sequence, and returns at the original
  CCPU unsimulate boundary.
- The selected `TerminateVDM -> terminate -> host_terminate` route transfers
  completion code `37` through the session terminal boundary.  It completes
  the session and returns to the caller without calling `ExitProcess`.
- The selected source graph remains the 407-edge original CCPU40 candidate.
  A rerun at `build/M0-T313/S5/ems-writeback-r3` reported `ninja: no work to
  do`, which is valid manifest-identity reuse of the fresh P4 run.

## Reached divergence disposition

The reread covered the source and binding paths reached by the bounded
execution and terminal routes:

- CCPU/SAS/FPU and its generated state remain the selected original owner
  bodies, with existing registered `MVDM-HOST-DIV-030`, `-048`, `-055`,
  `-127`, `-130`, `-137` and overlay registrations.
- `nt_reset.c`, `nt_event.c`, and `sim32.c` use the one registered
  `MVDM-HOST-DIV-147` typed session terminal, replacing only the historical
  process-wide exit effect.  They do not add a second executor or guest
  service policy.
- `emm_mngr.c` uses `MVDM-HOST-DIV-148` to select its already-original LIM
  writeback bodies; no EMS shim remains on the selected CCPU path.
- Startup calls the same-shaped `DBGInit` binding registered in
  `adapter-mvdm-host-out/debugger`.  The full original `mvdm-host/dbg/dbg.c`
  product was independently attempted and is not selected: it immediately
  imports the NT4 private Debug Subsystem `ntdbg.h` LPC/port protocol.  That
  package remains original mirror material for the later Debugger/BDE/FAX
  owner package; it is neither a fake startup provider nor an S5 debt.

## Verification and limits

The original debugger-package probe failed before object creation on private
`PORT_MESSAGE`/debug-subsystem contracts.  It was fully reverted and left no
source, build or exception-register change.  The accepted formal result is
therefore the prior fresh P4 external build:

```text
build/M0-T313/S5/ems-writeback-r3
original-softpc-candidate + original-softpc-forced-closure.dll
407/407, exit 0
```

`original-softpc-forced-closure.dll` remains an ownership audit generated
with `/FORCE`; it is not a runnable image.  This P does not claim BIOS boot,
NTIO/NTDOS loading, BOP execution, BaseSrv/CSRSS recovery, live device I/O,
or the full NT4 debugger product.
