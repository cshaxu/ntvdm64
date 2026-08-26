# Proposal: MVDM runtime/support/tool/firmware physical re-root

## Purpose

Make the approved original-MVDM ownership split physically true without
changing source bodies or build behavior. The temporary T274 exact source
union is an audit carrier, not the final component layout.

## Source-owner result

- `opennt-mvdm-host` retains true MVDM host-runtime provider packages.
- `opennt-mvdm-support` receives the exact `dirs`, `makefil0`, `inc`,
  `oemuni`, and `suballoc` paths.
- `opennt-mvdm-tools` receives exact `vdmutils` paths, including all original
  source, headers, resources and build descriptions.
- `opennt-mvdm-firmware` receives exact `softpc.new/base/bios`,
  `softpc.new/bios`, `softpc.new/roms` and `softpc.new/data` paths. These are
  immutable machine-input mirrors, never host-runtime translation units.

## Required method

1. Freeze a per-path destination ledger from the canonical 1,689-row union.
2. Use `git mv` only; retain package-internal relative paths and filenames.
3. Recompute source/hash identity after every move and prove no duplicate or
   missing selected path.
4. Update formal manifests/include inputs only to preserve source discovery;
   do not add a compile, archive or link edge.
5. Verify `opennt-mvdm-tools` has no inbound app/host runtime edge,
   `opennt-mvdm-support` has no automatic inbound link edge, and
   `opennt-mvdm-firmware` has no host compile/link edge.

## Non-goals

No original source-body edit, adapter implementation, support/tool build,
provider enablement, Bochs change, BOP behavior, guest execution or runtime
semantic change.

## Admission evidence

T276/S11 must complete the owner and interface disposition baseline first.
The re-root packet is a prerequisite to all subsequent support-library recovery
and prevents standalone tools from being accidentally classified as runtime.
