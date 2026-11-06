# Project Status

## Current Work

**Active: M0 T479 S1** - guest multitasking bridge contract and implementation
admission, with delivered S1/generic-thunk side integration. Guest S1
remains the mainline; workline-qualified S labels distinguish the two scopes.
T477 is closed in its [closure](../history/m0-t477-closure.md).

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T479 S1; Ordinary Mode, single executor/reviewer. |
| Admission And Approval | Owner admits a mixed T for the guest multitasking payload, now explicitly authorizes all committed adjacent work and duplicate S numbers distinguished by content. |
| Parallel Task Reservation | M0 T478; already allocated in the independent mirror-source-audit worktree, recorded in the linked admission plan. Not a local active packet or closure. |
| Candidate Proposal | [Guest bridge](../proposals/proposal-multitask-guest-vdm-frontends-001.md); [stage plan](../etc/operations/m0-t479-guest-bridge-plan.md). |
| Objective | Resolve the actual VMM identity, BOP continuation and legal wait/resume contract for 54:08 and 54:0A; freeze the smallest guest/host bridge before implementation. |
| Non-goals | No Windows core/immutable guest modification, hardware V86 monitor, resident DOS agent or replacement frontend authority. |
| Reference Baseline | cd8c8583e; T477 host foundation, protocol 67 and published ten-image package. |
| Files And ABI Surface | Project-owned VxD design/toolchain and NTVDM VM bridge; inspect cmdExec32/BOP/CCPU/VMM callers and existing frontend-base/NTSRV contracts. Side integration includes original WOW generic-call helper, its build selection and tests. |
| Applicable Rules | Execution, architecture, coding, documentation and source policy; original-owner recovery and explicit mirror-diff registration. |
| Verification | Trace both launch services through host stack return; identify exact VM/context and continuation ownership; source/provenance and isolated test matrix; side-import cached builds, focused thunk/guest checks, Full and coherent publication; documentation governance. |
| Expected Markers | Finite call-chain/ABI/lifecycle decisions, concrete component/test plan, documented unresolved evidence without success claims. |
| Asset Needs | Existing Win3.1 enhanced/Win95 research and owner media read-only; project-owned add-on built/staged separately. |
| Reporting Requirements | Separate calling-VM wait from whole-payload suspension; record OS-specific proof and every unsupported boundary. |
| Stop Conditions | Unproved VM identity or continuation safety; new semantic concessions or immutable guest changes require an explicit decision. |
| Exit Criteria | Reviewed exact bridge contract, source/toolchain provenance, implementation/test sequencing and no hidden design assumptions. |
| Original Owner Request | Implement guest multitasking: after loading 386/VxD, cmdExec32 creates a VM frontend and child chain without suspending the containing payload; also integrate the adjacent committed S1/S2 work. |
| Similar-Issue Sweep | System/pre-existing VMs, handle reuse, both command services, reentrancy, unload, completion/cancel races, nested chains and NIDDB interaction. |
| Temporary Evidence | build/t479-s1; owner S1; 60-minute phase, 2 GiB, 15-minute no-progress review budget; side integration uses build/t479-s1-import, 90 minutes/4 GiB. |

## Imported side-work record

Mainline source work is recorded in the
[S1 guest call-chain audit](../etc/operations/m0-t479-s1-guest-callchain.md).
The existing command wait blocks the whole worker; independent VM continuations
must leave that host stack. Historical two-VM evidence was recovered from Git,
including its duplicate-input failure. Follow-up source tracing selects a
VMM callback/semaphore plus emulated event-device candidate for validation,
confirms existing SAS paging translation, and excludes blindly forwarding
NIDDB initialization. Callback/wait, notification, input, presentation and
lifecycle proofs remain explicit S1 gates; no guest bridge runtime or new
publication is claimed.

[S1/generic-thunk integration](../etc/evidence/m0-t479-s1-thunk-import.md)
retains T478 source S1/S2 identities. The imported S1 inventory is explicitly
incomplete; no guest mainline milestone is closed by importing it.

## S4 Closure Record

[VM groups and integrated delivery](../etc/evidence/m0-t477-s4-vm-groups.md):
CONSOLE and VM frontend groups preserve independent worker chains; VM PID is
hostPID-VMID, TASK is empty, and VM management remains read-only. Cached
builds, actual RPC/renderer/service tests and Full 16/16 gates passed. All ten
images are hash-verified at both publication destinations.

## S3 Closure Record

[T477 S3 host composition evidence](../etc/evidence/m0-t477-s3-host-composition.md)
and its [design](../etc/operations/m0-t477-s3-vm-frontends-design.md).
No guest driver or actual guest multitasking capability is claimed.

## S2 Closure Record

[T477 S2 P4 integrated delivery](../etc/evidence/m0-t477-s2-p4-integration.md)
and [S2 implementation evidence](../etc/evidence/m0-t477-s2-frontend-instances.md).

## S1 Closure Record

[T477 S1 closure evidence](../etc/evidence/m0-t477-s1-frontend-base.md).

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

The [T479 side integration](../etc/evidence/m0-t479-s1-thunk-import.md) publishes
the ten-image product at application version 0.0.479 and protocol 67. Full
16/16 selected gates pass and both publication targets match. Guest media is unchanged.
Actual guest VM discovery and driver integration remain separately queued.
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
