# Proposal: Historical interface adapter recovery

## Purpose

This proposal is closed as the historical interface-family and specialist
adapter inventory. It does not authorize adapter implementation: package-led
recovery is governed by the successor package tracker and recovery-wave plan.

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

## Completed subtasks

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
The former S3--S9 implementation outline is superseded. Each named boundary is
now recovered only alongside its original MVDM owner package, after
[`proposal-mvdm-package-interface-closure-tracker-001.md`](proposal-mvdm-package-interface-closure-tracker-001.md)
closes and according to
[`mvdm-package-recovery-wave-plan-001.md`](mvdm-package-recovery-wave-plan-001.md).

## Closure criteria

- Every known historical external family has one named adapter owner and
  original caller/declaration evidence.
- No mirror source is modified to bypass an unassigned missing interface.
- No generic compatibility root is introduced.

## Non-goals

This closed task does not complete all MVDM providers, claim full
child-process/DOS lifecycle, or recreate CSRSS/BaseSrv/CCPU.
