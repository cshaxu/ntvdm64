# Project Status

## Current Work

**Active: M0 T404 S1**

## Active Packet

### M0 T404 S1 P1 — single-process WOW16 lifecycle recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S1 P1, Ordinary Mode. |
| Admission And Approval | Owner directed resumption of admission with Queue candidate 1, “Single-process WOW16 application lifecycle recovery.” |
| Candidate Proposal | [Single-process WOW16 application lifecycle recovery](../proposals/proposal-wow16-single-process-lifecycle-recovery-001.md) |
| Objective | Recover or precisely bound the immediate original owner that prevents the selected `WRITE.EXE` workload from starting, running, and returning in one Win32/x86 MVDM worker. |
| Non-goals | Broker/BaseSrv work, multi-process reuse, a private USER/CSRSS shell, replacement guest/UI failure policy, CPU tracing policy, x64 build work, and release-matrix claims. |
| Reference Baseline | Candidate-1 proposal; closed T396 first WOW32 dispatch frontier, T397 UI/service boundary, T400/T401 WRITE observations, and T403 x86 baseline. |
| Files And ABI Surface | Initial audit may update only current packet and indexed evidence. Any later source change must name its original MVDM/WOW owner, exact public or same-shaped ABI, and binding path before implementation. |
| Applicable Rules | `docs/design/GOAL.md`, architecture/coding rules, source policy, execution rules, and the candidate-1 proposal. |
| Verification | Audit the selected original start/return path and present runtime boundary; run focused x86 build/runtime checks only after their source and run root are recorded; run diff and governance checks at delivery. |
| Expected Markers | A source-owned low-memory dialog/start/return chain, or an exact unavailable original boundary and receiver; no broker or private USER/CSRSS substitute. |
| Asset Needs | Existing canonical OpenNT/WOW16 mirrors, selected `WRITE.EXE` workload, and current x86 runtime package; no new source or guest-media acquisition. |
| Reporting Requirements | Record source provenance, four-rung disposition, reached ABI/failure contract, focused evidence, limitations, and next owner boundary. |
| Stop Conditions | A required dependency outside the selected immediate owner, any need for broker/CSRSS/USER replacement, an unrecorded guest asset, or an architecture/boundary expansion pauses work for renewed admission. |
| Exit Criteria | One bounded Win16 workload has original-path start/return evidence, or the exact source-proven unavailable boundary and its receiver are recorded; every changed source path and runtime result is reviewed. |
| Original Owner Request | “好了好了。接下来我们恢复准入做WOW16吧。” |
| Similar-Issue Sweep | Inspect adjacent WOW32/WOWEXEC/low-memory-dialog and return-path ownership, but do not broaden beyond the first immediate owner. |

## Latest Packet Closure

[M0 T403 x64 mirror and mapping retirement](../history/m0-t403-x64-mirror-retirement.md)
removed the mapping manager, identity/token paths, and active x64-only
overlays from the sole Win32/x86 product. Formal x86 link, I386 staging,
focused Redirector validation, governance checks, and owner runtime testing
passed.

## Latest Product Closure

[M0 T401 WRITE diagnostic reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t401-s4-write-compatible-dc-non-equivalence-001.md).

## Latest Governance Closure

M0 Td S5 P1 clarified the unadmitted cross-process-broker proposal: `run16`
is the standalone BaseClient/CLI entry, `basesrv` owns broker selection and
cleanup, and `ntvdm` returns to the worker role.  It records a one-minute
idle-worker policy as a future product policy, not as an OpenNT fact.  No
product source, runtime, ABI, Queue order, or candidate admission changed.

M0 Td S2 P1 strengthened the governance gate with fixed-topology, compact
state, supporting-index, encoding, relative-link and anchor checks; it also
reconciled the public README, product UX, closed-evidence lifecycle and a
historical encoding defect. NXVM governance materials were read-only quality
references; no product, ABI, build, runtime or queue behavior changed.

M0 Td S3 P1 added governance self-test and CI execution, explicit one-candidate
one-proposal Queue rows, and mechanical Queue/TODO schema checks. No product,
ABI, build, runtime or queue ordering changed.

## Recent M0 Closures

| Task | Compact result |
| --- | --- |
| T403 | x64 mapping and overlay residue retired; formal x86 build and owner test passed. |

## Recent Governance

- **M0 Td S5 P1:** clarified the candidate-2 standalone broker split and proposed cleanup policy; no product behavior changed.
- **M0 Td S2 P1:** fixed topology, links, index, encoding, and compact status controls.
- **M0 Td S3 P1:** CI, self-test, and one-candidate/one-proposal queue controls.
- **M0 Td S3 P2:** identifier, authority schema, path allowlist, and queue/TODO checks.

## Current Technical Baseline

- The sole product and acceptance executable is the Win32/x86
  `ntvdm32.exe`, using the original SoftPC CCPU40 profile. Native x64 is not a
  product, build, runtime, or acceptance target.
- M0 T395 (CPU40 DPMI32/bootstrap), M0 T396 (WOW32 first-dispatch frontier),
  M0 T397 (first WOW16 UI/service boundary), M0 T400 (WRITE observation
  boundary), M0 T401, M0 T402, and M0 T403 are closed with their linked
  evidence and history.
- Candidate packages are authoritative only in [QUEUE.md](QUEUE.md). No
  candidate is a numeric task or active packet until owner admission here.

## Historical Status Ledger

The complete pre-compaction status ledger is retained unchanged in
[history/status-ledger-snapshot-20260910.md](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md).
Use it only for historical lookup; it does not define current work.

## Status Hygiene

This page contains only current packet state, compact baseline, and links to
closed records. Detailed run chronology belongs in `docs/etc/evidence/`;
closed-task facts belong in `docs/history/`.
