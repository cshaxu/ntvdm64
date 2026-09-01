# M0 T355 S25 — fixed-container diagnostic environment forwarding

## Narrow observer change

`ObserveSoftpcStartup.mjs` now accepts exactly one optional child-environment
form:

```text
--child-environment MVDM_SESSION_DISPOSE_REPORT_PATH=<absolute-path>
```

The observer rejects every other option and every other environment variable.
It forwards only that named value to the launched product and records the name
(not the value) in its durable JSON.  It does not change product arguments,
stage, media, current directory, console ownership, timeout, or inherited
environment otherwise.

## One fixed-container observation

The single admitted observation used the existing formal CPU40/x86 product,
unchanged `O:\\ntvdm` media stage, and the allowlisted report path.  Its durable
result was:

```text
container=console-owning-nondebug
result=exited
exit=0x00000048
timeout-ms=8000
fixed-system-root=O:\\ntvdm\\mvdm
```

The child report was written and contains:

```text
MVDM-SESSION-DISPOSE reason=binding-count code=2
```

The observer JSON records `childEnvironment` as the sole allowed name.  The
staged product SHA-256 is
`8569580d94a5ec3388976bf39f743beb34350c3cabcbba95dc2b5316d6e0dbd3`.

## Source-owned successor

This establishes that the failure is neither an invalid-session predicate nor
an armed termination escape.  `session_thread_bind` has only two production
owners: the outer SoftPC execution bridge and the session-aware thread-start
adapter.  The latter is used only by COMMAND/Redirector source translation
units; the observed BOP sequence contains neither `54:08/54:0A` EXEC nor a
`57:xx` Redirector request.  Original `nt_event.c` and `nt_timer.c` create
direct Win32 workers and never bind a session; BaseVDM merely registers a hook
which runs *after* a binding and does not increment `binding_count`.

The next bounded cohort is therefore a project-owned binding-lifetime ledger:
record bind/unbind ownership at the existing two binding entrances, reconcile
the successful outer execution return, and repair only the proven
source-shaped owner.  This S makes no lifecycle repair claim.
