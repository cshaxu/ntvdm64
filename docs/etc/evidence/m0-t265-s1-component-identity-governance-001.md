# M0 T265 S1 — Component Identity And Dependency Governance

## Question

What target ownership names and dependency directions must govern the Bochs
component migration before any source path or build input moves?

## Inputs And Procedure

Reviewed the owner-approved T265 proposal, current architecture/coding
authorities, architecture/coding rules, and T264's completed filename closure.
Updated only the authorities, active packet, queue and proposal; no production
source, build manifest, include path, or fixture was changed.

## Result

- The adopted mirror is named `bochs-core`; the Bochs-only assembly layer is
  named `adapter-bochs`.
- The only direct production edges are `app -> adapter-bochs -> bochs-core`.
  OpenNT-facing machine requests use an opaque endpoint installed by `app` in
  the neutral `session` contract, rather than adding a direct Bochs edge.
- Each `*-overlay` is private to its same-root native mirror. It has no public
  ABI and is not a direct app, adapter, session, fixture or other-mirror input.
- The old `src/bx-core` and `src/bx-mantle` names remain the actual source-tree
  paths until the separately admitted S2 `git mv` packet. This S1 result does
  not claim that the source migration has happened.

## Verification And Follow-up

The four architecture/coding authorities contain no target-name occurrence of
`bx-core` or `bx-mantle`; the proposal and Status agree on M0 T265 S1; and
the admitted candidate has been removed from the queue. S2 must inventory and
move the actual roots, build graph and fixtures using `git mv`; it may not
alter the ownership or dependency policy established here.
