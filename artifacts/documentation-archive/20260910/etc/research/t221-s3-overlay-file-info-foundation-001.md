# T221 S3 Overlay file-info foundation

## Question

Can an Overlay file token provide the size, attributes, and DOS time/date
facts required by the FCB owner without a Win32 `HANDLE`?

## Implementation

The private Overlay store now timestamps COW file/directory materialization
and mutation. `bx_ntvdm_dem_overlay_file_v1_info` returns attributes, byte
count and the stored DOS time/date after validating the private overlay token.
It exports neither a host handle nor a guest pointer.

## Verification

Focused MSVC x64 `/MT` fixture passed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T221S3DemOverlayFileProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\064
```

It proves COW data isolation plus nonzero DOS time/date, attributes and size
through an opaque Overlay token.

## Limit

This is only the FCB information prerequisite. No FCB BOP route changed.
