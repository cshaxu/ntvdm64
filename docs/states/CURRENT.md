# Project Status

## Current Work

**Active: M0 T483 S17** (Ordinary Mode). Owner authorized S16 closure and
next-S admission on 2026-11-20. Only S17 is active.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 完事了如果满足本S条件就可以收口本S，可以准入下一个S。继续逐层依赖审计，下一层为F。 |
| Identifier Mode | M0 T483 S17, Ordinary Mode. |
| Admission And Approval | Owner-authorized successor after verified S16 closure, 2026-11-20. |
| Objective | Audit and remove unapproved dependencies between distinct F product components; trace private source/header/link dependencies and actual runtime providers. Distinguish valid DLL imports and explicit protocol/callback composition. |
| Reference Baseline | S16 P2 2f6221b21; packagee6fc4605-1c7c-4e0a-b274-1c13ab6a74ce. [S16 closure](../etc/evidence/m0-t483-s16-closure.md). |
| Files And ABI Surface | F: ntcon-exe, nthook-dll, ntmon-exe, ntsrv-exe, ntvdm-exe, ntvwm-exe, run16-exe, vdmredir-dll, wow32-dll. Production compile/link inputs, callers and callback/import providers. |
| Candidate Proposal | [T483 parent](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Applicable Rules | Six-layer architecture. F may consume inner layers; distinct F components must not privately implement each other. Reviewed DLL/interface imports and callback composition are not violations by themselves. |
| Verification | Per changed complete chain: cached full x86/x64/reference build, dual11 publication/hash verification, affected targeted tests, governance, commit and push. No Full unless newly required by scope/evidence. |
| Expected Markers | DIR-ordered file ledger before modification, full caller/provider conclusions, counts separating violations from runtime peer evidence. |
| Asset Needs | Existing immutable guest fixtures and cached toolchains; no new asset import. |
| Non-goals | No addon work, original guest modification/rebuild, unrelated inner-layer reopening, broad feature repair or added queue/state machine. |
| Similar-Issue Sweep | Explicit/forced/transitive includes, search paths, live relocations, generated RPC, runtime binding and lifecycle ownership across supported product graphs. |
| Reporting Requirements | Per-file entry/exit: actual peer, disposition, candidate/confirmed/resolved counts, cumulative progress and non-error mirror delta. |
| Stop Conditions | No queue for same-thread decoupling, second state machine or hidden outer implementation forwarding. Mirror increases below5 lines and at most2 points have standing approval; larger increases need prior approval. Preserve unrelated work. |
| Exit Criteria | All F dependency candidates resolved with actual provider evidence; no unapproved same-layer implementation dependency; changed complete chains delivered by small-five. |

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

Admission only. Build the DIR-ordered F dependency inventory before product
changes. S17 implementation and verification have not yet started.

## Recent Governance

S15 P1 records owner-approved S14 closure and successor admission. The prior
status and its governance evidence are preserved in the S14 history above.

## S1 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S2 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S3 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S4 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S5 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S6 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S7 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S8 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S9 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S10 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S11 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S12 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S13 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S14 Closure Record

Owner-closed; [preserved closure and delivery evidence](../etc/evidence/m0-t483-s14-closure.md).

## S15 Closure Record

Owner-closed on2026-11-20. [Final ownership and delivery evidence](../etc/evidence/m0-t483-s15-closure.md).

## S16 Closure Record

Owner-authorized closure after verification. [S16 closure evidence](../etc/evidence/m0-t483-s16-closure.md).
