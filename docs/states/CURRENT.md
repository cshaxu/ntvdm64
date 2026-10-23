# Project Status

## Current Work

**Active: M0 T451 S1**

## Active Packet

### M0 T451 S1 — USER object and producer contract inventory

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T451 S1, Ordinary Mode. |
| Admission And Approval | The owner accepted and closed T450, then explicitly approved admission of the next queued T on 2026-10-23. |
| Objective | Produce the source-first, finite contract inventory for residual WOW32 C04--C07/object-C10 USER objects and PMODE32 direct consumers, separating work already closed by T447/T449 from T451-owned system/native/external-object and geometry cases. |
| Non-goals | No guest modification/rebuild, no USER-server reconstruction, no temporary provider, no broad control/dialog/message implementation, and no production behavior change before a later admitted S. |
| Reference Baseline | `6c661b579` is the owner-accepted T450 ten-image package. T447 delivered the local WND/CLS/desktop foundation; T449 delivered execution and targeted task retirement. |
| Files And ABI Surface | Read-only source, provenance, caller and test inventory; any detailed record belongs under `docs/etc/operations/` or `docs/etc/evidence/`. No new runtime ABI is admitted by S1. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), and [source policy](../etc/operations/policy/source-policy.md). |
| Verification | Trace each claimed C04--C07/object-C10 consumer to its original/provider source, current producer, authority state, update/invalidation and test. Mechanically confirm no production source/build/guest input changes in S1. |
| Expected Markers | A reconciled finite row set: direct reader, producer, identity class, authority, lifecycle/rollback rule, reuse disposition, executable evidence and exact successor S ownership. |
| Asset Needs | Existing selected original OpenNT/WOW32 sources, current x86 build graph and retained T447/T449 evidence; no external import or guest asset. |
| Reporting Requirements | Distinguish proved implementation gaps from inherited completed paths; list every unavailable/private boundary and avoid treating a trace hit as an implementation requirement. |
| Stop Conditions | Pause for owner decision if a needed residual behavior requires a new observable semantic concession, a new source import outside approved mirrors, or a cross-candidate dependency not bounded by a promotion. |
| Exit Criteria | Complete, reviewed and indexed contract ledger; no unassigned residual C04--C07/object-C10 case; verified no implementation change; commit/push the documentation delivery. |
| Original Owner Request | “可以，收口，然后准入下一个T任务。” |
| Similar-Issue Sweep | Include own-created, system/native and other-task identities; direct readers, first callback, geometry, stale reuse, nested destruction and failed creation. |
| Candidate Proposal | [WOW32 USER objects/shared view](../proposals/proposal-wow32-user-objects-shared-view-001.md). |

## Current Technical Baseline

The ten-image host package at `6c661b579` is published in both
`assets/release/` and `O:\winnt\system32`; guest media and guest configuration
remain immutable and unchanged. T450 is closed at
[its closure record](../history/m0-t450-closure.md). T451 S1 is an inventory
and design boundary only; it does not claim a new WOW32 capability.

## S1 Result Pending Owner Acceptance

The indexed [USER object contract ledger](../etc/operations/m0-t451-s1-user-object-contract-ledger.md)
reconciles ten finite rows across direct WND/CLS/MENU/ITEM readers, desktop and
system roots, read-only external objects, geometry/data publication, and
retirement.  It assigns the remaining publication, mutation and acceptance
obligations to S2--S4 without reopening T447/T449's accepted local execution
paths.  This documentation-only delivery changed no production source, build
input, guest asset, release asset or installed file.  Documentation governance
passed; commit/push is the remaining S1 P step.

## Recent M0 Closures

| Task | Disposition |
| --- | --- |
| T450 | [Owner-accepted style removal and Run16 Console-entry closure](../history/m0-t450-closure.md). |
| T449 | [Owner-accepted WOW32 execution continuation closure](../history/m0-t449-closure.md). |
| T448 | [Owner-accepted lifecycle closure](../history/m0-t448-closure.md). |

## Recent Governance

T450's completed S5 evidence is retained with its closure record. This
documentation-only transition admits T451 S1; it does not rebuild, replace or
republish the already hash-verified ten-image package.
