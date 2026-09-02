# M0 T373 S4 P1 — Win16 NE lifecycle source boundary

## Question

Can the current CPU40/x86 product compose the original WOWEXEC/Win16 NE
launch-and-return vertical without inventing an NE loader, a task scheduler or
a route around original WOW32 provider ingress?

## Inputs

* Immutable guest `src/mvdm-guest/wow16/test/shell/wowexec.c` and
  `WOWEXEC.EXE`.
* Immutable guest Kernel `kernel31/ld.asm`, `tasking.asm` and
  `wow16cal.asm`.
* Original host `src/mvdm-host/wow32/wkman.c:WK32WOWInitTask` and `W32Thread`.
* T373 S2 P2 dynamic-ingress boundary and S3 P1 callback revalidation.
* Current staging/build selection in `Stage-OriginalSoftpcRuntime.mjs` and
  `New-T310OriginalSoftpcNinja.ps1`.

## Observations

* WOWEXEC is an original Win16 guest task. Its `ExecProgram` obtains a
  `WOWINFO` record and calls guest `LoadModule`; Kernel `ld.asm` owns the NE
  header/module/segment/task load path. These guest bodies are immutable media
  and never belong in the host link graph.
* Original guest task startup reaches `WOW16_From_CallBack16` in
  `kernel31/wow16cal.asm` for non-return callback frames. That guest
  trampoline is a real guest-code continuation, not a host substitute for
  `CallBack16`.
* The host task handoff is `WK32WOWInitTask`: it sets `RET_TASKSTARTED`,
  starts/suspends original task threads, writes original TDB thread fields and
  calls `W32Thread`. `W32Thread` then establishes the original current TD and
  enters original host/guest synchronization around task startup.
* `WK32WOWInitTask` is reachable only through the global `aw32WOW` table in
  `W32Dispatch`; `W32Dispatch` is reached only after `MS_bop_1` has loaded
  WOW32 and original `W32Init` has succeeded.
* T373 S2 P2 records that `W32Init`'s required private USER handler,
  shared-task-list and hung-app shells are the earlier selected unavailable
  boundary. Therefore neither the host task handoff nor the guest WOWEXEC/NE
  entry can be honestly activated in this profile.
* The current runtime stager also selects no WOW16 media. That is a physical
  staging prerequisite, but it is later than the preceding source-owned
  WOW32 initializer boundary and must not be treated as the reason to invent
  a loader.

## Disposition

The first unrecoverable edge for the current selected vertical is retained as
the **original WOW32 `W32Init` product-shell contract**. No source is modified,
no WOW media is staged, and no NE parser, task state machine, guest trampoline
or static dispatch path is introduced. The original guest Kernel/WOWEXEC
algorithms remain correctly mirrored and are ready for a future whole-package
WOW32 product-shell admission; they are not currently a runnable product
path.

## Verification

* Static caller walk: `MS_bop_1 → W32Init → W32Dispatch → aw32WOW →
  WK32WOWInitTask/WK32WowGetNextVdmCommand → guest WOWEXEC → LoadModule →
  Kernel NE/task code`.
* Original source audit confirms `WOW16_From_CallBack16` is guest code and
  `W32Thread` is the historical host task/thread product lifecycle.
* Current build/stage selection contains neither a WOW32 provider DLL nor
  WOW16 media, consistent with the source-shaped unavailable disposition.

## Follow-up

S5 may formally reconcile the current CPU40/x86 product and retained fixed
runtime observation against this same boundary. It may not stage substitute
media or add a product observation that claims to launch WOWEXEC/NE before a
separate whole-package WOW32 product-shell task is admitted.
