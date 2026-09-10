# M0 T318 S2 P39 — Timer, PIC, and CPU40 Heartbeat Prerequisite Audit

Date: 2026-08-31  
Status: static prerequisite audit; no new runtime observation

## Scope

This audit closes the original machine-owner cohort reached when `host_reset`
resumes the heartbeat in the fixed `-f -o --ordinary-child` profile:

```text
HeartBeatThread -> Win32_host_timer
                -> time_tick -> SWTMR_time_tick
                -> RtcTick
                -> c_cpu_interrupt(CPU_TIMER_TICK, 0)
```

It does not add a timer, PIC, CPU, BOP, or guest-service implementation.

## Original initialization and ownership

The selected original BIOS reset path initializes the timer table before the
heartbeat can run:

```text
base/bios/reset.c
  SWTMR_init_funcptrs()
  time_of_day_init()
  timer_init()/timer_post()
```

`SWTMR_init_funcptrs` assigns `timer_tick_func = SWTMR_time_tick` in the
original `base/system/timer.c`; consequently `time_tick()` is not an
uninitialized generic callback in the NTVDM build.

For the NTVDM/CPU40 configuration, `base/system/ica.c` retains the original
direct PIC implementation.  Its required compatibility function-pointer
carrier is initialized at definition time to `ica_hw_interrupt`; the
non-NTVDM `SWPIC_init_funcptrs` branch is not selected and is not a missing
startup operation.

`cpu4gen.h` maps `cpu_interrupt` to the original CCPU40
`c_cpu_interrupt`.  The selected definition is in
`base/ccpu386/c_main.c`; `CPU_TIMER_TICK` therefore enters the original CCPU
interrupt map rather than a retired CPU30 stub or an adapter callback.

`rtc_init` in original `base/system/cmosnt.c` establishes host time, RTC
state and alarm state before `RtcTick`.  The heartbeat holds the original ICA
lock across `time_tick` and `RtcTick`, then releases it before making the
CPU-timer notification, matching the original ordering.

## Modern-host boundary

The active timer worker uses public Win32 events, `CreateThread`, waiting and
performance-counter APIs.  No current call in this cohort requires BaseSrv,
CSRSS, a private Console Server transport, or a new adapter.  Its real
console prerequisite was already isolated in P38 and is fixed by the formal
container's `CONIN$`/`CONOUT$` handles.

## Conclusion

No concrete source-selection or compatibility defect was found in the
complete timer/PIC/CPU40 cohort.  A product rebuild or another fixed-container
observation would change no product input and is therefore prohibited.  The
next static audit must continue along the original CPU40 startup execution
cohort that follows `host_start_cpu`, rather than adding a speculative timer
or PIC shim.  This remains no claim of NTIO, NTDOS, EXEC, or parent return.
