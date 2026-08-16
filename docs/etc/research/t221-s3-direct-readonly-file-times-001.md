# T221 S3 P65 — Direct/Readonly file-time mutation profile

## Scope

The whole-provider session fixture now exercises both outcomes of
`50:08` (`demFileTimes`):

- Direct gets existing DOS time/date from a token, then sets a valid DOS date
  and time on its fixture-owned temporary file.
- Readonly opens the declared `COMMAND.COM` image and submits the same
  set-time option; it receives `CF=1, AX=5`.

The Direct object is deleted before fixture exit. The Readonly image is
provider-owned, immutable state; no host metadata call is made for its set
request.

## Source and boundary

OpenNT `demhndl.c::demFileTimes` distinguishes get and set by the option
register. Its full historical translation unit depends on CCPU/SAS and old
host composition. The existing `dem_handle_route_partition_v1` retains that
option/result contract through typed tokens and checked transactions; its
Readonly source-derived service explicitly permits get and rejects set.

No BOP-local profile rule, Bochs change, or Overlay/Virtual behavior change is
introduced.

## Reproduction

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\168-direct-readonly-file-times `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` fixture compiled and returned exit `0`.

## Follow-up

This is one two-profile metadata matrix leg. The T221 29-identity closure audit
continues; no package closure is claimed.