# M0 T384 — CPU40 illegal-operation startup recovery

## Purpose

Recover the earliest source-owned blocker of pure DOS startup: the original
CPU40 `illegal_op_int` path reached after COMMAND initial environment and
before the first `54:01` result.

## Ordered subtasks

1. **S1 — CPU40 illegal-operation source/guest-state ledger.** Trace the
   original CPU40 dispatch for opcode `0x63`, the interrupt-frame producer,
   `illegal_op_int`, `unexpected_int` and the original error contract; map
   the `03f4:2128` byte location to the reached guest/control-flow producer.
   Select one complete owner cohort.  No run.
2. **S2 — Selected original CPU40 cohort recovery.** Restore only the proved
   CPU40/SoftPC instruction, interrupt-frame or guest-state prerequisite with
   original ordering and failure semantics.  An error dialog may not be
   converted into an unconditional continue.
3. **S3 — Formal link and focused CPU40 contract proof.** Rebuild x86 and
   prove the selected instruction/frame/error directions with current source.
4. **S4 — One fixed pure-DOS `EXIT` observation.** Reuse the fixed stage and
   observer once.  Success is a first `54:01` return; child/native execution
   remains out of scope.

## Boundaries

No COMMAND/Base VDM/provider patch, guest-media alteration, app-owned
executor, `cmdExec32`, DOS child/PSP change, WOW, Redirector, CPU30, Bochs or
x64 recovery belongs to this task.  Original CPU40/SoftPC is the owner.
