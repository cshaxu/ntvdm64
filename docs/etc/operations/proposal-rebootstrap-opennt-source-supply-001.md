# Proposal: Canonical OpenNT source supply

## Purpose

Create the authoritative, package-scope OpenNT/OpenNT-4.5 source supply for
all later recovery work, while keeping guest inputs load-only and avoiding
premature provider composition.

## Prerequisites

The repository and neutral runtime foundation candidate is closed. Bochs may
already be available, but this task does not require a working guest runtime.

## Boundary

This task creates `opennt-platform-abi`, `opennt-guest-dos`,
`opennt-guest-wow16` and `opennt-mvdm-host`. The non-guest/non-tool MVDM tree
remains one source component even when it produces multiple original package
libraries. No new adapter/provider implementation belongs here.

## Proposed subtasks

1. **S1 — Package-scope union ledger.** Decide every OpenNT/OpenNT-4.5 target
   path using package, build, resource and artifact lineage; record retained
   and rejected variant hashes.
2. **S2 — Platform ABI mirror.** Import only exact required declarations from
   outside MVDM; prohibit replacement behavior in this root.
3. **S3 — DOS guest mirror.** Import selected DOS/V86 source, resources, build
   descriptions, tools inputs, intermediates and original products with an
   immutable guest-image manifest.
4. **S4 — WOW16 guest mirror.** Perform the equivalent complete WOW16 import
   and guest-image manifest work.
5. **S5 — MVDM host topology.** Import public/private headers, build topology
   and package roots under `opennt-mvdm-host` without changing dormant source
   bodies to force a build.
6. **S6 — Host package supply audit.** Import selected DEM, COMMAND, XMS,
   DPMI32, VDMREDIR, WOW32, VDD/debugger, `softpc.new`, SIM/monitor, utility
   and OEM packages; verify every mirror classification and host/guest link
   isolation.

## Exit criteria

- Every selected target-relative file has one package-scope provenance choice.
- Guest roots hold all selected original inputs/products and none is a host
  compile or link input.
- Each MVDM host file is exact upstream, a registered true subset or a
  registered same-shaped minimal modification.
- Dormant source remains original/dormant; it is not replaced by convenience
  shims merely to make the root compile.

## Non-goals

This task does not enable BOP providers, build every dormant unit, recreate
unavailable NT4 interfaces or prove guest execution.
