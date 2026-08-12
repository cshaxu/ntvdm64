# Adapter Host Transaction ABI v1

## Shared contract

`bx_ntvdm_host_transaction_v1` is the common preflight record for every
adapter host-service implementation. It combines:

- one validated catalogue result (`family`, selector, service);
- the immutable copied CPU state at entry; and
- one pointer-free proposed physical guest write plus payload offset.

The record is 136 bytes on the declared C11 ABI. It contains no host path,
file handle, payload pointer, Bochs object, raw guest mapping, or CPU pointer.

## Preconditions

Preflight accepts only a known DEM or COMMAND range, a valid CPU snapshot, and
a complete guest-write descriptor whose guest and payload ranges both fit their
declared extents. Every reserved/flags field must be zero.

Preflight performs no write, state commit, IP advance, file operation or
service dispatch. The same generic record will be used by loader, file,
directory, console and lifecycle services, so a future handler cannot invent
its own partial-memory mutation protocol.

## Commit rule

An eventual same-island Bochs commit seam may execute only after this preflight
and its service-specific resource validation both succeed. It must validate
ordinary-RAM applicability, publish the full write, then apply an explicit
CPU result; a failed precondition leaves both guest memory and CPU state
unchanged. That mutation seam is intentionally not yet implemented.

## Verification

The UCRT C11 fixture verifies successful DEM load-shaped preflight and rejects
an out-of-range service, payload-range failure, invalid CPU state, and null
transaction.
