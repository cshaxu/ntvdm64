# T229 S2 P21 — DEM DASD/IOCTL Owner Group

## Question

Does the complete DEM DASD/IOCTL group preserve OpenNT metadata and no-device
failure semantics under Direct and Readonly without treating DEM as a raw
machine-device implementation?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demioctl.c`.
- `src/opennt/base/mvdm/dos/dem/demdasd.c`.
- `src/bx-vdm/bx_ntvdm_dem_ioctl_metadata_provider_v1.c` and
  `src/bx-vdm/bx_ntvdm_dem_raw_media_provider_v1.c`.
- `tests/bx-vdm/t226_s2_dem_dasd_ioctl_conformance_fixture.c`.

## Procedure

1. Verify `50:21` IOCTL metadata subfunctions `AL=08/09`, their invalid-drive
   branch, and the original invalid-function terminal for generic/block IOCTL.
2. Verify `50:29` absolute read and `50:2A` absolute write with no admitted
   BDS, BIOS disk or raw-device owner.
3. Build and run the dedicated formal fixture, then the package fixture in
   Direct and Readonly modes.

## Observations

- `AL=08/09` use the admitted host drive snapshot and retain OpenNT's AX/DX
  layouts, including `ERROR_INVALID_DRIVE`.
- Other IOCTL forms return `ERROR_INVALID_FUNCTION (1)` with CF set.
- Absolute read/write return `DOS_DRIVE_NOT_READY (21)` with CF set: this is
  the exact `demGetBDS(...) == NULL` path in `demAbsReadWrite`.
- Formal target `t226-s2-dem-dasd-ioctl-conformance-fixture.exe` and the
  Direct/Readonly `t225-s19-dem-package-lifecycle-fixture.exe` runs returned
  zero.

## Interpretation

The DASD/IOCTL owner group is code-complete for the selected profiles.  It
neither enables a raw host device nor weakens Direct/Readonly policy.  A future
raw-media feature remains a separate bx-mantle machine-owner admission, with
its own physical-device safety contract; DEM must consume only its typed result.

## Follow-up

Continue T229/S2 with the error-lock and fastread owner groups; do not infer
raw-device availability from the completed metadata/error contracts.