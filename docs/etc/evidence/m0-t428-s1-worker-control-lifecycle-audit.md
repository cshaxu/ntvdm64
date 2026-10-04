# T428 S1 — current worker control/lifecycle audit

## Admission, inputs and method

Owner admits the queue head on 2026-10-04 and requests current-state review
and architecture decisions first. S1 is source audit only, not production
repair or whole-task closure. Baseline HEAD 0e1597632; production 62a71fd90,
accepted T427 S5 system32 set, APP0.0.427/RPC38/I/O25. Side-session proposal/
queue additions are read as current planning and preserved.

Read the AGENTS authorities and source policy. Trace production callers,
service state, resource ownership and existing fixture entrypoints with rg
and Get-Content; no build, tests, desktop interaction, process enumeration/
termination or deployment. Locations below name current functions rather
than asserting a frozen line number. Original source semantics are retained
boundaries, not an import or a full mirror-diff audit in this S.

## Existing shared mechanisms: do not rebuild

- [worker_spawn](../../../src/ntsrv-exe/transport/worker_spawn.c): both
  creation profiles already use suspended creation, authenticated reservation,
  pre-resume resource preparation and failure rollback. Its transient startup
  Job is disarmed before Resume; it is not descendant observation/lifetime kill.
- [worker-base connection](../../../src/worker-base/connection.c): NTVDM and
  NTVWM share broker connect/watch/disconnect and I/O acquire/release RPCs.
  NTVDM bootstrap/console client and NTVWM main are actual callers.
- [frontend_registry](../../../src/ntsrv-exe/opennt/source/frontend_registry.c):
  FrontendRequest/AttachFrontendRequest already traverse one selected route,
  not separate DOS/native delivery loops. WorkerIoTransition reserves one
  active connection until worker and frontend acknowledge closure.
- [management](../../../src/ntsrv-exe/opennt/source/management.c): SnapshotCopy
  uses one tree builder and copied-row allocation. NTMON consumes it; no local
  process census or new task observation belongs here.
- Both synchronous text results already return through NTSRV. Actual DOS
  records versus NTVWM authenticated native results remain separate sources;
  GUI/WOW startup acknowledgment is not execution completion.

## Refreshed proposal disposition

| Item / source owner and location | Current fact / judgment | Minimal target; independent boundary |
| --- | --- | --- |
| Project run16 main DOS completion and frontend_scope resume_parent | Still reads actual Console members and selects a native worker before a zero-payload resume request. Thin-launcher gap remains. | NTSRV authorizes parent I/O restoration after the exact consumed child completion. Existing console_context stores root/console, not an explicit parent-worker identity; do not claim it already proves the whole parent relationship. Preserve final-frame/input acknowledgment and original DOS resume. |
| Project NTSRV frontend_registry FrontendRequest/AttachFrontendRequest | Shared request/attach loops already exist. Old proposal's two-loop description is stale. | Keep these loops; consolidate remaining authorization/cancellation predicates, not introduce another route abstraction. |
| Project NTSRV service_clear_frontend; OPENNT_FRONTEND_ROUTE.native_worker | Launcher rundown still treats an undelivered native route differently from an undelivered DOS route. | Make command cancellation versus delivered worker association explicit. Preserve canceled prepared DOS worker identity/tombstone and never silently select a new root. Behavior tests precede deleting the type field. |
| Project NTSRV worker_registry RetainCommandWorker/SelectNativeWorker | Common admitted-worker retaining entry already exists; selected_native_generation and service_native_root_selectable remain native-specific. | Common validation/retaining outer operation; original Check/Update and native reservation/root proof remain explicit adapters, with exact selected worker pinning. |
| Project NTSRV lifecycle service_root_has_worker/retire_expired; worker_registry RegisterNativeBackend; management bind_root | DOS lifetime checks Console association; native uses native_root plus Console fallback; management_root_generation is a parallel association projection. | One authenticated service-owned root-worker association drives lifecycle and monitor projection. Retain Console identity only for admission/reuse, not as a second persistent relationship authority. Determine bind/unbind/reuse timing before deleting fallback. |
| Project NTSRV service_terminate_worker | Native signals native_stop, waits native_closed then process; DOS/WOW directly TerminateProcess. Native controls register only in RegisterNativeBackend with a frontend. | Common management validation/pinning/result contract; resource close operations stay worker-specific. Consider existing shared shutdown instruction plus native closure acknowledgment registered independently of frontend. Do not replace original DOS/WOW stop semantics without explicit approval. |
| Project NTSRV FrontendUsage | native_inflight contributes pending Boolean; original DOS records contribute task count. Retained completed Win32 records are not occupancy. | Common pending/active/idle projection from explicit readers. Console lease-return eligibility is not identical to resident-worker lifetime or monitor stack. No synthetic task list. |
| Project NTSRV management service_copy_worker | One entry initializes row, calls DOS/WOW projector (native early-return), then supplements native. | Dispatch once to DOS/WOW/native record reader; common row/result initialization remains. Original records/locks and Direct-only native stack remain. |
| Project NTSRV service_core/connect, service_internal classification | reservation_kind/watch.kind, native_worker/wow and imported fVDM coexist. | One project-owned kind plus finite predicates; retain original fVDM carrier and permission checks. Frontend requirement is a route property, not equivalent to native kind because native GUI requires no text route. |
| Project NTSRV service_unbound_native_deadline | Native worker lacking frontend_associated/root/inflight gets ten-second deadline; WOW skipped. GUI record outlives carrier. | Requires explicit residency policy decision; do not generalize timeout to WOW/DOS or delete it for naming symmetry. |
| Project run16 classification/start/wait; original srvvdm/cmdmisc | DOS shared/exclusive, WOW startup-only, native text completion and GUI startup-only/--wait differ legitimately. | Retain source-owned decisions. Common transaction mechanics only; original DosSessionId/CloseOnExit terminate-or-Inactive behavior stays in cmdmisc. |
| Project reservation/registration/rollback | CreateKind/claim and suspended spawn already shared; original Update/Cleanup used only for DOS/WOW; native serializes per execution Console. | Consolidate outer cleanup/resource moves, not move original algorithms into common/worker-base or manufacture native DOS records. |

## Ownership naming

[NTCON native_console_frontend](../../../src/ntcon-exe/native_console_frontend.h),
[console_channel](../../../src/ntcon-exe/console_channel.h),
[console_frontend](../../../src/ntcon-exe/console_frontend.h) and
[console_video](../../../src/ntcon-exe/console_video.h) still expose
run16_* names although NTCON owns them. window_keyboard's native names mean
Windows INPUT_RECORD conversion, not an NTVWM-specific renderer.
Rename owner/format names and all production/test/build callers together;
retain true native command/resource names, imported spellings and public CLI.
Resolved text/DIB decoder names are not reopened. NTMON kind labels remain
DOS/Win16/Win32 and numeric mapping 0/1/2. No executable rename is proposed.

## Architecture decisions for owner review

1. One service-owned root-worker association, not separately authoritative
   native_root/Console fallback/management parent relations. Recommend accept;
   keep pinned Console evidence at admission, not erase it globally. Store
   relationship in existing service-owned worker state; no new registry.
2. Parent restoration coordinated by NTSRV at direct child completion, not
   run16 membership selection. Recommend accept. Add only the missing verified
   parent binding to the existing execution context if needed; no Windows
   ancestry inference, descendant hooks or scheduler. A bare completion event
   must not be published before required restored-parent I/O acknowledgment.
3. Win32 GUI-only worker residency: current native unbound ten-second policy
   differs from shared WOW residency. Choose retain bounded native retirement
   or align independent GUI carriers with shared WOW residency. GUI target
   remains alive independently and default launcher returns on startup in both
   choices. This is a product lifetime decision, not an automatic cleanup.
4. Explicit close: recommend common validation/pinned target/shutdown contract,
   retaining native true Console-closed proof and original DOS/WOW management
   behavior initially. Register native close acknowledgment before frontend
   attachment so GUI-only/pre-text carriers have a valid management boundary.
   A mandatory cooperative-then-force timeout policy for all workers would
   change DOS/WOW semantics and needs separate owner approval; not assumed.

All pure frontend naming and compatible outer-mechanism cleanup needs no new
product policy choice. Shared service authority stays in NTSRV, worker-only
client mechanics in worker-base and neutral transport in common. No NTCON
worker-kind dispatch, I/O relay, helper, new queue or new scheduler.

## Tests to extend, not results from this audit

[base_service_reservation_test](../../../tests/adapter-basesrv/base_service_reservation_test.c)
already contains native-worker/backend, frontend root/rundown, cancellation,
management stop, launcher survival, completion-rundown, WOW late-query and
explicit-time deadline cases. [common_worker_control_test](../../../tests/app/common_worker_control_test.c)
covers the common RPC client envelope. [common_management_test](../../../tests/app/common_management_test.c)
and [frontend_scope_lifetime_test](../../../tests/app/frontend_scope_lifetime_test.c)
are candidate consumers; the latter contains client stubs and cannot alone
prove authenticated parent selection or real input restoration.

Implementation must add paired DOS/native tests for common root associations,
launch cancellation before/after route delivery, no cross-root adoption,
parent resume before receipt publication, native close before first text bind,
GUI-only residency and duplicate/failed close. Then run actual bidirectional
nested text handoffs, retained Console17/Window17/WOW and coherent publication
for each production delivery. This S ran none of those tests. Existing sealed
T427 gates remain evidence for unchanged baseline only.

S1 remains open for owner architecture review. Implementation stage boundaries
will be set after decisions, not silently allocated by this audit.
