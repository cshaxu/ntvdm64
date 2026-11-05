# Project Status

## Current Work

**Active: M0 T477 S1** — extract the shared frontend connection foundation
and reconnect NTCON without changing its existing presentation or lifecycle.

Owner admitted the host frontend foundation in a new branch worktree on
2026-11-05, using the current main task number plus one. This worktree starts
at main commit `869f001e6719cc77e35b44197a75dbd2d3addc72`.
Main's T476 S2 mirror audit remains independent and is not closed by this
admission. Only T477 S1 is active here.

The [implementation sequence](../etc/operations/m0-t477-host-frontend-plan.md)
retains the complete approved host scope. The guest 386/VxD driver and actual
guest bridge remain a separate queued package. Admission is complete;
The owner now authorizes S1 implementation, build, tests and publication to
this worktree assets/release and O:/winnt/system32, followed by local commits
without pushing. P1 channel extraction passed x86/x64 fixtures and all 16 Full gates, then
published the coherent ten-image package to both targets with matching hashes.
See [S1 evidence](../etc/evidence/m0-t477-s1-frontend-base.md). P2 service-pump
extraction remains active. The owner explicitly confirms S1 admission
and requests the [S1 design](../etc/operations/m0-t477-s1-frontend-base-design.md);
it defines two production P deliveries, channel extraction then service extraction.

## Active Packet

### M0 T477 S1 — shared frontend foundation and NTCON integration

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T477 S1, Ordinary Mode. |
| Admission And Approval | Owner: create a branch worktree from main and admit implementation using the next T number. Latest owner correction makes NTVDM directly use common/frontend-base in S3; worker-base must not depend on frontend-base. The 1+N connection model remains. |
| Candidate Proposal | [Host common frontend foundation](../proposals/proposal-host-common-frontend-foundation-001.md). |
| Objective | Extract reusable per-instance frontend connection and channel lifecycle into common/frontend-base and make NTCON use it with unchanged Console/Window and handoff behavior. |
| Non-goals | No guest driver/media changes, NTVWM frontend enablement, new scheduler, I/O relay through NTSRV, or implementation of later S scopes during S1. |
| Reference Baseline | Main 869f001e6719cc77e35b44197a75dbd2d3addc72; owner-approved proposal and existing NTCON/common I/O and BaseSrv clients. |
| Files And ABI Surface | src/ntcon-exe; new src/common/frontend-base; selected common protocol/client and build composition; owning tests; architecture and evidence. Preserve current wire ABI in S1 unless an evidenced scope revision is recorded. |
| Applicable Rules | Architecture and coding ownership, source-first reuse, immutable guests, existing NTSRV lifecycle authority, execution and documentation gates. |
| Verification | Audit complete moved units and resource/lock ownership; build affected x64 frontend/shared and x86 worker consumers; unit cancellation/disconnect/rollback tests; actual NTCON DOS/native handoff, input/output, GUI and redirection regressions; coherent package publication and hash checks. Run git diff --check and tools/governance/Verify-DocumentationGovernance.ps1 including relative-link checks. |
| Expected Markers | Production NTCON links frontend-base; no common-to-worker/executable dependency; one active worker per frontend; unchanged handoff ordering and deterministic cleanup; reproducible test evidence. |
| Asset Needs | Existing checked-in project code, selected compiler islands, unchanged guest/package inputs and existing regression runners. No external source import is admitted. |
| Reporting Requirements | Per-P changed ownership, exact tests/results, package/hash evidence, source revision and remaining limits; distinguish host fixtures from real guest driver acceptance. |
| Stop Conditions | Required guest mutation, lifecycle authority change, unaudited mirror replacement or material contract expansion; record the issue and revise admission before dependent implementation. |
| Exit Criteria | NTCON uses the shared production implementation, displaced duplicates removed, required tests and coherent publication verified, reviewed local commit; owner explicitly defers push and requires both this worktree assets/release and O:/winnt/system32 publication. |
| Original Owner Request | Create a new branch worktree from main and admit the host frontend task as the current T number plus one; latest correction: S3 connects NTVDM directly to common/frontend-base, with no worker-base dependency on frontend-base; future NTVWM can use the same direct composition. |
| Similar-Issue Sweep | NTCON frontend_session/session_service/console_channel, common console client/protocol, both workers' I/O cleanup and NTSRV handoff assumptions. |
| Temporary Evidence | Worktree-local build/t477-s1; 90-minute phase, 4 GiB, 15-minute no-progress budget. Stable .ninja/.ccache are caches, not evidence. |

## M0 T475 S5 Closure

The imported process-hook is selected by the product; the recorded local
helper fix is retained. C++ source/build selection and Detours dependencies
were removed. x86/x64 builds, all four caller/target width pairs, missing-DLL,
negative and lifecycle checks, real guest tests, and the Full selected gates
passed. WRITE edit, save and restart passed against the pre-replacement and
current packages. The owner excludes DOS4GW/NESticle. The late observer's
new-thread exclusion limit was also present in the former Detours use and is
not a new closure requirement. Local release, commits and exact evidence are
recorded in [S5 evidence](../etc/evidence/m0-t475-s5-process-hook-import.md).
The owner directs no push or main-runtime publication.

## S12 Closure Record

The owner accepted [S12 source layout and compile-time boundaries](../etc/evidence/m0-t473-s12-source-layout-closure.md).

## S11 Closure Record

The [S11 compilation-condition closure](../etc/evidence/m0-t473-s11-condition-cleanup-closure.md) records the selected non-MVDM cleanup, published package equivalence and deferred recovery boundaries.

## S10 Closure Record

The [S10 audit](../etc/evidence/m0-t473-s10-closure.md) records bounded source-role decisions and S11–S19 handoff.

## T471 Closure Record

T471 closes at [its closure](../history/m0-t471-closure.md): same-artifact
application/concurrency/abnormal evidence passed, while physical WRITE input
remains the explicitly approved non-pass. No runtime or guest bytes changed.

## T469 Closure Record

T469 COMM records: [S1](../history/m0-t469-s1-closure.md),
[S2](../etc/evidence/m0-t469-s2-comm-source-disposition.md),
[S3](../etc/evidence/m0-t469-s3-comm-retained-route.md),
[closure](../history/m0-t469-closure.md). No guest bytes changed.

## T463 S2 Closure Record

The [T463 S2 record](../etc/evidence/m0-t463-s2-sol-options-attribution.md)
retains the public HWND binding, SOL witness and Full gate. S3 owns remaining
task lifecycle acceptance.

The [T461 S3 record](../etc/evidence/m0-t461-s3-registry-persistence-candidate.md)
retains the durable-overlay delivery and its acceptance evidence.

T461 S1's unchanged inventory is retained in its
[owner matrix](../etc/evidence/m0-t461-s1-shell-registry-owner-matrix.md).

## T461 S2 Closure Record

The [S2 acceptance](../etc/evidence/m0-t461-s2-binding-implementation.md)
records the reviewed Registry bindings and product gate. S3 owns durable
commit, restart tombstones and cross-worker serialization.

## S1 Closure Record

The [T473 baseline](../etc/evidence/m0-t473-s1-whole-closure-baseline.md) records original source and composition inventory.

## S2 Closure Record

The [T473 CPU40/DPMI reconciliation](../etc/evidence/m0-t473-s2-cpu40-dpmi-reconciliation.md) and companion owner records retain the per-package decisions.

## S3 Closure Record

The [T473 CPU40 lifecycle](../etc/evidence/m0-t473-s3-cpu40-execution-lifecycle.md) records the execution and retirement boundary.

## S4 Closure Record

The [T473 DOS/COMMAND/XMS lifecycle](../etc/evidence/m0-t473-s4-dos-command-xms-lifecycle.md) records the selected original owner paths.

## S5 Closure Record

The [T473 SoftPC device lifecycle](../etc/evidence/m0-t473-s5-softpc-device-lifecycle.md) records the bounded device-owner audit.

## S7 Closure Record

S7 closed at [its session/callback ownership audit](../etc/evidence/m0-t473-s7-wow-session-callback-audit.md)
without a runtime change; S8 owns public-USER object identity and retirement.

## S8 Closure Record

S8 closes at its [public-USER evidence](../etc/evidence/m0-t473-s8-public-user-identity-recovery.md).
The ten-image package is published; the skipped direct completion/input
control observation remains a non-pass. S9 owns generic wrappers.

## S9 Closure Record

The [T473 USER wrapper evidence](../etc/evidence/m0-t473-s9-generic-user-wrapper-audit.md) records the mapped carrier family and delivered r10 package.

## S6 Closure Record

S6 was a no-runtime [status-sequence correction](../etc/evidence/m0-t473-s6-status-sequence-correction.md);
S7 owns the WOW32 session/callback audit.

## T459 Closure Record

W18 is closed in [the T459 closure](../history/m0-t459-closure.md). It
preserves original common-dialog and OLE alias authorities, passes real guest
modeless-dialog retirement plus exact alias lifecycle evidence, and records
guest OLE client/server as an explicit immutable stopping boundary. No product
image or guest input changed.

## Closed predecessor evidence

- [T457 closure](../history/m0-t457-closure.md): dialog/input/caret and hook/timer recovery; per-S evidence is indexed there.
- [T456 closure](../history/m0-t456-closure.md): font/text/metafile recovery and retained acceptance limits.
- [T455 closure](../history/m0-t455-closure.md): original GDI/DC/DIB/palette recovery and application observations.
- [T453 closure](../history/m0-t453-closure.md): files, environment and OEM recovery.

Detailed closed-stage chronology belongs to those retained records; the active
packet above remains the only current S authority.

## Current Technical Baseline

The [latest source delivery](../../artifacts/mvdm-audit/latest-source-release-review.md)
builds all ten components from committed production code. Full selected
product gates pass; both deployment targets match. Guest media is unchanged.

## Temporary Evidence

T459 S1--S4 used `build/t459-s1` through `build/t459-s4` only for disposable
evidence. Their retained results are indexed from the closure; stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T467 S3/S4 disposable build and acceptance evidence uses `build/t467-s3`
(60-minute phase, 3-GiB, 10-minute no-progress budget).

T471 S1 package-freeze evidence uses `build/t471-s1` (30-minute phase,
1-GiB, 10-minute no-progress budget); S2/S3 acceptance evidence uses
`build/t471-s2` (60-minute phase, 3-GiB, 10-minute no-progress budget).

T473 S1/S9 may use only `build/t473-s9` for disposable closure-ledger and
WOW-session evidence,
with a 60-minute phase, 3-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T461 S1 may use only `build/t461-s1` for disposable source-audit evidence,
with a 30-minute phase, 2-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T461 S2/S3 disposable evidence uses `build/t461-s2` and `build/t461-s3`;
each has a 45-minute phase, 2-GiB size and 10-minute no-progress budget.

T462 and T463 disposable evidence remains under their `build/t462-s1` and
`build/t463-s2`/`s3` roots; the shared caches are not evidence roots.

T457's closed-stage evidence is indexed in its [closure](../history/m0-t457-closure.md).

T454 may use only declared `build/t454-s1`, `build/t454-s2`, `build/t454-s3`
and `build/t454-s4` roots for disposable evidence; stable `.ninja` and
`.ccache` remain reusable caches. Retain durable results before exact-target
cleanup; never use a task root as a build cache.

`build/M0-T451`, `build/t451-s3`, `build/t451-s4` and `build/t451-s5` remain
temporary evidence, not reusable caches. Process inventory access was denied
during closure; deletion requires verified quiescence and the existing
exact-target ownership checks. Durable results are linked from the closure.
Stable `.ninja` and `.ccache` caches remain untouched.

T456 S1/S2 disposable evidence uses `build/t456-s1` and `build/t456-s2`.

## Recent M0 Closures

[T460](../history/m0-t460-closure.md), [T454](../history/m0-t454-closure.md),
[T453](../history/m0-t453-closure.md), T452,
[T451](../history/m0-t451-closure.md), [T450](../history/m0-t450-closure.md),
[T449](../history/m0-t449-closure.md), [T448](../history/m0-t448-closure.md).

## Recent Governance

T460 package and Clipboard acceptance are recorded in its closure above.
