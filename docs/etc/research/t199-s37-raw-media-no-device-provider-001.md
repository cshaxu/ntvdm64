# T199 S37 Raw-Media No-Device Provider 001

## Scope

This record closes the no-device terminal disposition for the raw-media
subgroup `50:21`, `50:29`, and `50:2A`. It does not add a disk image, host
drive passthrough, BIOS INT13 route, or a Bochs device to bx-vdm.

## Original source contracts

- `src/opennt/base/mvdm/dos/dem/demioctl.c`: `demIoctlInvalid` sets
  `ERROR_INVALID_FUNCTION` (AX=1) and CF. The existing metadata provider
  preserves the separately implemented 08/09 branch first.
- `src/opennt/base/mvdm/dos/dem/demdasd.c`: `demAbsReadWrite` detects a
  missing BDS and sets `DOS_DRIVE_NOT_READY` (AX=21) with CF for both absolute
  read and write.

## Composition

`bx_ntvdm_dem_raw_media_provider_v1` is selector-blind within Bochs and is
called only after the existing immutable-drive metadata provider declines.
It owns the source-derived no-device outcomes for `50:21`, `50:29`, and
`50:2A`; it does not consume guest block buffers because no raw device is
admitted by the first CLI profile.

## Evidence

`artifacts/build/t199-s37-dem-package-r28/` is an MSVC x64 `/MT`
source-built regression. It checks the three result contracts and the full
canonical `50:00..48` typed-routing sweep. It runs no native Bochs trace.
