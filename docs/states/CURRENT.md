# Project Status

## Current Work

**Active: M0 T405 S2**

## Active Packet

### M0 T405 S2 — MVDM diff, overlay and patch audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T405 S2, Ordinary Mode with one agent acting sequentially as executor and reviewer. Only S2 is active. |
| Admission And Approval | Owner on 2026-09-12 admitted the audit-and-restoration T and subsequently required owner approval of audit findings before repairs. Audit proceeds now; restoration remains in this T but is gated on that approval. |
| Candidate Proposal | [OpenNT source-diff audit and restoration](../proposals/proposal-opennt-source-diff-audit-001.md). |
| Objective | Reconcile every MVDM internal diff, overlay and adopted patch with its original owner, current selection, changed semantics and proposed disposition; include working COMMAND/EDIT paths and inactive carriers. |
| Non-goals | No product behavior, build-selection or guest-media change; no source restoration, broker implementation or WRITE recovery claim. S3 will consolidate non-MVDM findings, then stop for owner discussion before repairs. |
| Reference Baseline | Repository `3d127962c82133cb6adad50f52733250bac5f11a`, including the explicitly unaccepted snapshot `1daff0ace`; [T404 closure](../history/m0-t404-owner-directed-closure.md). Runtime identity must be observed separately, not inferred from HEAD. |
| Files And ABI Surface | Status, indexed [S-stage plan](../etc/operations/m0-t405-audit-restoration-plan.md), [S1 conclusion](../etc/evidence/m0-t405-s1-audit-baseline-001.md#s1-bounded-conclusion-and-reviewer-handoff), [semantic audit](../etc/evidence/opennt-replacement-audit-20260912.md), provenance/build ledgers and bounded audit tooling. Source and external comparisons are read-only; no production ABI change. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [documents](../rules/DOCUMENT.md), and [source policy](../etc/operations/policy/source-policy.md). |
| Verification | Git identity/status; selected upstream provenance and hashes; manifest-to-build coverage checks; separate source/overlay/replacement counts; documentation governance, relative links and diff checks. This static S has no machine-execution acceptance row. |
| Expected Markers | Per-owner coverage and original paths, explicit duplicate/binding/diagnostic/inactive distinctions, missing interfaces, semantic risks and affected regression requirements. |
| Asset Needs | Existing selected OpenNT/OpenNT-4.5 sources, retained mirrors, ledgers, build graphs and runtime hashes; external inputs are read-only. No new media or runtime dependency. |
| Reporting Requirements | Separate MVDM/non-MVDM and accepted/WIP identities; report functional units, files and physical added/deleted lines separately; document unknown provenance and coverage rather than estimating a hack total. |
| Stop Conditions | Ambiguous provenance is recorded as unresolved and independent audit continues. A destructive baseline change, new external source/runtime, or expanded architecture boundary needs owner direction; missing evidence never authorizes a speculative repair. |
| Exit Criteria | Every in-scope MVDM diff/overlay/patch family has reviewed coverage and disposition, with original-owner and limitation evidence ready to join S3. No repair before owner approval of the combined audit. |
| Original Owner Request | “准入该T任务，开始进行审计。先作任务切分：S”; the task includes audit followed by implementation. |
| Similar-Issue Sweep | Mirror-internal special cases, overlays, adapters, app/session replacements, SoftPC patches, declarations, diagnostics and inactive providers, including successful COMMAND/EDIT paths. |

## S1 Closure Record

T405 S1 reached its bounded inventory conclusion in the
[reviewed baseline evidence](../etc/evidence/m0-t405-s1-audit-baseline-001.md):
5043 tracked inputs, separate source/body counts, resolved unpaired identities
and reproducible retained-graph membership. Its documentation delivery remains
pending; prior push was unavailable, and no fully delivered P is claimed.
S2 continues within the already admitted T, without broadening repair authority.

## Predecessor

T404/S5 ended by owner direction, not complete WRITE acceptance. Its source
snapshot is audit input, not a validated product baseline.

## T404 S3 Closure Record

S3 is closed as the native Console mouse-usability packet. Its delivered acceptance is the owner-verified [P12 mouse record](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md): host mouse movement and button transitions reach the original guest mouse path without the prior false-click/help-modal behavior. P13--P15 are retained startup-chain investigations and entry recovery evidence, not part of S3's mouse closure. They establish S4's baseline only; they do not claim KRNL386, DOSX, WOW32 or WRITE execution.

## T404 S4 Closure Record

S4 is closed as the bounded positional-bootstrap recovery: the sole public
positional command reaches DOSX and opens KRNL386, while same-version WOW32
provider composition transfers to S5. Its former characterization of DEM
`50:01` as the WOW BOP is withdrawn by the [S5 ingress correction](../etc/evidence/m0-t404-s5-dem-chmod-ingress-correction-001.md); it does not claim a
WOW32 load or a runnable WRITE window.

## T404 S1 Closure Record

[T404 S1 WOW hard-error recovery evidence](../etc/evidence/m0-t404-s1-wow-hard-error-recovery-001.md).

## T404 S2 Closure Record

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
