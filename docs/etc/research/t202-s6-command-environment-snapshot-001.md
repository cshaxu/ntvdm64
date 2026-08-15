# T202 S6: COMMAND copied initial-environment snapshot

## Scope

This record covers the admitted COMMAND bootstrap/context slice
`54:01/02/04/05/0F`.  It advances its shared environment input and does not
claim complete COMMAND, console, streams, child lifecycle, or native guest
execution.

## Source contract

OpenNT `cmdenv.c:cmdGetInitEnvironment` builds the initial DOS environment
before the first `COMMAND.COM` request. It excludes `=`-prefixed drive state,
replaces `COMSPEC`, excludes `windir` for the DOS path, converts to OEM,
uppercases variable names, supplies `PROMPT=$P$G` when absent, and reports the
required buffer as paragraphs in `BX`. `cmdmisc.c:cmdGetNextCmd` then reads
the published environment size from `CMDINFO` and requests a retry when its
buffer is too small.

## Delivered boundary

The session-private `bx_ntvdm_command_host_context_v1` now contains a bounded
double-NUL OEM environment snapshot in addition to the copied current
directory. The snapshot accepts neither `=` drive records, `COMSPEC`, nor
`WINDIR`; these exclusions are structural validation, not a convention.

At ordinary CLI composition admission, `GetEnvironmentStringsW` is read once,
each selected entry is copied through OEM conversion, variable names are
uppercased, the two excluded names are removed, and `PROMPT=$P$G` is supplied
if necessary. The snapshot is capped by the existing 4096-byte checked
mechanical-write ABI after reserving the maximum COMSPEC line. Admission
fails rather than truncating a host environment.

`54:02` copies the guest-supplied 16-bit COMSPEC string, prepends its
`COMSPEC=` record, then appends only the copied snapshot. `54:0F` retains the
OpenNT-shaped paragraph retry and single successful publication. `54:01`
uses that same published byte count when validating `CMDINFO`; it never
queries the ambient process environment. No host environment pointer,
handle, callback, guest pointer, or Bochs type crosses the adapter ABI.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-environment-r4 `
  -Fixture boot-namespace
```

The resulting manifest records MSVC x64 `/MT`, `linkExitCode: 0`,
`runExitCode: 0`, and `passed: true`. The bound package fixture installs a
copied `PATH`/`PROMPT` snapshot, verifies its ordered `54:02 → 54:0F` output
and paragraph retry, and then verifies `54:01` accepts the same published
environment size through generic ingress and checked guest RAM.

`bx_ntvdm_composition_runtime_v1.c` also source-compiles under MSVC x64 `/MT`
with `/W4 /WX`; that check exercises the real `GetEnvironmentStringsW` capture
path, but does not run a native guest trace.

## Limits

The 4096-byte bound is a present mechanical-write ABI limit, not a claim that
all possible Windows environments fit the final product. An oversized entry
or snapshot is admitted as an explicit setup failure rather than silently
discarding data. Any larger-environment transport requires a separately
admitted checked bulk-write ABI. This record does not add per-BOP fallbacks.
