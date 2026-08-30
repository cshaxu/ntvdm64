# CPU40-style CCPU original profile closure

## Queue status

This is the first candidate after the active T310 packet. Under task-identifier
governance it receives the next global `T` number only when admitted in
`docs/STATUS.md`.
It continues the selected CCPU40 owner-contract work after T310 without
reopening its closed packets.

## Objective

Recover and prove the sole selected original SoftPC profile:
`CPU_40_STYLE + CCPU`. The work must retain the original CCPU executor and
the original CPU40-style state/access structure. It must not create a second
x86 emulator, select Bochs, MONITOR or kernel VDM, or replace an available
SoftPC algorithm with a new adapter implementation.

## Why this is a separate T

The original CCPU40 configuration has a distinct generated state/access-table
contract, including protected-mode surfaces such as CPL, GDTR/IDTR, LDTR/TR
and MSW bits. It requires an independent source, ABI, initialization and
execution closure. Historical CPU30/MONITOR source remains mirror evidence,
not a supported product configuration.

## Planned S packets

1. **S1 — source/profile selection audit.** Establish exact CCPU40
   preprocessor selection, roots, generated headers and global state.
2. **S2 — CPU40 access-vector ABI closure.** Reconcile every selected
   `soft_ccpu_*` declaration, definition, initializer and caller on x86;
   retain unselected alternatives as original source evidence.
3. **S3 — protected-state contract.** Close CPL, GDTR/IDTR, LDTR/TR, MSW and
   flag access semantics, including fixed-width guest values and mapping-manager
   use only at an actual identity boundary.
4. **S4 — SAS/monitor and memory prerequisite disposition.** Recover only the
   CPU40 prerequisites demonstrably required by the original path; route later
   device owners forward instead of creating a substitute monitor.
5. **S5 — original lifecycle closure.** Prove creation, reset, CPU40 table
   initialization and `host_simulate`/stop/re-entry contract.
6. **S6 — formal x86 closure.** Build a fresh MSVC `/MT` x86 Ninja graph,
   audit link edges, then run a bounded source-shaped CCPU40 execution.

## Exit criteria

- CPU40 is a real selected build profile, not merely an excluded source branch.
- Its selected access table and protected-state calls have source-shaped,
  x86 contracts.
- A fresh x86 build and bounded execution reach the declared stop.
- Every unsatisfied external edge has one named owner, source contract and
  truthful unavailable/binding disposition.
- The result explicitly states whether CCPU40 is product-admissible. x64
  compatibility remains deferred product work. CPU30/MONITOR is retired
  historical source evidence and is not a deferred product route.
