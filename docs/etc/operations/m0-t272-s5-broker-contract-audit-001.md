# M0 T272 S5 broker contract audit

## Source decision

No project-local neutral broker source exists in `src.old/`; retained named
pipe and asynchronous Redirector code is provider-specific and cannot become
a general MVDM-session broker. OpenNT's CSR/BaseSrv coordination is a
historical product subsystem, not a reusable public client API. S5 therefore
defines only the smallest project-owned contract that later public IPC can
transport.

## Version-one wire contract

- `broker_wire_message` is exactly 48 bytes under both MSVC x64 and Win32/x86.
  It consists exclusively of `uint32_t` fields and a fixed 16-byte user key.
- Magic, version and exact byte count are mandatory. The operation set is
  registration, disconnect and notification; no service/provider payload is
  admitted.
- No field accepts a pointer, `HANDLE`, function pointer, CRT object, guest
  pointer or process-local mapping surrogate. Native values have no wire
  representation.
- The key is an authenticated transport input, not a caller-selected security
  assertion. A future named-pipe/RPC transport must derive it from public OS
  authentication and reject a claimed mismatch before calling `broker_dispatch`.

## Registry behavior

The bounded registry assigns monotonic nonzero broker IDs, records the fixed
user key and rejects operations from a different key. Disconnect clears the
record; a subsequent operation receives `UNKNOWN`. This proves cleanup and
per-user isolation without pretending an in-process test registry is IPC.

## Verification

`tests/broker/wire_test.c` passes as independent MSVC `/MT` x64 and Win32/x86
builds in `build/M0-T272-S5/r001/`. It proves wire size, version validation,
cross-user denial and disconnect cleanup. The S2 lifecycle, S3 mapping and
S4 lease focused fixtures already passed on both architectures and remain
independent of broker code.

## Deferred work

Public named-pipe/RPC transport, OS-token authentication, command queues,
notifications and broker process startup belong to the later historical
interface adapter and product-composition packages. They must preserve this
wire boundary and cannot introduce a raw local identity.
