# T449 S1 — WOW execution continuation audit

## Question, baseline and method

Determine which transferred T447 execution obligations remain on the actual
T448-delivered source and package, and identify complete implementation groups
without treating old partial passes as whole-feature proof. Baseline is
`1f2f968e8` (product release `55566cadb`). This first source pass reads the
T447 [execution plan](../operations/t447-wow-execution-plan.md), its
[S4 message ledger](m0-t447-s4-message-source-audit.md), the
[S3 control ledger](m0-t447-s3-control-source-audit.md), the
[S2 closure](../../history/m0-t447-s2-foundation-closure.md), the T448
[integrated acceptance](m0-t448-s5-integrated-acceptance.md) and current
selected sources. It does not execute a test or change guest, product source,
an exception budget or a compatibility boundary. The table is an audit in
progress, not a closure of S1 or a substitute for the detailed inherited rows.

The T448 source comparison `git diff 4d4ef4d47..HEAD` over the WOW32 mirror,
WOW adapter, service management/lifecycle and NTVDM worker lifecycle finds
changes in `wow32/wkman.c`, `wow32/wow32.c`, service `management.c` and
`lifecycle.c`, and the NTVDM worker lifecycle; no files in `ntvdm-exe/wow`
changed in that restricted comparison. `wkman.c` and `wow32.c` changes are
registered DIV-356 whole-worker exit authorization, not an individual-task
termination implementation. This is a file-level fact, not proof that all
dependent build inputs or later behavior are unchanged.

| Stable obligation | Selected chain and observed status | Existing evidence and remaining boundary |
| --- | --- | --- |
| M1, C01--C03/C09/C10: send, post, retrieval, filters and quit | `mvdm/wow32/wumsg.c` and message thunks retain original conversions and native USER transport; `wow_user_message_bridge.c` services the native send boundary, not a replacement queue. | `tests/observation/verify-win16-message-probe.ps1` asserts ordinary same-worker send/nesting/reply/post/filter/quit and has distinct cross-worker and external-peer modes. T447 recorded bounded passes; the script itself records unproved implicit focus, full converted structures and other variants. Do not infer complete M1 from one mode. |
| M2: mixed receive, reply and continuation | Original `WU32InSendMessage`/`WU32ReplyMessage` use native USER. Current `wow_user_task_lifecycle.c::native_call_begin/end` uses an execution scope; it does not manufacture a received SMS. | The old opening table in the S4 ledger describes a *superseded* target-based `psmsSent` candidate. The later ADAPTER-WOW-077 record and current source show it was removed. Keep actual mixed/implicit receive, duplicate/no-pending reply and reentry evidence separate from this source observation; full M2 is open. |
| M3: scheduling and exceptional waits | `opennt-host/.../taskman.c` retains original task scheduling; lifecycle scope supplies actual native-stack readiness. `mvdm/softpc.new/host/src/nt_eoi.c::BlockWOWIdle` remains the one-bit interrupt owner. | S4's final source disposition rejects speculative unconditional idle toggles. A real original `W32Dispatch/W32Exception` Ignore/Close and subsequent task/hardware-wakeup witness is not established by the normal callback fixture. Preserve original behavior until the whole exception path is proved. |
| M4: conversion and data lifetime | Original `wumsg.c`, `wmsg16.c`, `wmdisp32.c` and `wdde.c` own conversions; DIV-350--354 and existing call-local acquisition records are present in current source. | T447 source-body alias/queue fixtures and a real classic Execute/ACK witness are bounded evidence. They do not prove DATA/POKE/ADVISE/INITIATE, failed transfer, all output structures or complete cross-worker lifetime. Verify source and actual selected build identity before reusing an old candidate result. |
| M5: ordinary failure and retirement | Existing callback/borrow/terminal owners and original task cleanup remain distinct from requested individual deletion. T448 changes the service/worker close boundary and must be included in current chain review. | T447 S2 was closed under the explicitly accepted isolated common-dialog risk; S3 was closed with bounded control coverage. Those closures do not prove all stale-handle/reentry/receiver-death paths. Review actual changed boundary and retained raw-fault risk rather than reopen closed rows wholesale or dismiss the risks. |
| M6: integrated execution | Real two-task Win16, native x86/x64 peers, production WOW/CCPU and immutable guest remain required. | T448 final preclosure covers DOS17 in both presentation modes, game interactions, WRITE startup frontier and lifecycle/CAF; it is not the full inherited M1--M6 matrix. `verify-win16-message-probe.ps1` and its mode-specific assertions are the existing real-guest starting point, not a blanket pass for every mode. |
| Individual WOW task deletion | `management.c::service_append_worker` projects `WOWRECORD.iTask` into a management row and deliberately does not advertise targeted close. `lifecycle.c::OpenNtBaseServiceCloseManagementNode` accepts worker/frontend and refuses other categories. Original `wkman.c::W32Thread` has both `VDMInfoiTaskID` and `htask16`; `W32HungAppNotifyThread` selects by `htask16`. | This is a source-confirmed missing control path, not a failing test claim. The management sequence/request is not an HTASK. The admitted [termination requirements](../operations/t447-wow-execution-plan.md#approved-individual-wow-task-termination) require incarnation-safe identity, guest-launched/separate-WOW coverage, deletion/exit races and surviving sibling. `W32DestroyTask` signals the hung event before cleanup; that event is not completion. Original hung handling can escalate to another task/whole worker and must not do so without the already required separate confirmation. |
| Basic controls/dialog/menu dependencies and app witnesses | Original selected control/dialog/MENU-01 owners and bounded S3 fixes remain; all unrelated W02/W03/W06/W09 family completion stays with its existing candidate unless a specific execution prerequisite is demonstrated. | T447 control-interaction proof covers listed controls and three style modes, not all UI. T448 verifies WINMINE/SOL gameplay and only the WRITE startup frontier; WRITE editing/exit and intermittent direct-game long-play failure remain unproved. Actual owner SOL geometry and environment-dependent witnesses must not be silently reclassified as defects or passes. |

## Specific identity and lifetime observation

Original `src/mvdm/vdmdbg/vdmdbg.c::VDMTerminateTaskWOW` enumerates the
shared process record then starts `W32HungAppNotifyThread` in that worker with
the actual `htask`. Reusing its *contract and original in-worker algorithm*
does not authorize copying its remote function pointer into an NTSRV RPC or
introducing cross-component direct control. In the current service the
management label is keyed by a worker generation and a service-generated
incarnation sidecar for a `WOWRECORD`; its displayed `record->iTask` is an
admission/request identity. `W32Thread` writes the actual guest `wTDB` to
`td.htask16` and separately stores `iW32ExecTaskId` in `td.VDMInfoiTaskID`;
guest-created tasks may lack that request identity. A safe group therefore
must establish actual worker-owned binding and its lifetime, including task
reuse, before supporting Delete. The original `ptd` search and later guest
access also need a concrete race/ownership review; a search hit is not a
proof that an unprotected pointer remains live.

The current service display is also explicitly conditional: both
`service_capture_wow_labels` and `service_append_worker` only traverse
`WOWHead->WOWRecord` when the one `WOWHead` sequence matches the selected
worker. The original `srvvdm.c` record is a BaseSrv command/wait list, not
an enumeration of every original `TD`. `wkman.c` assigns `VDMInfoiTaskID=-1`
on its `W32Thread`-related internal task path. This source comparison means
the current projection cannot itself establish complete guest-task coverage,
particularly for internally created tasks and separate WOW workers; it does
not by itself establish which of those tasks actually appear in NTMON at
runtime. Resolving the observable task set requires tracing worker publication
and retirement through original TD/shared-task ownership and NTSRV's existing
RPC controls, not casting an `iTask` or duplicating a remote TD pointer.

At the current worker edge, `worker_base_controls` exposes the authenticated
server, worker-shutdown and Console-close signals; NTVDM's
`worker_lifecycle.c` maps them to the existing close routines. Neither this
signature nor `OpenNtBaseServiceCloseManagementNode` currently conveys a
task identity. A complete design must cover NTSRV's accepted task request,
worker-base dispatch and NTVDM/WOW32 execution/acknowledgement together using
the project's established RPC/I/O boundaries. Merely setting the existing
worker-shutdown event or adding a management UI item cannot implement the
targeted operation, and the shared worker-base must not interpret WOW policy.

## Existing real-message probe: assertion boundaries

The checked-in `tests/observation/verify-win16-message-probe.ps1` is not a
single all-features run. Its option validation deliberately separates modes:
`ClassicDde` cannot be combined with cross-worker, external peer, pending
send, construction or early-reply/timeout payload modes; cross-worker requires
its own native host and coordinator; external-native requires a separately
built peer. A passing ordinary invocation therefore cannot prove those other
directions. Each future result must identify its selected package and options.

The structured mode explicitly asserts only MINMAXINFO, WINDOWPOS and
COPYDATA in its named directions; the script says it does not prove physical
guest memory movement, all branches or allocation failures. Construction mode
asserts three MDI creates/destroys, title and parameter mutation, text and
WinHelp, but leaves fault injection to the separately identified diagnostic
provider. `ClassicDde` asserts native-to-Win16 EXECUTE, reverse ACK and alias
release only. The pending-send branch asserts a blocked-send timeout and
specific receiver-window destruction/result/identity values; it is stronger
than ordinary mode, but is not a general proof of receiver thread/process
death, every failure reason or all message payloads. The unconditional
`limits.json` wording about receiver death describes ordinary mode and must
not override the more specific pending-send assertions, nor be interpreted as
proving a completed pending-send run in the final package.

Normal same-worker mode additionally checks two actual task-retirement
records; cross-worker mode instead validates two separate worker identities,
process/window/thread associations and normal process retirement. These
assertions are useful M1/M5/M6 witnesses but do not establish NTMON-targeted
task deletion or sibling survival. Existing historical pass records must be
matched to these exact modes and package hashes before marking any row proved.

## M1--M3 receive, reply and scheduler boundary

Current `WU32InSendMessage` and `WU32ReplyMessage` directly call the host
USER APIs; no nominated target is passed to either. The project bridge
initializes the actual current thread's USER queue and uses non-removing
`PeekMessage` with `PM_QS_SENDMESSAGE` to admit pending synchronous native
callbacks. Guest Get/Peek retains posted messages and WM_QUIT. At native wait,
`GetQueueStatus`'s high word detects already-present synchronous work before
`MsgWaitForMultipleObjectsEx`; the continuation schedules the original task
event without creating an SMS object or asserting a fabricated receive.

The retained original `taskman.c` still contains `DirectedScheduleTask`,
send/receive lock counts and a `xxxSleepTask` branch based on `psmsCurrent`.
A repository-wide search finds no current production setter of `psmsCurrent`
or `psmsSent` outside that original body; the header fields remain to compile
the original structure. Thus the original private-SMS-specific branch is not
shown to execute with a real modern host SMS. This is consistent with the
already approved modern USER message/reply boundary, not proof that all NT4
early-reply execution ordering is preserved. It is also not justification to
construct a synthetic SMS or cast a native opaque value to the old structure.
Original non-SMS task selection/yield and existing execution scopes remain in
the path; mixed implicit/explicit receives, nested reply, pending wait and
same/cross-worker progress must be verified against the accepted contract.

There is no source basis in this pass to delete the bridge: the original
`xxxReceiveMessages` provider requires private NT4 USER queue state that is
not available as a composable current host interface. The project bridge is a
public-host service adapter, not a replacement guest message queue; its
non-removal and callback/execution boundaries are acceptance obligations.

## Object and native-call dependency reconciliation

The current creation path is not just a post-create decoration:
`wow_window_creation_binding.c` installs a thread-local CBT observer before
`CreateWindowExA/W` (or the selected dialog creation service), enrolls the
native window during its callbacks, and rejects a returned window if the
expected enrollment is absent or belongs to a different HWND. Its `__finally`
pops the nested scope, removes the hook, releases the borrowed window/class
owners and restores the native-call execution scope. A failed result retires
an enrolled partial window. This is source evidence of the intended paths,
not proof that every hook veto or exceptional reentry has been exercised.

`wow_user_native_call_begin/end` uses the existing task execution scope and
original `xxxSleepTask`/`xxxDirectedYield` when required. At return,
`publish_guest_return` refreshes the explicit window/payload graph, desktop
borrow and native menu views before guest continuation. A failed required
publication enters the existing terminal native-call failure boundary rather
than silently continuing on a partial view. The approved snapshot boundary
still applies to other processes; this synchronization is not a claim of
real-time global shared memory. A message cohort that crosses a native call
must therefore verify callback nesting, execution restoration and the view
it consumes together; testing its converter in isolation cannot prove them.

On terminal window retirement `wow_window_words_terminal` marks the local
original-compatible view destroyed, invokes the original control/dialog
finalizer before withdrawing identity, and rechecks enrollment after possible
reentry. `wow_user_window_retire` checks index, full handle uniqueness and
the expected object before withdrawing the view. Menu retirement checks
native `IsMenu`, withdraws copied parent views before children and preserves
detached-but-live menus; native destruction and original cleanup remain the
owners, not the copied graph. These source checks constrain a future repair:
do not replace original control finalization, destroy a merely detached menu,
or retire a new object because its short handle equals an old one. Reconcile
the existing S2/S3 test assertions for exceptional creation, nested destroy,
handle reuse and menu detach/reparent before assigning a pass or a specific
fix; this section does not establish those runtime cases by inspection.

The retained S3 control-interaction-r3 receipt supplies a bounded actual
guest witness: under each of Off/Win16/All it records zero errors, three
word callbacks and three nested callbacks, eight normally destroyed controls
and two left to exact-once task retirement; it also checks actual list
movement and both droppable combo forms' capture/release. Its subsequent
identity reconciliation checked the then-selected mirrors and package; these
were T447 P22/P23 identities, not a fresh T448 run. S3's closure separately
maps its fixed C1--C5 scope, but neither that witness nor source inspection
proves every current menu reparent, failed creation, foreign-object churn or
abnormal/reused handle combination. The old accepted common-dialog anomaly
and styled raw-fault limitations retain their own records and are not erased
by this control witness.

## M4 construction and unwind reconciliation

The historical S4 [nineteen-owner disposition](m0-t447-s4-message-source-audit.md#post-p18-complete-allocation-owner-disposition)
is the detailed denominator; it is not a list of nineteen independently
unimplemented features. Current `wmdisp32.c` and
`wow_message_lifetime.c` retain the original converters and use call-owned
captured tokens for CREATE/MDICREATE, text/map, heap outputs, temporary
clipboard/WinHelp and COPYDATA. Reverse EDIT/ListBox allocation checks and
the signed-count release pairing are present under DIV-346. These are source
observations; their existence does not by itself prove every failure path or
their execution in a given release.

The source ownership split determines finite implementation cohorts:

| Cohort | Existing owner and current source boundary | Required unresolved evidence or repair selection |
| --- | --- | --- |
| Guest callback frame and stack-based structures | Original callback stack and selected `wow_message_stack_enter/leave`; no independent stack allocation for MINMAXINFO, DRAW/MEASURE/DELETE/COMPAREITEM, NCCALCSIZE and WINDOWPOS. | Reconcile actual abnormal callback/exception return, movement and nested frame restoration against the current provider; never invent a second allocator or replay output on abort. |
| Call-owned heap/global and text/map | Original `malloc16`, `free16`, Global allocation and parameter map; DIV-345/347/349 and ADAPTER-WOW-101 retain the acquisition tokens. | Reconcile each of the nineteen named owners and reverse delegates on normal, partial construction and escaped exception; prove no output copyback or WinHelp activation on abort, and exactly-once release. |
| COPYDATA input and output capture | Original converter and CPDATA pairing plus existing captured call ownership. | Match normal, early-reply, timeout and CallWindowProc runs to exact package/mode; do not infer all structure families or sender-buffer lifetime from a COPYDATA-only pass. |
| Classic DDE protocol transfer | Original HDDE pair/list, original guest free/ACK rules and DIV-350--354 conversion-local rollback. | Reconcile all packet families and post-failure, retry, peer/task death and reentrancy before selecting repairs; generic call unwind cannot replace the transfer owner. |
| Reverse control converters | Original EDIT/ListBox and delegated converters; DIV-346 acquisition checks remain in selected source. | Preserve count and release predicates and establish exact-body failure evidence against selected source, then use real controls as integration evidence; do not replace original control logic. |

The source-exact ledger's old normal or fault-injection pass remains evidence
only for its recorded source and package. S1 must map these rows and their
applicable retained results; it must not label all exceptional cases fixed
merely because a common release function is present. Conversely, the presence
of captured release tokens is not evidence of a second allocation engine: its
implementation calls the existing original allocation/free owners and must be
judged by its actual contracts, not by its filename alone.

For the retained P22 receipt, the historical final seal lists WOW32
`ACD234099C6B9B58A9AE5846181CE85E6AA9E2E58AB88BB8D07C1F554C907F0F`,
the six message modes under three styles, cross-worker/pending-send rows,
source-exact construction negatives and separate diagnostic-only callback
fault images. `git log 29be71998..HEAD` over the selected lifetime source,
`wcall16`, EDIT/ListBox delegates and `wmdisp32`/`wmsg16` identifies one later
change, `3eae6c637`; its zero-context diff hunks in the latter two files are
confined to DDE cases. The other four inspected files are unchanged in that
comparison. This allows the old P22 results to remain bounded evidence for
the checked non-DDE source bodies; it does not prove an unchanged complete
binary, unchanged dependencies, current-release real execution, or validate
the later DDE modifications. The final T449 acceptance must identify its own
sealed package and run the applicable composed tests.

## Current worker control surface

The concrete control surface is `service.idl`'s authenticated
`VDM_CONNECTION`, process handle and generation on worker calls, with
`CloseManagementNode` for the manager and existing typed worker signals for
shutdown/Console-close. `worker_base_controls` returns server/shutdown/
Console-close handles; the worker-base layer forwards existing RPCs and
does not currently carry an HTASK-bearing command. The existing `Exit` RPC's
`wow_task` is the original BaseSrv launch/wait identifier used on worker
notification, not proof of the HTASK required by targeted termination.
`WowStarted` and `WowStyle` likewise use the existing launch task number;
their presence does not establish a targetable guest-created task inventory.

The future coherent group must extend the established authenticated RPC/control
contract to carry a worker-scoped task incarnation and its outcome, rather
than reinterpret those existing integers, send a raw TD address, introduce a
manager-to-worker connection or turn a whole-worker shutdown signal into a
task command. The service must validate the management key and worker
generation, and the worker must validate the current incarnation before
invoking any original task operation. The shared worker-base forwards the
instruction; only NTVDM/WOW32 may decide its task semantics. Exact command
delivery, publication, cancellation and completion records still require a
single end-to-end design and failure review; the existing event handles alone
cannot carry the identity or provide the task-specific completion receipt.

The existing management WOW row is specifically an admission-list projection:
`service_capture_wow_labels` binds its generated identity to a `WOWRECORD`
and its parent wait carrier, and `service_append_worker` does not advertise a
Delete action. Such an identity can prevent reuse of that *record* from
being confused with another record; it does not pin or identify a TD/HTASK.
`CloseManagementNode` currently rejects the WOW-task category, so there is no
hidden task-delete implementation behind the UI. A task publication design
must not simply reuse that sidecar as though it established the missing
worker-local binding.

The minimum outcome matrix for the future group is: (1) an accepted request
for the exact live worker/task incarnation; (2) an already-retired target or
a reused identity, without touching the new occupant; (3) normal task exit
concurrent with the request, with one retirement and no false target result;
(4) worker loss or whole-worker close, reported as such instead of successful
individual deletion; (5) requester/NTMON exit after acceptance, which must
not cancel the NTSRV-owned operation; (6) repeated requests and concurrent
distinct targets, without one process-wide hung-event signal completing the
wrong request; and (7) actual sibling task remains usable. This is an
acceptance and failure-partition requirement, not a claim that the current
protocol implements those states or that retries/timeouts are required.

The original execution entrypoints have different scopes. `W32EndTask`
explicitly requires the target application's own thread and invokes
`CallBack16(RET_FORCETASKEXIT)` against `CURRENTPTD`; calling it directly on
an arbitrary worker-base control thread would target the wrong context.
`WU32SetEventHook` similarly edits the *current* TD's guest return frame.
The original hung handler's injected `VDM_WOWHUNGAPP` keyboard interrupt is
process-global; the guest keyboard handler consumes the bit and executes DOS
`int 21h/4cff` in the then-executing guest context, not by checking the
requested HTASK. These original branches therefore cannot safely be exposed
as a general per-task RPC by forwarding an HTASK alone. Reuse requires a
verified transfer onto the selected task's own execution context and identity
recheck, while preserving guest code and the approved no-sibling/no-worker
escalation boundary. Source trace establishes why this is necessary; it does
not yet establish which existing scheduling entry can perform that transfer
without altering observable semantics.

The current selected `WK32InitializeHungAppSupport` registers
`W32HungAppNotifyThread`, not `W32EndTask`, with the imported
`xxxRegisterUserHungAppHandlers`; its `pfnW32EndTask` parameter name is not
evidence otherwise. The selected `exitwin.c` stores that pointer in
`lpfnWowExitTask`, and a source search finds no further production use of that
field in this tree. Thus this registration is not itself an existing targeted
management dispatcher or a demonstrated same-task call path. Any proposed
reuse must identify an actual reachable caller and execution context rather
than assuming a stored function pointer delivers a task-specific command.

The source-side original guest reference also rules out treating `LockTDB`
as an immediate context switch: `kernel31/schedule.asm` checks a candidate
task against `LockTDB` and refuses a switch to a different task; it also
avoids rescheduling while in DOS. The original hung path writes `LockTDB`,
then sends WOWEXEC a heartbeat and posts Escape key messages to the selected
native thread before waiting; only later does it inject the global interrupt.
This mechanism is an attempted schedule/wakeup sequence, not proof that the
target is currently executing when an interrupt is delivered. The original
`WOW16ForceTaskExit` likewise performs DOS `4cff` for the current context.
These read-only reference observations must be matched against the immutable
deployed guest and actual execution chain before choosing a target-selection
mechanism; no guest source or image is modified. A target-specific control
must not replace the required proof with a blind `LockTDB` write followed by
an interrupt.

## Original task publication and retirement chain

`wkman.c::AddProcessSharedList` registers a per-process shared-list object;
`AddTaskSharedList` first reserves an HTASK with module and thread unset,
and the later `W32Thread` call fills the module, current thread, module name
and image path. `vdmdbg.c::VDMEnumTaskWOWEx` walks the selected process's
task list and only invokes the callback for entries with a nonzero module.
This is a different original inventory from BaseSrv's launch/wait records.
It establishes a useful source for published HTASK identities, not that an
NTSRV launch record and every task have a one-to-one relationship.

The shared table is explicitly best-effort: `AddTaskSharedList` permits a
task to continue when the mapping is unavailable or its finite table is full.
Its reserved entry has no ready module or thread. Therefore an empty
enumeration, a reserved entry, or a successful launch cannot prove that a
particular live task is safely targetable. Any service-side projection must
report coverage honestly, bind an identity to the actual worker generation,
and reject stale/reused task identities; it must not treat missing inventory
as proof of task termination or pretend that a reservation is a live task.

Normal `WK32KillTask` calls `W32DestroyTask` before
`RemoveTaskSharedList`, which removes the current thread's record. The
`W32Thread` InitTask-failure and unexpected-return paths visible in this
file call `W32DestroyTask` and exit without the same explicit per-task
removal; a process-level shared-list cleanup or another path may still remove
them. This is an audit candidate, not a proven stale record: resolve those
paths and ownership before relying on list disappearance as completion or
adding another independent registry. A repository search found the
`RemoveProcessSharedList` definition and declaration but no direct call; it
does not establish that all process-death reclamation is absent, since the
shared-memory manager or another process might reclaim it. The follow-up
source trace identifies `wow32.c` startup calling `CleanseSharedList` before
`AddProcessSharedList`: the original cleanser removes a process entry when
`OpenProcess(SYNCHRONIZE)` fails. It does not walk that process's task chain
to mark its task objects available, and it does not itself distinguish an
access-denied open from a nonexistent process or protect against PID reuse.
`sharewow.h` maps a named section and serializes it with a mutex; its lock
function does not perform per-process reclamation. The section can disappear
when its last handle closes, but that does not prove cleanup while another
process keeps it open. These are concrete limits of the original publication
mechanism, not evidence that current users have hit a leak or permission
failure. Do not import the cleanser's `OpenProcess` failure test as an NTSRV
authoritative death decision; the server already owns worker lifetimes.
Also note that the original debugger
enumerator uses shared memory and callback-based observation; it is not an
authorization to let NTSRV directly control a worker or dereference its
private pointers. The eventual control remains through NTSRV and worker-base.

## Hung-task notification is not a targeted completion receipt

The original `W32HungAppNotifyThread` finds a matching `PTD` while holding
`gcsWOW`, releases that lock, then reads the pointer to select `LockTDB`.
`W32DestroyTask` removes a `PTD` under that lock after resource cleanup, and
the normal `W32Thread` TD is stack-owned. Thus a lookup result alone does not
establish that the pointer or its HTASK incarnation is still valid after the
lock is released. A future management request must perform incarnation-safe
validation and must not retain a naked TD pointer across this boundary.

The original notification event is a single process-wide manual-reset event:
each `W32HungAppNotifyThread` resets it, and every `W32DestroyTask` sets it
before `W32FreeTask`, guest cleanup and TD unlinking. It has neither task
identity nor completion generation. Consequently a signal may correspond to
another task, an overlapping request may reset another request's signal, and
even the intended task's signal precedes complete cleanup. The event remains
part of the original hung-app algorithm, but it cannot be the NTSRV receipt
that a particular requested incarnation has retired. Any additional receipt
must come from the existing service/worker control channel after verified
retirement, not from a new cross-component transport or a second scheduler.

Finally, the original hung path includes a heartbeat, a keyboard interrupt,
and user-mediated escalation to the active task or whole process. Its guest
keyboard interrupt consumes the process-wide `VDM_WOWHUNGAPP` bit. Selective
NTMON Delete must preserve the authorized task boundary; it cannot claim
success from this shared event, silently kill the active sibling, or trigger
the whole-worker escalation. This source conclusion fixes requirements for a
coherent identity, dispatch, retirement and sibling-survival group; it is not
an implementation or proof of all guest behavior.

## DDE evidence chronology correction

The late S4 ledger describes DIV-350--354 as a source candidate awaiting
delivery. `git log -S DDEBeginConversion -- src/mvdm/wow32/wdde.c` identifies
`3eae6c637` as the commit that incorporated the group; `git diff --stat
3eae6c637..HEAD` over `wdde.c`, `wdde.h`, `wmsg16.c`, `wmdisp32.c` and
`wumsg.c` is empty. T448's final cached build selected and released the
current WOW32 provider. Thus the earlier statement that this source has never
been built or delivered is stale. This does **not** upgrade old candidate
fixture results to proof for the final release: the recorded final T448
regression did not include the classic-DDE message-probe mode, and neither
the current final build nor a hash comparison proves real DDE exchange,
failure rollback or exactly-once payload retirement. The inherited real
Execute/ACK and source-body evidence retain their exact run/package identities;
the remaining M4 matrix must be reconciled by caller and ownership family.

One concrete failure-path candidate in the current source is the 16-to-32
posted DDE path: `WU32PostMessage` converts first, calls host `PostMessage`,
and invokes the ordinary unthunker even when posting fails. For POKE, DATA,
ADVISE and EXECUTE, `ThunkWMMsg16` can create an HDDE association and native
payload before that call; `UnThunkWMMsg16` has no corresponding cases, and
`wow_message_unwind16` only releases the explicitly listed text/COPYDATA
resources. The conversion-local rollback is used for construction failures,
not visibly carried to the post-result branch. This is a source-level
ownership risk, not yet a proven leak: the native packed-lParam contract,
the original HDDE retirement paths, retry via `WOWPRIVATEMSG`, and borrowed
versus newly created pairs must be traced and tested together before changing
cleanup. In particular, freeing everything on a failed post without proving
ownership can break reused pairs or cause double release. Include this path in
the finite M4 failure matrix; do not claim that DIV-354 already covers it.

The follow-up source trace separates the obligations further. ACK, POKE,
ADVISE and DATA can allocate packed native lParams; EXECUTE uses its native
payload handle directly. `WM32DDEAck` uses the reverse conversation keys and
native handle to locate an EXECUTE association and, when `fFree` applies,
honors `DDE_EXECUTE_FREE_MEM`/`DDE_EXECUTE_FREE_H16` before removing the pair.
`W32DdeFreeHandle16` normally retires non-EXECUTE aliases when guest frees
them, but for EXECUTE it records deferred free and returns FALSE. Therefore
guest-free notification alone is not a general fallback for an EXECUTE post
that never reaches a receiver and can never generate its ACK. Likewise,
`WU32CallMsgFilter` freeing a packed parameter from its own filter conversion
does not prove that `WU32PostMessage` frees a different packed parameter when
post fails. The caller also has an `ERROR_INVALID_PARAMETER` fallback that
posts the original 16-bit values as `WOWPRIVATEMSG` to a WOW destination;
its conversion acquisitions and the fallback's success/failure need distinct
disposition. `WU32SendMessage` also sets `fWhoCalled` to the POSTMESSAGE
conversion mode for legacy applications that synchronously send DDE, so a
post-failure fix must not blindly apply its transfer rules to synchronous
send or borrowed hook conversions. These are conditional source traces, not
a reproduced leak or permission to alter the normal ACK protocol.

Source identities (SHA-256) for this pass: `wkman.c`
`49BE04B3E5959F175D72B63A87239EA4F325CFDD38B051932DF2176D5527E860`;
`vdmdbg.c` `11CAD596254D09929987CF110F5B492862972D06077C5FECC100DC79B087E1D2`;
service `management.c` `A944E17AFD2E0403D1E8A2E1DC4757E9A14C776A1CF01A2FC492398B33AAD2A7`;
service `lifecycle.c` `4B9847C3399F8B05B8D37E24CE8D0CE7620030FFE2E30A9DD4755128921FB28C`;
`wow_user_task_lifecycle.c` `02068CECA2890AF3DE2A9FCE85C6FFBD259E0FA9625757D43D888BEB772576F0`;
selected `taskman.c` `FCCAB963DCDE796091AAE9972C86B874C899B81D2F91727B7081A3ACFD5A1898`.

## Remaining S1 audit

Complete the per-entry M1--M6 conversion/failure denominator, callback/object
responsibility reconciliation, and the task registration/identity/retirement
source chain against current production selections and test assertions. Then
specify dependency-ordered groups with exact original reuse, duplicate removal,
failure/lifetime rules and acceptance. Do not treat the individual-task gap as
the only open row or begin isolated repair before this audit is reconciled.

The dependency order currently supported by source evidence is: (1) reconcile
execution/callback, object and normal retirement first; (2) connect message
identity/reply and conversion/transfer cohorts with their failure paths;
(3) connect targetable task publication, worker-local termination and
NTSRV-mediated completion as one group, using the earlier execution/retirement
contract; (4) run final same-package real guest and native-direction acceptance.
This is not yet an admitted S breakdown: the remaining source and evidence
reconciliation can expose dependencies that require merging or reordering
groups. DDE protocol scope is limited to message ownership transferred into
this task; broader DDE remains with its existing candidate. A group must not
be presented as completed solely because its original implementation compiles.

## Transfer reconciliation for application and lifecycle witnesses

| Transfer item | Later evidence / current disposition | T449 ownership |
| --- | --- | --- |
| CAF close, Run16/frontend/worker authority | T448 S5 records real self-delete, external close and natural exit under its enumerated conhost cases. This supersedes T447's partial CAF status for those tested paths only. | Reuse the delivered service/worker control boundary; do not reopen CAF as an unproved WOW message feature, and do not infer all terminal-host interleavings passed. |
| COMMAND large inherited environment | T448 S13 captured an EnvSiz/guest-INIT overwrite with a host-only diagnostic; the owner declined a special workaround and retained it as a guest limitation. | Preserve the TODO/guest disposition; do not modify or rebuild COMMAND to force the WOW tests through it. Keep normal-environment DOS/launch witnesses in final acceptance. |
| WINMINE/SOL long-play exit | T448 S13 has three short runs each and a bounded synthetic WINMINE run, none proving the intermittent report gone. | Preserve the reported direct Run16 game/worker failure and capture exact process/package/retirement evidence if it recurs; short passes cannot close it. |
| SOL display/layout | The old 427x320 test-canvas failure was environment-dependent; T448 S13 measured a separate larger desktop, but did not capture the owner's original report geometry. | Do not change OpenNT geometry to force the small-canvas game to fit. Preserve the distinct owner-reported layout observation until its own geometry is established; conditional integration assertions must reflect the actual environment. |
| WRITE | T448 final package has a four-observation startup frontier; the earlier T447 illegal-instruction report is not a current proven failure or a proven fix. | Verify real editing/save/normal exit and attribute any recurrence before assigning a component or guest cause. |
| COM external peer and original guest defects | These are explicitly retained outside WOW execution in T448's closure and TODO. | Do not absorb into T449 or erase them as a side effect of WOW acceptance. |

These dispositions use the later [T448 S13 audit](m0-t448-s13-uncertain-todo-audit.md)
and [T448 S5 integrated acceptance](m0-t448-s5-integrated-acceptance.md), not
the stale T447 transfer checkpoint alone. A passing subset is not an overall
WOW-provider pass.
