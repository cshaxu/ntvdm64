# Proposal — Worker control, lifecycle and ownership naming unification

## Status and objective

Owner-requested planning dated 2026-10-03: put the component-divergence findings
into a candidate T proposal at Queue position 3 for unified repair. This is an
unnumbered implementation candidate, not admission or expansion of the active
T425 packet. Numeric T allocation and bounded S admission occur only in
[CURRENT](../states/CURRENT.md), under [Execution Rules](../rules/EXECUTION.md).

The owner's subsequent 2026-10-04 direction moved this candidate to the queue
head before execution-performance recovery. This is historical ordering, not
the present queue: [T428 closure](../history/m0-t428-worker-interface-unification-closure.md)
records the later admitted audit and delivery. The inventory below is retained
research; it is not an open duplicate repair packet or expansion of T432 S6.
Current ordering belongs to Queue and admission belongs to CURRENT.

Consolidate project-owned worker control, frontend routing, association,
occupancy and termination mechanisms across NTVDM and NTVWM, and align names
with their actual owner and semantic scope. Keep run16 a thin classifier,
submitter and result waiter; NTSRV owns authenticated admission, relationships,
direct results and orderly retirement; NTCON owns presentation and direct I/O;
NTMON consumes the service projection. Worker-local execution and resources
remain local. Original DOS/WOW records and completion remain at their owners.

This candidate follows the Console-root management and root/search-isolation
candidates. Consume their delivered interfaces instead of rebuilding their
trees, registries or path policy. The later
[task-trace proposal](proposal-worker-task-trace-observation-001.md) retains
Direct/Observed observation; this package does not implement its descendant
capture, modal or hooks. The
[architecture](../design/ARCHITECTURE.md) and
[coding rules](../rules/CODING.md) retain ownership and source-first authority.

## Investigation baseline and implementation findings

Inputs are the 2026-10-03 working-tree source audit and owner corrections:
already unified paths must not appear as open defects; implementation
divergence and naming debt must be distinguished. T425 S3 is delivered;
T425 S5 already replaces DOS-specific text/DIB decoder names. At admission,
refresh against the actual delivered baseline and remove resolved rows from
the repair checklist with evidence. The locations below are navigation
references to current functions, not frozen line numbers or runtime proof.

| Finding and current point | Unified repair and retained boundary |
| --- | --- |
| run16 DOS completion invokes `run16_frontend_scope_resume_parent`; [main.c](../../src/run16-exe/main.c) and [frontend_scope.c](../../src/run16-exe/frontend_scope.c), which reads Console members and selects a native worker before sending resume. | NTSRV decides the eligible parent restoration from authenticated admission/completion and execution relationships. run16 submits the result/control request without discovering a parent worker from local membership. Keep original worker block/resume and final-paint/input barriers. Record how existing direct-parent identity is sufficient, or the smallest necessary copied control extension. |
| NTSRV `OpenNtBaseServiceFrontendRequest` and `OpenNtBaseServiceAttachFrontendRequest`, [frontend_registry.c](../../src/ntsrv-exe/opennt/source/frontend_registry.c), search independent native routes and DOS launcher connections separately. | After each kind's original admission, use one worker-backed frontend route lifecycle for request retrieval and attachment. Preserve one authoritative execution record and one I/O association; do not create a second scheduler. |
| Frontend connection rundown in the same file uses route `native_worker` to decide retention/deletion after launcher exit. | Express validity, delivery, cancellation and worker/root lifetime explicitly in the common route. Preserve original DOS request cancellation and survive a short-lived launcher where admitted work remains valid. A neutral rename alone is not closure. |
| `OpenNtBaseServiceRetainCommandWorker`, `OpenNtBaseServiceSelectNativeWorker` and `service_native_root_selectable`, [worker_registry.c](../../src/ntsrv-exe/opennt/source/worker_registry.c), have separate selection/proof paths. | Consolidate the outer admitted-worker reference and validation result. DOS/WOW source-owned selection and native reservation/root proof remain bounded adapters. Never silently select a replacement after the selected worker dies. |
| Console matching/resident DOS fallback in `frontend_registry.c`; `service_root_has_worker` and root retirement in [lifecycle.c](../../src/ntsrv-exe/opennt/source/lifecycle.c) use Console association versus `native_root`. | One NTSRV-owned root-worker association and common retirement traversal, accepting kind-specific authenticated evidence. Keep execution ancestry, I/O association and residency distinct. Reuse the preceding Console-root management model, including missing-root handling. |
| `OpenNtBaseServiceTerminateWorker`, `lifecycle.c`, uses native stop/closed acknowledgments versus DOS/WOW `TerminateProcess`. | One public stop/result contract with worker-local resource closure and explicit failure. Native success must prove its hidden Console session closed; carrier death alone is insufficient. Preserve original DOS/WOW close semantics and bounded waits. Do not invent recursive descendant termination. |
| `OpenNtBaseServiceFrontendUsage`, `lifecycle.c`, derives native occupancy from inflight state and DOS occupancy from original records. | One occupancy projection with explicit pending/busy/idle meanings; source-specific readers supply facts. Retained completion receipts and observed task traces cannot pin execution or determine completion. |
| `OpenNtBaseServiceSnapshot`, `service_copy_management_record` and `service_copy_win32record`, [management.c](../../src/ntsrv-exe/opennt/source/management.c), perform an outer native supplement after the first projector returns. | One management projection entry and common enumeration/result initialization. Keep DOSRecord, WOWRecord and Win32Record readers and their lock/lifetime rules explicit. NTMON never reconstructs these facts. |
| Repeated `fVDM`, `native_worker`, `wow` combinations across frontend/worker registration and [native_commands.c](../../src/ntsrv-exe/opennt/source/native_commands.c); classification fields in [service_internal.h](../../src/ntsrv-exe/opennt/include/service_internal.h). | Establish a consistent project-owned classification source and bounded role/capability predicates. Keep imported `fVDM` compatibility and endpoint-specific permissions. Do not replace authorization with a permissive generic worker check. |

## Divergence requiring bounded adapters

Repair duplicate mechanisms within these paths, while preserving the stated
semantic difference. Each admission checklist row must state the shared outer
mechanism, retained adapter and actual positive/negative evidence.

| Current divergence | Required disposition |
| --- | --- |
| run16 DOS/PIF/WOW/native and CUI/GUI classification in `main.c`. | Retain target classification and original arguments; consolidate compatible submission/error transactions only. |
| WOW InitTask startup acknowledgment, DOS/native-text direct completion and GUI default startup-only / explicit `--wait`. | Share waiting mechanics where contracts match; keep startup and execution completion distinct. |
| DOS Console/session and frontend lease handling versus WOW exclusive-Console release. | Express character-frontend requirements clearly; retain redirected streams, actual Console state and no invented WOW character binding. |
| Native reservation serialization and WOW's absent DOS Console identity in [base_reservation.c](../../src/ntsrv-exe/opennt/source/base_reservation.c). | Common reservation transaction; preserve original DOS/WOW cardinality and native concurrent-start exclusion. |
| Claim/rollback/disconnect invokes original BaseSrv VDM update/cleanup only for DOS/WOW in [service_core.c](../../src/ntsrv-exe/opennt/source/service_core.c) and `worker_registry.c`. | Common outer resource/registration transaction with original-shaped kind-specific cleanup. Native records must not become synthetic DOS/WOW records. |
| Original VDM configuration/creation flags versus NTVWM hidden-Console startup in `worker_registry.c`. | Common creation/rollback mechanism with explicit local launch profiles and distinct resource requirements. |
| `service_unbound_native_deadline` and `unbound_native_deadline`. | Audit whether common admission/binding-phase policy replaces the type test. Preserve existing DOS/WOW failure boundaries and GUI residency; do not apply a blanket timeout merely for symmetry. |
| DOSRecord/WOWRecord/Win32Record task/state conversion. | Common management contract, retained record-family readers and source-defined completion. |

## Naming repair inventory

| Current name and point | Target meaning |
| --- | --- |
| [frontend_session.h](../../src/ntcon-exe/frontend_session.h) (formerly native_console_frontend.h), matching `.c`, former `run16_native_frontend` family. | NTCON-owned common frontend; remove misleading launcher and native-worker qualifiers from project-owned names and filenames. |
| [console_channel.h](../../src/ntcon-exe/console_channel.h), `run16_console_channel_*`. | Frontend-owned worker I/O channel. |
| [console_frontend.h](../../src/ntcon-exe/console_frontend.h), `run16_console_frontend` and dispatch family. | Frontend-owned Console operation state/dispatch. |
| [console_video.h](../../src/ntcon-exe/console_video.h), `run16_console_video_*`. | Frontend-owned copied video/publication state. |
| [window_keyboard.h](../../src/ntcon-exe/window_keyboard.h), `frontend_native_keyboard_*`, `FRONTEND_NATIVE_KEY_RECORDS`, delivery `native`. | Windows Console input-record/layout conversion shared by both workers; distinguish record format from worker kind. |
| `run16_frontend_scope_launch_native` versus `launch_gui`, `frontend_scope.c/h`. | Explicit Win32 text versus Win32 GUI request names; both are native. Their internal implementation is already shared. |
| `service_copy_management_record`, `management.c`. | Common projection entry or accurately named VDM-specific reader, consistent with the implementation repair above. |
| Route `native_worker`, `native_root`, `unbound_native_deadline`. | Rename only after route lifetime, common association and applicable admission-phase models are corrected; do not conceal unchanged type policy under neutral names. |

Select final spellings at admission from current semantics and existing naming
conventions. Update actual callers, headers, manifests, link boundaries and
production-linked tests; remove displaced aliases and dead parallel paths.
Preserve imported names and external compatibility spellings. Truly Win32-only
command/resource state such as `native_commands` and `native_inflight` may
retain accurate kind-specific names with a recorded rationale.

T425 S5's `decode_text_frame` and `frontend_window_decode_frame` are resolved
decoder naming, not pending defects here. NTCON format decoding and Console/
Window backend branches remain valid. NTMON's DOS/Win16/Win32 label mapping is
valid presentation, not an execution unification defect.

## Proposed bounded implementation sequence

1. Refresh the delivered source inventory; freeze source/ownership, retained
   adapters, copied ABI, lock/resource contracts and per-finding tests. This
   is the entry to repair, not a replacement audit-only outcome.
2. Consolidate frontend route request/attach/cancel/rundown and root-worker
   association against the delivered Console-root model; remove superseded
   launcher-dependent route paths.
3. Move parent-restoration arbitration out of run16; consolidate admitted
   worker selection, classification/role/capability validation and compatible
   registration/reservation/creation transactions.
4. Consolidate occupancy/management projection and stop/acknowledgment paths;
   resolve admission-phase deadline policy with original-semantics proof.
5. Complete remaining ownership/format naming and production caller/build/test
   updates, then integrated regression, source/diff review and owner handoff.

These are candidate stages, not active S allocations. Reorder bounded stages
at admission where dependencies require it without dropping any finding.

## Verification and exit criteria

Each implementation stage links exact production callers, tests, assertions,
commands and evidence for its admitted change. Extend current reservation,
worker/frontend control, common management, channel lifetime and frontend scope
fixtures instead of copying service implementations into tests. Relevant
existing sources include `tests/adapter-basesrv/base_service_reservation_test.c`,
`tests/app/common_worker_control_test.c`, `tests/app/common_management_test.c`,
`tests/app/console_channel_lifetime_test.c` and
`tests/app/frontend_scope_lifetime_test.c`. Names are fixture starting points,
not claims that their current coverage proves this proposal.

Cover concurrent startup, resident reuse, launch failure/rollback, launcher
departure before and after route delivery, cancellation, stale/cross-root
requests, worker/root/broker loss, independent roots, blocked parent occupancy,
pending handoff, duplicate stop, stop-before-ready, close failure and actual
resource closure. Exercise DOS -> native -> DOS and native -> DOS -> native,
including `run16 cmd -> run16 command -> cmd -> run16 command`, continuous
screen/cursor/input return and final-paint barriers. Retain WOW startup-only,
GUI default/--wait and redirected-stream cases.

Pass affected Win32/x86 /MT CCPU40 builds, focused production-linked tests,
the retained Console17/Window17, lifecycle/RPC/version/WOW gates and strict
repeated DIR/EDIT and rapid nested-return workloads. Review ABI/version changes
only where the actual wire contract changes. Each code-bearing P follows the
existing coherent eight-file recovery/publication and delivery rules. Physical
or unavailable checks retain their actual limitation, never an invented pass.

Closure requires a resolved disposition for every implementation and naming
row: shared mechanism actually production-wired with displaced paths removed,
or a source-proven semantic adapter with focused evidence. A neutral wrapper,
mass rename or completed inventory alone cannot close implementation divergence.

No new helper, generic worker framework, scheduler, execution/observation
registry, Job descendant mechanism, I/O relay through NTSRV, or relocation of
original DOS/WOW execution is included. No guest/firmware mutation, imported
library rewrite or arbitrary process enumeration is included. Follow source
policy before any later original-source research; unexpected original semantic
changes require bounded re-admission. This proposal introduces documentation
only and makes no runtime completion claim.
