# M0 T343 S1 — Broker/process boundary result

## Result

The current local Base VDM client is a correctly bounded **single-process**
recovery of the reached client call shape. It is not, and must not be called,
a recovery of the original cross-process BaseSrv broker.

## Evidence

- `base_vdm_client.c` copies caller `VDMINFO` into a local capture record,
  preserves the BaseClient retry-after-wait order, and never retains a caller
  pointer across the wait.
- `base_vdm_local.h` stores a session owner, critical section, wake event and
  copied command fields. Its bind operation permits one current record on the
  bound thread; it has no IPC endpoint, process registry or serialized native
  identity.
- Original `opennt-host/base/win32/server/srvvdm.c` owns the larger BaseSrv
  console/DOS/WOW record and CSR server lifecycle. That depends on private
  CSRSS transport and remains non-composable as a user-mode runtime server.

## S1 selection

The next T343 cohort is therefore a **bounded copied-record broker design**:
retain original client/server record ordering and failure/disconnect contracts,
but use only public process/event/pipe mechanisms behind a dedicated session
and broker boundary. It must not be an extension of COMMAND or BOP dispatch.
Before implementation, S2 must inventory the exact original record fields,
message cases, identity lifetime and disconnect semantics it needs.
