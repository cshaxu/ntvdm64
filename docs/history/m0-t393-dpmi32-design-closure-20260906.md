# M0 T393 — DPMI32 design audit closure

## Scope

T393 was admitted only for source/dependency design audit of the reached
DPMI32 boundary.  It did not admit source changes, graph changes, guest-media
changes or runtime claims.

## Result

The audit confirmed that the original DPMI32 x86 provider is a complete
16-body owner cohort behind `MS_bop_3` / `53:00`–`53:18`; the formal SoftPC
generator already compiles the selected DPMI32 source package.  The next
owner handoff is WOW32 at `MS_bop_1`, whose original provider has 77 active
bodies and 105 declaration carriers.

The audit also found that the current host-output `softpc` adapter family
mixes machine mechanics with COMMAND, VDD, lifecycle/observation and package
media semantics.  Continuing directly into DPMI32/WOW32 runtime recovery
would combine original-owner recovery with an unbounded adapter ownership
migration.

## Closure decision

T393 is closed as a design-only task.  No S2 is admitted.  The next queue
candidate is the complete adapter ownership migration; only after it closes
may a new task admit the complete DPMI32/WOW32 owner packages.

## Evidence

[T393 S1 DPMI32 source and dependency audit](../etc/evidence/m0-t393-s1-dosx-source-dependency-audit-001.md).
