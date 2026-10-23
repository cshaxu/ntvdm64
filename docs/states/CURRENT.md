# Project Status

## Current Work

**Active: M0 T449 S2**, Ordinary Mode, one implementer with independent
self-review. This is the owner-accepted S2 closure-record packet at
`8d9ce29c3`; it authorizes no further S2 implementation and no successor S is
admitted yet. The owner accepted and closed T448 and admitted the queued WOW32
execution continuation. T449 resumes the unfinished T447 execution
foundations; it does not restart or declare complete the earlier work.

## Active Packet

| Field | T449 S2 — execution, messages and ordinary lifecycle |
| --- | --- |
| Identifier Mode | M0 T449 S2, Ordinary Mode; one implementer and independent self-review. |
| Candidate Proposal | [WOW32 execution continuation](../proposals/proposal-wow32-execution-continuation-001.md). |
| Admission And Approval | Owner approved T448 closure and continuation of the unfinished WOW foundations; S1 handoff delivered at `2dd9cf0b0`. |
| Objective | Connect inherited M1--M5 message, callback, conversion, object and ordinary lifecycle paths as dependency-complete groups, preserving original behavior and approved host boundaries. |
| Non-goals | No guest changes, new semantic waiver, unrelated WOW expansion, individual-task deletion (S3), or claim of full provider completion. |
| Reference Baseline | T448 released commit `55566cadb`, [T448 closure](../history/m0-t448-closure.md), [S1 handoff](../etc/evidence/m0-t449-s1-wow-continuation-audit.md), and [T447 working plan](../etc/operations/t447-wow-execution-plan.md). |
| Files And ABI Surface | Selected WOW32 mirrors and host adapters, relevant worker/service boundaries, tests and their exception registers; no guest ABI change. |
| Applicable Rules | Execution, architecture, coding, source policy, guest immutability, approved modern USER/external-window boundaries and minimum registered mirror diff. |
| Verification | Source-first coherent implementation groups; then background x86/x64 builds, applicable actual guest/native message modes and failure/lifecycle tests, complete package publication and hash checks, governance and independent self-review. |
| Expected Markers | Exact caller/ownership and failure reconciliation for M1--M5, current-package guest/native witnesses, ordinary retirement and explicit residual boundaries; no mock-only or old test represented as real guest proof. |
| Asset Needs | Read-only OpenNT source, existing release/guest package, stable Ninja/ccache trees and recorded T447/T448 tests; no guest rebuild. |
| Reporting Requirements | Report original reuse, removed duplication, mirror diff and exceptions, tests, package hashes and unresolved boundaries. Completed disposable evidence is under `build/t449-s2`, `build/t449-s2-message-cross-r9`, `build/t449-s2-message-pending-r3`, `build/t449-s2-message-external-r2`, `build/t449-s2-message-construction-r6` and `build/t449-s2-component-build-r1`, plus the existing fixture's required direct children `build/t449-s2-dde-alias-r1` through `r4`; invalidated task-owned attempts are cross r2--r7 and construction r3/r5. Each is owned by this S, 1 GiB and 60 minutes per group, review after 15 minutes without progress. |
| Stop Conditions | Guest modification, new observable semantic concession, unapproved expanded task scope or an unsupported ownership assumption. |
| Exit Criteria | M1--M5 and promoted prerequisites reconciled against production source and scoped tests; whole-group repairs reviewed, built, tested, coherently published and hash-verified, committed and pushed. Individual deletion and final T acceptance remain separate. |
| Original Owner Request | Close T448 and admit the previously unfinished WOW foundational T task. |
| Similar-Issue Sweep | Include message families, callback nesting and original scheduler, construction/transfer/abort, object reuse and normal task retirement, control/dialog/menu prerequisites, both native directions and failure cleanup. |

## Inherited Scope

The [continuation proposal](../proposals/proposal-wow32-execution-continuation-001.md)
inherits all unfinished T447 M1--M6/C/W rows, promoted dependencies and prior
acceptance constraints. The complete [T447 execution plan](../etc/operations/t447-wow-execution-plan.md)
and linked source audits/evidence remain authoritative historical inputs.
Previously delivered foundations and approved external snapshot, modern USER
and thread-level idle boundaries remain intact. NTSRV owns lifecycle control;
this task owns the WOW task algorithms and individual-task integration.
Other WOW packages remain in the queue with their residual scopes.

The completed [S1 audit](../etc/evidence/m0-t449-s1-wow-continuation-audit.md) maps the
inherited rows, bounded prior evidence and unfinished work to the proposed
S2--S4 delivery groups. It confirms that individual WOW task deletion is not
connected and distinguishes the service request identity from the original
HTASK. Its documentation was reviewed, committed and pushed at `2dd9cf0b0`.
The handoff is not a runtime repair, product-test result or semantic waiver.

Owner execution direction: trace source and call chains first; preserve original
capabilities and semantics and minimize original-source diffs. Where an original
implementation is proved composable and replaces autonomous duplication, remove
the duplicate as part of the same coherent implementation group. Connect each
whole group before building and testing it; tests verify the connected group,
not substitute for the source audit. Keep single-person dual-role review.

## S1 Closure Record

The [S1 audit and fixed handoff](../etc/evidence/m0-t449-s1-wow-continuation-audit.md)
were reviewed, passed documentation governance, and were committed and
pushed as `2dd9cf0b0`. S1 was source/planning only: no production or guest
change, runtime acceptance, package publication or new semantic concession.
Unproved runtime rows remain explicitly assigned to S2--S4.

## Current Technical Baseline

T448's complete 13-image package at `55566cadb` was built, tested, published
and hash-verified at both destinations; see its
[closure evidence](../etc/evidence/m0-t448-s5-integrated-acceptance.md#t448-owner-requested-preclosure-repeat-2026-10-22-local).
WRITE editing/exit and intermittent long-play outcomes remain unproved. Original
COMMAND's large-environment defect remains in TODO as a guest limitation.

S2's DDE ownership group is owner-accepted and closed. Source tracing found
one bounded gap: `WU32PostMessage` discards the original stack-local DDE
conversion record before native `PostMessage` reports acceptance or rejection.
The current reduced candidate retains it only until that result, leaving the
original successful receiver/ACK path and synchronous `WU32SendMessage`
unchanged. It additionally retires only an original generated EXECUTE alias
when a rejected post has no ACK. Earlier speculative exception/reentry and
synchronous-send edits were removed. The reduced candidate compiled as WOW32
`E1424CAC174C3388D293142A1A7F159634088BA07E65797408997A3B83C5BC39` and
passed the 101-case exact-body fixture plus isolated real guest DDE traffic,
including the rejected DATA-post cleanup; detailed evidence is linked below.
This is not evidence for DDEML, other message families, all formats,
allocation failure or arbitrary interleavings. The separate synchronous
EXECUTE diagnostic fails on candidate, released provider and a native control;
it is outside documented Win32 DDE protocol and does not establish the original
AmiPro-specific compatibility.
The bounded source findings and evidence are recorded in the
[DDE ownership working audit](../etc/evidence/m0-t449-s2-dde-ownership-audit.md).
The linked [M1--M3 message/scheduler audit](../etc/evidence/m0-t449-s2-message-scheduler-audit.md)
confirms the selected original/adapter boundary and identifies no new source
repair in that cohort; its selected current-package runtime matrix is complete.
The [M5 ordinary-retirement audit](../etc/evidence/m0-t449-s2-retirement-source-audit.md)
similarly confirms original `W32DestroyTask` remains the selected owner; it
does not substitute the later targeted-termination scope.
The later current-candidate rerun passed the coherent cross-worker,
pending-send/receiver-destruction and external-native matrices: 6/6, 6/6 plus
the required negative, and 9/9 respectively, each across Off/Win16/All;
details are in the message/scheduler audit. Pending death beyond the selected
case, uncovered interleavings and other conversion families remain open.

The final P package is `build/t449-s2/media-dde-r31`: its whole-component
cached build recorded all thirteen x86/x64 images and a no-work second pass.
That exact package reran cross-worker 6/6, pending 6/6 plus the required
negative, external peer 9/9, and the construction/ordinary-retirement witness
with zero errors. All thirteen files were then synchronized to
`assets/release` and `O:\winnt\system32` and rehashed against the tracked
release manifest. The guest archive, guest files and guest configuration are
unchanged. S3 targeted task termination and S4 integrated/whole-provider
acceptance remain explicitly unstarted.

## S2 Closure Record

The owner accepted S2 after verified P delivery `8d9ce29c3`. The committed
thirteen-image package is deployed at both mandated destinations, its manifest
hashes match, and the immutable guest archive remains unchanged. The fixed
scope was DDE post-result ownership plus its M1--M5 selected execution and
ordinary-retirement witnesses. It does not close S3 targeted WOW task
termination or S4 whole-T reconciliation. No successor S is active until the
owner explicitly admits it.

## Recent M0 Closures

| Task | Disposition |
| --- | --- |
| T448 | [Owner-accepted lifecycle closure](../history/m0-t448-closure.md). |
| T447 | [Owner-directed scope-transfer closure](../history/m0-t447-transfer-closure.md), not full WOW functional acceptance. |

## Recent Governance

T448's final governance, publication and retained limitations are documented
in its [closure](../history/m0-t448-closure.md).
