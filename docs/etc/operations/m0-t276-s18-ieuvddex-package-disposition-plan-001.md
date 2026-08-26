# M0 T276 S18: W3 `ieuvddex` package disposition plan

## Objective

Audit every selected `ieuvddex` path as one original IEU/VDD extension package.
Record portable and architecture-specific build selection, original VDM_TIB/
context/event role and reached monitor/session/machine boundaries before
enabling any extension body.

## Non-goals

No source move/edit, VDD or IEU implementation, Bochs change, adapter body,
formal build edge, Ninja run or guest execution. This audit neither invents a
generic VDD provider nor permits raw host pointers or a second mapping manager.

## Acceptance

All 23 selected paths have one reproducible non-enabled disposition. The
evidence distinguishes portable and processor-specific sources, preserves
source-shaped VDD/monitor contracts, and assigns each reached external family
to `adapter-vdd`, `adapter-vdm-monitor`, the session mapping manager,
`adapter-softpc`, `adapter-bochs` or a separately named product boundary.
