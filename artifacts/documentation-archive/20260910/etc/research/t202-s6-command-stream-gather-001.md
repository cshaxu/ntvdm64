# T202 S6 COMMAND checked stream gather

## Question

Can the pending COMMAND child capability consume the original `54:08`
`STD_HANDLES` input shape without retaining a guest pointer or accepting a raw
host handle?

## Inputs and procedure

`cmd.h` defines `STD_HANDLES` as three packed 32-bit values ordered stderr,
stdout, stdin.  The original `cmdExec` obtains the record through `SS:BP`.
The COMMAND session now performs one checked 12-byte mechanical read at
`(SS << 4) + BP`, with a one-megabyte real-mode bound, and validates the
copied bytes against its own generation-bound tokens in that source order.

The regression first obtains all three tokens via `54:06`, writes only those
fixed-width values to guest RAM in source order, invokes `54:08`, and asserts
that the private session's validated-record count advances.  No guest address
is stored after the read.  A bad read or invalid token increments only the
private rejection accounting; neither form is a host handle.

The command was:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-stream-gather-r2 `
  -Fixture boot-namespace
```

The generated manifest records MSVC x64 `/MT`, `linkExitCode: 0`,
`runExitCode: 0`, and `passed: true`.

## Interpretation and follow-up

This is shared stream/child input substrate, not a `54:08` execution claim.
The provider continues to return its explicit `ERROR_NOT_SUPPORTED` lifecycle
result after validation.  It does not read a command tail or environment,
duplicate a stream handle, create a child, mutate process-global standard
handles, or use VDM re-entry.  The next required unit is the typed child
state machine that can own those completion semantics.
