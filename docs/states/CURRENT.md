# Project Status

## Current Work

**No active numeric M/T/S packet. Td S2 P1 is closed.**

M0 T401 remains closed. Td S2 P1 has converged the documentation tree to the
strict topology; no product T was closed or changed. Pending product work
remains preserved locally on `codex/product-wip-preserved-20260910`.

## Latest Td Closure

[Td S2 P1 strict documentation-topology convergence](../history/m0-td-consolidated-history.md) archived 1,001 non-authority files with hashes, retained five current design authorities, consolidated and task-introduction-named history records, and reduced `etc` to explicitly owned supporting material.

## Latest Product Closure

[M0 T401 WRITE diagnostic reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t401-s4-write-compatible-dc-non-equivalence-001.md).

## Latest Governance Closure

[M0 Td status governance compaction](../../artifacts/documentation-archive/20260910/strict-topology/history/raw/m0-td-status-governance-compaction-20260910.md).

## Current Technical Baseline

- The sole product and acceptance executable is the Win32/x86
  `ntvdm32.exe`, using the original SoftPC CCPU40 profile. Native x64 is not a
  product, build, runtime, or acceptance target.
- M0 T395 (CPU40 DPMI32/bootstrap), M0 T396 (WOW32 first-dispatch frontier),
  M0 T397 (first WOW16 UI/service boundary), M0 T400 (WRITE observation
  boundary), and M0 T401 are closed with the limits recorded in their linked
  evidence and history.
- Candidate packages are authoritative only in [QUEUE.md](QUEUE.md). No
  candidate is a numeric task or an active packet until admitted here.

## Historical Status Ledger

The complete pre-compaction status ledger is retained unchanged in
[history/status-ledger-snapshot-20260910.md](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md).
Use it only for historical lookup; it does not define current work.

## Status Hygiene

This page contains only the current packet state, compact baseline, and links
to retained records. Detailed run chronology belongs in `docs/etc/evidence/`;
closed-task facts belong in `docs/history/`.
