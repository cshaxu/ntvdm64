# T198 S54: `50:0D` boot-drive composition audit

## Original ownership

OpenNT `dossvc.h` defines `SVC_DEMGETBOOTDRIVE` as `0Dh`; `demdisp.c`
dispatches it to `demGetBootDrive` in `demgset.c`.  That original routine
derives a drive from the NT boot-root/registry environment and falls back to
C: when that data is unavailable or unsuitable.

The non-invasive CLI must not import the registry relationship.

## Existing provider and current route

`src/bx-vdm/bx_ntvdm_dem_boot_drive_service.c` is the existing
source-derived provider.  It validates exactly real-mode `C4 C4 50 0D`,
preserves AH, returns `AL=3` (C:), and resumes at `fault_rip + 4`.

It is currently used by detached `adapter_runtime` and `cpu_result_bridge`
paths.  `boot_namespace_composition_v1` owns the active finite route and does
not compile/link or dispatch it.  The DEM plane correctly classifies 0Dh as
GSET; the existing finite gset plane currently consumes only 0Fh.  Therefore
the gap is a narrow composition omission, not a missing BOP contract or host
capability.

## Next admission

Add the existing provider to the explicit finite source manifest and place a
`50:0D` dispatch in the active composition after normal ingress/provider
selection.  The provider's exact selector/service/mode validation remains the
gate.  No registry, drive scan, device, or CLI option is admitted.
