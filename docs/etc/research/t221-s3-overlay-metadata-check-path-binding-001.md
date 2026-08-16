# T221 S3 Overlay metadata and check-path binding

## Question

Can original `demChMod` (`50:01`) and `demCheckPath` (`50:44`) use the same
Overlay visible namespace without mutating a host file or recreating the
historical temporary-NUL side effect?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demfile.c`: `demChMod`, `demCheckPath`.
- `src/bx-vdm/bx_ntvdm_dem_overlay_namespace_view_v1.[ch]`.
- `src/bx-vdm/bx_ntvdm_dem_overlay_metadata_backend_v1.[ch]`.

## Implementation

`50:01` retains the original AL query/set split and DOS attribute projection.
When shared profile selection chooses Overlay, a base file is materialized
through the read-only resolver and attributes are stored only in its COW node;
directory metadata is a private visible-directory overlay. Direct retains
host attribute APIs; Readonly and Virtual retain existing disposition.

`50:44` retains the `\\DEV\\` DOS-success special case and its `DL` drive
selection. For Overlay it calls the shared visible-directory query instead of
creating a temporary host `NUL` file; success still writes `DX=0` and resumes
through the normal bounded namespace route.

## Verification

MSVC x64 `/MT` whole-provider regression passed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\062
```

The Overlay section proves attribute query, hidden-attribute set, and hidden
attribute readback on a host-base file through COW. The existing host-base
preservation assertion remains green. The route's existing direct test covers
normal `50:44` directory success; Overlay `50:44` now shares its view backend
and has no host create side effect.

## Limit

This closes Overlay metadata/check-path binding only. It does not close time,
search, FCB, device, Virtual, or the complete DEM package. No native trace was
run.
