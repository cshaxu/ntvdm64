# Proposal: Dual-architecture product composition

## Purpose

Compose the admitted source mirrors, machine and same-shaped interfaces into
the first formally buildable `ntvdm.exe` products for x86 and x64.

## Prerequisites

The preceding four rebootstrap candidates are closed. This task receives only
their registered production roots and manifests; it may not recover omitted
source packages by adding ad-hoc BOP handlers.

## Boundary

`app` owns CLI parsing, guest-image selection, session construction, adapter
wiring and broker connection/start. Existing app composition may be reused from
`src.old/` only after per-file owner/dependency review. Ninja owns host
dependency tracking and incremental library/link execution. The task tests
composition and boundaries, not general DOS/WOW runtime continuity.

## Proposed subtasks

1. **S1 — Formal Ninja graphs.** Generate isolated MSVC Win32/x86 `/MT` and
   MSVC x64 `/MT` graphs from the owner/package manifest, with static-library
   boundaries and configuration invalidation.
2. **S2 — Architecture-specific link closure.** Build admitted components and
   selected original package libraries independently for both architectures;
   prove no guest object/library or cross-architecture object enters a host link.
3. **S3 — CLI composition.** Audit and reuse compliant app composition, then
   assemble session, broker, adapters, machine and immutable guest-image
   manifest selection into `ntvdm.exe` without exposing old source roots or
   relocating existing `build/output/dos` or `build/output/wow16` evidence.
4. **S4 — Boundary regressions.** Test mapping sentinels/teardown, broker wire
   rejection, monitor binding, guest load-only isolation and controlled machine
   shutdown.
5. **S5 — Whole-tree audit.** Hash/compare mirrors, verify all divergences and
   overlays, scan dependency direction and prove `src.old` absence from every
   formal input. Publish the dependency-led owner-package work order.

## Exit criteria

- Formal Ninja produces separate x86 and x64 `ntvdm.exe` link closures.
- Every source input is under one of the thirteen roots and has a manifest
  owner; no transitional root or `src.old` input survives.
- The CLI can create one admitted session, connect to/start the broker, select
  an immutable guest image and perform controlled machine lifecycle operations.
- Build closure is reported separately from provider closure and guest runtime
  continuity.

## Non-goals

No claim is made that DOS/WOW workloads execute continuously, that every BOP
provider is enabled, or that trace-led leaf repair is authorized.
