# M0 T311 S1 — MVDM host support provenance and consumer closure

## Scope and method

`tools/governance/Audit-MvdmHostSupportTopology.mjs` walks every file below
`src/mvdm-support`, computes its SHA-256, and compares the same original-
relative path against the selected `OpenNT/base/mvdm` and OpenNT-src-2
`nt/private/mvdm` baselines.  Its complete, reproducible output is
[`m0-t311-s1-mvdm-support-provenance.tsv`](m0-t311-s1-mvdm-support-provenance.tsv).

The audit excludes neither source nor retained historical build products.  A
file's source identity is therefore distinct from whether it is a selected
runtime/build input.

## Provenance disposition

- The ledger contains 118 current paths: the component README plus 117
  original-relative support paths.
- 103 paths are byte-identical to the OpenNT baseline; 109 are byte-identical
  to OpenNT-src-2.  OpenNT-src-2 supplies six additional identical historical
  paths, including preserved object/build-product carriers.
- Seven declaration headers differ from both original baselines:
  `inc/apistruc.h`, `dossvc.h`, `intapi.h`, `mvdm.h`, `suballoc.h`,
  `vdmtib.h`, and `vrnmpipe.h`.  They are exactly the existing registered
  `MVDM-SUPPORT-DIV-001` through `-008` surface, with no newly discovered
  algorithm body divergence.
- `README.md` is project governance, not an upstream source.  `VDMTIB.INC`
  and `vdmtb.inc`, plus the retained `obj/` metadata/products, are supplied by
  OpenNT-src-2 where the older OpenNT baseline lacks them.  They retain their
  original-relative, no-new-runtime disposition.
- `mvdm-support-overlay/source/oemuni_pointer_width.c` is the sole current
  non-mirror support implementation.  It remains the registered
  `SUPPORT-OVERLAY-001` and moves as an overlay under the `oemuni` owner; no
  new overlay is admitted.

Two register hygiene findings transfer to S2 without changing a source body
in S1: `MVDM-SUPPORT-DIV-005` is duplicated in the old README, and the
`intapi.h` source comment incorrectly says `DIV-003` while its register row
is `DIV-005`.  The re-root must retain a single `DIV-005` registration and
correct that comment alongside the moved file.

## Consumer disposition

Only the following live consumers require a path update during S2/S3:

- `adapter-mvdm-host-out/softpc/include/softpc.h` forwards the original
  `softpc.h` declaration.
- `adapter-mvdm-host-out/redir/include/vdmredir.h` forwards the original
  `vdmredir.h` declaration.
- `tools/build/New-T310OriginalSoftpcNinja.ps1` supplies the active formal
  x86 CCPU40 graph's include root and selected `suballoc`/`oemuni` sources.
- `tests/mvdm-support/*` is test-only and must move or be renamed with its
  source owner; it is not a reason to retain the component.
- Current architecture/rules/status documents name the old component and must
  be reworded to the original `mvdm-host` topology in S3.

The WOW transaction header's only match is explanatory prose, not an include
or link edge.  Older `tools/build/New-T27*` through `New-T30*` scripts and
their supporting ledgers are retained evidence of prior packets: they are not
the current formal graph and should retain their historical paths rather than
be rewritten as live configuration.

## Result

All selected support paths are original MVDM host topology, and all live
consumers can follow that topology without a behavioral change.  No external
production owner or source-identity exception blocks S2's `git mv` migration.
The move destinations are `src/mvdm-host/{inc,oemuni,suballoc}` and
`src/mvdm-host-overlay/oemuni/oemuni_pointer_width.c`.
