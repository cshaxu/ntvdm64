# T221 S3 P55 — Package-session Direct/Readonly FCB file information

## Scope

Direct and Readonly now send `50:31` (`SVC_DEMGETFILEINFO`) through normal
BOP ingress, registry and the installed package session. The checked `DS:SI`
OEM pathname names the same existing host `C:` system `kernel32.dll` used by
P53.

The test proves the original source-shaped result layout is returned:
`AX` attributes, `CX` time, `DX` date, and `BX:DI` size. It validates the
corresponding five GPR16 write bits without exposing a host handle to guest
state.

## Source and safety boundary

OpenNT `base/mvdm/dos/dem/demfcb.c::demGetFileInfo` documents the `DS:SI`
input and `AX/CX/DX/BX:DI` output ABI. The current route retains that contract
through a typed adapter result.

Like P53, this is `OPEN_EXISTING`/read/query-only host integration. The
fixture performs no host creation, write, rename, delete, attribute update or
current-directory mutation.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\153-direct-readonly-fcb-info-session `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` manifest records source-built link and run exit codes `0`.

## Remaining work

The active three-profile matrix still requires the remaining FCB wildcard and
search owner-family integration plus the other DEM subfamilies. Virtual is
frozen under P54; no new Virtual behavior is implied by this result.