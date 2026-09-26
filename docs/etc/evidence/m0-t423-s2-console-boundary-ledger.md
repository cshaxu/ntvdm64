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

## Resident worker selection and frontend handoff design

base_service.c::OpenNtBaseServiceCheck first binds authenticated Console
membership. If the original Console record exists but has no pending original
GetNextVDMCommand wait, the current binding supplies null Console and original
CheckDOS chooses a separate session. Otherwise the original Check reply selects
the resident worker and parent wait. run16::launch_vdm currently only waits
that parent event in the VDM_PRESENT_AND_READY branch. New frontend registration
must therefore cover both new-worker Update and this resident-worker Check
result; startup reservation alone is insufficient.

Required association rules for implementation:

- Existing live root frontend: nested run16 remains a submitting/waiting
  client. It must not read Console input or replace the root endpoint.
- Resident worker without live frontend: the newly selected command's launcher
  may acquire frontend ownership only through the authenticated service result.
  Worker input/output waits for the new route before consuming the command;
  no timed polling or replay of the guest command is introduced.
- Separate original worker/session: it gets its own frontend association;
  Console membership cannot silently graft it onto an unrelated worker.
- Root/worker disconnect: revoke route generation and cancel channel waits.
  Preserve original task/lifetime failure handling; no artificial successful
  completion on EOF, no reconnect/replay during a live task.
- Native handoff: stop user input reads, drain acknowledged output, return
  pending keys with defined ordering, restore modes, then acknowledge release.
  Original resume reacquires the DOS route before waking the guest input loop.

The registration conveys only verified process/endpoint capabilities; the
payload channel is direct run16/worker. A copied packet contains version,
generation, operation, sequence and bounded payload sizes, never Console HANDLEs
or pointers. Input, stream output, cursor/resize and barriers retain order;
frame coalescing may not discard any of those operations. Define op-specific
payload validation and negative tests before connecting an untrusted decoder.

## Compiled-edge audit

The inventory script accepts -Dumpbin with the selected MSVC x86 dumpbin.
It inspects every selected worker object, including files without lexical
Console calls (function tables/macros can still create references). It reports
undefined function symbols and decorated Console imports, excluding ordinary
data globals. Missing selected objects fail instead of silently reducing scope.
This distinguishes compiled references from dormant source branches; it still
does not prove link reachability or runtime execution. Objects come from the
recorded S1 formal graph, not a fresh S2 production build.

Direct object inspection confirms nt_graph's WriteConsoleA, scroll/fill/cursor
imports. The complete script run with MSVC 14.43.34808 Hostx64/x86 dumpbin
passed and reported 73 compiled Console function references, including
function-table references in cmddisp.c and nt_reset.c missed by the lexical
call pattern. Assertions confirmed both __imp__WriteConsoleA@20 and
_ReadConsoleInputExW@20 were present. Direct inspection also confirms
nt_fulsc's ReadConsoleOutputA. nt_fulsc's lexical WriteConsoleOutput
occurrences are not corresponding imports in this selected object. Original
shared-text invalidation is handled by console_compat instead. Do not implement
all lexical occurrences as separate protocol operations merely to check boxes.

## In-progress command-worker authentication

### Direct channel association contract

The next binding stores one pending duplex pipe and a pinned selected-worker
reference on the existing authenticated launcher connection. No new registry
or command state machine is created. Original Check/Update remains the only
worker selector. A second live launcher for that worker receives ALREADY_EXISTS
and cannot replace the root frontend. Only the registered DOS worker may take
the pending pipe once; it also receives a query/synchronize-only reference to
the authenticated frontend and its connection generation. Pipe bytes never
pass through BaseSrv. Rundown closes pending capabilities; an already delivered
pipe is owned by its two endpoints and their process-liveness contract.

Recovery exception: these are new transport attachments required by the approved
frontend split, composed in existing base_service beside original CSR identity
substitutions. Original CSR/Console server transport cannot be imported under
the stopping-boundary rule. Existing typed-resource and connection ownership
are reused; no Console HANDLE, guest pointer or nominated PID crosses the API.
Unit tests must prove one-time delivery, duplicate-root rejection, wrong role/
generation, payload bypassing broker, and rundown closure. RPC and real DOS
handoff tests remain additional gates; a service-only pass is not delivery.

OpenNtBaseServiceRetainCommandWorker is now a WIP BaseSrv binding, not a mirror
change. Only an authenticated waiting launcher receives its original selected
DOS worker's non-inheritable query/synchronize handle. Selection reuses the
existing new-worker reservation or resident worker watch, not a caller PID.
No new registry, scheduling, termination right or frame transport is added.
Wrong generation, no pending task, completed task, worker/WOW caller and failed
worker are rejected. RPC exposure is wired; frontend use is not yet wired.

Recovery classification: original srvvdm Check/Update selection remains
unchanged. Existing base_service/reservation already replaces unavailable CSR
process identity; this finite capability query serves the explicitly admitted
frontend split. Importing CSR is excluded and another registry is unnecessary.
The final consumer must use the restricted reference for peer authentication;
the unused method alone is not a delivered capability.

Updated native service fixture passes new/resident worker identity, wrong
generation, no command, completed command, noninheritance, denied termination
and denied worker self-query. The reservation fixture's removed numeric-handle
APIs were replaced by real pipe one-time transfer and EOF assertions; it passes.
These are x86 /MT native service tests, not DOS end-to-end acceptance.

Builds reuse the mutable S1 formal graph/cache incrementally, not a sealed
evidence snapshot; changed sources recompile. An initial standalone test lives
under build/M0-T423/S2/association-tests. No intermediate is outside build.
The worker_failed edit has been rebuilt and both native reservation/service
fixtures rerun successfully. This implementation is uncommitted WIP, not deployed;
complete frontend wiring and production
regression gates before P delivery. S1's waiver does not apply.

### RPC continuation results

IDL and generated-interface references consistently use major 5; product
identity is 0.0.423/protocol 5. Correcting the stale v3 ifspec references fixed
the compile failure. Incremental x86 /MT build of run16, basesrv, ntvdm, dtmgr,
VDMREDIR and both reservation fixtures passed in the recorded formal cache.
WOW32 and six-file publication are not revalidated; deployment remains S1.

Verify-BasesrvProduct.mjs now builds the actual basesrv-exe client, not the
retired adapter path. Its first run in build/M0-T423/S2/rpc-association passed
registration, version/generation rejection, no-command CommandWorker rejection,
typed streams, disconnect and unreserved-registration rejection. The old
reservation fixture failed its exit-code assertion after waiting for worker
disconnect/exit before reading completion. Original srvvdm records completion
before teardown; base_service invokes BaseSrvCleanupVDMResources on worker
cleanup. Querying after cleanup does not prove retained task completion.

The fixture now holds the worker on a test-only event until the parent reads
completion and checks real RPC worker PID, noninheritance and denied termination
before resuming it. Initial reruns were refused with 1740 when an O:/winnt
broker acquired the endpoint during compilation. The next inspection found
the endpoint free: the real RPC suite then passed, including the revised
positive identity and completion case. No endpoint bypass or reconnect policy
was introduced. This is not yet DOS I/O integration.

### Direct channel attachment results

Next payload slice: copied, bounded text-Console operations retain the reached
nt_graph/nt_fulsc Win32 contract (stream bytes, fill, scroll, cursor, attributes,
geometry and mode query). The run16 dispatcher owns native input/output handles;
wire records contain none. A version, connection generation and strictly
increasing request sequence reject stale/malformed traffic before any side
effect. Win32 result/error and transferred counts are returned explicitly.
Stream payloads are bounded chunks, never inferred NUL-terminated strings.
An ordered barrier acknowledges preceding operations; it is not task completion.
This is the admitted frontend transport seam, not a replacement guest renderer.
Original Console-server internals remain excluded; original callers retain
their API shapes through worker-local bindings when this dispatcher is wired.

AttachFrontend and TakeFrontend are implemented in the existing service/RPC
client and formal IDL. A pinned worker and pending pipe live on the launcher
connection. The selected registered worker takes that pipe once and receives
the frontend's query/synchronize reference and generation; broker never reads
or writes payload. Root identity persists after Take until rundown or worker
exit, so a nested launcher cannot replace it. No imported mirror changed.

The x86 service reservation fixture passes all three invocations: default,
--management-terminate and --frontend-rundown. Checks cover wrong generation,
wrong role, non-pipe rejection, noninherited delivery, denied termination,
double-attach/take, nested-root rejection, bidirectional F/W bytes and EOF
after endpoint closure. The rundown case proves a pending endpoint closes
when its root disconnects and cannot subsequently be taken. Management cleanup
already releases reservations from its process watch; its assertion now accepts
the same success-or-already-removed result as the existing abrupt-exit case.

After rebuilding the IDL and affected x86 product targets, the real RPC suite
passed again with AttachFrontend/TakeFrontend in the production service. The
test worker acquires the endpoint, exchanges F/W bytes directly with its parent,
rejects a second Take and completes the original command with exit code 7.
Version/generation, receipts, explicit disconnect and process-exit rundown
regressions also passed. This establishes channel handoff, not a frame protocol
or migrated Console. run16/ntvdm production I/O consumers are still unwired;
input/output framing, range/order checks and native yield/resume remain next.
No P or six-file publication is claimed for this uncommitted implementation.

### Text operation dispatcher

The new run16-exe/console_frontend.c is a frontend-owned public Console
dispatcher; product-abi/console_io.h contains only fixed-width copied data.
Version, generation, nonzero consecutive sequence (including exhaustion),
payload length, operation, SHORT coordinate bounds and narrowed fields are
validated before side effects. Stream bytes have an explicit length and 16 KiB
bound. Screen info, stream counts, Win32 success/error, modes, cursor and
scroll/fill semantics are retained. Barrier only acknowledges serial dispatch;
it does not yet certify a transport or native handoff barrier.

New-T310OriginalSoftpcNinja.ps1 adds the selected run16 object and a native
console-frontend-test.exe target. The formal x86 cache was regenerated and
both targets compiled/linked. tests/app/console_frontend_test.c ran in a real
CREATE_NO_WINDOW Console using ProcessStartInfo (UseShellExecute=false,
CreateNoWindow=true, redirected stdout/stderr, 15-second timeout). It passed
actual A/B stream rows, moved ### cells and 0x2e attributes, cursor size and
visibility, input/output mode reads, invalid-handle errors, and malformed
version/generation/sequence/size/range rejection. This is native Console proof,
not a CCPU/DOS runtime result. No user desktop was switched or manipulated.

Next: connect this dispatcher to the authenticated byte channel and source-
shaped worker calls; then complete input readiness/return and original native
yield/resume ordering. Do not call a linked but unused dispatcher migrated I/O.

### Initial production stream channel and real DOS regression

The next uncommitted slice connects run16's cancellable overlapped channel pump
and worker-local console_client to the unchanged WriteConsoleA call shape.
BaseSrv still transfers capabilities only. Worker initialization takes the
authenticated endpoint; WOW receives NOT_SUPPORTED and retains its existing
route. Native result/count/error returns are preserved; a failed DOS transport
is latched rather than replayed through local Console output. Session ABI is 9.
The formal x86 build and affected WOW32 incremental link pass; logs are
build/M0-T423/S2/console-channel-build.log and console-channel-wow-build.log.

The first real DOS run from build/M0-T423/S2/p exited 1067 before task
completion. Its observed short root remained 41 characters. The existing
product-package validation constructs the original 64-byte COMMAND SHELL
value from two copies of that root, so this package fails validation before
the channel bootstrap. No guest bound or source contract was relaxed.

The same six candidate binaries were temporarily tested at O:/winnt on the
observer's private desktop, using Verify-CommandExitStatus.ps1 with
MVDM_OBSERVER_PRIVATE_DESKTOP=1 and the S1 formal console-startup-observer.exe.
Cases direct-mem and command-c passed (log prefix
m0-t423-s2-channel-shortroot). Cases empty, native-zero, mem, nested-empty,
nested-mem, mem-repeat, edit and dos-native-dos passed (log prefix
m0-t423-s2-channel-interactive). These checks assert guest text, expected
counts/interaction and exit codes, not just process creation. Raw logs are in
O:/winnt/logs. The edit case includes returning from EDIT and running MEM.

All six S1 binaries were backed up under
build/M0-T423/S2/baseline-db4ab61eb2db41b296d49d3ce6b8de6d before replacement.
Both runs used finally cleanup to stop exact package processes and restore
all six files; restored SHA-256 values matched the backup. Guest media and
configuration were not modified. O:/winnt remains the S1 side-test baseline.

This proves these ten regression cases with the partially wired candidate,
not that every output byte traversed the new transport or that S2 is complete.
Input, remaining screen calls, native-return ordering and resident frontend
renewal still require wiring and targeted ownership/failure tests. Full DOS17,
independent WOW frontiers, coherent final publication and P delivery remain
open. No S1 waiver is applied to S2.

### Screen-operation production binding

The next S2 slice adds same-shaped GetConsoleScreenBufferInfo,
SetConsoleCursorPosition, SetConsoleCursorInfo, FillConsoleOutputCharacterA,
FillConsoleOutputAttribute, ScrollConsoleScreenBufferA and
SetConsoleTextAttribute bindings. Original mirror source remains unchanged.
All use the same serialized session channel and sequence as WriteConsoleA;
native operation errors do not poison the channel, whereas transport/framing
failures do. Shared exchange code replaces the stream-only framing duplicate.
Rectangle/cell readback, resize ownership, input and native handoff are still
incomplete, not silently counted as covered by these seven operations.

tests/app/console_client_test.c is built as console-client-test.exe in the
formal graph. Its session/TakeFrontend binding is explicitly test-only; it
exercises the production worker client, a real pipe and production frontend
dispatcher against distinct Console screen buffers. It passes stream placement,
screen-info reply, cursor, fills and scrolling on the frontend, unchanged local
cells, native invalid-coordinate failure followed by a valid operation, and
peer EOF with latched failure/no local replay. The existing frontend native
fixture also passes. Both ran with CREATE_NO_WINDOW, a 15-second bound and no
user-desktop interaction. Build evidence is under build/M0-T423/S2 in
console-screen-build.log, console-client-test-build.log,
console-screen-final-build.log and console-screen-wow-build.log. These native
tests establish transport ownership, not guest or whole-S2 acceptance.

The rebuilt six-component set then passed Verify-CommandExitStatus.ps1 cases
direct-mem, command-c, nested-mem, edit and dos-native-dos with private-desktop
observation and log prefix m0-t423-s2-screen-transport in O:/winnt/logs.
The same recoverable six-file backup/finally restoration was used and all
restored hashes matched. These are regression passes for the new screen
bindings, not final side-test publication or P delivery.

### Rectangle cells and readback

Read the original OpenNT windows/core/ntcon/client/iostubs.c
ReadConsoleOutputInternal/WriteConsoleOutputInternal: clip to caller buffer,
copy the selected rectangle through a CSR capture and return native rectangle
and status. CSR remains an excluded system boundary. Same-shaped A/W bindings
now marshal bounded fixed-width cells instead, without editing original MVDM
callers or guest renderers. Reply framing version 2 has a 112-byte header and
an explicit optional payload; ordinary scalar replies do not send 16 KiB.
Cell layout is checked against CHAR_INFO. An 80x50 frame fits one tile;
larger visible extents can be tiled rather than rejected for wire capacity.

The native transport fixture now exercises nonzero caller origin, negative
screen-left clipping, two-row Unicode cells and attributes, ANSI read/write,
untouched clipped-out caller cells and an overwide 5,000-column request.
A direct native API comparator found and prevented a genuine tiling error:
a wholly off-screen write tile returns success with its request rectangle
unchanged; unioning it falsely enlarges the actual transferred rectangle.
The client now obtains frontend geometry and clips before tiling. A wholly
off-screen request is still submitted to the native provider: read returns
ERROR_INVALID_PARAMETER and updates the rectangle, unlike write. The fixture
compares both the failure and returned rectangle, not just BOOL success.
Both native fixtures pass after the correction; malformed dimensions/byte
counts also fail before effects. This is native provider evidence, not an
assertion that every possible resize race or large visible grid is verified.

The corrected production and test targets build in the same x86 cache;
console-cells-layout-build.log and console-cells-wow-build.log under
build/M0-T423/S2 record the final layout-checked build. Input ownership,
resize mechanics, native handoff and resident attachment renewal remain open.

The final six-component candidate passed the private-desktop real-package
direct-mem, nested-mem, edit and dos-native-dos cases with output assertions;
logs use prefix m0-t423-s2-cell-transport under O:/winnt/logs. Finally cleanup
restored all six S1 binaries and verified their backup hashes. This remains
partial S2 WIP, with no final publication, commit, push or closure claim.

### Resize ownership

Moved the existing ADAPTER-WIN32-052 console_grid_transfer/console_resize_grid
body from ntvdm-exe/win32/console_compat.c to the existing host-compat root's
console_grid.c, exporting opennt_console_resize_grid. Its algorithm and error
handling are retained; the graph compiles one object into both executable
owners. This stateless named public-Console binding is shared, not copied.
Worker MvdmSetConsoleScreenBufferSize/MvdmSetConsoleWindowInfo now marshal
DOS requests to run16; no-channel native callers retain the same local binding.
No mirror file or guest changes. No source directory was created.

The real-pipe client fixture adds viewport change, 80x25 to 40x12 shrink,
original cursor-containing row selection (rows 9..20), cursor clamp (0,11),
expansion, and an assertion that the distinct worker buffer's geometry did
not change. Under CREATE_NO_WINDOW, shrink failed with error 87; the direct
native SetConsoleScreenBufferSize comparator failed identically. This is a
fixture environment limitation, not a passing resize test or a reason to
change production behavior. The same fixture passed on the observer's normal
Console with MVDM_OBSERVER_PRIVATE_DESKTOP=1, without switching user desktop.
O:/winnt/logs/m0-t423-s2-resize-native.txt records exit 0 and the captured PASS.
Build evidence: console-resize-final-build.log and console-resize-wow-build.log
under build/M0-T423/S2. Initial compile rejected a test variable named small
(SDK macro); renaming that local variable fixed it without product changes.
Input/wait/prepend and native yield/resume ownership remain the next S2 work.

The coherent six-component candidate also passed direct-mem, nested-mem,
edit (including return to MEM), and dos-native-dos in the private desktop.
Verify-CommandExitStatus.ps1 logs use m0-t423-s2-resize-transport under
O:/winnt/logs. Finally cleanup restored all six baseline binaries and checked
their hashes. These are partial-S2 regression results, not S2 closure or a P.

### Raw input read ownership

Rechecked nt_event.c's three-object wait, ReadConsoleInputExW NOWAIT call,
nt_process_suspend_event acknowledgement, nt_block_event_thread output drain /
ReturnUnusedKeyEvents / ReturnBiosBufferKeys / Console-mode restore, and resume
ordering. The original consumer reads one raw record before RDP normalization
and guest dispatch. No new prefetch thread/queue is needed to move the read API.

ReadConsoleInputW and PeekConsoleInputW now have same-shaped worker bindings to
the run16 dispatcher. Copied input records contain only explicit fixed-width
fields, with type/narrowing/count/payload validation; native union padding and
handles never cross the channel. Worker-local prepend and paired Alt+Enter
filtering, original key history and machine input remain unchanged. Production
NOWAIT uses the original peek-before-read sequence; its suspension acknowledgement
still occurs on that original thread before native child execution. The channel
stop path cancels a synchronous Console read, including the entry/cancel race,
and drains pipe operations before freeing storage.

The native real-pipe fixture passes exact key make/break, repeat count, virtual
key, scan code, Unicode character and SHIFT state, mouse row/column, moved flag,
button down/release, non-consuming peek and empty peek. It ran on the normal
Console in the existing private-desktop observer; report
O:/winnt/logs/m0-t423-s2-input-native.txt has exit 0 and captured PASS. The fixture
does not prove the production channel-thread blocking-read cancellation race;
that lifecycle test is still required. Build logs console-input-build.log and
console-input-wow-build.log are under build/M0-T423/S2.

This is raw-read migration only. GetConsoleInputWaitHandle still returns the
worker's old shared-Console handle, and VDM prepend still resides in the worker.
Next migrate readiness through an authenticated typed event capability (no
input bytes through BaseSrv), then complete returned-input/native ownership
handoff and test pending-read/peer-death cleanup. Do not infer readiness or
native input exclusivity from a successful raw-record fixture.

The candidate six-component build then passed all 17 default cases of
Verify-CommandExitStatus.ps1, using the S1 formal observer, O:/winnt package,
MVDM_OBSERVER_PRIVATE_DESKTOP=1, log prefix m0-t423-s2-input-dos17 and the
generated test-only build/M0-T423/S2/G7.COM fixture. Guest text, expected
interaction/counts and exit assertions all passed, including EDIT return to
MEM. Original media was unchanged. Finally cleanup restored all six S1
binaries and verified their hashes. This complete regression run does not
close the remaining S2 ownership/lifecycle or WOW-frontier gates.

## Owner-requested WOW Console isolation side fix

The owner reported a visible `Hidden Console of WOW VDM` before WINMINE.
The original `mvdm/softpc.new/host/src/config.c` sets that title for WOW;
the launcher was allowing the WOW worker to inherit its visible Console.
Read-only comparison of OpenNT `base/win32/client/process.c`, lines
1322-1329, establishes the original rule: WOW uses CREATE_NO_WINDOW and
clears CREATE_NEW_CONSOLE/DETACHED_PROCESS. This is the original parent-side
creation contract, not a new hidden-native-text transport or a guest repair.
The full original CreateProcess unit is outside the admitted system boundary;
run16 already owns its finite public process-creation binding.

`run16-exe/main.c` now selects CREATE_NO_WINDOW for WIN16/SEPWOW only.
Its creation flags are assembled from scratch without either conflicting
flag; the DOS new-Console condition is unchanged. No mirror or guest changes
were made. The command startup record captured by BaseCheckVDM is unchanged;
WOW32 `wkman.c` consumes that record's show state. No SW_HIDE is injected.

Verification (MSVC 14.43.34808 Win32/x86 /MT; outputs exclusively below
`build/M0-T423/S2/wow-console-side`):

- `node tests/app/wow_console_startup_test.mjs`: passes source ordering,
  WOW-only flag selection, separate guest startup state and retained DOS rule.
- Formal graph's `obj/run16/entry.obj` compiler arguments, with only output
  redirected to the isolated run root: production main.c compiles; compile.log.
- `cl /MT /W4 tests/app/wow_console_native_test.c`, with /Fo and /Fe inside
  that root, linked /subsystem:console and user32.lib: native-build.log.
  Running the resulting wow-console-native.exe passes Console read/write,
  nonvisible Console and independent GUI ShowWindow visibility assertions.
  The child runs on a private desktop which is never switched to the user.

This native test does not execute CCPU40 or WOW guest code and is not WINMINE
acceptance. Full product relink, DOS/WOW runtime regressions and coherent
publication remain pending in the main delivery flow. O:/winnt was not changed;
no commit/push or S/P closure is claimed by this side fix. Earlier advice to
use CREATE_NEW_CONSOLE plus SW_HIDE is superseded by the original-source rule.

## Input readiness capability: in-progress verification

The direct frontend now publishes a manual-reset input-ready event alongside
its pipe. BaseSrv verifies the event type and retains only SYNCHRONIZE for
worker delivery. The frontend observes Console readiness without consuming
records; read/peek refresh the event after dispatch. The worker's original
GetConsoleInputWaitHandle facade selects this event. No mirror was changed.

The x86 incremental production build passed (console-ready-build.log under
build/M0-T423/S2). The updated basesrv-service-reservation-test.exe passed both
the default original Check/Update/Get/Exit lifecycle and --frontend-rundown.
Assertions include wrong-object rejection, notification set/reset visibility,
denial of worker SetEvent, non-inheritance and one-time delivery. Build evidence
is console-ready-service-build.log in the same build root.

These are local service assertions, not cross-process RPC or guest acceptance.
The updated real RPC fixture subsequently passed through Verify-BasesrvProduct.mjs
with OPENNT_BROKER_PRODUCT_BUILD=build/M0-T423/S1/restart-formal-x86 and
OPENNT_BROKER_TEST_BUILD=build/M0-T423/S2/rpc-ready. Its reservation case proves
cross-process notification delivery and denied SetEvent, alongside original
command delivery; reconnect, rundown and version rejection also passed.

The worker handle classifier now uses native GetConsoleMode, not FILE_TYPE_CHAR:
NUL is a character device but must not become a remote display. The real-pipe
console-client-test compares NUL WriteConsoleA failure against the native API.
The private-desktop observer report m0-t423-s2-ready-null-native.txt in
O:/winnt/logs records exit zero and the captured PASS. This supplements the
existing distinct-buffer, cell/input field, clipping and EOF assertions.

After incremental production/WOW relinks (console-ready-null-build.log,
console-ready-final-build.log and console-ready-wow-build.log below the S2
build root), Verify-CommandExitStatus.ps1 ran all 17 cases with the private
desktop observer, O:/winnt, prefix m0-t423-s2-ready-dos17 and build-root G7.COM.
All text/exit assertions passed, including EDIT, repeated/nested MEM, native
streams/EOF and exit-seven routes. The temporary coherent six-file candidate
was replaced in finally by the saved S1 set and each restored hash matched.

Abrupt frontend death, native-return ownership and remaining S2 closure gates
remain open. At that run, original nt_hosts/nt_fulsc/nt_event mode setting and
cursor query still called worker-local APIs; subsequent wiring is recorded below.
O:/winnt remains the coherent S1 baseline; no publication or P is claimed.

## Mode and cursor query wiring

Original nt_hosts/nt_fulsc/nt_event GetConsoleMode/SetConsoleMode and
GetConsoleCursorInfo now bind through the same-shaped conapi facade to the
ordered frontend channel. The original startup, block/resume and raw/cooked
call order is unchanged. Native handle capability checks distinguish input
from output; no mode state machine or mirror-file change is introduced.
WOW/no-channel and non-Console handles retain their native path.

console-mode-build.log below build/M0-T423/S2 records successful x86 product
and fixture compilation. console-client-test on the observer's unswitched
private desktop passed: different local/frontend output modes, remote query,
remote set without modifying the local buffer, input mouse-mode toggle and
restore, and remote cursor query. The captured PASS and zero exit are in
O:/winnt/logs/m0-t423-s2-mode-native.txt and its .console.txt capture. Existing
cell/input/error/EOF assertions in the same fixture also passed.

This is production wiring plus native verification, not a refreshed DOS17 or
WOW acceptance claim. Those runtime gates must run against the resulting
coherent six-file build before a production P. Native-return prepend, remaining
title/code-page/flush ownership and frontend-death handling remain open.

Mode wiring subsequently passed real private-desktop direct-mem, nested-mem,
edit and supplemental dos-native-dos cases through Verify-CommandExitStatus.ps1
(prefix m0-t423-s2-mode-handoff). The supplemental case enters interactive
cmd.exe, runs run16 mem there, exits CMD and runs MEM again in DOS; its captured
Windows banner and repeated guest memory text are assertions, not exit-only
evidence. The coherent candidate was built with console-mode-final-build.log
and console-mode-wow-build.log. This four-case pass is not the full P gate.

## Frontend death versus normal root completion: source decision

The existing base_rpc_client.c broker_lifetime_watch unconditionally terminates
on broker loss, which is correct for its approved failure boundary but cannot
be copied unchanged for frontend loss. Original srvvdm.c::BaseSrvGetNextVDMCommand
(lines 230-299) first publishes the finished DOSRecord exit code and signals
the parent's completion, then can return the ConsoleRecord wait for another
command. Parent exit therefore does not imply worker failure.

Original cmdmisc.c blocks the event thread before that completion request and
resumes after the next GetNextVDMCommand returns. Conversely nt_event.c caches
GetConsoleInputWaitHandle once before its loop (line 490), then treats failed
ReadConsoleInputExW as DisplayErrorTerm. Thus swapping/closing a readiness
handle under that loop is invalid; a per-worker stable wake handle is needed
across frontend generations. The current bootstrap-only TakeFrontend does not
implement renewal and must not be represented as resident-worker completion.

Required next implementation is to bind detach/reacquire at the original
command-completion/next-command boundary, preserving a nested live root. Only
loss during an active frontend lease is a failure; no unconditional frontend
watch kill, timer-based idle policy, command replay or new scheduler is admitted.
Tests must distinguish active root death, normal root completion with resident
wait, nested launcher exit, and acquisition by the next authenticated root.

## Stable input wake: first lifecycle binding

console_client now owns a stable local manual-reset wake event. A bounded
notification thread waits on stop, authenticated frontend process and input
readiness; after readiness it waits for a read/peek rearm instead of spinning
on the signalled source. It reads no Console records and selects no task.
Frontend process death wakes the cached original event-loop handle; transport
also rejects that dead peer before attempting I/O. Original ReadConsoleInputExW
failure handling remains the consumer, rather than a new unconditional worker
kill. Teardown stops/joins the notification thread before closing its handles.

The x86 product/fixture build passed (console-wake-build.log, followed by
console-wake-test-build.log below build/M0-T423/S2). console-client-test now
uses a real separate notification-peer process: readiness wakes the local
handle, and after explicitly resetting/asserting it nonsignalled, terminating
that process wakes the identical handle. The private-desktop observer's
m0-t423-s2-wake-native2.txt and .console.txt in O:/winnt/logs show exit zero and
captured PASS; existing native channel assertions also pass.

This proves local notification/death binding only. The fixture substitutes
broker delivery and the peer is not a DOS worker/frontend integration run.
Normal completion detach/reacquire, source replacement behind the stable event,
real active run16 death and refreshed DOS/WOW gates are still open. No runtime
package publication or P delivery is claimed.

Real active-frontend-loss acceptance was attempted through the existing
Verify-BrokerFinalLifecycle.ps1 with a new explicit -FrontendLoss case. It
requires worker failure while the observer still owns the Console, before
releasing scripted input; normal launcher loss remains a distinct historical
case. The attempt reached a real COMMAND prompt but stopped at the fixture's
owned-broker identification, before killing the frontend. Prefixes
m0-t423-s2-active-frontend-loss and m0-t423-s2-active-frontend-loss2 under
O:/winnt/logs are non-pass fixture evidence, not worker-death results.

The first restoration raced process termination and encountered an EXE lock.
An explicit follow-up verified no remaining package process, restored all six
files and checked each baseline hash. The second attempt waited for exact
package processes to exit before restoring; all six hashes matched. The fixture
now records owned IDs, process ancestry and observer reports when association
fails, to diagnose it without widening the set of controlled processes.

Ownership diagnosis: active-frontend-loss4-ownership.json captured all three
product processes with correct launcher ancestry, while the observer's final
report did not yet exist. That report is written only after exit. The old
fixture also excluded the live private-desktop observer relay from traversal.
CollectOwned now admits only the exact observer image and package paths in
the existing bounded ancestry walk; launcher selection follows the particular
live observer root rather than requiring its not-yet-written final report.

The corrected -FrontendLoss case (prefix m0-t423-s2-active-frontend-loss5) reached
COMMAND, killed its verified root run16, and **failed**: the worker did not exit
within five seconds while the observer still retained the Console. The finally
path restored and hash-verified all six S1 files. This contradicts any claim
that the local wake fixture proves full frontend-loss containment.

Source review finds nt_event's read-failure path invokes DisplayErrorTerm;
nt_error.c::DisplayErrorTerm calls host_error, which opens ErrorDialogBox and
returns ERR_CONT. Waking the reader therefore does not by itself guarantee
process completion. A runtime stop in that dialog is a hypothesis, not yet
proven by the failing process's stack. Next work must establish its actual
failure path and make active-lease loss bounded without treating normal root
completion/resident waiting as a fatal event.

The dialog hypothesis is now runtime-proven. A read-only test-owned worker
window snapshot (tests/observation/worker_window_snapshot.c, x86 /MT, object
and EXE below build/M0-T423/S2) enumerates the observer's explicitly named
private desktop, filters windows to the verified worker PID and reads text
with bounded SendMessageTimeout. It sends no input or close messages.
The -FrontendLoss run with prefix m0-t423-s2-active-frontend-loss9 captured a
visible #32770 window titled "16 bit MS-DOS Subsystem", containing "NTVDM has
encountered a System Error / The process terminated unexpectedly." and
Terminate/Ignore buttons. The worker remained alive. This directly joins the
frontend-death wake/read error with the source-proven interactive host_error
path; a notification-only repair cannot close active-loss containment.

Earlier attempts are retained as non-pass evidence: loss6 did not enumerate
the private desktop, loss7 did not reach the COMMAND prerequisite when run
under compiler environment, and loss8 lacked desktop enumeration rights.
Loss9 ran under ordinary environment and used a read-only OpenDesktop handle;
no desktop was activated. Every completed attempt restored/hash-checked the
six-file S1 baseline. The next repair belongs to the standalone active-frontend
lease boundary, not a blanket change to original error dialogs or resident
worker lifetime policy. The observation helper's final source additionally
returns after explicit-desktop enumeration to avoid redundant failed per-thread
queries; this cleanup requires rebuilding before its next execution.

## Active I/O peer-death containment

The direct worker exchange now terminates its own process with
ERROR_PROCESS_ABORTED (1067) only when an I/O request detects the authenticated
frontend process has exited. This reuses the existing standalone broker-loss
failure-containment mechanism; it bypasses the proven unusable interactive
error path without changing any original error-dialog or guest source.
The notification watcher does not kill on mere parent exit; an idle resident
with no active I/O is not assigned a new idle-exit policy. Normal route renewal
is still separate unfinished work, not certified by this change.

The coherent x86 formal/WOW build passed (console-active-loss-build.log and
console-active-loss-wow-build.log below build/M0-T423/S2). In the corrected
real -FrontendLoss test, prefix m0-t423-s2-active-frontend-loss11, COMMAND first
displayed its DOS prompt, the verified root launcher was terminated, and the
worker independently exited with exactly 1067 while the observer still held
the Console. The broker survived and a subsequent independent MEM produced
its real memory output and exit zero. The process handle is retained before
exit so the test rejects an unavailable ExitCode; the earlier loss10 attempt
had an empty ExitCode and is superseded, not an exact-code pass. All six S1
baseline hashes matched after restoration.

The same candidate subsequently passed all 17 text-gated DOS cases through
Verify-CommandExitStatus.ps1, prefix m0-t423-s2-active-loss-dos17, including
EDIT and direct/nested/return-code/native-stream paths. The temporary package
was again fully restored and hash-checked. This is non-regression evidence for
the bounded change, not proof of resident-frontend renewal or all S2 gates.

## Reachable resident renewal regression

Verify-CommandExitStatus.ps1 now has supplemental native-cmd-dos-repeat:
one native CMD /d /c runs two sequential independent run16 MEM.EXE roots in
the same Console. Unlike dos-native-dos, no original DOS root frontend stays
alive across the two submissions. It requires two actual memory reports and
zero final exit, so a first-command-only pass cannot satisfy it.

The current production candidate failed this case (prefix
m0-t423-s2-resident-repeat): captured Console has one MEM report, final launcher
exit is 0x42b/1067. The same coherent candidate's DOS17 pass therefore did not
cover resident renewal. All six baseline hashes matched after test restoration.

Original cmdmisc.c only terminates at this boundary when DosSessionId is
nonzero and CloseOnExit is set; an existing CMD Console keeps DosSessionId zero
and follows the original next-command wait. run16 explicitly preserves this
shared-Console branch. Original BaseClient vdm.c waits on WaitObjectForVDM,
sets ASKING_FOR_SECOND_TIME and retries without reconstructing the worker.
The current worker takes its frontend only during bootstrap, so the second
command encounters the departed first presenter. This is a reachable product
regression, not an optional speculative reuse feature.

Repair must reacquire a newly authenticated pending frontend before returning
the next real command to MVDM, retain an existing live root for nested commands,
preserve the stable local input-wake handle, and wait for attachment when
CheckVDM's command-ready notification wins the race against frontend registration.
No replay, new worker substitution, timed polling or original DOSRecord policy
replacement is acceptable. The new case remains failing until that production
binding and its race/negative checks pass.

## Command-delivery frontend renewal: initial production repair

BaseClient's existing copied Get reply now invokes a worker-registered local
binding before returning a successful non-wait, non-environment command. The
callback carries no guest data or new scheduling choice. A live root is kept
for nested commands. For a departed root, console_client stops/joins only its
notification thread, replaces authenticated pipe/process/readiness sources,
resets route sequence/failure, and restarts notification behind the unchanged
local wake handle before original COMMAND resumes its input thread.

The authenticated WaitFrontend RPC waits on a BaseSrv condition variable for
an endpoint attachment; it neither selects nor buffers commands. Attach,
frontend cleanup and worker termination wake the condition. A dead old root
pin is cleared during new attachment; a live root still rejects replacement.
This covers the race where original Check wakes Get before run16 attaches its
new endpoint. Nonblocking Take and its prior negative tests remain available.
No mvdm/opennt-host mirror source was edited for this repair.

The coherent formal/WOW build passed (console-renew-build.log and
console-renew-wow-build.log below build/M0-T423/S2). The formerly failing real
native-cmd-dos-repeat now passes with two captured MEM reports and exit zero,
prefix m0-t423-s2-resident-renew. The candidate was restored to the saved S1
six-file set and every hash matched. This proves the sequential workload, not
an independently asserted identical worker PID across both commands.

basesrv-service-reservation-test --frontend-wait also passed after
console-renew-wait-build.log: the worker's wait remains blocked for 100 ms
before attachment, then returns within five seconds; original lifecycle,
pipe exchange, synchronize-only event rights and one-time delivery assertions
remain checked. Cancellation/disconnect while waiting, multi-worker isolation,
full refreshed DOS17/WOW frontiers and final ownership closure are still open.

## Post-renewal regression and real RPC delivery

The unchanged renewal production candidate passed all 17 text-gated DOS cases
through Verify-CommandExitStatus.ps1, prefix m0-t423-s2-renew-dos17. This includes
native streams/EOF, nested COMMAND, repeated MEM, original exit codes and EDIT.
The temporary deployment restored all six S1 files and checked their hashes.
This closes the full DOS17 refresh for this candidate, not the separate WOW,
multi-worker isolation or remaining input-ownership obligations.

The x86 basesrv-service-reservation-test --frontend-wait-worker-loss was
re-executed and returned zero: death of the registered worker cancels its
pending frontend wait with ERROR_ACCESS_DENIED and publishes no handles or
generation. The fixture source contains the bounded waits and assertions;
this is a native service test, not an RPC-client-death test.

tests/app/base_client_rpc_first_test.c now also has --frontend-wait-parent
and --frontend-wait-child. The parent starts the registered child before
attaching presentation; the child announces its impending wait and calls the
actual OpenNtBaseClientWaitFrontend generated RPC route. The parent observes
the child alive before attachment, then supplies the channel. Existing pipe
round-trip, synchronize-only event, query-only process, one-time delivery,
original command and exit-result checks all remain required. The native
service --frontend-wait test separately proves waiting before attachment;
the client-side announcement alone is not proof of server wait entry.

Verify-BasesrvProduct.mjs runs the new case in its normal collection.
OPENNT_BROKER_PRODUCT_BUILD was build/M0-T423/S1/restart-formal-x86;
OPENNT_BROKER_TEST_BUILD was build/M0-T423/S2/rpc-renew-checked;
OPENNT_BROKER_TEST_LOGS was O:/winnt/logs. Its MSVC x86 /MT fixture build and
all real RPC assertions passed, including delayed delivery, version/role
rejection and rundown/reconnect. The owned test server was stopped.

The initial rpc-renew attempt failed only at the fixture's final reservation
release (line 247 at that revision), after the child had completed successfully.
Source inspection of service_worker_terminated and
OpenNtBaseReservationReleaseWorker established that the process-exit watch
already removes that exact reservation. Release after confirmed child death
can therefore return SUCCESS or NOT_FOUND; the fixture now additionally
requires a second release to return NOT_FOUND. No other error is accepted.
The corrected full rerun passed; no production semantics changed for this
test correction. S2 remains open pending final ownership and lifecycle/WOW
verification and coherent publication.

## Console handle direction and native prepend availability

The worker binding used GetConsoleMode alone to select both input and output
operations. CONIN$ passes that check too: a write/cursor call with an input
handle could therefore act on the frontend output instead of returning the
native invalid-handle result. console_client now separates mode-capable,
screen-buffer and input-queue classification. Mode calls accept both valid
directions; input/output calls retain their corresponding native failure path.
The real-pipe fixture compares wrong-direction WriteConsoleA, cursor query,
PeekConsoleInputW and ReadConsoleInputW against the native APIs. This does not
claim arbitrary access-right or all possible Console-handle coverage.

The x86 incremental build passed (console-direction-build.log under
build/M0-T423/S2). The private-desktop native fixture exited zero with its
captured PASS, prefix m0-t423-s2-direction-native. All 17 real DOS regressions
then passed, prefix m0-t423-s2-direction-dos17. All six deployed files were
restored/hash-checked against S1. WOW/frontier and publication gates remain open.

The input-return source audit revisited original OpenNT
windows/core/ntcon/client/private.c::WriteConsoleInputVDMW,
server/directio.c and server/input.c::PrependInputBuffer. The original API sets
Append=FALSE and serializes front insertion under the Console lock; a private
worker FIFO is not equivalent for a subsequent native child reading CONIN$.
Read-only dumpbin exports of this machine's SysWOW64/kernel32.dll showed that
WriteConsoleInputVDMW still exists (kernelbase lacks that named export).

The new tests/app/console_prepend_native_test.c checks actual behavior, not
symbol presence. Built with MSVC x86 /MT /W4 into
build/M0-T423/S2/console-prepend-native.exe and run through the unswitched
private-desktop observer, prefix m0-t423-s2-prepend-native, it seeded B,C via
WriteConsoleInputW and prepended A through the exact historical export.
Observed result=1, error=0, written=1, ready=3, order=0041 0042 0043; exit zero.
The test touches only its disposable Console input, not the user desktop.

This corrects the earlier assumption that a usable native prepend provider
was absent on this host. The export is a historical, undocumented interface:
this one-host result does not establish availability on every supported OS.
No binding has yet been switched by this probe. The next source-shaped route
is a copied frontend prepend operation invoking that exact native ABI in
run16, with explicit unavailable/error behavior, removing the worker FIFO
after order, repeated prepend, native-reader and handoff tests pass. Do not
replace it with non-atomic drain/append or claim the current FIFO is already
removed. The fixed wire record is reused; native handles never cross it.

## Native prepend connected; private worker queue removed

The subsequent implementation adds CONSOLE_IO_PREPEND_KEYS to the unpublished
S2 channel. WriteConsoleInputVDMW retains its original mirror callers and now
delegates to console_client; copied key scalars reach run16_console_dispatch,
which calls the exact kernel32 historical export. Missing export returns
ERROR_CALL_NOT_IMPLEMENTED, not append, fake success or a private queue.
No-channel callers use the same native export locally. The host owns the
atomic insertion and notification. No NT4 Console server/CSR implementation,
new scheduler or guest modification was imported.

Source inventory bounds are ReturnBiosBufferKeys' 32-record batch and
ReturnUnusedKeyEvents' MAX_KEY_EVENTS=100. Both fit one existing 16-KiB message;
the binding rejects oversize or non-key batches before sending anything.
The frontend validates the full batch before native insertion. This is a
finite reached-key contract, not a new unrestricted Console input API.

The old console_compat deque, allocation, lock, manual-reset event and private
read priority were deleted. nt_event restores original Events[2], the
two-object wait and status-zero input branch (3 additions/5 deletions relative
to current HEAD, reducing that mirror deviation). The removed extension's
header declaration and old fixture references were updated. The legacy
console_input_contract fixture now expects native input readiness to remain
set until both returned and pre-existing records are consumed; its separate
historical build graph has not yet been revalidated with S2's new link edges.

MSVC x86 /MT formal and WOW incremental builds pass:
console-prepend-build.log and console-prepend-wow-build.log under
build/M0-T423/S2. The real-pipe console_client fixture passes on the private
desktop (m0-t423-s2-prepend-channel): two returned batches precede four existing
records, with exact key make/break, repeat, Unicode and modifiers retained;
unwrapped native ReadConsoleInputW consumes them, proving they are not hidden
in the worker queue. Invalid type/oversize batches and empty writes are checked.
The frontend fixture also passes malformed length/type/capacity tests and its
existing operation checks (m0-t423-s2-prepend-dispatch). These are native
boundary proofs, not a claim of guest type-ahead handoff coverage.

All 17 real DOS cases pass after this production change, prefix
m0-t423-s2-prepend-dos17, including native child and nested/EDIT paths. The
temporary package restores and hash-checks all six S1 files. Targeted guest
type-ahead/native handoff, remaining Console APIs, isolation and WOW frontiers
still require completion before publication; no S2 P or closure is claimed.

## Continuous guest/native input and retained native contract

Verify-CommandExitStatus.ps1 now includes supplemental dos-native-typeahead,
the same COMMAND -> native CMD -> run16 MEM -> exit CMD -> DOS MEM -> exit
sequence without per-line waiting. A zero LineDelayMs is now passed explicitly
instead of being silently replaced by the observer's 1500-ms default. Ordinary
paired key emission and its per-character interval remain unchanged; this is
continuous input across owner transitions, not an assertion of instantaneous
whole-buffer injection or proof that every input record visited BIOS memory.

The coherent prepend candidate passes dos-native-dos, dos-native-typeahead and
native-cmd-dos-repeat under prefix m0-t423-s2-prepend-handoff. Captured real
Console output shows the native CMD banner, two complete MEM reports and the
return to DOS COMMAND; unexpected command-resolution errors are rejected.
Original expected exit codes remain 1 for the two interactive cases and 0
for the same-CMD independent-root case. All six S1 hashes match after restore.

The existing console_input_contract_fixture no longer references the deleted
private prepend-event function. It asserts shared native input remains ready
after consuming the returned front record while its original tail is pending,
then becomes non-ready after the tail is consumed. New-T310HostInputNinja.ps1
links the real Console client and shared grid binding. The fixture deliberately
stubs only unused frontend acquisition/command callbacks as unavailable, so
the actual production no-channel native API route is exercised.

That x86 /MT build passes in build/M0-T423/S2/input-contract, recorded in
console-input-contract-build.log; the unswitched private-desktop run exits zero
under prefix m0-t423-s2-input-contract. Its source assertions also cover the
existing 80x50 text registration/presentation contract. The older T388-specific
generator still contains pre-component-move paths and was not used or claimed
to pass. The current T310 entrypoint is the revalidated fixture build route.
Remaining work is final Console API ownership, multi-worker/failure coverage,
WOW frontier comparison and publication; these tests do not close S2.

## Native-root frontend ownership: reproduced acceptance failure

The owner's expanded S2 requirement retains the outermost run16 as sole
frontend even when its initial target is native CMD. Source review confirms
the native branch of run16/main.c only calls CreateProcessW and waits; it
does not connect/register or serve a frontend. Both channel-start sites remain
inside launch_vdm. AttachFrontend also requires the caller's original pending
DOS parent event through RetainCommandWorker. Consequently this surface can
authorize a DOS launcher but cannot represent an outer native launcher yet.

The existing default-off MVDM_S34_TRACE_PATH hook now records frontend-owner
only after successful channel creation. The new supplemental
Verify-CommandExitStatus.ps1 -Cases native-root-frontend runs native CMD under
root run16, then an inner run16 MEM. It first requires real MEM output and its
correct exit, then compares every recorded frontend owner with the observer's
root launcher PID. It does not accept shared Console membership, no extra
window or successful exit as frontend identity proof.

After root-frontend-observation-build.log's x86 launcher build, the real test
failed under prefix m0-t423-s2-native-root-owner: root=27188, actual=19332.
All six S1 files were restored and hash-checked. This is a current unmet S2
acceptance condition, not a DOS execution failure or a supported exception.

The required repair separates root-presentation registration from per-command
Check/Update admission. A native-root waiter must continue serving descendant
DOS I/O; inner launchers retain classification/submission/completion but never
take over presentation. Existing original command/worker selection remains
unchanged. Any association must carry authenticated kernel capabilities with
exact root/child lifetime and rights, not trust an environment PID, guessed
ancestor or Console membership alone. Worker pipe/frame traffic stays direct
to the root; BaseSrv carries registration/capability transfer only. Cross-hidden-
Console execution stays S3. This is the next implementation boundary; no new
root registration mechanism is claimed implemented by this evidence.
## Root capability boundary (implementation in progress)

The native-root ownership failure requires frontend registration independent
of an original DOS command receipt. Original `srvvdm.c` still selects commands
and workers; it cannot supply the product's run16 frontend lease because its
original Console frontend lives in the excluded NT Console server. Reusing
that server directly or behind a small facade would require the unavailable
CSR/Console-server lifecycle, and changing the guest/SoftPC cannot supply a
native launcher relationship. The approved frontend split therefore admits
a finite BaseSrv-owned authenticated resource association, not a new command
scheduler. Current attachment/worker selection is reused; no quarantined
display routing or alternate original-record policy is imported.

The proposed root reference is an unnamed kernel event retained by the
authenticated root connection. Descendants inherit a synchronize-only handle;
an environment value locates that local handle but is never accepted as
identity. Broker comparison must compare actual kernel objects received
through typed resource attachments. Independent events, dead/rundown roots,
wrong connection generations and duplicate registration must fail. The
association conveys no task completion, process-termination or scheduling
rights. Root/client rundown closes retained references. Task waits and original
exit-code queries remain per-launcher. No root ownership may be inferred from
Console membership or parent PID alone.

Native prerequisite evidence: `tests/app/frontend_capability_inheritance_test.c`
was compiled MSVC x86 `/MT /W4` with explicit object/executable paths under
`build/M0-T423/S2`. `O:/winnt/logs/m0-t423-s2-capability-inheritance.txt`
records direct, real CMD and two nested CMD launches passing. Each descendant
waited on the inherited event, was denied SetEvent, was released by the root,
and returned 37 through the native parents. CompareObjectHandles accepted a
restricted duplicate and rejected an independently created event. This proves
the current host's inheritance/object-comparison prerequisite only, not RPC
authentication, worker propagation, production frontend ownership or S2 closure.
The product still deploys the restored S1 six-file baseline.

`OpenNtBaseServiceRegisterFrontendRoot` and `RetainFrontendRoot` now implement
the service-local association: registration rejects named/auto-reset/non-event
objects, duplicate ownership and wrong caller generations; resolving compares
kernel-object identity and returns only a non-inherited query/synchronize root
process reference. Root disconnect removes the association and closes the lease;
a root process already exited is rejected before delivering a reference.
These functions do not select a DOS task/worker and are not yet wired to RPC
or launcher startup. No native-root product pass is claimed.

`basesrv-service-reservation-test.exe --frontend-root` passed with assertions
for restricted-duplicate resolution, independent-object rejection, wrong type,
named event, auto-reset event, wrong generation, duplicate registration,
disconnect and dead-root rejection. Build evidence is
`build/M0-T423/S2/root-capability-build.log`; runtime evidence is
`O:/winnt/logs/m0-t423-s2-root-capability.txt`. Initial test linkage exposed the
historical wchar CRT declaration for swprintf_s; the fixture uses its existing
ANSI sprintf_s/CreateEventA convention instead. Final build succeeds.
Existing `--frontend-wait`, `--frontend-wait-worker-loss` and default original
Check/Update/Get/Exit lifecycle cases pass again; logs have prefix
`m0-t423-s2-root-regression` and `m0-t423-s2-root-reservation-regression`.
Next work is typed RPC attachment, root notification/channel acquisition and
worker/native descendant propagation, followed by the failing real native-root
ownership assertion and full delivery gates. Unit evidence does not waive them.

The root operations now cross the selected typed RPC IDL and production
BaseClient/BaseSrv dispatch. The event is an `sh_event` resource attachment;
the returned restricted root reference is `sh_process`. Both operations use
the existing authenticated caller-process check and connection generation;
they accept no caller-nominated root PID. Client exception paths clear and
close any partial outputs. This extends the still-unpublished S2 protocol 5,
not the deployed S1 protocol.

Real RPC verification used `Verify-BasesrvProduct.mjs` with product build
`build/M0-T423/S1/restart-formal-x86`, test build
`build/M0-T423/S2/rpc-root-checked` and logs `O:/winnt/logs`.
The new `--frontend-root-parent` case registered the root, launched a separate
child holding only the synchronize-capability, rejected an unrelated event,
resolved the actual root PID/generation and a non-inherited process reference,
then disconnected the root and verified a second child cannot resolve the old
capability. Both child completion codes (37 and 38) were checked. The full
existing RPC verification passed too, including original reservation/command
delivery, delayed frontend attachment, stream/resource rundown and protocol/
app-version mismatch. Build log: `build/M0-T423/S2/root-rpc-build.log`;
focused runtime log: `O:/winnt/logs/m0-t423-s2-root-rpc.log`.

This is real service/client transport evidence, not yet the native-root
production ownership pass: run16 startup still does not register/join this
lease. Root notification and original-command-authorized channel attachment,
worker descendant capability propagation and launcher integration remain
required. Inner launchers retain their own original parent receipts/native
child process waits; root frontend lifetime must not replace those waits.

### Original-command-authorized root channel handoff

The service now represents channel attachments separately from their submitting
command connections. Each route retains its selected worker and points to its
authenticated root; root rundown and worker death remove routes under the
service lock. An inner command connection no longer has to own the route's
lifetime. This storage is frontend resource bookkeeping, not another DOS record
or task scheduler; `srvvdm.c`, Check/Update and original completion receipts
remain untouched.

`RequestFrontend` requires both the inherited root capability and the caller's
still-pending original DOS command. It signals the registered root event;
`FrontendRequest` gives that root the original selected worker plus the
requesting connection generation. `AttachFrontendRequest` revalidates that
same pending command before binding a channel. No caller supplies a worker PID.
The root event's retained service handle adds EVENT_MODIFY_STATE for this
notification; descendant duplicates remain synchronize-only. Empty request
scans reset the notification under the same service lock. An existing route
for the same root/worker returns ALREADY_EXISTS instead of replacing it.

Native `basesrv-service-reservation-test.exe --frontend-delegated` passes:
separate root and worker processes, unrelated-capability and wrong-request
rejection, notification/reset, selected-worker identity, attachment, and a
worker receiving the root's process/generation plus functioning pipe after
the submitting launcher connection has disconnected. This is service-boundary
evidence, not a real guest/launcher ownership pass. Exact build/run logs:
`build/M0-T423/S2/delegated-route-build.log` and
`O:/winnt/logs/m0-t423-s2-delegated-route.txt`. Default original lifecycle,
root capability, frontend wait, worker-loss cancellation and frontend rundown
native regressions also pass under log prefix `m0-t423-s2-delegated-`.

Typed RPC methods/client wrappers and the run16 root-request channel entry are
compiled and linked (`delegated-rpc-build.log`, `delegated-channel-build.log`
below the same build root). The channel entry reuses the existing dispatcher,
input readiness and teardown rather than introducing a second I/O engine.
Actual launcher root notification pumping, descendant environment/worker
capability rebinding and the new delegated RPC operation's end-to-end test
remain open. Multiple routes have storage, but independent simultaneous worker
ownership is not yet a runtime pass. Nothing in this entry closes S2 or
publishes the changed package.

After this route change, the full existing `Verify-BasesrvProduct.mjs` suite
passes again with test build `build/M0-T423/S2/rpc-delegated-checked`. This
rechecks existing generated RPC routes and root registration/resolution only;
it does not yet exercise the new delegated request RPC methods. Test-owned
broker exited normally and the O:/winnt runtime binaries were not replaced.

### Native-root launcher production path (partial wiring)

`run16-exe/frontend_scope.c` now owns a root notification pump and its direct
Console channel resources. A descendant resolves the inherited local handle
through the authenticated broker; it does not start another input reader.
The native CUI branch uses the existing original classifier's image-section
metadata contract (`vdm.c::GetBinaryTypeW`, NtCreateSection/NtQuerySection,
SectionImageInformation) to distinguish a native Console frontend from GUI
launch. This is frontend admission after original classification, not a new PE
parser or image execution policy. Native GUI retains its independent window
and process wait. Original DOS Check/Update/completion queries stay in place;
an associated DOS launcher requests root attachment, and the worker waits for
the route before consuming it. Native child waiting includes root-pump failure.

Formal graph includes the new scope source. Builds succeeded:
`build/M0-T423/S2/root-pump-compile.log` and `root-pump-wow-build.log`.
The first build invocation used wrong-case Ninja target VDMREDIR.DLL; the
correct graph target VDMREDIR.dll then built. This was a target spelling error,
not a product compilation pass on the first attempt.

Temporary six-file real-package tests on the private desktop:

- `m0-t423-s2-root-pump`, case `native-root-frontend`: PASS actual MEM output,
  native result 0 and owner PID equal to outermost run16. This repairs the
  previously evidenced outer-native-root/inner-DOS ownership failure.
- `m0-t423-s2-root-pump-repeat`, case `native-cmd-dos-repeat`: PASS two actual
  MEM reports and native result 0 while the root stays alive.
- Same prefix, case `dos-native-dos`: FAIL, only one of two required MEM
  reports appeared. The worker-to-native child path does not yet carry the
  authenticated root capability; ordinary DOS roots also still use the older
  command-local frontend startup. This branch must migrate to the same root
  scope, with authenticated worker capability acquisition and private native
  child environment rebinding. Do not put host handle locators into the guest
  environment. No S2 regression waiver is granted by the native-root pass.

Both test sessions restored all six baseline files and verified hashes.
The last case is an open regression in the uncommitted candidate, not an
accepted original limitation. Neither S2 nor a production P can close yet.
Remaining scope concerns include shell-fallback root entry, worker/native
propagation and root failure cancellation, then full DOS17/WOW/multiworker
delivery gates.

### Worker propagation and unified DOS roots

The previously failing DOS/native/DOS candidate path is repaired in production
bindings. Ordinary DOS roots now use the same root scope as native CUI roots;
COMSPEC fallback also joins/registers the frontend and continues native child
waiting. `WorkerFrontendCapability` only serves an authenticated registered DOS
worker whose existing route identifies a live root. It returns a
synchronize-only event attachment, not a caller-selected root/process ID.
The worker acquires it with the route and refreshes it on original command
delivery when the previous root has exited.

`command_process_compat.c::create_frontend_child` uses a per-call inheritable
restricted duplicate and builds a private native ANSI/Unicode environment
copy. It removes any guest-supplied locator, adds the local inherited-handle
locator and closes its own duplicate after CreateProcess. The original
`cmdexec.c::cmdCreateProcess` uses TRUE handle inheritance and retains child
suspension, original resume/wait and exit reporting. Neither its body nor
guest media is changed. `run16::begin_worker_win16_directory` filters this
private variable before BaseCreateVDMEnvironment, so no host handle locator
is introduced into the DOS guest environment. ADAPTER-WIN32-049 records this
standalone boundary; it is not an original NT4 algorithm claim.

Builds: `build/M0-T423/S2/root-propagation-build.log` and
`root-propagation-wow-build.log`, coherent MSVC x86 CCPU40 six-file composition.
Runtime evidence:

- Prefix `m0-t423-s2-root-propagation`: native-root-frontend,
  native-cmd-dos-repeat, dos-native-dos and dos-native-typeahead all pass actual
  text/exit checks. The last two now produce both expected MEM reports and
  retain the original DOS exit code 1.
- Prefix `m0-t423-s2-propagated-owner`: those same four cases pass again after
  adding RootFrontend assertions to every nested/repeated case. Every emitted
  frontend-owner trace must match the observer's outer launcher PID, not only
  the first trace. Input/exit evidence alone is not the identity check.
- Prefix `m0-t423-s2-propagation-dos17`: all 17 established DOS routes pass,
  including EDIT then MEM, native stream/EOF and original nested exit results.
- `m0-t423-s2-propagation-console-client.txt`: native Console client fixture
  exits 0 and emits its full transport/ownership/native-error/disconnect
  assertion marker. Its broker capability is explicitly mocked; Console I/O
  and transport remain real. Build: `propagation-console-client-build.log`.
- Real `Verify-BasesrvProduct.mjs`, test build
  `build/M0-T423/S2/rpc-propagation-checked`, passes after adding worker event
  rights, launcher rejection, root-generation match and delegated request /
  selected-worker / attach / notification-reset checks. Existing stream,
  original command exit (7), rundown and version rejection also pass.

Every temporary product run restored and hash-checked the S1 six-file baseline;
this is still uncommitted S2 work, not publication or closure. Remaining gates:
root pump/channel failure cancellation in DOS waits, root departure before
attachment, resource-retention/multiple-worker audit, remaining Console owner
calls, independent WOW frontiers and final six-file delivery/commit/push.

### Root failure waiting and removal of the obsolete local frontend entry

The DOS launcher now includes its root-process (descendant) or frontend-pump
thread (root) in the original completion wait. Native child waiting uses the
same failure handle. This does not replace the original task completion or
exit-code query. The new-worker wait avoids duplicate process handles when
the original completion object is the worker itself; frontend failure uses
the existing startup rollback rather than reporting task success.

`m0-t423-s2-root-failure-wait` passed the real active-frontend-loss case:
worker error 1067 occurred before observer teardown, the launcher-loss outcome
was bounded, the broker survived, and a subsequent MEM returned 0. The staging
helper restored and hash-checked all six S1 binaries. This proves active-route
loss only, not cancellation before route attachment.

A source-wide caller search found no remaining use of
`run16_console_channel_start`. Its local CommandWorker/AttachFrontend branch
has been deleted, leaving only the authenticated root request entry. That entry
consumes its supplied worker reference on every outcome and rejects a missing
request or invalid worker before creating resources. This is removal of an
obsolete project-owned branch, not a change to original BaseSrv task policy.
The incremental MSVC x86 formal build passed after deletion. Runtime regression
prefix `m0-t423-s2-root-only-dos17` passed all 17 actual text/exit cases,
including EDIT then MEM and the original exit-code results. The six-file S1
baseline was restored and hash-checked after the run. This remains WIP rather
than a production P: the other S2 closure gates above are still outstanding.
The subsequent `m0-t423-s2-root-only-nesting` run passed native-root-frontend,
native-cmd-dos-repeat, dos-native-dos and dos-native-typeahead, including all
RootFrontend trace-owner comparisons and actual output/exit checks. Its finally
block also restored and hash-checked the coherent six-file S1 package.

### Cancellation of an authenticated request before channel attachment

Source finding: RequestFrontend previously retained only a root generation on
the requesting connection. If that root/request disconnected before attachment,
WaitFrontend woke without a route and returned to its indefinite wait. An
active-route loss test could not cover this interval.

The existing frontend route now retains the original-command-selected worker
reference as soon as RequestFrontend succeeds. This is a pending transport
attachment, not another DOS record or scheduling decision. Successful attachment
replaces it with the actual pipe/ready handles; Take marks delivery. Before
delivery, root rundown cancels it and drops root/pipe/event references while
retaining only the selected worker reference until the existing worker-exit
cleanup. Requester rundown cancels an unattached request as well. Take/Wait
return ERROR_PROCESS_ABORTED with no output handles for a cancelled route.
After attachment, requester rundown still cannot revoke the root-owned route;
after delivery, root rundown keeps the existing active-channel death contract.
No guest or original srvvdm.c body changes are involved.

Focused x86 tests: build the formal `basesrv-service-reservation-test.exe`, then
run `--frontend-wait-root-loss` and `--frontend-wait-request-loss`. Both use real
original Check/Update records, distinct native processes and a concurrently
waiting production service call. Both passed with wait=0/error=1067 before the
test terminates the worker, and all returned handles/generation were zero.
Existing `--frontend-delegated`, `--frontend-rundown`, `--frontend-wait` and
`--frontend-wait-worker-loss` also passed. The rundown assertion now requires
explicit cancellation, not an indefinitely retryable NOT_READY result.

The first fixture link failed because a new printf call was not supplied by
its historical CRT composition. Using the fixture's existing fprintf entry
fixed that test-only issue; the subsequent x86 formal link and six focused
tests passed. Real RPC abrupt-process-death coverage and product regression
are separate gates; these native service results alone do not close S2.
Product follow-up: `m0-t423-s2-pending-cancel-dos17` passed all 17 text/exit
routes; `m0-t423-s2-pending-cancel-nesting` passed all four root-identity and
nested DOS/native cases. Each run restored and hash-checked the coherent
six-file S1 baseline. These normal-path runs do not substitute for a real RPC
process-death test during the pre-attachment interval.
The existing real RPC suite also passed with test build
`build/M0-T423/S2/rpc-pending-cancel`: original command delivery, delayed
frontend attachment, restricted root capability, independent-event rejection,
process-exit rundown, stream receipts and protocol/app-version rejection.
Its owned broker stopped normally. This suite's existing rundown case is not
the new pre-attachment fault injection, which remains a distinct test to add.

### Real RPC pre-attachment rundown verification

`tests/app/base_client_rpc_first_test.c` now has two production-transport cases
selected by `Verify-BasesrvProduct.mjs`. `--frontend-cancel-parent` submits an
original command, registers its real worker and root, waits until the worker
has connected, publishes RequestFrontend without attaching a pipe, and closes
the root RPC context. The worker's actual WaitFrontend must return 1067 with
all handles and generation zero, then exit 0 within five seconds.

`--frontend-cancel-killed-parent` uses an independent test monitor to terminate
that exact root process with exit 92 instead of calling Disconnect. The monitor
retains the test worker process handle, requires its successful cancellation
exit within five seconds, and writes a one-byte verdict below the test build
root. On failure it terminates only that fixture worker. The Node test requires
both root exit 92 and the successful monitor verdict, so a killed observer or
mere root exit cannot count as cancellation success. The child asserts the
actual RPC error/empty outputs before returning success. The result reader
waits through a short file-creation/write-sharing window, never treating an
absent or empty verdict as passing.

Both cases and the existing complete RPC suite passed in
`build/M0-T423/S2/rpc-cancel-abrupt`; logs include `frontend-cancel.log` and
`frontend-killed.log` under O:/winnt/logs. This proves automatic RPC rundown
after actual root death for an already registered worker awaiting attachment.
Worker-not-yet-registered cleanup, completed-channel retention/isolation and
remaining Console/WOW gates are still separate open audit items. This test-only
follow-up does not change the six-file product candidate or deployed S1 package.
The final result-reader revision passed the whole suite again using
`build/M0-T423/S2/rpc-cancel-abrupt-final`. An intervening attempt under
`rpc-cancel-abrupt-verified` correctly failed at broker startup with 1740 because
an O:/winnt broker owned the endpoint; it was not counted as a test pass. A fresh
process check showed no product run16/ntvdm session, so that exact idle broker
was ended under the owner's standing authorization before the final run.

### Nested chain liveness after worker death

Owner requirement: verify every root/inner launcher, not merely the worker or
outer observer. `Verify-BrokerFinalLifecycle.ps1 -NestedWorkerLoss` launches
root run16 -> CMD /c -> run16 -> CMD /c -> run16 -> guest COMMAND on a private
desktop. It requires the actual DOS prompt, records the complete PID/parent/
command-line chain, retains all five launcher/shell process handles before
ending the one selected worker, and requires each to exit 1067 within five
seconds. The broker must survive and a fresh MEM must print its real memory
report with exit 0. Only ancestry-proven test processes may be cleaned up.

`m0-t423-s2-nested-worker-loss` passed: run16 28456 / CMD 21568 / run16 57796 /
CMD 47420 / run16 13596 all returned 1067; the later MEM returned 0 with actual
output. The chain and per-layer results are recorded under O:/winnt/logs with
that prefix. The six-file S1 package was restored and hash-checked.

`-NestedInteractive` changes only the outer CMD to /k. The three inner
launcher/shell processes must exit 1067, but root run16 and its interactive
CMD must remain alive until the observer releases its input gate. It then
requires a standalone NESTED-RECOVERED output line and exit /b 23 propagated
through the root. The first run (`m0-t423-s2-nested-interactive-loss`) proved
those process outcomes but failed a test assertion: captured lines have [row]
prefixes. Its actual [5] NESTED-RECOVERED output was present. The assertion was
corrected to match a whole row, not merely the echoed command; that first run
remains a failed harness run rather than a passing acceptance result.
The corrected `m0-t423-s2-nested-interactive-verified` run passed: inner run16
58428 / CMD 27340 / run16 42824 returned 1067; root run16 48992 and interactive
CMD 24816 survived the worker loss, executed the recovery echo and propagated
the subsequent explicit exit code 23. Fresh MEM then passed with real output.
The helper restored and hash-checked all six baseline binaries.

These rows cover worker loss in a three-launcher native/DOS nesting chain;
inner-launcher loss, root/broker loss across that same full chain and unrelated
worker isolation remain explicit separate fault-matrix rows, not inferred passes.

### Owner clarification: task cancellation is not worker termination

On 2026-09-26 the owner clarified that a surviving worker may correctly become
empty; the orphaned guest program must end. The nested-launcher-loss test's
requirement that ntvdm itself exit 1067 was therefore too strong and is not the
acceptance contract. A native process snapshot alone cannot establish an empty
worker: require original task-record retirement, guest completion/cancellation,
no stuck error UI and ability to accept a subsequent admitted command.

The `m0-t423-s2-nested-broker-loss` run observed all three launchers, two CMDs
and worker exit 1722; the root-loss run observed root -1 and the remaining
chain 1067. These observations do not prove the clarified empty-worker contract.
The inner-loss runs left the worker in a real "The pipe is being closed"
error dialog, as captured by `m0-t423-s2-inner-loss-evidence-worker-windows.txt`;
that is not an empty worker. A trial normalization of pipe EOF/closed errors
to process-aborted made `m0-t423-s2-inner-loss-fixed` pass the overly strong
process-exit test, but was withdrawn from source immediately on clarification.
It is not an accepted fix. All temporary runs restored the six-file S1 package;
the mutable build cache must rebuild console_client.c before further tests.
Next recovery must distinguish the originating guest task and its original
cleanup/re-entry from the worker process lifetime; do not forcibly end unrelated
parent/other tasks or use worker termination to manufacture an empty state.

### Original task-exit versus asynchronous host cancellation audit

Read the selected `dos/command/cmdmisc.c::cmdGetNextCmd`,
`dos/command/cmdexec.c::cmdReturnExitCode`,
`dos/dem/demsrch.c::demTerminatePDB`,
`softpc.new/host/src/nt_msscs.c::HostTerminatePDB`,
`softpc.new/host/src/nt_event.c::CntrlHandler/nt_block_event_thread`,
`dos/v86/doskrnl/dos/abort.asm::DOS_ABORT`, and
`dpmi/dxstrt.asm::ChildTerminationHandler`. The external pinned OpenNT
nt_event.c control handler was also checked for the same ordering.

Findings: normal guest termination drives the cleanup before COMMAND reports
its result and calls GetNextVDMCommand. demTerminatePDB is expressly a guest
PDB-termination notification; HostTerminatePDB releases floppy/disk ownership,
not guest CPU execution. DPMI TerminateApp is called from the guest child
termination handler after owner-memory cleanup. These names are not proof of
an asynchronous host-side DOS task-kill API. SYSTEM_ROOT_CONSOLE_EVENT records
root departure and waits for voluntary app exit / the original blocked-state
conditions; it does not synthesize a per-task guest unwind. BaseSrvVDMTerminated
handles failed VDM startup/record cleanup, not live guest program termination.

Consequently the next implementation must not invoke a cleanup notification on
an arbitrary host thread, delete only the DOS record, or inject a raw guest
INT21 termination at an arbitrary CPU state. Those actions have not established
the PSP, guest stack, DOS reentrancy, parent and DPMI cleanup prerequisites.
This bounded audit identifies the normal reusable exit path but has not yet
established the safe asynchronous entry into it. The orphan-dialog regression
remains open; the earlier pipe-to-process-abort shortcut stays withdrawn.

### Execution-subtree policy and current launcher audit (2026-09-26)

Question: does the current implementation meet the owner's clarified rule that
failure of an inner launcher or its target cancels that execution subtree while
preserving the interactive parent? Inputs are current main.c, frontend_scope.c
and the original-exit audit above; this is source inspection, not a runtime pass.

`run16_frontend_scope_failure_handle` watches either the root frontend thread
or root process. `run16_frontend_scope_wait` waits on that object and the native
child. It has no distinct execution-parent failure object. The two native
CreateProcessW paths in main.c terminate only their direct child when their own
wait returns an error; abrupt launcher death cannot run these cleanup blocks.
Therefore root capability propagation alone does not implement the new subtree
rule. This finding requires execution ancestry separate from frontend identity.

The worker startup job is not a solution to this gap: it is kill-on-close only
between atomic process creation and broker PrepareWorker, then explicitly
disarmed. Keeping it armed for the whole launcher lifetime would couple shared
worker survival to one launcher, violating the preserved-parent requirement.
No such change was made. A native process containment mechanism must also avoid
capturing shared broker/worker processes; DOS task cancellation still needs the
original guest cleanup and parent re-entry prerequisites established above.

The proposal now requires root/middle/leaf launcher and target fault cases,
interactive ancestor recovery, unrelated-task isolation, completion races and
actual input/output checks. Earlier /c whole-chain exits are observations only,
not proof of this gate. Documentation governance and diff whitespace checks pass;
no production change, build, deployment or runtime acceptance is claimed here.

### Ctrl+C and whole-VDM shutdown are not subtree cancellation

Source procedure: inspect `dos/v86/doskrnl/dos/msctrlc.asm::CNTCHAND`
(lines 376--522), compare SHA-256 with the pinned external OpenNT file, then
check the selected formal build.ninja and its host termination provider.
Both local and original msctrlc.asm hash to
`8D1B22B12A10EADE8B6CF83276BE13AC9E13971F3F83EEAC3961AD8B3AA9120C`.
This proves the observed handler policy is original, not a project diff.

The original handler restores guest stack/context, calls INT 23h, and permits
IRET or carry-clear return to retry the interrupted operation. Only its abort
branch dispatches DOS EXIT. Consequently even correctly delivered Ctrl+C is a
cooperative request, not proof that an arbitrary guest task has ended; timing
out and silently counting it as cancelled would be invalid. The original host
nt_event.c also handles CTRL_C_EVENT/CTRL_BREAK_EVENT without initiating a
task unwind. This is a design boundary, not an attributed original guest bug.

The formal graph selects host/src/nt_reset.c (build.ninja line 525), not the
historical nt_term.c alternative. Its TerminateVDM invokes terminate(), whose
documented cleanup reaches host_terminate; that function calls ExitVDM and
ExitProcess(VdmExitCode). Therefore this selected shutdown path cannot preserve
a parent guest task in that worker. No provider was changed during this audit.

Implication: the implementation cannot claim the full subtree contract using
either synthetic Ctrl+C or whole-VDM termination. Remaining research must prove
a task-scoped safe original unwind entry and its prerequisites, including DOS
reentrancy and protected-mode children; absent such proof the row remains open,
not a passing fallback or permission to patch guest code/data.

### Original re-entry shares a worker; ExitVDM is not per-DOS-task cancellation

Inspection of srvvdm.c::BaseSrvCheckDOS (1118--1210) confirms that a busy
same-Console VDM receives another DOSRECORD, with its own parent wait but the
same ConsoleRecord/hVDM. main.c's VDM_PRESENT_AND_READY branch uses that wait
instead of creating a new worker. cmdexec.c::cmdExec32 waits for re-entry through
GetNextVDMCommand while cmdCreateProcess waits for the native child on another
thread. Nested DOS after a native child can therefore share the suspended DOS
parent's worker; separate launcher completion events are not task isolation.

The original BaseClient vdm.c::ExitVDM contract explicitly ignores iWowTask for
DOS. srvvdm.c::BaseSrvExitDOSTask (1735 onward) walks every DOSRECORD, signals
all parent waits, closes hVDM and frees the ConsoleRecord. It neither requests
one running guest task to unwind nor retains the parent's DOS record. WOW's
task-specific record removal cannot be substituted for this DOS path.

Disposition: preserve these original owners, do not call ExitVDM to implement
subtree cancellation, and do not allocate an independent worker for every
nested DOS command without an approved change to re-entry/state sharing.
Guaranteed forcible cancellation of an arbitrary shared-worker guest child
while preserving its parent is not supplied by the audited original paths.
This is an architectural constraint requiring explicit resolution, not a missing
RPC wrapper or an original guest defect eligible for the standing TODO waiver.

### Unclaimed frontend shutdown resource regression and repair

While the owner's shared-worker fault-boundary decision remains outstanding,
the independent pre-Connect resource path was tested. Added native case
`basesrv-service-reservation-test.exe --frontend-unclaimed-stop` creates real
original Check/Update records and a prepared suspended test process, registers
a frontend request, then disconnects before worker Connect. No worker-exit watch
has been installed. Original startup rundown ends that unclaimed process; the
cancelled frontend route retains its process handle for cancellation delivery.

Before the repair, the new test failed: process handle count immediately before
and after OpenNtBaseServiceStop was 113/113. Inspection confirmed Stop freed the
service without draining frontend_routes. After the repair, Stop reuses the
existing route destructor after the no-connections/no-watches stop preconditions;
the test passes with 113/112. It closes a retained handle, never calls process
termination and does not alter DOS records or the disputed guest cancellation
policy. This fixes final service shutdown only; it does not claim live-service
reclamation of every cancellation tombstone or guest-subtree cancellation.

Build: existing MSVC x86 /MT formal cache,
`ninja -C build/M0-T423/S1/restart-formal-x86 basesrv-service-reservation-test.exe basesrv.exe`.
Native cases `--frontend-unclaimed-stop`, `--frontend-wait-root-loss`,
`--frontend-wait-request-loss`, `--frontend-delegated`, and `--frontend-root`
all returned zero. Pending cancellation returned 1067 with no resource outputs;
delegated and root identity/rundown assertions also passed. No desktop or
O:/winnt package was used or replaced. Full production P gates remain pending.

SHA-256 of tested inputs/artifact:
- base_service.c: `51D1A236F9A3EE913F70E2EB9CD7EDAC451E0F0B9E735A04B259048EBC6AFC51`.
- base_service_reservation_test.c: `C2A32CBEA36E393F25F749F849480786B40309ABED5F4D99622A99BF4E98D28D`.
- basesrv-service-reservation-test.exe: `4F0430FE74F21D7C25D9AFAB3ADDE1D8D688407F3D8C63523395982DFA6812BB`.

Follow-up verification on the same repair: the default native fixture also
passes original Check/Update/Get/ExitVDM lifecycle. Real RPC suite
`tools/audit/Verify-BasesrvProduct.mjs` returned zero with
OPENNT_BROKER_PRODUCT_BUILD=`build/M0-T423/S1/restart-formal-x86`,
OPENNT_BROKER_TEST_BUILD=`build/M0-T423/S2/rpc-stop-drain-20260926`, and
OPENNT_BROKER_TEST_LOGS=`O:/winnt/logs`. It verified original command delivery,
file/pipe receipts, delayed attachment, explicit and abrupt root cancellation,
root capability authorization, reconnect/rundown and protocol/app-version
rejection. The owned server stopped; a subsequent process inspection found no
basesrv, reservation-fixture or RPC-fixture process. Logs are the suite's named
runtime logs and its unique build.log/result file; this is not a guest runtime
or subtree-isolation pass. O:/winnt binaries remain unchanged.
Tested basesrv.exe SHA-256:
`7017AF045138A45969AF40989424FE069B6AF4B867A251E412DBC301D86F33C2`;
suite script SHA-256:
`9419DA0476C067CB1AB5101CEC5EF33542F4B8D40801AEA46463ED7961CEB987`.

### Owner resolution of the shared-worker fault boundary

Owner accepts the proposed boundary: original OpenNT nesting semantics govern,
with corresponding handling added for run16. The proposal and active brief now
explicitly allow unrecoverable guest faults to end the affected worker and its
tasks; they no longer require preserving a DOS ancestor inside that same worker.
Recoverable parents outside it and unrelated workers remain protected. Normal
completion/empty residence is not itself a fault. No guest task-kill injection,
per-command worker isolation or new scheduler is admitted by this resolution.

The previously withdrawn closed-pipe containment experiment remains historical,
not automatically accepted production code. Follow-up must implement the agreed
failure distinction and rerun real fault/interactive-parent and normal-exit
regressions before any delivery. In particular active I/O losing its root must
not hang in DisplayErrorTerm; idle residence and next-command rebinding must not
be killed just because the prior launcher completed. This resolves the policy
question, not the remaining implementation and verification gates.

### Accepted active-channel containment and lifetime-pair clarification

Owner's latest clarification supersedes generic subtree cancellation: each
run16 and its corresponding target form a lifetime pair; cross-level behavior
must follow original OpenNT nesting, without an additional recursive kill policy.
The proposal and active brief reflect this. Native launcher abrupt-death cleanup
for its target still needs explicit verification; the cases below do not prove
every pair or every direction of death propagation.

Production console_client.c now maps broken/closed/not-connected pipe errors
from an active exchange to the existing process-aborted containment path. It
does not terminate an idle worker from the frontend watcher. Native fixture now
separates idle root loss (normal zero exit and cleanup) from `--broken-pipe`
(actual client process exit 1067 with a still-live presenter identity). The latter
uses its own process as the fixture identity so forced exit leaves no helper.
Normal I/O, geometry, native error and input checks still execute in both cases.

MSVC x86 incremental targets console-client-test.exe, ntvdm.exe and VDMREDIR.dll
passed. On an unswitched private desktop, reports
`m0-t423-s2-accepted-idle-native.txt` and
`m0-t423-s2-accepted-pipe-native.txt` below O:/winnt/logs record exited/0 and
exited/0x42b respectively. The original native-error-dialog path is not used
for this terminal transport failure.

Real temporary-package tests using build/M0-T423/S2/test-frontend-loss.ps1:
- `-Prefix m0-t423-s2-accepted-inner-loss -NestedWorkerLoss -LauncherLoss`:
  killed innermost launcher; /c launcher/CMD ancestors naturally returned -1,
  active worker exited 1067, subsequent MEM returned 0 with text assertions.
- `-Prefix m0-t423-s2-accepted-parent-recovery -NestedWorkerLoss -NestedInteractive`:
  worker death returned 1067 through the inner chain; outer interactive CMD and
  root stayed alive, executed the recovery echo and returned requested exit 23;
  subsequent MEM passed. This is actual parent interaction, not only liveness.

Both scripts hash-verified restoration of the coherent six-file S1 package.
Full DOS17, broader pair faults, independent WOW and final publication remain
pending; these checks are not S2 closure or a production P.

### Native lifetime-pair implementation admission

Original OpenNT native-child creation/wait returns the child's exit status but
does not supply the owner's standalone guarantee that a killed CLI launcher
cannot orphan its direct native target. Source/facade reuse cannot add that
guarantee to an already dead launcher. The admitted boundary is therefore a
run16-local OS job, not an imported-source change or new broker process registry.
Create the target atomically in a non-inherited kill-on-close job held only by
run16; SILENT_BREAKAWAY_OK excludes the target's children. Existing native wait
and exit-code paths stay unchanged. Failure to establish the job is a launch
error, never fallback to an unowned target. Broker and worker creation remain
on their existing independent paths. No recursive termination policy is added.

Microsoft's [Job Objects documentation](https://learn.microsoft.com/en-us/windows/win32/procthread/job-objects)
defines silent breakaway as excluding child processes and kill-on-close as
ending associated processes. Focused acceptance must use real run16 to prove
both death directions, exit-code preservation and an unassociated descendant
surviving direct-target cleanup. This new CLI-only binding is retained while
the owner's lifetime-pair policy applies; it is not historical OpenNT behavior.

Implementation and focused results: main.c::create_native_target wraps both
native CreateProcess callsites (ordinary image and COMSPEC command). It creates
the non-inherited job and passes PROC_THREAD_ATTRIBUTE_JOB_LIST atomically at
process creation, retaining the job through existing wait/return cleanup. No
mirror or broker protocol changes are required for this native pair.

Added `tests/app/native_lifetime_pair_test.c`. Compile once with MSVC x86 /MT
to build/M0-T423/S2/native-lifetime-pair.obj; link the console test executable
and the same object as GUI subsystem target with /entry:mainCRTStartup.
Run `native-lifetime-pair-test.exe <absolute-run16> <absolute-target>`.
The fixture uses private named mapping/events, retains exact test process
handles and cleans its descendants itself; the GUI target creates no windows.

Before/after evidence: against unchanged O:/winnt/run16.exe, mode 0 failed the
target-exit assertion after killing its launcher (test exit 1). Against formal
cache run16.exe the suite returned zero for GUI and CUI targets separately:
- mode 0: killed launcher; direct target ended; its descendant remained alive;
- mode 1: target killed with 73; launcher returned 73; descendant remained alive;
- mode 2: target completed with 42; launcher returned 42; descendant remained alive.

This tests paired lifetime without recursive descendant termination in both
native image classes. The CUI suite's own build broker PID 54280 was identified
by exact executable path and stopped after tests; no fixture processes remained.
No O:/winnt binaries were replaced. COMSPEC-specific, DOS17, nested DOS/native
and WOW regression/publication gates still remain before production delivery.

Follow-up normal-path regression: temporary coherent six-file staging via
build/M0-T423/S2/test-frontend-loss.ps1 passed all 17 text-gated DOS cases with
`-Prefix m0-t423-s2-native-pair-dos17 -DosRegression`. Expected/observed exits:
empty 0, native-zero 0, missing 0, native-seven 0, native-streams 0, native-eof 0,
mem 1, nested-empty 1, nested-mem 1, mem-repeat 1, direct-mem 0, command-c 0,
command-c-seven 0, guest-seven 7, command-c-mem 0, direct-seven 7, edit 1.
The script includes captured guest output, not exit-only acceptance.

`-Prefix m0-t423-s2-native-pair-nesting -Cases native-root-frontend,native-cmd-dos-repeat,dos-native-dos,dos-native-typeahead`
passed the four real ownership/re-entry cases with exits 0/0/1/1. Their exact
reports and Console captures are under O:/winnt/logs with these prefixes.
Both staging runs restored all six S1 hashes; final publication remains pending.

The formal graph generator now supplies native-lifetime-pair-test.exe and
native-lifetime-pair-target.exe from the tracked fixture source. Regeneration
and building these two targets succeeded in the reused x86 formal cache; all
three GUI pair cases also passed against the graph-built test/target binaries.
This supersedes the one-off compile recipe for future reproduction. Independent
WOW frontiers and the complete S2 ownership/failure checklist remain open.

### Owner-scoped WOW waiver and headless startup comparison

Owner states that historical WINMINE remained usable after NETWORK.DRV's prompt
and exempts all three Win16 applications from foreground/manual runtime testing
for this T; headless verification is sufficient. This supersedes the foreground
gameplay gate for T423 only, not DOS/native/lifecycle or publication requirements.
The proposal and active brief record this exception explicitly.

Added tests/observation/observe-wow-frontiers.ps1: launches each original guest
on the observer's unswitched private desktop, reads test-owned worker windows,
records bounded outcomes, cleans package processes and verifies unchanged
SYSTEM.INI. It neither sends input nor bypasses the existing modal. This is a
startup observation tool, not an application-functionality test.

Initial prefix m0-t423-s2-pair-wow-baseline lacked desktop access; it cannot
support application conclusions. The named-desktop retry detected stale helper
behavior (successful named enumeration followed by failed thread enumeration).
Rebuilt worker-window-snapshot.exe from current tracked source and made the
script reject desktop-enumeration errors rather than count them as observations.

Final prefixes under O:/winnt/logs:
- m0-t423-s2-pair-wow-baseline-final: original six-file S1 package;
- m0-t423-s2-pair-wow-candidate-final: current coherent six-file candidate.

WINMINE, SOL and WRITE each reached File Error / Cannot find NETWORK.DRV with
a live worker in both packages. Each launcher was still waiting at the 20-second
observation bound (`result=timeout`, observer marker 0x53504354); this is not a
guest exit code or proof of successful use after dismissing the modal. No new
early-process-exit or different visible fault was observed at this depth. The
candidate run restored and hash-checked all six baseline files. Configuration
and original guest binaries were unchanged. Deeper foreground functionality is
waived by the owner, not claimed to have passed this test.

### Multi-worker isolation and frontend channel retirement

The real lifecycle test now requires the unrelated worker to execute
`echo ISOLATION-ALIVE` after fault injection and captures that output before
normal exit. Merely retaining its PID is no longer sufficient. Existing logs
`m0-t423-s2-pair-worker-isolation-results.txt` and
`m0-t423-s2-pair-frontend-isolation-results.txt` under O:/winnt/logs prove the
post-fault command, original normal exit, surviving broker and subsequent MEM
success. Active frontend death terminated only its affected worker with 1067.
Both temporary deployments restored the coherent six-file S1 baseline.
LauncherLoss uses the same accepted active-frontend containment assertion;
the old unconditional claimed-worker-survival assertion is superseded, not
evidence of this policy. Its alias itself has not been rerun in this revision.

Resource review found that frontend_scope retained every completed channel
until the root launcher exited. This is local ownership debt in the newly
admitted frontend, not an OpenNT task-policy defect. The pump now removes only
entries whose channel thread is signaled, using the existing stop/join cleanup.
Only the pump accesses the list until scope_end joins it. Idle completed entries
can remain until the next request/wake or final scope teardown; repeated requests
no longer accumulate all historical channels. Active entries remain untouched.
There is no polling timer, new manager, worker termination or mirror change.

Tracked `tests/app/frontend_scope_lifetime_test.c` links the actual production
frontend_scope object with explicitly test-only broker/channel substitutes and
real threads/events. One channel stays live while 33 short channels complete;
each following admission must retire previous completions, and final teardown
must join/free all 34. The graph target is frontend-scope-lifetime-test.exe.
Before the fix, the retired-count assertion failed (exit 1); after the fix,
20 successive x86 /MT runs passed and run16.exe linked. This is resource-unit
evidence, not authenticated RPC or guest acceptance. The fixture retains its
own duplicate thread/event references while observing exit, so the production
pump is free to close its originals. Final fixture rebuild and real DOS/nesting
regression are recorded separately below when completed.

Final fixture rebuild passed 20 additional consecutive runs after the observer-
handle race was removed. Tested source/artifact SHA-256 identities:

| Input | SHA-256 |
| --- | --- |
| src/run16-exe/frontend_scope.c | 4E83F95D3F7D1DC50159E6ADFABEAC5B2027BE557E6E54AF9131AB2E146C114C |
| tests/app/frontend_scope_lifetime_test.c | 9A7FA47A49C6B75E14CDA91E54A42FDBB6BFFB38E4A14C6EF363BFE66DC9B15C |
| formal cache run16.exe | 2B8EE3B5A4FC828FFA779BC0926334E5FA498CC4B0D4EC917BB5FD737AC7A03B |
| formal cache frontend-scope-lifetime-test.exe | D6408281552ABB43B9E4EBDBB1AF7976522E9889FE3094AE3AFBFA9511DB2778 |

Real candidate validation used the existing temporary-staging helper:
`-Prefix m0-t423-s2-channel-retirement-nesting -Cases native-root-frontend,native-cmd-dos-repeat,dos-native-dos,dos-native-typeahead`
passed 0/0/1/1 with captured output and ownership/typeahead assertions.
`-Prefix m0-t423-s2-channel-retirement-dos17 -DosRegression` passed all 17
original text-gated cases, including EDIT and subsequent MEM. Reports remain
under O:/winnt/logs with those prefixes. Both completed with coherent six-file
S1 restoration. These results prove this repair's regression slice, not full
S2 closure. The latest production change still requires final affected-gate
reconciliation and publication before a P; no commit/push is claimed here.

### Console code-page query ownership

Selected original callers are dos/command/cmdkeyb.c::GetConsoleCP and
softpc.new/host/src/{nt_event,nt_fulsc}.c::GetConsoleOutputCP. Their keyboard
selection and byte conversion remain original worker logic, but their native
code-page source must be the run16-owned Console. Shared visible Console use
currently conceals this ownership gap. The standalone graftabl tool's code-page
setter is not part of this worker graph and is not moved into it.

Reuse the public native query unchanged in the frontend. Original Console
Server/CSR cannot compose as the standalone frontend, as already recorded for
this owner-approved I/O split; no source algorithm or external-code patch is
needed. Add one copied query operation with an input/output selector and retain
UINT/zero/error behavior in the existing worker facade. No-channel WOW/fixture
callers retain the native path. Channel failure must never fall back locally.
Verification must assert native values and actual request sequence changes,
both selector directions and no-channel behavior; generic DOS success alone
does not prove this boundary. Title and private window operations remain separate
uncompleted audit rows, not covered by this change.

Implementation: CONSOLE_IO_CODE_PAGE carries only the existing scalar input
selector; frontend calls GetConsoleCP/GetConsoleOutputCP and returns UINT or
native zero/error. The direct channel version is now 3 so mismatched peers
reject it rather than silently sharing the previous operation set. Original
mirror files are unchanged. cmdkeyb reaches the declaration via winconp.h;
SoftPC callers via conapi.h. MSVC dumpbin of selected cmdkeyb.obj and nt_event.obj
confirms _MvdmGetConsoleCP@0 and _MvdmGetConsoleOutputCP@0 respectively;
nt_fulsc.obj has no selected code-page reference in this build row.

Incremental x86 builds passed run16, ntvdm, VDMREDIR and both native Console
fixtures. WOW32 relink also passed against the current ntvdm import library.
The final real-pipe fixture sets its private Console input/output pages to
437/850, checks the distinct results plus each remote sequence increment,
temporarily removes the session binding to prove unchanged native behavior,
and restores both pages. `m0-t423-s2-codepage-distinct-native.txt` records exit 0.
The frontend fixture rejects input selector 2 and unexpected payload, then
accepts both valid selectors; `m0-t423-s2-codepage-negative-frontend.txt` records
exit 0. These are unswitched private-desktop native tests, not guest acceptance.

| Tested source | SHA-256 |
| --- | --- |
| product-abi/console_io.h | 9A7F18740FFE63017F08BEE5E78930B5D257F6BFD3796D046959AAEF6E4A8161 |
| ntvdm-exe/win32/console_client.c | 078145C28CE543C890203F132006BA912D26A16E8DAFE8B6B3D3F1AD3137BDC7 |
| run16-exe/console_frontend.c | 163AFBE6CDB92825BFF37EA2CBE62FA61807C2C6211ED61266FF70BD8DF7E161 |

Real guest follow-up `m0-t423-s2-codepage-dos17` passed all 17 captured-text/exit
cases through Verify-CommandExitStatus.ps1. The coherent temporary package was
restored afterward. This run used the previously built WOW32 (DOS routes do
not exercise WOW); the separate subsequent `m0-t423-s2-codepage-wow` observation
used the relinked latest WOW32 together with the current five other files.
All three original guests stayed alive at the existing NETWORK.DRV modal,
matching the recorded ordinary-profile baseline. Each observer reached its
20-second bound, not an application-success exit. No foreground acceptance is
claimed under the T423 waiver. That temporary run also restores all six S1 files.

Current built run16 SHA-256 is
0DD63DB6B5593D7E661C05D5E1F2E8814BF4606BFDB25A38C847DAA17B820A3D;
ntvdm is D2313B224D4C9896B08DE37EF03316DBD226CF57B28CC71D0D3E12012C95FB22;
relinked WOW32 is FDB1715EF376CC91519CAD71333D05D06279A7D1C7A0F13A25511C557F76B1A2.
These are build/test identities, not a published P manifest. Title/private
window query ownership and remaining per-layer failure acceptance are still
open; no S2 closure is inferred from the code-page repair.

### Private Console window-query ownership

Original nt_event.c queries VDM_IS_ICONIC; nt_mouse.c uses VDM_CLIENT_RECT and
VDM_CLIENT_TO_SCREEN before its own source-defined coordinate conversion.
The existing facade called GetConsoleWindow in the worker. The approved frontend
split requires those native queries at run16 instead, not a new guest mouse
algorithm. Reuse IsIconic/GetClientRect/ClientToScreen unchanged behind copied
scalars in the already admitted channel; Console Server/CSR remains unavailable
and excluded. No kernel/USER-server import or mirror edit is required.

The same-shaped VDMConsoleOperation facade maps only its three existing supported
selectors. No-channel callers retain the previous native path; unsupported
selectors and null data retain explicit failure. Bound-channel failure must not
fall back to the worker window. Window handles never cross IPC. Client/screen
coordinates retain signed LONG range rather than being narrowed to Console cell
SHORT coordinates. The direct protocol advances to 4. Original pointer clipping,
warping and other mouse policy are not changed or claimed migrated by this row.
Native transport tests will compare all three queries, use coordinates outside
SHORT range, assert sequence advancement and unsupported/no-channel behavior.

The x86 incremental build passed run16, ntvdm, VDMREDIR and both Console tests;
WOW32 relink also passed. The first transport test used a rectangle sampled at
Console creation and failed its later equality check. The revised test brackets
each remote rectangle query with native samples and compares only when those
samples agree, with at most ten attempts and a hard mismatch failure. It does
not tolerate a stable wrong remote value. Five final runs under
`m0-t423-s2-window-query-repeat-{1..5}.txt` passed all query, signed-LONG,
sequence, unsupported-selector and no-channel assertions. The earlier failed
run remains non-pass evidence, not silently converted into success.

The complete no-channel fixture was rebuilt with the actual console_compat,
console_client and session sources. Added coverage compares the original-shaped
VDMConsoleOperation calls against native results, including native failures,
and retains unsupported/null-data errors. Initial tests incorrectly required
GetClientRect on its newly allocated private Console to succeed; both attempts
returned test 42 and are not passes. The final native-equivalence test records
exit 0 in `m0-t423-s2-window-query-no-channel-equivalence.txt`. Positive remote
query evidence comes from the separate real-channel tests above, not from
assuming the fallback fixture always has a usable native window.

Remaining adjacent ownership is explicit: original nt_mouse still calls
ClipCursor/GetClipCursor/GetCursorPos/SetCursorPos for its pointer-warp path;
the source-owned calculation is distinct from these host UI calls and must not
be rewritten. Console title callers in config/nt_fulsc/cmdpif/cmdmisc also remain
outside this query migration. These are pending S2 audit/wiring, not silently
certified by generic MEM/EDIT success.

The refreshed active-pipe-loss fixture also exits 1067 after the new queries
(`m0-t423-s2-window-query-pipe-loss.txt`), retaining no-local-fallback containment.
DOS17 subsequently passes all 17 text/exit cases under
`m0-t423-s2-window-query-dos17`, with coherent S1 restoration before the separate
WOW observation. No new guest or mirror edits were made by this repair.

Tested source hashes:
- console_compat.c: E0743DD0A9ABFA3C37A2D8BB8A74AD2F09A3006B7DB821CFF1D921F4942CB95F.
- console_client.c: 29BF604E617BC85516C2A843EA6A1ACCA1AC526DFDA58410120E5C27969DE034.
- console_frontend.c: 027BA538E8E737A274CAA405FF54FD10379FAC68D2F4FB336E4A8D13DB864C43.
- console_io.h: D82CF410181A0DF18F029E4E4BFDD20FB059492424190C0DB06337640C0593C8.

The latest six-file `m0-t423-s2-window-query-wow` headless observation completed:
WINMINE, SOL and WRITE each remained alive at the unchanged NETWORK.DRV modal
until the observation bound, matching baseline depth under the owner waiver.
No foreground functionality is claimed. The staging helper completed exit 0
and hash-verified restoration of all six S1 binaries. Diff and documentation
governance checks passed; S2 remains uncommitted and open for its other gates.

### Title caller and native-contract audit

Selected title callers are config.c (MAX_PATH), nt_fulsc.c (MAX_TITLE_LEN=256),
cmdmisc.c (MAX_PATH minus prefix) and cmdpif.c (bounded PIF WinTitle). The first
two already include conapi.h; the latter two must be explicitly covered by the
build/declaration binding, not assumed reached because the SoftPC caller is.
The original SoftPC debugger vdmdebug.c also contains title calls but is absent
from the current formal graph. It is not evidence of an unbound selected caller.

Microsoft's [GetConsoleTitle contract](https://learn.microsoft.com/en-us/windows/console/getconsoletitle)
describes bounded output and a native return/error; its
[SetConsoleTitle contract](https://learn.microsoft.com/en-us/windows/console/setconsoletitle)
documents the size bound and title lifetime. The adapter must delegate actual
host behavior rather than synthesize title formatting, truncation or cleanup.
Before binding this family, a new tracked native fixture checks the current
host behavior independently: tests/app/console_title_native_test.c, formal
target console-title-native-test.exe, MSVC x86 /MT.

Run the existing console-startup-observer with the explicit observer-only
`--observation-timeout-ms 10000` argument, on its unswitched private desktop.
Without an explicit observer argument it injects its historical default DOS
arguments; the fixture correctly rejected that first attempt. The subsequent
misspelled --observe-timeout-ms also failed argument validation. Neither is an
API result. The verified report is m0-t423-s2-title-native-contract.txt and its
Console capture under O:/winnt/logs; result exited/0:

| Native observation | Result |
| --- | --- |
| Title TITLE-PARENT, output capacity 0 | Return 0, error 0, buffer untouched. |
| Same title, capacity 1 | Return 0, error 0, first byte NUL. |
| Same title, capacity 4 | Return 0, error 0, first byte NUL; last byte retains sentinel. |
| Same title, capacity 32 | Return 12, error 0. |
| Real child sets TITLE-CHILD, then exits | Parent still reads TITLE-CHILD, length 11. |
| Set empty title, query capacity 32 | Return 0, error 0, first byte remains sentinel. |

The fixture restores its saved title and touches no product binary or user
Console. These observations rule out adding a per-setting-process title restore
policy, and rule out blindly copying a zero-filled reply buffer on every zero
return. The eventual binding must preserve native return/error and actual bytes
written (including untouched output), while retaining original title-building
logic. This is audit/test progress only: title production routing remains open.

Final fixture initializes the saved title to empty before querying, so an
initially empty title is safe to restore. Rebuild and
m0-t423-s2-title-native-final.txt repeat all observations above with exit 0.

### Title production binding

The source-first route retains every original title builder and calls native
GetConsoleTitleA/SetConsoleTitleA in run16. Console Server/CSR cannot be composed
as the admitted standalone presenter; the existing copied channel is the minimal
binding, not a new title manager. Protocol 5 adds ANSI get/set. No per-process
title stack or exit restoration is introduced. Original calls in config and
nt_fulsc receive the declarations through conapi.h; a scoped forced include of
console_title.h binds cmdmisc/cmdpif without editing their mirror files.

This binding covers the selected bounded original callers (256/MAX_PATH and
PIF caption), not arbitrary third-party 64K title input. It rejects a request
larger than the existing 16KiB copied payload instead of truncating silently.
No new configurable capacity, allocation service or fragment state is added.
No-channel consumers retain native API behavior. Native zero return/error and
untouched output are preserved: the frontend only returns the bytes written,
including a sole NUL on the observed short-buffer path, not its whole scratch
buffer. Malformed unterminated/interior-NUL setter payloads are rejected before
the native call. Channel failures never fall back to a local worker title.

The real-pipe fixture compares native and forwarded return/error and every byte
of sentinel-filled buffers for titles TITLE-TRANSPORT, empty and AB with output
capacities 0/1/4/32. It asserts sequence increments, checks local no-channel
behavior and bounded precondition failures. The dispatcher fixture adds invalid
payload/size cases. Build/symbol and runtime results follow after execution.

Incremental x86 build passed run16, ntvdm, VDMREDIR, both Console fixtures and
WOW32 relink. dumpbin of the selected cmdmisc.obj, cmdpif.obj and config.obj
confirms references to MvdmGetConsoleTitleA/MvdmSetConsoleTitleA as applicable,
not the direct native imports. nt_fulsc.obj has no selected title reference in
the current profile. No mirror file was edited to obtain these bindings.

`m0-t423-s2-title-binding-client.txt` and `...-frontend.txt` record private-
desktop fixture exit 0. The final transport fixture adds a non-ASCII byte title
and compares it to the native ANSI API without adding encoding conversions;
`m0-t423-s2-title-binding-final-client.txt` also exits 0. Every forwarded get/set
increments the channel sequence, while no-channel calls do not. Buffer equality
includes untouched sentinel bytes, not just the displayed string. Full product
regressions are separate from these native boundary results.

Product refresh completed on the same title-binding build:

- `m0-t423-s2-title-binding-dos17-summary.json`: all 17 text/exit-gated
  routes pass, including EDIT followed by MEM in the same COMMAND session.
- `m0-t423-s2-title-binding-nesting-summary.json`: native-root-frontend and
  native-cmd-dos-repeat return 0; dos-native-dos and dos-native-typeahead
  return the expected original COMMAND result 1. All four output assertions
  pass. Procedure: Verify-CommandExitStatus.ps1 with these four Cases.
- `m0-t423-s2-title-binding-wow-{winmine,sol,write}` observations each find
  a live worker and the existing NETWORK.DRV modal on an unswitched private
  desktop. Observation ends by timeout, not successful application completion.
  This meets the bounded T423 headless observation requirement; it is not
  gameplay or full WOW acceptance.
- Temporary package runs restore all six baseline files and verify hashes.
  No foreground desktop interaction or guest/configuration change was needed.

### Remaining native pointer ownership audit

The selected formal x86 `obj/host/nt_mouse.obj` still imports native
ClipCursor, GetClipCursor, GetCursorPos and SetCursorPos. This is not merely
source under an excluded architecture conditional. Its source includes
conapi.h, allowing same-shaped declaration binding without editing the
original mouse algorithm. Runtime reachability is narrower than object presence:
the ordinary windowed text path with bPointerOff false uses Console mouse
records, whereas WarpSystemPointer uses native desktop position after the
original pointer-off transition. FullscreenWarpSystemPointer is another
original caller; its future Window acceptance belongs to the display stages.

Retain MovePointerToWindowCentre, CToS, HasConsoleClientRectChanged, pLast,
pMiddle, WarpBorderRect and the original movement calculations in nt_mouse.c.
Only the four native pointer operations should cross the copied frontend
boundary, retaining LONG coordinates, null ClipCursor release, native Boolean
results and error behavior. A record-delivery test does not prove clipping,
warping, focus-release or teardown. Tests must distinguish an inactive private
desktop's native unavailable result from a successful pointer mutation; do not
move the owner's desktop pointer to manufacture a pass.

ConsoleMenuControl currently explicitly returns ERROR_CALL_NOT_IMPLEMENTED.
ShowConsoleCursor retains the previously registered local compatibility count
and does not change the host mouse pointer or text insertion cursor. Those are
different contracts from the four available USER pointer APIs. Do not invent a
Console menu, turn the counter into SetConsoleCursorInfo, or claim pointer
visibility was restored by moving position calls. Their existing limitation
and frontend ownership disposition still need explicit final reconciliation.

### Pointer production binding and bounded tests

Protocol 6 now implements the four native pointer operations through the same
authenticated worker/frontend channel. The conapi declarations bind the
original calls without a new nt_mouse.c diff. Native USER remains the provider;
importing USER server or replacing its desktop state is neither necessary nor
admitted. Earlier-rung original mouse algorithms remain compiled unchanged;
the owner-approved frontend split requires only this copied process boundary.
No additional pointer cache, coordinate transform or guest event is introduced.

The initial x86 build of run16/ntvdm/VDMREDIR and native Console fixtures
succeeded. `m0-t423-s2-pointer-client.txt` and
`m0-t423-s2-pointer-frontend.txt` under O:/winnt/logs both exit 0 on private
desktops. Real-pipe query tests assert native Boolean/error behavior, unchanged
output on failure, sequence advancement, null-output rejection and no-channel
native routing. Invalid setter payload/flags are rejected before USER calls.
They deliberately do not move or clip the owner's desktop pointer.

`tests/app/console_pointer_dispatch_test.c` compiles the production dispatcher
with only the four USER APIs substituted by test functions. Its formal target
checks signed coordinates beyond SHORT, exact clip rectangle, null release and
four native failure results. This is mock boundary evidence, not real pointer
interaction. Its target compiled successfully in pointer-build.log; execution
and final WOW relink remain pending in the current command and must be checked
before reporting those gates passed. Product regression must be refreshed for
protocol 6; the earlier protocol-5 runs do not certify this new build.

The first mock executable could not start: the observer returned 67
(CreateProcess failure), and Start-Process independently reported that the
operation required elevation. No fixture assertions ran in those attempts.
The output shell had no child process and was terminated after the completed
build log was checked. A subsequent relink of that old output failed with
LNK1104; it was left untouched, not deleted. The final ordinary-user target is
`console-pointer-contract-test.exe`, explicitly embedding an asInvoker manifest.
It passes under the private-desktop observer: pointer-contract.txt records
exit 0 and the Console capture contains the production-dispatch assertion marker.
No elevation or security-setting change was used. The four substituted USER
calls cannot move/clip a real pointer. WOW32 relink also completed successfully;
the initial failed command is not counted as that result.

Formal nt_mouse.obj now references all four Mvdm pointer wrappers rather than
native imports. No mirror edit was required. Protocol-6 product regression
`m0-t423-s2-pointer-dos17-summary.json` passes all 17 established cases; the
test finally restored and hash-verified the six-file S1 baseline. Tested
run16 SHA-256 is 66F8FC76CBF5279AD5DE85F99067560C2E1CB8A549E7DDEED1823CB63DBEF1DF,
ntvdm is 576CB8F87304EE35F48647154549A51E5913E8D8434EED6ABE3D837F5F2B78DE,
WOW32 is 6F69E188EA5264CAF513685BC820E23827B51C5795CA604D6BA1C941DCAAA058.

Cleanup audit checked original OpenNT windows/core/ntuser/kernel/cursor.c
_ClipCursor/_GetClipCursor and focusact.c foreground-queue switching. The
original clip rectangle is global USER state, not a per-process resource
automatically restored by a channel destructor. The foreground switch clears
it explicitly; nt_mouse also explicitly releases on its original focus/menu
paths. Therefore do not invent an unconditional ClipCursor(NULL) at every
channel retirement: an unrelated active surface could own the current clip.
Retain native USER and original mouse focus policy; channel fault/focus
integration still requires specific evidence rather than a fabricated local
clip-owner cache or a claim that handle cleanup proves pointer cleanup.

`m0-t423-s2-pointer-wow-{winmine,sol,write}` observations completed under the
T423 headless waiver: each worker remains live at the existing NETWORK.DRV
modal until the bounded observation timeout. No new fault was observed; this
does not certify later gameplay. The test restored and hash-verified all six
S1 baseline files. Protocol-6 nested/fault and interactive pointer-specific
acceptance remain open; no S2 P, publication or closure is claimed by this
boundary increment.

### Inner launcher loss with a recoverable interactive parent

Protocol-6 normal nesting first passed all four cases in
`m0-t423-s2-pointer-nesting-summary.json`. The lifecycle test now also admits
`-NestedWorkerLoss -NestedInteractive -LauncherLoss`: root run16 starts
interactive CMD, then another run16/native CMD and an inner run16/COMMAND.
Only the inner DOS launcher is killed. The outer CMD must execute
NESTED-RECOVERED, exit 23, and a later independent MEM must produce real output.

The first real run, `m0-t423-s2-inner-launcher-interactive`, failed: native
intermediate pairs returned -1, but ntvdm PID 25676 survived. Earlier /c
chains could hide this because the root itself eventually ended. The pending
command's root frontend is not its submitting launcher; root liveness cannot
prove the launcher's task is still owned.

A first rundown-only guard and subsequent process-watch guard still failed
the real case. The diagnostic-only `m0-t423-s2-inner-launcher-record-trace.trace`
resolved why: the inner launcher's disconnect ran with process wait 258
(WAIT_TIMEOUT), an unsignalled original parent event, registered worker and
reservation 1. RPC teardown preceded process signalling. Those attempts are
failures, not passing cleanup evidence. Temporary logging was removed after
this observation; no diagnostic branch changes product behavior.

The final finite binding in base_service.c accepts either authenticated
pending-command disconnect or exact launcher process exit. Under the service
and original DOS list locks it matches original busy/to-take records through
that connection's parent receipt/event, then the original worker sequence.
Original UpdateDOSEntry's new-Console branch can return no parent event; that
case additionally requires the exact registered reservation and original
null-parent record. Normal completion, no task, unrelated workers and WOW are
excluded. Existing original worker-exit cleanup owns record removal and wakeup.
No guest unwind, second scheduler or process-tree traversal was added.
The process wait callback is joined outside the service lock before freeing
its connection. This binding implements the owner's explicit standalone
launcher/target pairing exception; original OpenNT does not provide that
launcher-process contract to copy verbatim.

Native service fixture `--launcher-pair-loss` passes on an existing-worker
command. Its initial failed wait used a borrowed receipt handle after
disconnect closed it; the final fixture retains its own synchronize-only
duplicate before abandonment and checks original completion. Default service
lifecycle and --management-terminate also pass. --frontend-rundown originally
failed at its obsolete manual TerminateProcess because the product had already
ended the worker; the updated test waits and asserts actual exit 1067 instead,
and passes. It retains the stale-channel rejection assertions.

Real `m0-t423-s2-inner-launcher-rundown-order` passes: inner launcher/native
chain returns -1, worker exits 1067, root run16 and outer CMD survive, the outer
CMD actually prints NESTED-RECOVERED and exits 23, and subsequent MEM exits 0
with its expected text. All six baseline binaries are restored and verified.
This closes that specific reproducer, not the entire layer/fault matrix;
refreshed DOS17 and other negative/race cases remain required for this build.

The final broker (SHA-256
42F60A1A883F7932D4BADA668BDF81763C72437597FF70B6D9944B848F87D879)
passes `m0-t423-s2-launcher-pair-dos17-summary.json`, all 17 output/exit cases.
The temporary deployment restored all six baseline hashes.

Added --launcher-pair-exit-watch to prove worker termination happens before
explicit Disconnect, independently of RPC rundown. Its first repeated runs
exposed a fixture bug: completion wait returned WAIT_FAILED/ERROR_INVALID_HANDLE.
This in-process service fixture had closed borrowed parent/worker-wait receipt
handles as though it were a remote RPC client; later receipt teardown could
close a newly reused numeric handle. The fixture now leaves borrowed references
to their service owner and closes only its explicitly duplicated observation
event. Production resource semantics were not changed for this test repair.
The corrected exit-watch case passes 12 consecutive native runs. Earlier failed
runs remain non-pass evidence, not a product-completion claim. Current service
source SHA-256 is E4F3361B0E685083EF4E73AFF0B844EF6B2DA06D534245C83BC321ADC7782720.
Remaining matrix cases, coherent final input reconciliation and delivery stay
open; no S2 commit/push or closure has occurred.

### Post-pair regression and fixture contract reconciliation

The unchanged final broker above passes real private-desktop runs
`m0-t423-s2-pair-isolation` (TwoWorkers + FrontendLoss) and
`m0-t423-s2-pair-normal-nesting-summary.json`. The first asserts worker exit
1067, actual ISOLATION-ALIVE output and normal exit from the unrelated worker,
and subsequent MEM. The latter passes all four native-root, repeated native
CMD/DOS, DOS/native/DOS and typeahead routes with original expected results.

`m0-t423-s2-pair-wow-{winmine,sol,write}` each observes a live worker at the
existing NETWORK.DRV modal until the bounded timeout. This satisfies only the
T423 headless observation comparison, not later gameplay or WOW completion.
Every temporary product run restores and hash-verifies the S1 six-file set.

The native service test was incrementally rebuilt under the existing x86 /MT
cache. --frontend-wait-request-loss initially failed its obsolete second
TerminateProcess assertion after the new pair binding had already ended the
worker. --frontend-delegated also still assumed an unfinished DOS task could
survive its submitting launcher. Both were pre-contract fixture assumptions,
not passing evidence. The updated tests assert worker exit 1067 on request
owner loss, retain no-handle publication assertions for canceled waits, and
exercise authorized root-channel data transfer while the submitting launcher
is alive. The delegated case then verifies the root process survives owner
loss. No production behavior was changed in this fixture reconciliation.

The final basesrv-service-reservation-test.exe passes
--frontend-wait-request-loss, --frontend-wait-root-loss,
--frontend-delegated and --frontend-unclaimed-stop. The last again observes
113 handles before Stop and 112 after: this proves final service-stop cleanup,
not bounded retention during a long-lived broker. In the pre-Connect path
there is no worker watch to remove the canceled route on exit, so the
long-lived resource review remains open. Remaining S2 gates are unchanged.

### Reclaim canceled pre-Connect routes during continued service

The new --frontend-unclaimed-reconnect native fixture reproduces retention
before the fix: after exact worker exit and a subsequent client Connect /
Disconnect, handle count remains 113 rather than dropping to 112. The old
Stop-only case did not cover this lifetime. The route is project-owned
authenticated frontend attachment state; original srvvdm.c has no such
attachment to reclaim. Keep its record/task cleanup unchanged and reclaim
only this finite adapter resource under the existing service lock.

On successful client admission, base_service.c now removes only canceled
routes with a signalled retained worker process handle. A live process keeps
its cancellation marker, so it cannot await or adopt a replacement root.
No process enumeration, PID-only authority, timer or scheduling state is added.
Final idle residue still belongs to service Stop. This avoids accumulating
dead canceled startup attachments across subsequent client admissions; it
does not claim asynchronous collection before any further activity.

After the fix, --frontend-unclaimed-reconnect reports 113 -> 112 before Stop,
then 112 -> 112 at Stop. --frontend-unclaimed-stop, both pending wait
cancellation cases, --frontend-delegated and --launcher-pair-exit-watch also
pass. The formal x86 /MT incremental six-file build succeeds (the first target
invocation misspelled Ninja's case-sensitive VDMREDIR.dll target and did not
build; the corrected invocation succeeded). Broker SHA-256:
0724DE60737E2F071414500BCB01A75CD0A0AD4B3B1CB9DA9D2151806C058CFC.
Fresh product regressions remain required; S2 is not closed or delivered.

The final reconnect fixture passes ten consecutive runs. Refreshed
`m0-t423-s2-prune-dos17-summary.json` passes all 17 output/exit checks and
restores all six S1 hashes. The private-desktop interactive inner-launcher
fault and three headless WOW comparisons are running as the next checks;
no result is inferred from their dispatch.

Those checks subsequently completed: `m0-t423-s2-prune-inner-loss` observes
the failed inner native chain exit -1 and worker exit 1067 while root run16
and interactive CMD survive; the recovery command and exit 23 pass, followed
by independent MEM exit 0. `m0-t423-s2-prune-wow-{winmine,sol,write}` each
retains a live worker at the known NETWORK.DRV modal until observation timeout.
No gameplay claim is made under the T423 waiver. Both staging operations
restore and hash-verify the coherent six-file S1 package. Remaining Console
ownership/input and full failure-matrix requirements still prevent S2 closure.

### Real DOS keymouse probe: passing route and unresolved repeat failure

Reused the unchanged test-only KMTST.COM from
build/M0-T420/S25/keymouse-r12 with its manifest SHA-256
E7AD00E9C0DB2E3113ACDFF83984078F42DC1267E9464A55F2FD301387BB6255;
tests/observation/keymouse_capability.asm is unchanged since 49785e1b9.
This is an authored test probe, not modified product guest media.
The native observer now logs failure stage/error and supports an opt-in
MVDM_TEST_KEYMOUSE_SHARED_CONSOLE=1 path. Its original new-Console/AttachConsole
path remains available. The builder now explicitly puts its .obj in BuildRoot.

Run under console-startup-observer.exe with MVDM_OBSERVER_PRIVATE_DESKTOP=1,
target build/M0-T423/S2/keymouse-observer.exe, working directory O:/winnt,
--observation-timeout-ms 60000 and an O:/winnt/logs/<prefix>-guest.txt argument.
Set TEST_RUNTIME_ROOT=O:/winnt and
MVDM_TEST_KEYMOUSE_COMMAND=O:/winnt/tests/KMTST.COM. No global input or desktop
switch is used. Test staging restores and verifies all six S1 files.

The original attach path fails before guest observation: both diagnostic runs
report stage=attach/error=5 after successful FreeConsole. Its cause remains
unproved; do not claim it as a product input failure or as a passing route.
The inherited-Console candidate run m0-t423-s2-keymouse-shared passes actual
guest markers for PPI, mouse reset/position, Ctrl-K, BIOS modifiers, all three
mouse callback masks and disabled-driver teardown, followed by MEM and
COMMAND exit 1. However, the final-observer repeat m0-t423-s2-keymouse-final
fails with guest S25_MOUSE_CALLBACK_FAIL, after keyboard/modifier success.
Therefore the keymouse requirement is NOT closed. Source inspection shows
the probe waits only for a nonzero callback count before testing the combined
move/down/up mask; an incomplete asynchronous sequence is a hypothesis, not
an established explanation and not permission to weaken assertions.

The same final observer and probe pass on S1 in
m0-t423-s2-keymouse-baseline. One baseline pass and one candidate pass do not
disprove a regression. Additional candidate repeats are collecting evidence.
Final observer SHA-256 is
819D6AA82D0EBA227FFABBF2C9DD0521CBA2F8B6606F255162A7BE9A8F03D0ED.
No production code or original guest media changed in this test increment.

Final observer candidate repeats m0-t423-s2-keymouse-repeat-1 through -3
finish with failure, pass, pass respectively; every run restores the six-file
baseline. This is unstable acceptance, not a majority-vote pass. Preserve the
failed guest output and investigate callback mask delivery/order against the
same baseline before changing original mouse behavior.

### Callback-set wait correction and narrowed failure

S1 baseline repeats m0-t423-s2-keymouse-base-repeat-1 through -3 all pass the
previous probe. The test-only assembly now waits for mask 7 (move/down/up),
not merely a nonzero callback count, within the same original 36 BIOS-tick
budget. All three bits and the original teardown assertions remain required.
This changes only authored test code; original guest media and product code
are untouched. The corrected Build-T420S25KeymouseGuestTest.ps1 puts both
observer .obj and .exe below its explicitly supplied repository build root.
NASM is the existing WinLibs MSVCRT mingw64/bin/nasm.exe installation.

The complete-set probe SHA-256
6FBB692F240CF1896956D7CB6AA5B139EE3CFB3C22AF54169134D87345E17D12
still yields failure/pass/pass/pass/pass in candidate runs
m0-t423-s2-keymouse-complete-1 through -5. Thus premature first-callback
checking is insufficient to explain the failure; no keymouse closure follows.

A test-only failure bitmap diagnostic was added without changing pass criteria.
Its probe SHA-256 is
536357D45966707171DC1935B7A94C026B70F301AA6C23FA1E282B1C9A736A83.
Runs m0-t423-s2-keymouse-mask-1 and -2 fail with S25_CALLBACK_MASK=1;
-3 passes. Movement reaches the guest, but down/up are absent from the
observed callback mask in both failed runs. Keyboard/modifier checks pass.
All runs restore the six-file baseline. The source-defined nt_event.c startup
DelayMouseEvents(2) discards mouse input for 110*(2+1) milliseconds, but no
runtime evidence yet ties it to these failures. Do not call that the cause.
Next investigation must distinguish host injection, copied channel records,
original mouse queue/interrupt delivery and guest callback observation.

### Source-defined startup mouse discard explains the captured failure

Temporary explicit-opt-in diagnostics compared run16 native reads with worker
decoded records. In m0-t423-s2-input-trace-3 the guest reports mask 1 despite
run16 reading all three records. In m0-t423-s2-dual-input-1, worker sequences
195/197/199 also contain exactly movement(buttons 0), press(1), release(0).
Thus that failure is downstream of correct copied transport, not evidence of
a serialization drop. Diagnostic runs are investigation, not release gates.

The final read-only diagnostic additionally sampled the original NoMouseTics.
m0-t423-s2-mouse-deadline-1 records:

| Native tick | Decoded read sequence | Buttons / flags | Original discard deadline |
| --- | --- | --- | --- |
| 955152109 | 195 | 0 / MOUSE_MOVED | 955152205 |
| 955152203 | 197 | 1 / 0 | 955152205 |
| 955152312 | 199 | 0 / 0 | 955152205 |

The original nt_event.c ConsoleEventThread calls DelayMouseEvents(2), which
sets the deadline to NtGetTickCount()+110*(2+1). nt_process_mouse discards
events while the deadline has not passed. The press arrives 2ms before it;
the later release has no preceding accepted press, so AssembleCallMask does
not report a release edge. The guest again reports mask 1. This is direct
evidence for the captured failure, not an inferred CPU defect. Other passing
runs do not authorize changing that original startup behavior.

The observer had incorrectly treated enabled Console mouse mode as completion
of this startup guard. It now waits 400ms after its ready/mode observations
before injecting the steady-state sequence. All three callback bits, the
36-tick complete-set deadline, disabled-driver checks and later MEM/COMMAND
return remain mandatory. This is test setup respecting the source contract,
not a product delay, input replay or weakened pass criterion.

Both temporary product logging helpers and their environment lookup were
removed; no original host mouse source or product guest media was changed.
The formal x86 launcher/worker/dependent DLLs were rebuilt without diagnostics.
Final probe SHA-256 remains
536357D45966707171DC1935B7A94C026B70F301AA6C23FA1E282B1C9A736A83;
observer SHA-256 is
87EB5378904C0CCAB6A6BEA4A2475477852DC2217D845DFC74E2265A0DA29D92.
Five non-diagnostic candidate repetitions are the next verification; their
dispatch does not yet count as a pass.

All five m0-t423-s2-keymouse-ready-1 through -5 subsequently pass with product
diagnostics removed. Each checks real guest keyboard/modifier/PPI, reset and
position, combined move/down/up callbacks, disabled-driver teardown, subsequent
MEM output and COMMAND exit 1. Every staging finally restores all six S1
hashes. This proves the inherited-Console steady-state keymouse slice; it
does not prove physical focus changes, clip cleanup, every input mode, or the
old separately failing AttachConsole test route. S2 remains open.

### Ordered input records and BIOS modifier release

The production-channel fixture `tests/app/console_client_test.c` now writes
seven native Console records: left press with Ctrl, focus loss, Ctrl release,
left release, 80x50 resize, menu command and focus gain. It peeks the complete
set, reads each record separately and checks exact contents/order plus an
empty final queue. The formal x86 fixture passes under the private-desktop
observer (`O:/winnt/logs/m0-t423-s2-input-order.txt`, exit 0). This proves
copied Console/IPC transport, not physical foreground activation or native
pointer clipping. No production input policy was changed for this test.

The authored DOS probe additionally queries INT 16h/AH=2 after the disabled
mouse wait and requires the Ctrl bit to be clear. Probe SHA-256:
92C642D5F92EC4EA8C255D956977B5075C068B26423287CA4BDB66F81A8F6629.
All three `m0-t423-s2-keymouse-release-1..3` runs reach
`S25_MODIFIER_RELEASE_OK` and the complete guest keymouse success markers.
Only run 1 passes the entire observer: runs 2/3 fail the later combined
MEM-return stage. The recorded error 183 is not a diagnosed cause: the old
combined screen-poll path did not set a meaningful timeout error. Do not
reclassify those two full-run failures as passes.

The observer now distinguishes MEM injection, output, and COMMAND-exit stages
and retains a separate failure screenshot text after successful guest evidence
has already been saved. Five `m0-t423-s2-keymouse-return-1..5` runs pass with
observer SHA-256
86FD16471C81113FECDD744331A8D70A1A4EBFADE4A62BBDA65303C5201B0970.
These repetitions do not establish the cause of the earlier intermittent
return failures. The guest prints success before its INT 21h exit; therefore
the sequential fixture is strengthened to wait for the current cursor-line
COMMAND prompt before MEM and again before EXIT, rather than an old prompt
in scrollback. Existing separate typeahead regression gates remain required.
No guest media or production execution/input logic is changed.

The strengthened observer is built in
`build/M0-T423/S2/keymouse-prompt-return` with SHA-256
A333893F7D3B0914A9B1166A1C73FFB964D35D7D2349246CA37728D46EB4FD51.
All three `m0-t423-s2-keymouse-prompt-1..3` runs pass the complete guest,
current-prompt, MEM-output and COMMAND-exit-1 assertions. Each staging finally
verifies restoration of the six S1 binaries. This closes the sequential
modifier-release witness, not physical focus/clip cleanup or the independent
typeahead/fault matrix. The two earlier failures remain retained, with cause
unproved rather than attributed to a product defect or declared repaired.

### Font-query owner completion (implementation admission)

Compiled-object review still finds native GetCurrentConsoleFont and
GetConsoleFontSize imports in nt_graph.c. Its cursor-size algorithm remains
directly composable and must not change. Original OpenNT
windows/core/ntcon/client/getset.c transports the output handle,
maximum-window Boolean or font index to Console Server and copies font index
and pixel dimensions back; failures return FALSE or {0,0} with last error.
The CSR transport/server cannot be composed in this standalone product.
Under the existing owner-approved frontend split, the first applicable rung
is a same-shaped binding to the two public Console APIs at run16, using the
existing copied protocol. No mirror edit/external intrusion or replacement
font/cursor algorithm is needed. Keep local native behavior without a bound
frontend and preserve original caller fallback on native query failure.
This is an S2 ownership completion, not a new font-selection policy. Verify
real-channel values/sequence, local fallback, invalid request and native error
mapping before claiming the row complete; production P regression remains due.

The two queries now use protocol 7 scalar request/reply fields; the wire
record sizes are unchanged. The scoped conapi binding redirects original
callers without changing nt_graph.c. Dumpbin on its final object shows
MvdmGetCurrentConsoleFont/MvdmGetConsoleFontSize rather than native imports.
X86 incremental formal build and dependent WOW32 relink pass. Real-channel
console_client_test checks both maximum-window flags, font index/pixel size,
native invalid-index behavior, sequence advancement and unbound local calls.
console_frontend_test rejects malformed Boolean/payload and returns exact
invalid-handle errors with empty outputs. Private-desktop reports
`m0-t423-s2-font-console-client-test.txt` and `m0-t423-s2-font-errors.txt`
both exit 0. These are host-boundary tests, not graphics rendering acceptance.

`m0-t423-s2-font-dos17-summary.json` records all 17 real regression rows passing,
including direct/nested COMMAND/MEM, EDIT, native streams and distinct exit
codes. The staging finally restored six S1 binaries. Candidate identities:

| Artifact | SHA-256 |
| --- | --- |
| run16.exe | 95CCB17C01F7582C3C5AD08B7ACAC5CF56D1B84C84FEF5B975D68394F501C688 |
| ntvdm.exe | 2B8769FDEBE7435FEB79384FBB8466D1ABC5719D3DC1D2861256147F4520D4C0 |
| WOW32.DLL | 6C1BA96A31FBE5066FAC8EB11D45F4CA379EFE78C6830C92EFE608CA8EAECB55 |

The same compiled-owner sweep exposes remaining calls which cannot be
silently considered moved: nt_graph graphics-buffer creation/activation,
nt_event display-mode/control handling, and cmdkeyb keyboard-layout query.
For the latter, original OpenNT server/getset.c first activates
Console->hklActive before GetKeyboardLayoutName. The existing local wrapper
queries the worker thread layout; moving that call to an arbitrary frontend
thread alone is not equivalence. Its active-layout source needs a bounded
disposition. Original graphics producers and control termination semantics
must likewise be distinguished from user-facing presentation. These remain
specific S2 ownership/contracts items, not a new global audit.

Headless observations on the font candidate use prefix
`m0-t423-s2-font-wow`. WINMINE and SOL retain live workers and the known
NETWORK.DRV modal. WRITE has a live worker but the single four-second window
snapshot contains no window. All launch observations reach their 20-second
timeout; this is not successful application completion. WRITE liveness is
proved, comparable modal depth is not proved by this sample. The observer
currently takes only one early window snapshot, so this cannot distinguish
late initialization from a changed frontier; retain that limitation for the
next comparison rather than treating the owner manual-test waiver as a pass.
The unchanged SYSTEM.INI hash and restored six S1 hashes were verified.

### Bounded repeated WOW observation

The checked-in observer now samples the same worker at 4/8/12/16 seconds
from launch instead of one four-second snapshot. It does not send input,
change profile/media, switch desktop or extend the existing 20-second launch
bound. Every sample records elapsed time and live process/window evidence;
desktop enumeration errors remain failures. An optional validated Guests
selection permits a targeted comparison without relaunching unrelated guests.

`m0-t423-s2-font-wow-sampled` observes all three workers alive at all four
samples. WRITE and SOL show the known NETWORK.DRV modal in every sample;
WINMINE has no window at approximately 4.1 seconds but shows that same modal
from approximately 8.0 seconds onward. This directly demonstrates why one
early empty snapshot cannot establish a regression. The earlier empty WRITE
sample remains historical evidence, not retroactively changed to a pass.
The candidate now has comparable known-modal headless depth for all three,
under the owner T423-only manual/gameplay waiver. These timeouts are not full
application execution success. The six S1 binaries and profile are restored.

The protocol-7 candidate also passes the four real nesting/typeahead rows in
`m0-t423-s2-font-nesting-summary.json`: native-root-frontend 0,
native-cmd-dos-repeat 0, dos-native-dos 1 and dos-native-typeahead 1. These
retain actual output-marker/count assertions, not only process exit checks.
The six original yield/resume source-order guards also pass. Neither replaces
the remaining per-layer abnormal-lifetime matrix.

### Exact native keyboard-layout API investigation

The installed x86 kernel32.dll exports GetConsoleKeyboardLayoutNameA/W.
Export availability alone is insufficient: the retained x86 /MT fixture
`tests/app/console_keyboard_layout_native_test.c` resolves both exact original
signatures, queries an isolated attached Console, verifies buffer canaries,
then detaches only itself and compares the thread-local query. It never
changes a keyboard layout, foreground desktop, registry or product behavior.
The build generator now contains console-keyboard-layout-native-test.exe;
the initial explicit cl build places object/exe under build/M0-T423/S2.

`m0-t423-s2-layout-native-both-contract.txt` records:

| Query | Result / error |
| --- | --- |
| Attached exact ANSI Console API | FALSE / 16385 |
| Attached exact Unicode Console API | FALSE / 16385 |
| Attached thread API | TRUE / 00000409 |
| Detached exact ANSI Console API | FALSE / ERROR_INVALID_HANDLE (6), output untouched |
| Detached thread API | TRUE / 00000409 |

Fixture SHA-256 is
5418B82CE56359989431380FB12C821C6492765CA5E84C60A8DC5EA30AC6C43C;
the observer records exit 2, explicitly unavailable, not a capability pass.
The earlier short-circuit probe failed at its success assertion with the same
16385 and did not yet call the Unicode form. Preserve both outcomes.
The experiment proves that these exported entry points are not usable as a
drop-in provider on this tested Console. It does not identify the internal
reason for 16385 or prove all Console hosts reject them. Consequently no
blind dynamic-import replacement and no arbitrary frontend-thread layout
substitution is admitted as equivalent. The existing layout seam remains
unchanged pending a bounded active-Console layout-source decision; no new
keyboard algorithm or mirror edit was introduced.

### Graphics producer and copied-presentation contract

Source inputs are selected nt_graph.h/nt_graph.c, nt_vga.c and original
OpenNT windows/core/ntcon/client/iostubs.c::CreateConsoleScreenBuffer.
The latter captures BITMAPINFO, asks Console Server for a graphics buffer,
and returns a process-local mutex and bitmap mapping. The original SoftPC
painters remain composable; CSR/shared server graphics buffers do not.
The admitted split must keep painter memory and synchronization in the worker
and copy presentation data to run16, not serialize session_video_event's
uintptr_t handles or reinterpret them as cross-process IDs. That existing
event type is local-only and is not an admissible wire frame.

| Source contract | Boundary obligation |
| --- | --- |
| MONO is 1bpp; CGA/EGA/VGA host images are 8bpp indexed | Preserve packed pixel/index meaning; do not assume all producers emit 32-bit RGB. Original guest video algorithms remain worker-owned. |
| CreateSpcDIB uses negative height and BI_RGB | Preserve top-down orientation; derive checked stride from original LONG-aligned scanlines, not untrusted biSizeImage alone. |
| DIB_PAL_COLORS stores WORD palette indices | Copy resolved palette entries with their generation; HPALETTE and WORD index table are not RGB pixels. Palette-only changes require re-presentation. |
| Painter grabs/releases bitmap mutex before InvalidateConsoleDIBits | Snapshot under the same local synchronization, release it before frontend I/O; no host pointer/lock crosses IPC. |
| Dirty rectangles have inclusive right/bottom bounds | Check signed coordinates, extent, stride, offset/span and overflow before copying. S4 conversion must account for any exclusive-edge consumer explicitly. |
| SetConsoleActiveScreenBuffer precedes palette publication | Mode, surface generation and palette ordering must survive transport; late chunks from a retired surface cannot repaint a new mode. |
| Text return reselects original text surface | Keep ordered text/stream/control events and existing final-output barrier; graphics latest-frame coalescing must not discard text or lifecycle events. |

`tests/observation/verify-dos-video-source-contract.ps1` passes 13 source
assertions covering producer bit depth, orientation, compression, palette
table, original buffer/palette calls, write-lock/invalidation order, inclusive
edges and the existing guest probe's mode13h-to-mode3 sequence. This is a
source-contract guard, explicitly not a pixel serializer or rendering test.
The future graphics transport still needs malformed-span/generation/palette
tests and production wiring before its row can be closed. Actual Window
composition remains S4; no disconnected renderer or fake success is added.

The checked-in command regression harness gains two opt-in cases with an
explicit hash-verified authored graphics-vram fixture. Builder
Build-T420S23VideoGuestTest.ps1 -GuestRoute graphics-vram produces
`build/M0-T423/S2/video-mode-return/VIDTST.COM`, SHA-256
EB0F9D1F7C20AFEB8CABB98B321E0560E138B986632A5F7E7E67F8666AB9301F.
The harness copies only this test probe to O:/winnt/tests/VTGRAPH.COM;
original guest media remains untouched. Native private-desktop observations
`m0-t423-s2-video-return-summary.json` pass direct-graphics-return (0) and
graphics-return inside COMMAND (1), requiring S23_GRAPHICS_VRAM_OK and, in
the parent route, subsequent real MEM output. The six S1 files are restored.
These results prove mode13h memory access and return to usable text on the
protocol-7 candidate, not graphics frame allocation/display acceptance.

### Direct native child admission and DOS parent recovery

The new checked-in `dos-native-loss.bat` and `Verify-BrokerFinalLifecycle.ps1
-DosNativeLoss` exercise original COMMAND running an interactive native CMD,
terminate that native target only, and require its launcher result plus real
subsequent DOS MEM output. The first attempt used LF-only test BAT bytes:
COMMAND echoed the remaining file as one argument instead of executing it.
The harness now publishes this authored fixture as ASCII CRLF. No original
guest media or product parsing was changed to accommodate the test.

The corrected fixture exposed a real missing owner edge. The retained
`m0-t423-s2-dos-native-target-loss-chain-chain.json` shows CMD parented directly
by ntvdm, with no inner run16. Original `mvdm/dos/command/cmdexec.c` passes
`pCommand32` to CreateProcess with NULL application, suspended creation, then
resumes/waits and returns its exit through original re-entry. That command
need not have a COMSPEC /c prefix. The existing adapter redirected only that
prefix; its direct-native branch bypassed the launcher lifetime pair.

Source-first disposition: retain the entire original cmdCreateProcess body
and guest. Reuse the existing command-process adapter and sibling-launcher
binding for the original NULL-application direct command too. Preserve its
already selected command text rather than applying the COMSPEC shell-tail
rewrite. Run16 retains type classification/native child creation and the
existing authenticated frontend association. No new scheduler, recursive
termination, mirror diff or standalone process manager is introduced.

Incremental x86 CCPU40 builds succeed (`native-direct-build.log` and
`native-direct-wow-build.log` under build/M0-T423/S2). The scoped run
`m0-t423-s2-dos-native-target-loss-bound` passes: exact native child and inner
run16 return -1, original DOS receives low byte 255, executes MEM with its
conventional-memory report, prints S2-DOS-END, and outer COMMAND /c returns 0.
This proves scripted DOS-parent recovery, not the entire interactive/layer
fault matrix. Candidate ntvdm SHA-256 is
2DB61E2975CCB545A20BBE3DEAADEC91905E657447F73AB96AA56BCEBE18AFA9;
WOW32 is C7A22205BBA26F849D479BEAE5CCF78C370C804AAA8BC4F9B4565D4E0268557C.
The later adapter comment clarification does not change its executable body.
Broad regression and final tested-input reconciliation remain separate gates.
The six-file S1 package was restored after the scoped test.

Follow-up evidence: `m0-t423-s2-native-direct-dos17-summary.json` passes all
17 text-gated cases; `m0-t423-s2-native-direct-nesting-summary.json` passes
all four root/repeated/nested/typeahead routes. Three independent
`m0-t423-s2-native-direct-wow-*-windows.txt` observations retain live workers
and the known NETWORK.DRV modal through 16 seconds under unchanged profile.
These are headless frontier comparisons under the T423 manual-test waiver,
not gameplay or full WOW functionality claims.

The opposite fault direction is `-DosNativeLoss -LauncherLoss`. Its first
assertion incorrectly required the Job-cleaned CMD status to equal the dead
launcher's externally assigned status. This is not the lifetime contract:
only a live launcher can collect and propagate its target's status. Both
processes had terminated and DOS had already run MEM. The corrected test
records both statuses, requires both process handles signalled and launcher
-1, and retains exact propagation for the target-death case. Run
`m0-t423-s2-dos-native-launcher-loss-job` passes: CMD 0 (Job cleanup), launcher
-1, DOS low byte 255, real MEM and final marker, outer COMMAND /c 0. No product
change was made to manufacture matching cleanup codes. Earlier failing runs
remain evidence. Six-file baseline restoration, source-order checks (six)
and documentation governance pass; this still does not close S2 or publish P.

### Native graphics allocation and quarantined-provider disposition

The selected original `nt_graph.c::graphicsResize` calls native
CreateConsoleScreenBuffer with CONSOLE_GRAPHICS_BUFFER; current conapi does
not intercept that call. Its successful VRAM probe does not prove allocation
or frame publication. A checked-in `tests/app/console_graphics_native_test.c`
now exercises the exact selected top-down BI_RGB/DIB_PAL_COLORS allocation
for 1bpp and 8bpp, preceded by a native text-buffer control. Compile x86 /MT
with host-compat/include and kernel32.lib; the formal graph generator adds
console-graphics-native-test.exe (generator syntax checked, existing graph
not regenerated for this isolated diagnostic). Initial object/executable live
under build/M0-T423/S2, executable SHA-256
6D41EFF0BCBD7166E4C68C0E6C1CBC79289BA8C8AC470F86C2C9731AB50E2FC9.

Private unswitched-desktop observer output
`m0-t423-s2-graphics-native-boundary.txt.console.txt` records successful text
control and both graphics depths rejected with ERROR_INVALID_PARAMETER (87),
NULL bitmap and NULL mutex. Target exit 2 means UNAVAILABLE, not a functional
pass. The observer's exit 0 is not substituted for that result. No guest,
runtime binary, registry, profile or visible desktop was changed.

Original `OpenNT/windows/core/ntcon/server/bitmap.c::CreateConsoleBitmap`
normalizes positive height to negative, copies BITMAPINFO, allocates a section,
maps server/client views and returns a duplicated mutex. Its SCREEN_INFORMATION,
CSR client process and server ownership prevent direct standalone composition;
the retained MVDM painters need only the finite local buffer/mutex contract.
`closeGraphicsBuffer` first selects text, closes the graphics identity, then
closes the painter mutex. Preserve that independent mutex-handle lifetime.

Per-file read-only review of reference branch 286d54a30:

- graphics_buffer.c has reusable bounded allocation/palette-copy mechanics,
  but its header and snapshot result depend directly on kvm_window_frame and
  KVM limits. Do not import that dependency into the worker. Its positive-height
  bottom-up handling is also not the original server's forced-top-down behavior;
  selected negative-height inputs agree, broader equivalence is not claimed.
- graphics_api.c retains useful same-shaped create/select/close/invalidate
  boundaries, but its global binding and text_route composition belong to the
  rejected worker-owned frontend. Do not import the file wholesale.
- graphics_console.h mixes painter storage with text snapshots, ready event,
  mode and presentation state. Split only the admitted local backing contract
  from frontend-owned display decisions; no parallel worker display controller.

Next production boundary remains explicit: original painters write worker-local
storage; lock-bounded snapshots release the painter mutex before IPC; only
copied dimensions/stride/pixels/resolved palette and surface generation cross
to run16. Current 16KiB messages cannot contain a 320x200 indexed frame, so
chunk completion and stale-generation rejection must be tested before frame
publication. Neither partial chunks nor an unavailable allocation may be
reported as a displayed frame. S2 copied-boundary tests and S4 actual Window
composition are still open; this diagnostic does not mark either complete.

### Local bitmap binding implementation (not yet production-selected)

`ntvdm-exe/win32/console_bitmap.[ch]` now implements the finite local backing
identified above. It deliberately has no KVM header, Window, frame conversion,
frontend policy or guest parser. The audited reference implementation's
allocation/palette-copy approach is retained, with the original top-down
normalization and packed DWORD-aligned 1bpp/8bpp rows. Dimensions fit the
original signed Console rectangle; arithmetic is widened before allocation.
biSizeImage does not override the derived allocation span. The independently
duplicated painter mutex outlives backing disposal, as original close order
requires. Final composition must stop all buffer users before disposal.

Palette entries and pixels are copied under that mutex into caller-owned
storage; only the subsequent transport may issue IPC. DIB_PAL_COLORS cannot
produce a valid snapshot until its palette has been supplied. Malformed
palette updates leave the previous complete palette unchanged. A timed-out
snapshot reports ERROR_BUSY. An abandoned painter mutex permanently invalidates
this backing for snapshots, rather than publishing potentially half-written
pixels on the next call. This is explicit finite transport failure handling,
not recovery of a guest task or a new display policy.

`tests/app/console_bitmap_test.c` builds x86 /MT /W4 /WX with the provider,
kernel32.lib and gdi32.lib. Objects, executable and final log are under
build/M0-T423/S2; five successive runs pass, followed by the retained
console-bitmap-test.log run. Checks cover both bit depths, padded rows and
sentinels, nonidentity WORD palette indices, unavailable/invalid palettes,
insufficient capacity, malformed header/size/depth/extents, RGB palette bytes,
positive-height normalization without flipping, held/abandoned mutex, and
duplicate-handle lifetime after disposal. Generator registers an explicit
console-bitmap-test.exe target and its syntax parses; graph regeneration is
pending. No machine/guest is executed by this local fixture.

This is an uncommitted implementation step, not a wired capability or P.
Production create/select/palette/invalidate/close binding and copied frontend
frame delivery must be completed together, followed by real guest and DOS17
regression. No S2 checklist row is closed merely by these unit tests, and
the O:/winnt six-file package has not changed.

### Protocol 8 copied-frame sender/receiver

The current build now selects run16/console_video.c through its production
Console dispatcher and channel teardown. Copied description contains dimensions,
packed stride/depth/span and 256 scalar 0x00RRGGBB entries, not a KVM structure
or native identity. BEGIN validates widened layout arithmetic and allocates a
pending frame; contiguous DATA publishes only the complete frame. TEXT retires
pending and complete frames. Nonzero increasing serials reject late pre-text
data. Broken offsets discard pending data without exposing it as a new frame.
Channel teardown disposes storage after joining its users.

The worker sender holds the existing transport critical section for the full
BEGIN/DATA transaction, so text/stream/control requests cannot interleave its
chunks. Its caller must first copy and release the painter mutex. Capacity,
serial exhaustion and transport errors fail explicitly; NULL description is
the text transition. Original guest graphics API hooks have not yet been
installed, so this is not a guest frame-delivery claim.

Formal graph regeneration and x86 incremental five-program build plus WOW32
relink pass (video-graph.log, video-formal-build.log, video-sender-build.log and
video-sender-wow-build.log below build/M0-T423/S2). The graph now registers the
bitmap, video and native allocation tests and includes the new frontend source
in run16's source manifest. Five isolated fixture reports under
O:/winnt/logs/m0-t423-s2-video8-* have target exit 0: video dispatcher, bitmap,
Console dispatcher, real client transport and mock-USER pointer dispatch.

The extended real-client fixture sends 64,000 bytes in four payload chunks,
asserts first/last pixels, copied palette, complete serial and TEXT retirement.
Its first run terminated 1067 because the fixture had already signalled its
own stop event before the new exchange. Moving that existing stop after the
exchange fixes the test ordering without a product change; retained
video8-real-channel-live report exits 0. The orphaned readiness helper from
the failed fixture was identified by exact image/argument and stopped.

An isolated /WX build initially rejected an existing console_grid warning;
that unchanged dependency was compiled using its formal /W4 /we4013 flags.
The new test also initially collided with Winsock's send declaration; its
helper is now named dispatch_frame. Both failed attempts remain diagnostics,
not product regressions or passing runs. Original API hook wiring, real guest
tests, DOS17/WOW comparisons on this new candidate and coherent publication
remain pending; O:/winnt is unchanged and no S2 P is delivered.

### Original graphics API selection and palette ownership

The subsequent formal graph selects console_graphics.c for the original
nt_graph CreateConsoleScreenBuffer, SetConsoleActiveScreenBuffer and
CloseHandle calls. The original object has unresolved references to the three
Mvdm wrappers and the final map resolves them to this provider. This retains
the original painter with worker-local bitmap/mutex backing and copied frame
transport; no guest or mirror edit and no worker KVM renderer was added.

OpenNT windows/core/ntcon/server/private.c SrvSetConsolePalette (lines 485-489)
deletes the previous installed palette when replaced. output.c FreeScreenBuffer
(lines 1016-1020) deletes the last palette at buffer destruction. The private
Console server cannot compose here; the bounded binding retains that ownership
rule locally. Reinstalling the same palette does not delete it; successful
installation transfers ownership before repaint, including a later transport
failure. Close preserves the transport error across resource cleanup.
Reactivating an existing surface republishes its pixels; first activation before
the original caller installs a palette remains allowed.

The checked-in console_client_test exercises the production API shape over the
real pipe: allocation, activation, palette installation/replacement, painter
mutex writes, invalidation, pixel/palette receipt, invalid rectangle rejection,
text retirement, reactivation and close. Its first ownership assertion used
GetObjectType on a deleted handle and failed at line 551; that is not proof
of continued palette usability. The revised assertion uses GetPaletteEntries
and a distinct replacement palette. The private-desktop report
O:/winnt/logs/m0-t423-s2-graphics-palette-channel-2.txt records target exit 0;
the independent painter mutex remains usable after surface close. Bitmap and
video-dispatch fixtures also pass. The failed fixture's exact-image readiness
helper was stopped; no unrelated process was targeted.

Incremental x86 production/fixture build and WOW32 relink pass, recorded in
build/M0-T423/S2/graphics-palette-build.log and graphics-palette-wow-build.log.
Real graphics-return and all 17 DOS routes pass under the
m0-t423-s2-palette-return and m0-t423-s2-palette-dos17 log prefixes.
The three independent m0-t423-s2-palette-wow observations retain live workers
through 16 seconds and the known NETWORK.DRV modal. Observation timeouts are
bounded test termination, not guest success; this is headless non-regression
evidence under the owner's T423-only manual/gameplay waiver.
The staging helper completed with exit 0 and verified restoration of the
coherent six-file S1 runtime package. Tested candidate SHA-256 values:

- run16.exe: F3C8D341122A920C95965A0E1E80138266A59004D8E10C949F780CFD7C824ACA
- ntvdm.exe: F75D1E349DBC16D8D4C585BA6D7457641033E89D6C1F7536486A4D44A0E3AB24
- WOW32.DLL: B7874C4765E7F7001F1B32FEEDE441A7E0A847579F321B3DC87229177111B4CB

These API fixtures do not prove a real guest painter delivered a frame, nor
Window rendering, and do not close S2 or deliver a production P.

### Real guest frame witness and unresolved text return

Added tests/observation/video_frame_delivery.asm: an independently authored
mode-13 DOS probe writes color indices 12h and 2Ah into the two 32,000-byte
halves of A000h, waits 55 BIOS ticks, then selects mode 3, prints a marker and
exits. Its default variant waits another 18 ticks after printing; NASM
IMMEDIATE_EXIT preserves the no-dwell case. No original guest media changes.

The formal graph has a test-only run16-video-observer.exe target. It executes
the production receiver via tests/app/console_video_observed.c and logs only
complete received frames plus text retirement. It is not selected by the
product run16 target. tests/app/run16_package_observed.c redirects only that
test translation unit's self-relative package discovery, permitting the test
EXE to stay below build while consuming O:/winnt. It does not substitute
authentication, guest execution, painter, protocol or frame validation.
This is instrumented frontend integration evidence, not a test of production
launcher path discovery or final Window rendering.

Reproducer: NASM -f bin -o build/M0-T423/S2/VFRAME.COM
tests/observation/video_frame_delivery.asm; then
tests/observation/verify-video-frame-delivery.ps1 with FormalBuild
build/M0-T423/S1/restart-formal-x86, WowBuild
build/M0-T423/S1/restart-wow-x86, Backup
build/M0-T423/S2/baseline-db4ab61eb2db41b296d49d3ce6b8de6d,
GuestProbe build/M0-T423/S2/VFRAME.COM and a fresh Prefix. Pass absolute paths.
The script stages six candidate binaries, uses an unswitched private desktop,
requires exact pixels/palette and text output, then restores and verifies all
six baseline hashes even on failure. No test EXE enters the package root.

Observations under O:/winnt/logs/m0-t423-s2-real-frame-2 and -3:

- A complete 640x400, 8bpp frame reaches the frontend: a=128000, b=128000,
  other=0; paletteA=202020 and paletteB=fc7c00. This directly proves the
  authored guest VRAM -> original painter -> local bitmap -> IPC -> production
  receiver path, not merely host-side API calls.
- A later TEXT record succeeds, and the launcher returns 0. Nevertheless the
  final Console capture lacks GRAPHICS_FRAME_RETURN_OK. Both immediate exit
  and an 18-tick text dwell fail that assertion. Therefore a short exit delay
  alone has not explained or fixed it; neither run is a full passing test.
- The earlier -1 attempt used a relocated package under build and exited 1067
  before any frame. Its startup cause remains unproved; the runtime-package
  test above separates that fixture relocation from graph delivery.
- The script restored O:/winnt after both failed assertions. No final S2
  publication, commit or P closure occurred.

Original nt_cga.c nt_init_text calls closeGraphicsBuffer; nt_graph.c selects
text painters, textResize and the original mode-change path. The next step is
to trace text-mode painting and frontend Console writes around that return,
not to declare this an original guest defect or assume a final-drain cause.

### Text-return failure localization

The same test-only observation translation unit now includes the production
Console dispatcher unchanged behind a logging wrapper. The observer link
substitutes that object instead of the separately compiled dispatcher; the
product run16 target remains unchanged. It records incoming stream/cell writes,
their payload text and the real dispatcher/Win32 result.

O:/winnt/logs/m0-t423-s2-real-frame-text-4 passes the full test. Following the
known picture and TEXT serial 3, WRITE_CELLS_A sequences 78 and 86 each carry
the marker in an 80x25 screen; status=0, result=1, error=0. This does not repair
or explain the previous failures. The immediately repeated -repeat-1 fails:
the same known picture is received, then a partially changed graphics picture
and TEXT serial 4, but no text WRITE operation reaches the dispatcher at all.
The three-repeat batch stops on that first failure, so it is not three runs.

The probe now also calls original INT10/AH=0F after selecting mode 3 and exits
49 if AL is not 3. The -frame-mode-check run still returns task status 0 but
fails the marker assertion, confirming the guest-visible BIOS mode selection
without proving host painter selection. Its package restoration passes.
The missing incoming write rules out merely losing an already-received text
payload in the frontend for this failure; it does not prove that frontend
timing cannot influence the worker. No production repair has been made.

Next source-defined boundary: nt_graph.c nt_graphics_tick defers mode selection
through mode_change_now/get_mode_change_required and choose_display_mode;
nt_set_paint_routine selects the text initializer, and nt_cga.c nt_init_text
closes the graphics buffer. Capture these original states before assigning a
root cause. The retained failed tests remain S2 gates, not an original-guest
limitation, a waived test, or a reason to modify CCPU semantics speculatively.

### Original video-tick diagnostic

A diagnostic-only copy of nt_graph.c lives under build/M0-T423/S2, compiled
with the formal object's flags and linked explicitly before the original host
archive into ntvdm-video-observed.exe. Its map resolves nt_graph symbols to
that diagnostic object; original source and formal objects remain unchanged.
The probe logs at nt_graphics_tick entry only. The first attempt used fprintf,
which the selected CRT facade redirects and left an empty log; the second uses
Win32 file I/O. Neither diagnostic binary is a production delivery. The test
script's optional DiagnosticWorker parameter selects it only during the
temporary six-file run and still restores the coherent baseline in finally.

The retained video-tick-2 and video-tick-3 runs fail the text marker. At their
last original video tick, bda=3, type=GRAPHICS, pending=1, delay=0: mode selection
has not yet reached its two-tick completion. Prior ticks report bda=19 and
pending=0. The original EGA_TICK_DELAY is 2 and TICKS_PER_FLUSH is 2; do not
invent a large fixed display delay or blame a nonexistent ten-second policy.

The authored TICK_REPORT probe writes the final wait's two WORD values to
O:/winnt/logs/VFTICK.BIN. In video-tick-3 they are 45391 and 45409, exactly
18 BIOS ticks: this rules out the proposed unsigned-backward-tick explanation
for this run. It does NOT prove 18 host video ticks or one second of wall time.
Original time_day.c INT1A reads TIMER_LOW/HIGH, and its BIOS IRQ path increments
that counter. nt_timer.c separately posts CPU_TIMER_TICK by host time, expressly
without making up lost events. These are distinct clocks; the precise reason
for their observed divergence here still needs measurement. No production
timer, CCPU, guest or mode-selection change is justified by this trace alone.

Current concrete failure boundary: task completion can precede the pending
original host mode-selection/paint, and no text payload has then been sent to
the frontend. Next verify the source-defined final-output boundary and measure
the copied-frame blocking interval before selecting a minimal repair. Keep
both the immediate-exit and paced/live-display tests; one cannot waive the
other. All diagnostic runs restored O:/winnt and no S2 P is delivered.

### Source-owned final-paint repair

Original nt_event.c nt_block_event_thread already blocks the input thread and
performs a final update_alg.calc_update before ResetConsoleState and timer
suspension. That call uses the old painter if nt_graph.c's deferred mode
selection is still pending, matching the observed final graphics frame instead
of text. The selected repair stays at this original final-flush owner: before
that existing call, advance the unchanged host_graphics_tick while pending,
bounded by the original EGA_TICK_DELAY. It completes the original state machine
including its private countdown; it does not reimplement mode selection,
advance guest time, inject interrupts, sleep, or change CCPU behavior.

Recovery order: directly reuse nt_graph's existing selected video provider;
no new provider or adapter is needed. A run16-side fix cannot create a missing
text frame, and simply draining IPC cannot repair a painter that was never
selected. Generic timer/CCPU changes and a replacement text renderer are
rejected. The three executable-line insertion stays in the original handoff
owner and is registered as MVDM-HOST-DIV-311. The reason BIOS and host tick
counts diverge does not become a speculative timer repair: the completion
boundary must flush its pending output regardless of preceding timing.

Incremental x86 build and dependent DLL relinks pass (final-paint-build.log and
final-paint-wow-build.log under build/M0-T423/S2). Validation runs the formal
worker, not ntvdm-video-observed.exe. Both paced and IMMEDIATE_EXIT variants
must pass exact guest pixels/palette, text retirement and final Console marker;
the frontend remains test-instrumented only for observation. Subsequent DOS17,
nesting and headless WOW comparisons remain required before any delivery.

The formal-worker final-paint-VFRAME-1/2/3 and final-paint-VFEXIT-1/2/3
reports all pass the complete assertion set. The latter have no text dwell.
The separate final-paint-production case selects the actual O:/winnt/run16.exe
and formal worker with no test code in either process; exit 0 and the actual
Console marker pass. It proves production final-output behavior, while exact
pixel/palette receipt is provided by the six instrumented-frontend tests.
All seven restore checks succeed. The tested formal ntvdm SHA-256 is
57D7505BF19CD93E9546FF3F96AAB507E3D4EB2FFC5907BC1BAC98A4A2E1B91C;
WOW32 SHA-256 is F178663170F6AF08BA41AAD59F3A3B45C2519F8B0D26193EC647F3874320E2F1.
The source-contract guard now also checks that this final paint follows the
original bounded pending-mode completion; it supplements, not replaces, runtime
verification. Broad regressions subsequently completed: the
final-paint-dos17-summary.json records all 17 expected results, and
final-paint-nesting-summary.json records all four nesting/typeahead routes.
The corresponding Console captures and test assertions check actual output,
not only exit status. The final-paint-wow WINMINE/SOL/WRITE window snapshots
retain the known NETWORK.DRV modal and live workers through the bounded
headless observation. Under the owner's T423-only waiver this is the retained
headless frontier, not a claim of interactive WOW functionality. These logs
reside under O:/winnt/logs with the m0-t423-s2- prefix. Each staging operation
restored the coherent six-file S1 backup. No P is delivered; the remaining
S2 layer-fault and Console-owner gates remain open.

### Final-paint candidate lifetime and packed-frame follow-up

The unchanged six-file candidate was re-tested with
tools/audit/Verify-BrokerFinalLifecycle.ps1 via the temporary staging helper.
All four runs restore the coherent S1 package; private desktops remain
unswitched. Logs under O:/winnt/logs use these exact prefixes:

- m0-t423-s2-final-paint-interactive-worker: NestedWorkerLoss and
  NestedInteractive. Nested launchers/native intermediary return 1067; the
  outer run16 and interactive CMD survive, execute NESTED-RECOVERED, return
  23, and a subsequent independent MEM produces its actual memory report.
- m0-t423-s2-final-paint-interactive-launcher: same arguments plus
  LauncherLoss. Killing the innermost DOS launcher ends the affected worker
  with 1067; the external interactive parent executes the same recovery and
  MEM assertions. Native intermediary/launcher exits retain -1.
- m0-t423-s2-final-paint-dos-native-target: DosNativeLoss. Killing the native
  target makes its launcher return -1; original DOS observes low-byte 255,
  runs MEM and completes its script with 0.
- m0-t423-s2-final-paint-dos-native-launcher: DosNativeLoss and LauncherLoss.
  The direct native target exits through its existing Job ownership (observed
  0), its killed launcher returns -1, and DOS again observes 255 and executes
  MEM. This is pair cleanup, not a claim of recursive descendant termination.

These are refreshed specific cases, not completion of the full root/middle/
inner fault matrix. In particular they do not test every middle-layer victim.

The checked-in tests/app/console_video_test.c now exercises production frame
dispatch with a 9x2, 1bpp frame: DWORD-padded rows, exact packed bytes/palette,
an 8bpp-to-1bpp replacement hidden until its final chunk, TEXT retirement of
both the completed frame and a partially received replacement, and rejection
of late DATA. Incremental MSVC Win32/x86 /MT compilation and execution of
console-video-test.exe pass. This is host protocol/receiver evidence only;
it neither renders a Window nor certifies guest monochrome video modes.
The source-contract script initially failed before running any assertions
under Windows PowerShell: PSScriptRoot was empty during parameter-default
evaluation. Resolve its default repository root in the script body instead;
the same no-argument invocation now passes all 14 source guards. Documentation
governance and git diff --check also pass. No production input changed in this
follow-up and no P is delivered.

### Native display-mode owner migration

Selected original config.c, nt_det.c and nt_event.c call GetConsoleDisplayMode
and SetConsoleDisplayMode. OpenNT windows/core/ntcon/client/private.c retains
the BOOL result, output flags or COORD written on success, and failure error;
its implementation invokes the private CSR Console server (Set also waits on
the server's completion event). Direct composition of that client requires
the excluded NT4 CSR/Console server and cannot operate the modern Console.
The smallest same-shaped binding is the existing native Win32 API, executed
by the authenticated run16 Console owner instead of the worker. Copy only
flags and signed dimensions. No new display algorithm, server, mirror body,
fullscreen emulation or guest policy is needed; native failure stays failure.
This is independent of S4's product display flag and kvm-window policy.
Protocol 9 adds the two operations; tests must compare success/output/error
with native calls, preserve no-channel fallback and reject malformed payload.
The unchanged original calls remain the owner of when and why they execute.

The two real-channel/native fixtures pass with logs
m0-t423-s2-display-fixed-console-client-test.txt and
m0-t423-s2-display-fixed-console-frontend-test.txt under O:/winnt/logs.
They prove native query/windowed-set results and dimensions, invalid-handle
error with unchanged output, null-output rejection without sending IPC,
unbound native query, and malformed-payload rejection. This is not hardware
fullscreen or kvm-window acceptance.

An earlier test wrongly required 0xffffffff flags to fail. Modern Console
accepted them and changed dimensions: consecutive native/remote calls reported
120x30 and 174x45. Comparing those state-changing calls as identical-state
observations was invalid. The display-flags-values Console capture retains
the evidence; no product special case was added. The fixture now uses the
original WINDOWED operation and an invalid handle for deterministic negative
coverage. Exact-path readiness child processes left by failed assertions were
ended before relinking; they were test helpers, not owner sessions.

Formal x86 build and dependent WOW relink pass (display-owner-build.log and
display-owner-wow-build.log under build/M0-T423/S2). Object inspection proves
nt_event.obj references MvdmSetConsoleDisplayMode. The current config/nt_det/
nt_fulsc objects contain no display-mode reference: their conditional original
source is not evidence of an executed caller. No new source body or mirror
diff is introduced for this API migration. Candidate SHA-256:

- run16.exe: FED646191882ACF5F96D3E7512BA7EDFFB7D980D5E132EC54FC25C30CCBF6D58
- ntvdm.exe: 91771277F24F99F8D756D03180DBA7DC1BB938E016626CE704C2A80352077E9F
- WOW32.DLL: 6FD32F54BB82CE161A6FE7183AC3FE9C9AB177C1DC619DC22DD3262AA519CEA8

The display-owner-dos17-summary.json records all 17 text-gated routes passing.
The display-owner-wow WINMINE/SOL/WRITE observations retain the known
NETWORK.DRV modal and live workers through 16 seconds. These bounded headless
comparisons satisfy the stated T423 waiver boundary, not gameplay acceptance.
Staging finished with all six S1 files restored. Documentation governance and
git diff --check pass. No production publication or P delivery is claimed;
remaining S2 owner/lifetime gates are unchanged.

### Middle native lifetime pair, without recursive cleanup

Verify-BrokerFinalLifecycle.ps1 adds MiddleLayerLoss, requiring
NestedWorkerLoss/NestedInteractive. The exact real chain is outer run16 ->
interactive CMD -> middle run16 -> CMD /c -> inner run16 -> DOS COMMAND.
Identity is selected from the observed test-owned parent relationships and
retained process handles, not a global name-based termination.

On the protocol-9 candidate, m0-t423-s2-middle-target and
m0-t423-s2-middle-launcher both pass. Killing the middle native target returns
-1 through its launcher. Killing the middle launcher returns -1 for it and
0 for its Job-cleaned direct target. In both cases root run16/outer CMD and
inner run16/DOS worker remain live: this proves the required direct pair
cleanup does not introduce recursive native descendant termination.

The test then explicitly injects a SECOND fault into its surviving worker;
the inner launcher returns 1067. Only after that release does the observer
send outer CMD's recovery input. CMD executes NESTED-RECOVERED, exits 23,
and a new MEM produces its memory report. Reports/results/chain identities
are under O:/winnt/logs with those prefixes. Both staging finally blocks
restore all six S1 hashes. This is deliberately not evidence that competing
native/DOS input readers are safe while both layers remain alive; that
intermediate ownership case remains open, not disguised as a pass.

A refreshed seven-object Console import sweep finds the bound stream, cell,
font, display, pointer and graphics calls. Remaining native GetKeyboardLayout
in nt_event is used by key normalization, not a Console renderer; its locale
source still needs the recorded layout decision. SetConsoleCtrlHandler
registers the worker's original CntrlHandler (guest close policy/state), not a
user input reader: moving its function pointer over IPC is invalid. The
run16 handler only retains waits for Ctrl-C/Break. Source inspection alone
does not certify control-event ordering. ConsoleMenuControl and
SetConsoleKeyShortcuts retain explicit unavailable returns, while
ShowConsoleCursor has the previously registered local count without native
pointer mutation. Their final frontend/limitation reconciliation remains
required; the sweep is finite owner evidence, not whole-S2 closure.

### Pre-delivery review, original line endings and repeated runtime gates

The protocol-9 production candidate remains uncommitted. The review rebuilt
the reservation, bitmap, video, pointer and frontend-resource fixtures under
the existing MSVC Win32/x86 /MT caches. Five standalone fixtures and ten
service cases pass: frontend-root, frontend-unclaimed-stop/reconnect,
frontend-delegated, frontend-wait-root/request/worker-loss, frontend-rundown,
launcher-pair-loss and launcher-pair-exit-watch. Logs use
O:/winnt/logs/m0-t423-s2-p1-review prefixes; the build log is
build/M0-T423/S2/p1-review-fixture-build.log. These are native boundary/resource
checks, not replacement evidence for DOS execution or physical focus.

Both pinned original nt_event.c copies use CRLF. The working copy's LF-only
format was restored to CRLF, with normalized contents verified unchanged.
Against HEAD, raw Git accounting is 2079 added/2073 removed lines; ignoring
line-ending whitespace leaves 12 added/6 removed lines. Do not count the
format-only difference as new behavior or claim that every normalized line
is newly authored code. Incremental ntvdm and dependent DLL builds pass;
logs are p1-mirror-format-build.log and p1-mirror-format-wow-build.log in the
same S2 build root. Candidate SHA-256 after those builds:

- ntvdm.exe: 39CD981E3CB4FD21A449E8C9B93FF2905373D240698E3C772450A74C48963727
- WOW32.DLL: AEA959754CB2F2AA287FE970AE8DDA28BA724B1848B057DF01F9B6D2488CCD8A
- run16.exe: FED646191882ACF5F96D3E7512BA7EDFFB7D980D5E132EC54FC25C30CCBF6D58

Real keymouse, all 17 DOS routes and the four native/DOS nesting/typeahead
routes pass with m0-t423-s2-p1-format-keymouse/dos17/nesting prefixes. Separate
WINMINE, SOL and WRITE observations under the format-wow prefix retain the
known NETWORK.DRV modal and live worker through the sampled interval, under
the owner's T423-only headless waiver. No modal dismissal or gameplay was
performed or inferred.

The rebuilt test-only frontend observer receives exact guest pixels/palette
and subsequent text retirement; the uninstrumented product launcher also
retains GRAPHICS_FRAME_RETURN_OK on immediate graphics-to-text exit. The
format-video and format-video-standard observations prove those respective
assertions, not Window rendering. The second run's restoration initially
failed on a transient ntvdm.exe sharing violation. Follow-up enumeration found
no live package process; six-file hash comparison exposed the incomplete
restore. All six S1 files were then explicitly restored and verified.

The checked-in verify-video-frame-delivery.ps1 now retries only sharing/lock
violations (32/33), at 200ms intervals bounded to five seconds. Other failures
and retry exhaustion remain fatal. A locked build-local destination proves
bounded failure; release followed by copying proves matching hashes. Repeated
real observed and standard-frontend runs with p1-restore-video prefixes both
pass and finish with verified coherent restoration. This is test-harness
cleanup, not a change to product lifetime policy. The ABI README now links
the actual guest-frame test instead of its stale pending-observation claim.

O:/winnt is again the coherent S1 baseline. No production P, S2 closure or
Window delivery is claimed. The remaining source-owner/control/input and
per-layer failure gates still require their stated evidence before closure.

### Real Console control events and keyboard-layout attribution

The original OpenNT base/mvdm/softpc.new/host/src/nt_event.c::CntrlHandler
handles CTRL_C_EVENT and CTRL_BREAK_EVENT without terminating the worker.
Its close/logoff/shutdown policy instead consults worker-local state; that
function is not a frontend input reader or a callback pointer to serialize.
The launcher retains its wait while the target receives the native event.

The checked-in tests/observation/verify-run16-dos-control.ps1 now has fresh
candidate evidence: m0-t423-s2-control-owner-current-r2-c.txt and -break.txt
under O:/winnt/logs. Each generated-control sidecar proves successful real
event generation (0 and 1 respectively); guest VER/MEM text, zero remaining
scripted input and COMMAND's original exit 1 prove subsequent operation.
This closes the tested C/Break continuation assertion, not Console close,
shutdown or arbitrary handler-order equivalence. No product handler changed.

The initial control-owner attempt used the older formal-cache observer and
produced no event sidecar, so it is explicitly NOT control-event evidence.
The source was newer than that EXE. Generate-ObservationNinja.mjs now builds
the current observer in build/M0-T423/S2/control-observer using x86 /MT /W4
/WX. Three test-only fopen calls were changed to existing fopen_s style to
meet that build's warning gate. A failed-build invocation never entered the
test; its staging finally restored S1. The successful repeated control test
also restored and verified the complete S1 package. Earlier DOS/frame reports
remain limited to their actual observer inputs, not retroactively attributed
to this rebuilt tool.

Source attribution also corrects ADAPTER-WIN32-034's prior equivalence claim.
OpenNT windows/core/ntcon/server/getset.c::SrvGetConsoleKeyboardLayoutName
first activates Console->hklActive, then calls GetKeyboardLayoutNameA/W.
Its client/getset.c counterpart transports the named Console request through
CSR. A bare worker GetKeyboardLayoutNameA lacks that Console selection, even
though the nine-character output shape matches. Modern exported Console
queries have already returned 16385 attached / 6 detached in the retained
native fixture. The adapter comment/register now state these observed facts;
the existing fallback behavior is unchanged. Neither copying a frontend
thread's HKL nor returning success from a different thread is accepted as
Console-layout equivalence. The bounded layout ownership question remains
open; this correction adds no layout cache, keyboard algorithm or mirror diff.

### Console layout source: demonstrated thread divergence

The bounded source follow-up identifies the historical input owner precisely:
OpenNT windows/core/ntcon/server/output.c handles WM_INPUTLANGCHANGE by storing
lParam in Console->hklActive. WM_SETFOCUS initializes an absent value from
SPI_GETDEFAULTINPUTLANG, then activates that Console-specific value. The
server getset.c query uses that saved value, not the requesting worker's HKL.

console_keyboard_layout_native_test.c now records the Console HWND, its class,
owner thread/PID and that thread's HKL. In the unswitched private desktop,
m0-t423-s2-layout-window-owner-r2.txt records ConsoleWindowClass and both local
and window-thread HKL 04090409, while the old Console exports still fail 16385.
Equality on this one host is not an equivalence proof for Terminal/pseudoconsole.

The subsequent layout-thread-separation run activates an already-loaded
08040804 only on the disposable fixture thread, obtains name 00000804, and
observes the Console window thread still at 04090409. It then restores the
fixture thread to its original layout and verifies restoration. No layout is
loaded/unloaded, no Registry setting changes and no foreground is activated.
This is a concrete counterexample to the former claim that worker-thread
GetKeyboardLayoutName necessarily reports the Console input locale. It is not
a product fix or proof of dynamic Terminal layout support.

The formal fixture target previously omitted USER32 despite its existing
GetKeyboardLayoutName dependency. It now uses the already-declared native
fixture rule that links USER32; MSVC x86 /MT compilation/link passes. A first
attempt referenced a later Ninja rule and was rejected by the parser; the
final generator uses the earlier declared rule and regenerates successfully.
Both failure attempts are build/test failures, not passing runtime results.
The explicit fixture result remains UNAVAILABLE / exit 2 for historical
Console API availability; successful owner observations do not relabel it PASS.
Product files and O:/winnt are unchanged by this investigation.

### Real Console close, direct and nested

Verify-BrokerFinalLifecycle.ps1 -ConsoleClose now exercises native close,
separately from killing a launcher or sending C/Break. It requires a private
desktop, waits for real DOS prompt output and retains process handles before
releasing the observer's input gate. The observer posts WM_CLOSE only to its
own Console and independently verifies its thread is on an NTVDMConsoleTest-
desktop. Its existing control handler records the native CTRL_CLOSE_EVENT
(DWORD 2). The controller checks termination before its finally cleanup.
No product hook, guest mutation or user-desktop input is involved.

Direct COMMAND and the NestedWorkerLoss variant both pass on the protocol-9
candidate. The latter observes three run16, two native CMD and one DOS worker
in the real nested chain. All six complete after native close without test
termination. Native exit codes are recorded, not mistaken for normal command
results: four upstream processes return STATUS_CONTROL_C_EXIT, while the
innermost launcher and worker return 0 and 255 in this run. Direct shutdown
likewise returns 0/255; this does not claim a successful guest command result.

Final evidence prefixes under O:/winnt/logs are
m0-t423-s2-console-close-guarded and
m0-t423-s2-console-close-nested-guarded. The close-chain JSON records exact
test-owned identities; console-close.txt.control.bin contains DWORD 2;
results.txt records each pre-cleanup completion. Both earlier unguarded
versions also passed, but final acceptance uses the guarded observer rebuilt
with MSVC x86 /MT /W4 /WX in build/M0-T423/S2/control-observer.
Temporary candidate staging restores and verifies all six S1 files after
each run. This closes the selected native Console-close cleanup assertion;
it does not simulate logoff/shutdown, certify Window X behavior, or close
the still-open middle-pair input ownership case.

### Middle-pair recovery with a live DOS descendant

Verify-BrokerFinalLifecycle.ps1 adds -MiddleLayerInputProbe, requiring
-MiddleLayerLoss -NestedWorkerLoss -NestedInteractive. Unlike the earlier
middle-pair test, it does not kill the surviving worker before releasing input.
It verifies the real outer-CMD recovery marker and exit 23, retains handles
to inner run16/worker until root completion, and requires those unfinished
descendants to terminate with nonzero results before any test cleanup.
Then a new MEM must produce its actual memory report.

Final candidate evidence m0-t423-s2-middle-live-final-target and
m0-t423-s2-middle-live-final-launcher both pass. Killing the middle target
returns -1 through its paired launcher; killing the middle launcher gives
-1 and Job-cleaned target 0. Initially root/outer CMD and inner run16/worker
all survive without recursive cleanup. Recovery input is sent while both
native and DOS layers are alive. Outer CMD prints NESTED-RECOVERED and exits
23; root frontend completion then contains inner launcher and worker, both
returning 1067. Their exits are checked through retained handles, not inferred
from the observer's Console attachment count or manufactured in finally.
Subsequent MEM passes. Earlier clean-target/clean-launcher runs also passed
the live-input portion. The S1 baseline run passed its narrower live-input
check, but did not include the final descendant-exit assertion.

One intervening repetition is excluded: middle-live-target-repeat ended
with an unexpected native CTRL_CLOSE_EVENT and an incomplete observer report,
not evidence of input starvation. The lifecycle harness restored absent
environment values through an ordinary PowerShell null-to-string call, which
created empty environment variables on this runtime. Win32 size queries see
an empty variable as present, accidentally enabling the newly added close
switch on the following invocation. A separate local environment probe
demonstrated ordinary restoration leaves the variable present, whereas
[NullString]::Value removes it. The harness now preserves absence explicitly
for its trace/gate/close/control-record settings, and the same-process clean
test checks their absence between cases. FinishObserved also rejects a
nonzero observer exit before interpreting a partial product report.

The final two runs pass with that correction and restore all six S1 package
hashes. This verifies the selected middle-native-pair/live-DOS recovery path;
it does not claim arbitrary simultaneous independent Console applications
are exclusively scheduled, or supply S3 hidden-Console routing. No product
input arbitration, recursive termination or guest policy was added.

### Win32 / DOS / Win32 inner-pair fault recovery

The lifecycle harness now supplies `-DosNativeLoss -NativeRoot`, optionally
`-LauncherLoss`. It reuses the existing authored DOS batch, production binaries
and private-desktop observer. The real chain is root run16 -> interactive
native CMD -> run16 -> DOS COMMAND /c -> run16 -> interactive native CMD.
The test retains native handles to the exact inner pair, DOS launcher and
outer CMD; it never selects an unrelated process merely by executable name.

Evidence prefixes under O:/winnt/logs are
`m0-t423-s2-native-dos-native-target` and
`m0-t423-s2-native-dos-native-launcher`. Both pass. Inner target termination
propagates -1 to its launcher; launcher termination instead invokes direct
Job cleanup (target 0, launcher -1). Original DOS observes the low byte 255,
prints its recovery marker, executes MEM with actual memory output, and
completes the batch normally. Its launcher returns 0 before recovery input is
released to the surviving outer CMD. CMD executes OUTER-NATIVE-RECOVERED and
exits 23; root run16 returns 23. These exits and output are checked before
test cleanup. Both runs restore and hash-check the six-file S1 runtime set.

This closes the selected inner native pair faults in the native/DOS/native
topology. It supplements the separate middle-pair/live-DOS and worker/broker/
frontend fault tests, rather than claiming every layer from this one case.
No new production scheduler, cancellation policy or guest change was needed.

After extending the fixture's finally block to release retained process handles
even when topology validation fails, the final source was rerun with prefixes
`m0-t423-s2-native-dos-native-final-target` and
`m0-t423-s2-native-dos-native-final-launcher`. Both pass the same assertions
and restore all six S1 hashes; these are the final fixture-source runs.

The preceding incremental x86 /MT build recompiles the changed Console binding
and relinks ntvdm and its dependent DLLs; unchanged run16/BaseSrv/DTMGR caches
are reused. An initial invocation used incorrect Ninja target case
VDMREDIR.DLL and stopped before compilation; the corrected graph targets
VDMREDIR.dll and wow32.dll build successfully. The refreshed
`m0-t423-s2-delivery-reconcile-dos17-summary.json` records all 17 actual-output
routes passing. Tested six-file SHA-256 identities are:

| Binary | SHA-256 |
| --- | --- |
| run16.exe | FED646191882ACF5F96D3E7512BA7EDFFB7D980D5E132EC54FC25C30CCBF6D58 |
| basesrv.exe | 0724DE60737E2F071414500BCB01A75CD0A0AD4B3B1CB9DA9D2151806C058CFC |
| ntvdm.exe | 08F5B07787DFFF8ABDD25C6409968CB6BBF8FBD72AF514D46BED1CC92EF86987 |
| dtmgr.exe | 7349D5C89384A00080D31ECD3D1B1AF9D543E72A58F125DFC37328FF9F02487C |
| WOW32.DLL | 26D745E67B7DBBB1FA8BD4B7FC158FA70C93EFE2C4C22A94E5A2B64CF34E5FC9 |
| VDMREDIR.DLL | DCBC92D789CF9E5656C9E88BF7EF24232097620F9BF5AAEC2100C21FD85A59F7 |

Status chronology is compacted into a current verified-scope/remaining-gates
summary; detailed preceding successes, failures and limitations remain here.
No S2 production P, permanent publication, or whole-packet closure is claimed.

The same rebuilt set's independent headless observations use prefix
`m0-t423-s2-delivery-reconcile-wow`. WINMINE, SOL and WRITE each retain a live
worker through the 16-second sample and the known Cannot find NETWORK.DRV
modal. The reports end at the deliberate observation timeout; they are not
normal guest exits or gameplay passes. This matches the retained bounded
baseline under the T423-only owner waiver, without changing SYSTEM.INI or
dismissing dialogs. All six S1 runtime files were restored after observation.

### First S2 production delivery, bounded scope

This delivery commits the authenticated root association, direct Console
transport, original-command rebinding, direct launcher/target lifetime binding,
copied original graphics backing/frame transport and final output coordination.
BaseSrv protocol is 5; Console protocol is 9; APP_VERSION remains 0.0.423.
S2 is not closed. Hidden Console, display/Window and Window mouse remain future
stages. Layout equivalence and physical pointer/focus cleanup remain open.

Final review covers endpoint identity/capability restriction, transfer size,
version/sequence checks, cancellation before buffer release, thread joining,
channel retirement, native pair completion, original command receipt/worker
selection and graphics mutex/copy lifetime. Neither mirror gains an invented
file. nt_event.c changes by normalized +12/-6 against the pre-delivery HEAD;
raw line-ending accounting is recorded above. No opennt-host source changes.
Native GUI/WOW retain their preexisting window route.

Final native fixtures pass: basesrv-reservation-test, console-bitmap-test,
console-video-test, console-pointer-contract-test (mock USER setters),
frontend-scope-lifetime-test, console-frontend-test and console-client-test.
The last two use the private desktop; report prefixes are
m0-t423-s2-delivery-console-frontend-test and
m0-t423-s2-delivery-console-client-test. Incremental fixture build reports no
work after checking the current formal dependency graph.

Final real-run prefixes additionally include m0-t423-s2-delivery-keymouse,
m0-t423-s2-delivery-nesting (four routes) and m0-t423-s2-delivery-video
(uninstrumented graphics-to-text exit). All pass their stated assertions.
Fourteen video source guards pass separately, not as Window rendering proof.
The final DOS17, headless WOW and nested fault evidence is recorded above.

Publication copied all six exact hashes in the preceding table to O:/winnt
and verified each destination. Generated manifest:
build/M0-T423/S2/first-production-publication.json. S1 stays recoverable at
build/M0-T423/S2/baseline-db4ab61eb2db41b296d49d3ce6b8de6d. No guest or
configuration change accompanied publication. Status now names the S2 side-test
package. The commit containing this record is the bounded production delivery;
S2/T423 stay open for remaining requirements and eventual owner audit.

The staged check then found an extra blank line at console_grid.c EOF. Only
that blank line was removed; the affected formal targets and fixtures were
rebuilt. Comparing PE .text sections against the pre-format package proves
unchanged executable code in run16, ntvdm, WOW32 and VDMREDIR. The compiler's
existing C4701 warning at the resize helper is retained: `after` is read only
inside the success branch following GetConsoleScreenBufferInfo, not on its
failure path; no warning-only semantic change was introduced. Refreshed
console-frontend/client native fixtures pass under m0-t423-s2-final prefixes.
Final linked identities, superseding the pre-format table for publication:

| Binary | SHA-256 |
| --- | --- |
| run16.exe | A3141E96E5ACED384D3E2F1EF9FEC4A1A02128971ED4410FA4E1793E185F2483 |
| basesrv.exe | 0724DE60737E2F071414500BCB01A75CD0A0AD4B3B1CB9DA9D2151806C058CFC |
| ntvdm.exe | 014F6C41C69E3199F4317E6F4854E8D13FAD7A9C0E15913207183840750F86FE |
| dtmgr.exe | 7349D5C89384A00080D31ECD3D1B1AF9D543E72A58F125DFC37328FF9F02487C |
| WOW32.DLL | 871BE105DCC55E6078EBA67734D873AE09BF8C4C076632F23334F5CB26DF3658 |
| VDMREDIR.DLL | C86A218787EE6B9682085FB467B28F663C19F59B8A0DEF3529CDEE9355877C75 |

All final-format runtime gates pass under m0-t423-s2-final-dos17 (17 cases),
final-nesting (four routes), final-keymouse and final-wow prefixes. The last
is only the approved independent 16-second live-worker/known-modal comparison.
Final six-file publication verifies the above hashes at O:/winnt; manifest
build/M0-T423/S2/final-production-publication.json supersedes the first
publication manifest without overwriting it. The prior tested S2 set is also
retained under build/M0-T423/S2/pre-final-format-package. Governance and
staged-diff checks pass; no binary/test artifact is included in the Git diff.

### Graphics output mouse-counter contract repair

Source boundary: OpenNT windows/core/ntcon/server/private.c
SrvShowConsoleCursor explicitly accepts CONSOLE_OUTPUT_HANDLE or
CONSOLE_GRAPHICS_OUTPUT_HANDLE, not input handles. Its counter is on the
screen buffer. Original nt_mouse.c MouseDisplay loops until the returned
count is nonnegative. The existing local wrapper instead validated with
GetConsoleMode: that accepts input handles and rejects S2's local graphics
backing. It therefore returned -1 indefinitely for a valid graphics surface.
This is a binding defect, not an original guest or CCPU defect. The loop risk
is source-proven; no claim is made that a real DOS program hung on that path.

The existing body was first relocated without behavioral changes from
console_compat.c into console_client.c so the production client fixture could
call the actual exported binding. A bounded assertion (no unbounded loop)
using production CreateConsoleScreenBuffer fails at ShowConsoleCursor(TRUE):
m0-t423-s2-graphics-cursor-red.txt exits 1, console line FAIL 582 error=6.
The fixture's surviving readiness peer was explicitly cleaned up.

The same-shaped function now accepts its session's live graphics backing and
updates that backing's separate counter under its existing lock. Retirement
resets the count. Native text-output validation uses screen-buffer info rather
than GetConsoleMode, rejecting CONIN$ without altering either counter. The
single-text-surface count remains unchanged. This is the already registered
ADAPTER-WIN32-050 facade: private ntcon/CSR cannot be composed; original caller,
counter ordering and failure remain, no new mirror hook or guest mutation.
Physical pointer ownership stays with conhost/Terminal and is not certified.

Three private-desktop runs m0-t423-s2-graphics-cursor-green-1/2/3 pass graphics
hide/show balance, independent text count, input rejection without count
mutation, repeated negative counts and closed-surface rejection. No native
cursor movement, clipping, hiding or user-desktop switch occurs. Formal x86
ntvdm and dependent DLL rebuild succeeds. Production regression remains a
separate gate, not replaced by these focused checks.

The final fixture additionally closes a graphics surface with a negative
count, recreates it and proves a fresh zero-based count. This passes under
m0-t423-s2-graphics-cursor-reset. The formal ntvdm map selects
softpc-win32-bindings:console_client.obj for ShowConsoleCursor and
console_graphics.obj for its graphics counter; no test substitute is linked.
The old host-input fixture generator omitted console_graphics/console_bitmap
after adding console_client. Its graph now links those production objects;
the rebuilt no-frontend contract passes on the private desktop under
m0-t423-s2-cursor-host-input, including text-caret visibility preservation.

Production gates pass with m0-t423-s2-cursor-dos17 (17 cases), cursor-nesting
(four routes), cursor-keymouse, cursor-video (copied pixels/palette and text
retirement), cursor-video-standard (uninstrumented final text), and cursor-wow
(three independent approved headless live-worker/known-modal comparisons).
No physical mouse visibility or gameplay acceptance is inferred.

The coherent six-file publication uses
build/M0-T423/S2/cursor-production-publication.json; every destination hash
matches its tested source. The preceding production package remains under
build/M0-T423/S2/cursor-pre-fix-package. Unchanged run16, basesrv and dtmgr
hashes match the previous table. Updated hashes:

- ntvdm.exe: C1255C30870FE9BD319FED8DBFD2744972221B5F9098825816CBD01E0C94100B
- WOW32.DLL: B0D5F1C8791F3683FD06B5118DD2203E1526900F2AD7FC3104508D947A92E74D
- VDMREDIR.DLL: 51DFFA2B1CDB60C578B610F7F6C4D439C8490A52B990C7F83D89E0E4EE7A38F8

This is a bounded S2 repair delivery; no mirror, guest, protocol version or
configuration changes, and no S2/T closure. Remaining owner/failure gates stay
explicit in Status. Governance and diff checks pass before commit.

### Root native target loss with a live nested DOS task

Question: when the root launcher's direct native target dies while a nested
DOS task is still active, does the root return that target's result, contain
the disconnected frontend chain, and leave the broker usable?

The checked-in Verify-BrokerFinalLifecycle.ps1 now exposes RootTargetLoss,
accepted only with NestedWorkerLoss and no other fault scenario. Invalid
combinations fail before resolving paths or opening process resources; tested
missing NestedWorkerLoss, conflicting DosNativeLoss, and ConsoleClose.
The positive command is:

```powershell
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
& tools/audit/Verify-BrokerFinalLifecycle.ps1 `
  -Observer build/M0-T423/S2/control-observer/console-startup-observer.exe `
  -PackageRoot O:/winnt -LogPrefix m0-t423-s2-root-native-target-loss-r2 `
  -NestedWorkerLoss -RootTargetLoss `
  -WorkerWindowObserver build/M0-T423/S2/worker-window-snapshot.exe
```

Both m0-t423-s2-root-native-target-loss and its -r2 repetition pass on the
unchanged 8e60ded8a production package, x86 /MT CCPU40. The unswitched private
desktop isolates the test; only its identified root CMD is explicitly killed.
Retained process handles prove termination before the harness's finally cleanup:

| Layer | Observed exit |
| --- | --- |
| Root CMD (injected victim) and root run16 | Both -1 / FFFFFFFF |
| Middle run16 and its CMD | Both 1067 |
| Inner run16 and DOS worker | Both 1067 |

The same broker remains alive. A subsequent fresh MEM request exits 0 and its
captured Console contains the actual available-XMS-memory output. Results,
process-chain identity, observer output and captured text are retained under
O:/winnt/logs with those prefixes. This verifies root-target loss, not a new
recursive kill policy or independent-worker isolation (covered separately).
No production, guest, configuration or publication inputs change in this P;
the existing six-file cursor-production-publication manifest remains selected.

### S2 exit-checklist reconciliation

The proposal's graphics-contract row is now checked: production sender and
receiver tests plus cursor-video prove exact indexed pixels/palette and
graphics-to-text retirement; cursor-video-standard proves the final text with
the standard frontend. Window rendering remains S4, not an inferred pass.

Root-target failure is now evidenced alongside the existing middle-pair and
inner-pair failures. The combined fault-matrix checkbox stays open until its
entire root/middle/inner and normal-completion-race matrix is reconciled.
Keyboard-layout source ownership, physical focus/pointer release, and the
remaining per-call Console-owner assertions likewise remain explicit S2 gates.
The display/CAF/AE/X implementation and tests remain assigned to S4 as originally
planned; their absence in S2 is not a hotkey pass. This record does not close S2.

### Focus-record transition through the real guest input path

Source: original nt_event.c dispatches FOCUS_EVENT to nt_process_focus,
which calls MouseOutOfFocus on loss and AltUpDownUp/MouseInFocus on gain.
It does not specify that losing focus synthesizes every held key's release.
Therefore the test sends a real Ctrl key-up record between focus loss and
gain, rather than inventing a new guest release policy.

The existing checked-in keymouse_capability_observer.c gains one opt-in test
mode, MVDM_TEST_KEYMOUSE_FOCUS_TRANSITION=1. With Ctrl still held after the
guest's S25_DISABLE_READY handshake, it injects FOCUS_EVENT(FALSE), Ctrl-up,
FOCUS_EVENT(TRUE), then the existing late mouse sequence. It reads actual
guest markers proving BIOS Ctrl release, all prior move/down/up callbacks,
and no callbacks after driver disable. It then requires the current COMMAND
prompt, actual MEM output, the next prompt and original COMMAND exit 1.

Build with tools/build/Build-T420S25KeymouseGuestTest.ps1, RepositoryRoot set
to this repository and BuildRoot to build/M0-T423/S2/keymouse-focus-transition.
MSVC x86 /MT compilation passes. The independent test-only KMTST.COM remains
byte-identical (92C642D5F92EC4EA8C255D956977B5075C068B26423287CA4BDB66F81A8F6629);
the rebuilt observer hash is
E6F13CAC17DACD8B98E79B63842821302BCEA082E2D1748D3C12B54908E848E5.
No original guest binary is modified.

Run through the private-desktop console-startup-observer.exe from
build/M0-T423/S2/control-observer with a 60000-ms timeout, target the rebuilt
keymouse-capability-observer.exe and pass its guest-log path as its only
argument. Set TEST_RUNTIME_ROOT=O:/winnt,
MVDM_TEST_KEYMOUSE_SHARED_CONSOLE=1 and
MVDM_TEST_KEYMOUSE_COMMAND=O:/winnt/tests/KMTST.COM. Copy only the independent
probe into the existing tests directory. The focus switch is absent (not an
empty environment value) for the baseline comparison.

All three private-desktop runs pass against the unchanged 8e60ded8a package:

| O:/winnt/logs prefix | Mode and result |
| --- | --- |
| m0-t423-s2-keymouse-focus | Injected focus transition, observer 0, guest markers pass, COMMAND 1 |
| m0-t423-s2-keymouse-focus-baseline | Switch absent, same complete keymouse/MEM/exit assertions pass |
| m0-t423-s2-keymouse-focus-repeat | Repeated transition, same assertions pass |

This is real guest processing of injected Console records through the
production frontend and original worker consumers, not physical focus movement.
No SetForegroundWindow, desktop switch or pointer clipping was introduced.
The probe does not select the original hidden-pointer/int33-motion path;
physical clip release and focus ownership remain separate unproved items.
Product hashes and configuration remain unchanged. This test-only delivery
does not close the combined focus/pointer checklist row or S2.

### Console layout contract candidate and retained typeahead failure

Original ownership is explicit in OpenNT windows/core/ntcon/server/getset.c:
SrvGetConsoleKeyboardLayoutName activates Console->hklActive before obtaining
the name. Its private Console/CSR state is not available for direct composition.
The native exact-name export is the smallest existing host binding; inventing
an HKL from the worker/frontend thread or a pseudoconsole HWND is not equivalent.
The candidate therefore removes the proven-wrong thread-local success fallback
and routes the original API through the frontend using direct protocol 10.
Only a successful, terminated nine-byte KLID is copied to the caller. Native
failures are preserved, missing exports fail explicitly, and without a bound
frontend the exact native API is queried locally. No new keyboard algorithm,
locale cache, original mirror diff, registry setting or guest change is added.

This does not restore native layout availability on this Windows host. Instead
it restores the original caller's failure decision: cmdkeyb.c immediately takes
NoInstallkb16, sets DX=0 and calls cmdInitConsole. The checked-in original-reader
fixture now proves that outcome without later code-page/directory queries or
guest-output writes. Its configured/malformed/absent cases still pass. Two
existing environment boundaries in that fixture needed same-shaped native
test bindings because the old fixture no longer linked the current cmdkeyb
object; they are explicitly not package-directory composition evidence.

MSVC x86 /MT formal six-file build passes. The first native fixture build failed
for a missing private API declaration, fixed in the client header. The first
original-reader link exposed the two old fixture dependencies, then passed
after the test-only bindings. Neither failed attempt counts as a pass.
The final native client, frontend and mock host dispatcher fixtures pass under
m0-t423-s2-layout-candidate-* prefixes. They prove actual native error propagation,
null rejection without a request, output canary preservation, and mocked success
and error payloads. Mock success does not claim native availability.

The preceding six-file package was hash-verified and preserved at
build/M0-T423/S2/layout-pre-fix-package before candidate staging. The complete
DOS17 run m0-t423-s2-layout-dos17 passes and restores that published baseline.
The nesting run passes native-root-frontend, native-cmd-dos-repeat and
dos-native-dos, but dos-native-typeahead FAILS: both MEM outputs are present,
the final intended exit is displayed as eit, and observation ends with timeout
53504354. This is an unresolved lost-key result, not successful completion.
The observer's native child is terminal and the harness restored the baseline
before any further run. Identical isolated baseline and candidate repetitions
both pass (layout-typeahead-baseline and layout-typeahead-candidate-r2).
Those passes do not erase the failure or prove its cause; publication remains
withheld pending attribution and resolution. The unchanged 100-ms key interval
and zero line delay are retained, not relaxed to hide the problem.

Tests here use build/M0-T423/S2/test-frontend-loss.ps1 as temporary staging glue
around the checked-in Verify-CommandExitStatus.ps1 and native fixtures. Its
backup now points to layout-pre-fix-package, not the older cursor repair backup.
The formal cache has a protocol-10 candidate; O:/winnt is restored to the
protocol-9 published set after each test. Never copy the candidate cache as
a release until all gates pass. S2 and the overall Window goal remain open.

Follow-up: layout-keymouse passes real guest callbacks, modifier release, MEM
and COMMAND return. layout-wow independently observes all three workers live
at 16 seconds at the known NETWORK.DRV modal, matching the approved headless
baseline, not gameplay. Four additional unchanged-baseline runs
layout-typeahead-control-1 through -4 and four candidate runs
layout-typeahead-candidate-control-1 through -4 all pass with unchanged input
timing. This bounds reproducibility, not causality or resolution of the failure.

The test-only console_video_observed.c receiver now also records copied READ
and PREPEND input batches (PID, generation, sequence, status and key fields).
Its product dispatcher is unchanged, and this object never enters run16.exe's
formal product link. layout-typeahead-input-diagnostic passes with the observed
launcher only; it is diagnostic, not product acceptance. The corresponding
layout-typeahead-input-boundary.txt shows final e/x returned as one original
PREPEND batch, then read back as down/up records (control=0), while later i/t
arrive with NUMLOCK_ON (control=32). All are visible in that successful run.
This identifies the original returned-key/toggle-state transition as a concrete
next inspection point; it does not establish that it lost x in the earlier run.
No pacing change, resend or original keyboard modification was made.

Every staging run has terminated and restored the hash-verified protocol-9
six-file baseline. Candidate source/build/test changes remain uncommitted while
the failed gate is investigated; do not represent this work-in-progress as a P.

### Original returned-key count repair candidate (DIV-312)

Source inspection found a deterministic defect in the selected original
nt_event.c::ReturnUnusedKeyEvents, also present in the pinned OpenNT original:
the loop copies N history records but passes N+1 to WriteConsoleInputVDMW.
The extra uninitialized input record can make the checked transport reject
the entire batch before the original function clears its history. A short
history also occupies the array tail, not the submitted prefix. This is host
source, not an immutable guest limitation.

The original translation unit remains compiled. The minimal registered
mirror repair bounds the copy by its existing array capacity, assigns the
record type at the copied index, and submits only the initialized tail in
oldest-first order. No alternative queue, retry policy, guest change or input
pacing change is introduced. Direct unchanged reuse cannot preserve valid
record/count semantics because the defect is in this owner; a facade cannot
infer the missing initialized count from the already incorrect request.
The selected rung is a registered local correction to the retained original
owner, not an independent reimplementation.

Reproducer: tests/observation/verify-unused-key-return.ps1 extracts the exact
production function and compiles tests/app/unused_key_return_test.c with MSVC
x86 /MT. Its external history/write/cleanup functions are controlled fixtures,
not a claim that the whole Console implementation is tested. The red run at
build/M0-T423/S2/key-return-red fails requested=2 present=2 sent=3. The green
run covers full, single, empty, short, absent, capacity and over-capacity history
with exact count, type, order and once-only cleanup assertions (seven cases).
The final CRLF source is byte-hashed in key-return-green-final-authorized:
E1D8B63D2FE0B9AC18E2E7324FC815956DDE16FD15A3B45E124FFA545F5A108C;
extracted body B212F925157448BB29794F0C20CDF6C707E139BE76C33DC36D389CA8849AEA43.
An earlier final-source fixture attempt could not resolve cl.exe in the
sandbox; the authorized installed-toolchain run passed, not the failed attempt.

Incremental formal and WOW builds pass (key-return-formal-build.log and
key-return-wow-build.log under the S2 build root). Real production-candidate
native-root-frontend, native-cmd-dos-repeat, dos-native-dos and
dos-native-typeahead all pass under m0-t423-s2-key-return-nesting, retaining
100-ms key spacing and zero line delay. The six-file published baseline was
restored and hash-checked afterward. This proves the concrete count defect
and passing affected integration routes; it does not prove that this was the
sole cause of the earlier intermittent missing x. Full candidate delivery
gates and the broader S2 checklist remain open.

#### Returned-key repair delivery gates and publication

The subsequent complete DOS17 run m0-t423-s2-key-return-dos17 passes all
17 actual-output/exit cases, including EDIT then MEM. key-return-keymouse
passes the real guest keyboard/mouse callback and MEM return assertions.
key-return-graphics proves copied guest pixels/palette and later text;
key-return-graphics-formal proves uninstrumented product graphics-to-text
return, final marker and exit. Neither claims Window rendering. Two additional
unchanged-timing runs key-return-typeahead-repeat-1 and -2 pass. All these
prefixes begin with m0-t423-s2- and their raw evidence is under O:/winnt/logs.

key-return-wow independently observes WINMINE, SOL and WRITE alive at
4/8/12/16 seconds at their known NETWORK.DRV modal. Observer timeout is
intentional sampling termination, not application success; no crash or earlier
frontier was seen versus the retained headless baseline. This meets the owner's
bounded T423 headless comparison waiver, not general WOW functionality.

The package also contains the already-tested layout source-contract repair;
its unchanged native/client/mock/original-reader fixture evidence is recorded
above. The final host-source red/green fixture and the new full product runs
cover the additional key-return change. The proved initialized-count correction
is delivered without claiming that repeat greens establish the earlier lost
x's unique cause. The original failed observation remains part of this record.

After the final test restored the prior package, publication verified all six
prior hashes, PE x86 machine fields, and all six deployed candidate hashes.
The recoverable prior set remains build/M0-T423/S2/layout-pre-fix-package.
Manifest: build/M0-T423/S2/key-return-production-publication.json.

| Published file | SHA-256 |
| --- | --- |
| run16.exe | 30CAB9861213103781F4FA7AFA796E1F724758F86061898C4C3B1BF532CB40C9 |
| basesrv.exe | 0724DE60737E2F071414500BCB01A75CD0A0AD4B3B1CB9DA9D2151806C058CFC |
| ntvdm.exe | 34413C6AD4AC8085993A63C82D77E136BAE75A33A570F69D73C432D01AEF8F82 |
| dtmgr.exe | 7349D5C89384A00080D31ECD3D1B1AF9D543E72A58F125DFC37328FF9F02487C |
| WOW32.DLL | 0D04702943D96E69D6CE84CAB7309418079FB9539BFA21902056787CD706076E |
| VDMREDIR.DLL | 0FECA788E63DAFB44E9EEAB70E08BC86D10A90E188DDD93CE60AB91ADA6169E7 |

Do not reuse the old temporary staging helper without refreshing its baseline:
its hash preflight now correctly rejects this newly published set. S2 still
requires its remaining owner/lifetime checklist reconciliation; S3-S6 and the
full Window goal are not completed by this bounded delivery.

### Production channel cancellation and acknowledged-EOF lifetime proof

Question: can root-frontend channel shutdown join both pending pipe I/O and a
native Console read, including peer EOF concurrent with owner cancellation,
without leaked channel handles or leaving the input waiter asleep?

tests/app/console_channel_lifetime_test.c compiles the production channel,
dispatcher, video backing and grid provider. Only the BaseSrv attach call is
substituted to retain the peer pipe and assign the test generation; this does
not prove authentication, broker task completion or real guest behavior.
The test uses real named pipes, OVERLAPPED transfers, Console handles and
threads. A wrapper signals entry to the READ_INPUT dispatch and then executes
the unchanged production dispatcher. No key is injected to unblock that read.

Each case first receives and validates a normal barrier acknowledgment. It
then exercises (1) cancellation during idle pipe receive, (2) cancellation of
an empty native Console read, or (3) peer EOF racing owner stop after that
acknowledgment. Every stop must join within five seconds, the retained thread
handle must be signalled with a non-success terminal result, and the retained
input-ready event must be signalled so its waiter can observe EOF. Sixteen
rounds after one warmup round cover 51 channel lifetimes with fresh generations;
the process handle count must match before and after the measured rounds.
This does not claim that arbitrary scheduling interleavings are exhausted.

Build: tests/observation/build-console-channel-lifetime.ps1 -BuildRoot
build/M0-T423/S2/channel-lifetime-r2, MSVC x86 /MT. The first link attempt
omitted the existing grid implementation and failed; the reproducible builder
now compiles that actual provider too, not a stub. Existing production compiler
warnings remain visible; no production edits were made for this fixture.
Executable SHA-256: D8949559CB31F1070333451196769A235AB5AF541B8E198D607EE4C0AD59045F.

Run via build/M0-T423/S2/control-observer/console-startup-observer.exe with
MVDM_OBSERVER_PRIVATE_DESKTOP=1, fixture test.exe as target, O:/winnt as working
directory, and --observation-timeout-ms 20000. The reports and captured text
under O:/winnt/logs/m0-t423-s2-channel-lifetime.txt and the -repeat variant
both show fixture exit 0 and the full 51-case pass message. No user desktop
switch, package replacement or guest modification occurred. The current
six-file publication remains 94762fa70's tested set.

The source-only verify-dos-frontend-handoff-source.ps1 guard also passes all
six original suspend/drain/re-entry/resume ordering checks. It is not runtime
proof. These results close the channel cancellation/join subcase, not the
entire broker/guest completion-race matrix or the physical pointer/focus gate.

### Current compiled Console owner reconciliation

At production revision 94762fa70, reran
tests/observation/audit-dos-console-build-surface.ps1 against
build/M0-T423/S1/restart-formal-x86/build.ninja with the installed MSVC x86
dumpbin. The graph selects 411 distinct worker C units; the lexical inventory
has 248 occurrences in 17 files and 88 spellings, including definitions and
inactive branches. These counts are not runtime call counts.

The new -VerifyFrontendBoundary switch checks every selected mirror object
for direct imported Console/pointer APIs. It requires Dumpbin and at least one
mirror object, fails on a missing object/tool failure, and permits only the
original nt_event.c process-local SetConsoleCtrlHandler registration. Current
run passes across 366 compiled mirror-object entries; raw output is
build/M0-T423/S2/compiled-owner-audit.txt. The check is intentionally not a
proof of generic ReadFile/WriteFile routing or adapter fallback reachability.

Below, original paths are under src/mvdm; client/compat/graphics denote the
corresponding src/ntvdm-exe/win32 files. The receiver is
src/run16-exe/console_frontend.c. The listed tests are checked-in under
tests/app unless otherwise qualified. This reconciles the selected Console
surface without claiming the still-open physical/lifecycle acceptance rows.

| Original caller / operation | Current owner and wire route | Assertion evidence / disposition |
| --- | --- | --- |
| nt_event: ReadConsoleInputExW, GetConsoleInputWaitHandle | compat peek/read -> client READ_INPUT/PEEK_INPUT; root owns native input and readiness | console_client_test actual queue/peek/read; keymouse guest and focus-record runs; channel lifetime cancellation |
| nt_event/cmdmisc: WriteConsoleInputVDMW | compat -> client PREPEND_KEYS -> native prepend in root | returned-key exact-body red/green, native prepend/client fixtures, real nested typeahead |
| nt_hosts/nt_event/nt_fulsc: Get/SetConsoleMode | client GET_MODE/SET_MODE -> root | client/native comparisons, real DOS/native return, source handoff ordering guard |
| nt_graph: WriteConsoleA, fill-character/attribute, scroll | client WRITE/FILL_CHARACTER/FILL_ATTRIBUTE/SCROLL -> root buffer | console_frontend_test exact characters/attributes/scroll and DOS17 actual output |
| nt_graph/nt_hosts/nt_event/nt_fulsc: screen and cursor queries/updates | client SCREEN_INFO/CURSOR_POSITION/CURSOR_INFO/GET_CURSOR_INFO | console_client_test and console_frontend_test native state comparison, EDIT then MEM |
| nt_fulsc: ReadConsoleOutputA; text invalidation: WriteConsoleOutputA | client READ_CELLS_A/WRITE_CELLS_A; generic A/W bindings retain copied cell layout | console_client_test rectangular/tiled reads and writes, console_frontend_test; real text return |
| nt_graph/nt_fulsc: buffer/window dimensions | client BUFFER_SIZE/WINDOW_RECT -> root shared grid provider | native/client resize and failure checks; no new worker presentation algorithm |
| nt_graph: GetCurrentConsoleFont/GetConsoleFontSize | client font queries -> root | client fixture compares native dimensions and errors; not a font rendering implementation |
| cmdkeyb/nt_event: Console input/output code page | client code-page query -> root | client native comparison; original keyboard reader cases |
| cmdkeyb: GetConsoleKeyboardLayoutNameA | client KEYBOARD_LAYOUT -> exact root Console query; source owns NoInstallkb16 | layout native/error/mock and original-reader tests; host layout availability is not claimed |
| config/cmdmisc/cmdpif: Get/SetConsoleTitle | client title query/set -> root | client exact capacity, empty, error and no-channel comparisons |
| nt_event: SetConsoleDisplayMode | client SET_DISPLAY_MODE -> root public API | display-owner fixture/native results; not hardware fullscreen or product display flag |
| nt_mouse/nt_event: VDMConsoleOperation | compat -> client WINDOW_QUERY -> root window | client rectangle/iconic/coordinate assertions; unavailable native operation remains failure |
| nt_mouse pointer query/warp/clip | conapi aliases -> client GET/SET_POINTER and GET/SET_POINTER_CLIP -> root USER API | pointer dispatcher signed-coordinate/null-release/error fixture; physical clip/focus cleanup still pending |
| nt_cga/nt_ega/nt_vga: InvalidateConsoleDIBits; nt_graph: palette and graphics buffer | compat/graphics keep painter backing/mutex local; copied VIDEO_BEGIN/DATA/TEXT to root | exact guest pixels/palette and text-return tests; Window renderer remains S4 |
| nt_det/nt_fulsc: RegisterConsoleVDM | worker-owned bounded text backing; no native hardware state mapping | retained registration/bitmap/client tests and guest text output; pointer cannot cross process |
| nt_mouse/nt_event: ShowConsoleCursor | local source-visible display count per text/graphics backing | cursor count red/green/recreation; does not pretend to change modern native pointer visibility |
| config: SetConsoleKeyShortcuts; nt_mouse: ConsoleMenuControl | explicit ERROR_CALL_NOT_IMPLEMENTED at compat | private NT4 menu/reservation unavailable; no fabricated menu/global hook; S4 product hotkeys are separate |
| nt_event: SetConsoleCtrlHandler | worker-local original guest control handler | real Ctrl+C/Break and Console close tests; callback address is not an IPC payload |

The compiled nt_fulsc/config/nt_det paths do not import the lexical
Get/SetConsoleHardwareState candidates. Those MONITOR-era branches are not
claimed as CCPU40 capabilities. nt_event's GetKeyboardLayout queries belong
to key normalization, not a Console reader or renderer; they are distinct from
the repaired Console-layout-name contract. The latter's tests must not be
used to claim every keyboard layout is supported.

Native Console imports remain in client/graphics for their explicit no-channel
or non-owned-handle routes. Inspected MvdmWriteConsoleA never falls back after
an admitted channel fails. nt_hosts still obtains/opens local CONIN$/CONOUT$
handles for original handle classification and source-facing state; selected
presentation APIs using those handles are bound as above. The inspected
nt_graph, nt_event, nt_hosts, cmdexec and cmdmisc contain no direct ReadFile or
WriteFile calls. This bounded observation does not exempt DEM/redirected file
I/O from its original file/pipe ownership or certify all source files globally.

Remaining closure work is now explicit: reconcile the complete per-layer
broker/guest normal-completion versus failure matrix and physical pointer/focus
disposition. S4 owns display/CAF/AE/X and Window focus; no S2 hotkey pass is
inferred. No production source or deployed artifact changed in this audit.

### Normal DOS completion precedes launcher death/rundown

Added two modes to tests/adapter-basesrv/base_service_reservation_test.c:
--launcher-completed-rundown and --launcher-completed-uncollected. Both use
the existing original Check/Update/Get path, complete the second command with
exit code 29 through GetNext, and require the original parent event signalled.
The first collects and verifies 29; the second deliberately leaves the result
uncollected. Then the test kills only that fixture-owned launcher process and
disconnects its service connection. Disconnect drains the registered process
watch, so no sleep is used to guess whether cleanup has occurred.

In both cases the existing worker must remain alive, its original GetNext wait
must remain unsignalled, and BaseSrvDOSWorkerWaitPending must still find that
same Console record. Only after these assertions does fixture cleanup end the
worker and require the service to become empty. These are real native process
and compiled original service assertions, with the fixture's existing Console
membership substitute; they do not execute a DOS guest or test transport RPC.

Build: Ninja basesrv-service-reservation-test.exe under the S1 formal x86 /MT
cache. The first collected-only run passed. Adding a printf diagnostic exposed
the fixture's no-inline-stdio link restriction; changing only that diagnostic
to its existing puts facility restored the link. The final executable is
E9501CA33804F32E531DF9DD733E0E1122493AB6D2D0F9C8D3A28F236C812351.

Final private-desktop runs use control-observer/console-startup-observer.exe,
MVDM_OBSERVER_PRIVATE_DESKTOP=1, a 20000-ms limit, the test executable and its
single mode argument. All four reports/captured outputs under O:/winnt/logs
exit 0 and carry their expected assertions:

| Prefix (all start m0-t423-s2-final-) | Required outcome |
| --- | --- |
| launcher-completed-rundown | Collected 29, late launcher exit leaves worker/GetNext wait alive |
| launcher-completed-uncollected | Completion event alone excludes the task from abandoned-pair termination |
| launcher-pair-loss | Existing unfinished task still invokes original worker-exit cleanup |
| launcher-pair-exit-watch | Existing process watch works before RPC rundown and completes the unfinished task |

Together these distinguish completed versus unfinished records and both
collection orderings; they do not claim every simultaneous scheduler
interleaving is exhausted. No production code, original guest, registry or
published six-binary package changed. This closes this service completion-
before-late-exit subcase, not untested topology combinations or S2 as a whole.

### DOS/native/DOS middle-loss failure, with a passing no-fault control

Added -NestedDosChild to Verify-BrokerFinalLifecycle.ps1, requiring
-DosNativeLoss and optionally -LauncherLoss. The authored
tests/observation/dos-native-dos-loss.bat starts CMD /c run16 COMMAND, then
checks the native result, prints recovery markers and runs MEM. No original
guest media is modified. The private observer holds input behind a gate until
the nested COMMAND prompt and its exact launcher process are established.

The fault run ends only the middle native target or its paired launcher. It
requires that pair to finish while the nested DOS launcher remains alive,
then sends a normal exit command to the nested guest (not another kill).
Outer DOS must subsequently execute MEM and finish. -NestedDosChildControl
uses the same batch/observer/input sequence without either injected fault.
This specifically covers a DOS-rooted nested topology, not the earlier
native-root/middle-native/live-DOS combination.

Current published 94762fa70 package results under O:/winnt/logs:

| Prefix (m0-t423-s2-) | Actual result |
| --- | --- |
| dos-native-dos-middle-target | FAIL: middle native/launcher -1/-1, inner DOS launcher remains live until exit input then returns 0; outer root times out, no recovery/MEM markers |
| dos-native-dos-middle-launcher | FAIL: Job-cleaned native 0, killed launcher -1; inner launcher returns 0 after exit input; same outer timeout |
| dos-native-dos-control | PASS: no fault, native and its launcher return 0, inner DOS returns 0, original parent executes MEM/recovery markers and root returns 0 |

Both failed observers are terminal (53504354 timeout), and the harness performs
its test-owned cleanup. No unresolved live job or automatic restart is being
treated as a test result. The published package was not replaced. The control
checks actual guest text; an exited inner launcher alone does not pass the
fault case. These are newly exposed failures, not retroactive passes or a
claim of regression caused by the current test-only changes.

Existing MVDM_S34_TRACE_PATH was enabled for the launcher-fault and control
runs. In the failure, reenter 32 (DECREMENT_REENTER_COUNT) precedes get-state
128 (RETURN_ON_NO_COMMAND), which returns a wait and never completes. In the
control, the initial get-state 128 wait precedes the decrement; then the
second-time get-state 136 returns without waiting and the parent progresses.
Raw logs have matching -s34.txt suffixes.

Source inspection: BaseSrvSetReenterCount signals hWaitForVDMDup on decrement;
BaseSrvGetNextVDMCommand's no-command first request resets that existing event,
whereas RETURN_ON_NO_COMMAND plus ASKING_FOR_SECOND_TIME returns immediately.
Whitespace-normalized comparisons of both reached source regions against
O:/repos.external/OpenNT/base/win32/server/srvvdm.c are equal. Thus an early
decrement followed by event reset is a concrete lost-wakeup candidate, not
proof of an invented adapter policy or a guest defect. Next required evidence:
a deterministic original-service test forcing both orderings before selecting
any repair. Do not patch guest, add a second scheduler or declare this an
approved immutable-guest limitation. S2 remains open for this real failure.

Pointer audit also confirms that nt_mouse.c MouseHide/MovePointerToWindowCentre
and MouseInFocus/MouseOutOfFocus can clip/warp outside MONITOR-only branches.
The physical pointer acceptance item cannot be discarded as dormant hardware
fullscreen. No desktop focus or pointer mutation was performed in this work.

### Deterministic re-entry notification ordering reproducer

The preceding causal candidate is now reproduced without scheduler timing or
guest changes. tests/adapter-basesrv/base_service_reservation_test.c adds
--reenter-before-return and --reenter-after-return. Both consume the same
second DOS command through original Check/Update/Get, increment the native
re-entry count, complete that command with 29 using RETURN_ON_NO_COMMAND,
and assert the parent completion event and collected result. The only changed
ordering is whether DECREMENT_REENTER_COUNT occurs before or after that Get.
The test uses compiled original BaseSrv through its production service adapter;
Console membership is the existing fixture substitute. It is not a guest or
RPC-transport acceptance test.

Build: MSVC Win32/x86 /MT, Ninja basesrv-service-reservation-test.exe in
build/M0-T423/S1/restart-formal-x86. Executable SHA-256:
8E57052F78039486CB873D128A6C396B477EAE844A74F704EE8BA7E639F19467.
Run with the S2 control-observer, MVDM_OBSERVER_PRIVATE_DESKTOP=1,
O:/winnt package root and --observation-timeout-ms 20000, followed by the
mode. Reports and captured Console text are in O:/winnt/logs:

| Prefix | Observed assertion |
| --- | --- |
| m0-t423-s2-reenter-before-return | RED: completed=29, wait=258 (WAIT_TIMEOUT), fixture exits 1 |
| m0-t423-s2-reenter-after-return | GREEN: completed=29, wait=0 (WAIT_OBJECT_0), fixture exits 0 |
| m0-t423-s2-reenter-control-launcher-completed-rundown | PASS: collected normal completion survives late launcher loss |
| m0-t423-s2-reenter-control-launcher-completed-uncollected | PASS: uncollected normal completion survives late launcher loss |
| m0-t423-s2-reenter-control-launcher-pair-loss | PASS: unfinished pair cleanup |
| m0-t423-s2-reenter-control-launcher-pair-exit-watch | PASS: unfinished pair cleanup before RPC rundown |

Both ordering modes explicitly terminate/join their fixture-owned children,
drain connections and require an empty service before checking the wake result.
All observers exited; the red test is not a stranded process or observation
timeout. No physical desktop focus was changed. This is a checked-in known
failing regression test, not a functionality pass or repaired production P.

Interpretation: early native completion followed by first Get clears the only
notification needed for the original client wait/retry. This matches the real
DOS/native/DOS failure trace. The relevant original wait is a NotificationEvent,
not an auto-reset event: simply deleting NtResetEvent would retain stale signals
and is not a justified repair. Also cmdExec32 creates its native worker thread
before that thread increments nReEntrancy, then makes a
NO_PARENT_TO_WAKE|RETURN_ON_NO_COMMAND request. Treating zero re-entry count
alone as immediate completion could race that startup. A repair must account
for these original caller distinctions and the nested-count case; the test does
not authorize a second scheduler, timeout-as-success, or guest alteration.

No production source, guest/configuration or O:/winnt six-binary package changed.
S2 remains open. Next: select the smallest source-owned completion binding,
verify early/late completion plus startup/pending-command/nested controls, then
repeat the real failing guest topology and every production P gate before
publication. The owner-approved T423 headless-only WOW3 acceptance remains in
effect and does not waive DOS/native lifecycle correctness.

### Selected repair boundary: coalesced native-return notification

OPENNT-HOST-064 is admitted within the existing S2 nested-return repair.
Original owner inputs are OpenNT/base/win32/server/srvvdm.c (SHA-256
C1E2177C6C00679D85CFA475F620841F6736B0E56D8DBF790B71AFE33E1ED80B)
and srvvdm.h (F590EF866F87CE80671C4B6F4E3BBE94347342E6B723BC062ED6F044FBB3820F).
The compiled original service is already composed directly: that first recovery
rung produces the deterministic red test. The same-shaped modern event facade
cannot distinguish native-return SetEvent from queued-command SetEvent or a
stale manual-event signal; changing it would alter unrelated event semantics.
Therefore the selected third rung is a minimal existing-owner correction, not
a new adapter policy or fourth-rung replacement scheduler.

Retain a single coalesced native-return pending bit under the original DOS
critical section. Set it with the original decrement notification; consume it
only when Get has no queued command and RETURN_ON_NO_COMMAND permits returning.
Preserve the original second-time exit, queued-command priority, native count,
parent result and event reset. Multiple notifications coalesce just as the
original NotificationEvent does. The bit is Console-record-local, never wire
or guest state, initialized with that record and released with it. There is no
new endpoint, queue, ABI operation, timer, thread or task scheduling decision.
This remains provisional until early/late, one-shot, startup, nested-count,
queued-command and real guest regression gates pass; registration alone is not
proof of a production repair.

### Native-return notification repair delivery

The selected OPENNT-HOST-064 correction passes the original red case and its
controls. Five service modes now assert the no-command reply or original
wait/retry completion, followed by a fresh NO_PARENT_TO_WAKE request that must
wait: a consumed notification cannot incorrectly finish a later shell-out.
The queued-command case additionally decodes and compares the original command
payload before testing its subsequent completion. Nested-count mode retains
one other native activity while consuming the completed activity's notification;
startup mode issues Get before the increment and must not return early.

Final test executable SHA-256 is
46B91E29B5C1C7563B3561D013284977A7E20447934683955E6D6B56E8A46F25.
All nine private-desktop tests exit 0 under O:/winnt/logs prefixes
m0-t423-s2-notification-final- followed by reenter-before-return,
reenter-after-return, reenter-nested-return, reenter-pending-command,
reenter-before-increment, launcher-completed-rundown,
launcher-completed-uncollected, launcher-pair-loss or launcher-pair-exit-watch.
The first five exercise the notification contract; the last four retain
normal-completion versus unfinished-task cleanup. Earlier failed baseline
evidence remains unchanged.

Production verification used the retained MSVC x86 /MT S1 formal and WOW
incremental graphs. Six formal targets link successfully; source/header
dependencies rebuild the affected original server and bindings libraries.
The prior published package is preserved under
build/M0-T423/S2/notification-pre-fix-package, checked against the preceding
key-return publication manifest before each run. The temporary staging helper
build/M0-T423/S2/test-reentry-notification.ps1 restores that complete baseline
after each test phase. Checked-in test entrypoints and actual outcomes:

| Entry / arguments | O:/winnt/logs prefix (m0-t423-s2-notification-) | Result |
| --- | --- | --- |
| Verify-BrokerFinalLifecycle.ps1 -DosNativeLoss -NestedDosChild | dos-native-dos-target | PASS: middle pair dies, inner DOS remains then exits 0, outer DOS prints 255/MEM/recovery and exits 0 |
| Same plus -LauncherLoss | dos-native-dos-launcher | PASS: paired native cleanup, inner DOS exit and outer MEM/recovery, no permanent wait |
| Same plus -NestedDosChildControl, without LauncherLoss | dos-native-dos-control | PASS: normal identical topology, actual MEM/recovery and root 0 |
| Verify-CommandExitStatus.ps1 with G7.COM | dos17 | PASS all 17 actual-output/exit checks, including EDIT return then MEM |
| Same -Cases native-cmd-dos,native-cmd-dos-repeat,dos-native-dos,dos-native-typeahead | nesting | PASS all four, including zero line-delay handoff |
| keymouse-capability-observer.exe with immutable authored KMTST.COM | keymouse-observer / keymouse-guest | PASS real keyboard/modifier-release/mouse callback/teardown and COMMAND completion |
| Verify-CommandExitStatus.ps1 -Cases direct-graphics-return,graphics-return with VIDTST.COM | graphics | PASS copied graphics witness and return to text/MEM |
| observe-wow-frontiers.ps1, separate WINMINE/SOL/WRITE | wow-winmine / wow-sol / wow-write | Headless non-regression: all remain alive through 16-second samples at original NETWORK.DRV modal; no gameplay/full-function claim |

All observers and harnesses are terminal and test-owned cleanup completed.
No user desktop switching, guest/configuration mutation or system-registry
write was performed. Headless WOW observation timeouts describe the bounded
live-modal baseline, not a successful application exit.

Published and hash-verified at O:/winnt as one tested set, using
build/M0-T423/S2/notification-production-publication.json:

| File | SHA-256 |
| --- | --- |
| run16.exe | D402CAFD10C9C8AFD8244EDD980B6C7DAA149C559DFAF31091314757AAB965B5 |
| basesrv.exe | 06E6EBFA490D38C730BBF12EBD1BD120A89F722F49ACD3CC1BA1843DACC0C97D |
| ntvdm.exe | 287769E1E8E5532AD21EB5522868658C93A785251CF1C6898A6C13BFB76DFFE0 |
| dtmgr.exe | 7349D5C89384A00080D31ECD3D1B1AF9D543E72A58F125DFC37328FF9F02487C |
| WOW32.DLL | 51E1CAF83D248289EE546A5C134E663EBA6F930B672318F5C25BA08665FE507F |
| VDMREDIR.DLL | E54302CAB3A027CEF5FC57AFDF932B51AC7710D1F046086735E8D9F2E8A12829 |

Scope accounting against preceding P: srvvdm.c +6/-1 and srvvdm.h +1/-0;
no MVDM, frontend, guest or wire-protocol production edits. The added state is
internal original-owner notification persistence, not new adapter code.
Both changed mirrors retain their existing LF encoding and existing semantic
divergences; they are not byte-identical or newline-normalized-identical to
upstream CRLF. No whole-file formatting rewrite is bundled into this repair.
The remaining full per-layer/focus S2 checklist stays open; this delivery is
not S2 closure or admission of hidden Console/Window work.

### Owner-directed unpaired lifecycle transition (new P, in progress)

The owner explicitly supersedes the lifetime-pair contract. First committed
and pushed the complete preceding notification repair as 9f9b49346; the worktree
was clean and HEAD/origin/main were 0/0. Its tested six-file publication remains
at O:/winnt. Then began a separate P under the revised Status, architecture
and proposal: launcher/target execution and root-I/O ownership are independent.
Prior kill-on-launcher-loss passes above are retained historical evidence only.

Current changes remove the run16 native kill-on-close Job and its allocation
wrapper; native waits observe their actual target only. DOS waits no longer use
frontend process/pump loss as task completion or as a reason to terminate the
worker. The pre-handoff suspended-worker Job and rollback remain. BaseSrv's
service_end_abandoned_dos_pair and its launcher-exit watch are deleted; root
route cleanup and original worker-exit cleanup remain. console_client exchange
returns/stores a disconnect error instead of terminating its own worker.
The change uses ordinary existing process waits and original DOS completion;
it does not create a scheduler, task-kill facility or new mirror/provider.

Incremental MSVC Win32/x86 /MT run16, basesrv and ntvdm links pass. The revised
base_service_reservation_test has nine passing private-desktop cases under
O:/winnt/logs prefix m0-t423-s2-unpaired-r3-:
launcher-exit-survival, launcher-disconnect-survival, frontend-rundown,
frontend-delegated, frontend-wait-root-loss, frontend-wait-request-loss,
launcher-completed-rundown, launcher-completed-uncollected, management-terminate.
Final fixture SHA-256:
263B11200ACED6F941DD23DCD2AC11CA3DC9F6EB1DC9A96DD750B3C04E32B28A.
Worker survival is checked after synchronous service rundown; the fixture then
completes the original DOS record with 29 and verifies the parent's event while
the worker remains alive. Explicit fixture process termination occurs only
after the survival/completion assertions. Root/request acquisition cancellation
still reports legacy 1067, now without worker termination; its error-category
reconciliation remains pending, not claimed as the final I/O error contract.

Earlier m0-t423-s2-unpaired-launcher-exit-survival and r2 completed-rundown
attempts failed their final service-empty assertion after the survival/result
checks. With the launcher exit watch removed, process signalling no longer
incidentally waits for the independent worker-exit callback. The fixture now
boundedly observes IsEmpty for up to 5 seconds after its explicit cleanup;
it still fails if cleanup never completes. These failures remain recorded,
not reclassified as passes. All observers are terminal and children cleaned.

No new-P product publication or commit yet. Required continuation: root/inner
real native and DOS survival, later I/O-error behavior, direct result propagation,
startup rollback, worker's own failure, unchanged notification regressions,
full DOS17/WOW headless and affected device/nesting checks. Existing real-chain
harnesses still expect obsolete pair termination and must be revised before
they can qualify as acceptance. No user desktop interaction was performed.

### Unpaired-lifecycle production verification and delivery

The new P completes the implementation described above. Authenticated root
process/channel loss now returns ERROR_PIPE_NOT_CONNECTED (233) through the
worker's original callers. Process-wait and task-result routes still use 1067
only for actual worker failure. Input, output, geometry query and mode-setting
return the stable disconnect error; the transport does not kill its process
or fall back to local native presentation. No guest or mirror file changed.
The five production files total +22/-187 lines, a net reduction of 165 lines.
Startup containment remains limited to unclaimed/pre-handoff work; original
VDM completion, explicit management termination and worker-exit cleanup remain.

All runs below use the unswitched private desktop. Prefixes are beneath
O:/winnt/logs; tests clean up retained survivors only after their assertions.

| Gate | Prefix after m0-t423-s2-unpaired- | Evidence |
| --- | --- | --- |
| Original-service fixture, 18 selected modes | final- | Default reservation/worker failure, frontend identity, unclaimed cleanup/reconnect, launcher/root/request rundown survival, task result 29 while worker lives, collected/uncollected completion, explicit management termination, five re-entry notification orders pass. |
| Real client/frontend Console transport | io-r6-live / io-r6-broken-pipe | Root-process loss and channel-only loss return 233 on repeated output, input, geometry and mode calls; client remains alive and teardown joins. |
| Root pump resource fixture | r4-scope | Completed channels reclaimed, live channel retained, final join succeeds. |
| Native CUI and GUI | native-r3-cui / native-r3-gui | Actual target 37 propagates; killed launcher 91 does not terminate target, which subsequently completes 37; direct target/root normal completion leaves its descendant able to finish 37. |
| Root loss with unrelated worker | root | A real COMMAND worker survives root death while observer retains Console; unrelated worker executes ISOLATION-ALIVE and exits; fresh MEM succeeds. |
| DOS -> native -> nested DOS launcher loss | inner | Native CMD and nested DOS survive inner launcher death; normal nested DOS exit completes CMD; outer DOS receives its direct run16 low-byte 255, executes MEM and completes. |
| Native nested execution faults | nested-root / root-target / inner-dos / middle-launcher / middle-target / nested-worker / middle-input | Root/target/inner/middle losses do not recursively kill survivors. Interactive outer CMD recovers and returns 23. Actual worker death propagates 1067. Middle-input case recovers without a second injected fault; later root completion preserves unfinished descendants. |
| Worker failure with isolation control | worker | Dead worker fails its launcher with 1067; unrelated worker executes and finishes normally; fresh MEM works. |
| Full DOS17 | dos17 | All 17 expected results and real guest output checks pass, including direct/nested COMMAND, MEM, EDIT, authored exit 7, streams and EOF. |
| Nested and typeahead regression | nesting | native-cmd-dos, native-cmd-dos-repeat, dos-native-dos and dos-native-typeahead pass. |
| Video return | graphics | Direct and interactive original guest graphics-to-text return pass. |
| Guest keyboard/mouse | keymouse-observer / keymouse-guest | Existing authored KMTST and production observer return 0 after keyboard, modifier-release, mouse callback and normal return checks. No physical desktop interaction is certified. |
| Headless WOW3 | wow-winmine / wow-sol / wow-write | All remain alive through the 16-second samples at the existing NETWORK.DRV modal. Observer timeout is an observation bound, not app completion or gameplay acceptance. |

The native fixture source is tests/observation/run16_unpaired_native_test.c;
build-run16-unpaired-native.ps1 builds x86 /MT CUI and GUI variants below build.
Run each through console-startup-observer with MVDM_OBSERVER_PRIVATE_DESKTOP=1,
TEST_RUNTIME_ROOT set to the package and UNPAIRED_NATIVE_REPORT to its assertion
log. The fixture's ready/release events prove post-launcher-death work, not
merely the presence of a PID. The GUI report is a file because a GUI-subsystem
test cannot rely on Console stdout. Both variants assert rounds 0, 1 and 2.
Real nested faults use the updated Verify-BrokerFinalLifecycle.ps1 switches;
the observer holds Console for 5 seconds after root exit to avoid confusing
observer teardown/CTRL_CLOSE_EVENT with a product-initiated kill.

Fixture identities (SHA-256):

- service: EFD5C05EDF6240ACED6F6B6DB82110493B444BC6BCDFA3F092BA04FE2C044B8F.
- Console client: 9C9A5BC8AD605504D7C7DF77577B65402F83D0580B0FF478380DAF3B78515837.
- native CUI: CBBA95CC2F80867394F1BAE4441DD491D1EE5B1FC7C842731542361099FD3B54.
- native GUI: 9ECF65AFFE0ADA5FBB7370C8D11123780C42BB390DF5C5196BD195F9D9CC46B2.

Retained non-pass evidence: native-r1 GUI completed with 0 but its harness
wrongly demanded Console stdout; r2 uses the file witness and r3 adds descendant
survival. io-r5-live failed an assertion against native FlushConsoleInputBuffer,
which is not a selected worker adapter call; r6 tests the actual SetConsoleMode
control route instead. r4-frontend-capability was a misspelled mode and ran the
default fixture, so it is not frontend identity evidence. The fixture now
rejects all unknown modes, including obsolete launcher-pair-loss, with 64;
modecheck logs prove rejection and the real frontend-root mode passes.

The formal and WOW x86 caches were incrementally built; no full rebuild or new
runtime directory was introduced. Temporary candidate tests always restored
the six-file 9f9b49346 baseline. After all above gates, the exact tested set was
published coherently to O:/winnt and each destination hash verified:

| File | SHA-256 |
| --- | --- |
| run16.exe | 08479BFC56190AC2F67867894A733D63FCB169501F5A052E32CAFA57650D3745 |
| basesrv.exe | 12199170D3E663B12C1C552EBBC6A9C79BAED6B28C6AE82DD1CC306E43C47663 |
| ntvdm.exe | 94ADF896F3354F27106E0177667DBC9770B5016E7FC653A129D77A99A76C8B33 |
| dtmgr.exe | 7349D5C89384A00080D31ECD3D1B1AF9D543E72A58F125DFC37328FF9F02487C |
| WOW32.DLL | F2B6353836E8B4019E3079C9F96F927484049207F3C9CB7D7735F0363ABB9076 |
| VDMREDIR.DLL | AC601B4E27FCA4BA078F209C0BF5CCEF49197C337CA8BE25240CBD9735A67A1F |

Reproduction scripts, publication manifest and the prior package backup remain
under build/M0-T423/S2/unpaired-* and test-unpaired-*.ps1. No original guest,
SYSTEM.INI or Registry change was made. The original execution/handoff source
order guard passes. This is a production P, not S2/T423 closure: the full S2
fault/completion-race and physical focus/pointer checklist remains open.

### Completed task result versus later worker cleanup

The follow-on S2 audit found a deterministic delivery defect, not a guest or
CCPU fault. The original DOS record had completed with exit 29 and its parent
event was signalled. Killing its worker before the launcher collected that
result let BaseSrvCleanupVDMResources/BaseSrvExitVDMWorker free the record;
the subsequent original BaseSrvGetVDMExitCode returned 0 for the missing record.
O:/winnt/logs/m0-t423-s2-completed-worker-loss-red.txt records fixture exit 1;
its Console log records query=0, exit=0, expected=29. Test-owned children are
cleaned before the final failing assertion. The production package was not
changed for this reproducer.

Source-first audit uses O:/repos.external/OpenNT/base/win32/server/srvvdm.c,
SHA-256 C1E2177C6C00679D85CFA475F620841F6736B0E56D8DBF790B71AFE33E1ED80B:
BaseSrvGetVDMExitCode at 1803 and BaseSrvCleanupVDMResources/ExitVDMWorker at
2635/2694. The already-selected mirror retains those policies. This establishes
the standalone cleanup/query ordering failure; it does not claim that the
entire original NT4 kernel/client integration had this observed failure.

Recovery ladder and boundary:

1. Keep the already-composed original result owner and record cleanup; no new
   original files or mirror edits are required.
2. Before worker cleanup, the service invokes that original result query for
   a signalled parent wait. The existing authenticated connection retains only
   the resulting reply and exact wait receipt until the direct parent reads it.
3. Receipt mismatch cannot consume that reply. Successful delivery consumes it;
   subsequent queries retain the original missing-record zero behavior. New
   command admission or reservation release clears the retained reply.
4. An unsignalled parent wait still becomes actual worker failure (1067).
   Launcher/root-I/O loss remains non-terminating. No DOS record algorithm,
   scheduler, guest callback or CPU behavior is recreated in the adapter.

This is a +31/-3-line change in base_service.c (net +28); no mirror diff,
wire-layout change, new component or original guest modification. The component
README also removes stale pre-6d28dfac2 pair-kill wording.

New fixture modes are --completion-rundown-race and --completed-worker-loss.
The first starts a real competing thread behind a ready/go barrier while the
worker submits original completion. Thirty pre-repair stress repetitions
(m0-t423-s2-completion-rundown-1 through -30) passed: parent completion remains
observable and the worker survives. This stress does not claim every scheduling
interleaving was exercised. Existing ordered before/after cases remain controls.
The second forces worker cleanup after completion but before collection and
proves the specific lost-result sequence. It now also checks wrong-receipt and
one-shot result consumption. All 22 service modes pass on fixture SHA-256
671124649075A0461443BDCE8FC9B37BD76292774B648686FCA5494ABB5FB7C1,
under prefix m0-t423-s2-result-final-. The earlier green proof is under
m0-t423-s2-completed-result-green-; later modes add the receipt assertions.

Full x86 six-file build passes using the existing formal/WOW caches. Production
regression is run by build/M0-T423/S2/test-completed-result.ps1; it backs up the
published 6d28dfac2 package to completed-result-pre-change-package, temporarily
stages the candidate, and restores that baseline on completion or failure.
The candidate identity is recorded in completed-result-production-publication.json.
Final regression/publication disposition is recorded below after completion;
neither this source fix nor fixture success closes the entire S2 checklist.

Final production regression completed successfully and restored the previous
package before publication. Log prefix m0-t423-s2-result- records DOS17 (17/17),
nesting (4/4), graphics-to-text (2/2), guest keymouse, actual worker loss with
unrelated-worker isolation, root frontend loss, and inner-launcher loss with
surviving native/nested DOS targets. Fresh MEM and actual text are asserted.
Headless WINMINE/SOL/WRITE remain alive at the known NETWORK.DRV modal;
observation timeout is not gameplay or full application acceptance.

The exact tested six files are published together to O:/winnt:

| File | SHA-256 |
| --- | --- |
| run16.exe | 56013C6477CE2E40221A164B1A6FB5AD52D90958835D852F0A221A5C1AB0FD74 |
| basesrv.exe | 87DAC2335446431F127170B79EE00870E990A4A026E0006AB6E6C9A080FA0E93 |
| ntvdm.exe | A3F27ED9C97D82088CE51D2776C106DF8348A9CA7A1AF6F2D5688C0855667FB8 |
| dtmgr.exe | 7349D5C89384A00080D31ECD3D1B1AF9D543E72A58F125DFC37328FF9F02487C |
| WOW32.DLL | D22C407AE8F769A556492F6AF83A06AD5DCEFC7E1F0877F0B933DBA18D151326 |
| VDMREDIR.DLL | C1670FE36E9E9E40141689ED98CCD1B49B2AE1312BF7EA722BE2EC98E11DC094 |

No original guest, Registry, SYSTEM.INI, shared library or foreground desktop
interaction changed. S2 remains open for its explicitly remaining gates.

### Post-result-fix failure matrix verification

The published fab033613 package (six hashes above) was exercised unchanged
with Verify-BrokerFinalLifecycle.ps1 and the private-desktop control-observer.
No desktop switch, production edit or guest-media change was made. Evidence
under O:/winnt/logs uses the prefix m0-t423-s2-post-result-:

| Suffix and switches | Observed assertions |
| --- | --- |
| broker: BrokerLoss, TwoWorkers | Both direct waits fail with 1722; workers end on broker loss; a fresh broker/worker runs MEM with actual memory output and returns 0. This is not root-frontend loss. |
| native-target: DosNativeLoss, NativeRoot | Inner native target and its launcher return -1; DOS observes low-byte 255, runs MEM and finishes; outer interactive CMD remains live, executes recovery input and returns 23 through root run16. |
| inner-launcher: NestedWorkerLoss, NestedInteractive, LauncherLoss | Innermost launcher exits -1; its native ancestors naturally observe their direct child result, DOS worker survives; outer interactive CMD executes recovery input and root returns 23; fresh MEM passes. |
| middle-launcher: NestedWorkerLoss, NestedInteractive, MiddleLayerLoss, MiddleLayerInputProbe, LauncherLoss | Middle launcher exits -1; its native target, nested DOS launcher/worker and ancestors survive. Recovery input is sent without a second fault; outer CMD/root complete with 23 while the unfinished DOS descendant remains alive. Fresh MEM passes. |

Each script asserts process states before its finally block explicitly cleans
test-owned survivors. Cleanup is not counted as product termination. These
checks strengthen current-candidate evidence, but do not prove all completion
interleavings, physical pointer/focus behavior or future Window input routing.

A production-source termination sweep finds no service_end_abandoned_dos_pair
or long-lived native-target kill Job. Remaining calls are scoped: run16's
suspended-startup Job and pre-resume rollback; unclaimed reservation rollback;
explicit management termination; the private Console-query helper's cleanup;
and the worker's own broker-death failure. They are not a newly authorized
launcher-death or frontend-loss execution-tree policy.

### Channel terminal paths beyond explicit cancellation

The production channel/dispatcher fixture now adds oversized-request rejection
and death of a real disposable suspended process to the existing three modes.
The latter uses a real process wait handle, not a signalled-event substitute.
The oversized case must return ERROR_INVALID_DATA; process death must return
ERROR_PROCESS_ABORTED. Both must signal readiness and expose ERROR_BROKEN_PIPE
with zero bytes to the peer before owner stop. Each channel is subsequently
stopped/joined normally. Across 85 cases, no owned-handle growth is allowed.
Only broker attachment is substituted; no original guest or production file
changes. The suspended child never executes guest or other application code.

x86 /MT compilation succeeds via tests/observation/build-console-channel-lifetime.ps1
under build/M0-T423/S2/channel-terminal-eof. Fixture SHA-256:
2B800CC4203D08D43699CE6924A1FCACAD59C871B416364148BE882997816949.
Two unswitched private-desktop runs, O:/winnt/logs/m0-t423-s2-channel-terminal-eof-a
and -b (.txt reports and .txt.console.txt actual output), both exit 0 and print
the 85-case PASS marker. Earlier channel-terminal-paths lacked the explicit
peer EOF assertion and is not used to claim it. Compiler warnings in included
production code remain recorded; this is successful compilation, not /WX.

Pointer audit does not introduce speculative cleanup: original nt_mouse.c
MouseOutOfFocus releases clipping, while original USER kernel focusact.c and
queue.c clear global clipping during activation. Modern ClipCursor likewise
documents a shared resource and release responsibility:
[Microsoft ClipCursor contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-clipcursor).
An unconditional channel-stop release could affect another current user;
these source observations do not prove physical focus/clip cleanup on modern
Terminal. That gate stays open. This fixture never clips or moves the pointer.

### Guest progress after frontend loss: retained completion failure

tests/observation/frontend_loss_noio.asm is an independently authored COM probe,
not modified original media. It creates logs/NIOREADY, polls for logs/NIOGO,
then creates logs/NIODONE and invokes DOS exit 7. It makes no Console input or
output request. The native controller frontend_loss_noio_test.c creates GO
only after its root run16 has actually exited. Therefore DONE proves guest
work after root loss, rather than mere worker process survival.

The initial direct abnormal-root case passes: controller terminates root with
91, observes its exit, releases guest and sees DONE. Evidence is
O:/winnt/logs/m0-t423-s2-frontend-noio.txt and its Console sidecar.
The --normal case instead runs a native target through root run16; that target
starts the inner DOS launcher, waits for guest READY, publishes its PID and
returns 37. Root propagates 37; the controller verifies inner launcher is live
before allowing guest to finish. This case additionally requires inner exit 7.

The initial normal case omitted handle inheritance in its authored native
target, preventing the nested launcher from retaining the frontend capability;
r2 reports child 1727 before guest READY. The corrected test uses inheritance
for that nested launch, as CMD does, without changing production authentication.
It reaches READY and DONE, but r3 and r4 fail at nested-result. These failures
are retained, not reclassified as success. No promise is made for an arbitrary
native application that deliberately suppresses capability inheritance.

r4 uses existing MVDM_S34_TRACE_PATH at
O:/winnt/logs/m0-t423-s2-noio-normal-run16.txt. Root returns 37. There is no later
GetNextVDMCommand completion RPC in that trace. A temporarily compiled wrapper
around the existing native_console_observer.c thread sampler captured and
resumed the test-owned worker; raw evidence is
O:/winnt/logs/m0-t423-s2-noio-worker-stack.txt. With actual image base 003E0000
and the tested formal-cache map's preferred base 00400000, frames resolve to:

- CPU thread: nt_block_event_thread+53 -> cmdGetNextCmd+26C -> CmdDispatch ->
  MS_bop_4 -> ccpu. It waits for the original event-thread stall acknowledgement.
- Event thread: ErrorDialogBox+186 -> host_error+72 -> DisplayErrorTerm+A0 ->
  nt_event_loop+135 -> ConsoleEventThread. It has entered original fatal-input
  error presentation instead of acknowledging that stall.

Selected nt_event.c handles ReadConsoleInputExW failure by DisplayErrorTerm;
nt_error.c passes ERR_QUIT to host_error. The original source has that error
branch too. The new frontend-disconnect error reaches it; this is not evidence
of a guest defect, completed broker record loss, or a license to kill a worker
on launcher death. The requested original error-handling contract and bounded
task-completion expectation need reconciliation at this boundary. The complete
--normal assertion remains failing; only continued no-Console file work passed.

Build with NASM -f bin and MSVC x86 /MT /W4 /WX, all outputs under
build/M0-T423/S2/frontend-loss-noio. Stage the authored NOIO.COM only into the
existing package tests directory. Run the native controller through the existing
private-desktop console-startup-observer, with working directory O:/winnt,
optional --normal after observer options. The four fixed NIOREADY/NIOGO/NIODONE/
NIOPID files under logs must be absent; the test refuses existing markers.
After recording evidence, stop only the identified test-owned processes before
removing these handshake files. Production binaries and original media remain
unchanged. Physical desktop input remains prohibited by the owner's new reply.

Final fixture identities: NOIO.COM
C5686BF3619BA5020DA62181AA74D4097669C43873C6D334C7F939E4DB1F721D;
controller test.exe
21465604DCA33596879832A317F4B3BF96598DEAFAF3B410FCA05C9B4553A028.
The final-direct repetition also exits 0 and proves DONE after root death.
The same controller's normal-r4 remains the failing completion reproducer.

## Root frontend session close, original VDM callback and result preservation

Owner admission on 2026-09-26 supersedes the preceding root-loss survival
contract: root run16 defines the interactive session lifetime. Its normal or
abnormal end closes associated DOS workers; non-root launcher loss still does
not terminate handed-off execution. Display switching is not session close.
Native descendants and unrelated workers are not recursively killed. The old
NOIO result remains valid historical evidence under the former contract, not
the acceptance expectation for this revision. No input-pump retirement bypass
was implemented.

Source-first recovery and stopping boundary:

- Original OpenNT base/mvdm/softpc.new/host/src/nt_event.c::CntrlHandler,
  SHA-256 7F555A87BA029627D6811C0F7B96964BB8A96AF90C60C89012975A4AC5442076,
  already composes in the worker. Its CTRL_CLOSE_EVENT branch and original
  termination chain remain unchanged; invoke it rather than recreate its state.
- OpenNT windows/core/ntcon/server/input.c::CreateCtrlThread/KillProcess,
  SHA-256 84D7D4C5A422208D14900C8F3A86B0F9648A3755F03D6D49765B35266617252A,
  owns remote control callback, hung-close handling and eventual termination.
  The translation unit requires private CSRSS/USER process lists, callback
  injection and close/retry/cancel UI. It is a stopping boundary, not an
  import. The already authenticated root process capability replaces only
  session identity/delivery. The existing worker watcher invokes a bound local
  callback and bounds unresolved closure to five seconds before terminating
  only its own process. This forced-close choice is an explicit standalone
  product mapping: the vanished frontend cannot present a cancel dialog.
  Five seconds is a close grace, not a worker idle timer or a claim about
  every original NT4 registry-selected timeout. No task enumeration/kill,
  new IPC field, broker frame route, mirror file or guest change is needed.
- Ordinary pipe failure remains 233 and does not initiate closure. Only the
  retained root process object's signalled state triggers this path. Remove
  dead-root command rebinding instead of adding an orphan-input state machine.
- Original BaseSrvExitDOSTask frees the Console/DOS records before process
  rundown. srvvdm.c SHA-256 remains
  C1E2177C6C00679D85CFA475F620841F6736B0E56D8DBF790B71AFE33E1ED80B.
  The existing completed-reply protection must run before BOTH orderly DOS
  ExitVDM and process cleanup. Extract/share that binding, retaining original
  result queries, authenticated receipts and one-shot delivery; do not alter
  original records or WOW per-task ExitVDM behavior.

Two new service fixture modes demonstrate the prior gap. Under prefixes
m0-t423-s2-close-completed-red and m0-t423-s2-close-unfinished-red, the first
prints query=0 exit=0 expected=29 and fails; the second fails its required
ERROR_PROCESS_ABORTED assertion. After repair, completed-worker-exit and
unfinished-worker-exit pass along with all existing modes. The failed second
fixture left a suspended test child (8532); its exact image/command line was
verified and it was terminated before relinking. No product process was
mistaken for this build lock.

Reproducible checked-in tests:

- tests/observation/verify-console-close-contract.ps1 takes Observer,
  BuildRoot and Prefix. It forces an unswitched private desktop, runs three
  native Console-client cases and the complete service fixture mode catalogue
  (24 including default, excluding reservation-child), checking actual PASS
  output and exact exit code. Prefix m0-t423-s2-close-contract-final passes:
  callback exits 73 by test substitute, hung callback reaches C000013A within
  the bounded close, broken pipe survives and exits 0; all 24 service cases
  exit 0. The callback substitute proves binding/delivery/bounded failure,
  not guest cleanup. An existing re-entry fixture's IsEmpty assertion raced
  the asynchronous process watch; it now waits for that actual cleanup with
  the same bounded deadline already used by sibling tests, without weakening
  the re-entry/wakeup assertions.
- tests/observation/frontend_loss_noio_test.c plus the unchanged authored
  frontend_loss_noio.asm: the native controller captures the exact worker
  before ending the root. Abnormal root=91 closes the worker without any guest
  Console request. --normal holds the native target until the controller has
  its nested identity, returns 37, and asserts worker closure plus unfinished
  inner run16=1067. NIODONE must remain absent. Prefix
  m0-t423-s2-session-close-final passes both. The added NIOROOT file is a test
  handshake, not a product mechanism. A sharing-violation race while publishing
  NIOPID was fixed by waiting for its writer to close; this failure and the
  earlier fixture-argument/VS-environment runs are not product passes.
- tools/audit/Verify-BrokerFinalLifecycle.ps1 replaces only the superseded
  root-survival expectations. Non-root/handed-off native survival and unrelated
  worker post-fault command output remain required. Test cleanup is separate
  from asserted product shutdown.

Build uses the existing MSVC Win32/x86 /MT Ninja caches under
build/M0-T423/S1/restart-formal-x86 and restart-wow-x86. Changed closure is
rebuilt, including VDMREDIR's regenerated worker import library dependency.
Original media/configuration and the WOW binary remain unchanged. No object
or new directory is emitted outside repository build. Runtime reports stay
under O:/winnt/logs; NOIO.COM stays under its existing tests directory.

The final six-file candidate passes the production gate under
m0-t423-s2-close-final-: DOS17, four native/DOS nesting/typeahead routes, two
graphics-to-text returns, guest keymouse, root loss with independent worker
output, inner launcher loss with native/nested DOS survival, and actual native
Console close. WINMINE/SOL/WRITE individually retain their live NETWORK.DRV
modal through headless samples; observation timeout is not application success.
No physical foreground/pointer test is claimed. Extra failure-matrix results
and the exact published manifest are recorded below after final verification.

Production line accounting against ad1db0a86: console_client.c +26/-22;
base_service.c +22/-12; total +48/-34, net +14, including comments. The two
original mirrors have zero changed files/lines. No overlay, new component,
wire-layout or guest-media change. Documentation and tests are separate.

Final extra matrix, prefix m0-t423-s2-close-final-:

| Suffix / entrypoint switches | Asserted result |
| --- | --- |
| middle / NestedWorkerLoss, NestedInteractive, MiddleLayerLoss, MiddleLayerInputProbe, LauncherLoss | Middle native target and nested DOS survive non-root launcher death; outer CMD accepts recovery input and returns 23. Its later root completion closes the unfinished associated DOS endpoint. No second injected worker fault. |
| nested-root / NestedWorkerLoss, FrontendLoss | Root ends -1; associated worker closes; nested wait returns 1067 and native /c parents return that direct result naturally. |
| root-target / NestedWorkerLoss, RootTargetLoss | Root's direct native target and root return -1; associated worker closes and unfinished nested task returns 1067. |
| worker / WorkerLoss, TwoWorkers | Failed task returns 1067; independent worker executes its post-fault marker and exits normally; fresh MEM prints its report. |
| broker / BrokerLoss, TwoWorkers | Both waits report 1722, workers close, no replay; a new startup runs MEM normally. |
| native-cui / native-gui | Each checked-in unpaired-native fixture passes all three rounds: target/root=37; killed launcher=91 with target continuing to 37; normally completed root leaves native descendant able to finish 37. |

Production package has been published and all six destination hashes verified
against the actually tested files in build/M0-T423/S2/session-close-tested.json:

| O:/winnt file | SHA-256 |
| --- | --- |
| run16.exe | 0AF0457B0B256D93CD9BA019B7BCD8AD0ED9263C7385DA9758850CFEB594B20C |
| basesrv.exe | 187365292FEA3D46D9E1F67E13573B9879A93AAAEA16323793D620B41BC1114C |
| ntvdm.exe | 66C268B73696EEFDADD41AEC52660BB13E4A515F040B2C66199E89C42C0F0A84 |
| dtmgr.exe | 7349D5C89384A00080D31ECD3D1B1AF9D543E72A58F125DFC37328FF9F02487C |
| WOW32.DLL | D22C407AE8F769A556492F6AF83A06AD5DCEFC7E1F0877F0B933DBA18D151326 |
| VDMREDIR.DLL | 947D28F85E5BE1E3C1AA279E3979B8238A48119CF2227B2C131C9F43E5DCBAB5 |

The preceding fab033613 package is retained intact at
build/M0-T423/S2/session-close-pre-change-package. Each temporary candidate
test restored it in finally; permanent publication happened only after all
final gates passed. Five disposable NIO handshake files were removed after
test-owned process cleanup; no original media or user data was removed.
S2 remains open for its residual physical-input disposition; this delivery does
not admit hidden Console/Window work or claim owner desktop verification.

### Published close-package focus-record recheck

Question: does the d0f856a50 root-session-close change preserve the previously
verified focus-record/keymouse/COMMAND return path? All six published hashes
match the session-close-tested.json manifest above. No production input,
original guest or configuration was changed and no rebuild was needed.

Reused observers: build/M0-T423/S2/control-observer/console-startup-observer.exe
and build/M0-T423/S2/keymouse-focus-transition/keymouse-capability-observer.exe.
The procedure is the earlier Focus-record transition section: private desktop,
TEST_RUNTIME_ROOT=O:/winnt, MVDM_TEST_KEYMOUSE_SHARED_CONSOLE=1,
MVDM_TEST_KEYMOUSE_FOCUS_TRANSITION=1, and
MVDM_TEST_KEYMOUSE_COMMAND=O:/winnt/tests/KMTST.COM. Pass the keymouse observer
as the startup observer's target, O:/winnt as its working directory,
--observation-timeout-ms 60000 and the guest-report filename as target argument.
Reports are O:/winnt/logs/m0-t423-s2-close-focus-recheck.txt and
m0-t423-s2-close-focus-recheck-guest.txt. The target exits 0; actual guest
markers confirm keyboard, modifiers/release, PPI, mouse reset/position/callback
and teardown. The observer then reads MEM output and the returned prompt;
its final diagnostic is keymouse passed=yes stage=command-exit error=0 exit=1
focus-record-transition=injected. This is a fresh pass on the published close
package, not an inference from the older package.

Checklist reconciliation: the actual Console-close case in
m0-t423-s2-close-final-console and callback/hung-callback cases in
m0-t423-s2-close-contract-final establish close delivery and bounded completion.
The callback fixture substitutes CntrlHandler; the real Console-close lifecycle
case complements it. Neither demonstrates future display switching. The
proposal now checks only this proved close portion and explicitly retains
CAF/AE/X non-close verification in S4, where those controls are implemented.

Physical focus movement and pointer clipping/release are still unproved.
Injected records are not a substitute. No desktop switch, physical input,
foreground activation or new clipping operation was performed. S2 remains
active; this evidence-only P neither admits S3 nor changes its exit standard.

### Nonzero DOS completion and batch-entry contrast

Added tests/observation/verify-dos-nonzero-continue.ps1 and its
dos-nonzero-continue.bat. The runner assembles the existing independently
authored dpmi_exec_child.asm without defines: tail " OK" prints its success
marker and exits 7; absent tail prints its negative marker and exits 99.
The DOS batch checks both exact ERRORLEVEL values before executing MEM and a
final marker. It runs through the published run16 COMMAND.COM /c path on an
unswitched private desktop. No original guest media or production code changes.

Reproduce with -Observer build/M0-T423/S2/control-observer/console-startup-observer.exe,
-Nasm pointing to the installed WinLibs nasm.exe,
-BuildRoot build/M0-T423/S2/nonzero-continue, -PackageRoot O:/winnt and a fresh
-Prefix. Resolve Observer and BuildRoot to absolute Windows paths. The runner
refuses overwriting reports or different runtime probe contents. Its COM and
BAT copies reside only in O:/winnt/tests; logs remain in O:/winnt/logs.

Prefix m0-t423-s2-nonzero-continue-final passes: actual output contains both
child markers, S2-DOS-EXIT-7-RETAINED, S2-DOS-EXIT-99-RETAINED, MEM's conventional
memory report and S2-DOS-AFTER-NONZERO; COMMAND /c finishes 0. This establishes
continued execution after guest nonzero completion. It does not substitute
for BaseSrv-record receipt tests or imply physical focus acceptance.

Two preceding non-pass observations are retained. Prefix
m0-t423-s2-nonzero-continue-batch had all the same output and exit 0, but the
first runner incorrectly expected interactive COMMAND's exit 1 for /c; the
corrected runner was rerun rather than declaring that failed invocation green.
Earlier m0-t423-s2-nonzero-continue typed tests\S2NZ.BAT into interactive
COMMAND and timed out without batch markers. A native inner run16 with
TESTS\S2NZ.BAT remained after observer timeout. Its exact process identity and
the test broker were checked before cleanup. The startup path differs from
COMMAND /c: run16 main.c's non-image branch delegates the copied command to
COMSPEC. The cause of this interactive batch stall is not yet established;
it is an open S2 integration investigation, not an original-guest limitation
or an accepted exclusion. Keep the combined completion/interaction checklist
open until that contrast and remaining gates have their proper disposition.

### Resolved batch-entry contrast and nested nonzero return

The preceding timeout is now attributed without production changes. Prefix
m0-t423-s2-batch-stall-dialog reproduces the same typed tests\S2NZ.BAT.
Live CIM identities show root run16 -> ntvdm -> inner run16 TESTS\S2NZ.BAT
-> native cmd.exe /c TESTS\S2NZ.BAT. The existing read-only
build/M0-T423/S2/worker-window-snapshot.exe was called with exact CMD PID 14576
and desktop NTVDMConsoleTest-2436, named for the outer observer PID. It found
a visible #32770 "Unsupported 16-Bit Application" dialog naming
O:\winnt\tests\S2EXIT7.COM and stating incompatibility with 64-bit Windows.
Captured text: O:/winnt/logs/m0-t423-s2-batch-stall-dialog-windows.txt.
No foreground activation, desktop switch or dialog interaction occurred.
After timeout, only identified test-owned CMD/launcher/broker processes were
ended. The first experiment's surviving native CMD was also identified and
cleaned, correcting that experiment's incomplete test cleanup. Retained native
target lifetime itself is not a product leak claim.

Source agrees: cmdexec.c cmdExec's AH-selected COMSPEC path delegates to the
native command processor; command_process_compat.c routes the simple tail
through run16; run16 main.c's non-image branch preserves COMSPEC /c. Native
CMD cannot directly CreateProcess this DOS COM on modern Win64. The proposal
already requires explicit run16 when entering DOS from native CMD. This is a
test-route error against that contract, not worker completion deadlock, an
original-guest bug or a newly waived requirement. No shell parser, process
hook or mirror change is added to hide that unavailable OS facility.

Added dos-nonzero-native.bat and -NativeBatch to the existing runner. Typed
into interactive DOS COMMAND, the variant executes under native CMD and uses
explicit run16 for both test COM invocations and MEM. CMD checks exact 7/99;
actual guest markers and MEM output are mandatory, then outer COMMAND exits 1.
Prefix m0-t423-s2-nonzero-native-final passes. Default DOS COMMAND /c mode is
rerun as m0-t423-s2-nonzero-dos-recheck and passes with batch completion 0.
Both use the unchanged six-file d0f856a50 production manifest. These prove
nonzero DOS completion and continued execution through native/DOS re-entry.

Together with close-contract-final's completed-worker-exit and
unfinished-worker-exit cases (retained 29 versus failure 1067), they cover the
proposal's nonzero/ordered-close row, now checked. Broader topology, I/O and
physical-input rows are not implicitly closed.
