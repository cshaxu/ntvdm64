# T221 S3 P59 — Direct package-session namespace-positive chain

## Scope

The Direct profile now runs the rest of its normal namespace-positive chain in
a self-owned temporary directory on host `C:`:

`50:22 → 50:02 → 50:17 → 50:01 → 50:04 → 50:44 → 50:06 → 50:05`

The fixture creates and closes a new file, renames it, queries its attributes,
creates a subdirectory, checks that C: path, removes the subdirectory, then
deletes the renamed file. It uses checked guest RAM for every pathname and the
existing opaque `AX:BP` token only where a file handle is required.

## Source and host boundary

OpenNT `base/mvdm/dos/dem/demfile.c` is the source reference for
`demCreateNew`, `demRename`, and `demChMod`; `demdir.c` supplies the directory
create/remove behavior, and `demCheckPath` supplies the path-query contract.
The test follows their register shapes: `DS:SI` for create-new/check-path,
`DS:DX` for pathname operations, `ES:DI` for the rename destination, and
`DL=3` for host C: in the path check.

The fixture creates its root via `GetTempFileNameW` followed by
`CreateDirectoryW`; an `atexit` cleanup enumerates and removes only objects in
that generated root. This is Direct profile host integration against owned
temporary test objects, not access to ambient files or host configuration.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\158-direct-namespace-positive `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` fixture compiled and the bounded executable returned exit
code `0`.

## Remaining work

This is a Direct namespace-positive leg only. FCB wildcard/search identities,
the complete Direct/Readonly owner-family matrix, and all remaining DEM
families are still open. Overlay and Virtual implementations and their typed
file-view/profile seams are retained unchanged and receive no new behavior.