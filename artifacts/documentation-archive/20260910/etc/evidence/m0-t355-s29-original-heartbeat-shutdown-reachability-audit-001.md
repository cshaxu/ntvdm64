# M0 T355 S29 — original heartbeat shutdown reachability audit

## Question

Why does the fixed CPU40/x86 product reach session disposal with the original
`nt_timer.c::HeartBeatThread` binding still live when the selected source
already contains `TerminateHeartBeat`?

## Original source route

- The normal source route is `host_main -> host_start_cpu -> host_applClose`.
  `host_applClose` invokes `TerminateHeartBeat`.
- `TerminateHeartBeat` calls `NtAlertThread` and waits for the heartbeat
  handle.  The selected public adapter delivers that alert with an APC; the
  original alertable `DelayHeartBeat` wait accepts `STATUS_USER_APC` and exits
  through the retained `ExitThread` body.
- The original `host_applInit` branch without historical `-f` ends with
  `ExitProcess(0)`.  The current session replacement initially escaped before
  the close cohort, so S29 restored `host_applClose` before that session escape.
- All three upstream source baselines instead rely on `ExitProcess(0)` at that
  branch.  They also contain an independent `base/bios/reset.c` direct
  `host_terminate()` route.  In NT4 that direct route likewise relied on
  process termination to destroy remaining workers.

## Selected binding and build facts

- `nt_timer.c` is force-included with the selected CDECL thread bridge and
  selected `ntexapi` declarations in the formal CPU40/x86 Ninja graph.
- The bridge binds the original worker under its source spelling and releases
  it before a retained `ExitThread` call.
- `NtAlertThread` and alertable `NtWaitForMultipleObjects` are existing public
  Win32 same-shaped adapters.  Static reachability alone does not prove that
  the observed termination route reaches either call.

## Single observation

After restoring the early `host_applInit` close call, one fixed-container run
at `O:\ntvdm\m0-t355-s29-heartbeat-close.txt` still recorded:

```text
exit=0x00000048
MVDM-SESSION-DISPOSE reason=binding-count code=2 total=1 entry=0 worker=1 unspecified=0 worker-source=HeartBeatThread
```

Therefore the early no-`-f` replacement is not the observed source exit path.
This observation neither proves a broken alert adapter nor authorizes a new
timer implementation.

## Conclusion and successor

The remaining source-owned gap is complete process-exit cleanup for direct
`host_terminate()` routes.  S30 recovers that exact cohort: make original
`host_applClose` idempotent for one host start and invoke it before every
selected `host_terminate` session escape.  It does not force-unbind workers or
invent a parallel cancellation mechanism.
