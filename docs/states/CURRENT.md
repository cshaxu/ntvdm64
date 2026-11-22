# Project Status

## Current Work

**Active: M0 T483 S20** (Ordinary Mode). Owner authorized sequential
S18-S20 execution. S19 is closed; S20 is the only active packet.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 清理 compat→non-mvdm、compat→mvdm-host、non-mvdm→mvdm-host；C拆为C1 non-mvdm、C2 mvdm-host，连续执行S18、S19、S20。 |
| Identifier Mode | M0 T483 S20, Ordinary Mode. |
| Admission And Approval | Owner accepted S17 and authorized successor admission, 2026-11-20. |
| Objective | Remove B-to-C1 private coupling through B-owned typed callbacks supplied by C1; preserve original algorithms, state ownership and synchronous lifetime. |
| Reference Baseline | S19 P29 verified packagedefd1dcf-12b6-4637-b606-fa7b8dbb49d5. [S19 closure](../etc/evidence/m0-t483-s19-closure.md). |
| Files And ABI Surface | DIR-ordered compat callers and actual non-mvdm providers; callback signatures, binding and teardown, original layouts and current product graphs. |
| Candidate Proposal | [T483 parent](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Applicable Rules | Ordered A/B/C1/C2/D/E/F architecture; historical B-to-C exceptions are S19/S20 cleanup obligations; reviewed inner-owned callbacks and cross-image ABI rules. |
| Verification | Exercise consolidated checks against current graphs and negative controls. Any production/build changes receive cached full build, dual11 publication, targeted tests, commit and push; evidence-only changes use governance/link checks without redundant product builds. |
| Expected Markers | DIR-ordered affected-file ledger, separate per-layer violation and approved runtime relationship counts, explicit coverage gaps and reproducible gate commands. |
| Asset Needs | Existing built products, compiler header records, linker maps and source evidence; no new guest assets. |
| Non-goals | No fresh mirror-hunk audit, addon work, original guest modification/rebuild, queued T admission or broad product redesign. |
| Similar-Issue Sweep | Direct/transitive/forced headers, private search paths, linked and generated providers, dynamic binding and callback ownership; distinguish named-image dependencies. |
| Reporting Requirements | Per-file findings and cumulative progress; A-F counts separately; non-error mirror point/line changes; no silent exceptions or unsupported zero claims. |
| Stop Conditions | No artificial same-thread queue, duplicate state machine or blanket exception. Mirror increases below5 lines and at most2 points, or exactly1 new point and fewer than100 added diff lines, have standing approval; larger increases require approval. Preserve unrelated work. |
| Exit Criteria | All B-to-C1 candidates have provider/lifetime dispositions; direct private implementation dependencies removed, typed callbacks verified, small-five delivered; S19 B-to-C2 zero baseline retained. |

## Current Technical Baseline

S14 production is delivered and owner accepted. Its [621-ID closure](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-final-dependency-closure.json)
and [P163 callback simplification](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-control-callbacks.md)
record B/C violations0/0 and approved callbacks D9/F3. Runtime callbacks are not
reported as absent. One necessary Console cleanup handoff remains.

## S15 Progress

[S15 final closure audit](../../artifacts/mvdm-audit/t483-d-layer-dependencies/final-closure.json):
12/12 candidate files resolved, zero pending;27 compiled D source/IDL units
covered. Ordinary outer live relocations, compiled headers and private search
roots are zero. The [DIR ledger](../../artifacts/mvdm-audit/t483-d-layer-dependencies/initial-file-inventory.json)
records each file's actual provider and disposition.

P2 removed F search roots from eight D files (80 repeated product rows).
P3 moved MIDL allocation into D, removing2 ordinary outer relocations and8
allocator rows from generated RPC evidence. Cached full x86/x64/reference,
dual11 publication, management128 assertions, real monitor RPC and COMMAND
return passed; delivered commit f7ede72b0, package80616b98-933d-4375-bdb8-f742686d4ae6.
P4 completes ownership evidence with no product change: native service-loss
and carrier/target/descendant cleanup also passed before test containment.
Non-error mirror delta throughout S15:0 points,0 lines.

Permitted runtime relationships remain explicit: one D-owned broker-death
callback slot with two F subscribers;64 generated NTSRV dispatch/rundown rows;
95 repeated named-package/path observations. None is described as absent or
silently ignored. VDMREDIR link-map coverage and generated/assembler gaps are
resolved in the final audit. S15 owner acceptance and closure recorded on2026-11-20; S16 is admitted. The independent queued T candidates retain their order and scope.

## Recent M0 Closures

- [T483 S14 closure and inherited earlier closure index](../etc/evidence/m0-t483-s14-closure.md).
- [T482 closure](../history/m0-t482-closure.md).
- [T481 closure](../history/m0-t481-closure.md).

## S16 Progress

[DIR-ordered E inventory](../../artifacts/mvdm-audit/t483-e-layer-dependencies/file-inventory.json):
13/13 files audited,0 pending (6 C implementations,5 headers,2 README files).
[Final delivery](../../artifacts/mvdm-audit/t483-e-layer-dependencies/delivery.json)
records actual callback providers, execution threads and join-before-release.
No unapproved E-to-F or frontend-base/worker-base dependency remains. Typed
product callbacks remain explicit; no claim of absent runtime peers is made.

P2 corrects2 outdated component descriptions and1 drain-contract comment.
No algorithm/ABI or guest changes; non-error mirror delta0 points/0 lines.
Cached full x86/x64/reference and dual11 publication passed. Six focused
fixtures passed, including input641, shutdown37 and VM frontend233 assertions;
real COMMAND modern EDIT/CtrlQ, CMD echo, DOS MEM and launcher return passed.
Package e6fc4605-1c7c-4e0a-b274-1c13ab6a74ce. S16 closure criteria verified and owner-authorized closure recorded;
S17 admitted. Necessary cross-thread handoffs remain documented.

## S17 Progress

[F file inventory](../../artifacts/mvdm-audit/t483-f-layer-dependencies/file-inventory.json):
14/14 audited,0 pending. P2 removes16 private search-root records across13
files. Final sibling private includes/search roots and ordinary live
relocations are0. Two DLL ABI imports and6 named process-image observations
remain explicitly reviewed loading/protocol relationships.

[Delivery](../../artifacts/mvdm-audit/t483-f-layer-dependencies/delivery.json):
cached full x86/x64/reference, dual11 publication, classification424 checks,
real bootstrap and COMMAND/EDIT/CMD/MEM return passed. No algorithm/ABI or
guest changes; non-error mirror delta0 points/0 lines. Package
733c98ae-ab34-4dad-afd4-380e0f43f0b5. S17 P2 eaa229156 is committed and pushed; owner accepted and closed S17.
[S17 closure](../etc/evidence/m0-t483-s17-closure.md) records verification.

## S18 Progress

Owner corrected scope before implementation: S18 handles C1-to-C2, then
S19 handles B-to-C2 and S20 B-to-C1. [Sequential worklist](../etc/operations/t483-compat-implementation-migration-plan.md#sequential-s-worklist).
Initial current census:29 candidate source files,11 linked relocation rows
(9 unique caller/symbol/provider relationships),11 compiled-header records,
and252 private search-root records. [DIR inventory](../../artifacts/mvdm-audit/t483-c1-c2-dependencies/file-inventory.json).
Owner clarification supersedes zero-edge target: original C1/C2 connections
are approved exceptions; OLE callback candidate withdrawn without implementation.
Only B-to-C1/C2 uses new callback interfaces in S19/S20. All29 files now
have final dispositions: private headers11->0, private search roots252->0.
Seven live relocation records represent five retained original WOW APIs.
Original USER resource callbacks are restored; no new table or queue.
Non-error mirror delta0 points/0 lines. Cached full x86/x64/reference and
dual11 publication passed, package9aab55f9-15db-4ce1-83ff-32c0c385a661.
Resource, actual Win16 OLE, SOL menu/dialog and COMMAND-return tests passed.
[Delivery record](../../artifacts/mvdm-audit/t483-c1-c2-dependencies/delivery.json).
Final governance checks passed. S18 P3 delivery includes this closure and S19 admission.

## Recent Governance

S15 P1 records owner-approved S14 closure and successor admission. The prior
status and its governance evidence are preserved in the S14 history above.

## S1 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S2 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S3 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S4 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S5 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S6 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S7 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S8 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S9 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S10 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S11 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S12 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S13 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S14 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S15 Closure Record

Owner-closed on2026-11-20. [Final ownership and delivery evidence](../etc/evidence/m0-t483-s15-closure.md).

## S16 Closure Record

Owner-authorized closure after verification. [S16 closure evidence](../etc/evidence/m0-t483-s16-closure.md).

## S17 Closure Record

Owner accepted; [S17 closure and delivery](../etc/evidence/m0-t483-s17-closure.md).

## S18 Closure Record

Owner-authorized sequential closure; [S18 verification and publication](../etc/evidence/m0-t483-s18-closure.md).

## S19 Closure Record

[S19 closure](../etc/evidence/m0-t483-s19-closure.md):38/38 files complete;
B-to-C2 direct links, compiled headers and search roots are zero. P29 small-five
is the closure delivery. Typed runtime callbacks remain explicit.

## S20 Progress

[DIR inventory](../../artifacts/mvdm-audit/t483-b-c1-dependencies/file-inventory.json):
94 files,21 completed,73 remaining. B-to-C1:55 unique/67 relocation
records,472 compiled header rows,0 private search-root rows. These are separate
metrics, not additive counts. B-to-C2 direct/header/search remains0.
DIR work remains at BC1F010/011 shared USER consumer group; three P9 text
consumers are complete, other consumers remain open. S20 cumulative non-error
mirror increment2points/+210lines; S19 retained increment+12points/+574lines.

[S20 P1 review](../../artifacts/mvdm-audit/t483-b-c1-dependencies/base-dispatch-review.json)
is implemented by [P2](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p2-delivery.json):
base_dispatch.c has zero C1 compile/link/search dependencies. B keeps the
entry/8 dispatches/wait ownership; C1 supplies two synchronous projections.
Cached full, dual11,140 assertions,5 real RPC wait cases and real Win16/COMMAND
passed. Mirror vdm.c:20points/505lines ->19points/569lines; R2736 retained.

P3-P7 removed registry, capture, classifier, BASE-client and config header
coupling. Their cached full builds, dual11 publications, targeted tests and
actual Win16/COMMAND checks are recorded in the individual delivery records:
[P3](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p3-delivery.json),
[P4](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p4-delivery.json),
[P5](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p5-delivery.json),
[P6](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p6-delivery.json),
[P7](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p7-delivery.json).
P3's unrelated full legacy fixture wire-size assertion remains a recorded
limitation; no full-fixture pass was claimed. R2737-R2741 retain provenance.

S20 P8 grouped delivery verifies BC1F008/009/023/024:10/94 files complete,
84 remain. BC1F007 duplicate ntsend include is removed; its transitive USER
layout group remains open and next DIR work continues at BC1F010/011.
B-to-C1 remains58 unique/70raw relocations; compiled header rows555->541,
private search rows20->0. B-to-C2 direct/header/search remains0. The explicit
B-owned monitor callback has C2-owned TIB storage and two synchronous operations;
F only assembles it. No duplicate state or queue. Mirror batch delta0points/+32lines,
S20 cumulative0points/+124lines (R2742/R2743). Cached full build, dual11,
59CCPU cases, classifier and actual Win16/COMMAND passed; final gates passed.
[Delivery evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p8-delivery.json).

P9/P10 text and bitmap boundaries: original LARGE_STRING and bitmap conversion
remain C-owned; B uses opaque pointer and synchronous callbacks. The verified
[procedures](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p9-delivery.json)
and [bitmap delivery](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p10-delivery.json)
retain their build, test and publication evidence (R2744/R2745).

P11 completes BC1F013/032: B owns opaque cleanup ABI and synchronous callback;
original C algorithm and private declarations remain C-owned. This removes
1direct relationship and29compiled header rows. Mirror0points/+52lines,
R2746/R2747; S20 cumulative0points/+191lines. Cached full build, dual11,
original source reconstruction, menu/task lifecycle, actual Win16/COMMAND
and owner-requested Full all passed. Full fixture initialization and link/input
omissions were repaired without weakening assertions or changing products.
[P11 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p11-delivery.json).

P12 completes BC1F014/045: one direct relation removed via B-owned synchronous
callback to original C window procedure. Header rows remain487: BC1F052 still
reaches original userrtl and remains open. Mirror+2points/+8lines, R2748/2749;
cumulative+2points/+199lines. Cached full, dual11, control group and actual
Win16/COMMAND passed; [P12 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p12-delivery.json).

P13 completes BC1F015: B edit-memory no longer imports C globals.h; the original
selected edit layout owns its macros. B-C1 header rows487->482, direct55 remains.
Cached full, dual11, edit/dialog control group and actual Win16/COMMAND passed.
Mirror0points/+3lines R2750; S20 cumulative+2points/+202lines.
[P13 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p13-delivery.json).

P14 completes BC1F017: SDK oleidl.h supplies OBJECTDESCRIPTOR with x86 size
and field-offset assertions, replacing duplicated declaration and private nt.h.
B-C1 headers482->476; direct55 unchanged. Mirror0points/0lines. Cached full,
dual11, both actual OLE descriptor roundtrips and COMMAND return passed.
[P14 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p14-delivery.json).

P15 clears part of BC1F018: B dialog-state interface passes opaque DLG; actual
C dialog/getset layouts remain in their owners. Header rows476->472; direct55
and files21/94 remain unchanged. Cached full, dual11, dialog/control and actual
Win16/COMMAND passed. Mirror0points/+8lines R2751; cumulative+2points/+210lines.
[P15 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p15-delivery.json).

P16 owner-requested Full: cached x86/x64/reference build and dual11 publication
passed, package dd2efa9f-ff51-4e23-a11c-c6a5ce871ee2. Full selected gates
passed, including actual DOS4GW/NESticle menu and Escape hide/restore.
No source change; S20 remains active. [Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p16-full-delivery.json).

P17 completes BC1F019: removed redundant private nt.h; SDK declarations remain
available via existing inner task-order header. Actual task-order and control
fixtures, Win16/COMMAND and architecture gates passed. Cached full build and
dual11 publication passed, package 3863aa66-5e58-4175-8cbb-7938966b4d47.
Files22/94 complete; B-C1 compiled headers472->466, direct55 unchanged;
non-error mirror delta0 points/0 lines. [Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p17-delivery.json).

P18 completes BC1F020: C1 owns profile validation and original readers; B
uses typed synchronous providers bound by F before initialization. Direct
B-C1 relations55->53, compiled header rows466->460; files23/94 complete.
Non-error mirror0points/+14lines R2752; cumulative+2points/+224lines.
Cached full, dual11 and targeted task/profile/control and real Win16/COMMAND
passed; package c3f39ae3-6ab0-4d0a-8275-add061bb9ec0.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p18-delivery.json).

P19 partially addresses BC1F021: standard-class ANSI capture now uses the
C1 original capture through the existing synchronous class provider, retaining
allocation and cleanup. Direct B-C1 relations53->52, compiled header rows
460->458; files23/94 complete. Mirror0points/+9lines R2753; cumulative
+2points/+233lines. Cached full, dual11, CCPU standard-class, init/task,
control/capture and real Win16/COMMAND passed; package
38a14d52-2234-4c07-803b-b4faa394f273.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p19-delivery.json).

P20 completes BC1F022: COMMAND uses its existing synchronous service table
to call original C1 status conversion; its TEB side effect remains. Direct
B-C1 relations52->51, compiled headers458 unchanged, files24/94 complete.
Mirror0points/0lines; cumulative+2points/+233lines. Cached full, dual11,
COMMAND fault/capture and real Win16/COMMAND passed; package
1f6221da-7af1-4960-be33-42f88ec7c908.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p20-delivery.json).

P21 partially addresses BC1F026: C1 projects original button state and supplies
original lookaside release; B retains oldest-frame lifetime. B-C1 unique
direct51->50 (three relocation rows removed), compiled headers458->449;
files24/94 complete. Mirror+1point/+6lines R2754; cumulative+3points/+239lines.
Cached full, dual11, button/control, CCPU, init/task and Win16/COMMAND passed;
package 01e7dd12-e11d-4129-a353-10b6ebd63137.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p21-delivery.json).

P22 partially addresses BC1F027: removed unused private nt.h from the B
button resource owner and made the SDK declaration explicit. Original OEM
geometry layout remains open. Compiled B-C1 headers449->443; direct50 and
files24/94 complete unchanged. Mirror0points/0lines; cumulative+3points/+239lines.
Cached full, dual11, button/control and real Win16/COMMAND passed; package
c089c798-2ce2-447a-8682-fbc8f8304284.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p22-delivery.json).

P23 completes BC1F028: removed unused private nt.h from the callback-frame
lease owner; original cross-image CPU and memory ABI remains explicit. B-C1
compiled headers443->437, direct50 unchanged; files25/94 complete.
Mirror0points/0lines; cumulative+3points/+239lines. Cached full, dual11,
original callback frame, CCPU abort and real Win16/COMMAND passed; package
166fe66a-4916-4c3d-b8d5-c4ab533e7a23.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p23-delivery.json).

P24 completes BC1F029: moved the existing original class-client extraction to
WOW32, with B-owned typed synchronous provider dispatch. B-C1 direct unique
50->48 (raw 60->56), compiled headers 437->423; files 26/94 complete.
Non-error mirror delta 0 points/0 lines; cumulative +3 points/+239 lines.
Cached full build, dual11, class/control and real Win16/COMMAND targeted tests
passed; Full remains required at S20 closure. Package
f5ef3a75-1551-4baa-98b9-a93bc8b5d5c2.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p24-delivery.json).

P25 completes BC1F030: B now reaches original C1 _RegisterClassEx through
the existing synchronous class provider and no longer includes private nt.h.
B-C1 direct unique 48->47 (raw 56->55), compiled headers 423->417; files
27/94 complete. Non-error mirror delta 0 points/0 lines; cumulative +3 points/
+239 lines. Cached full build, dual11, class/lifecycle and real Win16/COMMAND
targeted tests passed; Full remains required at S20 closure. Package
02ae11a2-6061-462f-acc4-8b4b6b299978.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p25-delivery.json).

P26 partially addresses BC1F031: original C1 class lookup and unregister
are called via the existing synchronous provider; original CLS layout remains
open. B-C1 direct unique 47->44 (raw 55->52), compiled headers remain 417;
files 27/94 complete. Non-error mirror delta 0 points/0 lines; cumulative
+3 points/+239 lines. Cached full build, dual11, class/lifecycle and actual
Win16/COMMAND targeted tests passed; Full remains required at S20 closure.
Package d3f202a6-ca20-4dc0-b1f7-61c4ffc54762.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p26-delivery.json).

P27 completes BC1F033: removed unused private nt.h from client dispatch.
B-C1 direct unique remains 44, compiled headers 417->411; files 28/94
complete. Mirror delta 0 points/0 lines; cumulative +3 points/+239 lines.
Cached full build, dual11, dispatch and real Win16/COMMAND targeted tests
passed. Full remains required at S20 closure. Package
741da410-408d-4c0f-97a6-ff7c1a7cbe6e. BC1F031 original-layout dependency
remains open after a reverted incomplete experiment.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p27-delivery.json).

P28 completes BC1F034: removed unused private nt.h from the common-dialog
callback owner. B-C1 direct unique remains 44, compiled headers 411->405;
files 29/94 complete. Mirror delta 0 points/0 lines; cumulative +3 points/
+239 lines. Cached full build, dual11, common-dialog, control and real
Win16/COMMAND targeted tests passed. Full remains required at S20 closure.
Package 31e719bb-0c8a-4d4f-acd8-1010490fa3d6.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p28-delivery.json).

P29 completes BC1F035: removed unused private nt.h from common-dialog calls.
B-C1 direct unique remains 44, compiled headers 405->399; files 30/94
complete. Mirror delta 0 points/0 lines; cumulative +3 points/+239 lines.
Cached full build, dual11, common-dialog calls and real Win16/COMMAND
targeted tests passed. Full remains required at S20 closure. Package
9d83662e-1899-4814-93a9-c758456d0e6f.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p29-delivery.json).

P30 completes BC1F036: relocated the original brush body to WOW32 and
uses a synchronous provider while preserving the B borrow scope. B-C1
direct unique remains 44, compiled headers 399->391; files 31/94 complete.
BC1F037 remains open. Mirror delta 0 points/0 lines; cumulative +3 points/
+239 lines. Cached full build, dual11, brush/control and real Win16/COMMAND
targeted tests passed. Full remains required at S20 closure. Package
aa18ab7b-8a72-48b9-85a2-2d6b693de5fd.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p30-delivery.json).

P31 completes BC1F037: WOW32 reads original WND fields; B uses HWND and
opaque WND pointers with the existing borrow/cleanup scope. B-C1 direct
unique remains 44, compiled headers 391->383; files 32/94 complete. Mirror
delta 0 points/0 lines; cumulative +3 points/+239 lines. Cached full build,
dual11, brush/control and real Win16/COMMAND targeted tests passed. Full
remains required at S20 closure. Package 46959985-4704-4f7f-9c9f-38f3b840bae3.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p31-delivery.json).

P32 partially addresses BC1F038: B calls original C1 ClientDrawState via
a synchronous provider. Original button atlas layout remains open with
BC1F027. B-C1 direct unique 44->43 (raw 52->51), compiled headers 383->377;
files 32/94 complete. Mirror delta 0 points/0 lines; cumulative +3 points/
+239 lines. Cached full build, dual11, drawing/control and real Win16/COMMAND
targeted tests passed. Full remains required at S20 closure. Package
8fa9c6b7-9381-4c06-a20a-ed179130a7e2.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p32-delivery.json).

P33 completes BC1F039: original control-layout size calculation is now
WOW32-owned and supplied to B through a synchronous provider. B-C1 direct
unique remains 43, compiled headers 377->367; files 33/94 complete. Mirror
delta 0 points/0 lines; cumulative +3 points/+239 lines. Cached full build,
dual11, control extents and real Win16/COMMAND targeted tests passed. Full
remains required at S20 closure. Package 2ef4b65b-c605-4daa-86ba-627c22f70a48.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p33-delivery.json).

P34 completes BC1F040: the extracted original FNID body is WOW32-owned;
C1 uses a B synchronous entry and WOW32 supplies the provider. B-C1 direct
unique remains 43, compiled headers 367->357; files 34/94 complete. Mirror
delta 0 points/0 lines; cumulative +3 points/+239 lines. Cached full build,
dual11, FNID/control and real Win16/COMMAND targeted tests passed. Full
remains required at S20 closure. Package 65dc4acd-f464-4b05-b561-b7370c7fea70.
[Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p34-delivery.json).
