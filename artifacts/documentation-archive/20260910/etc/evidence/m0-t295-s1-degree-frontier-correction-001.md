# M0 T295 S1 — zero/first-degree frontier correction evidence

## Corrected invariant

`mvdm-host` zero degree is the full set of function definitions in every
provenance-selected MVDM C/C++ source file.  A first-degree candidate is only
a direct call whose target cannot be resolved to that zero-degree set under
the original same-file/static and external-linkage rules.  Physical source
membership alone is never relabelled as an external package dependency.

## Procedure

The former regular-expression definition collector was replaced with
Universal Ctags over exactly the 522 selected source paths from
`mvdm-file-recovery-ledger.tsv`.  Ctags supplies function identity, line and
end line for ordinary C, K&R and SoftPC macro-form definitions; the generator
normalizes `IFN`/`IPT` tag spellings from their Ctags type evidence.  Node then
uses the selected original byte stream to derive hash, linkage, source
identity and direct calls.

No current product source, `src.old`, compilation output, build graph, adapter
or runtime implementation is an input.

## Result

- 522 selected MVDM C/C++ source files were enumerated.
- 11,872 distinct zero-degree definition identities were recorded.
- 24 selected C/C++ files have no Ctags function tag; each is a data, table,
  resource or historical support-data translation unit, and its explicit
  `no-C-C++-function-definition-found` row remains in the coverage ledger.
- 329 ambiguous historical variant calls were retained as zero-degree
  conditional targets.
- 2,821 direct calls remain as the corrected first-degree candidate frontier:
  1,157 initially public Win32/CRT candidates and 1,664 source-audit
  candidates.

The generator rereads every candidate after generation and throws if its
caller is missing from zero degree or its target resolves to a same-file,
unique external, package-local, or conditional zero-degree definition.  The
final run passed this mutual-exclusion assertion.

## Supersession

The old 4,499-definition/2,353-candidate T293 frontier and every T294 ledger
derived from it are superseded as current inputs.  They remain preserved as
historical method evidence in the
`mvdm-degree-audit-supersession-ledger.tsv`; no recovery package may consume
them.

## Required successor

A newly admitted first-degree audit must consume only the regenerated
2,821-row candidate ledger.  It may inspect a candidate definition/declaration
under its own packet, but must never move an MVDM-resident definition back into
first degree.
