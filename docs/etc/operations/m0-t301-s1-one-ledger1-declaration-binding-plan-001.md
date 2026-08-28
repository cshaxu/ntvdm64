# M0 T301 S1 P19 — `one-ledger1` Declaration and Linkage Binding Plan

## Objective

Refine P18's physical definition-candidate rows without examining a candidate
function body.  For each direct one-degree caller, reconstruct the reachable
original include/declaration frontier; record matching declaration identities,
candidate linkage, and whether an exact physical binding is unique, ambiguous
or absent.

## Scope

- Read the committed `one-ledger1` call/resolution/candidate ledgers and the
  approved OpenNT/OpenNT-4.5 source union.
- Preserve caller path/hash/line/symbol and all candidate identities.
- Treat modern public API/CRT declarations, macros, assembly/import-only
  interfaces and source absence as explicit outcomes, never fabricated source
  definitions.

## Non-goals

No callee-body inspection or second-degree expansion; no package import,
adapter/overlay/provider implementation, build/link admission, BOP enablement
or runtime/Bochs change.

## Acceptance

Every P18 physical definition candidate has an include/declaration/linkage
disposition.  A candidate is selected only when its original declaration and
physical source identity prove it; all other alternatives remain explicit.
