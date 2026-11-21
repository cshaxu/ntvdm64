# Project Status

## Current Work

**Active: M0 T483 S19** (Ordinary Mode). Owner authorized sequential
S18-S20 execution. S18 verification is complete; S19 is the only active packet.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 清理 compat→non-mvdm、compat→mvdm-host、non-mvdm→mvdm-host；C拆为C1 non-mvdm、C2 mvdm-host，连续执行S18、S19、S20。 |
| Identifier Mode | M0 T483 S19, Ordinary Mode. |
| Admission And Approval | Owner accepted S17 and authorized successor admission, 2026-11-20. |
| Objective | Remove B-to-C2 private coupling through B-owned typed callbacks supplied by C2; preserve original algorithms, state ownership and synchronous lifetime. |
| Reference Baseline | S18 P3 verified package9aab55f9-15db-4ce1-83ff-32c0c385a661. [S18 closure](../etc/evidence/m0-t483-s18-closure.md). |
| Files And ABI Surface | DIR-ordered compat callers and actual mvdm-host providers; callback signatures, binding and teardown, original layouts and current product graphs. |
| Candidate Proposal | [T483 parent](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Applicable Rules | Ordered A/B/C1/C2/D/E/F architecture; historical B-to-C exceptions are S19/S20 cleanup obligations; reviewed inner-owned callbacks and cross-image ABI rules. |
| Verification | Exercise consolidated checks against current graphs and negative controls. Any production/build changes receive cached full build, dual11 publication, targeted tests, commit and push; evidence-only changes use governance/link checks without redundant product builds. |
| Expected Markers | DIR-ordered affected-file ledger, separate per-layer violation and approved runtime relationship counts, explicit coverage gaps and reproducible gate commands. |
| Asset Needs | Existing built products, compiler header records, linker maps and source evidence; no new guest assets. |
| Non-goals | No fresh mirror-hunk audit, addon work, original guest modification/rebuild, queued T admission or broad product redesign. |
| Similar-Issue Sweep | Direct/transitive/forced headers, private search paths, linked and generated providers, dynamic binding and callback ownership; distinguish named-image dependencies. |
| Reporting Requirements | Per-file findings and cumulative progress; A-F counts separately; non-error mirror point/line changes; no silent exceptions or unsupported zero claims. |
| Stop Conditions | No artificial same-thread queue, duplicate state machine or blanket exception. Mirror increases below5 lines and at most2 points have standing approval; larger increases require approval. Preserve unrelated work. |
| Exit Criteria | All B-to-C2 candidates have provider/lifetime dispositions; direct private implementation dependencies removed, typed callbacks verified, small-five delivered before S20 admission. |

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

## S19 Progress

[DIR inventory](../../artifacts/mvdm-audit/t483-b-c2-dependencies/file-inventory.json):
38 candidates,24 resolved,14 remaining. P13 cached build, dual publication and targeted runtime passed. P1 cleared debug/COMMAND
preparation; P2 cleared COMMAND capture leases; P3 clears keyboard IRQ retirement,
descriptor projection and effective-address consumers including their public header.
Unique direct B-to-C2 relationships337->235; relocation records606->254.
Compiled private C2 header rows144->111; private C2 search roots74->11.
B-to-C1 remains58 unique/70 records for S20. C1/C2 retains its five original
WOW APIs (seven relocation rows), not callback-conversion candidates.
Owner-approved P1 mirror delta+2 points/+19 lines is registered as R2721/R2722;
P2/P3/P4 mirror delta0 points/0 lines. Original C2 algorithms remain providers of
B-owned synchronous callbacks, installed before session/device initialization.
[P1](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p1-delivery.json),
[P2](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p2-delivery.json) and
[P3](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p3-delivery.json)
record cached full builds, dual11 publication, targeted native/real-CPU/guest
checks and actual provider/lifetime dispositions.
[Directory candidate](../../artifacts/mvdm-audit/t483-b-c2-dependencies/directory-provider-decision.json)
(+1 point/+22 lines) and [vector candidate](../../artifacts/mvdm-audit/t483-b-c2-dependencies/vector-provider-decision.json)
(+1 point/+87 lines) compiled with product parameters and await owner approval.
Neither is implemented or counted as a closed dependency.
[Mouse owner candidate](../../artifacts/mvdm-audit/t483-b-c2-dependencies/mouse-guest-provider-decision.json)
compiled with product parameters; +1 point/+41 lines awaits approval and is not implemented.
[Text-video candidate](../../artifacts/mvdm-audit/t483-b-c2-dependencies/text-video-provider-decision.json)
compiled; +1 point/+97 lines awaits approval. No production change.
[WOW page-domain review](../../artifacts/mvdm-audit/t483-b-c2-dependencies/wow-page-domain-provider-review.json)
identifies24 actual C2 providers; P9 implements their callbacks and preserves
the existing reclamation failure-injection contract.
[P4](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p4-delivery.json),
[P5](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p5-delivery.json),
[P6](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p6-delivery.json), and
[P7](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p7-delivery.json)
record verified printer, async pipe, mouse and termination boundaries.
Cumulative S19 mirror delta+2 points/+22 lines; no original guest changes.
[P9](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p9-delivery.json) clears
24 page-domain C2 relations through the existing CPU callback table, with no
mirror change. Real CCPU page-domain/reclaim, exact owned reads, actual Win16
SOL modal menu and COMMAND return passed; SDK declarations replace nt.h.
Original DOSX status-query inversion reproduced on the S18 control and P1;
status-query semantics are not claimed passed. No original guest changed.
[P10](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p10-delivery.json) removes
nine unused TIB C2 search roots and fifteen VM BOP direct relationships.
Existing CCPU callbacks supply registers; F lends the BIOS slot only during
startup. Real CCPU BOP rejection/result registers, notification3194 assertions
and actual COMMAND/EDIT/CMD/MEM return passed. Mirror delta0 points/0 lines.
[P11](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p11-delivery.json) clears
31 direct relationships across the eight DIR-ordered VM bridge/capture/entry/
continuation/I/O/input/notification/wake files. Original ICA, SAS, keycode and
CPU bodies remain providers. Notification3194, ingress73, capture165, actual
CCPU BOP and COMMAND return pass. Eight algorithm comparisons match after
reversing callback binding; no mirror or guest change.
[P12 clipboard delivery](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p12-clipboard-review.json)
uses a caller-supplied synchronous retirement callback. Cached full build,
dual11 publication and epoch56 assertions pass. Real Win16 transfer passes
all18 checkpoints on the shared desktop; P11 control also passes. Earlier
access-denial and foreign RadeonSoftware lock failures remain in evidence.
The native test peer waits only for a bounded identified foreign lock; no
product retry or guest modification. B-to-C2 is238. Mirror delta0 points/0 lines.
[Remaining WOW provider review](../../artifacts/mvdm-audit/t483-b-c2-dependencies/wow-remaining-provider-review.json)
records DIR28-38:39 direct C2 relationships plus private-layout obligations.
DIR28 is now delivered in P13; DIR29-38 remain source/provider evidence only.
[P13 dialog delivery](../../artifacts/mvdm-audit/t483-b-c2-dependencies/p13-delivery.json)
clears three direct C2 calls through immutable same-image callbacks bound before
original DLL attach. Nested/resource guards, actual loader, real Win16 common
and ordinary dialogs, and COMMAND return pass. Dual11 package
42cfa459-5dba-46cc-aaf8-2be639ec98ce; mirror delta0 points/0 lines.
S19 remains active; S20 not admitted.
