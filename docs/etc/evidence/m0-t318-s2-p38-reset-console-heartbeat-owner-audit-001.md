# M0 T318 S2 P38 — Reset, Console, and Heartbeat Owner Audit

Date: 2026-08-31  
Status: static prerequisite audit; no new runtime observation

## Fixed-profile branch selection

The fixed container invokes the product with `-f -o --ordinary-child`.
Original `nt_reset.c` parses `-o` by setting `StreamIoSwitchOn = FALSE`.
`host_applInit` consequently does not enable stream I/O, and original
`host_reset()` selects:

```text
ConsoleInit()
-> MouseAttachMenuItem(...)
-> ResumeThread(ThreadInfo.HeartBeat.Handle)
-> host_ica_unlock()
```

This is the actual startup branch for every fixed-container observation; it is
not a headless or inherited-console approximation.

## Console disposition

The selected CPU40 graph does not define `X86GFX`.  The historical hardware
fullscreen, Console Server and display-state calls in that conditional region
are therefore not in this startup path.

The active windowed `ConsoleInit` path calls `initTextSection`, creates the
session focus event, obtains `GetConsoleScreenBufferInfo(sc.OutputHandle, ...)`
through `calcScreenParams`, and performs original screen/BIOS setup.  The
former text-section registration boundary is already source/link closed by
P19.  The remaining Console API precondition is a valid output console handle,
which the fixed launcher supplies as real `CONOUT$`; an API failure follows
original `ErrorExit`, not an unclassified access-violation continuation.

There is no reached `DisplayErrorTerm`/private CSR requirement in this
non-`X86GFX` `ConsoleInit` prefix.  Later full-screen transition and console
event code remains a separate, currently unentered owner path.

## Heartbeat disposition

`TimerInit` creates public Win32 synchronization events before `host_main`.
The timer device's original `host_timer_init` creates the heartbeat thread
suspended.  `host_reset` is its first normal resume point.  The selected
cdecl-to-WINAPI wrapper preserves the original `HeartBeatThread(PVOID)` call
form and session binding.

Once resumed, the original worker executes:

```text
Win32_host_timer
-> time_tick
-> RtcTick
-> cpu_interrupt(CPU_TIMER_TICK, 0)
```

under the original ICA lock.  These are machine/timer/PIC owner operations,
not BaseSrv, COMMAND, DEM, console, or BOP service calls.  Their complete
original provider set is the next static owner cohort.

## Result

No source change is justified in the console portion, and the fixed container
must not be rerun.  The next source audit proceeds through the complete
timer/PIC machine cohort before any further product change or observation.
