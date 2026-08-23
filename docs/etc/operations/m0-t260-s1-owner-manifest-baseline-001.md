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

The current generated baseline contains 1,004 unique rows:

| Kind | Count | S1 disposition |
| --- | ---: | --- |
| Formal module source | 291 | Owner rule applied; mixed files remain explicitly marked for the owning later S. |
| Fixture source | 78 | `TEST-CLASSIFY`; must follow the component under test in S8. |
| Executable target source | 1 | `app`, S4. |
| Include root | 14 | `HEADER-CLASSIFY`; S1 must replace roots with the reached header closure. |
| Build input | 2 | Retain under `tools/build`; update paths in S8. |
| DOS/WOW guest source candidate | 618 | `opennt-guest`; S4 must distinguish source-build membership from retained guest-source evidence. |

The first classified source distribution is: `bx-core` 122, `bx-mantle` 8,
`adapter-softpc` 58, `adapter-win32` 1, `opennt-bop` 60, `opennt-host` 27,
`opennt-guest` 618, and `app` 16. These are provisional only where a row says
`review/split` or where the target is a classification marker.

## Remaining S1 work

1. Expand each formal include root to the actually reached header closure.
2. Add guest source/artifact build inputs which are intentionally outside the
   modern formal runtime graph.
3. Replace every `HEADER-CLASSIFY` and `TEST-CLASSIFY` marker with one target
   owner and one migration disposition.
4. Reconcile the resulting rows against the BOP tracker, build generator, and
   current live-call scan before any source move is admitted.

The manifest is therefore **in progress**, not an S1 closure claim.
