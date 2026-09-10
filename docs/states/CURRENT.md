# Project Status

## Current Work

**Active: Td S1 P1 — documentation repository declutter.**

M0 T401 is closed as a bounded single-process WRITE diagnostic
reconciliation. Its observations rule out only the named boundaries; they do
not identify a WRITE repair or authorize a lifecycle recovery. A next package
requires fresh owner approval and admission here before implementation begins.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | Td S1 P1, ordinary documentation-governance mode; no numeric T is allocated. |
| Admission And Approval | Owner approved: "没错，批准。请你继续Td治理提交推送。" |
| Objective | Move generated ledgers and raw historical material out of `docs`, leaving only current authorities, proposals, compact states, and one task record per task. |
| Non-goals | No product source, ABI, build, runtime, guest-media, or behavioral change. Existing unrelated working-tree changes are excluded from this delivery. |
| Reference Baseline | 4,103 `docs` files: 315 operations TSV files consume about 299.6 MiB; `etc/evidence`, `etc/research`, and fragmented history contain the remaining historical material. |
| Files And ABI Surface | `docs`, `artifacts` archival manifests, and documentation/governance tools only; no product ABI surface. |
| Applicable Rules | `rules/EXECUTION.md`, `rules/DOCUMENT.md`, source policy, and the approved documentation-governance skill. |
| Verification | File/hash manifests; direct-reference scan; documentation link and governance verification; focused exporter regression; diff review; commit and push. |
| Expected Markers | No raw ledger under `docs`; current `docs` topology contains only allowed authority/state/proposal/history records; external archive has a manifest. |
| Asset Needs | Existing repository files only; external OpenNT remains read-only comparison material. |
| Reporting Requirements | Record moved scope, hashes, retained summaries, limitations, commit, and push result in the Td history record. |
| Stop Conditions | A path is a live build/runtime input, a required artifact has no safe archival manifest, or unrelated user changes would be staged. |
| Exit Criteria | Approved material relocation is verified; active references resolve; required checks pass; only Td-owned changes are committed and pushed. |
| Original Owner Request | "现在doc怎么还有这么多文件？" "你觉得哪些可以清理出去？这docs不应该这么庞杂" "没错，批准。请你继续Td治理提交推送。" |
| Similar-Issue Sweep | Operations, evidence, research, archive, design, history, tooling paths, and untracked working-tree boundaries are included. |

## Latest Product Closure

[M0 T401 WRITE diagnostic reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t401-s4-write-compatible-dc-non-equivalence-001.md).

## Latest Governance Closure

[M0 Td status governance compaction](../history/m0-td-status-governance-compaction-20260910.md).

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
