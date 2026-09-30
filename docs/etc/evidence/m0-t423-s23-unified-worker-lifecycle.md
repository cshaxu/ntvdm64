# M0 T423 S23 — Unified Worker Lifecycle Authority

## Result

NTSRV is now the sole lifecycle authority for an admitted NTCON worker, just
as it already is for an NTVDM worker.  A native worker must first claim the
prepared native reservation before it can register its backend.  Management
snapshots are derived only from authenticated worker watches; a stale native
route can no longer create a second, bare row.

Frontend-root loss revokes only copied presentation handles.  NTCON closes its
presentation binding and remains registered READY/EMPTY, so a later compatible
authenticated root can bind it again.  Conversely, NTSRV explicit worker close
now uses the authenticated worker watch and native stop/closed contract even
after the old root is gone; it no longer incorrectly relies on `native_root`.

## Verification

- `basesrv-service-reservation-test --native-worker`: pass.  Covers prepared
  registration, route loss, resident rebind, worker-only channel rejection,
  and explicit NTSRV close after root rundown.
- `basesrv-service-reservation-test --native-backend`: pass.  An unreserved
  process cannot register a native backend and produces no snapshot row.
- `frontend-scope-lifetime-test`: pass.
- `monitor-rpc-test --empty`: pass, including management ABI version rejection
  and empty snapshot behavior.
- x86 objects, protocol-19 MIDL stubs, NTSRV and NTCON were rebuilt from the
  formal graph under `build/M0-T423/S23/formal`.  The cache was seeded only for
  unchanged objects; all protocol-19 RPC objects and S23-modified objects were
  regenerated before link.

## Boundaries

No guest, shared library, original DOS/WOW record ownership, scheduler,
process-tree termination, helper, or polling policy changed.  PID-accurate
native participant records and management projection remain S24/S26 work.
