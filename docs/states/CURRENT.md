# Project Status

## Current Work

**Active: M0 T484 S5** (Ordinary Mode). S4 closed after P46
`5b88ee331` was pushed with both final matrix preparation chains verified.
S5 consolidates native preparation ownership. S6 remains the clean
interpreter-free acceptance stage.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T484 S5, Ordinary Mode. |
| Admission And Approval | Owner approved continuous T484 execution and directed reuse of the existing PIF and SYSTEM.INI utilities. [Sequenced S5 scope](../etc/operations/t484-cmd-build-release-plan.md#s5-consolidate-native-preparation-tools). |
| Original Owner Request | Use CMD for build scripting and consolidate native preparation tools by extending the existing editors instead of creating duplicate tools. |
| Objective | Retire redundant native fixture producers and use the existing PIF and guest INI owners for their formats. Preserve observable fixture behavior and real callers. |
| Candidate Proposal | [Unified build/release](../proposals/proposal-unified-build-cmd-release-001.md) |
| Reference Baseline | P46 `5b88ee331`, [final S4 evidence](../etc/evidence/t484-s4-matrix-preparation.md), and the delivered S4 native package and INI preparation changes. |
| Files And ABI Surface | Authored PIF and INI editors, CMD preparation, selected test fixtures and their build targets. Preserve product ABI and original guest images. |
| Applicable Rules | CMD-only build scripting, LF for authored files, source and architecture rules, and verification of actual product-facing changes. |
| Verification | Record the DIR-ordered producers and active callers. Verify retained PIF layouts and fields, INI byte/section behavior, dependent focused runtime cases, cached full build, publication hashes and delivery. |
| Expected Markers | One normal writer per format, malformed or separately meaningful layout fixtures explicitly scoped, superseded targets and callers removed, and no new interpreter dependency. |
| Asset Needs | Existing released tools, project-written fixtures and a coherent published product package. |
| Non-goals | No original guest rebuild, broad guest runtime redesign, general scripting in a native tool or forced merger of unrelated binary-binding contracts. |
| Similar-Issue Sweep | Existing and newly migrated PIF/INI producers, fixed CONFIG.NT/AUTOEXEC.NT generation, and native binary-binding fixtures. |
| Reporting Requirements | Report before/after executable, parser, source line, build target and caller counts, along with retained unique fixture coverage and validation limits. |
| Stop Conditions | Pause changes that would alter a fixture's intentional format coverage or require unapproved changes to mirror code or production behavior. |
| Exit Criteria | Consolidated owners exercise their actual consumers; redundant implementations and references are removed; applicable tests, cached build, publication and push are verified. |

## Current Progress

S5 P5 promotes the existing guest INI editor to INI.EXE and removes both launch scripts' duplicate INI writers. The released tool passed Win31 six-profile, Win95 four-profile and apply/unapply package verification. [Evidence](../etc/evidence/t484-s5-shared-ini.md).

S5 P4 replaces the standard and NT-only native PIF writers with authored layouts and the existing PIF editor. Both layouts and the x86/x64 native payload checks passed. [Evidence](../etc/evidence/t484-s5-pif-layouts.md).

S5 P3 consolidates the keyboard profile through the released PIF editor. Byte equivalence and the real DOS keyboard startup and reader checks passed. [Evidence](../etc/evidence/t484-s5-keyboard-pif-consolidation.md).

S5 P2 consolidates pure DOS and XMS through the same CMD/PIF owner and retires two native writers. Five variants are byte-identical and two differ only in unused legacy string tails. Actual pure DOS and XMS cases passed. The cached host build and host/addon hashes match the existing release. [Evidence](../etc/evidence/t484-s5-pure-xms-pif-consolidation.md).

S5 P1 consolidates EMS profile generation through the current PIF editor and CMD. Native field tests, byte equivalence, two real EMS cases and a released-tool EMS case passed. All fourteen addons were verified and only PIF.EXE changed. The two host publication sets retained all eleven hashes. [Evidence](../etc/evidence/t484-s5-ems-pif-consolidation.md).

Earlier S4 preparation deliveries are indexed in [supporting material](../etc/README.md). The [S3 P181–P213 delivery record](../etc/evidence/t484-s3-early-deliveries.md#p181-p213-deliveries) retains those batch results. The [S4 closure record](#s4-closure-record) identifies the final batch and runtime boundaries.

## S4 Closure Record

[P46 and final S4 matrix evidence](../etc/evidence/t484-s4-matrix-preparation.md)
record the last delivery, pushed as 5b88ee331. All 355 frozen candidates
and the supplemental staging chains have a disposition. The published
host sets were hash-verified. Recorded Win95 runtime limitations remain
open within their stated boundaries. Clean acceptance of the final
consolidated build and publication graph remains S6.

## S3 Closure Record

[S3 closure and S4 handoff](../etc/evidence/t484-s3-closure.md) records
the delivered addon/test producers through P217, actual validation and32
unresolved inventory entries plus24 supplemental preparation chains retained
for S4. Runtime failures remain explicit; final clean acceptance remains S6.

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

S5 uses the approved existing-editor consolidation and leaves final clean acceptance to S6.
