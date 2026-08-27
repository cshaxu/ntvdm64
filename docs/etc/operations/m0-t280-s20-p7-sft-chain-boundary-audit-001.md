# M0 T280 S20 P7 — SFT chain boundary audit

## Result

The remaining retained `pSFTHead` cannot be recovered by the scalar/DTA
pattern alone.  The original SFT chain is guest memory, but reached OpenNT
host functions return writable `PDOSSFT` and `PBYTE` pointers to their host
callers:

* `VDDAllocateDosHandle` returns both an SFT pointer and a JFT pointer;
* `VDDAssociateNtHandle` accepts that SFT pointer later;
* `VDDReleaseDosHandle` and `VDDRetrieveNtHandle` return or mutate the same
  guest-backed SFT/JFT values; and
* `wow32/wkfileio.c` directly mutates the returned values before/after those
  calls.

A short guest-memory lease may not cross these host callbacks.  Replacing the
pointer with an opaque token would also break original source field access and
is prohibited for numeric guest addresses.  Therefore no correct S20 patch
may simply substitute `mvdm_guest_location` for `PDOSSF` and declare closure.

## Original source evidence

* `demgset.c:575-576` obtains `DOSWOWDATA.lpSftAddr` and installs the chain
  head.
* `demfile.c:GetFreeSftEntry`, `VDDAllocateDosHandle`,
  `VDDReleaseDosHandle`, and `VDDRetrieveNtHandle` traverse and expose the
  chain.
* `wow32/wkfileio.c` has direct uses of all four VDD functions and direct
  mutations of returned `pSft`/`pJFT` values.
* Debugger/extension sources also inspect `pSFTHead`, but are not in the
  active runtime closure and do not reduce the host ABI requirement above.

## Required next owner package

The correct recovery unit is a source-shaped **VDD SFT/JFT host-buffer
contract**, not a generic raw `GetVDMAddr` API.  Its minimum design must:

1. retain numeric SFT/JFT guest locations and chain links;
2. give original VDD/WOW callers host-local mutable shadow buffers only when
   their source ABI requires direct fields;
3. use the existing session `host_resource` mapping-manager instance to own
   those host-only buffers and their identities;
4. commit a shadow buffer to a fresh bounded guest lease at a defined original
   VDD lifecycle boundary; and
5. reject/clean stale or abandoned buffers without retaining a guest pointer
   across a callback.

This is a material new adapter capability, not an incidental S20 scalar
conversion.  It needs its own admitted design/implementation packet before
the current `pSFTHead` global is changed.

## Current S20 disposition

S20 has fully recovered extended-error, DTA and current-PDB retained
locations.  It must remain active because `pSFTHead` is still a native
retained pointer.  The SFT prerequisite is now explicit rather than hidden in
a future source build failure.
