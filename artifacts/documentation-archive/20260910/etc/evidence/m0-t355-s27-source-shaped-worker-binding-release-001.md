# M0 T355 S27 — source-shaped worker binding release

## Recovered exit contract

The existing session-aware thread facade is the smallest same-shaped boundary
between original cdecl worker entries and public Win32 `CreateThread`.
COMMAND `cmdCreateProcess` and pipe workers, plus Redirector DLC/named-pipe
workers, retain their original bodies and `CreateThread` calls through that
facade.

Several original bodies call `ExitThread(0)` instead of returning to their C
thunk—`cmdexec.c::cmdCreateProcess` and both `cmdredir.c` pipe workers are
explicit examples.  The adapter now exposes an identically shaped `ExitThread`
binding: it releases only the current thread's session binding, then invokes
the real public `ExitThread` with the unchanged source exit code.  Normal
returns retain the existing thunk release.  No MVDM mirror body or worker
business ordering changes.

## Formal link and one observation

The selected CPU40/x86 formal graph linked successfully in 12 incremental
actions.  Historical warnings remain visible and unmodified.

One unchanged `O:\\ntvdm` observation still exited `0x48` with:

```text
MVDM-SESSION-DISPOSE reason=binding-count code=2 total=1 entry=0 worker=1 unspecified=0
```

Therefore the reached worker did not take an `ExitThread` path during this
startup.  This does not invalidate the recovered source contract; it rejects
the false conclusion that every worker binding is an ExitThread leak.

## Source-owned successor

`vrnmpipe.c::VrpAsyncNmPipeThread` is an original long-lived loop which exits
only after `mvdm_redirector_async_worker_stop_requested()`.  Its facade binding
is intentionally live until that source-owned stop-and-join lifecycle runs.
The next owner cohort is Redirector asynchronous worker termination before app
session disposal.  It must recover the original stop/wait ordering; it must not
force-unbind a live worker or alter BOP/pipe semantics.
