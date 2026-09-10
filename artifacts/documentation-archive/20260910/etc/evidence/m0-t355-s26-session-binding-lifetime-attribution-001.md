# M0 T355 S26 — session binding-lifetime attribution

## Source boundary

The existing binding count can be incremented at only two production
entrances:

1. `mvdm_softpc_execution_run_original_entry` / `run_until_return`, the outer
   SoftPC entry bridge;
2. `thread_start_compat.c` thunks, which carry an original COMMAND or
   Redirector worker into a session-bound Win32 thread.

Original `nt_event.c` and `nt_timer.c` use direct `CreateThread` and do not
bind a session.  The BaseVDM local record registers a hook executed after a
binding; it does not increment `binding_count`.

S26 adds source-category counters to the existing project-owned session bind
and unbind operations.  The outer bridge uses `SOFTPC_ENTRY`; every existing
thread facade thunk uses `ORIGINAL_WORKER`.  The counters change only with the
same existing total binding operation and do not alter execution or teardown.

## Formal build

The selected CPU40/x86 command was:

```text
build\M0-T355\S20\formal-x86\run-ninja-parallel.cmd original-softpc-process.exe
```

It completed 37 incremental actions and linked the selected product.  Existing
historical warnings remained visible and were not changed by S26.

## One fixed-container observation

The one admitted `O:\\ntvdm` observation retained the same stage, timeout,
media and launcher.  It exited `0x00000048` and wrote:

```text
MVDM-SESSION-DISPOSE reason=binding-count code=2 total=1 entry=0 worker=1 unspecified=0
```

The outer SoftPC entry binding is balanced.  The sole outstanding count belongs
to the original-worker facade.  This is attribution evidence, not a repair.

## Selected successor

The next complete owner cohort is the session-aware historical worker facade:
`adapter-mvdm-host-out/win32/thread_start_compat` together with all reached
original COMMAND/Redirector worker termination forms.  In particular,
source `ExitThread` can bypass a thunk's ordinary return-path unbind.  A
successor must audit every wrapped start routine and preserve original worker
termination behavior while ensuring the session binding is released.  It must
not modify the outer SoftPC execution bridge or treat this as a BOP repair.
