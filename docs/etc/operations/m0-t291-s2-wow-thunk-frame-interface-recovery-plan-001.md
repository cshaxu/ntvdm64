# M0 T291 S2 — WOW thunk/frame interface recovery design

## Purpose

Recover the source-shaped boundary contract required by the twelve original
WOW32 thunk/frame/structure bodies before any of those bodies are enabled.
This is the first dependency group selected by T291/S1; it is not a Win16 API
or callback implementation.

## Required source surface

- `GETVDMPTR`/`FREEVDMPTR` and `Sim32GetVDMPointer`/`GetVDMAddr` call forms.
- `CallBack16`, `VDMFRAME` construction/return, and their worker/session
  continuation assumptions.
- Original x86 16/32 pointer and native-handle fields reached by
  `mapembed.c`, `walias.c`, `walloc16.c`, `wcall16.c`, `wcall32.c`,
  `wstruc.c` and the remaining active thunk/frame subdomain bodies.

## Boundary rules

- `session.guest_memory` alone supplies synchronous checked guest leases.
- `session.host_resource` alone supplies persisted native identity surrogates.
- `adapter-mvdm-host-out/softpc` retains historical SoftPC/CCPU call shapes;
  `adapter-mvdm-host-out/monitor` retains monitor/frame call shapes; and
  `adapter-mvdm-host-out/wow` owns only WOW-specific interface adaptation.
- No direct Bochs object, raw native pointer/HANDLE, static current-session,
  new mapping manager, generic adapter, CSRSS/BaseSrv recreation or callback
  scheduler is admitted.

## Deliverable

A per-call source/ABI/failure ledger must state the original declaration,
callers, field layout, lease/identity duration, existing same-shaped adapter
availability, exact unavailable dependency, and a minimal future binding or
failure result. It must prove whether current adapter families suffice before
any body is compiled or changed.
