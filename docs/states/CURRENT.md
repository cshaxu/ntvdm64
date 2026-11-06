# Project Status

## Current Work

**Active: M0 T479 S5** - Win3.1 multitask command continuation.
S4's Win3.1 checkpoint is delivered; its Win95 continuation remains open.
[Entry/return audit](../etc/operations/m0-t479-s5-command-continuation.md).
Local CDS/text application and selected-VM keyboard consumption pass in Win3.1
diagnostics; NetCDS limited. VIDEO palette passes. Ordinary guest VGA writes
pass both fonts in Win3.1; BIOS and nested-helper application do not. Failed
helper candidates were removed with source-hash-verified restoration.
Frame commit/rollback and off-CPU execution helpers pass focused host tests.
Run152 passes the isolated real NTSRV/Run16/NTVWM chain through production
BOP entry, capture and the worker-owned executor: both54:08 forms return37/38
in two Win3.1 VMs;54:0A returns0 through the approved DOS carrier. A fallback
to diagnostic entry now fails the test. Input remains a translated-key fixture.
Five host suites pass. Physical guest input, fonts, complete I/O and final
returned-key delivery remain unaccepted; full product delivery gates remain.
Cached complete x86/x64 builds and shared tests pass. Generation-reuse rejection
was reproduced and fixed in host tests; real combined reuse remains unproved.
Chain158 and real Win3.1 continuation159 pass. The chained gate runner lost
USERPROFILE; fresh-process release160 passes focused/Full selected gates.
Run161 passes real native-command execution followed by forced VM retirement,
stale-wake rejection and normal Win3.1 exit on the production worker.
Run163 also passes actual VM handle reuse and a new native command returning37.
Run164 confirms native CMD absence before fixture scope cleanup.
Runs166/167 pass both deliberate native exit orders with full callback checks.
Forced retirement overlap has host-unit proof; full I/O remains S6 work.
Win95 is deferred.
[S5 P1 host checkpoint](../etc/evidence/m0-t479-s5-p1-host-checkpoint.md) is
published at both destinations; commit/push pending. S5 is not closed.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask frontends](../proposals/proposal-multitask-guest-vdm-frontends-001.md). |
| Identifier Mode | M0 T479 S5/multitask-command; Ordinary Mode, single implementer/reviewer. |
| Admission And Approval | Owner approves continuous S2--S7 delivery and subsequently prioritizes complete Win3.1 before Win95. S4's bounded Win3.1 handoff admits S5; Win95 proof remains mandatory before T closure. |
| Objective | Connect both 54:08/0A to independent host commands and restore the actual calling VM's continuation and complete client state once, while other VMs and guest Windows keep running. |
| Non-goals | No original guest rebuild/patch, hardware V86 monitor, CPU30, new polling/timeouts/control transport, placeholder I/O or semantic concessions. Full I/O and nested acceptance remain S6; no new Win95 work yet. |
| Reference Baseline | S4 Win3.1 bridge proofs and published ten-image package ca3b367a-3d54-4d05-a9a8-147594d7450d, Full 16/16. Candidate ntvdm64.386 has actual callback/wake/retirement/reuse/arm-failure evidence; it does not yet execute host commands. |
| Files And ABI Surface | Project ntvdm64-386 driver, NTVDM command preparation/continuation and VM frontend ownership, existing NTSRV/RPC lifecycle and tests. Minimal justified mirror command/BOP seams must be registered. Add-ons only assets/release. |
| Applicable Rules | Execution, architecture/coding/document rules and source policy; preserve original guest and register minimum mirror differences. |
| Verification | Source trace both BOPs, real guest continuation/register/CF/AL/stack state, two VM commands completing in both orders, independent request data, normal/failure/retirement paths. Cached full builds, applicable full product gates and coherent publication per S. |
| Expected Markers | No host wait inside CCPU BOP or cached guest pointer across scheduling. Actual VM identity/generation; NTSRV lifecycle authority; measured guest/other-VM progress. No dummy provider. |
| Asset Needs | Immutable Win3.1 installation copied to isolated fixture, verified normal package and independently authored add-on; stable Ninja/ccache roots. |
| Reporting Requirements | Separate source facts, implementation and tests; record unproved boundaries. |
| Stop Conditions | New semantic concession, original guest modification, unknown provenance or incompatible architecture needs explicit disposition. Missing proof is not a pass. |
| Exit Criteria | Both BOP continuations and independent execution pass actual Win3.1 tests; required complete host/add-on publication, reviewed commit and push. S6 receives complete I/O scope, not a reduced substitute. |
| Original Owner Request | Execute host-first S2--S7 continuously; first run Win3.1 correctly, defer Win95 work. |
| Similar-Issue Sweep | Both BOPs; native/DOS descendants; reordered completion, VM death/reuse, nesting/redirection; unchanged non-bridge path. |
| Temporary Evidence | build/t479-s5-command; owned 60-minute phases, 2 GiB, 15-minute no-progress review. Retain build/t479-s4-driver until durable handoff checks finish. |

## S4 Win3.1 Checkpoint And Deferred Win95

[Evidence and P deliveries](../etc/evidence/m0-t479-s4-driver-load.md) record
actual identities, 320 callbacks, counted and blocked cross-VM waits, native
thread/PIC wake, cancellation, forced queued retirement, actual handle reuse,
normal exit and injected initialization rollback. ntvdm64.386 is published as
a bridge candidate only. Ten host components remain the Full 16/16 P1 package.

[The roadmap](../etc/operations/m0-t479-guest-bridge-plan.md) retains S4's
Win95 wait/wake/lifecycle proof after the Win3.1 S5/S6 path and before S7/T
closure. It is not passed or dropped. Full guest ICW1-reset combinations and
exhaustive VMM allocation failures remain explicitly unverified in S7's matrix.

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
