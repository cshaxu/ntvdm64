# T228 S1 P21 — Bounded Software-Interrupt CLI Observation Result

## Question

Can P20’s selector-blind, default-off machine records be consumed by the
existing diagnostic CLI lifecycle without becoming an adapter service or
altering Direct/Readonly guest execution?

## Build and Local Checks

The fresh diagnostic graph
`build/M0-T228-S1/p21-software-int-cli-20260818a` compiled the complete x64
`/MT`, CPU5/Pentium-MMX closure with
`softwareInterruptDiagnostic: true`; its CLI SHA-256 is
`562c2dff333c44f8649f6d6b9f6ca0a657760465a0c38dfecd58736c7460bb76`.
The P20 focused fixture passed. The guarded CLI option
`--observe-software-interrupts` was accepted in both Direct and Readonly
`--validate-only` paths.

A separate fresh default graph
`build/M0-T228-S1/p21-default-cli-20260818a` has
`softwareInterruptDiagnostic: false` and configuration hash
`819b3e14c0f0cbf9cece49845300503bb189b5cd34ec469a8103f31dfd2a520a`.
Its native CLI rejected the diagnostic-only option with usage exit code `2`.
Thus the normal product CLI does not expose or arm this profile.

## Paired Immutable Observation

The diagnostic CLI ran the hash-locked input manifest
`c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`
with a one-million-tick budget and the same constrained child environment in
both modes. Both runs returned the normal budget terminal exit code `4`:

| Mode | Elapsed ms | stdout SHA-256 | stderr SHA-256 |
| --- | ---: | --- | --- |
| Direct | 149.379 | `f29dcd9334efccdd68154f16a042e647d15e440d9cf6b9038134cad998359361` | `52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860` |
| Readonly | 115.012 | `f29dcd9334efccdd68154f16a042e647d15e440d9cf6b9038134cad998359361` | `52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860` |

Each output contains the same bounded 64-record ring. The retained records
are generic real-mode software interrupts, chiefly `21h` and one `2Fh`; they
are copied raw facts, not BOP or DOS-service attribution.

## Interpretation

P21 closes the diagnostic lifecycle integration: configure before the existing
worker, copy after join, reset on every observed completion/failure path, and
exclude the option from default builds. Direct/Readonly parity transfers only
the already mapped complete COMMAND/NTDOS transient/MCB owner question from
P19. It does not identify a repair, prove any BOP provider, or authorize a
trace-led leaf task.