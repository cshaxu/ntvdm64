# M0 Td S1 P4 — BOP Owner-Package Sequencing Governance

## Decision

This standalone Td delivery records the owner-directed order for the next
candidate packages.  It allocates no numeric T and admits no implementation
work.

1. The first eight candidates each close one complete BOP owner package or the
   BOP-reachable machine/BIOS selector family: DEM, COMMAND, XMS, DPMI,
   Redirector, WOW16, debugger/VDD/top-level, then bx machine/BIOS/selectors.
2. The ninth candidate is a cross-family BOP completion audit and integrated
   verification task.  It reconciles the full catalogue and runs declared
   Direct/Readonly and native integration matrices, but cannot create
   trace-selected leaf fixes.
3. The tenth candidate is Overlay mutation-capability completion.  It remains
   after the BOP audit so Overlay is not used to hide incomplete Direct or
   Readonly owner behavior.

## Rationale

The order preserves whole-owner source recovery and prevents trace-led
incremental BOP work.  It also separates implementation closure from the
later question of whether all package routes compose under real guest
execution.

## Verification

The Queue contains exactly ten unnumbered candidate packages in this order;
items 1--8 cite their owner-package proposals, item 9 cites the new
cross-family-audit proposal, and item 10 cites the Overlay proposal.  The Td
changes no product source, build output, numeric T assignment, or active
implementation scope.
