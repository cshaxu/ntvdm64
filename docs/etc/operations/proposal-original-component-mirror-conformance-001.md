# Original-component mirror conformance proposal

## Proposed task

**M0 T262 — Original component mirror conformance and exception closure**

## Decision

The original-code components `bx-core`, `opennt-guest`, and `opennt-host` are
not general compatibility roots. Each production file must be exactly one of:

1. a byte-identical file at the re-rooted Bochs/OpenNT relative path;
2. a reachable subset of one named original file, with every omitted/replaced
   expression marked locally as `DIVERGENCE(<ID>)` and indexed in that
   component's README; or
3. a minimally edited original file with the same local-marker/README rule.

A project-authored file is not an admissible fourth category. It must be moved
to its actual adapter, `session`, `app`, or test owner; merged into the
appropriate original file only when it is a documented minimal divergence; or
deleted. A README records exceptions to a mirror file; it does not authorize a
new non-mirror source file below the component root.

## S sequence

| S | Outcome | Exit condition |
| --- | --- | --- |
| S1 | Full file/hash/diff/marker/README ledger for the three components. | Every production file has a source identity and disposition; no remediation is inferred from path names. |
| S2 | `bx-core` repair: move its non-mirror header to the actual mechanical owner and make every retained Bochs diff hunk locally marked and README-indexed. | No project-authored source remains in `bx-core`; each retained upstream edit has a local marker and precise register row. |
| S3 | `opennt-guest` zero-difference closure. | Every retained guest input is versioned or explicitly excluded as a non-production artifact; the two source roots remain byte-identical. |
| S4 | `opennt-host` subset and minimal-edit closure. | Every non-exact host file is a named original subset/edit, its local markers match its README rows, and no non-mirror file remains. |
| S5 | Cross-component final audit and formal-build regression. | Hash/marker/index checks pass; component boundaries remain unchanged; formal Ninja focused closure and documentation governance pass. |

## Source roots

- Bochs: `O:\repos.external\bochs-2.6-compat\bochs-2.6`
- Guest: `refs/opennt/base/mvdm/dos/v86` and `refs/opennt/base/mvdm/wow16`
- Host: source paths recorded by the T261 ledger and each component README.

## Constraints

- Preserve original file names and relative paths inside a component root.
- Use `git mv` for a pure-owner relocation.
- Do not clean, stage, delete or reinterpret unrelated untracked historical
  tools, artifacts or guest inputs during this package.
- The central Bochs intrusion register remains evidence; `bx-core/README.md`
  is the local index required for every actual source change.
