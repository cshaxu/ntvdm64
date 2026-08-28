# M0 T301 S1 P11 — Full Original-MVDM Candidate Sweep and Closure Expansion

## Full frontier correction

P11 extends P9/P10 beyond declaration-shaped calls.  All 12,426 physical
current first-degree calls were compared against the complete original MVDM
function-definition index, then gated by selected source root, linkage and
the caller's recorded original include-package frontier.

- 380 physical calls have exactly one passing original-MVDM body;
- zero calls have multiple passing bodies; and
- 12,046 calls remain outside the passing MVDM-body set.

The full gate is retained in
`mvdm-first-degree-rebaselined-full-mvdm-boundary-gate-ledger.tsv`.

## Confirmed seed expansion

The full source-shape gate confirms call/declaration/definition arity for 202
of those 380 calls.  These 202 calls refer to 69 distinct original MVDM bodies.
Starting from those bodies, P11 recursively parses only direct calls in the
approved original MVDM sources:

- 17 additional original-MVDM bodies are reached;
- the resulting expansion contains 86 physical bodies;
- it emits 343 direct edges;
- 120 edges have a unique original-MVDM internal candidate; and
- 223 edges leave the newly discovered bodies and become explicit T301
  classification inputs.

The body and edge ledgers preserve physical source identities and do not add
any adapter, provider, build or runtime dependency.

## Limitation and next gate

P11 is a discovery/rebase input, not a final zero-degree rewrite.  The 120
new internal edges require the same source-root/linkage/include-frontier and
shape gates before inclusion.  The 223 exits require normal first-degree
classification.  No body or exit is collapsed solely by name.
