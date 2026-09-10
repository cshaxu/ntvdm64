# M0 T245 S10 — Physical-drive table and no-floppy map

## Physical-Drive Data Flow

The prerequisite table is OpenNT `demgset.c:PhysicalDriveTypes[26]`.

1. `demGetDrives` is its writer.  It uses `GetDriveTypeOem`, then calls
   `GetPhysicalDriveType` for a nominally fixed letter to reject SUBST and
   remote-like volumes.
2. `demGetPhysicalDriveType` is a pure table reader.
3. `demDasd.c:demFdiskInit` reads the table before calling `nt_fdisk_init`.
4. `demioctl.c` and COMMAND `cmdmisc.c` also use the table then retain their
   historical `GetDriveTypeOem` fallback.

The S9 guard on `nt_fdisk_init` alone was therefore insufficient: the
upstream writer could still ask about an excluded letter.  S10 adds a named,
session-private guard at both original discovery calls:

- the shared `GetDriveTypeOem` returns `DRIVE_UNKNOWN` without a Win32 query
  for an unadmitted letter;
- imported `GetPhysicalDriveType` returns `DRIVE_UNKNOWN` before its original
  NT-native probe for the same letter.

For an admitted letter neither guard replaces the original source body.  In
particular the source `GetPhysicalDriveType` body still decides whether a
fixed-looking drive is physical rather than SUBST.  This retains the original
classification rather than incorrectly treating the CLI inventory as a raw
DOS drive table.

## No-Floppy Source Result

OpenNT `demFloppyInit` saves AX/BX/CX/DX/DI/ES, calls `diskette_io` for reset,
then parameters, and only enters the floppy-record loop when the parameter
call clears CF and returns a nonzero DL count.  The existing bx-vdm named
`diskette_io` seam returns CF set with an invalid-function condition.  Thus
the original body takes its zero-floppy branch and restores all six saved
registers; it does not imply FDC, DMA, CMOS, media, or INT13 implementation.

## Remaining 5E Composition Seam

`MS_bop_E` itself is a three-byte top-level BOP and invokes `UMBNotify(0)`
(original no-op) then `demDasdInit`.  The current scoped DEM body helper is
correctly tied to four-byte `50:xx` windows, so it cannot be reused silently.
The smallest next component is a named bx-vdm top-level scoped-body seam that
provides the same register-scoped source call, commits a typed `RIP+3`
resume, and has no BOP identity in bx-mantle.  It may call the source body
only after the S9/S10 policy state is bound.

## Verification

Fresh formal graph `build/M0-T245-S10/formal-r1` rebuilt the changed import
and shim closure.  `t245-s9-demdasd-drive-policy-fixture` passed: with a
synthetic C/D/E inventory that admits C/D then excludes D, both source
discovery paths report D unknown; `nt_fdisk_init(D)` and an unbound
`nt_fdisk_init(C)` fail with `ERROR_INVALID_DRIVE` before host-volume access.
