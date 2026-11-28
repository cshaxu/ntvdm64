# Project Status

## Current Work

**Active: M0 T485 S15** (Ordinary Mode), N013-N026.
[S14 closure](../etc/evidence/t485-s1/s14-closure.md): N004-N012 delivered;
overall57/159 directories, H001-H036 and N001-N021. S15 runtime65 is the current
published package. The [working plan](../etc/operations/m0-t485-h-repair-plan.md)
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

9/14 directories delivered. P1 N013 and P2 N014 completed; directory50
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

[P21 N021 NT entry](../etc/evidence/t485-s1/n021-nt.md) delivered:
nt.h XP framework plus six exact prerequisite headers; SDK and local RTL
boundaries retained. Cached build, runtime21 dual13-image26hash publication,
Full24/24 passed. N0214/17; next nt_vdd.h. Ten-file checkpoint resets to zero.
Overall56/159, S15 8/14. Census1324files,774exact,785nonerror same/539different,
7741points/154642lines; delta+6files,+6exact,+6same,0different,+12points/-108lines.
1250XP/74OpenNT. LPC and executive entry limitations recorded for paired migration.

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
correction, all attempts and census. This is the latest Full checkpoint.

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

[P30 N022 button](../etc/evidence/t485-s1/n022-button.md) delivered runtime31;
its receipt retains the paired ABI, tests, census and checkpoint counts.

[P31 N022 bitmap](../etc/evidence/t485-s1/n022-dib.md),
[P32 N022 globals](../etc/evidence/t485-s1/n022-globals.md) and
[P33 N022 menu](../etc/evidence/t485-s1/n022-menu.md) delivered.
Their receipts retain full package identities, tests, per-file/global census,
failed attempts and source boundaries; P31 includes Full24/24.

[P34 N022 messages](../etc/evidence/t485-s1/n022-message.md) and
[P35 N022 resources](../etc/evidence/t485-s1/n022-resources.md) are delivered.
Their receipts retain original ABI, package identities, tests, initial failures,
per-file/global census and original baseline transitions.

[P36 N022 combo](../etc/evidence/t485-s1/n022-combo.md) delivered runtime41,
cached build, dual publication, controls/lifecycle/SOL and Full24/24. This is
that Full checkpoint; its receipt retains all census and initial failures.

[P37 combo initialization](../etc/evidence/t485-s1/n022-comboini.md) and
[P38 combo directory](../etc/evidence/t485-s1/n022-combodir.md) are delivered.
Their receipts retain source identities, ordered-DIR correction, package/tests,
complete census and checkpoint counts. No guest changes.

[P39 dialog creation](../etc/evidence/t485-s1/n022-dlgbegin-review.md) and
[P40 dialog ending](../etc/evidence/t485-s1/n022-dlgend.md) delivered runtime44/45.
Their receipts preserve source identities, complete census, initial failures
and final build/publication/control/lifecycle/SOL evidence. No guest changes.

[P41 N022 dialog manager](../etc/evidence/t485-s1/n022-dlgmgr.md) delivered
runtime46 and its recorded targeted gates; its receipt retains full census.

[P42 N022 dialog keyboard](../etc/evidence/t485-s1/n022-dlgmgr2.md) delivers
runtime47: complete XP keyboard navigation and existing callback lifetimes.
Cached build, dual13-image26hash, controls40/40, real CCPU window-borrow,
SOL Options and Full24/24 passed. Initial fixture link failure retained.
N02213/39; overall57/159; S15 9/14. Census1329files,782exact,793nonerror same/
536different,7797points/117070lines; delta0files/0exact/0same/0different/
-25points/-36lines.1279XP/50OpenNT. P42 is the new ten-file Full checkpoint.
[P43 dialog navigation](../etc/evidence/t485-s1/n022-dlgmgrc.md) and
[P44 mnemonics](../etc/evidence/t485-s1/n022-dmmnem.md) delivered runtime48/49,
with cached build, dual publication and targeted gates; their linked receipts
retain full source, census and runtime evidence.

[P45 N022 control drawing](../etc/evidence/t485-s1/n022-draw.md) delivered
runtime50 with draw.c byte-identical to XP. Its receipt retains full census,
source identity, dual publication and targeted runtime verification.

[P46 XP text drawing](../etc/evidence/t485-s1/n022-drawtext.md) delivered
runtime54; its receipt retains client/RTL source identities, native GDI
bindings, package/tests, full census and initial failure history.

[P47 EDIT rare operations](../etc/evidence/t485-s1/n022-edecrare.md) delivered
runtime55; its receipt retains original identities, paired ABI, initial fixture
failure, final publication/targeted verification and full mirror census.

[P48 EDIT core](../etc/evidence/t485-s1/n022-editec.md) delivered runtime57;
its receipt retains XP algorithms, IME boundary, package/tests and census.

[P49 multiline EDIT](../etc/evidence/t485-s1/n022-editml.md) delivered runtime58;
its receipt retains original algorithms, native providers, package and tests.

[P50 single-line EDIT](../etc/evidence/t485-s1/n022-editsl.md) delivered runtime59;
its receipt retains original algorithms, paired ABI, package/tests and full
mirror census. No guest change or S/T closure.

[P51 multiline creation](../etc/evidence/t485-s1/n022-edmlonce.md) delivered
runtime60 with exact XP source and Full24/24. Its receipt retains provider
binding, initial failure, final package/tests and complete mirror census.
P51 is the current ten-file Full checkpoint.

[P52 multiline auxiliary operations](../etc/evidence/t485-s1/n022-edmlrare.md)
delivered runtime61; original algorithms, paired ABI, package/tests and
complete census are retained in its receipt.

[P53 single-line auxiliary operations](../etc/evidence/t485-s1/n022-edslrare.md)
delivered runtime62; its receipt retains original identity, ABI, targeted
verification and complete mirror census.

[P54 USER global declarations](../etc/evidence/t485-s1/n022-globals-h.md)
delivered runtime63; original ABI, package, targeted results and census remain
in its receipt. No original guest change.

[P55 DDE data handles](../etc/evidence/t485-s1/n022-hdata.md) delivered runtime64;
its receipt retains the original subset, ABI, package/tests and mirror census.
Four completed mirrors since P51 Full24/24. No guest change or S/T closure.

[P56 ListBox dispatch](../etc/evidence/t485-s1/n022-lb1.md) delivers runtime65:
XP whole lb1.c with original layout/allocation/event dependencies. Cached
build, dual13-image26hash, controls40/40, CCPU window-borrow, SOL Options and
real Win16 controls pass. N02227/39; S15 9/14, overall57/159. Census1329/785/
796/533/8076/77980; delta0/0/0/0/+239/-14389.1294XP/35OpenNT.
Five mirrors since P51 Full24/24. No guest change or S/T closure.
Next lboxctl1.c whole-file migration; paired code retains OpenNT baseline.

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
