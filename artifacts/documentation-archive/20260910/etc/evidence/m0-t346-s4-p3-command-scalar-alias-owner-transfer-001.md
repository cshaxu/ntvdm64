# M0 T346 S4 P3 — COMMAND/DEM scalar-alias owner transfer

## Purpose

T346/S4 needs a real guest graphics workload but must not turn an early trace
marker into a graphics-local or one-service repair.  This source audit records
the complete owner cohort reached by the observed `54:05` ingress and assigns
it to a future, separately admitted package.

## Original contract

`mvdm-guest/dos/v86/doskrnl/dos/msinit.asm` calls `SVC_CMDSETINFO` with three
independent DOSDATA locations:

- `DS:DX` — `SCSINFO`, including `SCS_ToSync`;
- `DS:BX` — `SCS_Is_Dos_Binary`;
- `DS:CX` — `SCS_FDACCESS`.

The original `cmdSetInfo` in `mvdm-host/dos/command/cmdmisc.c` retained all
three as host-process aliases.  In the historical x86 process this was valid
because `GetVDMAddr` numerically formed a low VDM address.  It is not a valid
cross-session or mapping-manager lifetime contract for this product.

## Current composition

The existing `mvdm_command_guest_state` correctly retains only the first
location as a session-thread-local real-mode location and leases it at the
original `SCS_ToSync` access points.  `cmdSetInfo` still assigns raw
`GetVDMAddr` aliases to `pIsDosBinary` and `pFDAccess`.

Those aliases are not isolated leaf writes:

- `pIsDosBinary` is written by original COMMAND `cmdGetNextCmd` and
  `cmdpif.c`, then consumed and reset by NTDOS `msproc.asm`.
- `pFDAccess` is read/reset by original DEM `demDiskReset`, incremented and
  decremented by original `nt_fdisk.c` and `nt_rflop.c`, and read by NTDOS
  `misc.asm` before its disk-reset host service.
- The immediate NTDOS continuation performs device initialization before the
  later `SVC_DEMGETDPBLIST`; `CONFIG.NT` is the still-later `54:0C` service.

## Disposition

This is a single original COMMAND/DEM/SoftPC scalar-alias lifecycle cohort:
the three registration positions, all original read/write sites, per-session
binding, cancellation/teardown and failure result must be recovered together.
The existing session mapping manager is the only permissible address/lease
authority.  A future package may retain original source bodies and replace
only their alias acquisition/release through the existing same-shaped adapter;
it must not create a second mapper or assign the current fixed timeout to this
cohort without distinct evidence.

No source body, BOP result, guest image, graphics controller or runtime input
changed in this P.  The fixed observation establishes only ingress at `54:05`,
not whether this cohort caused the subsequent timeout.
