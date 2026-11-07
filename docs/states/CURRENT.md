# Project Status

## Current Work

**Active: M0 T479 S9 — Win3.1 DOS display-transition repair.**
Owner keeps T479 open. [S8 delivery](../etc/evidence/m0-t479-s8-shared-delivery.md)
imports unchanged shared components, passes full cached builds and Full16/16,
and publishes all11 host images as package49af0246-027c-4b11-9ac0-ae94c2a8cfa5.
Its reviewed commit/push is the current delivery checkpoint.
The archived S6 candidate remains unselected; its incomplete work remains in
the [queued successor](../proposals/proposal-guest-multitask-io-completion-001.md).
[S6 handoff](../etc/evidence/m0-t479-s6-closure-handoff.md) preserves that scope.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Windowed DOS text repair](../proposals/proposal-win31-windowed-dos-text-debug-001.md); [S8/S9 plan](../etc/operations/m0-t479-s8-s9-import-plan.md). |
| Identifier Mode | M0 T479 S9/display-transitions; Ordinary Mode, single implementer/reviewer. |
| Admission And Approval | Owner adds this S after unchanged shared import, explicitly covering all three Win3.1 DOS window/fullscreen failures and directing reuse of SoftPC recent S records. |
| Objective | Correct window-to-fullscreen garbling, fullscreen-to-window character corruption, and invisible typed characters with moving cursor after return. |
| Non-goals | No original guest modification/rebuild, unrelated Win95 changes, archived S6 restoration or T closure. |
| Reference Baseline | S8 published package49af0246; SoftPC accepted S33 final state1ec0d551, including P1/P3 CHAIN2 and P4 BDA text aperture repairs. |
| Files And ABI Surface | Reached mvdm-host cvidc/video/host nt_cga paths and corresponding local tests/adapters; audit exact mapping before importing. |
| Applicable Rules | Execution, source policy, mirror provenance/diff registration and architecture/coding/document rules. |
| Verification | Source dependency and semantic comparison; matched targeted controls, real Win3.1 repeated window/fullscreen transitions with input/DIR/CLS/VER/exit; affected DOS/WOW regressions, cached full builds, Full and coherent publication. |
| Expected Markers | Correct visible cells and input after both transition directions, preserved guest behavior and no false pass from upstream evidence alone. |
| Asset Needs | Read-only SoftPC source/evidence and existing immutable Win3.1 installation/media; isolated probe copies. |
| Reporting Requirements | Exact imported repair/lineage, mirror diff, before/after evidence, remaining boundaries and release hashes. |
| Stop Conditions | Unproved semantic loss, need to modify original guest, or new compatibility concession; preserve work and investigate. |
| Exit Criteria | All three defects verified, applicable gates pass, full package published/hash checked, reviewed commit and push; T remains open. |
| Original Owner Request | Import SoftPC fixes for all three Win3.1 MS-DOS window/fullscreen display failures. |
| Similar-Issue Sweep | CHAIN2 scalar/string/rule addressing, text BIOS read/write aperture, redraw and input visibility across repeated mode changes. |
| Temporary Evidence | build/t479-s9-display, executor-owned60-minute phases/2GiB/15-minute no-progress review; archive necessary evidence before cleanup. |

## S4 Win3.1 Checkpoint And Deferred Win95

[Evidence and P deliveries](../etc/evidence/m0-t479-s4-driver-load.md) record
actual identities, 320 callbacks, counted and blocked cross-VM waits, native
thread/PIC wake, cancellation, forced queued retirement, actual handle reuse,
normal exit and injected initialization rollback. ntvdm64.386 is published as
a bridge candidate only. Ten host components remain the Full 16/16 P1 package.

[The roadmap](../etc/operations/m0-t479-guest-bridge-plan.md) retains S4's
Win95 wait/wake/lifecycle proof. Owner-directed S6 closure now transfers this
unpassed obligation, full guest ICW1-reset combinations and unverified VMM
allocation failures to the queue-first successor, not the bounded S7 audit.

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

The [S10 audit](../etc/evidence/m0-t473-s10-closure.md) records bounded source-role decisions and S11--S19 handoff.

## T469 Closure Record

T469 COMM records: [S1](../history/m0-t469-s1-closure.md),
[S2](../etc/evidence/m0-t469-s2-comm-source-disposition.md),
[S3](../etc/evidence/m0-t469-s3-comm-retained-route.md),
[closure](../history/m0-t469-closure.md). No guest bytes changed.

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

The current published baseline is P4 package daa392bf-02e0-439f-9b1c-e21804398133,
application0.0.479/protocol67, eleven host images and bounded authored addons.
S7 audit201 rechecks both publication destinations and unchanged production
source against70e829d88. Full09116/16 remains prior accepted-source evidence.
Guest media is unchanged; complete I/O/nested/Win95 acceptance belongs to the
queued successor, not this bounded closure.
The earlier [source release review](../../artifacts/mvdm-audit/latest-source-release-review.md)
remains retained baseline evidence.

## Temporary Evidence

The active packet above owns the current temporary scope and budgets. Earlier
T451--T473 task directories remain disposable evidence, not reusable caches;
their stage records retain results and handoffs. This compaction does not
authorize deletion: verify ownership, process quiescence, retained hashes and
exact targets through the execution rules before cleanup. Historical process
inventory failures are not proof of quiescence. Stable build/.ninja and
build/.ccache remain reusable and untouched.

## Recent M0 Closures

[T460](../history/m0-t460-closure.md), [T454](../history/m0-t454-closure.md),
[T453](../history/m0-t453-closure.md), T452,
[T451](../history/m0-t451-closure.md), [T450](../history/m0-t450-closure.md),
[T449](../history/m0-t449-closure.md), [T448](../history/m0-t448-closure.md).

## Recent Governance

T460 package and Clipboard acceptance are recorded in its closure above.
