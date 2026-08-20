# M0 T232 — Unified Host-Handle Manager Plan

## Purpose

Replace the independently allocated DEM and COMMAND host-handle token tables
with one adapter-owned, session-owned resource manager.  It represents a host
`HANDLE` on the guest-facing BOP ABI as an opaque, nonzero `uint16_t` value;
the host pointer value never crosses that boundary.

This is an ABI-safety and lifetime-composition task.  It does not reproduce a
DOS JFN/SFT table, alter an OpenNT provider's service semantics, or cover XMS
allocation handles.

## Fixed Contract

- The manager owns two indexes over one entry set: `HANDLE -> guest_handle`
  and `guest_handle -> HANDLE`.
- `guest_handle` zero is invalid. New values allocate monotonically from one.
  A released value is not reused during a session, preventing a stale guest
  register pair from naming a later host object.
- A publication of an already mapped host `HANDLE` is idempotent.
- Entry ownership is explicit: `owned` entries are closed by release/reset;
  `borrowed` entries are removed but never closed by the manager.
- Guest ABI shims keep their historical register layouts. They marshal the
  16-bit opaque ID in the low word and require the high word to be zero; they
  do not split or reconstruct a real host handle.
- The manager is `bx-vdm` infrastructure. Bochs and bx-mantle receive no
  handle, BOP selector, OpenNT or DOS policy.

## S Sequence

| S | Deliverable | Exit gate |
| --- | --- | --- |
| S1 | Manager ABI, bidirectional indexes, ownership/reset behavior and focused regression | Formal source-built fixture proves allocation, lookup, idempotence, error and ownership cases. |
| S2 | DEM context/session migration | DEM `50:xx` host-file handle publication, lookup and close use the shared manager; private DEM 64-slot table is deleted. |
| S3 | COMMAND session migration | COMMAND `54:06`/redirection and launch lookup use the shared manager; private COMMAND 64-slot table is deleted. |
| S4 | Cross-family closure | Direct DEM and COMMAND fixtures prove one manager contract, formal full graph passes, stale/high-word/cross-session/cleanup regressions pass, and no duplicate private host-handle table remains. |

## Source-Recovery Classification

OpenNT's DEM and COMMAND source assumes an NT4-sized host `HANDLE` can pass
through fixed guest register fields.  That assumption cannot be retained on a
modern x64 host.  The manager is therefore the smallest adapter/shim under
the source-recovery ladder: original provider bodies and register layouts stay
in place; only the nonportable host-handle representation is replaced.

## Deferred Work

XMS/DPMI/Redirector guest-native allocation or descriptor handles, DOS kernel
JFN/SFT semantics, cross-process object duplication, and virtual/overlay
profiles are outside T232.
