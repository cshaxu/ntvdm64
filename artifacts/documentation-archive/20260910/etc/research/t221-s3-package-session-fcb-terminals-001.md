# T221 S3 P48 — Package-session FCB terminals and deterministic search

## FCB scope

The normal package-session matrix now sends two source-owned, safe FCB direct
route terminals in every Direct, Readonly, Overlay, and Virtual profile:

- `50:30` returns DOS date/time register results; and
- `50:2E` accepts the original zero-token close no-op.

Neither case opens a host file or invents an FCB namespace mapping. The
read/write FCB families still require a profile-owned file fixture and remain
separate work.

## Search correction

The declared search chain now uses `C:\\*.COM`, which matches the two declared
immutable COM images. The test proves the continuation by successfully sending
`50:0B`; it no longer treats individual bytes of its cryptographically random
continuation token as a semantic assertion. This removes test nondeterminism
without weakening the actual first/next transport check.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\146-package-session-fcb-terminals-fixed `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` manifest reports source-built compile/link/run exit code
`0`.

## Remaining work

P48 is not FCB package closure. The `07/20/2C/2D/2F/31` wildcard, open,
I/O, metadata and lifecycle families still need their source-shaped,
profile-owned matrix fixtures; private Overlay/Virtual search still remains
separate from declared-snapshot search.
