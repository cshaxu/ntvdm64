# M0 T280 S20 P5 — DTA location recovery

## Recovered source path

`pulDTALocation` is removed from the reached DEM source.  The original
`DS:AX` setter input is retained as numeric `dta_location`, the four-byte DOS
far-address cell is read under a short read lease, and each current DTA target
is acquired separately.

* `demFindFirst` and `demFindNext` acquire the exact 43-byte `SRCHDTA` range
  read/write, invoke the original search logic, then commit/release before
  returning to guest code.
* `demFCBIO` obtains its exact `CX` byte range once, uses a read/write lease
  only when host `ReadFile` writes guest bytes, and uses a read-only lease for
  host `WriteFile`.  It preserves partial-read commit behavior and releases
  on every original completion/failure branch.
* A failed numeric-cell read, target lease acquisition, or commit follows the
  existing DEM error direction with `ERROR_INVALID_ADDRESS`; it never
  substitutes success or exposes a native pointer.

## Verification

The formal DEM source syntax graph including `demgset`, `demsrch`, `demfcb`
and `demhndl` passed on MSVC x86 and x64.  The bounded-location fixture also
passed on both architectures, including a fresh read of a four-byte DOS far
address cell followed by its target lease.

The graph continues to report pre-existing source warnings in unrelated
historical paths; there are no DTA-conversion syntax errors.

## Remaining S20 scope

`pusCurrentPDB` and `pSFTHead` remain the two unrecovered retained native
pointers.  PDB is a cross-owner DEM/SoftPC scalar read; SFT is a variable
chain requiring hop-bounded leases.  S20 remains active.
