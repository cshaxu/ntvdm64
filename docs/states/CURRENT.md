# Project Status

## Current Work

**Active: M0 T485 S14** (Ordinary Mode). Resume N004-N012 after
[S13 closure](../etc/evidence/t485-h-repair/s13-closure.md).
Migration delivered39/159 directories: H001-H036 and N001-N003.
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

0/9 directories delivered in this S. Next N004:
`src/non-mvdm/base/win32/client/vdm.c` (one existing file).
Three-way inventory says both project and XP changed from OpenNT.
Audit all existing retained adaptations, actual callers and XP changes before
editing. N004 is the40/159 Full checkpoint, not yet delivered.

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

## Previous Delivery Evidence

- [S1 inventory and media](../etc/evidence/t485-s1/README.md).
- [S2 H001-H008](../etc/evidence/t485-s1/h005-h008.md).
- [S3 H009-H022](../etc/evidence/t485-s1/h022.md).
- [S4 H023-H036](../etc/evidence/t485-s1/s4-closure.md).
- [S5 N001-N003 and startup repairs](../etc/evidence/t485-s1/s5-p4-dpmi-ownership.md).
- [S6 selector](../etc/evidence/t485-h-repair/s6-selector-investigation.md).
- [S7 structural repairs](../etc/evidence/t485-h-repair/s7-progress.md).
- [S8 format restoration](../etc/evidence/t485-h-repair/s8-progress.md).
- [S9 CPU boundaries](../etc/evidence/t485-h-repair/s9-progress.md).
- [S10 input/display](../etc/evidence/t485-h-repair/s10-progress.md).
- [S11 WOW](../etc/evidence/t485-h-repair/s11-progress.md).
- [S12 COMMAND/DEM/redirector](../etc/evidence/t485-h-repair/s12-progress.md).
- [S13 detailed attempts](../etc/evidence/t485-h-repair/s13-progress.md).

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Current Technical Baseline

Version0.0.485 XP migration retains selected CCPU, immutable guest and thirteen
host images including NTVDMD.DLL, VCDEX.DLL and VJOY.DLL. Source baseline is
XP SP1; approved retail reference is English x86 Professional SP3. The owner
approved local source use and ordinary delivery. Required dependencies may be
imported with the current directory; independent XP additions belong to the
next T. Source/media provenance and original notice obligations remain.
