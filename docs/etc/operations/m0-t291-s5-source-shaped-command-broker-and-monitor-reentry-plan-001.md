# M0 T291 S5 — Source-shaped command broker and monitor re-entry recovery

## Admission dependency

S4 must close first. S5 is the named follow-on, not a claim that the current
single immediate command source is already a BaseSrv/CSR replacement.

## Objective

Recover only the original, publicly reproducible command-broker and monitor
re-entry behavior needed by reached MVDM callers. Preserve `GetNextVDMCommand`
and `VDMINFO` shapes, original state/capacity/re-entry ordering, and the
separation between command acquisition, `NtVdmControl`, and synchronous WOW
callback execution.

## Required source-first sequence

1. Audit `vdmapi.h`, `cmdexec.c`, `cmdmisc.c`, `config.c` and `wkman.c` as one
   command-broker package: capture ownership, caller state flags, no-command
   success, capacity retry, re-entry count, notification and wait/wake order.
2. Define one monitor-owned typed provider protocol that represents those
   source operations; the `broker` component may carry copied queue/event
   records only where the source evidence requires producer/consumer wake-up.
   It may not become a generic operation dispatcher.
3. Recover the applicable one-session COMMAND and WOW consumer arbitration
   before admitting any multi-process broker extension. Every unavailable
   WOWEXEC/CSR/private-server behavior retains an explicit source failure or
   deferred result.
4. For each admitted `host_simulate` group, compose the original caller's
   preconditions and postconditions around the same-shaped mechanical entry;
   never attach business interpretation to `adapter-softpc` itself.
5. Extend the WOW TD/TEB projection only when a selected original owner body
   requires a copied numeric field. Native task/thread/window pointers remain
   adapter-private and session-bound.

## Non-goals

No BaseSrv/CSRSS clone, global current task/session, generic dispatcher,
second CPU executor, raw pointer/HANDLE transport, fast WOW assembler or
selector enablement without its owner package.

## Exit criteria

Each newly admitted command or monitor consumer has a source call-order and
failure record, an owning typed capability, x86/x64 focused proof and no
cross-plane fallback. Unadmitted private product-shell behavior remains
explicitly unavailable rather than being silently routed through COMMAND.
