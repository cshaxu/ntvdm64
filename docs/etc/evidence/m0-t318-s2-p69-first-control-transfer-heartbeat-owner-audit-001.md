# M0 T318 S2 P69 — First Control-Transfer Heartbeat Owner Audit

## Question

Once original CCPU has fetched NTIO's initial relative jump, which complete
original host-owned path may run before the next guest instruction?  The
question is intentionally source-first: fixed-console output is not used as an
instruction counter.

## Original CPU40 Transition

The selected `NTIO.SYS` starts with `E9 6D 03`, a 16-bit relative jump from
`0070:0000` to its `0070:0370` `msinit.asm::init` entry.  In the original
`softpc.new/base/ccpu386/c_main.c`, opcode `E9`:

1. decodes its 16-bit relative operand;
2. calls original `JMPR`/`update_relative_ip`;
3. cancels the cached host instruction cursor; and
4. leaves quick mode so that the original inter-instruction event checks run
before `NEXT_INST` fetches the target.

This is ordinary original CCPU control-transfer behavior, not a BOP route or
an adapter-created state machine.

## Heartbeat Candidate

The selected NT host reset path resumes the original heartbeat thread in
`softpc.new/host/src/nt_reset.c::host_reset`.  Its original
`nt_timer.c` loop eventually calls `cpu_interrupt(CPU_TIMER_TICK, 0)`.  CCPU's
inter-instruction check consumes that flag through `host_timer_event`.

The retained original `host_timer_event` body fans out to these owner units:

- C-VID/video macro `host_graphics_tick` and original `GetScaleEvent`;
- original idle policy `IDLE_tick` and `PrioWaitIfIdle`;
- original communication, printer and floppy heartbeat providers;
- original `DemHeartBeat`;
- original `time_strobe`; and
- original continuous-tone provider.

All are existing original source owners.  The heartbeat event is conditional:
this audit does **not** assert that a tick was pending in any particular fixed
run, nor does it attribute a stall to any member of the fan-out.

## Disposition

The first post-control-transfer external candidate is therefore the full
original heartbeat owner cohort, not an individual BOP or guest instruction.
The next recovery audit must examine its selected bindings and initialization
order as one unit.  It must not bypass the heartbeat, suppress CCPU event
checks, or replace NTIO/DEM execution merely to advance a trace.

## Evidence Limits

P68 established the entry CPU state only.  Its state reporter was removed
because later console writes affected the bounded display trace.  This P69
uses only static source order and makes no additional runtime-reachability
claim.
