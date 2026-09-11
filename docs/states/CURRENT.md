# Project Status

## Current Work

**Active: M0 T404 S3**

## Active Packet

### M0 T404 S3 P13 — CPU40 BOP entry-contract audit (delivered)

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S3 P13, Ordinary Mode, delivered read-only audit. P12's accepted CPU40 direct INT33 callback repair is the reference case. |
| Admission And Approval | Owner directed on 2026-09-11: “对，你来审计。” This follows the P12 discussion of whether BOP entry bytes should be recovered or the stale assumption removed. |
| Candidate Proposal | [CPU40 BOP entry-contract audit](../proposals/proposal-cpu40-bop-entry-contract-audit-001.md); delivered [P13 evidence](../etc/evidence/m0-t404-s3-p13-cpu40-bop-entry-contract-audit-001.md); preceding delivery: [P12 audit](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md). |
| Reference Baseline | Selected x86 CPU40 source and composition, including `HOST_BOP_IP_FUDGE = -2`; P12 proves one guest-registered mouse callback is direct guest code rather than a BOP entry. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [document](../rules/DOCUMENT.md), and the [source policy](../etc/operations/policy/source-policy.md). |
| Objective | Classify every selected production `HOST_BOP_IP_FUDGE` route as a demonstrated BOP/trampoline, a direct guest entry, or unresolved; propose no code change. |
| Non-goals | Changing CCPU decoding/execution, `HOST_BOP_IP_FUDGE`, CPU30, any BOP route, guest media, callbacks, coordinate semantics, Console modes, or runtime package behavior. |
| Files And ABI Surface | Read-only selected CCPU40/SoftPC source and build manifests; new proposal and indexed audit evidence only. |
| Verification | Exhaustive text/call-site inventory; original-source and CPU40 entry-shape review for every occurrence; diff and documentation governance checks. |
| Expected Markers | Every occurrence has a source owner, operand provenance, entry classification, evidence, and retain/direct/unresolved disposition. |
| Asset Needs | Existing selected source, build manifests, and P12 logs only; no new source, guest binary, firmware, or Microsoft component. |
| Reporting Requirements | Record exact inventory, proof level, direct-entry versus host-stub distinction, and explicitly deferred routes in indexed evidence. |
| Similar-Issue Sweep | Search definitions, additions/subtractions, address setters, BOP stub constructors, and CPU40 fetch entry only; do not widen to unrelated BOP services. |
| Stop Conditions | Any proposed behavioral change, runtime instrumentation, guest-memory modification, CPU30 activation, or CCPU/BOP decoder change requires renewed admission. |
| Exit Criteria | Met: the indexed audit covers every selected production occurrence and gives a justified per-route disposition; no product code changed. |
| Original Owner Request | “对，你来审计。” |

P12's [button-state, callback-entry, and acceptance record](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md) is delivered. P13 establishes that neither remaining CPU40 `reset.c` consumer targets a BOP: both target DOSX `enrm` continuations. A separate admission is required to make those two CPU40 entries direct and test DOSX/WOW; P13 is read-only and does not reopen P12 behavior.

## S1 Closure Record

[T404 S1 WOW hard-error recovery evidence](../etc/evidence/m0-t404-s1-wow-hard-error-recovery-001.md).

## S2 Closure Record

[T404 S2 DOSX BOP FD contract audit](../etc/evidence/m0-t404-s2-dosx-bop-fd-contract-audit-001.md).

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
