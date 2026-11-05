# Project Status

## Current Work

**Active: M0 T477 S2** — NTSRV frontend-instance registration, identity and
handoff. Owner admits S2 and requests a design on 2026-11-05. This checkpoint
is audit/design only; no S2 production implementation or publication is claimed.

S1 closed with P1 154a61877 and P2 6caaf5d78; [S1 evidence](../etc/evidence/m0-t477-s1-frontend-base.md)
records builds, tests and both publications. The baseline is 6caaf5d78.
The [S2 design](../etc/operations/m0-t477-s2-frontend-instances-design.md)
separates authenticated host connections from frontend instances while reusing
the existing NTSRV lifecycle and direct frontend/worker I/O. Worker-base must
not depend on frontend-base. S3 owns direct NTVDM VM integration; S4 owns NTMON
presentation; guest drivers remain separately queued. No push is authorized.

## Active Packet

### M0 T477 S2 — instance-addressed frontend control

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T477 S2, Ordinary Mode. |
| Admission And Approval | Owner: admit S2 and provide its design. This checkpoint is design-only; proposed production parts await implementation direction. |
| Candidate Proposal | [Host frontend foundation](../proposals/proposal-host-common-frontend-foundation-001.md). |
| Objective | Design authenticated multiple frontend instances per host, isolated handoff and scoped teardown using existing NTSRV authorities. |
| Non-goals | No guest/VxD implementation, NTVDM VM adapter/registry implementation, worker-base dependency on frontend-base, NTMON new VM UI, or second scheduler. |
| Reference Baseline | 6caaf5d78; completed S1 shared channel/service; existing frontend_registry, lifecycle, client and IDL. |
| Files And ABI Surface | NTSRV private records/registry/lifecycle/transport; common protocol and BaseSrv clients; NTCON adapter; owning tests and design. Instance-addressed control RPC requires a protocol revision during implementation. |
| Applicable Rules | Current ownership, source reuse, lifecycle authority, immutable mirrors/guests, execution and document rules. |
| Verification | Design: source-backed ownership/caller audit, documentation governance and diff checks. Implementation: two instances on one authenticated host, wrong/stale identities, independent chains, release ordering, instance/host loss, existing NTCON and Full regression, coherent package hashes. |
| Expected Markers | Process authentication remains connection-owned; instance selection is explicit; one active worker per frontend; a VM frontend close never becomes host-worker exit. |
| Asset Needs | Existing project-owned code and unchanged S1 test/package inputs; no import. |
| Reporting Requirements | Distinguish planned behavior, local fixtures and actual NTVDM/guest integration; record per-P validation and publication. |
| Stop Conditions | Need to change guest execution/scheduling, introduce reverse component dependencies or bypass peer/capability authorization; revise scope before dependent work. |
| Exit Criteria | Explicit-instance service/client path selected by NTCON; real service multi-instance fixtures, negative/lifecycle tests and product regression pass; both publication targets match; local commits, no push. Not met by this design checkpoint. |
| Original Owner Request | Admit S2 and provide a design following completed S1; explain host connection versus frontend instance and control RPC. |
| Similar-Issue Sweep | Root-generation references, route ownership, participant exit, startup reservations, retained capabilities, worker watches, management projection and connection rundown. |
| Temporary Evidence | build/t477-s2; 90-minute phase, 4 GiB, 15-minute no-progress bound. S1 delivery/rollback records remain in build/t477-s1; caches remain .ninja/.ccache. |

## S1 Closure Record

T477 S1 closed at 6caaf5d78. [S1 evidence](../etc/evidence/m0-t477-s1-frontend-base.md)
records its two local deliveries, full verification and matching publication
at both targets. No push, by explicit owner direction.

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

## T473 S1 Closure Record

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

This worktree's [T477 S1 delivery](../etc/evidence/m0-t477-s1-frontend-base.md)
builds all ten components at application version 0.0.477. Full selected product
gates pass and both publication targets match. Guest media is unchanged.
The earlier [source release review](../../artifacts/mvdm-audit/latest-source-release-review.md)
remains retained baseline evidence.

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
