# M0 T294 S1 — MVDM host first-degree function audit plan

## Objective

Consume exactly the frozen 2,353 call-site rows in
`mvdm-host-first-degree-candidate-ledger.tsv`.  Resolve every row without
merging same-named functions merely by spelling, and emit each resolved
function's direct outgoing calls as frozen prospective second-degree edges.

## Inputs and source limits

- T293's first-degree candidate ledger is immutable input; every input row
  must have exactly one T294 result row.
- Definition and declaration bytes may be read only from the two approved
  original OpenNT trees, using recorded path, hash, line, linkage and package
  evidence. `src.old`, current product bodies, compiler output and external
  internet sources are prohibited inputs.
- Public modern Win32/CRT functions terminate as leaves after declaration and
  ABI/owner evidence. They are not recursively expanded through Windows or
  CRT source.
- CSR/CSRSS, NTDLL CSR transport, kernel VDM, Win32k, USER/GDI server and
  similarly private product shells are declaration-only hard boundaries.

## Required outputs

- `mvdm-host-first-degree-resolution-ledger.tsv`: exactly one row per frozen
  candidate call site, with exact selected definition identity, public leaf,
  declaration-only hard boundary, finite shallow replacement, or explicit
  source-unavailable result; final disposition and owner are mandatory.
- `mvdm-host-first-degree-definition-ledger.tsv`: one distinct resolved
  external definition identity per path/hash/line/linkage/signature, including
  its leaf outcome and source/package disposition. Same spelling in distinct
  files or build variants remains separate.
- `mvdm-host-first-degree-source-coverage-ledger.tsv`: every original source
  file selected by a non-public definition has source/hash/package coverage.
- `mvdm-host-second-degree-candidate-ledger.tsv`: every direct non-leaf
  outgoing call from a resolved first-degree definition, with caller identity,
  call line and source form only. T294 never reads that callee body.
- `mvdm-host-first-degree-call-resolution-ledger.tsv`: explicit conditional
  definition variants and macro/function-pointer/declaration-only outcomes,
  so no such case is silently lost or promoted to an invented implementation.

## Resolution order

1. Preserve each frozen call-site identity and determine whether its spelling
   is a public modern API/CRT leaf from source declaration evidence.
2. Otherwise locate all exact original definition candidates by spelling,
   path/hash/line and linkage. Resolve with caller declaration context and
   original package/product condition; retain conditional variants explicitly
   where a selected build is not yet chosen.
3. Classify each outcome as `direct`, `binding-only`, `adapter-backed`,
   `overlay-required`, `not-host-runtime`, `tool-only`, `hard-boundary`,
   `finite-shallow-replacement`, or `unresolved`.
4. Parse only the selected/resolved first-degree body. Every direct exit is
   recorded as a prospective second-degree edge; no exit body, package BFS,
   import, adapter or runtime implementation is admitted.

## Exit criteria

- All 2,353 T293 candidate IDs have exactly one result row; none is omitted or
  silently collapsed by spelling.
- Every non-public result has exact source/declaration evidence or an explicit
  unavailable/hard-boundary reason, owner and final disposition.
- Every resolved source body has direct-call evidence or an explicit leaf
  basis; all resulting second-degree exits are recorded but unexpanded.
- Conditional variants, macros and function-pointer forms have dedicated,
  source-evidenced dispositions.
- Generation assertions, documentation governance and `git diff --check`
  pass. No production source, build graph, adapter, BOP route or Bochs code
  changes.
