# T176 S20 — Post-Termination Continuation Correlation 001

## Question

Does the accepted S19 `50:3C` continuation lead immediately to COMMAND
`54:11`, or does original guest DOS own intervening process teardown?

## Trace facts

S19 records the exact guest sequence:

1. target `SHARE.EXE` invokes INT 21h at `0D10:0106`;
2. the terminated process reaches `50:3C` at `0032:532F` with `BX=0D00`;
3. the finite provider resumes at `0032:5333` with zero CPU delta;
4. several subsequent guest INT 21h/vector events occur; `54:11` is absent;
5. only later does the pre-existing native stack-prefetch/third-exception
   failure occur.

## Source facts

`src/opennt/base/mvdm/dos/v86/doskrnl/dos/msctrlc.asm:971-1031` is the
original exit-inner sequence. It emits `SVC_PDBTERMINATE` at line 977, then
immediately invokes `arena_free_process` (line 980), `DOS_ABORT` (line 988),
restores the parent `CurrentPDB`, clears abort state, and restores the parent
PDB's saved user stack. `arena_free_process` is guest DOS arena logic in
`alloc.asm:54-99`; it is not an adapter or Bochs service.

The trace's next instruction pointer (`5333`) is therefore consistent with
the immediately following original guest teardown sequence. There is no
source basis to expect a COMMAND `54:11` directly after `50:3C`. COMMAND
calls `SVC_CMDGETNEXTCMD` (`54:01`) at its own request point in
`cmd/command/tcode.asm:557`; it can only be re-entered after the guest DOS
process cleanup and restored parent execution reach that COMMAND continuation.

## Interpretation

High confidence: the absence of `54:11` in this bounded trace is not evidence
that the newly accepted `50:3C` provider failed. The immediate owner after
`50:3C` is guest DOS process teardown; specifically, arena release, DOS abort
and parent-stack restoration. No adapter provider, VDD, device or BOP change
is warranted.

Medium confidence: the later stack-prefetch failure prevents reaching the
restored COMMAND continuation. The trace establishes temporal ordering, but
does not itself bind the later first bad stack/control state to a particular
guest source instruction. That is a separate Bochs-mechanics/source-correlation
question, not a BOP-family expansion.

## Follow-up

T176 S21 may compare the source-required parent-stack restoration with the
already recorded native stack-fault transition, strictly as a Bochs mechanics
audit. It must not add BOP behavior or reopen host-service composition.
