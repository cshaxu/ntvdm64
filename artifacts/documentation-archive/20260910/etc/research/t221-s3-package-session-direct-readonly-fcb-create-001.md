# T221 S3 P62 — Direct/Readonly FCB create admission

## Question

Does the `demCreateFCB` (`50:2C`) lifecycle use the same profile-owned file
view as the rest of the Direct/Readonly DEM package, rather than a new
FCB-specific policy branch?

## Source and boundary

OpenNT `base/mvdm/dos/dem/demfcb.c::demCreateFCB` calls `demFCBCommon` with
`CREATE_ALWAYS`; the historical routine is not independently composable here
because it binds CCPU/SAS address conversion and historical host process
composition. The retained source-derived seam is the existing FCB handle
partition calling `dem_local_file_backend_v1_open_ex`, whose shared
`dem_file_view_v1_admit` selects the mutation profile before Win32 creation.

This preserves the original source family and fixed result ABI without moving
BOP semantics into Bochs or inventing a separate FCB policy.

## Procedure and observations

The Direct package session copies a path in its fixture-owned temporary C:
root through checked guest RAM, invokes `50:2C`, receives an opaque token,
closes it through `50:2E`, then deletes the same owned file through `50:05`.
Readonly sends the identical syntactically valid create request and receives
`CF=1, AX=5`. The denial occurs at the shared file-view/local-backend admission
point, before a host create call. Overlay and Virtual behavior is not invoked
or changed.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\163-direct-readonly-fcb-create `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` fixture compiled and the bounded executable returned exit
code `0`.

## Follow-up

This covers the FCB-create lifecycle and its Readonly admission only. The
remaining Direct/Readonly full-matrix negative legs and other package-family
closure evidence remain open.