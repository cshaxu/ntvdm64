# M0 T382 — Current-formal SoftPC LPT runtime recovery

## Purpose

Restore the original CPU40 SoftPC LPT conventional-device path required for
the current formal product to proceed from COMMAND initialisation toward its
first command record.  This is an immediate predecessor of T381's selected
`cmdExec32` workload, not a printer feature project.

The exact current observation maps `inb(0x3BD)` to original
`printer_.c:printer_io`; the supporting original owner chain is
`printer_.c -> ios.c -> printer.c -> host/src/nt_lpt.c`, with BIOS reset and
configuration initialization as its inputs.

## Boundaries

- CPU40/SoftPC is the sole machine.  Guest DOS/WOW media stay immutable.
- The original SoftPC bodies remain owners.  A public-modern binding may only
  replace an unavailable NT4 host-printer transport while retaining the
  original status, timeout, initialization and failure contract.
- No virtual printer, per-port synthetic return, direct host-port policy,
  BOP patch, app command workaround, child launcher, `cmdExec32` change,
  DOS PSP work, WOW, Redirector, CPU30, Bochs or x64 recovery is in scope.
- The outcome may be an original bounded unavailable/no-device result when
  that is the source-defined state; it may not be a fabricated ready status.

## Ordered subtasks

1. **S1 — Current original LPT owner and identity ledger.** Compare the
   current formal graph, image map and original source path with the old T363
   evidence.  Reconcile BIOS BDA LPT addresses/timeouts, `printer_init`, IOS
   adapter registration, `printer_io`, `printer_inb`, `nt_lpt` status and
   timer/config inputs.  Select one complete recovery cohort or an earlier
   original terminal.  No product run or port-value change.
2. **S2 — Original LPT status/timeout cohort recovery.** Compose the exact
   cohort selected by S1.  Prefer original source and same-shaped public
   Win32 binding; preserve no-device/error/timeout semantics.  No leaf shim.
3. **S3 — Formal link and focused contract proof.** Rebuild CPU40/x86 and
   prove reset/BDA, IOS registration, status read, unavailable transport,
   timeout and cleanup through original callers.
4. **S4 — Frozen pure-DOS continuation observation.** Make one bounded,
   non-debug fixed-container observation of the unchanged declared workload.
   Record whether execution passes this owner and reaches the next original
   COMMAND boundary; do not repair a trace-selected successor.

## Completion standard

T382 closes only when the current formal product's original LPT status path
has a source-shaped recovery or exact source-defined terminal with local and
formal proof, and the one frozen observation is classified.  Its runtime
success marker is progress beyond `printer_io` into the next original COMMAND
boundary; it does not claim child execution.
