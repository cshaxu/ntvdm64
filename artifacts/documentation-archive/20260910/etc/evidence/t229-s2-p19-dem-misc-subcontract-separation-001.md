# T229 S2 P19 — DEM Misc Subcontract Separation

## Question

Does the current `misc-lifecycle-console-wow` provider preserve the distinct
OpenNT contracts for the non-debug/no-VDD/non-WOW CLI profile, rather than
claiming that all of them are one generic success path?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demmisc.c`: `demSetV86KernelAddr`,
  `demLoadDosAppSym`, `demFreeDosAppSym`, `demEntryDosApp`,
  `demDOSDispCall`, `demDOSDispRet`, `demOutputString`, `demInputString`,
  `demDiskReset`, and `demWOWFiles`.
- `src/opennt/base/mvdm/dos/command/cmdmisc.c`: `cmdSetInfo`, which records
  `pFDAccess` from the guest `DS:CX` locator.
- `src/bx-vdm/bx_ntvdm_dem_misc_plane_v1.c`.

## Procedure

1. Split the existing combined `cli_noop_service` classifier into explicit
   ordinary-return contracts and an isolated `demDiskReset` branch.
2. Preserve the four-byte BOP continuation and untouched CPU state for the
   selected no-debug/no-VDD/non-WOW branches.
3. Compile and run the formal Ninja package fixture in both selected modes.

## Observations

- `50:1D`, `50:34`, `50:35`, `50:36`, `50:37`, `50:38`, `50:39`, `50:3A`,
  and `50:3E` each reach source-proven ordinary-return branches in the
  contained profile; they are no longer represented as an undifferentiated
  provider claim.
- `50:23` is isolated.  OpenNT resets host floppy/fixed-disk state and clears
  the guest word registered by COMMAND `54:05`; the standalone DEM session
  currently owns neither a raw-media backend nor that cross-package locator.
  Its current resume is valid only for the explicit empty-resource branch,
  not an implementation of media reset or a write to an inferred guest address.
- Formal target `t225-s19-dem-package-lifecycle-fixture.exe` rebuilt and
  returned zero for `direct` and `readonly`.

## Interpretation

This is a source-contract correction, not completion of the misc owner group.
A later COMMAND/DEM composition step must pass the already validated `54:05`
`fd_access` physical locator through a fixed boundary before `50:23` may clear
it, and any admitted raw-media reset stays owned by the machine/media layer.
No Bochs, DOS, device, or selector meaning was introduced below `bx-vdm`.

## Follow-up

Continue T229/S2's `misc-lifecycle-console-wow` group with the lifecycle and
terminal providers, then add the cross-package `cmdSetInfo`/`demDiskReset`
composition contract when its COMMAND owner package is admitted.