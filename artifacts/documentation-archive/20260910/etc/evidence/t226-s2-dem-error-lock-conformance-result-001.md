# T226 S2 DEM error/lock conformance result 001

## Question

Does the complete three-service DEM error/lock group retain its original
bounded contracts under the current bx-vdm package route?

## Source basis

- `demerror.c`: `demSetHardErrorInfo` stores DS:DX and DS:BX locators without
  host I/O; `demRetry` has the debug invalid-service `AX=FF, CF=1` outcome.
- `demlock.c`: a failed lock/unlock reaches `demClientError`; without an
  admitted host file handle, the CLI's contained zero-last-error failure is
  `ERROR_ACCESS_DENIED` (`AX=5, CF=1`).
- The original CCPU/SAS translation units cannot compose into the x64 CLI;
  `bx_ntvdm_dem_error_lock_plane_v1` is the checked source-derived seam.

## Procedure and result

The formal MSVC x64 `/MT` Ninja graph built
`t226_s2_dem_error_lock_conformance_fixture.c` with `bx-vdm.lib`. The fixture
asserted all three records through BOP ingress and the DEM package plane:

- `50:32` accepts only bounded real-mode DS:DX and DS:BX locators, stores
  `0x0da2`/`0x0ae0`, resumes at `RIP+4`, and rejects aperture crossing;
- `50:33` produces `AX=00FF, CF=1` without a hidden retry composition; and
- `50:3F` produces `AX=0005, CF=1` without a host HANDLE or lock operation.

The fixture printed its conformance marker and exited zero.

## Limitation

This is a closed semantic subfamily inside DEM S2. It neither implements a
hard-error UI nor authorizes host lock operations, and it does not close the
remaining DEM owner groups.