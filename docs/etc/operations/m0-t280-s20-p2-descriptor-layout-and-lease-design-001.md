# M0 T280 S20 P2 — DEM persistent guest-location descriptor and lease design

## Decision

The four long-lived locations installed by the original
`demSetDTALocation` are guest numeric locations, not host pointers and not
mapping-manager identities.  Their retained representation must therefore be
a fixed-width `ULONG` in the original 16:16 value form.  The existing
session `guest_memory` mapping-manager instance remains the only guest-memory
manager; no fourth mapper and no guest-address token namespace is introduced.

`adapter-softpc` will own the source-shaped boundary which turns one retained
numeric location into one fresh, exact, bounded session guest-memory lease.
The adapter resolves the numeric address through `adapter-bochs`, acquires the
lease from the thread-bound session, and releases it before the original
caller returns.  `opennt-mvdm-host` must never retain the returned byte
pointer.  The later source changes retain original control and error order;
they replace only a direct dereference with acquire/use/release.

## Reached retained values and exact baseline layout

* `pulDTALocation` is the location of a four-byte far-address cell.  It is
  installed from `DS:AX`, then read to obtain the DTA target.  The far-address
  cell is four bytes and the reached search DTA (`SRCHDTA`) is 43 bytes.
* `pusCurrentPDB` is the location of a two-byte PDB segment value, installed
  from `DS:DX`.  Every reached DEM and SoftPC host-control read needs a fresh
  two-byte read lease.  It is a cross-owner S20 conversion, not a DEM-only
  replacement.
* `pExtendedError` is the location of the packed DOS extended-error record,
  installed from `DS:CX`.  Original `DEMEXTERR` is exactly 9 bytes:
  `UCHAR + USHORT + UCHAR + UCHAR + ULONG`.  The reached error path writes
  only offsets 0 through 4, but the source-defined record remains a nine-byte
  guest layout.  Its final pointer-shaped field is a guest 32-bit value, not
  an x64 host pointer.
* `pSFTHead` is derived synchronously from the local `DOSWOWDATA` input at
  `DS:SI`, whose packed layout is exactly 36 bytes.  `lpSftAddr` becomes the
  retained numeric SFT-chain head.  A DOSSF header is 8 bytes; each packed
  DOSSFT element is 33 bytes.  Its chain must be read as bounded header and
  element leases, never as a permanently dereferenceable `PDOSSF`.

`pDosWowData` itself is deliberately absent from retained state: it is only
the immediate input needed to read `lpSftAddr` during the setter call.

## Required source-preserving adapter surface

The implementation may add a small source-shaped descriptor type and these
operations in `adapter-softpc`; names are provisional until the corresponding
original SoftPC declaration location is selected:

* create a descriptor from original segment/offset inputs;
* acquire a read or read-write lease for an exact numeric guest span;
* decode/read a packed 16:16 value cell without retaining its byte address;
* release/commit the lease on every original success and failure path.

The descriptor is not a host-resource ID.  The existing session
`guest_memory_mappings` instance is used only by the established session lease
subsystem; guest linear/16:16 arithmetic remains numeric so DOS and SFT chain
semantics are preserved.

## Consumer conversion order

1. Define the descriptor and exact packed guest layouts, including x86/x64
   layout assertions for `DEMEXTERR`, `DOSWOWDATA`, `DOSSF` and `DOSSFT`.
2. Convert the setter and the isolated extended-error writer; its four field
   writes give the smallest complete write-lease proof.
3. Convert DTA-cell reads plus the DTA search/FCB consumers, retaining their
   original buffer sizes and flush semantics.
4. Convert all current-PDB consumers together: DEM search/file code and the
   selected SoftPC disk/floppy host-control code.
5. Convert SFT/PDB/JFT chain traversal as bounded hop-by-hop leases.  This
   has to complete before `pSFTHead` can cease being a source pointer.

## Explicit exclusions

This packet does not legalize the broad historical `GetVDMAddr` or
`Sim32GetVDMPointer` façade.  It does not enable a BOP/provider, change a
guest ABI, alter Bochs, introduce a host pointer into guest state, or recover
unreached WOW debugger/extension consumers.  Those consumers remain tracker
inputs for their owning packages.

## P2 result

The representation and every reached layout/consumer class are now defined.
Implementation remains open: no source pointer has yet been converted and S20
cannot close on this design record alone.
