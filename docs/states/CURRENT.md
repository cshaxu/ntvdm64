# Project Status

## Current Work

**Active: M0 T485 S7** (Ordinary Mode). S2 delivered H001–H008 in commit 574d526e0. The S2 receipt identifies its twelve host images. The guest and host inputs are listed in the [S2 package receipt](../etc/evidence/t485-s1/h001-h008-package22.csv). S3 delivered H009–H022, bringing the total to 22/159. S4 delivered H023 host headers and H024 link resources, bringing the total to 24/159. H025 host sources brings the total to 25/159. H026 delivered the worker image and exports, bringing the total to 26/159. H027 delivered the suballocator baseline, bringing the total to 27/159. H028 delivered the XP VCDEX provider, bringing the total to 28/159. H029 restored the sample1 directory selector (29/159). H030 delivered sample2 and the 30/159 Full checkpoint. H031 delivered the XP VDMDBG reference (31/159). H032 delivered the redirector and matching SP3 guest (32/159). H033 delivered the XP WOW32 package and paired guest dependencies (33/159). See the [H033 receipt](../etc/evidence/t485-s1/h033.md). H034 delivered XMS and the startup repair checkpoint (34/159); Full passed all 23 gates on runtime58. See the [H034 receipt](../etc/evidence/t485-s1/h034.md). H035–H036 retired duplicate selectors; S4 closed at 36/159. See the [S4 closure](../etc/evidence/t485-s1/s4-closure.md). N001 delivered the XP capture ABI and utilities (37/159); see the [N001 receipt](../etc/evidence/t485-s1/n001.md). N002 delivered the XP ProbeForRead baseline (38/159); see the [N002 receipt](../etc/evidence/t485-s1/n002.md), including the reproduced baseline WRITE test limitation. N003 delivered the XP RTL environment, error tables and full x86 primitives (39/159); see the [N003 receipt](../etc/evidence/t485-s1/n003.md). S5 P4 repairs DPMI dispatch ownership and CAF Window continuity, adds strict enhanced Win3.1/Win95 Alt+Enter round trips, and passes Full 23/23; N004 follows S6-S13 repair, not P4 alone. See the [H032 receipt](../etc/evidence/t485-s1/h032.md). See the [H031 receipt](../etc/evidence/t485-s1/h031.md). See the [H030 receipt](../etc/evidence/t485-s1/h030.md). See the [H029 receipt](../etc/evidence/t485-s1/h029.md). See the [H028 receipt](../etc/evidence/t485-s1/h028.md). See the [H027 receipt](../etc/evidence/t485-s1/h027.md). See the [H026 receipt](../etc/evidence/t485-s1/h026.md). See the [H025 receipt](../etc/evidence/t485-s1/h025.md). See the [H023–H024 receipt](../etc/evidence/t485-s1/h023-h024.md). See the [S3 closure and H022 release receipt](../etc/evidence/t485-s1/h022.md).

## Owner-admitted repair insertion

On 2026-11-26 the owner inserted the [H-series repair sequence](../etc/operations/m0-t485-h-repair-plan.md) before N004. S5 has completed N001-N003 and P4 delivery. S6 is delivered; S7 is admitted next in S6-S13, then S14 resumes N004-N012; later migration packets become S15-S20. The [persistent audit handoff](../etc/evidence/t485-h-repair/README.md) assigns every file and point. Audit classification is not repair acceptance. N004 is gated on S13 closure; migration progress remains 39/159.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S7, Ordinary Mode |
| Candidate Proposal | [XP migration](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md) and [H repair plan](../etc/operations/m0-t485-h-repair-plan.md) |
| Admission And Approval | Owner ordered continuous execution of S6-S13 before N004. S6 final package passed its admitted gates; S7 now begins its twenty-file structural repair. |
| Objective | Repair the twenty exclusion-pairing and adjacent files in relative-path order; finish every retained difference, preserve P4/S6 fixes, minimize non-error mirror points and lines. |
| Reference Baseline | S6 P1 package and targeted results linked in S6 closure; preceding S5 P4 Full 23/23; [audit handoff](../etc/evidence/t485-h-repair/README.md), pinned XP SP1 and three-way migration ledger. |
| Verification | Cached full build, dual hash-verified publication, targeted selected-preprocessor/ABI and affected runtime tests; commit/push per substantive complete-file batch. Check original guards/order, unsupported arms and non-overlapping masks. Full follows the admitted cadence. |
| Constraints | Preserve guest binaries, latest P4 fixes, original ownership, exclusion frameworks and diagnostic lines; minimal non-error mirror diff and layer dependencies. Preserve unrelated work. |
| Non-goals | No N004 implementation before S13 closure; no blanket removal of required selector compatibility, guest patch or new state machine. |
| Files And ABI Surface | Twenty files assigned S7 in the H repair files.csv; 1,055 frozen audit points. Start dpmi32/dpmi32.c, then its ordered DPMI/header/input/video/WOW files and necessary coupled providers. |
| Applicable Rules | Execution, architecture, coding and source policies listed by docs/README.md. |
| Expected Markers | Original condition ordering, error-only excluded bodies and correct diagnostic lines; retained semantic differences have actual provider evidence and final-package targeted validation. |
| Asset Needs | Fixed XP source, audit control receipts and existing immutable guest fixtures. |
| Reporting Requirements | Entry brief, file/function scope, three-way necessity evidence; before/after non-error mirror points and lines, exact tests, publication and delivery status. |
| Stop Conditions | Unresolved failure of the selected CPU profile or scope outside admitted provider closure. |
| Exit Criteria | All twenty assigned files and points resolved; required cached build, dual publication, targeted tests and commit/push per substantive delivery; handoff to S8. |
| Original Owner Request | Execute the new T485 S repair sequence before resuming N004-G097, without excessive repeated verification. |
| Similar-Issue Sweep | All exclusion masks, effective guards and selected bodies of each owned file; retain runtime ownership and failure semantics. |

## S7 P6 Checkpoint

[S7 P1-P6](../etc/evidence/t485-h-repair/s7-progress.md): ten files final-verified; ten remain, next softpc.new/base/video/video.c. P6 mouse_io.c reduces non-error differences from 682 points/6038 lines to 47/185, with residuals registered. S7 cumulative reduction is 737 points/6757 lines. Runtime9 cached build, dual 13-image publication and affected targeted gates passed; Win95 retains its approved exit-one boundary. Commit/push completes P6; S7 remains active.

## S6 Closure Record

[S6 selector ownership repair](../etc/evidence/t485-h-repair/s6-selector-investigation.md): cached full build, dual publication, 60 CPU cases, real WOW, three desktops and DOS4GW passed. Net non-error mirror reduction: one point and one line. Migration remains 39/159.

## S5 Closure Record

[S5 P4 repair and closure](../etc/evidence/t485-s1/s5-p4-dpmi-ownership.md).
N001-N003 and P4 are delivered in sequence; Full4 passed 23/23 on the final
13-image package. Old S5 N004-N012 transfers to S14 after S6-S13 repair.

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
