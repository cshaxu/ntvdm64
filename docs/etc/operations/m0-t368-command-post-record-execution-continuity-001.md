# M0 T368 — COMMAND post-record DOS execution continuity

## Purpose

T367 delivered one byte-identical original `.COM` workload through the existing
Base VDM record and proved the final product contains the original DEM read
provider.  Its single fixed observation reached the original first `54:01`
return and later stopped in an original `54:01` continuation/wait.  This packet
recovers the **complete original COMMAND owner cohort** between the delivered
`/C <DOS program>` record and either the guest `$Exec` handoff or an exact
source-defined COMMAND terminal.

No part of this packet may replace COMMAND with an app parser, a host-side DOS
loader, a new BOP leaf handler, or an artificial no-next-command result.

## Source boundary

The owner package is the imported original COMMAND source under
`src/mvdm-host/dos/command/`, in particular the `cmdGetNextCmd`/`CmdDispatch`
record-consumption and transient-command control flow, together with its
existing BaseClient/BaseSrv and session-neutral binding seam.  Guest NTDOS
`$Exec` stays immutable and load-only; `54:08/0A/0B` native-child behavior,
WOW, Redirector, and CSRSS remain separate later owner packages.

## S plan

### S1 — Original post-record COMMAND topology ledger

Trace the exact original call/data/failure path from a returned `54:01` record
through `cmdGetNextCmd`, `CMDINFO`, command-tail/environment exchange, transient
COMMAND processing, record repeat/no-next semantics and any handoff toward DOS
`$Exec`.  Identify every existing adapter/BaseClient/session seam and the
single earliest complete S2 cohort.  No product run.

### S2 — Earliest complete original COMMAND cohort

Recover only the owner-complete missing slice established by S1, preferring the
already imported original body and same-shaped Base VDM/session binding.  Keep
the original command record layout, call sequence and terminal behavior; do
not make a separate command interpreter or directly invoke DOS EXEC.

### S3 — Formal COMMAND boundary closure

Rebuild/link the selected CPU40/x86 product, prove the original COMMAND source
and required BaseClient/session bindings are final inputs, and run focused
source-shaped contract checks.  No fixed runtime observation.

### S4 — One fixed original `.COM` continuation observation

Make one fixed-container observation using the staged byte-identical
`LOADFIX.COM` workload.  Record a guest `$Exec` handoff, another exact original
terminal, or the earliest failing original owner; no trace-selected repair.

## Completion boundary

T368 closes at a source-shaped COMMAND-to-DOS transfer or an exact earlier
original COMMAND terminal.  It cannot claim guest execution, child termination,
parent restoration, `54:0B`, native child, MZ, WOW, Redirector or x64 runtime
without their separate evidence.
