# M0 T318 S2 P20 — Non-debug Console Observation Attribution

## Question

Does the P19 windowed registration repair have valid ordinary-process runtime
evidence when the observer does not attach a debugger?

## Observation

The existing disposable launcher was invoked in its `plain` mode.  This mode
starts the selected x86 product without `DEBUG_ONLY_THIS_PROCESS`, but it does
not allocate and explicitly pass `CONIN$`/`CONOUT$` handles as the
console-owning mode does.

The child exited before CPU execution with `0xC0000005` and zero accumulated
CPU time:

```text
25640 exited 3221225477 cpu-ms 0
ntio-prefix absent
ntdos-anchor absent
demLoadDos not-installed
display-error not-installed
```

## Interpretation

This result cannot validate or invalidate P19.  It changes two historical
variables at once—the debugger attachment and the original process's console
handle ownership—so it cannot be attributed to the registration adapter.
In particular, it does not establish an NTIO, NTDOS, `EXEC`, or parent-return
failure.

## Required Next Evidence

The next observation must be non-debug **and** console-owning: it must pass
real `CONIN$`/`CONOUT$` handles exactly as the original host startup expects,
then use a bounded ordinary process run.  No additional product instrumentation
or BOP route is admitted by this evidence correction.
