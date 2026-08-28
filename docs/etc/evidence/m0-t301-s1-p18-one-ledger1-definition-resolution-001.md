# M0 T301 S1 P18 — `one-ledger1` Physical Definition Resolution

## Inputs

- `docs/etc/operations/zero-ledger1.tsv`: 3,720 selected physical zero-degree
  identities after the P17 SoftPC second-executor cut.
- `docs/etc/operations/one-ledger1.tsv`: 7,367 direct calls classified as
  `outside-zero-ledger1`; the 319 zero-binding-ambiguous rows remain outside
  this external-definition pass.
- The approved OpenNT/OpenNT-4.5 source union.  The reusable Ctags index is a
  disposable derived cache only; every emitted candidate source file is
  re-hashed against the original source before it is recorded.

## Procedure

`tools/governance/export-t301-s1-one-ledger1-definition-resolution.mjs` reads
only the physical `one-ledger1` caller rows and resolves their callee spelling
to every matching physical C/C++ function definition in the original source
union.  It writes one resolution row per external call and one candidate row
per matching physical definition.  It neither reads a callee body nor chooses
a declaration, linkage, package, provider, adapter or runtime route.

## Result

- 7,367 external direct-call rows received a result.
- 4,262 rows have one or more physical original definition candidates.
- 3,105 rows have no original C/C++ definition candidate; these remain for a
  later public-platform, macro/indirect, assembly/import-only or genuinely
  missing-interface classification.
- 11,134 candidate rows were recorded: 10,443 in non-MVDM OpenNT source and
  691 in selected-MVDM/extracted-SoftPC source.  This is a source-location
  fact only; it does not admit the latter to the host closure.

## Interpretation and limitation

The result is an identity-preserving lookup frontier, not a package decision.
Same-spelled functions in different source files stay distinct candidate rows.
The next audit must use original declarations, linkage and package closure to
choose a disposition; it may not treat a spelling match as a callable body.
