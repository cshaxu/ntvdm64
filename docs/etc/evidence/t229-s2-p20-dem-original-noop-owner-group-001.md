# T229 S2 P20 — DEM Original-Noop Owner Group

## Question

Do every DEM identities selected by OpenNT's `demNotYetImplemented` preserve
that original CF-clear return in both product modes, without becoming a generic
CLI-unavailable failure?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdisp.c`: entries for `50:1F`, `50:24`,
  `50:26`, `50:28`, `50:2B`, `50:40`, and `50:43`.
- `src/bx-vdm/bx_ntvdm_dem_plane_v1.c` and
  `src/bx-vdm/bx_ntvdm_dem_provider_v1.c`.
- `tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`.

## Procedure

1. Treat all seven dispatcher identities as one `original-noop` owner group.
2. Assert one typed four-byte BOP resume for each identity, no GPR delta, and
   a CF-only update that clears CF.
3. Rebuild and run the formal Ninja package fixture for Direct and Readonly.

## Observations

All seven identities reach the original-noop provider in both modes.  Each
returned `RESUME` at `fault_rip + 4`, preserved general registers, and wrote
only `CF=0`.  The formal target
`t225-s19-dem-package-lifecycle-fixture.exe` returned zero for `direct` and
`readonly`.

## Interpretation

This owner group is code-complete for the selected profiles.  These calls do
not request a host capability, so Direct and Readonly deliberately share the
original behavior.  The result does not imply that adjacent raw-media,
error-lock, fastread, or misc owner groups are complete.

## Follow-up

Continue T229/S2 with the DASD/IOCTL owner group and preserve its explicit
machine/device ownership boundary.