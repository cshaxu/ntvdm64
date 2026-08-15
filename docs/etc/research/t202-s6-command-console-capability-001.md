# T202 S6: COMMAND console capability

This record covers the one COMMAND console/keyboard/start-state slice:
`54:09`, `54:0E`, and `54:10`.

OpenNT maps them to `cmdInitConsole`, `cmdGetKbdLayout`, and
`cmdGetStartInfo`. The historic first function enables VDD output/event-thread
state; the keyboard function's no-install branch calls it then returns
`DX=0`; start-info returns only whether `DosSessionId` represents a new
console. The CLI composition does not admit VDD, KEYB/KB16, GUI, a host
handle, or ambient console inspection.

`bx_ntvdm_command_console_capability_v1` is one session-owned fixed-width
record. Its default copied policy is an existing-console session (`AL=0`).
`54:09` records local initialization and resumes; `54:0E` records the same
no-install transition and returns `DX=0`; `54:10` returns the policy value in
`AL`, preserving `AH`. No service reads or writes guest scratch buffers,
opens the registry, calls a VDD, or crosses a host resource through the ABI.

The bound package regression was run with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-console-r1 `
  -Fixture boot-namespace
```

Its manifest records MSVC x64 `/MT`, `linkExitCode: 0`, `runExitCode: 0`, and
`passed: true`. This is package routing evidence only. It does not claim an
interactive console, keyboard installation, standard streams, child process
lifecycle, or native guest closure.
