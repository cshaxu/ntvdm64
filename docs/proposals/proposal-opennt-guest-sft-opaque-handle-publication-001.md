# Proposal: OpenNT guest SFT opaque-handle publication

## Purpose

Recover the original guest-DOS to host-handle publication boundary needed by
the retained Redirector named-pipe sources, without restoring the historical
raw `HANDLE` representation in guest SFT state.

## Source basis

`src/opennt/base/mvdm/dos/v86/redir/namepipe.asm:MapNtHandle` maps a DOS JFN
through the guest SFT and returns `sf_NtHandle` in `BP:BX`.  The original
assembly assumes that field contains a raw 32-bit NT handle.  The modern
product's shared handle manager deliberately prohibits that representation:
guest-visible values must be session-owned opaque tokens.

The retained DOS sources including `fcbio2.asm`, `file.asm`, `ioctl.asm` and
`sf.inc` are the owner evidence for production and retirement of SFT entries.

## Boundary

The guest DOS owner retains JFN/SFT lookup, SFT lifetime and its original
failure order.  `bx-vdm` retains the shared 32-bit opaque token manager and
checked fixed-width result contracts.  bx-core and bx-mantle remain blind to
SFT, DOS, Redirector and host-handle identity.

## Admission plan

1. **S1 — source/ABI audit:** identify every retained producer, consumer and
   close/dup/terminate path of `sf_NtHandle`; compare it to current v2 token
   publication and define one exact opaque-field contract.
2. **S2 — source-first recovery:** reuse the smallest composable original
   producer/consumer source through an adapter/shim.  An unavoidable guest
   overlay must be limited to replacing raw-handle storage with the existing
   opaque 32-bit token and must document every altered line.
3. **S3 — lifecycle verification:** prove open/publication, `MapNtHandle`
   consumption, invalid/closed token rejection, dup/close/termination and
   no raw `HANDLE` bytes in checked guest memory.

## Exit rule

This package does not implement asynchronous Redirector I/O or guest ANR
delivery.  It publishes the one prerequisite token seam.  Functional
`57:23/24` remains blocked until the independently owned selector-blind
`int5c` completion-delivery package is also complete.
