# M0 T272 S3 mapping-manager reuse audit

## Purpose

This record fixes the implementation boundary for the rebootstrap
compatibility object space.  The resulting component has one generic manager
implementation and three independent instances owned by each `session`:
guest-memory, host-resource and completion/callback identity.

## Reuse decision

- `src.old/adapter-softpc/guest_pointer_manager.{c,h}` is reference-only.
  Its process-global registry, active-machine state and linear/real-mode
  acquisition are machine-facing semantics.  S3 cannot import them without
  violating the neutral session boundary.
- `src.old/adapter-softpc/host_handle_manager.{c,h}` is reference-only.  It
  aliases the same global registry and adds `HANDLE` ownership/release rules.
  S3 must neither expose `HANDLE` nor close a resource.
- The new `src/session/mapping_manager.{c,h}` is therefore a deliberately
  small, project-owned neutral implementation.  It is not a fourth special
  mapping family: it is the sole common implementation used by the three
  distinct session fields.

## Contract

- `uint32_t` identifiers are opaque surrogate values.  The manager never
  identity-passes a native pointer, handle or guest address into an imported
  ABI.
- Each instance has separate forward and reverse indexes.  Equal native
  values in different instances may receive equal numeric identifiers without
  colliding because the instance itself is the namespace.
- Allocation is monotonic for the lifetime of an instance.  Released records
  remain as identifier tombstones; their numeric identifier is never reused.
- The caller selects reserved ABI sentinel values.  The session's three
  instances reserve zero and `UINT32_MAX`, while the generic implementation
  remains usable by a future ABI that admits zero.
- `guest_memory` is identity only in S3.  A bounded memory lease belongs to
  the later adapter-softpc seam, not to this neutral object manager.

## Verification

`tests/session/mapping_manager_test.c` verifies x64 and x86 builds for
bidirectional publication, per-instance isolation, idempotent publication,
tombstoning/no reuse, zero-sentinel policy, exhaustion and session teardown.
The existing lifecycle fixture is relinked against the manager to ensure that
the S2 lifecycle remains valid.

## Non-goals

This S packet neither recovers a SoftPC/CCPU interface nor defines a BOP,
broker IPC record, guest-memory lease, host resource ownership policy or
callback invocation policy.
