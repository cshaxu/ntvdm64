# M0 T280 S20 P8 — VDD SFT/JFT shadow and commit design

## Purpose

Recover the reached original VDD SFT/JFT interface without retaining a native
guest pointer.  This is the final non-scalar persistent-location unit in S20.
It preserves the original exported VDD signatures and the original mutable
`PDOSSFT` / `PBYTE` caller contract, while replacing the old NTVDM assumption
that those values permanently alias this process's VDM address space.

## Source constraint

`VDDAllocateDosHandle` returns `PDOSSFT` and `PBYTE` to its caller.  Reached
WOW source subsequently writes the returned objects before it calls
`VDDAssociateNtHandle` or leaves the VDD operation.  Consequently, neither of
the following is legal:

* a short guest-memory lease returned as `PDOSSFT` / `PBYTE`; or
* a 32-bit opaque identifier substituted for either numeric DOS address.

The first expires before the source caller writes it.  The second destroys
the source-visible structure layout and DOS arithmetic.

## Source-shaped replacement

`adapter-softpc` owns an explicit `mvdm_vdd_sft_shadow` object.  One object
holds exactly the source-visible SFT entry and the required JFT byte range,
plus numeric guest descriptors for their write-back locations.  It has these
lifetimes:

1. **Borrow:** fresh bounded leases snapshot PDB/JFT and selected SFT entry
   into host-owned buffers; original VDD API outputs point only at those
   buffers.
2. **Use:** original DEM/WOW code may read/write the returned structures with
   the unmodified `PDOSSFT` / `PBYTE` signatures.
3. **Commit:** each modified original call site explicitly commits its shadow
   after its final source-visible write.  Commit obtains new exact
   read/write leases and writes only the affected JFT/SFT ranges.
4. **Release:** the host shadow and its `host_resource` identity are released
   on commit, explicit failure cleanup, or session teardown.  No guest pointer
   is persisted across any callback.

The identity is used only to validate and own the host-local shadow.  DOS
locations remain numerical descriptors, never mapping-manager tokens.

## Host handle rule

`DOSSFT.SFT_NTHandle` is a 32-bit historical storage field.  On x64, a native
`HANDLE` cannot be stored there.  The shadow implementation must publish the
native handle through the session's existing `host_resource` mapping-manager
instance and write the resulting opaque 32-bit handle identity into that
field.  `VDDRetrieveNtHandle` resolves it only through the same session.
This is the existing handle-identity policy; it is not a new mapping manager.

## Required source changes

The original VDD function names, parameters, return types, search order and
failure results stay intact.  Only direct mutable-pointer call sites require
small `DIVERGENCE`-marked commits after their last write.  The reached set is
the DEM VDD body and the direct `wow32/wkfileio.c` callers; debugger-only
inspection source remains outside the runtime closure.

## Verification

The implementation fixture must prove on both x86 and x64:

* exact SFT/JFT snapshot and commit ranges;
* no commit after a failed lease acquisition;
* stale/released shadow rejection;
* session isolation and teardown cleanup;
* native host-handle identity round trip without pointer truncation; and
* no retained native guest pointer in DEM globals.

## Stop conditions

Do not add a raw `GetVDMAddr` facade, expose a Bochs pointer, tokenise a DOS
numeric address, allow an uncommitted shadow across asynchronous work, or
enable a provider/BOP route.
