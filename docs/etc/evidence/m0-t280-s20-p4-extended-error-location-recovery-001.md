# M0 T280 S20 P4 — extended-error location recovery

## Recovered source path

The first retained DEM guest location is now source-connected:

* `demSetDTALocation` preserves original `DS:CX` as
  `extended_error_location`, a numeric real-mode guest descriptor;
* original `demRead` retains its named-pipe decision, error classification,
  field order, and success/failure branches;
* instead of retaining `PDEMEXTERR`, its one reached write path acquires a
  fresh read/write lease for the exact nine-byte `DEMEXTERR` guest record,
  stores the original four fields, commits and releases before continuing;
* failed acquisition or commit follows the pre-existing `readFailureExit`
  path rather than manufacturing success.

The original last `ExtendedErrorPointer` field is explicitly a numeric guest
`ULONG`, preserving the packed nine-byte DOS ABI across x86/x64.  It is not
used by this reached writer.

## Verification

* `mvdm_guest_location` compile/link/run fixture passed on formal MSVC x86
  and x64.
* The formal DEM source syntax graph, now including `demhndl.c`, passed on
  x86 and x64.
* The declaration syntax fixture has assertions for `DEMEXTERR == 9`,
  `DOSWOWDATA == 36`, `DOSSF == 8`, and `DOSSFT == 33`; it passed on x86 and
  x64.  This is intentionally a `/Zs` syntax/layout graph, so it emits no
  executable.  An attempted x64 executable invocation was rejected only
  because that graph correctly creates no `.exe`; the successful Ninja syntax
  result is the relevant evidence.

## Remaining S20 scope

`pulDTALocation`, `pusCurrentPDB`, and `pSFTHead` remain native retained
pointers in their original bodies.  DTA/FCB, PDB and SFT chain consumers must
be converted before S20 can close.
