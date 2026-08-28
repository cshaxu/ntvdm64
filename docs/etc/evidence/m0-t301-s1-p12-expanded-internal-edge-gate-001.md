# M0 T301 S1 P12 — Expanded Original-MVDM Internal-Edge Gate

## Scope

P12 consumes the 120 direct edges that P11 found from newly reached original
MVDM bodies to a unique original-MVDM internal candidate.  It applies the same
physical source requirements used by P9/P10.

## Result

- 80 edges are confirmed: either their call and body are in the same original
  translation unit with matching arity, or the caller's recursively reachable
  original header declares the target with matching call and definition arity;
- 40 edges remain unconfirmed cross-file candidates and stay visible for
  source-shape/frontier review; and
- no edge is admitted because of spelling alone.

The evidence ledger records the direct call line, selected definition identity,
call/definition/header arities, package-frontier membership and precise basis.

## Next use

The 80 confirmed edges may participate in the zero-degree rebase.  The 40
unconfirmed edges and P11's 223 direct exits remain first-degree audit inputs.
This does not yet replace the authoritative zero-degree or one-degree ledgers.
