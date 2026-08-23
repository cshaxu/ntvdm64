# M0 T260 S1 owner-manifest baseline

## Scope

This is the first reproducible inventory pass for T260 S1. It changes no
product source and does not authorize a move. The generated TSV is
[`m0-t260-s1-owner-provenance-build-manifest.tsv`](m0-t260-s1-owner-provenance-build-manifest.tsv).

## Generator and input

`tools/governance/Export-T260S1OwnerManifest.ps1` consumes the current formal
module manifest, `tools/build/t225-s7-full-module-manifest.json`, and records
every declared module source, fixture source, executable source, include root,
and generator input.

The completed S1 manifest contains 3,715 unique rows:

| Kind | Count | S1 disposition |
| --- | ---: | --- |
| Formal module source | 291 | Owner rule applied; mixed files remain explicitly marked for the owning later S. |
| Fixture source | 78 | `TEST-CLASSIFY`; must follow the component under test in S8. |
| Executable target source | 1 | `app`, S4. |
| Reached header | 259 | Recursively resolved from formal module, fixture and target `#include` input; each has a target owner. |
| Include root | 14 | Formal build-graph input retained under `tools/build`; it is not a source-owner placeholder. |
| Build input | 2 | Retain under `tools/build`; update paths in S8. |
| DOS/WOW guest source candidate | 618 | `opennt-guest`; S4 must distinguish source-build membership from retained guest-source evidence. |
| Non-formal legacy product input | 112 | Classified to a target component although absent from the current formal module graph. |
| Non-formal OpenNT evidence input | 2,332 | `refs/opennt`; explicitly retained provenance, never a product provider. |

The first classified source distribution is: `bx-core` 122, `bx-mantle` 8,
`adapter-softpc` 58, `adapter-win32` 1, `opennt-bop` 60, `opennt-host` 27,
`opennt-guest` 618, `opennt-bop` 176, `adapter-softpc` 152, `bx-core` 181,
`opennt-host` 61, `app` 34, `bx-mantle` 21, `tools/build` 16, and
`adapter-win32` 4. All formal sources, reached headers, fixtures, targets and
build inputs now have one target owner. Rows marked `review/split` still
require their owning S to confirm the file split.

## S1 closure evidence

The final current-tree cross-check finds 3,542 files below `src/`; all 3,542
occur exactly once in the manifest. There are zero missing paths, zero duplicate
paths and zero unresolved owner markers. Every formal source, reached header,
fixture, target, build input, guest candidate and non-formal legacy/evidence
input has one target owner and migration disposition.

S1 is closed. It did not claim that a later S had performed any `git mv`,
created a component root, or made a source/build closure valid under the new
paths.

## Live manifest refresh

The manifest is a live generated input to T260.  After S2 root creation and
S3's 42 history-preserving moves, the refresh contains 3,735 rows for 3,542
`src/` files, still with zero missing paths, duplicate paths or unresolved
owners.  The changed count is a layout update, not a change to the S1
classification rule or a build-closure claim.
