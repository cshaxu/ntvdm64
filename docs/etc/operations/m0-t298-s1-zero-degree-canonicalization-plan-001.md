# M0 T298 S1 — MVDM-host zero-degree canonicalization plan

## Objective

Produce a second, derived view of the current T295 audit. It must preserve the
11,872 raw source/path/hash/line/linkage identities while making three facts
explicit:

1. the canonical source-declared function name, including a name supplied by
   a function-definition macro;
2. the smallest safe deduplication scope; and
3. the distinct first-degree external interface spellings directly referenced
   by the zero-degree bodies.

## Invariants

- The existing T295 definition and call-site ledgers are immutable raw
  evidence. T298 adds projections; it does not overwrite or renumber them.
- A file-static function is scoped by selected source path and source hash.
  Same spellings in separate files never collapse.
- A non-static same-name group is an original product/conditional variant
  family, not a chosen provider. Its individual raw identities remain listed.
- A macro-generated definition is renamed only from its source-declared macro
  argument with file/line/signature evidence. Unresolved forms stay explicit
  and cannot collapse by macro spelling.
- A raw T295 first-degree candidate remains immutable evidence.  After
  source-proven macro-name normalization it is either a true external
  interface or is explicitly reclassified to its canonical zero-degree target.
  It never silently disappears merely because its raw spelling overlapped.

## Deliverables

- A canonical zero-degree function ledger with one row per raw identity and a
  canonical group key.
- A canonical group summary that reports singleton, file-static, product
  variant and macro-generated groups without deleting raw identities.
- A canonical first-degree interface ledger, one row per remaining external
  spelling, with all raw call-site IDs retained.
- A raw-call resolution ledger that accounts for every raw first-degree call
  as either an external interface or a source-proven canonical zero-degree
  target.
- Reproducible generator, evidence and closure history.

## Verification

The generator must assert complete raw-row coverage, valid source-derived
macro names, source-scope keys for statics and complete raw-call resolution.
It must explicitly reconcile raw first-degree spellings with canonical names
after macro normalization, rather than treating a raw spelling overlap as an
error. Documentation governance and `git diff --check` apply; any historical
mirror formatting diagnostics are reported without changing imported bytes.
