# M0 T313 S3 P5 — Original Timer Package Formal Closure

## Selected original package

The selected x86 CCPU40 formal graph contains both sides of the original
SoftPC timer contract:

- `softpc.new/host/src/nt_timer.c`, including `host_timer_init`,
  `Win32_host_timer`, `DelayHeartBeat` and the original heartbeat ordering;
- `softpc.new/base/system/timer.c`, including `time_tick`,
  `timer_generate_int` and `TimerGenerateMultipleInterrupts`.

The original source order remains:

`Win32_host_timer -> time_tick -> timer_generate_int ->
TimerGenerateMultipleInterrupts -> ica_hw_interrupt`.

`nt_timer.c` also performs its original RTC update, CCPU timer event and
`WOWIdle` notification in that heartbeat loop.  No project timer controller,
parallel executor or replacement source body is introduced.

## Formal evidence

The fresh external graph `build/M0-T313/S3/r2` completed all **403/403**
selected x86 CCPU40 edges.  Its generated graph contains:

- `obj/host/nt_timer.obj` from original `nt_timer.c`;
- `obj/system/timer.obj` from original `timer.c`.

This is a source/build closure fact.  The independently bounded original
`DelayHeartBeat` suspend/resume proof and the S3 P4 original PIC IRQ0 proof
remain the behavior evidence for the two ends of this chain.

## Deliberate limit

The formal archive does not by itself prove a live full-period PIT/BDA/EOI
cycle.  That cycle depends on the selected original timer state, BIOS Data
Area, EOI hooks and device initialization being run together.  It must not be
simulated by a fixture-local `time_tick` or a new heartbeat controller.  The
next S3 work can either exercise that complete original package coherently or
record an explicit bounded-unavailable result; it cannot claim the cycle from
the archive alone.
