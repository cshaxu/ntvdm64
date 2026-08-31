# M0 T318 — NTDOS/COMMAND Guest EXEC And Parent-Return Completion

## Purpose

Recover the guest-owned DOS `EXEC` lifecycle needed after host COMMAND child
completion: PSP/arena/JFN/environment setup, child execution, parent state
restoration and ordinary `54:0B` return. This is the missing owner package
between a closed host `cmdExec32` worker and continuous guest execution.

## Queue relationship

This is the admission plan for the leading queue candidate, **NTDOS guest EXEC and
parent-return recovery**.  It may be admitted only after its tracker rows and
machine prerequisites are frozen; its package boundary may not expand into a
COMMAND host broker or a machine-device task.
## Boundary

NTDOS/COMMAND guest sources own DOS process semantics. The established
`adapter-mvdm-host-out` and `session` boundary may only pass the already
defined BOP result and controlled session outcome. `mvdm-host` SoftPC owns
CPU, interrupts and machine mechanics. No guest lifecycle is reimplemented in
a host COMMAND provider, app, session, or adapter.

## Admission Plan

1. **S1 — Source and contract freeze.** Map original NTDOS/COMMAND EXEC, PSP,
   arena, JFN, environment and return
   code to the staged source-built guest images and existing BOP contracts.
2. **S2 — Original guest recovery and dual-width source closure.** Recover each
   composable guest path from original source; classify missing machine or
   host prerequisites to their owners. The same selected CPU40 package graph
   must compile and normally link under both Win32/x86 and Win32/x64. Only x86
   is a current runtime-observation row.
3. **S3 — Controlled-return evidence.** Verify one declared DOS child → parent ordinary-return profile, then link
   its remaining owner transfers into the cross-family audit.

## Exit Rule

Do not claim `54:0B` ordinary guest-parent continuity merely because the host
child worker returned an exit code. The declared guest profile must execute and
restore its parent state through original ownership. Do not claim x64 guest
execution from its required same-source compile/link closure.
