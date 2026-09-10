# M0 T381 — Original `cmdExec32` stage-to-return recovery

## Purpose

Recover the complete original COMMAND native-child interval exposed by T380:
from original `54:08`/`cmdExec` stage 0 through `cmdExec32`, the original
`cmdCreateProcess` worker, Base VDM re-entry, child completion and the
original `54:08` return. The selected public workload remains
`C:\Windows\System32\cmd.exe /c exit 37`.

T380 proved that guest COMMAND reaches this exact owner boundary after
original DEM opens `CMD.EXE`; it did not prove a native child began or
returned. Under the dependency-first rule, this complete owner cohort takes
priority over WOW16 activation.

## Boundaries

- CPU40/SoftPC remains the sole machine; immutable guest media is unchanged.
- Original `mvdm-host/dos/command/cmdexec.c` remains the lifecycle owner.
  `app` never becomes a child launcher.
- Same-shaped Base VDM/session/Win32 bindings may provide unavailable NT4
  mechanics only. They preserve original creation, wait, re-entry,
  cancellation, cleanup and failure ordering.
- One process and one session only. DOS PSP parent return, WOW, Redirector
  pipe protocol, CSRSS/BaseSrv cross-process brokerage and synthetic DOS
  execution remain outside this T.

## Ordered subtasks

1. **S1 — Complete stage-0-to-return source/ABI rebaseline.** Reconcile every
   original call, structure, state transition and failure branch between
   `cmdExec`, `cmdExec32`, `cmdCreateProcess`, Base VDM re-entry and
   `cmdReturnExitCode` with currently linked same-shaped bindings. Name one
   complete smallest missing source-owned cohort; no product run or leaf repair.
2. **S2 — Original worker and re-entry cohort recovery.** Compose the full
   cohort selected by S1. Prefer direct original source and original names;
   unavailable NT4 mechanics use the smallest named adapter. No synchronous
   app substitute, fake completion or per-BOP workaround.
3. **S3 — Formal lifecycle link and focused proof.** Rebuild the CPU40/x86
   formal product and verify creation, inherited standard streams, child
   completion, cancellation, cleanup, re-entry balance and result propagation
   through the original caller path.
4. **S4 — Frozen native-child completion observation.** Make one bounded,
   non-debug fixed-container run of the selected workload. Record the first
   original result; do not create a trace-selected repair.

## Completion standard

T381 closes only when the selected original child launches and the `54:08`
lifecycle returns through its original owner path, or an earlier complete
original owner is proven unavailable with its original result preserved. A
stage-0 marker, host-only fixture, app-owned child or synthetic completion
does not meet this standard.
