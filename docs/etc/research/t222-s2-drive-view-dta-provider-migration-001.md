# T222 S2 Drive-View DTA Provider Migration

## Question

Can `50:1B demSetDTALocation` retain the OpenNT `demgset.c` contract while
moving from the boot-namespace residual route to the shared drive-view
provider?

## Inputs And Procedure

OpenNT `src/opennt/base/mvdm/dos/dem/demgset.c:548-569` defines the four
DS-relative locations and reads `DOSWOWDATA::lpSftAddr` at offset 32.  The
existing typed DTA service preserves that bounded two-byte read.  The
drive-view provider now wraps that service; package session remains the only
mechanical reader and installs the copied registration after completion.

The normal Direct/Readonly session fixture places the SFT offset at
`DS:SI+32`, invokes `50:1B`, and verifies all four copied physical locations.
MSVC x64 `/MT` build/link/run in
`build/M0-T222-S2/023-drive-view-dta-final` exited zero.

## Observation And Interpretation

`demSetDTALocation` has no historical error or flags result.  Its correct
terminal is a forward resume, not a synthetic CF-clear result.  Both Direct
and Readonly therefore register the identical bounded guest state without
host mutation.  Overlay/Virtual behavior was not changed.

## Follow-up

The DTA registration is now under the one drive-view provider; the remaining
DPB/media/clock identities still need the complete family regression.
