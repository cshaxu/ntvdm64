# M0 T343 — Broker process and cross-process coordination closure

## Closed result

T343 recovers the selected original DOS BaseVDM copied-record boundary without
pretending that the NT4 BaseSrv/CSRSS product shell exists in the modern
process.

- The original BaseClient/BaseSrv record, wait/retry and source-consume facts
  are recorded in the [S1 boundary evidence](../etc/evidence/m0-t343-s1-broker-process-boundary-001.md)
  and [S2 record ledger](../etc/operations/m0-t343-s2-basesrv-record-message-contract-ledger.tsv).
- `broker` now owns a fixed-width copied DOS record with deterministic
  pending/second-empty-call, peek/copy/consume and close-once behavior.
- The app's declared DOS command goes through the only permitted
  adapter-mvdm-host-out BaseVDM bridge before the original public
  `GetNextVDMCommand` caller sees its local copied result.
- Both x86 and x64 focused record/bridge tests and formal CPU40 candidate
  graphs pass. The selected S3 formal result is recorded in
  [its closure evidence](../etc/evidence/m0-t343-s3-p3-copied-record-formal-closure-001.md).

## Explicit transfers

- CSRSS/BaseSrv server process records, CSR transport and kernel VDM are not
  reimplemented.
- Named-pipe/event transport, cross-process authentication and cross-user
  coordination are not falsely claimed by the current in-process binding.
- PIF, separate/shared WOW, parent/child lifecycle, batch records and raw
  handle/pointer transfer retain their original owner packages.
- The x86 full-archive `nt_sec.c` virtual-memory imports remain a SoftPC
  display/regen-memory recovery item, not a broker or BaseVDM change.

The next queue item may audit in-process multi-session reentrancy, but it must
start from the original global/thread-state inventory rather than enabling a
second session by assumption.
