# True Zero Ledger (`zero-ledger0.tsv`)

`zero-ledger0.tsv` is a derived, **narrow** view of the unchanged historical
zero-degree audit. It contains only definitions whose original MVDM path falls
under a physical subtree presently owned by `src/mvdm-host`.

It excludes `bin86/**`, `dos/v86/**`, `wow16/**`, `tools16/**`, `vdmutils/**`,
and every other historical MVDM path not physically owned by that component.
Those rows remain preserved in the full original ledger and, when excluded by
policy, in the directory/definition exclusion ledgers. No audit evidence is
deleted.

Generation:

```text
node tools/governance/export-t301-s1-true-zero-ledger0.mjs .
```
