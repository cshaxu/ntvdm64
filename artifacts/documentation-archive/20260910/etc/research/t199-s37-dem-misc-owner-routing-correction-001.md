# T199 S37 DEM MISC Owner Routing Correction 001

## Finding

The source-built bound-composition regression exposed two reachable DEM owner
errors that the per-service typed-result sweep could not distinguish:

- `50:11 demLoadDos` is defined in OpenNT `demmisc.c`, but bx-vdm admitted its
  NTDOS loader only after a Namespace-component check.  Its MISC classification
  therefore made the loader unreachable.
- `50:3C demTerminatePDB` is defined in `demsrch.c` and is classified as a
  MISC lifecycle notification.  Its provider incorrectly required Namespace
  ownership and the special precedence reserved for `50:42`; it too was
  unreachable.

## Correction

`50:11` remains MISC-owned.  The boot namespace plane now accepts that MISC
identity solely to copy the already-admitted `NTDOS.SYS` image into checked
guest RAM.  It does not open a host file or provide a raw device.

`50:3C` now accepts its normal original-OpenNT provider selection and MISC
plane classification.  It invokes the existing provider lifecycle release for
the terminating PDB's profile-search state, then returns the original
no-register normal result.  It does not add a VDD hook or host process
manager.

## Evidence

- `artifacts/build/t199-s37-composition-r65-lifecycle/`: source-built MSVC
  x64 `/MT` bound-composition fixture exits zero.  It verifies NTDOS bytes at
  the DI-segment destination, the image-mismatch controlled stop, and the
  `demTerminatePDB` normal result through the common ingress/session route.
- `artifacts/build/t199-s37-dem-package-r66-misc/`: source-built MSVC x64
  `/MT` all-DEM fixture exits zero, retaining the all-73 typed/no-raw-
  pass-through invariant.
- `artifacts/build/t199-s37-composition-r67-pdb-release/`: extends the
  bound-composition fixture with an actual per-PDB search continuation.  It
  invokes `50:3C`, verifies the no-register normal return, then proves that
  the same continuation is rejected after the PDB release.
- The same source-built x64 `/MT` closure also builds and runs
  `t199-s37-dem-session-lifecycle.exe`.  Its fixture-local mechanical-action
  stub is never exercised by the lifecycle provider; it exists only because
  the shared composition object closure has that mechanical ABI reference.
  The test independently verifies MISC ownership, original-OpenNT precedence,
  release of two matching-PDB sessions, retention of a foreign-PDB session,
  and idempotent termination when no matching list remains.

## Boundary

This is an owner/routing repair within the current DEM MISC package.  It does
not close MISC, DEM, or native CLI execution, and no native guest trace was
run.
