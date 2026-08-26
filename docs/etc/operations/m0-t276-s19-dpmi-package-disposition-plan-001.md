# M0 T276 S19: W6 `dpmi` package disposition plan

## Objective

Audit all selected `dpmi` paths as one original DPMI provider/control package.
Record original build selection, provider/data/control role and its reached
protected-machine, monitor, SoftPC/SAS, BOP and session boundaries before
enabling any DPMI body.

## Non-goals

No source move/edit, DPMI implementation, Bochs change, adapter body, formal
build edge, Ninja run or guest execution. This audit does not infer protected
mode, LDT, exception or memory semantics from a trace.

## Acceptance

All 59 selected paths have one reproducible non-enabled disposition. The
evidence preserves source-shaped DPMI interfaces and identifies the exact
preconditions that a later package must close through the existing adapters.
