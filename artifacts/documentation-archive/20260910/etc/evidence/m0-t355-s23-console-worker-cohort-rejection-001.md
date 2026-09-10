# M0 T355 S23 — console-worker cohort rejection

## Objective and result

S23 tested the S22 hypothesis that the original console-event worker retained
a session binding until after app disposal.  The hypothesis is false.

`softpc.new/host/src/nt_event.c::nt_start_event_thread` and
`softpc.new/host/src/nt_timer.c::host_timer_init` use the original public
`CreateThread` declaration directly.  Neither translation unit includes
`adapter-mvdm-host-out/win32/include/thread_start_compat.h`; neither worker
calls `session_thread_bind`; neither can change `session.binding_count`.

The temporary wait/handle-ownership experiment was formally linked and one
unchanged fixed-container observation still ended at `0x48`
(`APP_STARTUP_DISPOSE_FAILURE`).  It therefore did not establish either a
cause or a valid recovery.  The experiment is removed: `nt_event.c` is again
the original mirror form and no `MVDM-HOST-DIV-190` is retained.

## Current bounded fact

The original startup now reaches many DEM and COMMAND BOPs after the selected
original XMS lifecycle, and no longer reports the former direct-RAM XMS fault.
It later returns to app cleanup, where `session_dispose` rejects one of only
three predicates:

1. invalid session state;
2. nonzero `binding_count`;
3. armed termination escape.

Exit status `72` cannot attribute which predicate is true.  COMMAND and
Redirector translation units using `thread_start_compat.h` remain possible
binding owners, but no current observation proves their creation or lifetime.

## Disposition

No original host behavior is changed.  A successor may add a default-off,
session-owned disposal-predicate observer with a fixed result format; it must
not alter BOP results, guest state, worker scheduling, media or the fixed
launch container.  That observer is a diagnostic admission, not a lifecycle
repair.
