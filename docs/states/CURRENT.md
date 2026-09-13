# Project Status

## Current Work

**Active: M0 T405 S3**

S3 static-audit findings are ready for owner discussion in the
[combined audit conclusion](../etc/evidence/opennt-replacement-audit-20260912.md).
The 37 confirmed overlap rows and eight unresolved families are separate.
Audit-source implementation is paused at the owner's requested discussion
gate; no S4/S5 repair is admitted. T405 remains open. Delivery is pending.

## Active Packet

### M0 T405 S3 — Non-MVDM owner and replacement audit consolidation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T405 S3, Ordinary Mode with one agent acting sequentially as executor and reviewer. Only S3 is active. |
| Admission And Approval | Owner on 2026-09-12 admitted the audit-and-restoration T and subsequently required owner approval of audit findings before repairs. Audit proceeds now; restoration remains in this T but is gated on that approval. |
| Candidate Proposal | [OpenNT source-diff audit and restoration](../proposals/proposal-opennt-source-diff-audit-001.md). |
| Objective | Reconcile non-MVDM original owners and autonomous providers wherever located, joining S2 without double counting. Produce the combined diff/overlay/replacement discussion ledger, including COMMAND/EDIT paths and inactive carriers. |
| Non-goals | No product behavior, build-selection or guest-media change; no source restoration, broker implementation or WRITE recovery claim. Stop after the combined audit for owner discussion before repairs. |
| Reference Baseline | Repository `3d127962c82133cb6adad50f52733250bac5f11a`, including the explicitly unaccepted snapshot `1daff0ace`; [T404 closure](../history/m0-t404-owner-directed-closure.md). Runtime identity must be observed separately, not inferred from HEAD. |
| Files And ABI Surface | Status, indexed [S-stage plan](../etc/operations/m0-t405-audit-restoration-plan.md), [S1 conclusion](../etc/evidence/m0-t405-s1-audit-baseline-001.md#s1-bounded-conclusion-and-reviewer-handoff), [semantic audit](../etc/evidence/opennt-replacement-audit-20260912.md), provenance/build ledgers and bounded audit tooling. Source and external comparisons are read-only; no production ABI change. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [documents](../rules/DOCUMENT.md), and [source policy](../etc/operations/policy/source-policy.md). |
| Verification | Git identity/status; selected upstream provenance and hashes; manifest-to-build coverage checks; separate source/overlay/replacement counts; documentation governance, relative links and diff checks. This static S has no machine-execution acceptance row. |
| Expected Markers | Per-owner coverage and original paths, explicit duplicate/binding/diagnostic/inactive distinctions, missing interfaces, semantic risks and affected regression requirements. |
| Asset Needs | Existing selected OpenNT/OpenNT-4.5 sources, retained mirrors, ledgers, build graphs and runtime hashes; external inputs are read-only. No new media or runtime dependency. |
| Reporting Requirements | Separate MVDM/non-MVDM and accepted/WIP identities; report functional units, files and physical added/deleted lines separately; document unknown provenance and coverage rather than estimating a hack total. |
| Stop Conditions | Ambiguous provenance is recorded as unresolved and independent audit continues. A destructive baseline change, new external source/runtime, or expanded architecture boundary needs owner direction; missing evidence never authorizes a speculative repair. |
| Exit Criteria | Non-MVDM owners and replacement families have original-source, selection and disposition evidence joined to S2; confirmed overlap, necessary bindings, diagnostics and unresolved cases are separately reported. Stop for owner approval before repairs. |
| Original Owner Request | “准入该T任务，开始进行审计。先作任务切分：S”; the task includes audit followed by implementation. |
| Similar-Issue Sweep | Mirror-internal special cases, overlays, adapters, app/session replacements, SoftPC patches, declarations, diagnostics and inactive providers, including successful COMMAND/EDIT paths. |

## S2 Closure Record

S2 reached its [bounded MVDM coverage conclusion](../etc/evidence/opennt-replacement-audit-20260912.md)
in the S2 conclusion and S3 handoff section:
174 text-different paths, seven overlay carriers and five patch carriers have
reviewed dispositions. Source hashes and retained graph membership were
revalidated; unresolved semantics transfer explicitly to S3, not to repairs.
Documentation delivery remains pending. The complete audit is not yet closed.

## S1 Closure Record

T405 S1 reached its bounded inventory conclusion in the
[reviewed baseline evidence](../etc/evidence/m0-t405-s1-audit-baseline-001.md):
5043 tracked inputs, separate source/body counts, resolved unpaired identities
and reproducible retained-graph membership. Its documentation delivery remains
pending; prior push was unavailable, and no fully delivered P is claimed.
S3 continues within the already admitted T, without broadening repair authority.

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

M0 Td S10 P1, Ordinary Mode, is authorized by the owner's request:
“批准，请你Td治理队列任务和proposal们，提交并推送”.
Objective: assign U03-U08 to existing candidate owners, with bounded subitems
and acceptance; preserve candidate order and one proposal per candidate.
Baseline: de5c4f969 and the T405 discussion findings. Files: Queue and affected
existing proposals plus this compact record; no ABI, source, build or runtime
change. Apply documentation/execution skills and repository rules. Verify
ownership coverage, relative links, documentation governance and diff checks,
then review, commit and push. Stop for conflicting ownership or expanded
architecture; preserve the pre-existing physical-mapping proposal edit outside
this delivery. All six families now have candidate owners and bounded acceptance
in Queue and eight existing proposals; no new proposal or numeric packet was
created. Documentation governance (including relative paths/anchors) and
git diff --check passed. Review confirms candidate order and product behavior
unchanged. Commit/push verification follows this record; an unavailable push
is pending delivery, not a clean-workspace claim. T405 S3 remains unchanged.

## Previous Governance Closure

M0 Td S9 P1 consolidates the
[U02 physical-memory mapping candidate](../proposals/proposal-physical-memory-mapping-restoration-001.md)
at the Queue head and the existing
[U01 CCPU40 C-VID/event candidate](../proposals/proposal-ccpu40-cvid-event-path-restoration-001.md)
before broker work. Both require original-owner research before restoration;
neither assumes that current behavior is a proved runtime defect. This is
documentation only: T405 S3 remains active and no source implementation is
admitted. Governance/link and diff checks apply; remote delivery is verified
separately from this record.

M0 Td S8 P1 records the owner-requested
[DPMI/XMS restoration candidate](../proposals/proposal-dpmi-xms-contract-restoration-001.md)
at the Queue head: D36-D37 shared memory ownership first, then D35 transition
frames with explicit U04/U07 boundaries. Other candidates retain their relative
order. This is documentation only; T405 S3 remains active and no source repair
is admitted. Governance/link and diff verification apply; a local commit alone
does not establish remote delivery.

M0 Td S7 P1 consolidates the owner-approved RTL, Console/resource and
[WOW/debugger proposals](../proposals/proposal-wow-debugger-original-owner-restoration-001.md)
with the broker proposal and revised Queue. Candidate order is RTL, DOS file
services, Console/resources, broker, WOW/debugger, WOW16 workload, then release.
This supersedes the S6 sequencing hold; T405 S3 remains the sole active numeric
packet and no source implementation is admitted. Documentation governance/link
and diff checks are required; this record does not itself prove remote delivery.

M0 Td S6 P1 consolidates the owner-requested candidate planning documents:
the [broker proposal](../proposals/proposal-cross-process-broker-closure-001.md)
includes D01-D07/D10, staged source-first recovery and the non-mutating D03
boundary; the [DOS file-service proposal](../proposals/proposal-dos-file-service-restoration-001.md)
defines two sequential stages for D31-D33 and D08 and occupies the Queue
head. Broker-first sequencing remains a proposal, not an adopted reorder.
This documentation-only change leaves T405 S3 active and admits no product
implementation. Governance/link and diff checks apply; remote delivery is
not established by the local commit alone.

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
