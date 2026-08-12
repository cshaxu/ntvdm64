# M86 Core Second-Consumer Candidate Audit

Status: completed read-only audit, 2026-08-08. Source examined:
`D:\home\repos.hobby\ntvdm64`. No source in that repository was modified.

## Question

M70/M79 require a real independent first-party consumer before `ntvdm64/core`
can admit either generic configured decode observation or a paused ordinary-RAM
multi-span transaction. This audit tests whether the current NXVM firmware,
debugger, reset, entry-plan, or mantle-shape code already supplies that
consumer.

## Configured Decode Observation

No current independent consumer exists.

- `src/vm/machine/debug.c` implements breakpoints by comparing the retained
  CPU `CS:EIP`/linear state in `vm_machine_debug_refresh`, before each run
  quantum. It does not match instruction bytes or receive a CPU decode event.
- `src/vm/composition/session/runner.c` invokes that refresh outside
  `core_machine_run`; turning it into a configured-byte observer would change
  retained debugger timing and requires a separately approved debugger feature
  contract.
- Default-profile firmware is assembled into ROM and uses normal CPU/firmware
  execution. `src/vm/composition/session/profile_firmware.c` contains no
  transition-registration call.
- The only current registration call is the focused core corpus in
  `tests/core/machine_undefined_instruction_transition_smoke.c`. Test coverage
  is not a product consumer.

The existing `#UD` interface remains correctly useful for its narrow purpose,
but it is not an admissible generic decode point: it fires only after a
real-mode undefined-instruction exception.

## Paused Ordinary-RAM Multi-Span Transaction

No current independent consumer exists.

- `src/vm/profile/default_profile/firmware/bios.c` restores IVT and BDA bytes
  one at a time during reset. `src/vm/composition/session/runner.c` performs
  reset before resuming the guest run loop; no existing observable partial
  publication defect establishes a transaction requirement.
- `core_machine_apply_entry_plan` already preflights bounded ordinary-RAM
  preloads and applies them at a stopped reset boundary. It is an initial-state
  facility, not a general transaction. The only mantle-oriented consumer,
  `tests/machine/core_mantle_shape_smoke.c`, uses one entry-plan preload and
  is explicitly a shape test.
- Debugger reads/writes are currently individual operations through the
  paused-boundary borrow adapters. No retained debugger command requires one
  atomic multi-range patch.

Calling either reset restoration, a test fixture, or a hypothetical debugger
paste operation a consumer would merely reverse-engineer the desired NTDOS
facility into core.

## M6/Mantle Status

`ntvdm64/TODO.md` names M6 mantle as a future neutral lifecycle/execution
envelope. It is a legitimate *potential* future consumer, but it is not
implemented and cannot satisfy the evidence rule today. A mantle design that
later requires a dynamic all-or-none state publication may become the second
consumer only after it has its own product-neutral contract and focused tests.

## Decision

The M70/M79 gate remains closed. Do not request either extension from core
solely for this external profile, and do not create a local substitute in
`ntdos64`. The first legitimate admission must start from a separate NXVM or
NXVDM/M6 behavior with all of the following evidence:

1. a documented user-visible or machine-consistency requirement;
2. a source-owned call site outside a test and outside any NT4 adapter;
3. a neutral contract with finite outcomes and lifecycle constraints; and
4. focused core and product regression evidence.

Until then, the next runnable-profile work remains blocked after M84/M85;
guest source closure and historical behavior-ledger work may continue without
pretending to provide execution.
