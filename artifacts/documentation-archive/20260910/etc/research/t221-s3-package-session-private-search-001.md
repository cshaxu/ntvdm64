# T221 S3 P51 — Package-session private pathname search

## Scope

The Overlay and Virtual legs now exercise their own namespace views through
the normal pathname-search owner route. The fixture creates two private files
with `50:03`:

- `C:\\FCBTEST.COM`
- `C:\\FCBTWO.COM`

It then supplies `C:\\FCB*.COM` through checked guest RAM and runs
`50:09` (PATHFIRST) followed by `50:0B` (PATHNEXT). The two entries provide a
real private continuation; the same DTA far output pointer and copied
CurrentPDB ABI used by the declared-search leg are carried by the session.

## Boundary

This is deliberately separate from Direct/Readonly declared-snapshot search.
Overlay and Virtual enumerate their private views only; no host directory and
no declared profile snapshot is substituted into their result set.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\149-private-search `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` manifest reports source-built compile/link/run exit code
`0`.

## Remaining work

P51 covers only private pathname first/next. FCB first/next, wildcard and
metadata routes, Direct/Readonly FCB fixtures, namespace mutation, and the
complete 29-identity matrix remain open.
