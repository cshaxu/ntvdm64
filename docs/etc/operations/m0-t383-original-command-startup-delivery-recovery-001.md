# M0 T383 — Original COMMAND startup command-delivery recovery

## Purpose

Recover the earlier original COMMAND interval that turns the already completed
`54:0F cmdGetInitEnvironment` result into the first original
`54:01 cmdGetNextCmd` return.  This is the pure-DOS startup gate for the
declared command record and the sole immediate predecessor of T381's
`54:08/cmdExec32` native-child interval.

## Boundaries

- CPU40/SoftPC and guest media remain unchanged.
- Original `cmdmisc.c::cmdGetNextCmd`, COMMAND dispatch, BaseClient shape and
  the local Base VDM record remain owners.  The current local BaseSrv slice
  may only supply the original command-record, first-VDM, wait/retry and
  result contract.
- No app-owned execution, BOP leaf patch, guest/media mutation, printer
  change, `cmdExec32` source change, DOS PSP return, WOW, Redirector, CPU30,
  Bochs, or x64 work is in scope.

## Ordered subtasks

1. **S1 — Initial-environment-to-first-command owner/ABI ledger.** Reconcile
   original `cmdGetInitEnvironment`, `cmdGetNextCmd`, `GetNextVDMCommand`,
   first-VDM state, initial command record, environment/current-directory
   sizes, CR/LF/NUL command shape, and every current same-shaped binding.
   Select one complete source-owned gap; no product run.
2. **S2 — Original startup command-delivery cohort recovery.** Recover the
   selected complete COMMAND/Base VDM cohort with original names, ordering and
   failure directions.  No app-side command consumption or synthetic guest
   record is permitted.
3. **S3 — Formal link and focused original-caller proof.** Rebuild CPU40/x86
   and prove the first command record is consumed exactly once, the original
   first/retry state is retained, capacity failures preserve their original
   direction, and the resulting `54:01` return reaches its original caller.
4. **S4 — Frozen pure-DOS startup observation.** Make one bounded fixed
   non-debug observation of the unchanged declared command.  The success
   marker is a first original `54:01` return, not child execution.

## Completion standard

T383 closes only when the current product proves a first original COMMAND
command retrieval or preserves an earlier complete original unavailable
result.  It does not claim `54:08`, native-child execution, or a DOS PSP
return; those remain T381 and later guest lifecycle owners.
