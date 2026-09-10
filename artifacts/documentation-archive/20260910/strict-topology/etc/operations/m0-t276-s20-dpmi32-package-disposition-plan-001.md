# M0 T276 S20: W6 `dpmi32` package disposition plan

## Objective

Audit all selected `dpmi32` paths as one original 32-bit DPMI package. Record
original build selection, provider/control/data role and reached protected
machine, monitor, session, BOP and guest-memory boundaries before enabling any
body.

## Non-goals

No source move/edit, provider implementation, Bochs change, adapter body,
formal build edge, Ninja run or guest execution.

## Acceptance

All 25 selected paths have reproducible non-enabled dispositions and identify
the exact source-shaped prerequisites for any later package recovery.
