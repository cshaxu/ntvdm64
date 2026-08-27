# M0 T294 S1 — MVDM host first-degree function audit closure

## Scope and frozen input

This closure consumes the 2,353 immutable call-site rows emitted by T293 in
`mvdm-host-first-degree-candidate-ledger.tsv`.  It neither imports a source
package nor changes any runtime, adapter, build graph, BOP route, or Bochs
mechanics.

The authoritative generated results are:

- `mvdm-host-first-degree-include-frontier-ledger.tsv`;
- `mvdm-host-first-degree-resolution-ledger.tsv`;
- `mvdm-host-first-degree-definition-ledger.tsv`;
- `mvdm-host-first-degree-source-coverage-ledger.tsv`;
- `mvdm-host-first-degree-call-resolution-ledger.tsv`; and
- `mvdm-host-second-degree-candidate-ledger.tsv`.

## Procedure

`export-t294-s1-include-frontier.mjs` starts from each selected original
caller and follows only its OpenNT-local `#include` closure.  It resolves a
qualified include only when content identity is unique; for a bare name it
uses the caller's MVDM package first, then a unique MVDM-only identity.  It
does not collect arbitrary same-named SDK, CRT, or unrelated product headers.
An ambiguous include terminates at declaration evidence instead of expanding
the search.

`export-t294-s1-first-degree-function-ledgers.mjs` searches function bodies
only in the package roots allowed by that frontier.  It recognizes ordinary C
and OpenNT K&R definitions, excludes prototypes and conditional call
expressions, records path/hash/line identities, and freezes only direct
outgoing calls.  It never reads a prospective second-degree callee body.

The scripts enforce one-result-per-frozen-ID and reject a source definition
for a terminal frontier row.

## Results

| Result | Call sites |
| --- | ---: |
| Public modern Win32/CRT terminal leaf | 1,065 |
| Caller belongs to a non-runtime original package | 855 |
| Exact original OpenNT definition | 188 |
| Same-shaped named adapter required | 126 |
| Declaration-only private/kernel/product boundary | 89 |
| Macro or function-pointer contract | 21 |
| Public modern Win32 binding selected from source shape | 9 |
| **Total** | **2,353** |

The 188 exact-definition call sites resolve to 53 distinct original
path/hash/line identities in 13 source files.  Their direct bodies yield 582
prospective second-degree call-site rows.  Those rows are frozen output for
the next degree only; no body behind them was examined here.

Every result row has a non-empty final disposition and owner.  The final
disposition totals are 1,074 `binding-only`, 746 `not-host-runtime`, 314
`adapter-backed`, 110 `hard-boundary`, and 109 `tool-only`; no row remains
`unresolved`.

## Interpretation and limits

- `adapter-backed` is a disposition, not an implementation claim.  It names
  the existing owner family (`adapter-mvdm-host-out/softpc`, `monitor`,
  `wow`, `redir`, `vdd`, or `win32`) that must preserve the historical call
  shape if/when its owner package is admitted.
- The 855 non-runtime caller exits remain source/contract evidence.  In
  particular, historical alternate SoftPC executor/device bodies do not
  create work to recover a second CPU or machine beneath Bochs.
- Kernel VDM, CSR/CSRSS, native-NT/private product shells and macro/function
  pointer forms are explicit stopping boundaries, not failed searches.
- This is lexical source analysis rather than a selected historical compiler
  build.  Conditional source variants retain separate path/hash/line evidence
  where applicable; build/profile selection belongs to a later owner-package
  admission.

## Verification

The final generator run reported:

```text
input candidates: 2353; symbols: 116; definition identities: 53;
source files: 13; second-degree exits: 582
blank-owner=0 blank-disposition=0 results=2353
```

`git diff --check` passed after generation.  Documentation inventory and
governance verification are recorded with the delivery.
