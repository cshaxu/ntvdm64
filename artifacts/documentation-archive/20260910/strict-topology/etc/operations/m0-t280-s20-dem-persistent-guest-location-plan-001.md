# M0 T280 S20 — DEM persistent guest-location recovery

Recover the original `demgset.c:demSetDTALocation` and every reached runtime
consumer as one cross-owner package. Replace only the four long-lived native
`GetVDMAddr` results (`pulDTALocation`, `pusCurrentPDB`, `pExtendedError`,
`pSFTHead`) with fixed guest-address descriptors that preserve the original
16:16/linear32 numeric meaning. `pDosWowData` is a local nested lease input,
not retained state. At each source dereference, use a fresh bounded
`adapter-softpc -> session.guest_memory` lease.

This is not permission to write a generic raw-pointer `GetVDMAddr` shim or to
enable a DEM BOP. It must inventory every global consumer before changing the
original body, preserve its original status/carry/error branches, and provide
x86/x64 fixtures for descriptor persistence, stale/out-of-range rejection and
lease release. This includes the reached SoftPC host-control consumers of
`pusCurrentPDB`; no descriptor is an opaque mapping token, host pointer or
broker value.
