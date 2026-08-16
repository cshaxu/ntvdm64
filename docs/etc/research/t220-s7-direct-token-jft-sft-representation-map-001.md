# T220 S7: direct-token JFT/SFT representation map

## Question

Can the current direct DEM opaque token be treated as, or replaced by, an
adapter-owned JFT/SFT representation in order to clean up a terminated DOS
process?

## Inputs

- OpenNT `dem/demfile.c`, `dem/demhndl.c`, `v86/doskrnl/dos/abort.asm`,
  `v86/inc/pdb.inc`, and `inc/doswow.h`.
- Current direct namespace and handle partitions plus file session.
- T220 S5 direct-token transaction and S6 termination map.

## Original structure and publication contract

`DOSPDB` has `PDB_JFN_Length` at offset 50 and `PDB_JFN_Pointer` at offset
52. `DOSSFT` includes the SFT reference count, mode, flags, PID and 32-bit NT
handle. `VDDAllocateDosHandle` maps the PDB and JFT, selects a free JFN and
SFT, writes the JFT's SFN, zeroes the SFT, and sets its reference count.
`VDDAssociateNtHandle` then stores the host handle in the SFT.

The normal Open/Create service contract is different from allocation: OpenNT
`demOpen` and `demCreateCommon` return the 32-bit host-handle value in
`AX:BP`. Guest DOS owns the subsequent JFT/SFT publication. On ordinary or
abort close, guest DOS passes that same `AX:BP` value through `SVC_DEMCLOSE`.
`demClose` closes it after the guest's JFT/SFT logic has chosen the entry.

## Current representation

The direct namespace partition returns an opaque 32-bit token in the same
`AX:BP` shape. The current guest DOS therefore remains the owner that stores
this value in its SFT and later selects it through `$close`/`DOS_ABORT`.
`bx_ntvdm_dem_handle_partition_v1` receives that token for `50:02` and performs
the corresponding one-token `file_session::release`.

The token is deliberately not a Win32 handle and is not independently a DOS
JFN, SFN, or SFT. It is a host-private replacement for the SFT's historical
NT-handle field. The copied PDB in S5 supplements ownership for diagnostics
and a future proven compatibility seam; it does not duplicate the guest's
JFT/SFT tables.

## Feasibility decision

**Reject an adapter-owned JFT/SFT representation.** It would require reading
and modifying the guest PDB/JFT/SFT chain, duplicate the guest DOS allocator
and close ordering, and create a competing filesystem/kernel owner in
`bx-vdm`. The current direct token already reaches the original post-selection
close location through `50:02`; no additional JFT/SFT bridge is required for
that path.

This also confirms S6: `release_owner(PDB)` has no admitted normal lifecycle
caller. It must remain unbound rather than racing `DOS_ABORT`'s per-SFT
`SVC_DEMCLOSE` calls. Session teardown may still close residual process-local
handles only at composition destruction, which is a distinct host-lifetime
operation and not a DOS PDB termination emulation.

## Failure, scope, and confidence

Opaque-token lookup failure remains the current explicit invalid-handle BOP
failure. Guest JFT/SFT failure and allocation semantics remain guest DOS
behavior; this map does not assert that every DOS handle family is implemented.
FCB, search, Redirector, VDD and device resource ownership remain separate.

Confidence is high for the direct Open/Create-to-Close path because both the
original `AX:BP` contract and the current equivalent token route are explicit
in source. No guest read/write, BOP implementation, runtime execution, or
native trace occurred.

## Result

T220's process-resource ownership audit is complete: the bounded direct-token
transaction is admitted and source-built; premature PDB cleanup and
adapter-owned JFT/SFT reconstruction are both rejected. Future work must
recover complete owner packages rather than add a lifecycle leaf workaround.
