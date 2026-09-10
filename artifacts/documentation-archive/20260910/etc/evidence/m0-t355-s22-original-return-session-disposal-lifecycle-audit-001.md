# M0 T355 S22 — original return and session-disposal lifecycle audit

## Original and current paths

The selected original top-level path is:

```text
ntvdm.c::main
  -> TimerInit
  -> host_main
     -> host_start_cpu
     -> host_applClose
        -> nt_remove_event_thread
        -> TerminateHeartBeat
```

The current application enters that unchanged sequence through
`mvdm_softpc_execution_run_original_entry`.  On an ordinary C return it first
records the mechanical return, disarms the session termination escape, ends the
guest-memory scope, and unbinds its calling thread.  Thus the bridge itself
cannot leave its own binding or termination escape armed before `app/entry.c`
calls `session_dispose`.

`session_dispose` has exactly three rejection predicates: invalid session,
nonzero `binding_count`, or an armed termination escape.  The observed S21
path reached the app's cleanup return, so its own normal bridge had already
disarmed and unbound.  Exit code 72 alone cannot distinguish the remaining
predicates.

## Selected cohort

`nt_start_event_thread` creates the original `ConsoleEventThread` directly
through the public original `CreateThread` declaration.  It does **not**
include or route through `thread_start_compat.h`; the event worker therefore
does not increment the current session's `binding_count`.  The original
heartbeat worker is also created directly.  Their shutdown/handle lifetime
remains relevant to process hygiene, but cannot establish the observed
`session_dispose` predicate.

The original `host_applClose` calls `nt_remove_event_thread`, but its latter
routine only calls `NtAlertThread`: it does not wait for the event worker.
The original `ConsoleEventThread` itself closes the sole waitable handle as it
returns.  This is valid for historical NTVDM because the dedicated process is
then terminated.  It is not evidence of a session-binding defect in the
present app.

The only currently selected source locations that can increment
`binding_count` are the adapter's execution bridge and translation units that
explicitly include `thread_start_compat.h` (COMMAND child/pipe workers and
selected Redirector workers).  The reached trace contains no proof that such a
worker was created or remained live.  The bridge's ordinary-return path
already disarms, ends the memory scope and unbinds.

## Recovery disposition

No lifecycle repair is selected by this audit.  The required successor is a
default-off, session-owned predicate observation that reports which of
`session_valid`, `binding_count`, or `termination_armed` rejects disposal,
without changing original host or guest behavior.  Only after that observation
may a source-owned cleanup cohort be admitted.
