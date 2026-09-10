# Retired Bochs source/component removal

## Candidate package

Remove the retired Bochs experiment after transferring every still-live
machine-facing call to the selected original SoftPC/CCPU40 composition.

## Decision

`bochs-core`, `bochs-core-overlay` and `adapter-bochs` are no longer a product
machine route. They may not remain as a hidden dependency of `app`, `session`,
an MVDM mirror or an MVDM historical-interface adapter. Historical source and
test material is retained only as indexed evidence outside production roots.

## Scope and S breakdown

### S1 — live dependency and ownership audit

Enumerate every production source, build manifest, test and current authority
that names a retired Bochs component. For each production call, identify the
original SoftPC/CCPU40 interface that must own the behavior or record a
source-proven unavailable disposition. No file is deleted in this step.

### S2 — machine-facing binding transfer

Replace each live `adapter-bochs` include/call with the corresponding
same-shaped `adapter-mvdm-host-out/softpc` interface backed by selected
original SoftPC/CCPU40 source. `app` must compose the original machine route
only. Do not add a replacement emulator or move DOS/VDM semantics into an
adapter.

### S3 — retired tree and fixture removal

After S2 proves no live consumer remains, remove `src/bochs-core`,
`src/bochs-core-overlay`, `src/adapter-bochs`, their retired production build
manifests and retired fixtures. Rehome any needed historical source evidence
below `docs/etc/legacy_code` with provenance; do not retain it as build input.

### S4 — selected-profile verification and closure

Run the selected Win32/x86 CCPU40 formal graph and the focused transferred
machine-interface checks. Prove no production/build/runtime/acceptance input
names or reaches a Bochs component. Update architecture, coding and source
policy authorities, then run documentation governance and `git diff --check`.

## Exit criteria

The selected original SoftPC/CCPU40 route is the only production machine
composition; no live product source, build graph, test or current authority
depends on a Bochs component; every deleted path has a provenance/evidence
disposition; and the selected formal graph remains build-closed.

## Non-goals

- No CCPU30/MONITOR admission.
- No x64 compatibility recovery.
- No new machine implementation, BOP provider or guest behavior.
- No use of `src.old`.
