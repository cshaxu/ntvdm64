# M0 T280 S19 — DEM guest lease contract evidence

## Question

Can the five original `demsrch.c` `GetVDMAddr` uses be recovered as one
bounded `adapter-softpc`/`session.guest_memory` contract without preserving
native guest pointers across calls?

## Inputs and procedure

- Reviewed all `GetVDMAddr` sites in selected `dos/dem/demsrch.c`.
- Followed `pulDTALocation` and `pusCurrentPDB` assignments to original
  `dos/dem/demgset.c:559-569`.
- Checked exact DTA/FCB structures in selected `dos/dem/dosdef.h` and existing
  session lease API semantics.
- Distinguished guest numeric segment:offset values from host-local search
  objects resolved by S14-S18.

## Findings

The complete five-row result is the
[guest lease ledger](m0-t280-s19-demsrch-guest-lease-ledger-001.tsv).

`demsrch.c` itself has four direct access forms: two DOS wildcard strings,
one DTA, one FCB search buffer, plus current-PDB reads in helper paths. DTA
and SRCHBUF have exact source-defined extents (43 and 52 bytes respectively),
but the pathname forms require a separately proved bounded string extent.

Most importantly, `demSetDTALocation` saves `GetVDMAddr` results in the
process-global `pulDTALocation`, `pusCurrentPDB`, `pExtendedError`,
`pDosWowData` and `pSFTHead`. The first two are later dereferenced by
`demsrch.c`. A session lease cannot legally survive from `demSetDTALocation`
to a later BOP callback, so a same-named raw `GetVDMAddr` implementation would
either retain a native pointer illegally or guess an unbounded extent.

## Decision

S19 does not implement `GetVDMAddr`. It proves two required source-led
preconditions:

1. recover the DEM long-lived guest-location globals as fixed guest-address
   descriptors, then take fresh leases at each actual dereference; and
2. establish a generic selector-blind imported-host invocation scope that
   opens/closes all adapter-softpc guest-memory leases around a synchronous
   callback.

Both use the existing `session.guest_memory` mapping-manager instance. They do
not use `host_resource`, do not create a mapper, and do not expose a Bochs
pointer. The next package addresses the first precondition across the original
`demgset.c` and its DEM consumers; only then can a typed, source-facing
GetVDMAddr/Sim32 lease body be admitted.
