# M0 T343 S2 — Original BaseSrv copied-record contract

## Result

The original BaseSrv source is not a single command dispatcher.  Its reachable
MVDM contract separates DOS-console task records, shared-WOW task records,
PIF/environment/normal payload forms, parent completion, re-entry accounting,
directory transfer, batch state and process-disconnect cleanup.  The selected
current local binding is valid only for its documented one-session subset.

The next implementation cohort is fixed accordingly: a **BaseVDM copied-record
broker operation** owned by `broker` and exposed to original callers only
through `adapter-mvdm-host-out/basesrv`.  It extends the existing neutral
versioned broker contract rather than creating COMMAND- or BOP-specific IPC.
Its payload consists exclusively of broker/session/task IDs, request state,
lengths, result/exit codes and copied byte arrays.  It may not serialize a
CSR capture buffer, `VDMINFO` pointer, guest pointer, native `HANDLE`, local
mapping-manager ID, process address or callback.

## Original evidence

- `src/opennt-host/base/win32/client/vdm.c::GetNextVDMCommand` establishes the
  caller-capture, returned-wait and retry-with-`ASKING_FOR_SECOND_TIME`
  ordering.
- `src/opennt-host/base/win32/server/srvvdm.h` defines the DOS, console, WOW,
  WOW-head and batch records.  The records identify the original state and
  ownership boundaries; their pointer and handle fields are not a portable
  wire format.
- `src/opennt-host/base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand`
  distinguishes DOS wait/retry from shared-WOW nonblocking polling, preserves
  PIF and environment as separate transfer forms, validates all normal payload
  capacities before consuming the source VDMINFO, and transfers standard
  handle ownership only inside the NT4 server/product boundary.
- `BaseSrvExitVDM`, `BaseSrvGetVDMExitCode`, `BaseSrvSetReenterCount`,
  `BaseSrvSetVDMCurDirs`, `BaseSrvGetVDMCurDirs`, `BaseSrvBatNotification`,
  `BaseSrvRegisterWowExec` and `BaseSrvCleanupVDMResources` prove the separate
  lifetime and disconnect responsibilities recorded in the S2 ledger.

## Current-boundary conclusion

`base_vdm_client.c` plus `base_vdm_local.c` preserve the original caller-copy,
DOS wake/retry, empty-WOW, environment sizing, normal capacity/copy/consume,
re-entry, current-directory and one-session controlled-stop rules.  They do
not implement server process registration, a DOS task list, parent completion
records, PIF/separate-WOW, shared-WOW registration/notification, batch
records, handle duplication or CSRSS disconnect cleanup.

`src/broker` already supplies a versioned fixed-width identity/registration
fixture.  It is not yet a BaseVDM broker: its three generic operations do not
carry source-shaped command payloads, wait/result state or task ownership.
It remains the correct project owner for a later, separately admitted
fixed-width BaseVDM operation because it prevents raw process identity and
resource values from crossing the boundary.

## Explicit transfers

- Original CSRSS transport, server-global process records, token/desktop
  checks and `NtDuplicateObject` client-handle transfer remain hard boundaries.
- COMMAND owns batch and DOS child/parent lifecycle; a BaseVDM broker may
  transport its source-defined record state but does not invent those
  semantics.
- WOW owns shared-WOW registration, task and callback semantics; a broker may
  later provide a copied registration/notification boundary only after its
  owner package is admitted.
- PIF/separate-WOW selection is not silently represented as an empty command;
  it remains unavailable until its full original owner path is selected.

## S2 exit result

Every selected record and message form has a current disposition or an explicit
owner transfer in
[`m0-t343-s2-basesrv-record-message-contract-ledger.tsv`](../operations/m0-t343-s2-basesrv-record-message-contract-ledger.tsv).
No new transport or runtime behavior is enabled by this audit.
