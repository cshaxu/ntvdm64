# Project Status

## Current Work

**Active: M0 T485 S9** (Ordinary Mode). S2 delivered H001–H008 in commit 574d526e0. The S2 receipt identifies its twelve host images. The guest and host inputs are listed in the [S2 package receipt](../etc/evidence/t485-s1/h001-h008-package22.csv). S3 delivered H009–H022, bringing the total to 22/159. S4 delivered H023 host headers and H024 link resources, bringing the total to 24/159. H025 host sources brings the total to 25/159. H026 delivered the worker image and exports, bringing the total to 26/159. H027 delivered the suballocator baseline, bringing the total to 27/159. H028 delivered the XP VCDEX provider, bringing the total to 28/159. H029 restored the sample1 directory selector (29/159). H030 delivered sample2 and the 30/159 Full checkpoint. H031 delivered the XP VDMDBG reference (31/159). H032 delivered the redirector and matching SP3 guest (32/159). H033 delivered the XP WOW32 package and paired guest dependencies (33/159). See the [H033 receipt](../etc/evidence/t485-s1/h033.md). H034 delivered XMS and the startup repair checkpoint (34/159); Full passed all 23 gates on runtime58. See the [H034 receipt](../etc/evidence/t485-s1/h034.md). H035–H036 retired duplicate selectors; S4 closed at 36/159. See the [S4 closure](../etc/evidence/t485-s1/s4-closure.md). N001 delivered the XP capture ABI and utilities (37/159); see the [N001 receipt](../etc/evidence/t485-s1/n001.md). N002 delivered the XP ProbeForRead baseline (38/159); see the [N002 receipt](../etc/evidence/t485-s1/n002.md), including the reproduced baseline WRITE test limitation. N003 delivered the XP RTL environment, error tables and full x86 primitives (39/159); see the [N003 receipt](../etc/evidence/t485-s1/n003.md). S5 P4 repairs DPMI dispatch ownership and CAF Window continuity, adds strict enhanced Win3.1/Win95 Alt+Enter round trips, and passes Full 23/23; N004 follows S6-S13 repair, not P4 alone. See the [H032 receipt](../etc/evidence/t485-s1/h032.md). See the [H031 receipt](../etc/evidence/t485-s1/h031.md). See the [H030 receipt](../etc/evidence/t485-s1/h030.md). See the [H029 receipt](../etc/evidence/t485-s1/h029.md). See the [H028 receipt](../etc/evidence/t485-s1/h028.md). See the [H027 receipt](../etc/evidence/t485-s1/h027.md). See the [H026 receipt](../etc/evidence/t485-s1/h026.md). See the [H025 receipt](../etc/evidence/t485-s1/h025.md). See the [H023–H024 receipt](../etc/evidence/t485-s1/h023-h024.md). See the [S3 closure and H022 release receipt](../etc/evidence/t485-s1/h022.md).

## Owner-admitted repair insertion

On 2026-11-26 the owner inserted the [H-series repair sequence](../etc/operations/m0-t485-h-repair-plan.md) before N004. S5 has completed N001-N003 and P4 delivery. S6-S8 are delivered; S9 is now active in S6-S13, then S14 resumes N004-N012; later migration packets become S15-S20. The [persistent audit handoff](../etc/evidence/t485-h-repair/README.md) assigns every file and point. Audit classification is not repair acceptance. N004 is gated on S13 closure; migration progress remains 39/159.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S9, Ordinary Mode |
| Candidate Proposal | [XP migration](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md) and [H repair plan](../etc/operations/m0-t485-h-repair-plan.md) |
| Admission And Approval | Owner ordered continuous S6-S13 repair execution; S8 P14 fc9492342 is delivered and pushed. |
| Objective | In relative-path order audit and simplify the 41 S9-assigned CPU/DPMI/BIOS/PIC/LIM/XMS/debug/header files, preserving demonstrated correctness fixes and original ownership. |
| Reference Baseline | S8 runtime14 and fixed XP objects; files.csv S9 ownership and frozen points.csv. |
| Verification | Whole-file original-source/exclusion/line review; cached full build, dual publication, relevant targeted tests, commit/push. CPU profile and scalar/RMW/string/stack/fetch, failure paths, DOS4GW and guest transitions as affected. |
| Constraints | Original-first; no original guest modification/rebuild; preserve exclusions, x87/permission/width/BOP fixes, layer boundaries and unrelated changes. Count non-error byte-sensitive diff points and lines including newline and EOF. |
| Non-goals | No N004 before S13; no speculative reimplementation, second state machine or migration progress inflation. |
| Files And ABI Surface | 41 S9 rows in files.csv, canonical relative-path order; inspect coupled providers before changes. |
| Applicable Rules | Execution, architecture, coding, document and source rules linked by docs/README.md. |
| Expected Markers | Every file has evidence-backed restoration or necessary residual, correct hashes/lines, final-package verification and delivery status. |
| Asset Needs | Existing pinned XP inputs and immutable guest fixtures. |
| Reporting Requirements | Per-file entry/exit, non-error points/lines before/after, cumulative totals and separate reviewed/verified/delivered counts. |
| Stop Conditions | Unapproved material scope or mirror increase outside owner allowance; unexplained semantic/ABI change. |
| Exit Criteria | All 41 files audited and admitted repairs delivered; no unassigned blockers; handoff to S10. |
| Original Owner Request | Finish inserted repair sequence before continuing N004-G097, prioritizing useful progress over excessive verification. |
| Similar-Issue Sweep | Original ownership, redundant state projections, ABI seams and unsupported-profile boundaries. |

## S9 Ordered Progress

[S9 progress](../etc/evidence/t485-h-repair/s9-progress.md): 35/41 source-reviewed,
35 delivered through P8 abe34fd2a; six unreviewed.
208/437 frozen points reviewed, 229 remain. P8 non-error 27/651 ->46/61
(+19/-590); S9 cumulative +77/-24037 against fc9492342. Forty measured files
421/5917; unchanged timer.c still awaits mapping and is outside absolute total.
Fifty-two restored points final-verified and delivered.
P8 cached build, dual publication (26 hashes), mouse restore, MEM/NTVDMD and NESticle passed; P8 committed and pushed. Next terminat.c.
Migration remains 39/159; Full stays S13.

## S8 Closure Record

[S8 progress](../etc/evidence/t485-h-repair/s8-progress.md): 851/851 source-reviewed
and final-verified, zero unreviewed. P13 8d96fbf50 is pushed; P14 fc9492342 is committed and pushed. Final non-error byte-sensitive census: 3465 points/328879 lines ->
1818 points/1818 lines, reduction 1647/327061; 606 byte-exact files and 245
with only #line residuals after exclusion projection. CRLF/LF and EOF count.
All 851 latest hashes match. Runtime14 cached full build, dual 13-image
publication (26 hashes), actual MEM/NTVDMD and NESticle tests passed.
N004 remains gated on S13; migration stays 39/159. S8 is closed; S9 is admitted above.
## S7 Closure Record

[S7 closure](../etc/evidence/t485-h-repair/s7-progress.md), delivered through a1f5bff4b.

## S7 P13 Final Verification

[Whole-file review](../etc/evidence/t485-h-repair/s7-p13-startup-review.md):
ntvdm.c 6/18 -> 5/17; wkgthunk.c 2/4 unchanged; wow32.c 18/103 -> 15/69.
20/20 source-reviewed and final-verified. Batch reduction 4/35; cumulative
869/7704 including the P10 header. Runtime16 cached build, dual 13-image
hash-verified publication, dispatch exception fixture, actual WOW, three
desktops, enhanced Alt+Enter roundtrips and NESticle passed. Win95 retains
knownExitOne. P13 is committed and pushed as a1f5bff4b; S7 is closed. Migration remains 39/159; S8 follows, not N004.
## S7 P12 Checkpoint

[SIM32 restoration](../etc/evidence/t485-h-repair/s7-p12-sim32.md): 19 points/107
lines to 14 points/74 lines. Fourteen necessary residuals are registered;
newly excluded translated-cache code is counted in full. Runtime15 cached
build, dual publication, CPU 123/0, high-linear/flush, real WOW, three desktops,
enhanced same-window roundtrips and NESticle passed. Win95 retains knownExitOne.
S7 is 17/20; three remain, next obj.vdm/ntvdm.c. Cumulative reduction including
the P10 header is 865 points/7669 lines. Commit/push closes P12 only.

## S7 P11 Checkpoint

[FM service admission](../etc/evidence/t485-h-repair/s7-p11-fm-service.md):
original device lifecycle restored; existing FMActive follows actual service
success. Non-error diff is 4 points/4 lines to 3 points/5 lines, with three
registered residuals. Runtime14 cached build, dual thirteen-image publication,
product FM/VDM ABI fixtures, actual WOW, enhanced Win3.1 strict roundtrip/normal
exit and NESticle passed. S7 is 16/20; four remain, next sim32.c. Cumulative
reduction is 860 points/7636 lines including the P10 header. Commit/push closes
P11 only; Full and audio capability completion are not claimed.

## S7 P10 Checkpoint

[Sound CLI restoration](../etc/evidence/t485-h-repair/s7-p10-sound-cli.md):
nt_sb.c is byte-exact XP (4 points/4 lines removed); required original vdm.h
request declarations reduce XP-relative differences by 3 points/45 lines.
Runtime13 cached build, dual 13-image publication, CLI ABI, real WOW, Win3.1
enhanced normal exit/strict fullscreen roundtrip and DOS4GW passed. S7 is
15/20 primary files; five remain, next nt_sbfm.c. Cumulative reduction including
the required header is 859 points/7637 lines. N016 is not declared migrated;
39/159 migration progress is unchanged. Commit/push completes P10 only.

## S7 P9 Checkpoint

[S7 P1-P9](../etc/evidence/t485-h-repair/s7-progress.md): fourteen files final-verified; six remain, next softpc.new/host/src/nt_sb.c. P9 cleans copy_fnc.c exclusions/lines and restores nt_fulsc.c formatting/regional structure. Batch reduction is 92 points/738 lines; cumulative S7 reduction is 852 points/7588 lines. Runtime12 cached build, dual 13-image publication, actual WOW, three desktop gates with enhanced fullscreen roundtrips, and DOS4GW passed. Win95 retains its approved exit-one boundary. Residuals are registered; commit/push completes P9. S7 remains active.

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
