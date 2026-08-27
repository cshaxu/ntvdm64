# M0 T280 S20 P3 — guest-location lease facade fixture

## Implemented boundary

`adapter-softpc/mvdm_guest_location_*` is the source-facing mechanical
boundary for a retained real-mode guest location.  The retained object has
only `segment`, `offset` and a validity marker.  It is not a host pointer,
opaque identity, mapping key or retained lease buffer.

The facade obtains its bytes only through the current session's existing
guest-memory lease service.  Numeric 16:16 conversion stays numeric:
`segment << 4 + offset`.  The session owns the guest-memory mapping-manager
instance and lease context; this facade deliberately does not publish the
numeric address through `mapping_manager`, because doing so would turn a DOS
address into an opaque value and change its arithmetic semantics.

## Fixture coverage

`tests/adapter-softpc/t280_s20_mvdm_guest_location_fixture.c` proves:

* real-mode descriptor persistence and the expected linear address;
* fresh read/write bounded lease acquisition and committed writeback;
* packed far-value decoding without pointer conversion;
* out-of-range rejection; and
* release, unbind and session disposal with no retained lease.

## Formal execution

Both formal Ninja graphs compiled, linked and ran successfully outside the
sandbox on 2026-08-26:

* MSVC x64: `build/M0-T280/x64-mvdm-guest-location`;
* MSVC x86: `build/M0-T280/x86-mvdm-guest-location`.

## Limit

This is a reusable boundary proof only.  No OpenNT DEM mirror body is yet
rewritten to use it, and it does not replace the broad historical
`GetVDMAddr`/`Sim32GetVDMPointer` APIs.  The pending S20 work is to convert
the four retained DEM locations and all reached consumers using the P2 order.
