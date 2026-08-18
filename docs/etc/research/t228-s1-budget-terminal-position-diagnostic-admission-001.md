# T228 S1 budget-terminal position diagnostic source/ABI admission

## Question

After immutable Direct/Readonly runs reach original `$Exec` `50:36` but consume
100M ticks without child termination, what whole owner domain owns the next
continuity diagnosis?

## Existing mechanics

`src/bx-mantle/bx_ntvdm_machine_stage_v1.cc` owns the active minimal machine.
It already copies the active real-mode CS:IP through
`bx_ntvdm_machine_stage_v1_copy_real_mode_entry` without exposing a Bochs
object. Its execution function returns from `bx_cpu.cpu_loop()` before timer
unregistration and before `bx_ntvdm_machine_stage_v1_reset` destroys the
machine. `src/bx-mantle/bx_ntvdm_engine_run_v1.c` currently maps only the
opaque execution status and resets immediately; the CLI can consequently see
an execution-budget terminal but cannot see the last copied position.

## Selected boundary

Add one default-off, mantle-owned copied terminal-position observation. When
an active machine stage returns from `cpu_loop`, it may copy exactly current
real-mode CS:IP into a fixed-width record if observation is enabled. The
engine and CLI may query only that copied record after their existing run
returns. It must be absent by default and has no selector/service/BOP,
OpenNT/DOS, instruction bytes, guest-memory read, register write, callback,
Bochs object, device, firmware or host-capability field.

This is not a Bochs intrusion: it uses the already exposed mantle-owned CPU
position copy point and changes no `bx-core` source. It is narrower than the
registered instruction-history diagnostic, which remains unselected unless a
terminal position cannot classify the full continuity owner.

## Recovery ladder

1. Original OpenNT/SoftPC source cannot provide a current minimal-Bochs
   budget-terminal copy through this new mantle boundary.
2. Existing mantle ABI supplies the exact copied CS:IP primitive; selected as
   the smallest seam.
3. Bochs intrusion is not applicable.
4. The small default-off observation record is project-authored diagnostic
   plumbing only; it has no product execution semantics.

## Local acceptance

A native mantle fixture must prove: disabled state copies nothing; enabled
budget terminal records expected CS:IP; controlled generic stop does not
misreport a budget position; reset/next run clears the record. The regular
formal graph must preserve all normal fixtures with observation disabled.