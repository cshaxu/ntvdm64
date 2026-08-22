# M0 Td S1 P5 — No-external-dependency BOP Governance

## Decision

The next implementation candidate is one medium-sized T, not separate low-
dependency and internal-dependency T packages. It contains every BOP entry and
OpenNT lifecycle/helper dependency whose complete declared path has no external
machine prerequisite, legacy/private API decision, or unresolved external
owner. Purely internal BOP/session/DAG dependencies belong in that same T.

The next candidate excludes bx machine/BIOS work, legacy or private API
compatibility, DPMI, Redirector, WOW16, VDD/debugger, guest EXEC/PSP and other
external-owner work. Those transfer to candidate 2, then candidate 3 performs
the integrated completion audit.

## Reconciled records

- `M0 T237 S5 P2` is committed and pushed as `e59681c2`; Status no longer
  describes it as pending push.
- The live tracker is reconciled to 102 complete, 2 partial and 99 unfinished
  BOP entries: XMS `52:06` and top-level `59` are complete; only `52:09` and
  `FE` remain partial.
- The former internal-dependency proposal is retained as superseded evidence;
  the queue links the expanded no-external-dependency proposal instead.

## Verification

This Td changes no product code, build graph, numeric T allocation or active
implementation scope. Documentation governance and inventory verification are
required before this record is accepted.
