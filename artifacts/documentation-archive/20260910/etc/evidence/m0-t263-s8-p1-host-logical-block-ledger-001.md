# M0 T263 S8 P1 — reachable OpenNT host logical-block ledger

Date: 2026-08-24

## Criterion

S8 measures source fidelity by **currently admitted logical feature block**,
not whole-file byte or line count. A restored block should carry its original
module and algorithm comments where practical, so that its provenance remains
readable in-place. A block that is not reached by the current single-session
composition may remain absent or explicitly compiled out. A retained block
must preserve the corresponding OpenNT algorithm, data structure, ordering
and failure path, except at a statement-local same-shaped adapter call.

## Current block dispositions

- `vdmredir/vrputil.c`
  - Retained blocks: `VrpMapLastError`, `VrpMapDosError`, and
    `VrpTranslateDosNetPath`—the entire functional content of the original
    file.
  - The absent material is file documentation plus `VR_DIAGNOSE` output, not
    an admitted provider feature. The only functional divergence is the
    warning-safe spelling of the original scan assignment/`toupper` input.
  - P2 action: retain the existing full logical content; restore original
    declarations/order where the existing adapter include surface permits,
    without treating comments as required logic.

- `vdmredir/vrnmpipe.c`
  - Retained/admitted blocks: local pipe-name conversion; per-session open
    pipe metadata; synchronous `VrReadNamedPipe` and `VrWriteNamedPipe`.
  - `VrTerminateNamedPipes(DosPdb)` is retained as OpenNT's original empty
    lifecycle hook; process-level disposal is not invented in this mirror.
  - Cropped blocks: pipe state/peek/transact/call/wait; asynchronous request
    queue and worker; guest completion interrupt; cancellation-list ownership
    beyond the local session record; remote/Net APIs.
  - Repair action: recover the original synchronous blocks and their original
    data structures first; no async/remote/VDD block is restored without a
    reached caller.

- `vdmredir/vrinit.c`
  - Retained/admitted block: the minimal named-pipe provider lifecycle gate.
  - Cropped blocks: VDD hooks, NetBIOS, DLC, ICA interrupt and asynchronous
    completion broker.
  - Repair action: preserve a narrow original-shaped init/uninit path for the
    admitted pipe lifecycle; do not restore unrelated product initialization.

- `softpc.new/host/src/nt_error.c`
  - Retained/admitted block: `host_direct_access_error` used by BOP 59,
    including reason handling, duplicate suppression, Ignore resume and
    session controlled-stop outcome.
  - Cropped blocks: generic SoftPC host errors, legacy resource/dialog
    machinery, full-screen/window management, WOW-specific error boxes and
    debug-break utilities.
  - Repair action: restore the original direct-access block's decision and
    failure structure through the public-Win32 dialog facade; leave unrelated
    error/UI blocks cropped until reached.

## Adapter boundary

The existing original-name header facades (`nt.h`, `ntrtl.h`, `nturtl.h`,
`softpc.h`, `vrdlctab.h`, `vdmredir.h`, `vrinit.h`) are the starting point.
S8 may expand only the declarations/functions actually reached by the blocks
above. `adapter-win32` owns public Win32/NT compatibility; `adapter-softpc`
owns machine/CCPU/SAS/ICA-shaped calls; `session` owns only neutral lifetime
and completion primitives.
