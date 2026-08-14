# T198 S100 DEM handle ABI map 001

## Question

Can the S97 `50:42` call name the existing immutable-namespace handle, or is
the source-derived DEM open/read lifecycle encoding it incorrectly?

## Original OpenNT ABI

`dem.h` defines `GETHANDLE(hi, lo)` as `((hi << 16) + lo)`.
`demfile.c` returns an open handle with `BP = low word` and `AX = high word`.
`handle.asm` then loads `BP` from `sf_NTHandle` and `AX` from
`sf_NTHandle+2` immediately before `SVC_DEMFASTREAD`.  `demhndl.c` consumes
that exact `GETHANDLE(getAX(), getBP())` layout for close and read.

The same fast path obtains `ZF` from `sf_nt_seek`; when ZF is clear, the
current position is `BX:SI`; the destination is `DS:DX` and count is `CX`.

## Current mismatch

The contained `50:12` provider currently returns its generated namespace
token as `AX=low`, `BP=high`.  Its `token()` decoder uses the corresponding
inverse `(BP << 16) | AX`.  That self-consistency lets isolated tests pass but
does not match NTDOS, which stores/recovers `AX:BP` in the historical order.

S97's observed fast-read `EAX=4005h` cannot therefore be assumed to identify
the current namespace generation under the reversed decoder.  The existing
provider's correct CF/error result on mismatch is not evidence that the
original guest requested an unavailable capability.

## Disposition

S101 may change the one shared source-derived DEM handle codec to original
`AX:BP` order, update the complete open/seek/read/fast-read/close tests, and
then source-build the current composition without a guest run.  It must not
change fast-read operation semantics, add host state, or admit FASTWRITE.
