# S2 Console boundary ledger

## Question and baseline

Move user-facing DOS Console I/O to run16 without replacing original guest
devices, execution or re-entry. Baseline c39b9ca0c; selected x86 CCPU40.
This initial source audit is not production or runtime closure.

## Procedure and findings

Search ntvdm-exe and mvdm/softpc.new/host/src for Console read/write,
scroll and buffer operations; inspect original caller bodies before choosing
transport. Paths below are repository-relative.

| Family | Current producer/consumer | Required disposition and proof |
| --- | --- | --- |
| Input | nt_event.c::nt_event_loop waits for Console input, suspend and prepend events, reads one record without blocking, then applies nt_rdp_normalize_key and key history. | Keep normalization and guest dispatch in worker; replace host-read/wait with transported records plus cancellation. Test ordering, prepend, suspend, key releases and queue exhaustion. |
| Stream output | nt_graph.c::host_stream_io_update calls WriteConsoleA with explicit byte count. | Preserve byte/codepage and ordering in run16; periodic cell snapshots cannot replace scrollback or final output drain. Test MEM and long output. |
| Cell output/readback | nt_fulsc.c uses WriteConsoleOutput and ReadConsoleOutput around guest text transitions. | Preserve rectangles, attributes and readback; audit selected branches before choosing local frame state versus frontend reply. Test EDIT then MEM and native return. |
| Scroll | nt_graph.c calls ScrollConsoleScreenBuffer. | Ordered frontend operation, not latest-frame-only delivery; retain fill and cursor rules. Test small viewport and scrollback. |
| Resize | ntvdm-exe/win32/console_compat.c::console_resize_grid preserves rows across modern Console reflow. | Move host grid mechanics to frontend while retaining existing source-derived ordering. |
| Native transition | dos/command/cmdexec.c::cmdCreateProcess and cmdExec32 retain original execution/completion. | Establish existing suspend/resume points and output barrier before wiring handoff. Process existence alone cannot determine input ownership. |

The native-child binding in ntvdm-exe/command/source copies bounded guest
command/environment/stream inputs into session-owned state. It is not a user
Console reader; its guest leases must not move into run16 merely for UI ownership.

## Recovery and remaining work

Keep original MVDM event/video/COMMAND bodies selected. Console server/CSR is
a stopping boundary, not a new import. The owner-required frontend split admits
a minimal copied I/O protocol; exact authentication, layout, capacity, failure
and teardown contracts must be reviewed before implementation.

The reference branch contains worker-owned presentation/console_route,
graphics_api, graphics_buffer and graphics_console. These are comparison
candidates, not approved imports; do not copy their worker-owned frontend
composition wholesale. No library import in S2; S4 uses latest nxvm libraries.

- [ ] Complete selected caller/handle/mode/flush/title/cursor inventory.
- [ ] Prove native yield/resume and nested launcher association.
- [ ] Register finite protocol, authentication and disconnect behavior.
- [ ] Implement/test complete input/output/handoff production paths.
- [ ] Pass S2 regression/negative/lifecycle gates and publish coherent package.

No production file, binary, guest or runtime configuration changed in this
audit. S1 build-only waiver does not apply to S2.

## Original yield/resume contract established

Read cmdexec.c::cmdExec32/cmdCreateProcess, cmdmisc.c::cmdGetNextCmd and
nt_event.c::nt_block_event_thread/nt_resume_event_thread/nt_process_suspend_event.
The source order is now guarded by
tests/observation/verify-dos-frontend-handoff-source.ps1; six assertions passed
on c39b9ca0c production source. This is a source-order test, not runtime proof.

1. cmdExec32 calls nt_block_event_thread(0) before starting cmdCreateProcess.
2. Block signals hConsoleSuspend and waits for hConsoleWaitStall. The input
   thread acknowledges and waits on hConsoleWait; it is not terminated.
3. Block flushes stream/video output, returns unused hardware and BIOS keys,
   flushes mouse events and restores original input/output Console modes.
   Only after this returns may the native thread start its child.
4. cmdCreateProcess increments original re-entry, creates suspended, releases
   guest capture, resumes/waits the child, obtains its exit code and decrements
   original re-entry. No frontend scheduler should replace this ordering.
5. Meanwhile cmdExec32 calls GetNextVDMCommand with
   NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND. A command sets IsRepeatCall;
   cmdGetNextCmd later resumes the blocked event path after receiving it.
   With no command, cmdExec32 returns native completion and resumes DOS itself.
6. Resume sets DOS Console mode and restores device/timer state before waking
   the event thread. A native parent can still be alive when nested DOS resumes.

Therefore a frontend lease follows original block/resume, not child process
exit or a new READY/BUSY classifier. The protocol needs an acknowledged input
stop plus output/returned-input drain before native execution, and an acquired
DOS input route before worker input wakeup. It must permit these transitions
while an outer native child remains alive. Existing task completion continues
through BaseSrv, never through a frame-channel EOF.

## Input queue and identity review

Current console_compat.c implements VDM prepend in a worker-local list because
public WriteConsoleInput only appends. Original nt_event consumes that list
through a separate wait event. Moving public reads alone is insufficient:
frontend-prefetched records, worker-returned keys and native Console input
must have a single explicit order at yield. In particular, copying returned
keys into a worker-only list does not prove a native consumer can read them.
This is an S2 migration obligation, not a newly proven baseline defect.
Do not silently flush/discard pending keys to simplify transition.

Existing basesrv transport/service.idl authenticates the process with typed
process attachments and connection generation; Prepare binds the registered
worker. Console membership helper accepts pinned authenticated process IDs
and supplies membership observation only, not identity or reuse authority.
Reuse this authentication boundary to authorize a frontend/worker association;
do not trust an inherited environment PID or duplicate Console HANDLE numbers.
Current IDL has no frontend channel registration: the final association API
and lifecycle need a separately reviewed finite addition, not task-record policy.

Remaining before production implementation: complete API/handle inventory,
exact association/transport contract, and queued-key/native handoff proof.
No S2 runtime pass is claimed by the source guard.

## Build-selected Console surface inventory

Run tests/observation/audit-dos-console-build-surface.ps1 against the S1 formal
build.ninja; add -Callsites for reproducible file/line candidates. The script
removes comments/string literals, reads only graph-selected worker C files,
and does not generate output outside build. It is not a preprocessor or call
graph: definitions and inactive branches remain candidates. It reports 408
selected C units, 199 lexical occurrences, 56 spellings and these 15 files:

| Original-relative file under src/mvdm, unless noted | Occurrences | Initial owner disposition |
| --- | --- | --- |
| dos/command/cmdconf.c | 1 | Title/configuration presentation boundary; original policy stays. |
| dos/command/cmdkeyb.c | 3 | Layout/codepage queries versus guest keyboard work must be distinguished. |
| dos/command/cmdmisc.c | 4 | Task title/input-return boundary; original completion/re-entry stays. |
| dos/command/cmdpif.c | 1 | Original PIF Console operation, not a second display policy. |
| softpc.new/host/src/config.c | 9 | Initial Console geometry/mode setup and queries. |
| softpc.new/host/src/nt_cga.c | 9 | Graphics invalidation/producer boundary; no move of guest renderer. |
| softpc.new/host/src/nt_det.c | 4 | Display detection/state queries. |
| softpc.new/host/src/nt_ega.c | 12 | Graphics invalidation/producer boundary. |
| softpc.new/host/src/nt_event.c | 17 | Input acquisition, yield/resume and native mode restoration. |
| softpc.new/host/src/nt_fulsc.c | 44 | Text buffer/readback, cursor/geometry, transition and graphics buffer binding. |
| softpc.new/host/src/nt_graph.c | 25 | Stream, text, scrolling, palette and host display binding. |
| softpc.new/host/src/nt_hosts.c | 10 | Host startup/control/mode ownership. |
| softpc.new/host/src/nt_mouse.c | 11 | Host pointer/menu/display operations versus guest mouse semantics. |
| softpc.new/host/src/nt_vga.c | 16 | Graphics invalidation/producer boundary. |
| src/ntvdm-exe/win32/console_compat.c | 33 | Existing public Console boundary implementations; select/move mechanics, do not duplicate them. |

This inventory is deliberately not called complete I/O reachability: aliases,
headers/macros, ReadFile/WriteFile and standard-handle operations need separate
review. Formal preprocessing must distinguish selected CCPU code from dormant
MONITOR branches. S2 establishes frame/mode contracts; S4 owns actual graphics
Window rendering, so selected graphics producers are retained, not rewritten.

## Existing association mechanism reuse

base_reservation.c already records launcher PID/generation, task, Console
identity and a retained worker process. PrepareWorker checks launcher identity
and live worker, forbids replacement, and pins the handle; ClaimWorker checks
live process plus connection generation. base_service.c separately retains
authenticated connections, reservation, Console and registered-worker state.
These are the candidates for the minimal frontend capability binding, not a
reason to add another task registry or change srvvdm scheduling.

A launch reservation alone does not resolve resident-worker reattachment or
nested run16. Those paths must use the worker selected by original Check/Get
and authenticated Console association. No environment variable or numerical
worker identifier supplies authority. Before implementation, specify root
frontend lifetime separately from per-command wait lifetime, including root
exit while original worker remains resident. Broker conveys capabilities only;
it never transports input/frame payloads.
