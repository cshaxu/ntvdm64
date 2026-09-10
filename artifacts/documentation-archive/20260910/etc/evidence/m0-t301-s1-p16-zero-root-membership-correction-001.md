# M0 T301 S1 P16 — Zero-Degree Root Membership Correction

## Result

P15 verified source identity and body shape for its input, but its root input
was too broad: it admitted the historical MVDM directory as though it were the
`mvdm-host` source root. The full input remains immutable historical evidence;
the corrected root test is the physical subtree ownership of the current
`src/mvdm-host` component.

- The immutable full historical ledger contains **12,202** physical
  definitions and is not rewritten.
- The derived true-zero view, `operations/zero-ledger0.tsv`, contains
  **11,949** entries under current `src/mvdm-host` subtrees.
- It excludes **230** `wow16/**` entries and 23 non-host `inc/oemuni/suballoc`
  entries. They remain available for audit in the full ledger and, where
  policy-excluded, in the exclusion ledgers; no source identity is discarded.
- `bin86/**`, `dos/v86/**`, `wow16/**`, `tools16/**`, and `vdmutils/**` have
  zero rows in `zero-ledger0.tsv`.

## Rule applied

True zero degree is not a directory inventory. It contains only definitions in
the actual `mvdm-host` mirror subtrees. Guest images and tools are load-only or
tool inputs; their source bodies cannot be silently promoted into the host
source graph. A future continuous-MVDM call must first be separately recorded,
then be materialized into a later derived ledger rather than rewriting history.

## Scope

This corrects membership evidence only.  First-degree classification remains
paused.  It neither imports source nor enables a provider, adapter, build,
runtime route or BOP.
