# M0 T355 S30 — source-owned direct-termination cleanup recovery

## Scope

S30 recovers the cleanup that NT4 received implicitly when a dedicated
`ntvdm.exe` process exited. The selected product instead returns only the
current session to the application, so the original source-owned close cohort
must run before that escape. This packet changes neither guest state, BOP
business logic, CPU behavior, nor session binding ownership.

## Recovered source-shaped route

- `softpc.new/host/src/nt_reset.c::host_applClose` is now a once-per-original-
  host-start close cohort. It retains its original cleanup order, including
  `TerminateHeartBeat`.
- The original no-`-f` startup escape invokes that cohort before passing its
  original completion to the session boundary.
- The original direct `host_terminate` route invokes that same cohort before
  its selected session termination replacement. This covers reset paths that
  historically relied on process teardown.
- The bounded application composition invokes the same original close body
  after either an original entry return or its selected session escape and
  before releasing session memory or the entry binding. The original body
  rejects pre-start and repeated calls.
- `NtAlertThread` prefers the same-named `ntdll` entry when it is exported by
  the running system, with the existing public APC wake-up retained only as
  an unavailable-entry fallback. No generic timer/cancellation model or
  force-unbind path is introduced.

## Formal build

The formal CPU40/x86 Ninja graph rebuilt the product after this cohort was
linked:

```text
tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86
  -BuildRoot build/M0-T355/S20/formal-x86 -ParallelJobs 8
build/M0-T355/S20/formal-x86/original-softpc-process.exe
```

The selected executable was written at `2026-09-01 16:25`.

## Fixed-container observation

The one admitted non-debug, console-owning observation used the fixed stage
at `O:\ntvdm` and wrote
`O:\ntvdm\m0-t355-s30-composition-close.txt`:

```text
container=console-owning-nondebug
result=exited
exit=0x00000078
```

The observer also supplied `MVDM_SESSION_DISPOSE_REPORT_PATH`, but
`O:\ntvdm\m0-t355-s30-composition-close-dispose.txt` was absent. In
`app/entry.c`, a failed session disposal returns the app-owned `0x48` and
writes that report. Consequently the observed `0x78` is the original entry
result returning through a successful disposal, not the former live
`HeartBeatThread` binding failure.

## Result

The original heartbeat is no longer retained at the session-disposal boundary.
This closes only the direct-termination cleanup cohort. It does not claim
that the original entry result `0x78` is a successful guest bootstrap; that
separate original startup/guest execution result remains subject to the next
admitted owner package.
