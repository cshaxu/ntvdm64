# Project Status

## Current Work

**Active: M0 T479 S4** - project guest VxD registration and wait/wake proof.
The accepted shared-library side stage is closed; this is the resumed guest mainline.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask frontends](../proposals/proposal-multitask-guest-vdm-frontends-001.md). |
| Identifier Mode | M0 T479 S4/guest-driver, implementation and executable-proof phase; Ordinary Mode, single implementer/reviewer. |
| Admission And Approval | Owner approves the host-first S2--S7 roadmap and continuous execution; each S builds, tests, publishes, commits and pushes before advancing. |
| Objective | Build/load the project VxD and prove actual VM identity/inventory, generation/retirement, callback and counted wait/wake through a claimed emulated notification resource on Win3.1 enhanced and Win95. |
| Non-goals | No original guest changes/rebuild, hardware V86 monitor, new polling/timeouts/transport, or semantic concessions. Full 54:08/0A execution and I/O remain S5/S6. |
| Reference Baseline | S3 host command preparation delivery; package 74269114-368c-45ab-b52d-bf103d08b0b9, Full 16/16, 31 capture and 59 launch assertions. Actual guest multitasking remains unproved. |
| Files And ABI Surface | Project driver in src/addon/ntvdm64-386; release assets/release/ntvdm64.386 and ntvdm64.vxd. NTVDM VM bridge/notification and matching tests/build/load tools. Necessary mirror hooks require source proof/difference registration; original guest stays immutable. |
| Applicable Rules | Execution, architecture/coding/document rules and source policy; immutable original guest and minimum registered mirror differences. |
| Verification | Original VMM call-chain/ABI review; actual driver load, VM enumeration/retirement, callback return, counted wait/wake, resource claim/reset and cancellation on both guests; cached ten-component build, full gates and coherent publication. |
| Expected Markers | Actual VMM VM identity, not focus/CR3 inference; callback stack/state preservation; independent notification wake while target VM waits; generation-safe retirement; no host wait inside CCPU callback. |
| Asset Needs | Existing immutable Win3.1/Win95 fixtures and verified package; recovered project VxD toolchain with recorded provenance; stable Ninja/ccache roots. Original guest is never rebuilt. |
| Reporting Requirements | Separate confirmed source facts, design decisions and runtime evidence; explain capability consequences rather than repeat next-step slogans. |
| Stop Conditions | New semantic concession, guest patch, unknown provenance or incompatible architecture requires explicit disposition; missing proof is not a pass. |
| Exit Criteria | Actual guest driver registration/wait/wake contracts pass independently on both guests; complete package built/tested/published, committed and pushed. S5 then wires multitask commands; no host-only substitute counts as guest proof. |
| Original Owner Request | Set a persistent goal and execute approved host-first S2--S7 continuously, delivering each stage before advancing. |
| Similar-Issue Sweep | Both 54:08/0A; system/ordinary/reused VMs; single-task transitions; nested children; input/output/failure; NIDDB independent owner. |
| Temporary Evidence | build/t479-s4-driver; owned 60-minute phases, 2 GiB, 15-minute no-progress review. S3 evidence remains until its reviewed summary and delivery are durable. |

S4 [P1 checkpoint](../etc/evidence/m0-t479-s4-driver-load.md): Win3.1 native-thread
PIC wake, forced queued retirement and normal exit pass. Full 16/16; normal
ten-component package ca3b367a-3d54-4d05-a9a8-147594d7450d published to both
destinations. Actual Win3.1 VM handle reuse also passes. Driver remains a
candidate; initialization rollback and command integration remain open.
S4 is not closed.

Owner priority: finish the Win3.1 enhanced-mode path first. Do not launch new
Win95 investigations or tests while completing that path. Retain existing
Win95 evidence; its remaining acceptance obligations are deferred, not passed.

## S3 Host Command Preparation Closure

[Delivery and review](../etc/evidence/m0-t479-s3-command-preparation.md) records
independent capture and launch ownership, 31/59 assertions, cached full builds
and Full 16/16. Package 74269114-368c-45ab-b52d-bf103d08b0b9 is published and
hash-verified at both destinations. No mirror/guest changes. Continuous execution
now admits S4; guest VM registration and wait/wake remain executable proof gates.

## S2 Host Foundation Closure

[Implementation and closure](../etc/evidence/m0-t479-s2-host-foundation.md)
records independent VM retirement, cancellable dispatch and paired launch
contexts. Cached complete builds, actual RPC, 233 VM assertions and Full 16/16
pass; package a0f0dc2c-9f29-4bbe-8dad-8859cabea128 is published and hash-verified
at both destinations. No original mirror/guest changes. Continuous execution
now admits S3; actual guest multitasking remains unproved until later stages.

## S2 Guest Design Checkpoint

[Boundary audit](../etc/operations/m0-t479-s2-guest-design-boundaries.md) records
seven concrete hazards and selected corrections: retirement locking, dispatch
cancellation, paired launch capabilities, single-active COMMAND state (standard handles already TLS), VM death,
callback restoration and independent wakeup. Guest ABI/event execution and the
complete I/O provider remain explicit S2--S5 proof gates, not passed claims.
No runtime or published image changes accompany this research checkpoint.

## S2 Shared Import Closure

[Delivery evidence](../etc/evidence/m0-t479-s2-shared-import.md) records 46/46
exact upstream source files, one generic test suite, MSVC/GCC x86/x64 passes,
both cross-width matrices and Full 16/16. The coherent ten-image package
f86fa099-e0ba-4d7f-bbd3-721fbee9b824 is published at both destinations.
Owner acceptance closes this side stage at 967ef3d31. Guest-bridge implementation remains a separate workline; this delivery does
not imply guest multitasking support.

## S1 Closure Record

[Closure and mandatory handoff](../etc/evidence/m0-t479-s1-guest-research-closure.md)
records the bounded research outcome and transfers executable proof obligations
to S2--S5. The candidate is VMM callback/semaphore plus an emulated notification
device; exact runtime ABI, input/presentation and lifecycle behavior are not
claimed proven. No runtime, mirror or guest bytes changed in this closure.

[Stage plan](../etc/operations/m0-t479-guest-bridge-plan.md) and
[call-chain research](../etc/operations/m0-t479-s1-guest-callchain.md) retain the
full objective. No new semantic concession or guest modification is authorized.

## Delivered S1 Side Integration

[Generic-thunk integration](../etc/evidence/m0-t479-s1-thunk-import.md) retains
its separate source history, Full 16/16 acceptance and coherent published
0.0.479/protocol-67 ten-image package. These passes are not guest-bridge proof.
T478 remains the independently reserved worktree task, not a local packet.

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

## T477 S1 Closure Record

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

The delivered [T479 host foundation](../etc/evidence/m0-t479-s2-host-foundation.md) publishes
the ten-image product at application version 0.0.479 and protocol 67. Full
16/16 selected gates pass and both publication targets match. Guest media is unchanged.
S3 prepares independent host requests; actual guest VM discovery and driver execution remain S4 acceptance obligations.
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
