# Adapter Startup Snapshot ABI v1

## Implemented Boundary

`bx_ntvdm_startup_snapshot_transaction_v1` is a new adapter-only C11 record.
It is intentionally separate from the already-proven single-range observation
ABI v1, so r15 behavior and consumers do not acquire hidden multi-range
semantics.

The 240-byte fixed record contains one copied `#UD` exception event, one
copied CPU snapshot, an ordered list of one to four opaque range descriptors,
and their exact total output byte count. It contains no output pointer, Bochs
object, path, firmware label, IVT number, BDA name, NTIO address, BOP selector
or host capability.

## Enforced Preconditions

- exact ABI header and zero reserved/flag fields;
- valid copied CPU exception boundary with vector `6` only;
- valid CPU state;
- 1..4 non-empty physical ranges within the supplied aperture;
- non-zero unique opaque IDs;
- no overlapping ranges;
- overflow-safe sum exactly equal to the adapter-owned output capacity.

This is sufficient to describe the three declared pre-entry ranges—IVT slice,
BDA byte and virtual-interrupt span—without assigning their guest meaning to
Bochs or the wire format.

## Verification

`bx-ntvdm-startup-snapshot-abi-test` was configured, built and run in
`artifacts/build/current/adapter-cli-r1` with the existing MinGW C11 toolchain.
It accepts the three-range 53-byte layout and rejects duplicate IDs, overlap,
incorrect total output and a non-`#UD` boundary.

## Session Lifecycle

The record is now a third, separate host-session pending-operation kind. It
can be queued only after ABI preflight, carries the same copied boundary, and
is transferred exactly once. Wrong-kind takes leave it intact; a same-kind
boundary mismatch cancels it before exposing output. The existing host-session
test passes its write, single-range observation and snapshot lifecycle cases.

## Not Yet Connected

The record is not parsed from BYOB profile metadata and has no Bochs consumer.
Consequently it cannot yet observe new guest memory or affect r16. Those remain
distinct gates in `design/ADAPTER-STARTUP-FIRMWARE-SNAPSHOT.md`.
