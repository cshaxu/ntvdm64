# One Ledger 1 (`one-ledger1.tsv`)

`one-ledger1.tsv` is the direct-call frontier generated only from the physical
function identities in `zero-ledger1.tsv`.  The older
`mvdm-first-degree-*` files remain historical evidence and are intentionally
not read or modified by this export.

Each row preserves the caller's original source root, relative path, SHA-256,
definition line and symbol.  It contains a named, direct call that could not
be proven to target a unique `zero-ledger1` body.  `outside-zero-ledger1` is a
proper one-degree source/disposition-audit seed.  `zero-ledger1-binding-ambiguous`
is retained separately in the same ledger so that same-named physical bodies
are never silently merged or misclassified as external.

This is deliberately not a provider selection, package-import decision, or
second-degree traversal.  It never reads an alleged callee body.

Generation:

```text
node tools/governance/export-t301-s1-one-ledger1.mjs .
```
