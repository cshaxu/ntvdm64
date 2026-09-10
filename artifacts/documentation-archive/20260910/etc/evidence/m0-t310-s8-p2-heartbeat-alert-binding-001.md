# M0 T310 S8 P2 — original heartbeat alert binding

## Original control path

The selected original path remains intact:

```text
timer_init -> host_timer_init / TimerInit -> HeartBeatThread
    -> Win32_host_timer -> DelayHeartBeat
    -> time_tick / RtcTick / cpu_interrupt(CPU_TIMER_TICK, 0)
```

`TerminateHeartBeat` calls `NtAlertThread(ThreadInfo.HeartBeat.Handle)` while
`DelayHeartBeat` is in an alertable NT wait. The historical terminal result is
`STATUS_ALERTED`, followed by close/reset of the heartbeat handle and
`ExitThread(0)`.

## Modern same-shaped binding

`NtAlertThread(HANDLE)` is a private NT4 product primitive, not a supported
modern application interface. `ADAPTER-WIN32-020` retains the exact source
name, handle parameter and `NTSTATUS` direction. It queues a no-op APC through
public `QueueUserAPC`; the original alertable wait then returns
`STATUS_USER_APC`. `MVDM-HOST-DIV-040` extends only the original terminal
comparison so that this public completion has the same heartbeat-termination
meaning as `STATUS_ALERTED`.

This does not replace `nt_timer.c`, `timer.c`, `ica.c`, PIC delivery, RTC
update, BIOS-data-area update, or session teardown. In particular, it adds no
timer thread, no controller algorithm and no Bochs/MONITOR route.

## Conditional source correction

`MIPS_BIT_MASK` in the original `vint.h` denotes the x86/MIPS VDM distinction.
MIPS is not an admitted product guest. `MVDM-HOST-DIV-039` therefore makes the
reached `nt_msscs.c` location a true subset retaining only the original x86
bit-clearing line. Neither x86 nor x64 product graph defines the historical
`i386` macro globally. A future source unit that truly needs it must have its
own narrow, registered build exception.

## Formal verification

Outside the sandbox, both commands completed successfully:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310HeartbeatNinja.ps1 `
  -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p2-heartbeat/x86 verify

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310HeartbeatNinja.ps1 `
  -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p2-heartbeat/x64 verify
```

The fixture proves both failure for a null target and successful wake-up of a
real thread in `SleepEx(..., TRUE)`, which completes as `WAIT_IO_COMPLETION`.
That is the public Win32 completion represented by `STATUS_USER_APC` in the
original `DelayHeartBeat` binding.

The selected original `nt_msscs.c` and the adapter source also compile in the
formal x86/x64 original-SoftPC Ninja graph without a global `/Di386`.

## Worker-session binding follow-up

The same cdecl `CreateThread` bridge now captures only the creator's
thread-bound `session` in its private process-local context. Before invoking
the unchanged original callback it binds that session on the worker; it
unwinds the binding immediately after the callback returns. The original
`host_timer_init` call still passes `HeartBeatThread, NULL` and no session
identity is stored in a guest or fixed-width MVDM field.

The focused `thread-start-session-fixture` activates and binds one session on
the creator thread, starts a cdecl worker through the exact bridge, checks the
worker sees that same session, then verifies normal unbind/disposal. The
combined alert and worker fixtures passed under formal MSVC `/MT` Ninja on
both x86 and x64. This proves worker ownership only; it does not yet claim
that the original heartbeat has reached BDA, PIT, RTC, or PIC delivery.

## Remaining P2 work

The still-open group is original heartbeat creation/resume, session-owned
stop/teardown, BDA write validation, timer/PIC interrupt delivery and the
original `nt_eoi.c` delayed-interrupt path. Those need direct original-source
composition evidence before P2 can close.
