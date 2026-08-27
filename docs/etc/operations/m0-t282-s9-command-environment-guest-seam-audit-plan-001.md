# M0 T282 S9 — COMMAND environment guest-memory seam audit

Before modifying any original environment/current-directory source, recover the
exact guest-memory contracts reached by `cmdComSpec`, `cmdGetCurrentDir` and
`cmdGetInitEnvironment`. Their historic `GetVDMAddr` calls are pointer-shaped
and lack explicit spans; an x64 product cannot convert them into unbounded
host pointers.

## Objective

For every reached guest read/write in the three service paths and their helper
calls, record segment:offset/address expression, source operation, minimum
span/termination condition, access direction, lifetime, layout and source
failure behavior. Decide whether the existing adapter-softpc bounded lease
facade can retain the source call shape, or whether the smallest per-call
mirror divergence/overlay is mandatory.

## Non-goals

No raw-pointer `GetVDMAddr` facade, generic guest mapper, source body change,
BOP route, host environment policy, or Bochs change.

## Exit criteria

Every reached pointer operation has a safe mapping disposition and no later
implementation is allowed to infer a span from a naked historical pointer.
The evidence names the exact smallest source changes, if any, before an
environment/current-directory behavior S can be admitted.
