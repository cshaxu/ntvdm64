# Proposal: NTDOS/COMMAND Guest EXEC And Parent-Return Completion

## Purpose

Recover the guest-owned DOS `EXEC` lifecycle needed after host COMMAND child
completion: PSP/arena/JFN/environment setup, child execution, parent state
restoration and ordinary `54:0B` return. This is the missing owner package
between a closed host `cmdExec32` worker and continuous guest execution.

## Boundary

NTDOS/COMMAND guest sources own DOS process semantics. `bx-vdm` may only pass
the already-defined BOP result and checked guest-memory effects; bx-core and
bx-mantle retain CPU, interrupts and machine mechanics. No guest lifecycle is
reimplemented in the COMMAND host shim.

## Admission Plan

1. Map original NTDOS/COMMAND EXEC, PSP, arena, JFN, environment and return
   code to the staged source-built guest images and existing BOP contracts.
2. Recover each composable guest path from original source; classify missing
   machine or host prerequisites to their owners.
3. Verify one declared DOS child → parent ordinary-return profile, then link
   its remaining owner transfers into the cross-family audit.

## Exit Rule

Do not claim `54:0B` ordinary guest-parent continuity merely because the host
child worker returned an exit code. The declared guest profile must execute and
restore its parent state through original ownership.
