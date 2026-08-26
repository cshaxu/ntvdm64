# Proposal: Historical interface adapter recovery

## Purpose

Recover the minimum same-shaped historical interface families that let selected
original MVDM packages compose against the new machine and supported Windows.

## Prerequisites

The neutral foundation, Bochs machine foundation and canonical OpenNT source
supply candidates are closed. Every admitted adapter entry has an original
caller and source/ABI/failure evidence.

## Boundary

The task owns `adapter-bop`, `adapter-softpc`, `adapter-win32`,
`adapter-vdm-monitor`, `adapter-redir`, `adapter-wow`, `adapter-vdd`,
`adapter-debugger` and the minimum cooperative `broker` implementation.
Existing `adapter-*`, `app` and `session` files in `src.old/` are audited
recovery candidates. Their compliant source must be reused or copied into the
corresponding new root before a replacement is authored; each exception is
recorded with its source/ABI reason. Adapters preserve reached original names,
parameters, layouts, ordering and failure contracts. They do not become
alternate DEM/COMMAND/XMS/DPMI providers or introduce Bochs knowledge into
imported OpenNT source.

The audit includes existing `bochs-core` mechanics where an adapter needs a
machine-facing counterpart, but preserves the one-way boundary: only
`adapter-bochs` calls `bochs-core`; no recovered adapter imports Bochs internals
directly.

## Proposed subtasks

1. **S1 — Complete interface and reuse ledger.** Freeze original caller,
   declaration, calling convention, layout, host dependency, disposition and
   test for each reachable interface family; classify each existing adapter
   file as copy, reference-only, split, replace or retire.
2. **S2 — Specialist adapter component inventory.** Freeze every absent
   historical external/product boundary as one existing adapter, one named new
   specialist adapter, or an original internal owner. In particular, establish
   `adapter-redir`, `adapter-wow`, `adapter-vdd` and `adapter-debugger` as
   separate source-owner boundaries; do not change an OpenNT mirror merely to
   avoid an absent interface, and do not create a generic compatibility root.
3. **S3 — BOP transport.** Recover selector-blind copied frame ingress,
   checked transfer and typed completion with no selector/service interpretation.
4. **S4 — SoftPC facade.** Recover reached SoftPC/CCPU/SAS calls through typed
   `adapter-bochs` mechanics and session mapping leases.
5. **S5 — Win32 facade.** Recover reached unavailable Win32/NTDLL calls with
   public APIs, retaining the original observable failure result where modern
   Windows cannot offer the historical operation.
6. **S6 — VDM-monitor facade.** Recover the complete same-shaped
   `NtVdmControl`/`VDM_TIB`/V86-event/handler disposition family with bounded
   session/thread binding.
7. **S7 — Specialist adapter contracts.** Recover the bounded, source-shaped
   contracts for Redirector, WOW, VDD and debugger adapters. Each retains its
   own original package boundary; unavailable private NT4 behavior is explicit
   and no adapter implements an alternate provider.
8. **S8 — Cooperative broker.** Implement versioned registration, identity,
   queue, notification, lease and disconnect cleanup using public IPC only.
9. **S9 — Local composition matrix.** Link selected original package callers
   against adapters and run interface-family positive/negative tests.

## Exit criteria

- Every enabled adapter entry cites an original caller and has a deterministic
  success or unavailable/failure result.
- `opennt-mvdm-host` does not call Bochs directly; `adapter-softpc` never
  includes a Bochs type.
- Broker IPC transports no local surrogate, native resource, guest pointer or
  Bochs object.
- Unsupported kernel/CSRSS behavior is explicit rather than emulated by a
  hidden replacement product shell.

## Non-goals

This task does not complete all MVDM providers, claim full child-process/DOS
lifecycle, or recreate CSRSS/BaseSrv/CCPU.
