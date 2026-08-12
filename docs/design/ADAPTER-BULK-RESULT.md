# Adapter Bounded Bulk Result

## Purpose

This is the generic mechanics counterpart needed when one source-derived
adapter operation has already produced a bounded payload that is too large for
the 4096-byte structured multi-write ABI. It does not add a filesystem,
BOP/DEM dispatch, host API, selector, path, token, or Bochs device feature.

The first intended consumer is the already source-closed `DEMREAD` family,
whose one historical call may return 0 through 65535 bytes into one normalized
real-mode guest range. That consumer remains outside this ABI.

## v1 shape

One versioned transaction contains:

- the immutable copied exception boundary and CPU snapshot;
- one physical guest address;
- one declared byte count in the inclusive range `0..65535`;
- one checked generic v2 CPU result, bound to the same boundary; and
- no payload pointer, selector, service, segment, host handle, path, or
  callback.

The payload is an adapter-owned, fixed 65535-byte same-island arena obtained
only through a matching take operation. It is not part of the public record,
and the record's byte count must exactly equal the borrowed payload length.
After a successful take, the metadata is consumed but the borrowed bytes remain
valid until the next bulk queue or session teardown, so the generic Bochs
consumer can copy the declared span without a host-pointer handoff.
The v1 record has exactly one target range: multiple writes remain owned by
the existing bounded multi-write ABI and cannot be smuggled into this one.

A zero-length transaction is valid only as an empty payload plus a valid CPU
result; it causes no RAM copy. This preserves EOF-style source results without
inventing a zero-length memory operation. A nonzero transaction requires one
ordinary-RAM range fully preflighted before its first byte is written.

## Bochs mechanics

Bochs receives the opaque record and same-island payload only after the
adapter has matched the copied boundary. It must:

1. reject an invalid record, an aperture overflow, a nonmatching CPU result,
   or payload-size mismatch;
2. for nonzero length, use the existing `ordinary_ram_writable` check over the
   entire range, then the existing `copy_to_ordinary_ram` exactly once;
3. apply the generic CPU result only after the whole copy succeeds; and
4. return to the existing native exception path on any failure.

This uses existing generic ordinary-RAM mechanics. A future Bochs consumer
block must be documented as a single generic bridge intrusion and must contain
no service, selector, filename, handle, error, or DOS term.

## Ownership

| Layer | Owns | Must not own |
| --- | --- | --- |
| Adapter mechanics | record validation, one pending lifecycle, fixed payload arena | file, BOP, DEM, path, handle, error or CPU meaning |
| Adapter host capability | source-derived operation and conversion of its result to the generic record | Bochs memory mapping or CPU object |
| Bochs | complete-span ordinary-RAM validation/copy and generic CPU-result application | DOS, host file, token or payload interpretation |
| Guest OpenNT | source call, register meaning, SFT and DOS flow | host resource lifetime |

## Admission and tests

Implementation starts only after C11 ABI tests cover nonzero maximum payload,
zero-payload result, range/aperture overflow, payload mismatch, stale/mismatched
boundary cancellation, and one-take lifecycle. The Bochs focused fixture must
prove: non-RAM rejection before any copy, a cross-page successful copy,
zero-payload resume, and no partial write on rejected input.

`DEMREAD` dispatch, Open/Seek/Close tokens, configuration materialization and
CLI policy are separate consumers and cannot be bundled into this mechanics
change.
