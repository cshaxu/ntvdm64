# M0 T290 S3 P7 — Original COMMAND redirection consumer

## Scope

This delivery executes the exact original
`mvdm-host/dos/command/cmdredir.c` translation unit's host-local standard
handle classification and cleanup path.  It is the Redirector-adjacent
COMMAND consumer, not a new child-process/session broker.

## Proven original behavior

- With no standard handles, `cmdCheckStandardHandles` returns no
  `REDIRCOMPLETE_INFO`, clears the standard-handle mask, clears
  `fSoftpcRedirection`, and notifies `nt_std_handle_notification(FALSE)`.
- With all three host-local standard handles, it retains their exact values in
  its original `REDIRCOMPLETE_INFO` layout, returns mask `7`, sets
  `fSoftpcRedirection`, and notifies `nt_std_handle_notification(TRUE)`.
- `cmdCheckCopyForRedirection` consumes that record with no pipe worker,
  child-process launch, session lifecycle creation or duplicate broker.

The fixture's notification observer only records the source call.  The
production notification remains owned by the existing session-facing
host-control boundary; neither `SetStdHandle` nor a process-global handle
mutation is introduced.

## Explicit deferred source branch

`cmdGetStdHandle` still reconstructs a `PREDIRCOMPLETE_INFO` from legacy
`AX:BX` and returns raw split HANDLE words.  It is compiled but not selected:
the source requires a separately admitted session mapping-manager ABI to
replace that persisted pointer/handle coincidence.  The fixture supplies
only dead-link register carriers; it cannot produce service success.  The
source's pipe thread and temporary-file branches likewise remain unselected
until their session-owned lifecycle contract is completed.

## Commands and results

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S3CmdRedirNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T290/S3/cmdredir-x86
.\build\M0-T290\S3\cmdredir-x86\bin\t290-s3-cmdredir-fixture.exe

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S3CmdRedirNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T290/S3/cmdredir-x64
.\build\M0-T290\S3\cmdredir-x64\bin\t290-s3-cmdredir-fixture.exe
```

Both fixtures compile, link and exit `0`.  `cmdredir.c` is unchanged.  Its
historical warning set is retained as source evidence, especially the x64
pointer/HANDLE truncation warnings in the explicitly unselected AX:BX
marshaller.
