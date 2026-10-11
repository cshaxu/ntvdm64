# M0 T443 S13 — NtAlertThread disposition

## Question

Does the project-owned `NtAlertThread` compatibility carrier need another
status predicate or a new fallback repair?

## Inputs

`src/ntvdm-exe/win32/nt_thread_alert_compat.c`, the original SoftPC consumer
`src/ntvdm-exe/softpc/mvdm_softpc_event_thread.c`, and the existing focused
fixture `tests/adapter-mvdm-host-out/nt_thread_alert_compat_fixture.c`.

## Observation

The carrier first resolves and calls the same-shaped `ntdll!NtAlertThread`.
On the supported host this export is present.  The original wait path already
distinguishes the APC wake status; no additional `STATUS_ALERTED` condition is
needed at the caller.  The `QueueUserAPC` branch is only an absent-export
fallback and is not a claim that APC queuing creates the durable NT alert
state.

## Disposition

No production change is admitted.  The normal runtime path retains the native
same-shaped call.  The absent-export fallback remains a documented limitation,
not a reason to add an extra caller predicate or a scheduler.  A future host
that lacks the export requires a separately admitted no-export equivalence
test before this fallback can be expanded or relied upon.

