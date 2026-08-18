# T226 S2 DEM DASD/IOCTL and lifecycle formal result 001

## Question

Do the current DEM raw-media/IOCTL and lifecycle routing groups retain their
source-derived Direct/Readonly contracts in the formal x64 `/MT` closure?

## Inputs

- Original sources: `demioctl.c`, `demdasd.c`, `demdisp.c`, and `demmisc.c`
  under `src/opennt/base/mvdm/dos/dem/`.
- T226 source/ABI records:
  `t199-s37-dem-dasd-ioctl-family-regression-001.md`,
  `t96-s3-demioctl-source-contract-001.md`, and
  `t226-s2-dem-lifecycle-routing-source-abi-correction-001.md`.
- Formal graph root:
  `build/M0-T226-S2/dasd-lifecycle-20260817b/`.
- MSVC x64 static `/MT`, projected CPU5/P Bochs configuration and the
  manifest-bound `bx-vdm.lib` closure.

## Procedure

1. Generated a fresh formal Ninja graph from the clean T226 manifest.
2. Built the DASD/IOCTL group fixture, lifecycle routing fixture, and the
   existing complete DEM package fixture.
3. Ran the first two fixtures once and the complete package fixture once in
   each enabled mutation profile: Direct and Readonly.

## Observations

All commands exited zero:

- `t226-s2-dem-dasd-ioctl-conformance-fixture.exe` printed its source-derived
  conformance marker. It verifies 50:21 metadata 08/09, raw generic/query
  refusal (AX=1), and 50:29/2A no-BDS refusal (AX=21).
- `t226-s2-dem-lifecycle-routing-fixture.exe` verifies 50:36's no-VDD
  lifecycle return and confirms 50:24 no longer intercepts that route.
- `t225-s19-dem-package-lifecycle-fixture.exe direct` and `readonly` both
  passed, including the added 50:24 CF-clearing assertion.

The lifecycle fixture initially exposed a stale test profile that supplied
only legacy `target_placement`; it was corrected to provide the current
`declared_targets[0]` input. This changes fixture admission only, not product
provider behavior.

## Interpretation

The decimal/hexadecimal service-index defect is repaired: original
`demNotYetImplemented` remains an explicit CF-clearing terminal and the
actual `demEntryDosApp` service owns the source-derived no-VDD return. The
raw-device/BDS profile is still not admitted; no fixture opens a raw host
device, enables an INT13 path, changes Bochs, or adds Overlay behavior.

This is grouped T226/S2 local code-completion evidence. It does not close the
full DEM package or imply native guest trace validation.