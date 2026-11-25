# Project Status

## Current Work

**Active: M0 T485 S5** (Ordinary Mode). S2 delivered H001–H008 in commit 574d526e0. The S2 receipt identifies its twelve host images. The guest and host inputs are listed in the [S2 package receipt](../etc/evidence/t485-s1/h001-h008-package22.csv). S3 delivered H009–H022, bringing the total to 22/159. S4 delivered H023 host headers and H024 link resources, bringing the total to 24/159. H025 host sources brings the total to 25/159. H026 delivered the worker image and exports, bringing the total to 26/159. H027 delivered the suballocator baseline, bringing the total to 27/159. H028 delivered the XP VCDEX provider, bringing the total to 28/159. H029 restored the sample1 directory selector (29/159). H030 delivered sample2 and the 30/159 Full checkpoint. H031 delivered the XP VDMDBG reference (31/159). H032 delivered the redirector and matching SP3 guest (32/159). H033 delivered the XP WOW32 package and paired guest dependencies (33/159). See the [H033 receipt](../etc/evidence/t485-s1/h033.md). H034 delivered XMS and the startup repair checkpoint (34/159); Full passed all 23 gates on runtime58. See the [H034 receipt](../etc/evidence/t485-s1/h034.md). H035–H036 retired duplicate selectors; S4 closed at 36/159. See the [S4 closure](../etc/evidence/t485-s1/s4-closure.md). N001 delivered the XP capture ABI and utilities (37/159); see the [N001 receipt](../etc/evidence/t485-s1/n001.md). N002 delivered the XP ProbeForRead baseline (38/159); see the [N002 receipt](../etc/evidence/t485-s1/n002.md), including the reproduced baseline WRITE test limitation. N003 delivered the XP RTL environment, error tables and full x86 primitives (39/159); see the [N003 receipt](../etc/evidence/t485-s1/n003.md). Owner-admitted P4 first repairs DPMI dispatch ownership and adds enhanced Win3.1/Win95 MS-DOS Alt+Enter round trips to Full; N004 follows the newly inserted S6-S13 repair sequence, not P4 alone. See the [H032 receipt](../etc/evidence/t485-s1/h032.md). See the [H031 receipt](../etc/evidence/t485-s1/h031.md). See the [H030 receipt](../etc/evidence/t485-s1/h030.md). See the [H029 receipt](../etc/evidence/t485-s1/h029.md). See the [H028 receipt](../etc/evidence/t485-s1/h028.md). See the [H027 receipt](../etc/evidence/t485-s1/h027.md). See the [H026 receipt](../etc/evidence/t485-s1/h026.md). See the [H025 receipt](../etc/evidence/t485-s1/h025.md). See the [H023–H024 receipt](../etc/evidence/t485-s1/h023-h024.md). See the [S3 closure and H022 release receipt](../etc/evidence/t485-s1/h022.md).

## Owner-admitted repair insertion

On 2026-11-26 the owner inserted the [H-series repair sequence](../etc/operations/m0-t485-h-repair-plan.md) before N004. S5 remains active for N001-N003 and existing P4 delivery. Next are S6-S13, then S14 resumes N004-N012; later migration packets become S15-S20. The [persistent audit handoff](../etc/evidence/t485-h-repair/README.md) assigns every file and point. Audit classification is not repair acceptance. N004 is gated on S13 closure; migration progress remains 39/159.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S5, Ordinary Mode |
| Candidate Proposal | [XP migration](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md) |
| Admission And Approval | Owner approved use of the pinned XP SP1 source throughout the [migration sequence](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md), including delivery to the existing origin. |
| Host binary acceptance | Ship the locally compiled host components, including NTVDMD.DLL and necessary project adaptations. Use the selected SP3 binaries for ABI and behavior comparisons. Verify both installed copies against the manifest of the project build. |
| Objective | P4: unify DOSX ownership at hardware/software/fault dispatch, preserve guest state on rejection, and verify MS-DOS window/fullscreen round trips in Full. Owner follow-up also repairs NTCON destroying the CAF-selected Window during a graphics/text frame gap. Close the delivered N001–N003 scope and existing P4, then hand off to inserted S6–S13; N004–N012 resumes in S14. Reconcile Base/NT, Win32, fonts, OLE and network support against OpenNT, current code and pinned XP SP1, retaining required callers and ABI. |
| Reference Baseline | S1 file ledger, pinned XP source, S4 runtime58 and selected immutable SP3 media. |
| Verification | Compile supported targets, test affected host bindings and paired guest interfaces, publish the paired package, verify hashes, commit and push. Record directory membership, preserved adaptations and rollback. |
| Constraints | Preserve guest binaries, approved archive membership and unrelated files. Within migrated files, retain unused original code unless concrete evidence requires exclusion or adaptation; lack of callers alone is not evidence. Independent new XP components remain queued. |
| Non-goals | No original guest modifications or independent XP component imports. |
| Files And ABI Surface | P4: dpmiint.c, NTCON window controller and its fixture, existing CCPU monitor fixture, guest desktop/matrix test and Full runner; unchanged guest binaries. Delivered N001-N003 paths and their required existing header/caller dependencies; the inserted plan and persistent audit handoff. N004-N012 implementation is deferred to S14. |
| Applicable Rules | Execution, architecture, coding and source policies listed by docs/README.md. |
| Expected Markers | Supported builds and affected production tests pass on the published package. |
| Asset Needs | Pinned XP SP1 tree, immutable SP3 media and existing test fixtures. |
| Reporting Requirements | Before each batch: brief, affected files and functional additions/removals/changes. Report non-#error mirror points and added+deleted lines before versus fixed OpenNT and after versus fixed XP SP1, their delta, remaining adaptations, validation and delivered count; do not substitute commit-relative source line counts. |
| Stop Conditions | Failed production contract or required scope beyond the admitted component closure. |
| Exit Criteria | N001-N003 and existing P4 delivered with dual publication, recorded validation, commits and pushes; repair handoff recorded. Admit S6 next; N004-N012 requires S13 closure and transfers to S14. |
| Original Owner Request | Continue XP migration in ledger order; owner on 2026-11-26 inserts all H-series audit findings as ROI-ordered repair S packets before resuming N004. Preserve targeted tests and Full cadence. |
| Similar-Issue Sweep | Update all consumers of changed declarations together; keep unchanged contexts and guest boundaries intact. |

## S1 Closure Record

[Inventory, media qualification, H001 ABI evidence and rollback](../etc/evidence/t485-s1/README.md). The selected English Professional SP3 disc supplies 61 direct binary matches. Source and media span different service packs. Product migration and runtime verification begin in S2.

## S2 Closure Record

[H001-H008 delivery](../etc/evidence/t485-s1/h005-h008.md).

## S3 Closure Record

[H009-H022 delivery](../etc/evidence/t485-s1/h022.md).

## S4 Closure Record

[H023-H036 closure](../etc/evidence/t485-s1/s4-closure.md).

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Current Technical Baseline

H030 Full passed 20/20 on product42 after dual publication. H024 Full checkpoint: product36 was built for x64/x86 and published at both locations. The DPMI fixes, package identity and test evidence are in the [checkpoint receipt](../etc/evidence/t485-s1/h024-full-checkpoint.md). Full passed all 20 gates on product36. H026 product38 passed its targeted PE export, resource and DOS4GW tests.

The S4 product36 package contains thirteen host images, including VJOY.DLL. Commit 4e6e4bd7f contains the previous product34 package. T485 S1 catalogues 159 batches and 3,986 tracked files. P4 verifies eleven rollback images at both locations. The candidate declares 5.1.2600.1106. Its README reports leaked provenance. The owner approved local source use on the OpenNT basis. The owner selected English x86 Professional SP3 media. The 61 direct matches were independently verified on the selected disc. Source/media ABI compatibility is verified per batch. The project owner has approved XP source use and ordinary project delivery for all T485 batches. H001 build ownership is traced.

## Recent Governance

Existing-component batches can include a required provider from another component. XP-only additions with independent scope are reserved for the next T.
