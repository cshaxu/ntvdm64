# M84 Redacted Trace Envelope

Status: completed fixture-level diagnostic contract, 2026-08-08.

## Purpose

This record implements the remaining Track A trace-method item without
creating an engine, a BOP/SVC dispatcher, guest-memory access, or a historical
runtime link. `src/redacted_trace.{h,c}` is a modern, copied-value diagnostic
envelope for a future profile adapter or fixture. It is not linked into
`ntdos64-run`, an OpenNT source island, an NTVDMx64 source island, or a default
historical trace target.

## Stored Form

Each of at most 64 ordered records contains only:

- monotonic local sequence number;
- event, subject, and finite outcome enums;
- caller-defined finite detail and route-class codes;
- byte count; and
- an eight-byte identity prefix.

The event structure has no path, string, command text, guest address, host
handle/token, raw pointer, register, timestamp, or guest-byte field. The
identity prefix is copied by value and is suitable only for the already
permitted short hash/role correlation. A future adapter must map its richer
private information to these finite values before recording an event.

This realizes the common trace shape in
`LANE-P-TRACE-FIXTURE-PLAN.md` and the resource-diagnostic rule in
`CONTAINED-DOS-NAMESPACE-DOSSIER.md`: role/identity, length, route/result, and
terminal category are observable; protected bytes and ambient host details are
not.

## Integrity Rules

`ntdos64_redacted_trace_record` validates all enums before changing state,
copies the full input event, assigns its own sequence number, and rejects a
full buffer without mutation. Reset clears all retained diagnostic values.
The public `at` accessor returns no record outside the retained range.

The type does not by itself grant runtime authority. In particular, a record
with a protocol or terminal category is evidence only of a fixture's declared
claim; it is not evidence that the historical monitor, DEM, BOP service, or
guest has run.

## Verification

`redacted-trace-test` verifies copied input/sequence assignment, invalid-input
non-mutation, bounded-full non-mutation, out-of-range rejection, and reset.
It is `EXCLUDE_FROM_ALL` and must be named explicitly when built. It has no
dependency on guest artifacts, source islands, or frozen instrumentation.

## Boundary Result

M84 closes the passive trace-format preparation only. It does not change the
M70/M79 backend admission gate: configured decode observation and a paused
ordinary-RAM multi-span transaction remain external core requirements before a
historical-image prefix fixture or executable profile can begin.
