# T202 S6 bounded native COMMAND integration observation

## Question

Does one current-source native CPU5 run reach the recovered COMMAND provider
after the S6 package regression passes?

## Procedure

Ran the current-manifest source-built native fixture once:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-native-integration-r1
```

## Observation

The probe rebuilt current adapter/CLI objects under MSVC x64 `/MT`, linked a
CPU5/Pentium-MMX fixture, and executed exactly once.  Its retained manifest
reports `runExitCode: 2`.  The run log reports valid startup/CMDINFO evidence
and the first canonical BOP `50:11`, followed by DEM lifecycle observations.
It contains no observed `54:xx` invocation.

## Interpretation and follow-up

This is a bounded integration observation, not COMMAND native closure.  It
proves the current S6 sources compile/link into the native fixture and does
not introduce a new early failure before the pre-existing DEM frontier.  It
does not prove that the new stream or typed-child path executes natively.
Per the owner-package rule, the missing reach is recorded only as integration
ordering information; it does not create a `54:xx` patch task.  COMMAND
re-entry/console capability disposition remains package work, while the next
native trace is deferred until an already-admitted package changes that
frontier.
