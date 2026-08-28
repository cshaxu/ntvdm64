# M0 T304 S1 — fixed-width machine-event ingress plan

## Objective

Locate the reached original MVDM machine-event forms and define the smallest
selector-blind copied-record ingress boundary that can be implemented without
turning a machine callback into a BOP dispatcher.

## Scope

- Trace declarations, definitions and current bindings for reached monitor or
  SoftPC event records.
- Record exact field widths, producer/consumer ownership, stop/outcome
  direction and any existing source-shaped call site.
- Produce one implementation-ready S2 brief with a local x86/x64 test matrix.

## Exclusions

No BOP selector/service routing, guest tracing, device enablement, DEM or
COMMAND provider behavior, XMS/DPMI policy, or CCPU executor recovery.

## Exit

The S2 brief names a single copied mechanical event ABI, its adapter owner and
its failure direction. It may not include OpenNT service or guest semantic
fields.
