# M0 T245 S9 — DEM DASD admitted-drive policy closure

## Scope

This closes only the fixed-volume admission prerequisite mapped by
`BOP-DEPENDENCY-110`.  It does not route BOP `5E`, reserve UMB, enable
floppies, or change any bx-core/bx-mantle behavior.

## Original Body And Minimal Divergence

OpenNT `demdasd.c:demFdiskInit` owns the historical all-letter enumeration.
Its per-letter host owner (`nt_fdisk.c`) expected an NT4 VDM process that
could inspect every fixed volume.  The existing project divergence is
`src/bx-vdm/bop/shim/demdasd_ioctl_shim.c:nt_fdisk_init`, which already uses
public Win32 `GetDriveTypeA`, `CreateFileA`, geometry and free-space calls.

The new named divergence is deliberately earlier and narrower:

1. `dem_v2_startup` captures a `bx_ntvdm_host_drive_snapshot_v1` once from
   the copied engine include/exclude masks.
2. `dem_v2_runtime_session_bind` copies that snapshot into the active
   single-session DASD shim before DEM ingress exists.
3. `nt_fdisk_init` rejects a missing, unbound, or excluded letter with the
   normal `ERROR_INVALID_DRIVE` failure before its first host-volume query.
4. An admitted letter still follows the original fixed-volume eligibility
   branch and then the pre-existing public-Win32 compatibility body.

Thus the original DEM enumeration and failure handling stay in the imported
source; the shim merely enforces the CLI admission boundary that OpenNT's
ambient NT4 host did not need.

## Verification

Fresh formal graph: `build/M0-T245-S9/formal-r2`.

`t245-s9-demdasd-drive-policy-fixture` uses a synthetic C/D/E fixed-volume
inventory, admits C/D, then excludes D.  It proves C is admitted, D and E
are not, D fails `nt_fdisk_init` with `ERROR_INVALID_DRIVE` without querying
the host, and a reset session rejects C before a probe as well.  The focused
fixture compiled and passed in the 114-edge formal bx-vdm build.

## Remaining 5E Boundary

`MS_bop_E` still cannot resume: its next original call is
`demDasdInit`, which also invokes `demFloppyInit`.  Floppy/INT13/FDC/DMA/CMOS
semantics remain a distinct selector-blind machine owner.  This S does not
replace that call with an ignore or a partial reset.
