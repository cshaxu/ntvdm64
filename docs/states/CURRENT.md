# Project Status

## Current Work

**Active: M0 T485 S15** (Ordinary Mode), N013-N026.
[S14 closure](../etc/evidence/t485-s1/s14-closure.md): N004-N012 delivered;
overall49/159 directories, H001-H036 and N001-N013. S15 runtime1 is the current
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

1/14 directories delivered; next N014. Last Full checkpoint S14 P10/runtime12;
one completed file since that checkpoint. [P1 N013](../etc/evidence/t485-s1/n013.md)
relocates dlcio.h to XP net/inc, preserves declarations and two exclusions,
removes old include search paths. Cached full build, runtime1 dual13-image26hash
publication, lifecycle fixtures and real guest NetAPI passed. Current non-guest
mirror census1319files:byte-exact740,nonerror same742/different577,
7761points/196407lines. P1 delta0files/0exact/0same/0different,+1point/-1468lines;
LF restoration reveals two original-line anchors.1189 XP,130 OpenNT-family files.

## S14 Closure Record

9/9 directories verified for delivery in S14. [N004 BaseClient](../etc/evidence/t485-s1/n004.md)
and [receipt](../etc/evidence/t485-s1/n004-result.json): XP vdm.c plus required
original vdm.h; paired WOW registration field leads N005. Cached full build,
dual13-image26hash publication, targeted tests and consolidated Full23/23 pass.
First Full exposed Run16's rejection of XP's PE64 classification; one native
acceptance condition now admits both32/64 through the existing launch path.
Nine unaffected Full passes and three Console cases were retained; only failed
and unexecuted cases continued. Runtime2 is the current published package.
Nonerror vdm.c diff: OpenNT1point/6087lines -> XP16points/239lines; original LF
restoration exposes residual points. Paired basemsg.h adds0points/2lines.
S14 P1 delivered N004. [N005 Base declarations](../etc/evidence/t485-s1/n005.md)
([receipt](../etc/evidence/t485-s1/n005-result.json)) completes P2: cached full
build, dual13-image26hash publication and affected BASE/RPC/COMMAND/WOW tests
passed. Four mirror files reduce nonerror6points/983lines to0/0; ntcsrmsg.h
leads N021 only as a dependency. Runtime3 was N005's published package. N004 Full remains
its checkpoint; no repeat Full is claimed. Four files since that checkpoint.
[N006 BaseSrv VDM](../etc/evidence/t485-s1/n006.md) and
[receipt](../etc/evidence/t485-s1/n006-result.json) complete P3 verification:
XP desktop/logon shared-WOW records, real worker task/Console binding and
original request validation. Runtime6 cached-build/dual13-image26hash pass;
Full24/24 consolidates21 initial passes,2 bounded continuations and the new
SOL Game > Options modal-dialog gate. Initial CAF find-KVM failure is retained;
no full-suite rerun. Runtime6 differs from Full-tested runtime5 only in named
package-ID/PE timestamp fields after format/#line cleanup. Primary mirrors
OpenNT3points/6586lines -> XP17points/95lines; paired vdm.c drops4lines,
5 original declaration headers are byte-exact. This is the new ten-file
checkpoint. Other paired directories remain open.
[N007 system fonts](../etc/evidence/t485-s1/n007.md):42/42 actual files are
byte-exact OpenNT/XP originals, with matching complete directory membership.
No production or media change; nonerror0points/0lines. P4 documentation-only
registration retains N006 runtime6 Full24/24 without rebuild/republication.
The unchanged42-file audit uses that same checkpoint. [N008 OLE conversion](../etc/evidence/t485-s1/n008.md) completes P5 verification: XP output initialization and active-path line mapping, runtime7 cached full build and dual13-image26hash publication, real Win16 descriptor roundtrip/invalid-input/ownership pass. Nonerror OpenNT1point/5096lines -> XP22points/3936lines; LF restoration exposes residual points. One changed file since the checkpoint; no repeat Full. [N009 Netlib](../etc/evidence/t485-s1/n009.md) completes P6:10 existing sources plus required original midluser/RPC declarations; runtime8 cached full build, dual13-image26hash publication, targeted packstr and Full24/24 pass. Primary nonerror10points/7071lines -> 15points/19lines; paired header costs are separately recorded. This is the new Full checkpoint. P6 committed/pushed0262e23b1. [P7 omission repair](../etc/evidence/t485-s1/p7-omissions.md) completes
bounded H markfrom/N001-N003 follow-up:14 changed files, protected XP
ntcsrmsg/BaseClient and retained S13 closure. Runtime9 cached full build,
dual13-image26hash publication, x86/x64 capture3475 checks each and Full24/24
pass. Nonerror XP byte diff14points/16684lines -> 11points/257lines; CSR
normalized9points/500lines -> 5points/240lines. Eight modified files are XP
byte-exact. No original guest change. This is the new ten-file Full checkpoint.
[P8 N010 RAP](../etc/evidence/t485-s1/n010.md) delivered7 XP-byte-exact
sources plus2 paired declarations; runtime10 cached full build, dual13-image
26hash publication,11 RAP assertions and real guest NetAPI pass. Full remains
P7 runtime9;9 files since checkpoint. [Global mixed-baseline census](../etc/evidence/t485-s1/n010-census.json):
1319 files; byte-exact729; nonerror same731/different588;7765points/214351lines.
P8 delta: total0, exact+8, nonerror same+8/different-8,points-8,lines-7736.
[P9 N011](../etc/evidence/t485-s1/n011.md) relocates apiworke.h to XP's
public/internal/net/inc path byte-exact and removes old include search paths.
Runtime11 cached full build, dual13-image26hash publication, real guest NetAPI
and Full24/24 pass. This is the ten-file checkpoint after N010's nine files.
P9 global census1319files:byte-exact730,nonerror same732/different587,
7764points/214251lines. P9 delta:total0,exact+1,same+1,different-1,
points-1,lines-100;1 relocation and OpenNT->XP baseline switch.
1173 files use XP,146 retain OpenNT-family originals. S14 delivery8/9,
overall47/159; next N012 XACTSRV, then mandatory S14 closure Full.




[P10 N012](../etc/evidence/t485-s1/n012.md) delivers XP XACTSRV handlers and
conversion helpers; obsolete xsdata removed, original xsdebug replaces local
macros. Runtime12 cached full build, dual13-image26hash publication, RAP11/11,
enum42/42, real guest NetAPI and Full24/24 passed. Existing Win95 exit1 boundary
retained. Current global census1319files:byte-exact740,nonerror same742/different577,
7760points/197875lines; P10 delta0files,+10exact,+10same,-10different,-4points,
-16376lines.1188 XP and131 OpenNT-family originals. All N004-N012 are verified
for delivery:9/9 S14,48/159 total. [S14 closure audit](../etc/evidence/t485-s1/s14-closure.md) passed; S15 admitted.


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
