# T221 S3 P50 — Package-session private FCB I/O

## Scope

The existing Overlay/Virtual private FCB chain now continues through the
original `50:2F` route before `50:2E` closes the token:

```text
50:03 create -> 50:2D open -> 50:2F write -> 50:2F read -> 50:2E close
```

`50:2F` uses the DTA physical address established by `50:1B`. The write case
sets `BX=0` and copies `ab` from checked guest RAM into the private file; the
read case sets `BX=1` and writes those two bytes back through the normal
mechanical action. The opaque token remains in the original `AX:BP` register
pair throughout I/O.

## Isolation

Only Overlay and Virtual execute this chain. Their file is created by the
normal private namespace provider, so neither route performs host file I/O or
receives a test-injected backend token.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\148-private-fcb-io `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` manifest reports source-built compile/link/run exit code
`0`.

## Remaining work

This closes one private FCB I/O transport chain only. FCB wildcard mutation,
metadata, Direct/Readonly FCB fixtures, private search, and the full
29-identity matrix remain open.
