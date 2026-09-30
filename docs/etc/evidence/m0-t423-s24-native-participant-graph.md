# M0 T423 S24 — Native Participant Graph

## Result

NTSRV now owns a PID-accurate `CONRECORD` projection for NTCON.  A direct
record is bound only by the authenticated NTCON worker passing the typed
process handle returned by its successful `CreateProcess`; it is not a
launcher-nominated PID.  Periodic worker samples carry actual Console member
PIDs.  NTSRV creates and retires only `OBSERVED` records from that sample,
while a `DIRECT` record remains until its original completion path retires it.

This removes the previous `members=<count>` inference.  A residual observed
Console client keeps the native worker non-empty; an empty sample leaves the
registered NTCON resident and reusable under NTSRV authority.

## Verification

- `basesrv-service-reservation-test --native-worker`: pass.  Covers typed
  direct-target binding, sampled observed participants, reservation/authentication,
  route loss, rebind, completion and worker rundown.
- `ntcon-execution-lifetime-test`: 367 checks, zero failures.  Covers normal
  and failed execution barriers, cancellation, request-handle release and a
  target surviving worker-side request cleanup.
- `monitor-rpc-test --empty`: pass with regenerated protocol-20 client stubs.
- Protocol-20 MIDL, NTSRV, NTCON, NTVDM worker RPC client/stub and NTMON
  client/stub were regenerated and linked in `build/M0-T423/S24/formal`.

The lifetime fixture explicitly supplies a permissive test-only local pipe
DACL.  The runner's RDP token otherwise rejects its own local pipe client with
`ERROR_ACCESS_DENIED`; production pipe authentication is unchanged.

## Boundaries

No guest, shared library, original DOS/WOW record, helper, scheduler, polling
or process-tree termination policy changed.  S25 standardizes external worker
control, S26 standardizes the monitor DTO, and S27 removes remaining
cross-worker handling divergence.
