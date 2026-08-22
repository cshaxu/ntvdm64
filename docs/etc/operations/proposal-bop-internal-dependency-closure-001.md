# Proposal: Internal-BOP Dependency Closure

## Purpose

Close the second dependency layer: BOPs whose remaining work is composition
of already declared internal BOP/session contracts. This includes shared
state, continuation, handle/token, DTA/FCB, bootstrap and other lifecycle
links only where the tracker proves that no external capability is missing.

## Strict boundary

This package may consume contracts completed in the first candidate and may
complete internal links among its own source-owner groups. It must not:

- add CPU, memory, A20, BIOS, interrupt, device, port or firmware behavior to
  `bx-core` or `bx-mantle`;
- call, emulate, or rely on a legacy/private/removed Win32 or NT4 API;
- reopen Redirector, WOW16, DPMI, VDD/debugger, host broker, or guest EXEC
  behavior merely because an internal BOP reaches their boundary.

When a source branch names one of those dependencies, the item remains
partial and transfers to candidate 3 with a tracker row. It is not made
successful by an adapter fallback.

## Admission plan

1. Produce an acyclic dependency slice from `BOP-…` and
   `BOP-DEPENDENCY-…` records; identify its roots as completed candidate-1
   contracts.
2. Recover each original dispatcher/module group as a unit, preserving
   layout, ordering, continuation and original failure behavior.
3. Run whole-slice regressions across producer/consumer BOPs, including
   cleanup and rejected external branch tests, then one bounded native
   observation.

## Exit rule

All selected paths have complete source/ABI and internal lifecycle evidence.
Any remaining gap has a named external owner and is transferred to candidate
3; no hidden external seam remains in an “internal” closure claim.
