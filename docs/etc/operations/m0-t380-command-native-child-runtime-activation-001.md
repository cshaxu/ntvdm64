# M0 T380 — COMMAND native-child runtime activation

## Purpose

Turn T370's closed original COMMAND source/binding baseline into one actual
single-process native-child lifecycle.  The selected route must retain the
original `54:08`, `54:0A` and `54:0B` ownership through `cmdExec`,
`cmdExecComspec32`, `cmdExec32`, `cmdCreateProcess`, re-entry accounting and
`cmdReturnExitCode`; it must not replace that route with an app-owned child
launcher.

## Fixed boundaries

- CPU40/SoftPC remains the one machine. Guest media remains immutable.
- Original COMMAND bodies under `src/mvdm-host/dos/command` remain the source
  owner. Same-shaped public Win32/session bindings may supply only unavailable
  historical host mechanics.
- A session-owned child record may hold modern process and stream identities;
  no raw host handle or guest pointer crosses the asynchronous boundary.
- The task is one process and one session. CSRSS/BaseSrv multi-VDM brokerage,
  WOW launch, Redirector pipe protocol and synthetic DOS `EXEC`/PSP behavior
  remain out of scope.

## Ordered subtasks

1. **S1 — Whole original owner and runtime-predecessor rebaseline.** Re-read
   every `54:08/0A/0B` caller, state/data transition, failure path, binding,
   T370 evidence and subsequent pure-DOS runtime closures. Select one complete
   public native-child workload and name its first still-unproved original
   predecessor. No production change or product run.
2. **S2 — Source-shaped lifecycle completion.** Recover the one complete
   selected original cohort, including child creation, standard-stream
   inheritance, completion, cancellation, re-entry and result return. Prefer
   an original body and same-named binding; a new adapter is last resort.
3. **S3 — Formal product and owner proof.** Rebuild the selected CPU40/x86
   formal product and execute focused source-owner checks for success, invalid
   input, completion, cancellation, child cleanup, re-entry balance and host
   standard-handle isolation.
4. **S4 — Frozen native-child observation.** Make one bounded non-debug
   fixed-container observation of the selected workload. Classify its first
   original child result or earlier source owner; do not create a leaf repair
   from that trace.

## Completion standard

T380 closes only when an original COMMAND native-child workload is actually
started through the original `54:08/0A/0B` lifecycle or an earlier complete
original boundary is proven unavailable with its original result preserved.
It does not claim DOS PSP return, WOW, cross-process brokerage or general
native-child compatibility.
