# M0 T347 S1 — COMMAND/DEM/SoftPC scalar-alias registration and lifetime ledger

## Question

What original contract is carried by the three persistent guest locations
registered through `SVC_CMDSETINFO`, and what is the smallest existing
mapping-manager route that can recover it without keeping a host pointer?

## Inputs

- Selected original guest source: `src/mvdm-guest/dos/v86/doskrnl/dos/`
  `msinit.asm`, `msproc.asm`, `macro.asm`, and `misc.asm`.
- Selected original host source: `src/mvdm-host/dos/command/` `cmdmisc.c`,
  `cmdpif.c`, `cmddata.c`, `cmd.h`; `dos/dem/demmisc.c`; and
  `softpc.new/host/src/nt_fdisk.c`, `nt_rflop.c`.
- Existing implementation seam:
  `adapter-mvdm-host-out/softpc/include/mvdm_command_guest_state.h`,
  `mvdm_command_guest_state.c`, `mvdm_guest_location.[ch]`, and session
  guest-memory leases.
- OpenNT original comparison counterparts under
  `O:\repos.external\opennt\base\mvdm`.

## Original registration and use facts

| Location | Registration | Original host consumers | Original guest consumers | Width and observable rule |
| --- | --- | --- | --- | --- |
| `SCS_ToSync` | `msinit.asm` puts `DS:DX` at `SCSINFO`; `cmdSetInfo` derives `&pSCSInfo->SCS_ToSync`. | `cmdGetNextCmd` writes `0xff` after building a command response. | `macro.asm` tests the byte while synchronising CDS state, then clears it to zero. | One byte. A failed location write must take the existing COMMAND invalid-address result before the response is reported. |
| `SCS_Is_Dos_Binary` | `msinit.asm` puts `DS:BX` at the DOSDATA byte; `cmdSetInfo` stores its pointer in `pIsDosBinary`. | `cmdmisc.c` writes `1` on both command-result paths; `cmdpif.c` resets to `0` for a PIF. | `msproc.asm` performs `xchg` with `AL`, consuming and clearing the byte. | One byte. `1` means the host has already classified a DOS binary; zero requests ordinary DOS classification. |
| `SCS_FDACCESS` | `msinit.asm` puts `DS:CX` at the DOSDATA word; `cmdSetInfo` stores its pointer in `pFDAccess`. | `nt_fdisk.c` and `nt_rflop.c` increment/decrement it for automatic locks; `demDiskReset` resets it to zero. | `misc.asm` tests nonzero before issuing `SVC_DEMDISKRESET`. | One little-endian `WORD`; increments/decrements retain original unsigned 16-bit wrap behavior and reset writes zero. |

`pIsDosBinary` is defined by original `cmddata.c`; `pFDAccess` is defined by
original `nt_fdisk.c` and declared by `cmd.h`/`nt_fdisk.h`. The original
process-wide pointer storage is therefore not a separate service provider: it
is the shared host representation of three guest-owned DOSDATA values.

## Current binding facts

- `cmdSetInfo` already records the `SCSINFO` base and `SCS_ToSync` offset in
  `mvdm_command_guest_state`; its two direct writes use a bounded write lease.
- It still stores raw `GetVDMAddr(DS:BX)` and `GetVDMAddr(DS:CX)` results in
  `pIsDosBinary` and `pFDAccess`.
- Consequently the five host access forms (`binary=1`, `binary=0`, `fd=0`,
  `fd++`, `fd--`, plus `fd` test) remain outside the current lease/epoch
  contract. They must not be declared closed merely because `SCS_ToSync` has
  been migrated.
- A `mvdm_guest_location` stores only 16:16 numeric location data. Each
  operation acquires a `session_guest_memory` lease through the bound session;
  acquisition fails when guest memory is inactive, the span is invalid or the
  session is not bound. Lease release rejects epoch mismatch. Session disposal
  ends guest memory and disposes its mapping instances.

## Selected recovery disposition

The first workable recovery rung is the existing same-shaped adapter family,
not a new mapper or a new COMMAND/DEM provider:

1. Keep all original NTDOS, COMMAND, DEM and SoftPC bodies and their ordering.
2. Extend the existing thread-bound `mvdm_command_guest_state` state to retain
   the two additional numeric `mvdm_guest_location` values, together with the
   existing owner/session validity condition.
3. Provide only scalar operations in that adapter:
   `write_is_dos_binary(u8)`, `read_fd_access(u16*)`,
   `write_fd_access(u16)`, and `add_fd_access(u16 modular_delta)`.
   The `UINT16_MAX` delta is the source-preserving decrement representation.
   No operation returns a guest pointer.
4. Bind all three locations atomically in `cmdSetInfo`. Registration failure
   preserves its existing `CF=1`, `AX=ERROR_INVALID_ADDRESS` result. Each
   later access obtains and releases a synchronous lease at its original
   access point; unavailable/stale locations return the source caller's
   existing failure direction, never a synthetic success.

The adapter must capture the active session epoch at registration and reject a
later execution whose bound session or epoch no longer matches. The existing
lease epoch protects an acquired lease; the added registration epoch prevents
a durable numeric location from silently surviving a guest-memory end/begin
cycle.

## Recovery ladder

| Rung | Disposition |
| --- | --- |
| Original source reuse | Selected. All algorithmic bodies and guest layout/order remain in their original mirrors. |
| Smallest same-shaped facade | Selected. Existing `adapter-mvdm-host-out/softpc` location/lease seam supplies only the unavailable safe address binding. |
| External-code intrusion | Rejected. No external code needs modification. |
| Newly authored provider | Rejected. The adapter owns only bounded mechanical location operations, not COMMAND, DEM, disk or BOP policy. |

## Procedure and observations

1. Searched all selected guest and host mirrors for the three DOSDATA symbol
   names and both historical pointer globals.
2. Compared the relevant selected host files against the OpenNT source
   counterparts. The original control order is retained; the existing
   `SCS_ToSync` divergence is a partial, not a complete, migration.
3. Read the existing location and session lease implementations to confirm
   fixed-width 16:16 locations, bound-session acquisition, checked spans and
   lease epoch rejection.

## Interpretation and confidence

High confidence: the three locations are one original registration cohort and
must migrate together. The selected adapter contract is bounded and does not
change guest layout, BOP selection, host device policy or the original DOS
meaning of any value. S2 can implement this contract without a new mapping
manager or a new provider.

## Follow-up

T347/S2 implements the selected adapter contract and focused negative tests.
T347/S3 then changes every listed original pointer-retention/dereference site
in source order and removes the raw durable aliases only after every consumer
has migrated.
