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

S5 P3 consolidates the keyboard profile through the released PIF editor. Byte equivalence and the real DOS keyboard startup and reader checks passed. [Evidence](../etc/evidence/t484-s5-keyboard-pif-consolidation.md).

S5 P2 consolidates pure DOS and XMS through the same CMD/PIF owner and retires two native writers. Five variants are byte-identical and two differ only in unused legacy string tails. Actual pure DOS and XMS cases passed. The cached host build and host/addon hashes match the existing release. [Evidence](../etc/evidence/t484-s5-pure-xms-pif-consolidation.md).

S5 P1 consolidates EMS profile generation through the current PIF editor and CMD. Native field tests, byte equivalence, two real EMS cases and a released-tool EMS case passed. All fourteen addons were verified and only PIF.EXE changed. The two host publication sets retained all eleven hashes. [Evidence](../etc/evidence/t484-s5-ems-pif-consolidation.md).

S4 P46 moves the two remaining matrix preparation chains to existing CMD tools and separates profile caches by installation path. Win31 passed all fourteen cases. Win95 startup passed all four configurations, and the supported scheduling controls matched the old preparation. The driver-disabled DOS timeout reproduced with both. Cached full build and both host publication identities verified. [Evidence](../etc/evidence/t484-s4-matrix-preparation.md).

S4 P45 moves both Win31 profile test callers to shared CMD preparation and the current launcher. Enhanced empty-profile and DDE passed; Standard DDE passed. Cached full build and both published package identities verified. [Evidence](../etc/evidence/t484-s4-win31-profile-preparation.md).

S4 P44 moves Product Control fixture preparation to CMD. Cached full build, package identity tests and the complete Control gate passed. Both published host packages retained all eleven hashes. [Evidence](../etc/evidence/t484-s4-product-control-preparation.md).

S4 P43 moves DOS identity package and guest preparation into existing CMD/native owners. Both real DOSONLY cases collected the earlier task-pairing evidence. Windows retirement still times out. [Evidence](../etc/evidence/t484-s4-dos-identity-preparation.md).

S4 P42 shares CMD patch reapply and guest witness preparation. Real Win95 identity and guest-only cases passed; the launcher exit-code boundary remains. [Evidence](../etc/evidence/t484-s4-win95-identity-preparation.md).

S4 P41 moves Win95 editor setup to shared CMD launcher and prompt preparation. All eleven generated inputs match the old preparation. The old and new real editor runs both time out after guest readiness. [Evidence](../etc/evidence/t484-s4-win95-editor-preparation.md).

S4 P40 shares CMD launch preparation between the Win31 editor and identity tests, and moves identity witness copies to the existing input tool. The real Win31 identity pair and staged configuration contracts pass. [Evidence](../etc/evidence/t484-s4-win31-identity-preparation.md).

S4 P39 builds the Win31 continuation observer through the current CMD worker graph and stages it with the existing package owner. The ordinary Win31 control and dual-editor mouse, save and normal exit pass. The optional explicit VMD startup failure is recorded separately. [Evidence](../etc/evidence/t484-s4-continuation-observer.md).

S4 P38 closes shared Win31 matrix preparation with CMD monitor copy and identity binding. The isolated guest and live registry snapshot passed. Dual-editor diagnostic acceptance remains pending. [Evidence](../etc/evidence/t484-s4-win31-monitor-preparation.md).

S1 inventory/design and S2 native host migration are closed. The
[execution plan](../etc/operations/t484-cmd-build-release-plan.md) retains
the complete task scope. [S3](../etc/evidence/t484-s3-closure.md) and
[S4](../etc/evidence/t484-s4-matrix-preparation.md) are closed. S5 is active.
S6 clean, interpreter-free end-to-end acceptance remains pending.

S4 deliveries through P46 are recorded; detailed results and limitations remain in
these records. Shared primitives alone never close an unverified caller.

| P | Delivered scope and evidence |
| --- | --- |
| 1 | [Native package primitives](../etc/evidence/t484-s4-package-primitives.md) |
| 2 | [System32 CMD package owner and failure contracts](../etc/evidence/t484-s4-system32-cmd.md) |
| 3 | [Addon utility publication](../etc/evidence/t484-s4-addon-utility-publication.md) |
| 4 | [All14 addon publication](../etc/evidence/t484-s4-addon-driver-publication.md) |
| 5 | [Fresh package identity](../etc/evidence/t484-s4-stage-identity-refresh.md) |
| 6 | [Physical staging; inherited-environment limitation retained](../etc/evidence/t484-s4-physical-stage.md) |
| 7 | [Three SOL/WRITE applications](../etc/evidence/t484-s4-wow-app-preparation.md) |
| 8 | [Cleanup and broker handoff classification](../etc/evidence/t484-s4-independent-consumers.md) |
| 9 | [PIC and serial input preparation](../etc/evidence/t484-s4-device-inputs.md) |
| 10 | [Keyboard PIF/registry preparation](../etc/evidence/t484-s4-keyboard-preparation.md) |
| 11 | [Print preparation and retired-reporter correction](../etc/evidence/t484-s4-print-preparation.md) |
| 12 | [latency preparation](../etc/evidence/t484-s4-latency-preparation.md) |
| 13 | [wow diagnostic preparation](../etc/evidence/t484-s4-wow-diagnostic-preparation.md) |
| 14 | [idle ddeml preparation](../etc/evidence/t484-s4-idle-ddeml-preparation.md) |
| 15 | [dpmi xms preparation](../etc/evidence/t484-s4-dpmi-xms-preparation.md) |
| 16 | [dlc generic preparation](../etc/evidence/t484-s4-dlc-generic-preparation.md) |
| 17 | [mouse dem wow preparation](../etc/evidence/t484-s4-mouse-dem-wow-preparation.md) |
| 18 | [tabstop logon preparation](../etc/evidence/t484-s4-tabstop-logon-preparation.md) |
| 19 | [remote preparation](../etc/evidence/t484-s4-remote-preparation.md) |
| 20 | [fcb preparation](../etc/evidence/t484-s4-fcb-preparation.md) |
| 21 | [cursor preparation](../etc/evidence/t484-s4-cursor-preparation.md) |
| 22 | [oem preparation](../etc/evidence/t484-s4-oem-preparation.md) |
| 23 | [fatal preparation](../etc/evidence/t484-s4-fatal-preparation.md) |
| 24 | [memory preparation](../etc/evidence/t484-s4-memory-preparation.md) |
| 25 | [video7 preparation](../etc/evidence/t484-s4-video7-preparation.md) |
| 26 | [forcedos preparation](../etc/evidence/t484-s4-forcedos-preparation.md) |
| 27 | [native output preparation](../etc/evidence/t484-s4-native-output-preparation.md) |
| 28 | [clipboard preparation](../etc/evidence/t484-s4-clipboard-preparation.md) |
| 29 | [external message preparation](../etc/evidence/t484-s4-external-message-preparation.md) |
| 30 | [network profile preparation](../etc/evidence/t484-s4-network-profile-preparation.md) |
| 31 | [mine preparation](../etc/evidence/t484-s4-mine-preparation.md) |
| 32 | [multi dialog preparation](../etc/evidence/t484-s4-multi-dialog-preparation.md) |
| 33 | [Guest copying and ownership cleanup](../etc/evidence/t484-s4-guest-copy-ownership.md) |
| 34 | [Native hive preparation](../etc/evidence/t484-s4-hive-relocation.md) |
| 35 | [Shared copied-guest paths](../etc/evidence/t484-s4-guest-paths.md) |
| 36 | [Guest INI reuse and naming](../etc/evidence/t484-s4-guest-ini-reuse.md) |

| S3 delivery | Actual result |
| --- | --- |
| [P1-P180](../etc/evidence/t484-s3-early-deliveries.md) | Earlier delivered producer migrations; complete results and evidence links retained in the batch index. |

| [P181-P213](../etc/evidence/t484-s3-early-deliveries.md#p181-p213-deliveries) | Completed producer cleanup and obsolete-tool retirement;319/355 disposed,36 pending, plus23 supplemental preparation chains. |
| [P214 runtime compiler lookup](../etc/evidence/t484-s3-runtime-tool-path.md) | Removed two obsolete Win95 NASM runtime lookups; cached full build and unchanged dual-publication identities verified. Counts remain319/355 and23 supplemental chains. |
| [P215 inline C# retirement](../etc/evidence/t484-s3-dynamic-compiler-retirement.md) | Retired3 obsolete samplers; EDIT uses existing native Userenv reader and real logon case passes. Unchanged inherited-environment case fails on both old/new scripts. Frozen319/355;24 supplemental preparation chains remain. |
| [P216 historical compiler retirement](../etc/evidence/t484-s3-historical-compiler-retirement.md) | Retired5 obsolete compiler scripts; current mouse callback assertions moved to CMD/native and pass, real DPMI suite passes,14 addon raw/release hashes match. Frozen319/355 and24 supplemental chains remain. |

Current census: **355/355 disposed, 0 pending**. All supplemental preparation chains are disposed.
Runtime limitations remain recorded in the relevant deliveries.

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

T484 S3 P39 records shared native editor composition and its delivery under the
owner's full-task authorization. The containing commit is its delivery
identity. S3 is not closed; queue order is unchanged.
