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
`adapter-vdm-monitor` and the minimum cooperative `broker` implementation.
Existing adapter files in `src.old/` are audited recovery candidates and may
be copied into their corresponding new roots when their interface contract and
dependency direction are compliant. Adapters preserve reached original names,
parameters, layouts, ordering and failure contracts. They do not become
alternate DEM/COMMAND/XMS/DPMI providers or introduce Bochs knowledge into
imported OpenNT source.

## Proposed subtasks

1. **S1 — Complete interface and reuse ledger.** Freeze original caller,
   declaration, calling convention, layout, host dependency, disposition and
   test for each reachable interface family; classify each existing adapter
   file as copy, reference-only, split, replace or retire.
2. **S2 — BOP transport.** Recover selector-blind copied frame ingress,
   checked transfer and typed completion with no selector/service interpretation.
3. **S3 — SoftPC facade.** Recover reached SoftPC/CCPU/SAS calls through typed
   `adapter-bochs` mechanics and session mapping leases.
4. **S4 — Win32 facade.** Recover reached unavailable Win32/NTDLL calls with
   public APIs, retaining the original observable failure result where modern
   Windows cannot offer the historical operation.
5. **S5 — VDM-monitor facade.** Recover the complete same-shaped
   `NtVdmControl`/`VDM_TIB`/V86-event/handler disposition family with bounded
   session/thread binding.
6. **S6 — Cooperative broker.** Implement versioned registration, identity,
   queue, notification, lease and disconnect cleanup using public IPC only.
7. **S7 — Local composition matrix.** Link selected original package callers
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
