# T221 S3 P41: Virtual FCB family closure

## Question

Can the complete `demfcb.c` owner family operate against the explicit Virtual
profile without a host namespace query, host handle, or Direct fallback?

## Inputs

- Original owner: `src/opennt/base/mvdm/dos/dem/demfcb.c`:
  `demDeleteFCB`, `demRenameFCB`, `demCreateFCB`, `demOpenFCB`,
  `demCloseFCB`, `demFCBIO`, `demDate16`, and `demGetFileInfo`.
- Existing fixed-width FCB BOP/register contract in
  `src/bx-vdm/bx_ntvdm_dem_fcb_{handle,wildcard,io_route,path_route}_partition_v1.c`.
- Existing private Virtual namespace/file token and mutation contracts.

## Recovery decision

`demfcb.c` cannot be linked directly: its CCPU/SAS and historical host
composition dependencies do not compose in the x64 CLI process.  This change
therefore remains a smallest source-derived adapter seam.  It retains the
original owner grouping, open/create/close/token layout, FCB I/O result
registers, wildcard filtering and per-match mutation ordering.  It introduces
no Bochs change and no new guest ABI.

## Implementation

- `50:2C/2D/2E/2F/31` select the existing private `VIRTUAL_FILE` object where
  the Virtual view has been selected; `30` remains the shared `demDate16`
  local-time layout.
- The FCB-info helper is deliberately private-file mechanical code and now
  accepts both `OVERLAY_FILE` and `VIRTUAL_FILE` token kinds.  It does not
  perform a host attribute query for a Virtual token.
- `50:07/20` enumerate and mutate the no-host-I/O Virtual view through its
  existing private delete/rename backend.  Direct and Overlay retain their
  distinct routes; Readonly retains access-denied disposition.
- The controlled `50:2F` write direction continues to use the existing
  checked DTA gather action before it mutates the private file object.

## Verification

Fresh build root: `build/M0-T221-S3/129`.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\129
```

The MSVC x64 `/MT` whole-provider fixture passed.  Its Virtual cases prove:

- an absent host-backed wildcard reports `FILE_NOT_FOUND` rather than seeing
  an admitted host file;
- `2D`, checked `2F` gather/complete, `2E`, `31`, and `2C` use a private
  token/file object;
- `20` rename and `07` delete affect only private Virtual names; and
- private files are removed before the private directory rmdir succeeds.

## Disposition

This closes the Virtual FCB owner subfamily only.  It does not close Virtual
search (`50:09..0C`), the full 29-identity DEM provider package, native
execution, or the separate PDB/JFT/SFT lifecycle boundary.
