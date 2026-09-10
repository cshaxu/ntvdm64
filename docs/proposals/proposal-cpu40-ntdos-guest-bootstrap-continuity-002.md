# CPU40/NTDOS guest-bootstrap continuity

## Purpose

Continue the previously closed post-`SVC_CMDSETINFO` source mapping only after
its named prerequisites have closed: COMMAND/DEM/SoftPC scalar leases (T347),
ordinary host drive/media capability (T345), resident device/FDC ownership
(T340), and executable-relative package admission (T349).  This is a new
implementation packet, not a reopening of T336's earlier terminal-boundary
audit.

## Ordered subtasks

1. **S1 — current source/state re-admission.** Reconcile the original
   `MS_bop_4 -> CmdDispatch -> cmdSetInfo -> msinit.asm` transition with all
   current prerequisite closures.  Identify the complete first CPU40/SAS/
   BIOS owner cohort and its source-defined stop conditions.
2. **S2 — source-shaped CPU40/SAS/bootstrap recovery.** Recover the admitted
   cohort only through selected original MVDM source, an existing same-shaped
   adapter, or a registered smallest overlay.  No BOP-specific result,
   guest-loader rewrite, second memory mapper or synthetic BIOS state.
3. **S3 — local contract and formal closure.** Verify the admitted state
   transition with focused owner tests and a selected formal CPU40/Win32/x86
   link; preserve original warning visibility.
4. **S4 — one fixed-container integration observation.** Only after S3,
   run the unchanged console-owning CPU40/x86 package once to decide whether
   it reached the next original source-defined ingress or terminal boundary.

## Exclusions

No trace-selected leaf BOP repair, DOS child/PSP lifecycle, guest source
rewrite/rebuild, virtual media/drive, BOP provider redesign, CPU30, Bochs,
x64 runtime admission, host-system installation or kernel/CSRSS recreation.

## Completion

The first complete original guest-bootstrap owner cohort after the original
COMMAND service return is code/formal closed and one fixed observation is
classified without overclaiming COMMAND.COM or DOS child execution.
