# T221 S3 P53 — Package-session Direct/Readonly FCB host-read path

## Scope

The Direct and Readonly package-session legs now exercise the OpenNT-shaped
FCB local-file contract through normal ingress, registry, session dispatch and
bounded DTA transport:

1. `50:2D` (`SVC_DEMOPENFCB`) opens an existing host file in read mode;
2. `50:2F` (`SVC_DEMFCBIO`) reads one byte into the registered guest DTA; and
3. `50:2E` (`SVC_DEMCLOSEFCB`) releases the opaque FCB token.

The test derives an OEM pathname for the existing `C:` system
`kernel32.dll` using `GetSystemDirectoryW` and `WideCharToMultiByte(CP_OEMCP)`.
It is a host-integration witness for the configured C-drive projection, not a
synthetic boot-image fallback.

## Source and safety boundary

OpenNT `base/mvdm/dos/dem/demfcb.c` defines the original register contract:
`DS:SI` pathname and `AL` mode for open, `AX:BP` token plus `BX=1` for FCB
read, then `AX:SI` for close. The test follows those token locations after the
adapter's typed-token substitution.

The only host operation is `OPEN_EXISTING` plus one read. The fixture does not
create, write, rename, delete, alter attributes, or change the current
location of a host object. Thus Direct proves permitted normal Win32 host
integration while Readonly proves the same safe read path under its profile.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\152-direct-readonly-fcb-session `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` manifest records source-built link and run exit codes `0`.

## Remaining work

P53 does not prove Direct mutation, FCB wildcard or first/next search,
metadata/time/error/misc/GSET/DASD/device/state routes, nor complete the
29-identity DEM matrix.