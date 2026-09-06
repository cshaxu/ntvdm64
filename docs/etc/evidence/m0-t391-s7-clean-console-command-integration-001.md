# M0 T391 S7 — clean Console nested COMMAND integration

## Purpose

The ordinary Codex command runner exposes a pseudoconsole and pipe standard
handles.  It is not a substitute for the released product's real Console
contract.  The older startup observer is also unsuitable as the sole result:
before it starts the product it injects diagnostic `MVDM_*_REPORT_PATH`
variables for exception, BOP, BaseVDM, DEM and SAS observation.

`tests/observation/clean_console_command_integration.c` is the bounded release
integration fixture for this particular distinction.  It opens the currently
attached `CONIN$` first (a pseudoconsole has no window but may still be
attached), allocates a Console only if that fails, and explicitly supplies
`CONIN$`/`CONOUT$` as the child standard handles.  It removes only the known
diagnostic `MVDM_*` variables around `CreateProcess`; it adds none.

## Published x86 result

Product: `O:\\ntvdm64\\ntvdm32.exe`

SHA-256: `85a667dcd161329b897ba2580a46c8254591bd50ceb6e063bc8ba93e04761f51`

The formal x86 fixture was built with
`tools/build/Generate-CleanConsoleCommandIntegrationNinja.mjs` and returned:

```text
container=clean-console-no-mvdm-diagnostics
case=command.com /c ver
result=0
case=command.com /c command.com /c ver
result=0
```

For each row the fixture clears the Console, starts the product in
`O:\\ntvdm64`, waits up to 30 seconds, requires process exit `0`, then reads
the Console screen buffer and requires the literal `MS-DOS Version 5.00.500`.
Therefore both the single and two-level child app-entry paths reached the DOS
`COMMAND.COM` `VER` implementation; this is not merely a successful host
`cmd.exe` handoff.

## Disposition

This supersedes the earlier agent-runner failure as product evidence.  That
failure was caused by the test container's pipe handles (and, in the observer
case, its diagnostic environment), not reproduced by this clean Console
fixture nor by the owner's ordinary `cmd.exe` result.
