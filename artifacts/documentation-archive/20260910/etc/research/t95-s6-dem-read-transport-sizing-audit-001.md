# T95 S6 DEM read transport sizing audit 001

## Question

Can the existing generic gather-read and multi-write transports carry the
source-defined `SVC_DEMOPEN` / `SVC_DEMREAD` / `SVC_DEMCLOSE` family without
silently changing a DOS read request?

## Inputs

- `base/mvdm/dos/dem/demfile.c:117-280` (`demOpen`).
- `base/mvdm/dos/dem/demhndl.c:28-210` (`demClose`, `demRead`).
- `base/mvdm/dos/v86/doskrnl/dos/file.asm:205-220` and
  `handle.asm:458-474` (NTDOS call sites).
- Current generic transports:
  `src/bx-ntvdm-adapter/bx_ntvdm_guest_gather_read_action_v1.h` and
  `bx_ntvdm_multi_write_abi.h`.

## Observations

1. `DEMOPEN` consumes a canonical NUL-terminated DOS path at `DS:SI`.  The
   first profile has source evidence for the 128-byte `OpenBuf` staging form,
   so the existing 256-byte gather-read ceiling can carry the *input* only
   after an exact open-request decoder is admitted.
2. `DEMREAD` receives `CX` as an unsigned 16-bit byte count.  Its source
   passes `(DWORD)getCX()` to `ReadFile` and writes that many bytes into the
   guest buffer at `DS:DX`; it returns the actual byte count in `AX`.  A DOS
   caller can therefore request any size from 0 through 65535, not merely the
   256-byte gather bound or the current 4096-byte multi-write payload limit.
3. The current multi-write v1 is intentionally capped at 4096 payload bytes
   and the installed bridge owns a fixed 4096-byte payload arena.  It cannot
   represent a successful 4097--65535 byte `DEMREAD` result.  Splitting one
   historical BOP across several guest-visible commits would violate the
   single `ReadFile` completion/`AX` contract and creates partially visible
   data if a later chunk fails.
4. `DEMREAD` can validly return a short read, including zero at EOF.  The
   future result is consequently one checked range of exactly the returned
   byte count plus `AX`, CF and resume; it must not promise `CX` bytes before
   the host read completes.
5. Neither the existing gather-read record nor multi-write record exposes a
   host pointer.  Their fixed-width range/result model is structurally usable,
   but their current capacity is deliberately too small for this service.
6. The NTDOS caller invokes `Align_Buffer` before the BOP. It folds
   `DS:DX` into an equivalent normalized real-mode pair (`DS += DX >> 4`,
   `DX &= 0x0f`) and then reduces `CX` if `DX + CX` would cross 64 KiB.
   The actual BOP snapshot therefore has one non-wrapping guest range whose
   physical start is `DS*16 + DX`; a request at offset `ffffh` that would have
   zero remaining bytes fails before `DEMREAD`. The service result's physical
   range must use that post-normalization snapshot, not the original DOS API
   pointer and not a speculative segment-wrap algorithm.

## Decision

No existing transport is reused as though it supported the whole source
contract.  The read-handle family needs a separately versioned, generic
**bounded bulk-result transport** before any `50:16` dispatch.  Its maximum
must cover the full DOS `CX` range (65535 bytes), it must preflight one
physical guest range and the entire copied payload before any byte is written,
and its result must be applied only after the source-derived adapter operation
has completed.  The transport must contain no selector, service, path, handle
or host I/O policy.

The existing 4096-byte multi-write remains correct for small structured
results such as current-directory/DPB records and must not have its limit
silently widened.  A dedicated versioned bulk record prevents that
configuration-oriented requirement from turning into an unbounded generic
memory channel.

## Remaining source gates

- Exact `demClientError` / hard-error mapping for Open, Read and Close.
- Source-shaped guest representation of an adapter-private open handle in the
  NTDOS SFT, including collision/stale-handle behavior.
- Full `50:00` seek lifecycle/error contract. The reached CONFIG.SYS path
  requires seek-to-end and seek-to-begin before its first read, so seek is now
  part of the paired capability rather than an optional later extension.

No Bochs modification, host file I/O, BOP dispatch or transport code is
admitted by this audit.
