# T221 S3 P57 — Direct package-session handle and namespace chain

## Scope

The Direct profile now runs one source-shaped, normal package-session chain in
a test-owned temporary directory on host `C:`:

`50:03 → 50:1E → 50:00 → 50:16 → 50:08 → 50:27 → 50:02 → 50:05`

That is create, write, seek, read, file-time query, commit, close and delete.
All paths and bytes enter through checked guest RAM; all file identities remain
opaque `AX:BP` tokens. The test proves the write/read payload `xy`, seek
position zero, timestamp result slots, token lifecycle and final delete.

## Source and host boundary

`demfile.c` defines `demCreate`/`demDelete`; the retained handle partition
implements `demChgFilePtr`, `demRead`, `demWrite`, `demFileTimes` and
`demCommit` using the existing typed Direct file session. The test container is
created with `GetTempFileNameW` followed by `CreateDirectoryW`; an `atexit`
cleanup enumerates only that generated directory before removing it. No ambient
host file is opened, changed or deleted.

This is normal non-invasive Win32 host integration: it changes a deliberately
owned temporary test object, not the host system configuration or files outside
that object.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\156-direct-handle-namespace-chain-witness `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` manifest reports source-built link exit `0` and run exit
`0`.

## Remaining work

P57 is a Direct owner-family chain only. Readonly mutation refusal, remaining
namespace identities, FCB wildcard/search, full two-profile negative matrix and
all other DEM owner families remain open. Frozen Overlay/Virtual interfaces
receive no new behavior.