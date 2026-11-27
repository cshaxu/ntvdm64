# Project Status

## Current Work

**Active: M0 T485 S14** (Ordinary Mode). Resume N004-N012 after
[S13 closure](../etc/evidence/t485-h-repair/s13-closure.md).
Migration verified for delivery47/159 directories: H001-H036 and N001-N011.
The owner approved continuous execution; the [working plan](../etc/operations/m0-t485-h-repair-plan.md)
retains S15-S20 and all remaining migration/capability obligations.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S14, Ordinary Mode. |
| Candidate Proposal | [XP baseline migration](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md). |
| Admission And Approval | Owner continuous-execution approval; S6-S13 repair restart gate reconciled in the S13 closure. P8 product4a849ef9e is pushed; P9 records closure and this admission. |
| Objective | Migrate existing N004-N012 owners to pinned XP SP1, preserving current supported CCPU and product boundaries, with complete three-way dispositions and delivery. |
| Reference Baseline | Current product4a849ef9e/runtime2; XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89, Source/XPSP1/NT; per-file OpenNT and project identities in the S1 inventory. |
| Files And ABI Surface | N004 BaseClient VDM; N005 Base declarations; N006 BaseSrv VDM; N007 existing font media; N008 OLE thunk; N009 Netlib; N010 RAP; N011 RPC translation declarations; N012 XACTSRV. 83 existing files; only indispensable paired dependencies may lead their later batch. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [documents](../rules/DOCUMENT.md), [source policy](../etc/operations/policy/source-policy.md). |
| Verification | Each changed batch: cached full build, dual publication/hash verification, affected targeted tests, commit, push. N004 reaches40/159 and requires Full. Every S closure/every10 audited files also follows Full cadence. Retain unchanged passing Full gates; retry only failures and run unexecuted gates once. |
| Expected Markers | Pinned originals, three-way diff, retired/relocated obsolete paths, current ABI callers/providers, nonerror byte diff before/after, coherent final package, genuine guest/host acceptance and clean pushed tree. |
| Asset Needs | Existing fixed XP SP1 source and selected English Professional SP3 media; immutable guest binaries. |
| Constraints | Preserve original compile selections, #error/#line and architecture boundaries. No original guest changes/rebuilds, Computer Use, unrelated changes or unapproved large mirror increases. |
| Non-goals | No wholesale Kernel32/CSR import; no independent XP-only components or unrelated capability rewrite. |
| Reporting Requirements | Brief each batch before editing; affected files and behavior, original-relative nonerror diff points/lines versus prior OpenNT baseline, migration totals and delivery status. |
| Stop Conditions | Unsupported required ABI, unapproved material boundary or mirror growth, missing original input, unexplained regression; resolve within authority or seek bounded decision. |
| Exit Criteria | N004-N012 delivered in order, all83 existing files reconciled, paired dependencies recorded once, mandatory final-package Full evidence, commit/push and clean workspace. |
| Original Owner Request | Complete current repairs then N004-G097 migration to XP SP1 with minimal mirror diff and proper component layers; do not over-verify unchanged code. |
| Similar-Issue Sweep | XP-absent obsolete files, retained unused original code, changed compile guards, pointer/handle ABI, lost environment/exit ownership, guest package mismatch, false test pass/skip. |

## S14 Progress

8/9 directories verified for delivery in this S. [N004 BaseClient](../etc/evidence/t485-s1/n004.md)
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
Current global census1319files:byte-exact730,nonerror same732/different587,
7764points/214251lines. P9 delta:total0,exact+1,same+1,different-1,
points-1,lines-100;1 relocation and OpenNT->XP baseline switch.
1173 files use XP,146 retain OpenNT-family originals. S14 delivery8/9,
overall47/159; next N012 XACTSRV, then mandatory S14 closure Full.




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
