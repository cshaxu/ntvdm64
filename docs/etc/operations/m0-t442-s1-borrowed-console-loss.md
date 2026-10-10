# M0 T442 S1 — borrowed Console-loss lifecycle

## Contract

`run16` marks a frontend lease as borrowed whenever it inherits a Console; it
does not classify its parent executable.  NTCON attaches to that caller's
Console, authenticates one self-containing member snapshot, and reports it to
NTSRV before the ordinary lease registration.

NTSRV keeps the process handles from that snapshot as the root's *initial
external Console identity*.  It registers one-shot waits for every member
other than the NTCON root itself.  A callback only sets the existing frontend
lifetime event.  The service's existing lifecycle decision then treats the
root as lost only after every external initial member has exited, sets
`frontend_closing`, and uses its existing worker-shutdown route.

The identity is deliberately not refreshed.  A later process attaching to the
Console object that NTCON kept alive cannot resurrect a frontend whose original
user-facing Console has gone away.

## Boundaries

- NTSRV owns waits, the closing decision and worker shutdown.
- NTCON owns only physical attachment and an event-driven candidate-launcher
  same-Console check requested by NTSRV.  It no longer chooses an anchor,
  re-samples members for liveness, or retires itself because an anchor died.
- A launcher-owned/self-created Console has `borrowed=FALSE`; it never uses
  the external-member-loss predicate.
- No new wire ABI, polling loop, process-tree/Observed record, target-specific
  behavior, or CMD/PowerShell name exception is introduced.

## Verification shape

The service fixture must cover both ownership modes with a real terminated
external process object: borrowed closes the root and signals its non-WOW
worker shutdown event; owned does neither.  Existing frontend authority and
join-notification fixtures remain required because they cover startup ordering
and NTSRV-directed admission separately.
