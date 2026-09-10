# M0 T291 S2 — WOW thunk/frame recovery decision

## Scope and source facts

This decision covers the twelve S1-selected original WOW32 thunk/frame and
structure bodies.  It does not enable a WOW body, a Win16 API, BOP ingress or
a guest load.  The reproducible occurrence ledger covers every non-comment
use of `GETVDMPTR`, `FREEVDMPTR`, `FLUSHVDMPTR`, `GETFRAMEPTR`, `CallBack16`,
`Sim32GetVDMPointer`, `GetVDMAddr`, `NtCurrentTeb`, and `NtCurrentPeb` in the
selected bodies.

The original `wow32.h` implements these macros through the NT4 process-wide
`INTEL_MEMORY_BASE`/selector aliases.  `FREEVDMPTR` is a no-op outside debug
builds.  `wcall16.c` then writes a `CBVDMFRAME`, sets the historical VDM stack,
calls `host_simulate` or the fast monitor path, and consumes the modified
frame after return.  Those facts prevent an x86/x64 product from treating an
arbitrary numeric `VPVOID` as a durable host pointer.

## Decision

- The existing Redirector pointer scope is not a general recovery seam.  It
  requires each fixed guest span to be declared before entry and only supports
  a small, synchronous Redirector request.  Reusing it for dynamically sized
  WOW structure converters would silently broaden its contract and create a
  second generic guest-pointer mechanism.
- `GETVDMPTR`/`FREEVDMPTR`/`FLUSHVDMPTR` require a source-shaped WOW scope in
  the existing `adapter-mvdm-host-out/softpc` family.  It must acquire a
  `session.guest_memory` lease for the exact span, record whether a source
  flush commits it, and release the lease in the original order.  The scope
  does not own a mapping manager and may not return a pointer beyond the
  calling synchronous source operation.
- `GETFRAMEPTR` requires a bounded protected task-frame projection owned by
  `adapter-mvdm-host-out/monitor`, with its bytes obtained through the same
  `session.guest_memory` lease rule.  Current monitor `VDM_TIB` storage is
  sufficient only for the reached DPMI fields; it is not a WOW task/stack
  projection.
- `CallBack16` is a whole transaction, not a function-shaped convenience
  callback.  Its first implementation must preserve `CBVDMFRAME` construction,
  source stack replacement, typed machine pause/resume and AX:DX result
  retrieval, but it must not recreate CSRSS/BaseSrv, a CCPU executor or a
  second scheduler.  This is the first S3 implementation prerequisite.
- `Sim32GetVDMPointer(0,0,0)` must never recover `IntelMemoryBase`: that call
  represents a whole-memory NT4 process alias.  Future compile admission must
  bind every reached nonzero span to a specific lease, or retain the original
  unavailable/fallback branch.  The same applies to `GetVDMAddr` BIOS-data
  writes.
- `NtCurrentTeb` has a limited current-worker adapter precedent, but not the
  historical monitor TEB or `CurrentMonitorTeb` product shell.  A focused
  monitor-thread facade may be added only at its source call boundary.  The
  private `NtCurrentPeb()->FastPebLock` AeDebug check has no public-equivalent
  pointer contract and remains source-defined fallback/unavailable pending its
  own hard-error policy group.

## Recovery order

1. Implement and test the paired, bounded WOW guest-pointer scope in the
   existing SoftPC adapter family.
2. Implement and test monitor-owned numeric WOW task/frame projection.
3. Implement the one-session source-shaped `CallBack16` transaction using
   those two seams and typed `adapter-bochs` pause/resume mechanics.
4. Only then compile the complete thunk/frame subdomain as a package and keep
   every private USER/GDI, AeDebug, fast-monitor and unsupported callback
   branch on its source-defined unavailable/fallback route.

No new adapter component, mapping manager, direct Bochs call or provider body
is admitted by this decision.
