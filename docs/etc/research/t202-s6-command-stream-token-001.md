# T202 S6 COMMAND stream token substrate

## Question

Can the COMMAND package replace the `54:06` controlled-stop placeholder with
one direct-profile, session-private stream-token boundary while leaving child
execution deferred until its complete lifecycle contract exists?

## Inputs

- `cmdredir.c`'s `cmdGetStdHandle` ABI and `cmd.h`'s stream record layouts;
- the admitted stream-and-child capability plan; and
- the existing bound COMMAND common-ingress fixture.

## Implementation

`bx_ntvdm_command_stream_child_v1` is initialized once with the COMMAND
session.  It captures the three normal process stream values only into
COMMAND-private storage and assigns a session generation.  In a direct profile
with the shared COMMAND `HOST_GLOBAL` grant, `54:06` accepts source selectors
0/1/2 and returns a generation-bound opaque 32-bit token in the historical
`BX:CX` result shape, with `AX` and `DX` zero and CF clear.

The token has a fixed `0xB...` namespace plus the private session generation
and slot.  It is not a `HANDLE`, pointer, PID or adapter bridge field.  The
private captured handle values never leave the COMMAND session.  Missing
grant, invalid selector, or unavailable private stream returns a checked
CF-set/zero-register failure; it never falls back to a raw handle or the old
controlled-stop placeholder.

`54:08/0A` remain in their one execution-lifecycle provider with the existing
`CF=0, AL=ERROR_NOT_SUPPORTED` source-shaped deferred result.  This change
does not launch a child, read the `STD_HANDLES` guest record, call
`SetStdHandle`, duplicate a handle, create a temporary pipe file, or invoke
VDM broker/VDD behavior.

## Procedure and observation

Ran:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-stream-token-r1 `
  -Fixture boot-namespace
```

The retained manifest reports MSVC x64 `/MT`, `linkExitCode: 0`,
`runExitCode: 0`, and `passed: true`.  Its all-17-service COMMAND common
ingress sweep now checks that `54:06` resumes at the copied BOP boundary,
clears CF, overwrites only the historical `AX/BX/CX/DX` return registers, and
returns a `0xB...` opaque value rather than the old terminal stop.

## Interpretation and follow-up

This proves only the first private stream boundary.  It is neither a stream
backend nor a child-process implementation; it does not establish that a
guest-supplied `STD_HANDLES` record is valid.  The next step remains the
checked 12-byte gather and token validation that all child paths must share.
