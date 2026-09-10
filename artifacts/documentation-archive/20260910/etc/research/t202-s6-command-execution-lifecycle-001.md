# T202 S6 COMMAND execution-lifecycle provider

## Question

Can the four connected COMMAND execution identities (`54:07/08/0A/0B`) have
one source-derived provider and explicit result ABI before the separate opaque
stream and typed-child backend exists?

## Source basis

`src/opennt/base/mvdm/dos/command/cmdexec.c` establishes one connected
historical lifecycle:

| Service | Original routine | Relevant result form |
| --- | --- | --- |
| `54:07` | `cmdCheckBinary` | `DontCheckDosBinaryType` clears CF for the contained DOS domain. |
| `54:08` | `cmdExec` | Synchronous launch failure clears CF and returns the Win32 error in AL. |
| `54:0A` | `cmdExecComspec32` | Has the same launch result contract. |
| `54:0B` | `cmdReturnExitCode` | Sets CF for an available re-entry command; otherwise clears CF and returns the terminal exit byte. |

The original execution body also depends on `GetVDMAddr`, raw standard
`HANDLE` values, `GetNextVDMCommand` re-entry counting, event threads and VDD
notification.  `CreateProcess` and `GetBinaryType` themselves are ordinary
modern Win32 APIs; those NT4 composition dependencies, not a missing API,
remain the blocker to executing a child safely.

## Implementation

`bx_ntvdm_command_execution_lifecycle_v1` is the sole COMMAND session record
for all four identities.  It owns no host process, handle, pointer or guest
address.

- `54:07` preserves the declared DOS-only profile's source branch: resume
  with CF clear.
- `54:08` and `54:0A` no longer issue a controlled machine stop.  Until an
  admitted opaque stream plus typed-child lifecycle exists, they resume with
  CF clear and copied `AL=50` (`ERROR_NOT_SUPPORTED`), preserving AH.  This
  follows the source's launch-failure register shape without claiming a host
  process was started.
- `54:0B` delegates an active declared-DOS return to the existing typed
  `get-next` state.  Before any declared slot exists, it retains the previous
  source-derived terminal result in the same lifecycle owner.

## Verification

Ran:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-execution-lifecycle-r1 `
  -Fixture boot-namespace
```

The retained manifest reports MSVC x64 `/MT`, `linkExitCode: 0`,
`runExitCode: 0`, and `passed: true`.  Its common-ingress COMMAND family sweep
checks both deferred execution identities for resume, CF clear and preserved
AH with `AL=50`; it also checks the DOS-domain binary result and the
pre-delivery terminal return form.

## Interpretation and limit

This is routing and source-shaped failure closure for the complete execution
lifecycle family, not child-process recovery.  No host process is created,
no raw handle is transported, and no VDD or legacy VDM re-entry mechanism is
introduced.  The next implementation must supply one package-owned opaque
stream table and typed child-result contract before replacing the two deferred
execution results with `CreateProcess` behavior.
