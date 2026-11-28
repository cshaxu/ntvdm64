# Project Status

## Current Work

**Active: M0 T485 S15** (Ordinary Mode), N013-N026.
[S14 closure](../etc/evidence/t485-s1/s14-closure.md): N004-N012 delivered;
overall60/159 directories, H001-H036 and N001-N024. S15 runtime105 is the current published and verified package.
The [working plan](../etc/operations/m0-t485-h-repair-plan.md)
retains all S16-S20 and remaining migration/capability obligations.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S15, Ordinary Mode. |
| Candidate Proposal | [XP baseline migration](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md). |
| Admission And Approval | Owner continuous-execution approval; S14 P10 closes N004-N012 with runtime12 Full24/24 and admits this next directory cohort. |
| Objective | Migrate existing N013-N026 owners to pinned XP SP1, preserving the selected product ABI, original algorithms and minimal evidenced host adaptations. |
| Reference Baseline | S14 P10/runtime12; fixed XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89; per-file registered OpenNT originals for not-yet-migrated files. |
| Files And ABI Surface | N013 private headers; N014 Windows private headers; N015 DDK; N016 Base declarations; N017 DS declarations; N018 Net; N019 Shell; N020 Windows declarations; N021 SDK; N022-N026 USER client/inc/kernel/rtl/server. 137 original files across14 directories. Already migrated paired headers retain their individual XP baseline. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [documents](../rules/DOCUMENT.md), [source policy](../etc/operations/policy/source-policy.md). |
| Verification | Each P: cached full build, dual publication/hash verification, affected targeted tests, commit, push. Every10 completed files and S closure require final-package Full. Retry only failed/unexecuted gates for unchanged inputs. |
| Expected Markers | Pinned originals, three-way diff, retired/relocated obsolete paths, current ABI callers/providers, nonerror byte diff before/after, coherent final package, genuine guest/host acceptance and clean pushed tree. |
| Asset Needs | Existing fixed XP SP1 source and selected English Professional SP3 media; immutable guest binaries. |
| Constraints | Preserve original compile selections, #error/#line and architecture boundaries. No original guest changes/rebuilds, Computer Use, unrelated changes or unapproved large mirror increases. |
| Non-goals | No wholesale Kernel32/CSR import; no independent XP-only components or unrelated capability rewrite. |
| Reporting Requirements | Brief each batch before editing; affected files and behavior, original-relative nonerror diff points/lines versus prior OpenNT baseline, migration totals and delivery status. |
| Stop Conditions | Unsupported required ABI, unapproved material boundary or mirror growth, missing original input, unexplained regression; resolve within authority or seek bounded decision. |
| Exit Criteria | All N013-N026 inventory files reconciled in order with necessary dependencies, fixed original identities and minimal residual evidence; final dual-published package Full, clean committed and pushed tree. |
| Original Owner Request | Continue existing-component XP baseline migration through G097; preserve architecture and immutable original guest, minimize original-relative mirror diff without excessive repeated validation. |
| Similar-Issue Sweep | XP-absent obsolete files, retained unused original code, changed compile guards, pointer/handle ABI, lost environment/exit ownership, guest package mismatch, false test pass/skip. |

## S15 Progress

12/14 directories delivered. P1 N013 and P2 N014 completed; directory50
Full24/24 passed. [P3 N015](../etc/evidence/t485-s1/n015.md),
[P4 N016 first-five](../etc/evidence/t485-s1/n016-first-five.md),
[P5 packing/SMB](../etc/evidence/t485-s1/n016-packing-smb.md) and
[P6 VDM](../etc/evidence/t485-s1/n016-vdm.md) are delivered. Their receipts retain
source identities, package hashes, targeted/Full gates and historical census.

P7 historical delivery remains indexed in this packet's evidence directory.

[P8 N017 first-three](../etc/evidence/t485-s1/n017-first-three.md) delivered.

[P9 N017 middle-ten](../etc/evidence/t485-s1/n017-middle.md) delivered; Full24/24 passed.

[P10 N017 final-six](../etc/evidence/t485-s1/n017-tail.md) delivered.

[P11 N018](../etc/evidence/t485-s1/n018.md) delivered; detailed census and
verification remain in its receipt.

[P12 N019](../etc/evidence/t485-s1/n019.md) delivered; detailed package,
Shell verification and census remain in its receipt.

[P13 N020 first-file](../etc/evidence/t485-s1/n020-first.md) delivered;
runtime13 Full24/24 and census remain in its receipt.

[P14 N020 PIF](../etc/evidence/t485-s1/n020-pif.md) delivered; package,
PIF tests and census remain in its receipt.

P15-P18 N020 deliveries are retained in the
[S15 P29 status snapshot](../etc/evidence/t485-s1/s15-p29-status.md)
and their linked per-P receipts.

[P19 N020 WOW USER](../etc/evidence/t485-s1/n020-wowuser.md) delivered runtime19
and Full24/24, completing N0207/7; detailed ABI, boundaries and census remain
in that receipt. Win95 exit1 boundary retained.

[P20 N021 first-three](../etc/evidence/t485-s1/n021-first.md) delivered runtime20;
its receipt retains targeted verification, original identities and census.

[P21 N021 NT entry](../etc/evidence/t485-s1/n021-nt.md) delivered runtime21
and Full24/24; its receipt retains paired prerequisites, census and boundaries.

[P22 N021 VDD](../etc/evidence/t485-s1/n021-vdd.md) delivered runtime22;
its receipt retains original ABI, compile exclusions, targeted runtime evidence
and census. No guest change.

[P23 N021 CSR](../etc/evidence/t485-s1/n021-csr.md) delivered runtime24,
original XP CSR headers and targeted capture/layout, service and guest NetAPI.
The receipt retains original ABI restoration and all historical census values.

[P24 N021 executive/LPC](../etc/evidence/t485-s1/n021-executive.md) delivered
runtime25 and targeted capture/layout, service and guest NetAPI. Its receipt
retains original DEVL/C++ recovery and census delta-23points/-208lines.

[P25 N021 process/thread](../etc/evidence/t485-s1/n021-process.md) delivered
runtime26, capture/layout, service42 and guest NetAPI. Its receipt retains
all ABI evidence and census delta+31points/-2012lines; N02111/17 then.

[P26 N021 registry](../etc/evidence/t485-s1/n021-registry.md) delivered
runtime27 with consolidated Full24/24. Its receipt preserves the observer
correction, all attempts and census. Historical Full evidence remains in that receipt.

[P27 N021 RTL](../etc/evidence/t485-s1/n021-rtl.md) and
[P28 N021 user RTL](../etc/evidence/t485-s1/n021-user-rtl.md) delivered;
their receipts retain package, verification, original identities and census.

[P29 N021 final SDK](../etc/evidence/t485-s1/n021-final.md) delivered runtime30:
ptypes32 exact XP; VDD stdcall boundaries/exclusions retained; debugger
original declarations restored. Cached build, dual13-image26hash, capture/
layout, service42, guest NetAPI, VDD contract and live debug/export pass.
Census1325files,777exact,788nonerror same/537different,7784points/135222lines;
delta0files/+1exact/+1same/-1different/+11points/-1266lines.1262XP/63OpenNT.
N02117/17 complete; overall57/159, S15 9/14. Six mirror files completed since
P26 Full24/24. Next N022 USER client in DIR order; original guest unchanged.

P30-P51 N022 deliveries remain in the indexed
[S15 evidence directory](../etc/evidence/t485-s1/README.md), with their individual
source identities, ABI decisions, publication hashes, tests and full census.
Full checkpoints in this range are P31, P36, P42 and P51; P61 superseded them.

[P52](../etc/evidence/t485-s1/n022-edmlrare.md),
[P53](../etc/evidence/t485-s1/n022-edslrare.md),
[P54](../etc/evidence/t485-s1/n022-globals-h.md) and
[P55](../etc/evidence/t485-s1/n022-hdata.md) delivered runtimes61-64;
linked receipts retain full source, ABI, package, tests and mirror census.

[P56 ListBox dispatch](../etc/evidence/t485-s1/n022-lb1.md) delivered runtime65;
its receipt retains original layout/allocation/event dependencies, all targeted
results, paired-header baselines and complete mirror census.

[P57 ListBox data operations](../etc/evidence/t485-s1/n022-lboxctl1.md)
delivered runtime66; its receipt retains all tests, baseline and census.
[P58 ListBox interaction](../etc/evidence/t485-s1/n022-lboxctl2.md) delivered
runtime67; its receipt retains the approved RIT boundary, ABI, all tests and census.
[P59 multi-column ListBox](../etc/evidence/t485-s1/n022-lboxmult.md) delivered
runtime68; its receipt retains byte identity, real multi-column scrolling and census.
[P60 ListBox auxiliary operations](../etc/evidence/t485-s1/n022-lboxrare.md)
delivered runtime69; its receipt retains source identity, tests and census.
[P61 directory ListBox](../etc/evidence/t485-s1/n022-lboxctl3.md) delivered
runtime70 and Full24/24, the previous ten-file checkpoint. Its receipt retains
original exclusions, directory-order correction, census and final tests.
[P62 variable-height ListBox](../etc/evidence/t485-s1/n022-lboxvar.md) delivered
runtime71; original bytes and targeted verification retained in its receipt.
[P63 state drawing](../etc/evidence/t485-s1/n022-mngrayc.md) delivered runtime72;
its receipt retains original semantics, provider bindings and verification.
[P64 client stubs](../etc/evidence/t485-s1/n022-ntstubs-review.md) delivered
runtime73; receipt preserves restored bodies, service bindings and targeted tests.
[P65 USER resources](../etc/evidence/t485-s1/n022-res.md) delivered runtime75;
its receipt retains source/dependency identities, resource and runtime verification.
[P66 capture/lookaside](../etc/evidence/t485-s1/n022-rtlinit.md) delivered runtime77;
receipt retains verification.
[P67 USER client declarations](../etc/evidence/t485-s1/n022-usercli.md) delivered
runtime78; see receipt.
[P68 client window helpers](../etc/evidence/t485-s1/n022-winmgrc.md) delivered
runtime79 and completed N02239/39. Its receipt retains all tests and census.
[P69 diagnostic mapping and N023 audit](../etc/evidence/t485-s1/n023-ntcftxt-review.md)
delivered runtime80: cached build, dual26hash, selected compiler-line verification
and Full24/24 pass. This resolves P66's #line placement error and is the current
ten-file Full checkpoint. N023 first-file ABI review is recorded, not migrated; capture and
class-publication ABI dependencies must migrate together.
S15 10/14; overall58/159. Census1330/789/800/530/7971/66379;
all six deltas0;1307XP/23OpenNT. No S/T closure. Next N023 in DIR order.

[P70 capture macros](../etc/evidence/t485-s1/n023-ntsend.md),
[P71 registration ABI](../etc/evidence/t485-s1/n023-class-abi.md),
[P72 USER declarations](../etc/evidence/t485-s1/n023-user.md) and
[P73 obsolete header retirement](../etc/evidence/t485-s1/n023-winerrp.md)
delivered runtimes81/82/86/87 and completed N0234/4. Their receipts retain
source identities, paired ABI changes, all initial failures and final tests,
dual-publication hashes and complete per-commit census/deltas. S15 reached
11/14, overall59/159; seven files completed since P69 Full24/24.

[P74 class registration](../etc/evidence/t485-s1/n024-class.md) delivered
runtime88: XP registration/lookup/removal, original class tables and paired
providers; cached build, dual26hash, four class/CCPU owner cases and real Win16
controls pass. Receipt retains the corrected non-Win16 test expectation.
Census1332/792/803/529/7948/64273; delta0/0/0/0/+12/-438;
1314XP/18OpenNT. Original LF recovery expands the former whole-file diff.
N024 first file delivered; S15 11/14, overall59/159. Eight completed files
since P69 Full24/24. Next cleanup.c; no S/T closure.

[P75 XP module/task cleanup](../etc/evidence/t485-s1/n024-cleanup-review.md) delivered
runtime91 Full24/24; its receipt retains all tests, corrections and census.

[P76 XP WOW registration](../etc/evidence/t485-s1/n024-exitwin.md) delivered runtime92;
its receipt retains all tests and census.

[P77 XP window data](../etc/evidence/t485-s1/n024-getset.md) delivered runtime93;
its receipt retains complete tests, failures, corrections and census.

[P78 XP profile](../etc/evidence/t485-s1/n024-profile.md) delivered runtime94;
its receipt retains complete tests, corrections and census.

[P79 XP task initialization](../etc/evidence/t485-s1/n024-queue.md) delivered
runtime95; its receipt retains all tests, corrections and census.

[P80 XP message tables](../etc/evidence/t485-s1/n024-server.md) delivered runtime96;
its receipt retains all tests, corrections and census.

[P81 XP task scheduler](../etc/evidence/t485-s1/n024-taskman.md) delivered
runtime97; its receipt retains all tests, original source identities and census.

[P82 XP kernel declarations](../etc/evidence/t485-s1/n024-userk.md) verified
runtime98: cached full build, dual26hash, controls40/40, lifecycle/rollback,
real Win16 controls and Full24/24 pass. Preserve initial failed attempts.
Census1338/798/809/529/8148/51728; delta+6/+6/+6/0/+104/-9917;1328XP/10OpenNT.
N0249/9, S15 12/14, overall60/159; Full counter reset. Next N025.
P82 delivery records are complete; next N025. No S/T closure.

[P89 N025](../etc/evidence/t485-s1/n025.md): runtime105 cached full build,
dual26hash, native controls40/40 and genuine Win16 controls pass. userrtl.h
restores XP client declarations and retains the four local-view REBASE macros.
Census1339/800/811/528/8180/47758; delta0/0/0/0/+5/-330;1336XP/3OpenNT.
N0258/10, overall60/159, S15 12/14. Eight files since P82 Full including the
paired client owner. Next winmgr.c in DIR order. No S/T closure.
## S14 Closure Record

S14 completed N004-N012; final runtime12 Full24/24 and clean delivery admitted
S15. [Closure](../etc/evidence/t485-s1/s14-closure.md) and
[archived detailed status](../etc/evidence/t485-s1/s14-status-history.md)
retain all per-P results, counterexamples and approved boundaries.

## S13 Closure Record

[Closure](../etc/evidence/t485-h-repair/s13-closure.md) and
[receipt](../etc/evidence/t485-h-repair/s13-closure.json) retain the1039-file
hash audit, frozen diff dispositions, corrected historical totals, published
package equivalence and Full evidence. Win95 exit-one remains a boundary.

## S1 Closure Record

[Inventory, media qualification, H001 ABI evidence and rollback](../etc/evidence/t485-s1/README.md).

## S2 Closure Record

[H001-H008 delivery](../etc/evidence/t485-s1/h005-h008.md).

## S3 Closure Record

[H009-H022 delivery](../etc/evidence/t485-s1/h022.md).

## S4 Closure Record

[H023-H036 closure](../etc/evidence/t485-s1/s4-closure.md).

## S5 Closure Record

[S5 P4 repair and closure](../etc/evidence/t485-s1/s5-p4-dpmi-ownership.md).

## S6 Closure Record

[S6 selector ownership repair](../etc/evidence/t485-h-repair/s6-selector-investigation.md).

## S7 Closure Record

[S7 closure](../etc/evidence/t485-h-repair/s7-progress.md).

## S8 Closure Record

[S8 progress](../etc/evidence/t485-h-repair/s8-progress.md).

## S9 Closure Record

[S9 closure](../etc/evidence/t485-h-repair/s9-progress.md).

## S10 Closure Record

[S10 progress](../etc/evidence/t485-h-repair/s10-progress.md).

## S11 Closure Record

[S11 closure](../etc/evidence/t485-h-repair/s11-progress.md).

## S12 Closure Record

[S12 list](../etc/evidence/t485-h-repair/s12-progress.md).

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Recent Governance

S14 N004 restores the required governance section omitted by the prior
status compaction; the current delivery rechecks structure and links.

## Current Technical Baseline

Version0.0.485 XP migration retains selected CCPU, immutable guest and thirteen
host images including NTVDMD.DLL, VCDEX.DLL and VJOY.DLL. Source baseline is
XP SP1; approved retail reference is English x86 Professional SP3. The owner
approved local source use and ordinary delivery. Required dependencies may be
imported with the current directory; independent XP additions belong to the
next T. Source/media provenance and original notice obligations remain.
