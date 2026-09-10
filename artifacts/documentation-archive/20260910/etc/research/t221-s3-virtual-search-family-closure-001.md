# T221 S3 P42: Virtual search family closure

## Question

Can all four stateful `demsrch.c` services search the explicit Virtual
namespace without consulting an admitted host directory or changing their
pathname/FCB continuation contracts?

## Inputs

- Original owner: `src/opennt/base/mvdm/dos/dem/demsrch.c`:
  `demFindFirst`, `demFindNext`, `demFindFirstFCB`, and `demFindNextFCB`.
- Existing checked gather/result transactions and search-session state in
  `src/bx-vdm`.
- Existing selector-blind Overlay and Virtual namespace views.

## Recovery decision

The historical translation unit is not independently composable because it
uses CCPU/SAS/host composition mechanisms.  The selected source-derived seam
retains its one stateful four-service owner package: first calls choose an
entry source once, while next calls consume the existing typed continuation.
No individual selector receives a profile-specific implementation.

## Implementation

`dem_search_partition` now has one private-entry helper.  Overlay selects the
existing merged host/COW view; Virtual selects the private-only view.  Both
then feed the existing pathname and FCB first-search transactions, including
their original separate DTA and SRCHBUF encoders.  `0B/0C` remain unchanged
continuation consumers.  Virtual has no host-namespace argument in its
enumeration call and therefore cannot fall through to Direct.

## Verification

Fresh source-built MSVC x64 `/MT` fixtures passed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemSearchPartitionProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\130

powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\131
```

The dedicated fixture deliberately leaves real host `ALPHA.TXT` and
`ZETA.TXT` present, then creates only private `GAMMA.TXT` and `OMEGA.TXT`.
Both pathname `09/0B` and FCB `0A/0C` return the private ordered pair.  This
is direct evidence that Virtual search does not enumerate the host directory.

## Disposition

This closes the Virtual `demsrch.c` subfamily only.  It does not close the
entire 29-service DEM provider, raw device/VDD services, process-resource
lifecycle, or native Bochs execution.
