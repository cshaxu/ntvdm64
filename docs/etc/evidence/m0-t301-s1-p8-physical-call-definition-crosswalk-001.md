# M0 T301 S1 P8 — Physical Call / Original-MVDM Definition Crosswalk

## Purpose

P7 proved that the declaration-shaped first-degree frontier contains original
MVDM definition bodies missed by the lightweight parser.  This P8 artifact
joins those bodies back to every affected *physical call record* without
collapsing calls or definitions by spelling.

## Inputs

- `mvdm-first-degree-rebaselined-boundary-ledger.tsv` — the physical T300 call
  identity;
- `mvdm-first-degree-rebaselined-declaration-shape-ledger.tsv` — the P6
  declaration-form partition; and
- `mvdm-first-degree-rebaselined-mvdm-definition-form-candidate-ledger.tsv` —
  the P7 original-source definition index.

All source-derived identities remain approved OpenNT/OpenNT-4.5 identities.
No current-product source, `src.old`, runtime observation or build result is
an input.

## Result

The generated
`mvdm-first-degree-rebaselined-definition-candidate-crosswalk-ledger.tsv`
has exactly one row for each of the 473 declaration-shaped physical calls.

- 236 calls have at least one physical original-MVDM definition candidate;
- 237 calls have no original-MVDM definition candidate and continue through
  T301's approved-source-union resolution; and
- candidate source identities preserve root, relative path, SHA-256 and line.

For example, physical calls from `dos/command/cmdconf.c` to
`cmdCreateTempFile` retain two edition identities of the original
`dos/command/cmdredir.c` body.  In contrast, the physical `CmdDispatch →
DbgPrint` call correctly has no MVDM body candidate.

## Constraint

The 236 rows are **not yet zero-degree reclassifications**.  The next parser
correction must establish, for each call, compatible include frontier, linkage
and selected source variant.  Until then, the row remains an explicit T301
candidate rather than a spelling-based merge.  This crosswalk merely removes
the prior false premise that every declared first-degree form lacks a body in
the original MVDM package.
