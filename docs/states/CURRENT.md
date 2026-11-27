# Project Status

## Current Work

**Active: M0 T485 S15** (Ordinary Mode), N013-N026.
[S14 closure](../etc/evidence/t485-s1/s14-closure.md): N004-N012 delivered;
overall56/159 directories, H001-H036 and N001-N020. S15 runtime22 is the current
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

8/14 directories delivered. P1 N013 and P2 N014 are complete; directory50
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

[P10 N017 final-six](../etc/evidence/t485-s1/n017-tail.md) delivered: all six
original XP paths/declarations restored, four byte-exact and two only#error.
Cached build, runtime10 dual13-image26hash publication, RAP11/11, lifecycle
and real guest NetAPI passed. Six files since P9 Full24/24; no repeat Full.
N017 is19/19 complete; overall53/159 and S15 5/14. Next N018.
Current census1315files,760exact,768nonerror same/547different,
7748points/164027lines; delta0files,+4exact,+6same,-6different,-6points/-3856lines.
1229XP/86OpenNT. P9 census remains historical.

[P11 N018](../etc/evidence/t485-s1/n018.md) delivered: tstring.h byte-exact XP,
no functional adaptation/exclusion. Cached build, runtime11 dual13-image26hash
publication, RAP11/11, both lifecycle fixtures and guest NetAPI passed.
Seven files since P9 Full24/24; no repeat Full. N018 is1/1 complete;
overall54/159 and S15 6/14. Next N019. Current census1315files,761exact,
769nonerror same/546different,7747points/163505lines;
delta0files,+1exact,+1same,-1different,-1point/-522lines.1230XP/85OpenNT.

[P12 N019](../etc/evidence/t485-s1/n019.md) delivered: wowshlp.h moved to its
XP Base path byte-exact; wowcmndg.h remains exact. Fixed the existing host
Shell binding's two-argument call to match the XP three-argument WORD callback.
Cached full build, runtime12 dual13-image26hash publication, four Shell cases
and real WINMINE window passed. Initial include-path failure and native-provider
inner-exception limitation retained in evidence. Nine inventory files reviewed
since P9 Full24/24; no repeat Full. Overall55/159, S15 7/14. Next N020.
Current census1315files,762exact,770nonerror same/545different,
7746points/163445lines; delta0files,+1exact,+1same,-1different,-1point/-60lines.
1231XP/84OpenNT. No #error/#line change or original guest modification.

[P13 N020 first-file](../etc/evidence/t485-s1/n020-first.md) delivered:
newres.h byte-exact XP at windows/core/w32inc; clres.c include follows with
unchanged OpenNT baseline/diff. Cached build, runtime13 dual13-image26hash
publication and Full24/24 passed. Ten-file checkpoint resets to zero.
N0201/7 delivered, six remain; overall55/159 and S15 7/14. Next pif.h.
Current census1315files,763exact,771nonerror same/544different,
7745points/163285lines; delta0files,+1exact,+1same,-1different,-1point/-160lines.
1232XP/83OpenNT. No #error/#line change. Win95 exit1 boundary remains.

[P14 N020 PIF](../etc/evidence/t485-s1/n020-pif.md) delivered original XP
Shell PIF declarations, seven existing excluded arms and five #line anchors.
Cached full host/addon build, runtime14 dual13-image26hash and PIF publication,
existing byte-contract/hash/negative/root-update tests passed. N0202/7 done,
five remain; overall55/159 and S15 7/14. One file since P13 Full24/24.
Current census1315files,763exact,771nonerror same/544different,
7735points/163271lines; delta0files,0exact,0same,0different,-10points/-14lines.
1233XP/82OpenNT. Next winconp.h; original guest unchanged.

[P15 N020 Console](../etc/evidence/t485-s1/n020-console.md) delivered complete
XP winconp.h declarations. Three existing one-line #error substitutions preserve
physical original lines; no nonerror diff. Cached build, runtime15 dual13-image
26hash publication, font/input-batch and seven actual COMMAND cases passed.
N0203/7 done, four remain; overall55/159 and S15 7/14. Next winddi.h.
Current census1315files,763exact,772nonerror same/543different,
7728points/163264lines; delta0files,0exact,+1same,-1different,-7points/-7lines.
1234XP/81OpenNT. Two files since P13 Full24/24; original guest unchanged.

[P16 N020 DDI](../etc/evidence/t485-s1/n020-ddi.md) delivered complete XP OAK
winddi.h, existing FLOATOBJ exclusion and two #line anchors. Cached build,
runtime16 dual13-image26hash publication, output/font contracts, real Win16
GDI marker and WINMINE/WRITE windows passed. N0204/7 done, three remain;
overall55/159 and S15 7/14 unchanged. Next wingdip.h.
Current census1315files,763exact,772nonerror same/543different,
7729points/158246lines; delta0files,0exact,0same,0different,+1point/-5018lines.
1235XP/80OpenNT. LF restoration exposes two #line blocks rather than one
whole-file difference. Three files since P13 Full24/24; guest unchanged.

[P17 N020 GDI](../etc/evidence/t485-s1/n020-gdi.md) delivered wingdip.h with
zero nonerror diff and three required XP byte-exact PolyPatBlt headers.
Cached build, runtime17 dual13-image26hash publication, output/font contracts,
real Win16 GDI and WINMINE/WRITE windows passed. N0205/7; next wowgdip.h.
Overall55/159, S15 7/14 unchanged. Census1318files,766exact,
776nonerror same/542different,7728points/157392lines;
delta+3files,+3exact,+4same/-1different,-1point/-854lines.1239XP/79OpenNT.
Seven mirror files since P13 Full24/24 including three required dependencies.

[P18 N020 WOW GDI](../etc/evidence/t485-s1/n020-wowgdi.md) delivered complete
XP bytes with unchanged declarations. Cached build, runtime18 dual13-image
26hash publication, output/font contracts, real Win16 GDI and WOW windows
passed. N0206/7; next wowuserp.h. Overall55/159 and S15 7/14 unchanged.
Census1318files,767exact,777nonerror same/541different,7727points/157354lines;
delta0files,+1exact,+1same/-1different,-1point/-38lines.1240XP/78OpenNT.
Eight files since P13 Full24/24; original guest unchanged.

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
