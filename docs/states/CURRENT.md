# Project Status

## Current Work

**Active: M0 T484 S4** (Ordinary Mode). Owner requested continuous execution
through T484 closure. S3 addon/test producers are closed; S4 package
and publication migration is now admitted. Final interpreter-free clean acceptance remains S5.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T484 S4, Ordinary Mode. |
| Admission And Approval | Owner: 准入新t任务！要求构建脚步去除python powershell javascript 仅使用cmd. |
| Original Owner Request | Admit unified build/release; CMD-only build scripts, removing Python, PowerShell and JavaScript dependencies. |
| Objective | Migrate required runtime configuration, isolated package preparation, manifests and publication to CMD/native ownership; retire obsolete wrappers and preserve identity, failure and cleanup contracts. |
| Candidate Proposal | [Unified build/release](../proposals/proposal-unified-build-cmd-release-001.md) |
| Reference Baseline | T483 closure e74afba09; product P85 9e7eb19c0, package100b16d9-992d-4e2c-8d99-66a4e891dd26. |
| Files And ABI Surface | Build/publication/staging scripts and caller preparation; no product ABI change or original guest rebuild. |
| Applicable Rules | CMD-only project build scripting, existing layers, source-owner inventories and caches, immutable original guest, truthful failures. |
| Verification | Verify actual shared staging, host/addon publication and failure cases; targeted tests, coherent dual release hashes, governance and commit/push. Final clean interpreter-free acceptance remains S5. |
| Expected Markers | Single package owner, complete manifests, exact image identities, CMD/native production without hidden forbidden interpreters. |
| Asset Needs | Existing source/toolchains and approved assets only. |
| Non-goals | No runtime redesign, mirror audit, original guest rebuild or wholesale standalone test/audit language conversion. |
| Similar-Issue Sweep | Nested interpreters, configure hooks, generated commands, post-link steps, manifest/hash tools, addon/test producers and clean bootstrap. |
| Reporting Requirements | Separate admission, inventory, implementation and delivery; count remaining forbidden-interpreter build edges and output coverage. |
| Stop Conditions | No forbidden interpreter behind CMD or another script mode; no parallel build graph or stale output as clean-build proof. |
| Exit Criteria | Required configuration, staging and publication chains use CMD/native; callers repaired, rollback/failure checks pass, coherent dual release verified and delivery pushed. |

## Current Progress

S1 inventory/design and S2 native host migration are closed. The
[execution plan](../etc/operations/t484-cmd-build-release-plan.md) retains
the complete task scope. [S3 is closed](../etc/evidence/t484-s3-closure.md); S4 is active and S5 clean,
interpreter-free end-to-end acceptance have not completed.

S4 P1 adds bounded UUID/PE operations to the existing native package tool;
[tests and evidence](../etc/evidence/t484-s4-package-primitives.md) retain the
unchanged32 pending entries and24 supplemental preparation chains. No staging
caller is counted as migrated merely because these primitives exist.

| S3 delivery | Actual result |
| --- | --- |
| [P1-P180](../etc/evidence/t484-s3-early-deliveries.md) | Earlier delivered producer migrations; complete results and evidence links retained in the batch index. |

| [P181-P213](../etc/evidence/t484-s3-early-deliveries.md#p181-p213-deliveries) | Completed producer cleanup and obsolete-tool retirement;319/355 disposed,36 pending, plus23 supplemental preparation chains. |
| [P214 runtime compiler lookup](../etc/evidence/t484-s3-runtime-tool-path.md) | Removed two obsolete Win95 NASM runtime lookups; cached full build and unchanged dual-publication identities verified. Counts remain319/355 and23 supplemental chains. |
| [P215 inline C# retirement](../etc/evidence/t484-s3-dynamic-compiler-retirement.md) | Retired3 obsolete samplers; EDIT uses existing native Userenv reader and real logon case passes. Unchanged inherited-environment case fails on both old/new scripts. Frozen319/355;24 supplemental preparation chains remain. |
| [P216 historical compiler retirement](../etc/evidence/t484-s3-historical-compiler-retirement.md) | Retired5 obsolete compiler scripts; current mouse callback assertions moved to CMD/native and pass, real DPMI suite passes,14 addon raw/release hashes match. Frozen319/355 and24 supplemental chains remain. |

Current census: **323/355 disposed,32 pending**, plus24 supplemental
preparation chains (overlapping scopes, not additive file counts).

P217 retires four superseded Win31 setup scripts and three duplicate tests;
exact binary adaptation assertions now use the released native tool through
the current CMD roundtrip. Verification and ledger reconciliation are in
[the delivery record](../etc/evidence/t484-s3-win31-retirement.md).

P183 found18 additional direct NASM sites; P184-P191 migrate or retire those
producers. Their shared preparation chains remain explicitly pending.
The frozen355 census is not the complete remaining invocation graph. P193's13 additional candidates were resolved by P198; P199's broader30
candidates were classified by P211. Neither supplemental review closes the
remaining configuration/staging chains.

Completed implementation batches through P141 passed their cached full host build. P73 checks11 raw host inputs
and22 published hashes unchanged; no changed product images required
republication. Other test producers remain pending. Static candidate counts
are not live-edge counts or completion evidence. Detailed runs, limitations,
source profiles and delivery identities belong to the linked evidence.

## S3 Closure Record

[S3 closure and S4 handoff](../etc/evidence/t484-s3-closure.md) records
the delivered addon/test producers through P217, actual validation and32
unresolved inventory entries plus24 supplemental preparation chains retained
for S4. Runtime failures remain explicit; final clean acceptance remains S5.

## S2 Closure Record

The [candidate reconciliation](../etc/evidence/t484-progress-reconciliation.md)
tracks all355 original paths and separates file changes from completed
dispositions. Remaining disposition work is explicit; batch counts are not a
completion percentage.

[S2 P7 closure](../etc/evidence/t484-s2-worker-cmake.md), commit ce4fc0f9c,
completes the native host composition and CMD entry. All11 images are
published to both locations under package d8983d2b-3fe1-4d18-8ea9-64613c84346c.
The final worker reconciliation covers791 compiler profiles,67 archives,
153 links and274 export lines. Prior S2 batches and their actual comparison
limits are indexed in [supporting material](../etc/README.md).
S3 was admitted under the owner's continuous T484 authorization.

## S1 Closure Record

[S1 inventory/design](../etc/evidence/t484-s1-build-inventory.md) fixes the
ordered worklist:817 scanned inputs,355 migration candidates and1116 literal
references. These are lexical inventory counts, not proved live dependencies.
No production build/publication/runtime pass is attributed to S1.

## Current Technical Baseline

[T483 closure](../history/m0-t483-closure.md) indexes all S1-S21 outcomes,
retained mirror decisions and known boundaries. Final production revision is
9e7eb19c0 (S20 P85), package100b16d9-992d-4e2c-8d99-66a4e891dd26.
The current publication is T484 S2 P7, recorded above; P85 is its behavior
comparison baseline. P85 Full20/20 runtime evidence is reused for this
byte-equivalent/timestamp-only build change, not reported as a new Full run.

S19 completed38/38 B-to-C2 files; S20 completed94/94 B-to-C1 files.
Private compiled headers/search roots and linked implementation dependencies
from B to C1/C2 are zero. Legal synchronous callbacks and original named-image
APIs remain explicit; C1/C2 preserve original OpenNT connections.

P85 non-error mirror delta+4points/+244lines; S20 cumulative+15/+815;
S19 cumulative+12/+574. Retained decisions are not blanket-approved by closure.
The inherited environment failure and other boundaries remain in
[TODO](TODO.md) and the closure record. No product or guest change in S21.

## Recent M0 Closures

- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Recent Governance

T484 S3 P39 records shared native editor composition and its delivery under the
owner's full-task authorization. The containing commit is its delivery
identity. S3 is not closed; queue order is unchanged.
