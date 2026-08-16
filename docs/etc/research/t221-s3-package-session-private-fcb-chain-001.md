# T221 S3 P49 — Package-session private namespace to FCB chain

## Scope

The Overlay and Virtual matrix legs now establish their FCB file fixture
through the actual BOP owner sequence, not by writing a provider-private
backend token in the test:

```text
50:03 private create  ->  50:2D FCB open  ->  50:2E FCB close
```

The pathname `C:\\FCBTEST.COM` is supplied only through checked guest RAM.
The normal session performs the existing DTA/CurrentPDB owner read for the
namespace create, then the FCB path gather and typed opaque-token lifecycle.

## Isolation

This runs only for Overlay and Virtual. Their create and FCB file objects are
private provider state; the fixture does not create, modify, or delete a host
file. Direct and Readonly retain their distinct host/readonly dispositions and
are not silently substituted with this private fixture.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\147-private-fcb-chain `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` manifest reports source-built compile/link/run exit code
`0`.

## Remaining work

P49 closes only private fixture creation and FCB open/close transport. FCB
read/write, wildcard mutation, Direct/Readonly file fixture behavior, private
search, and the complete 29-identity matrix remain open.
