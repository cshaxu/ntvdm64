# M0 T276 S25: final expected-integration disposition backfill

## Why this packet exists

S11–S23 produced source identity, build selection, package ownership and
interface-boundary evidence. Those records were intentionally non-enablement
audits, but they did not yet state the final product conclusion required for
each path. S25 converts that evidence into the one-row-per-path final tracker;
it does not revise source mirrors or implement adapters.

## Scope and order

1. Support/tool/firmware roots from S11/S12: settle direct candidates versus
   `tool-only`, `firmware-only` and `not-host-runtime` first.
2. Historical machine/control roots from S12/S15–S18: settle Bochs-replaced
   source, monitor product shell and debugger evidence before provider roots.
3. DOS/COMMAND/XMS/DPMI/DPMI32/Redirector roots from S13/S14/S19–S21: choose
   `binding-only`, `adapter-backed`, explicit non-runtime or `unresolved`
   only from their existing original caller/interface evidence.
4. WOW32/fax from S22/S23: preserve private USER/GDI, graphics-engine and
   broker conclusions rather than claiming modern public APIs are equivalent.

## Per-row requirement

Each row must state exactly one taxonomy value, final owner/link boundary,
mirror change class, named adapter (or `none`), x86/x64 mapping implication and
specific original/package evidence. `unresolved` is valid only with a concrete
missing decision or unavailable contract; it is not a placeholder for a body
that is merely not enabled.

## Stop conditions

Stop and retain `unresolved` if an end-state conclusion would require invented
behavior, an unreviewed private Windows/NT interface, a second mapping manager
or a guest pointer/HANDLE cast. Do not use a later trace or current compile
failure as a substitute for package evidence.

## Exit evidence

The verifier reports taxonomy-valid, nonblank tracker fields for all prior
package-audited rows, per-package counts and explicit evidence for every
remaining `unresolved` conclusion. Only then may a new original package enter
the audit queue.
