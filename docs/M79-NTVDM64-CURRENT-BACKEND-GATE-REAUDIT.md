# M79 Current Backend Gate Reaudit

Status: completed read-only source reaudit, 2026-08-08. Source examined:
`D:\home\repos.hobby\ntvdm64\src\core\machine`. No source in that
repository was modified.

## Question

M70 admits a wrapper only after two neutral facilities exist with a separate
first-party consumer, focused core tests, and a narrow copied-value external
ABI:

1. configured decode-point observation; and
2. paused ordinary-RAM multi-span all-or-none transaction.

This reaudit tests the current source rather than inferring either facility
from the presence of recent machine or PC/AT work.

## Current Evidence

| M70 need | Current implementation evidence | Result |
| --- | --- | --- |
| Configured decode-point observation | `undefined_instruction_transition_interface.h` still exports only `core_machine_register_undefined_instruction_transition`. `cpu_instructions.c` calls its dispatcher only when execution raised `VCPUINS_EXCEPT_UD`, and only when `CR0.PE` is clear. `machine.c` matches frozen byte patterns after that fault. | Not present. This is a bounded real-mode `#UD` recovery hook, not observation of an otherwise valid configured instruction. |
| Paused ordinary-RAM multi-span transaction | `memory_interface.h` exposes checked one-range read, write, and query operations only. No transaction/begin/commit/rollback API exists in `src/core/machine`. | Not present. |
| Entry-time preload | `entry_plan_interface.c` preflights a bounded list of non-overlapping ordinary-RAM preloads while stopped, then assigns a real-mode CPU candidate and performs individual writes. It has no transaction handle, lifecycle invalidation, generic paused use, publication rollback, or external ABI. | Useful foundation only; not a substitute for the M70 transaction. |
| Frozen registration discipline | `machine.c` rejects registration after configuration closes and freezes the existing undefined-instruction registry. | Useful foundation only; its trigger and context remain too narrow for M70 candidate A. |

The current `#UD` hook remains correctly constrained: its handler receives
copied bytes, GPRs, and FLAGS, and core owns instruction-pointer advancement,
the approved FLAGS mask, and the finite resume/stop/fault outcomes. It does
not create an opening for arbitrary mode, segment, mapping, device, or IRQ
mutation. That discipline should be retained if a separate configured-decode
facility is ever admitted.

## Recent PC/AT Work Does Not Close This Gate

The current `ntvdm64` planning status records active FDC work and prior
machine foundations such as RTC, ROM mapping, checked memory, entry plans,
and the bounded `#UD` transition. Controller extraction may be independently
justified for NXVM/NXVDM consumers, but it neither supplies an executable
decode observation point nor proves the all-or-none ordinary-RAM publication
contract. It therefore does not constitute the second consumer required by
M70 for either candidate.

## Boundary Result

The M70 gate remains closed. `ntdos64` must continue to avoid a local CPU
executor, decoder, raw guest-memory mapping, synthetic BOP/DEM dispatcher,
or serial-write imitation of a transaction. The next admissible work here is
source/behavior-oracle extraction and wrapper-policy preparation; runtime
integration begins only after a core owner presents the M70 admission evidence.

## Direct Source Basis

- `D:\home\repos.hobby\ntvdm64\src\core\machine\undefined_instruction_transition_interface.h`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\cpu_instructions.c`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\machine.c`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\memory_interface.h`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\entry_plan_interface.c`
- `D:\home\repos.hobby\ntvdm64\docs\planning\status.md`

This record updates the conclusion of M58; it does not revise the M70
contract.
