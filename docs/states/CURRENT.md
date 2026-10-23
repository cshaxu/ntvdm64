# Project Status

## Current Work

**Active: M0 T449 S4**, Ordinary Mode, one implementer with independent
self-review. The owner approved S4 integrated reconciliation and final T449
acceptance preparation on 2026-10-23.

## Active Packet

| Field | T449 S4 — integrated reconciliation and acceptance |
| --- | --- |
| Identifier Mode | M0 T449 S4, Ordinary Mode; one implementer and independent self-review. |
| Candidate Proposal | [WOW32 execution continuation](../proposals/proposal-wow32-execution-continuation-001.md). |
| Admission And Approval | Owner approved S4 after accepting S3 targeted task termination. |
| Objective | Reconcile every inherited T449 execution, lifecycle and application witness against one sealed production package, correcting only traced host-side defects. |
| Non-goals | No guest change/rebuild, new transport, speculative feature expansion, or reimplementation of selected original WOW algorithms. |
| Reference Baseline | S2 `8d9ce29c3`, S3 `7bdc5169f`, the inherited T447 ledgers and the continuation proposal. |
| Files And ABI Surface | Existing selected WOW32, NTVDM, NTSRV, worker-base and test/evidence surfaces only; no guest ABI. |
| Applicable Rules | Execution, architecture, coding, source policy, immutable guest, original/modern-host boundary, existing RPC/IO-only control policy and minimum registered mirror diff. |
| Verification | Map every residual C/W/M and application row to a selected production caller, test entrypoint, package and observed result; exercise real guest/native boundaries and lifecycle negatives; then build, publish, hash-verify and review one package. |
| Expected Markers | Each retained claim has exact evidence; any failure is root-caused, repaired as one coherent group or retained as a bounded, attributed limitation. |
| Asset Needs | Read-only original OpenNT source and existing immutable guest/release package; stable Ninja/ccache trees; no guest rebuild. |
| Reporting Requirements | Record row-to-evidence mapping, source ownership, mirror diffs/exceptions, package hashes and every limitation. |
| Stop Conditions | Guest change, unapproved semantic concession, untraced replacement of original behavior, or scope growth beyond the inherited ledger. |
| Exit Criteria | All inherited S4 rows are proven or honestly bounded with evidence; package is built, published, hash-verified, reviewed, committed and pushed for owner acceptance. |
| Original Owner Request | Continue T449 under the established source-first, minimal-diff WOW recovery rules until its pre-acceptance boundary. |
| Similar-Issue Sweep | Internal and guest-created tasks, separate WOW workers, natural exit, duplicate management requests, nested send/unresponsive task, worker death and distinct console/worker-close paths. |

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

S4's later `package-r2` supersedes its stale initial candidate: the current
thirteen-image package was rebuilt, exercised through the selected message,
application, guest-contract and CAF matrices, published and hash-verified at
both destinations.  Its evidence is in
`m0-t449-s4-reconciliation-ledger.md`; only WRITE physical edit/save/normal
exit remains an open S4 witness, not a passing row.

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

The owner accepted [S2 after verified P delivery](../etc/evidence/m0-t449-s2-dde-ownership-audit.md)
`8d9ce29c3`. The committed
thirteen-image package is deployed at both mandated destinations, its manifest
hashes match, and the immutable guest archive remains unchanged. The fixed
scope was DDE post-result ownership plus its M1--M5 selected execution and
ordinary-retirement witnesses. It does not close S3 targeted WOW task
termination or S4 whole-T reconciliation. No successor S is active until the
owner explicitly admits it.

## S3 Closure Record

The [S3 targeted-termination witness](../etc/evidence/m0-t449-s3-targeted-termination-progress.md)
records the accepted close boundary. The current candidate routes only an authenticated shared-WOW `iTask` through
the existing management RPC, worker-base close event and original WOW32
`W32HungAppNotifyThread`/`W32DestroyTask` path.  Separate WOW remains
worker-close-only.  The two original user-cancel exits now report ticket
failure, so a later explicit request can retry without inventing a terminator.
The post-link candidate package `build/t449-s3/media-r6` has passed service
ticket lifecycle and worker event regressions, and all thirteen images are
published/hash-verified at `assets/release` and `O:\winnt\system32`.  It is
not S3 closure: no accessible desktop currently permits the required real
two-task/sibling/reuse/worker-loss matrix.

That desktop limitation was later resolved.  The same-session witness in
`m0-t449-s3-targeted-termination-progress.md` exercised two real tasks in one
shared worker, target-only retirement, sibling survival, reuse, duplicate
request handling and whole-worker close.  S3 is therefore closed; guest media
and configuration remain unchanged, and S4 is not admitted by this record.

## Recent M0 Closures

| Task | Disposition |
| --- | --- |
| T448 | [Owner-accepted lifecycle closure](../history/m0-t448-closure.md). |
| T447 | [Owner-directed scope-transfer closure](../history/m0-t447-transfer-closure.md), not full WOW functional acceptance. |

## Recent Governance

T448's final governance, publication and retained limitations are documented
in its [closure](../history/m0-t448-closure.md).
