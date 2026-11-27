# Project Status

## Current Work

**Active: M0 T485 S15** (Ordinary Mode), N013-N026.
[S14 closure](../etc/evidence/t485-s1/s14-closure.md): N004-N012 delivered;
overall57/159 directories, H001-H036 and N001-N021. S15 runtime32 is the current
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

9/14 directories delivered. P1 N013 and P2 N014 are complete; directory50
Full24/24 passed. [P3 N015](../etc/evidence/t485-s1/n015.md) completed the five
DDK declarations with cached build, runtime3 dual publication, lifecycle,
guest NetAPI and all32 Video7 modes. See its receipt for the observer retry and historical census.

[P4 N016 first-five](../etc/evidence/t485-s1/n016-first-five.md) delivered the
five original declarations and paired Console ABI. Runtime4 dual publication,
NetAPI and Full24/24 passed; its detailed record retains historical counters.

[P5 N016 positions6-11](../etc/evidence/t485-s1/n016-packing-smb.md) delivered
packing/SMB/tstr with21necessary #line differences. Runtime5 cached build,
dual publication, RAP11/11, lifecycle and NetAPI passed. Detailed census and
LF block-count explanation remain in that receipt.

[P6 N016 VDM](../etc/evidence/t485-s1/n016-vdm.md) delivered XP VDM pointer
ABI and relocated winbase template. Runtime6 cached build, dual publication,
TIB/ABI, guest NetAPI and Full24/24 passed without retry. Per-file census and
baseline transitions remain in its receipt.

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

[P19 N020 WOW USER](../etc/evidence/t485-s1/n020-wowuser.md) delivered XP
registration declarations; three evidenced legacy USER ABI groups remain.
Cached build, runtime19 dual13-image26hash publication, registration/window
lifecycle and Full24/24 passed. N0207/7 complete; overall56/159, S15 8/14.
Ten-file checkpoint resets to zero. Next N021. Win95 exit1 boundary retained.
Census1318files,767exact,777nonerror same/541different,7731points/156836lines;
delta0files,0exact,0same,0different,+4points/-518lines.1241XP/77OpenNT.

[P20 N021 first-three](../etc/evidence/t485-s1/n021-first.md) delivered:
callconv remains XP-exact; devioctl/dlcapi restore XP with zero nonerror diff.
Cached build, runtime20 dual13-image26hash publication, RAP11/11, lifecycle
and real guest NetAPI passed. N0213/17; next nt.h. Overall56/159, S15 8/14.
Census1318files,768exact,779nonerror same/539different,7729points/154750lines;
delta0files,+1exact,+2same/-2different,-2points/-2086lines.1243XP/75OpenNT.
Three reviewed files since P19 Full24/24; no guest change.

[P21 N021 NT entry](../etc/evidence/t485-s1/n021-nt.md) delivered:
nt.h XP framework plus six exact prerequisite headers; SDK and local RTL
boundaries retained. Cached build, runtime21 dual13-image26hash publication,
Full24/24 passed. N0214/17; next nt_vdd.h. Ten-file checkpoint resets to zero.
Overall56/159, S15 8/14. Census1324files,774exact,785nonerror same/539different,
7741points/154642lines; delta+6files,+6exact,+6same,0different,+12points/-108lines.
1250XP/74OpenNT. LPC and executive entry limitations recorded for paired migration.

[P22 N021 VDD](../etc/evidence/t485-s1/n021-vdd.md) delivered XP declarations:
nt_vdd.h2points/4lines; paired DPMI macro override adds1line. Cached build,
runtime22 dual13-image26hash publication, BIOS123, VDD/IOS contracts and real
VDD/MEM exports/lifecycle passed. Two missing fixture boundaries corrected;
initial failures retained. N0215/17; next ntcsrdll.h. Overall56/159, S15 8/14.
Census1324files,774exact,785nonerror same/539different,7742points/154167lines;
delta0files,0exact,0same,0different,+1point/-475lines.1251XP/73OpenNT.
Two changed mirror files since P21 Full24/24; no guest change.

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

[P27 N021 RTL](../etc/evidence/t485-s1/n021-rtl.md) delivered runtime28:
full cached build, dual13-image26hash, x86/x64 capture/layout, service42 and
actual guest NetAPI passed. XP RTL restored with byte-exact paired string/
buffer header; SDK boundaries and original logical lines retained. Census
1325files,776exact,787nonerror same/538different,7766points/138569lines;
delta+1file/+1exact/+1same/0different/+16points/-12245lines.1258XP/67OpenNT.
N02113/17; next nturtl.h. Overall56/159 and S15 8/14 unchanged. Two mirror
files completed since P26 Full24/24. Original guest unchanged.

[P28 N021 user RTL](../etc/evidence/t485-s1/n021-user-rtl.md) delivered runtime29:
XP declarations restored; eight proven SDK duplicate type omissions retained.
Cached full build, dual13-image26hash, x86/x64 capture/layout, service42 and
actual guest NetAPI passed. Census1325files,776exact,787nonerror same/538
different,7773points/136488lines; delta0files/0exact/0same/0different,
+7points/-2081lines.1259XP/66OpenNT. N02114/17; next ptypes32.h.
Overall56/159 and S15 8/14 unchanged. Three mirror files completed since
P26 Full24/24. Original guest unchanged.

[P29 N021 final SDK](../etc/evidence/t485-s1/n021-final.md) delivered runtime30:
ptypes32 exact XP; VDD stdcall boundaries/exclusions retained; debugger
original declarations restored. Cached build, dual13-image26hash, capture/
layout, service42, guest NetAPI, VDD contract and live debug/export pass.
Census1325files,777exact,788nonerror same/537different,7784points/135222lines;
delta0files/+1exact/+1same/-1different/+11points/-1266lines.1262XP/63OpenNT.
N02117/17 complete; overall57/159, S15 9/14. Six mirror files completed since
P26 Full24/24. Next N022 USER client in DIR order; original guest unchanged.

[P30 N022 button](../etc/evidence/t485-s1/n022-button.md) delivers XP button,
paired layout/message prerequisites and native UI-state binding. Cached full
build, runtime31 dual13-image26hash, controls38/38 and real SOL Options pass.
Census1325files,777exact,788nonerror same/537different,7798points/131529lines;
delta0files/0exact/0same/0different,+14points/-3693lines.1263XP/62OpenNT.
N0221/39 complete; overall57/159, S15 9/14. Nine mirror files since P26 Full24/24;
next cldib.c triggers Full. Paired user.h/server.c remain OpenNT until migrated.
Original guest unchanged.

[P31 N022 bitmap](../etc/evidence/t485-s1/n022-dib.md) delivered runtime32:
XP scaling exception handling and rounded palette conversion retained;
only the existing host include differs (1point/2lines, -1290lines).
Cached build, dual13-image26hash publication, native bitmap/registration
checks and Full24/24 passed. N0222/39; overall57/159; S15 9/14.
Census1325files,777exact,788nonerror same/537different,7798points/130239lines;
delta0files/0exact/0same/0different/0points/-1290lines.1264XP/61OpenNT.
Ten-file checkpoint resets to zero. Next clglobal.c, in DIR order.

## S14 Closure Record

S14 completed N004-N012; final runtime12 Full24/24 and clean delivery admitted
S15. [Closure](../etc/evidence/t485-s1/s14-closure.md) and
[archived detailed status](../etc/evidence/t485-s1/s14-status-history.md)
retain all per-P results, counterexamples and approved boundaries.

## S13 Closure Record

[Closure](../etc/evidence/t485-h-repair/s13-closure.md) and
[receipt](../etc/evidence/t485-h-repair/s13-closure.json):1039file hashes verified,
4550frozen IDs reconciled;1598restored,1215residual,1737superseded by explicit
final-file maps. Pending5056records comprise1327semantic/binding and3729line
records; none is a claim of blanket owner approval. Current byte-sensitive
nonerror census7322points/12851lines versus fixed snapshot6198/548853.
CRLF restoration exposes separated blocks; this is not a block-count reduction.
S12 old cumulative1394lines corrects to1390 by per-file recount, no source edit.

S13 P8 cached full build, dual13-image26hash publication and61CPU cases pass.
Runtime2 differs from Full-tested S12 runtime14 only by package identity and
named PE timestamps through the P7/P8 byte comparisons. Retain Full23/23;
no new runtime2 Full execution is claimed. Actual Win3.1 standard/enhanced and
Win95 desktops pass; both enhanced Alt+Enter roundtrips preserve one KVM window.
Win95 known exit-one boundary remains explicit. Product4a849ef9e is pushed.

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
