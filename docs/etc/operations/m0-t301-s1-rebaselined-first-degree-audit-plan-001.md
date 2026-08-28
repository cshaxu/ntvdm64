# M0 T301 S1 — Rebaselined MVDM First-Degree Audit Plan

## Objective

Consume every physical boundary-call row emitted by T300. A row is first
degree only because its resolved caller is inside the selected OpenNT MVDM
source closure and that direct callee lies outside it. This packet establishes
the exact original declaration/body disposition of each such call without
selecting a runtime provider.

## Inputs and boundaries

- `mvdm-first-degree-rebaselined-boundary-ledger.tsv` is immutable input: it
  contains 34,429 physical call occurrences.
- `mvdm-first-degree-function-candidate-ledger.tsv` is an index only. It has
  3,963 spellings but cannot merge identities by spelling.
- Definition and declaration bytes may be read only from the approved
  `OpenNT` and `OpenNT-4.5` source trees. `src.old`, product mirrors,
  adapters, build output and runtime traces are forbidden inputs.
- Public modern Win32/CRT interfaces terminate as source leaves. CSR/CSRSS,
  NTDLL CSR transport, kernel VDM, Win32k and comparable private product
  shells terminate as declaration-only hard boundaries.
- A selected first-degree body may be parsed for direct calls only. No
  second-degree callee body, source import, adapter or product binding may be
  introduced.

## Procedure

1. Reconstruct each caller’s original physical source identity from T300’s
   zero-degree closure, then compute its original include frontier.
2. Resolve the call by declaration-guided package roots. If no constrained
   body exists, use the approved source union only to enumerate unselected
   variants or prove source absence.
3. Preserve every path/hash/line/linkage/signature identity. Same spelling in
   distinct bodies, editions or conditional forms remains distinct.
4. Classify every call as `public-leaf`, `original-definition`,
   `hard-boundary`, `finite-shallow-replacement`, `macro-or-indirect`, or
   `source-unavailable`, with a final static disposition and owner.
5. Parse only selected original first-degree bodies and emit their direct exits
   into an initial, unexpanded second-degree ledger.

## Required outputs

- a per-call include-frontier and resolution ledger;
- a physical first-degree definition ledger with variants retained;
- a source-file coverage ledger;
- a first-degree call-to-definition identity ledger; and
- an unexpanded second-degree initial-candidate ledger plus closure evidence.

## Exit criteria

Every T300 boundary-call ID has exactly one result row; every selected original
first-degree body has exact source identity and direct-call evidence; public,
hard-boundary, macro/indirect and unavailable outcomes remain explicit; and no
second-degree body is read. The documentation inventory/governance checks and
`git diff --check` pass.
