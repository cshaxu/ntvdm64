# Proposal: Canonical OpenNT source supply

## Purpose

Create the authoritative, package-scope OpenNT/OpenNT-4.5 **non-guest** source
supply for all later recovery work, while directly recovering the already-
complete repository-local guest mirrors as load-only inputs and avoiding
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

1. **S1 — Non-guest package-scope union ledger.** Decide every non-guest
   OpenNT/OpenNT-4.5 target path using package, build, resource and artifact
   lineage; record retained and rejected variant hashes.
2. **S2 — Platform ABI mirror.** Import only exact required declarations from
   outside MVDM; prohibit replacement behavior in this root.
3. **S3 — DOS guest mirror direct recovery.** Recover the already-complete
   `src.old/opennt-guest/dos-v86` guest mirror into `opennt-guest-dos` without
   consulting OpenNT/OpenNT-4.5 as a second copy source. Preserve its
   source/resource/build/intermediate/product inventory and add an immutable
   guest-image manifest.
4. **S4 — WOW16 guest mirror direct recovery.** Recover the already-complete
   `src.old/opennt-guest/wow16` guest mirror into `opennt-guest-wow16` by the
   same direct route. Preserve `build/output/dos` and `build/output/wow16`
   exactly where they are as prior build outputs/evidence: do not move, copy,
   relink or treat them as host-build inputs.
5. **S5 — MVDM host topology.** Import public/private headers, build topology
   and package roots under `opennt-mvdm-host` without changing dormant source
   bodies to force a build.
6. **S6 — Host package supply audit.** Import selected DEM, COMMAND, XMS,
   DPMI32, VDMREDIR, WOW32, VDD/debugger, `softpc.new`, SIM/monitor, utility
   and OEM packages; verify every mirror classification and host/guest link
   isolation.

## Exit criteria

- Every selected non-guest target-relative file has one package-scope
  provenance choice; guest carry-forward inventories retain their existing
  provenance/hash records.
- Guest roots hold the complete carried-forward original inputs/products and
  none is a host compile or link input.
- Each MVDM host file is exact upstream, a registered true subset or a
  registered same-shaped minimal modification.
- Dormant source remains original/dormant; it is not replaced by convenience
  shims merely to make the root compile.

## Non-goals

This task does not enable BOP providers, build every dormant unit, recreate
unavailable NT4 interfaces or prove guest execution.
