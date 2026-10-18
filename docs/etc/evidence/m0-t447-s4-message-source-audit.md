# T447 S4 message source audit

## Question, inputs and proof level

Continuing from clean fd079c68c, single-person implementer/reviewer. Determine
the complete execution/receive/reply repair group before editing runtime,
not by repeatedly running a failing application. The active M1--M6 scope is
in the [working plan](../operations/t447-wow-execution-plan.md#s4-message-closure-admission-and-fixed-source-inventory).
The [existing native-boundary research](../operations/t447-native-execution-boundary-design.md)
is essential retained source evidence, with historical decisions superseded
only by the current approved modern-USER contract.

Inputs: selected MVDM wumsg.c/wmsg16.c/wmdisp32.c/walias.c; selected
ntuser/kernel/taskman.c/userk.h; existing lifecycle, runtime, message bridge,
private-access redirects, callback registration and fixtures. Read-only
canonical sendmsg.c/focusact.c/queue.c at pinned OpenNT
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c supply the private-server reference.
No original guest or new runtime source is imported/edited by this audit.
Published DLL import evidence comes from S4 source-audit-r1, not a new runtime
test. All implementation conclusions below are source conclusions unless
explicitly identified as retained S3 execution evidence.

The expanded owned source-audit-r2 (PID35064, start
2026-10-18T02:23:08.6455032Z, exit0) pins the canonical version and records
the original/current input hashes and every selected task-selection write.
Canonical SHA-256: sendmsg.c
AF68BADCB022EFCB3947968FC6E3DBF148B697072EBEB2898F97FBCB774039F3;
focusact.c AAF521712C64F0E3E9EECBF383321519E6A1F8CE46A774940A88BFF3CEE3CB8C;
queue.c 0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8;
taskman.c E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E.
These are read-only reference inputs, not new source/build dependencies.

## Confirmed whole-chain findings

| Finding | Actual source / consumer | Consequence for the finite group |
| --- | --- | --- |
| Native receive/reply is already the real resource owner | Original WU32InSendMessage calls InSendMessage; WU32ReplyMessage calls ReplyMessage. Published WOW32 imports those actual USER32 functions. | Do not author another receive/reply engine or infer native sender identity from an adapter record. Native flags/results remain authoritative. This does not establish WOW continuation handoff. |
| Outer HWND does not enumerate implicit message receivers | Native-call begin resolves only its window argument. Canonical focusact.c xxxSendFocusMessages sends WM_KILLFOCUS to the old focus before WM_SETFOCUS to the new one; NULL or caller-owned new focus does not identify that old receiver. | The repair must cover implicit callbacks and actual execution readiness, not merely explicit SendMessage targets. Do not add filename/message-tuple heuristics or special-case SetFocus as a complete solution. |
| The current call carrier is not received SMS identity | Lifecycle begin writes sender psmsSent; end calls DirectedScheduleTask using the nominated receiver. No selected adapter produces psmsCurrent. Original taskman consumes that received record for cross-process lock accounting. | Retire target-based pseudo-reply scheduling as part of the replacement group; do not fix it by assigning psmsCurrent to an arbitrary pending send. Preserve original scheduling through truthful ready/suspended boundaries. |
| HEVENT_REMOVEME does not persist native-stack exclusion | Original xxxSleepTask skips self for that one scan, then InsertTask retains its real event count. A different task's later scan checks nEvents>0, not whether the first thread's native operation returned. | A native-suspended task with pending events can be selected although its stack cannot enter guest execution. Do not clear nEvents, poll, cap time or merely add another SleepTask call. Persistent eligibility must be paired with original selection and all other selection entries. |
| Public queue wake is not an executable guest continuation | Original queue SetWakeBit respects the current wake mask; native SendMessageTimeout owns SMTO_BLOCK delivery suppression. | No extra PeekMessage to force a blocked sender to receive, and no interpretation of arbitrary queued posts as native-return readiness. Preserve timeout/result independently of reacquiring the cooperative guest CPU. |
| Same-thread and nested callbacks do not prove a new received message | Original sendmsg same-thread branch does not replace psmsCurrent; true xxxReceiveMessage saves/replaces/restores it. With the approved native-only transport, native USER already performs that distinction. | Do not create an adapter current-message stack from HWND/message equality, hook notification or callback count. Execution scopes may nest, but cannot become another receive authority. |
| Original conversion has important movement and retry rules | WU32SendMessage copies scalar arguments before ThunkMsg16 and frees flat argument references before native dispatch; ReplyMessage explicitly warns of pointer invalidation. PostMessage retries eligible same-WOW private messages on ERROR_INVALID_PARAMETER. | Retain those original bodies. M4 pairs their existing converters and lease owners through return/failure/nesting; no universal marshaller or blanket posted-pointer refusal is selected. |

The eligibility counterexample is already established in the retained design;
it is not a new runtime discovery or permission to rerun old failed probes.
The original scheduler is usable for priority/event/task selection. Its
unavailable kernel message/wait eligibility input is the boundary to adapt,
not a reason to copy its algorithm into a project scheduling loop.

## Original recovery ladder and state authorities

1. Retain the already selected MVDM conversion, callback and original taskman
   algorithms. They are actual production owners, not reference-only candidates.
2. Bind the missing native-stack eligibility/execution transitions through the
   existing bound task and LIFO call/callback scopes. Exact selection seams
   must account for the entry set below before runtime selection. A minimal
   registered selection predicate is a source-shaped adapter candidate, not
   permission to rebuild the scheduler or reconstruct a native SMS.
3. Importing full canonical xxxInterSendMsgEx/xxxReceiveMessage/_ReplyMessage
   is not composable under the approved native delivery owner: these bodies
   own kernel SMS allocation/lists, wake/reply waits, desktop objects, hook
   delivery, receiver death and private USER transport. Their unavailable
   dependency closure is not solved by retaining their names or copying a
   few branches. The earlier local-SMS/notification candidates and negative
   mixed-origin proofs remain retained, not revived as a second transport.
4. Newly authored transport/priority/runnable queues are rejected because
   native USER and original taskman already provide those respective owners.
   No adopted third-party code intrusion is needed or selected in this audit.

Authority map: native USER owns receive state, delivery, reply, native timeout
and native queue contents; original taskman owns runnable ordering and guest
execution selection; current task/native-call scopes describe whether that
stack can actually enter guest code; original converters and existing lease
owners own their buffers; existing task/window owners own retirement. A
queued event is not another authority for native-stack readiness.

## Finite implementation review set

The replacement must be reviewed as one group, not a succession of send-only
patches. These are required cases, not an assertion that the design is already
implemented or that new compatibility concessions are authorized.

| Original selection / transition | Required rule and evidence |
| --- | --- |
| Priority/event scan | Retain original ordering/counts; exclude a native-suspended stack until actual callback or native return is ready. Preserve events rather than erase them. |
| Idle-hook temporary selection | Callback eligibility must hold before any guest-capable hook, not only before CCPU simulation. No new global hook policy. |
| WOWEXEC hardware branch | Retain original hardware/event handling and its real thread context; pending hardware work is not a generic fake native return. |
| InitTask forced selection | New committed task is a genuine ready continuation; preserve creator/startup rendezvous and failure rollback. |
| Task lock | Preserve CurrentTaskLock semantics. Do not clear nTaskLock or select another task merely to avoid an application-created lock dependency. |
| Callback nesting/inherited owner | Enter before guest-capable marshalling, preserve same-thread inherited execution, restore the outer native-suspended state after copyback; all retained callback families share the boundary. |
| Native return/failure/timeout | Actual syscall completion makes that caller ready, not any inferred receiver reply. Restore execution, return value, LastError and object publication before returning to guest. |
| Retirement | Invalidate call eligibility before thread storage release; no callback into a retired task or pointer into a returned sender stack. Normal cleanup remains independent of S5's requested task deletion. |

Existing adapter callout census (native/CPD CallWindowProc, creation/failure
destruction, modal loops, controls/menus and queue servicing) must be reconciled
against current S3-selected implementations, not restored from obsolete staged
versions. The current source already repaired many earlier lifetime/creation
items; do not reintroduce the old native-dialog approximation or duplicate gate.
In particular, an eligibility flag alone is insufficient: entry must not
manufacture a gratuitous task switch for a synchronous same-thread callback,
and queued input must not revive an outer blocked native frame during nested
return. No wait-state sampling or timing heuristic is an authority for either
decision. These obligations belong to the same eight-entry review, not new
features or permission for an undocumented scheduling concession.

## Acceptance-contract correction before implementation

The old native-identity fixture requires psmsCurrent to equal the pending
local psmsSent. That is an NT4-representation assertion incompatible with the
approved modern receive/reply owner, not a valid repair target. Replace it
in the full group with public native receive facts, duplicate/invalid reply
outcomes, once-only callback/result and mixed/nested isolation. Retain the
old witness as historical negative evidence; never manufacture a pointer
equality merely to make it green.

The old early-reply rendezvous waits without yielding inside the receiver
until the WOW sender executes. It tests the message-associated NT4 handoff
whose removal the owner subsequently approved for all native transport/reply
paths. Keep native transport return and cooperative WOW execution reacquisition
as separate assertions, with actual guest waits yielding through the original
task owner. Do not report the old test as passing or silently delete timeout,
duplicate-reply, error, single-execution-owner or lifecycle assertions.

Required integrated denominator remains all M1--M6: real two-task Win16 sends
and posts, same/cross-worker and both native directions, implicit native sends,
mixed nested traffic, early/duplicate/no-pending replies, queue filters/quit,
SMTO_BLOCK suppression, converted input/output and death/failure cleanup.
Native doubles supplement but do not replace those real boundaries. No new
test is run merely to rediscover a source-proven missing producer.

## Current disposition

This checkpoint identifies the persistent-eligibility integration and removal
set; it does not certify a finished runtime design. Complete the eight-entry
selection/transition review before freezing its exact mirror budget and wiring
the whole group. No new runtime/mirror/guest diff or product build/publication
is selected here. S4 remains active, the P22 package remains deployed, and
the full WOW32 objective is unachieved. There is no user-approval blocker:
continue within the existing native transport/original scheduler boundary.

## Registered eight-entry execution group staged

ADAPTER-WOW-077 and OPENNT-HOST-123 register the current complete execution
dependency group before its mirror edit. Five predicate operands enter the
existing taskman scan/idle/hardware/locked-wake/wait-completion conditions;
event counts, original priority/list policy and CurrentTaskLock remain intact.
Fresh InitTask storage is ready by construction; no inherited frame or guest
layout field is introduced. Terminal withdrawal happens only after refusing
cleanup edges have passed, immediately before original task removal under
the same lock. Retryable unbind refuses an active native scope unchanged.

The existing lifecycle task owns a borrowed LIFO frame pointer, never another
task registry. Native call frames are ineligible, real callbacks temporarily
publish ready frames, and actual native return reacquires entry-owned
execution through original DirectedYield. Synchronous same-task/inline calls
retain inherited execution; an actual different-task callback may park an
owner already in a native frame through the original nonblocking SleepTask
branch. That branch cannot wait or invoke a guest idle hook in this use.
No source or destination message identity is inferred by this transition.
Old receiver-id/pseudo-SMS/psmsSent/native-return pseudo-reply code is removed,
not retained as a fallback. Native USER InSendMessage/ReplyMessage stay direct.

The staged callout group includes inline Peek/receive, enumeration and next-
Hook dispatch, real MsgWait and native MessageBox/ShellAbout; original modal
dialog/control/converter bodies are unchanged. Existing object publication,
return/error and exception cleanup remain at the common owner. Original
resource-only pre-InitTask queue creation retains its separate data-lock pair.

Authored tests add persistent eligibility across different task scans without
discarding events, task-lock self wake, idle/hardware exclusion, actual nested
native frames and unbind refusal. Native fixtures now enter the actual common
callback scope before invoking WOW services; private-SMS equality is replaced
by native receive flags and zero fabricated associations. Early/duplicate
reply separates transport completion from an actual cooperative product wait,
retaining first-reply result and cleanup assertions. These are staged inputs,
not new passing tests or full real Win16 message acceptance.

The entire dependency group is now ready for its first grouped compiler and
native/original/guest acceptance. No partial candidate is published. Full S4
M1--M6, including real two-task mixed-message proof, is still required; this
implementation group does not shrink the goal or add a semantic exception.

### Renewed installed WINMINE startup report

The owner again reports very slow `run16 winmine`, now apparently hanging
compared with the earlier black-and-white implementation. Prioritize this
diagnosis before expanding S4; do not infer that the uninstalled S4 candidate
causes the installed symptom or discard the pending execution work.

`build/M0-T447/S4/installed-mine-regression-r1.ps1` runs hidden, owned,
serial installed-package observations with a360-second supervisor bound.
Supervisor PID28852 exits0. Production NTVDM and WOW32 still match the P22
hashes in Current and assets/release. No project process was present at the
initial identity inventory. No package/profile/guest or released file changed.

First, `verify-wow-logon-gameplay.ps1 -Installed` uses the full supported
CreateEnvironmentBlock for the current token, inherit=false, without filtering
or replacing PATH. It records45 entries/3349 UTF-16 code units including entry
terminators and final terminator. Off, Win16 and All pass real owned-window
flag/reset/open pixel changes, WM_CLOSE and run16 normal exit0. Probe-local
window-ready times are4266/4859/4953ms, followed by board-ready1781/1813/1860ms.
These are phase times, not exact user's shell-to-frame measurements or an
acceptable-performance declaration. The unstyled Off case remains native
frame=1; the two style cases report native frame=0.

Second, the same installed images and Console-owning observer run
`observe-wow-frontiers.ps1 -VisibleDesktop -WaitTarget -Guests WINMINE.EXE`
with inherited developer environment. Supervisor-start shape records78 entries
and5983 characters; this shape is not a byte-exact measurement of the eventual
guest environment, since the test/observer have their own process variables.
At4195ms, and still at8185ms, the worker has the error text
`CS:05e3 IP:20c2 OP:63 61 6c 5c 54` and no Minesweeper main window. The observer
eventually reports `result=timeout`, exit0x53504354. The frontier runner's exit0
means observation/cleanup completed, not successful guest startup. Error HWND
visibility is0 in this hidden observer run; do not claim the owner necessarily
saw that dialog. Cleanup is pinned to the test's installed image paths and
process identities, not a normal-exit assertion for the failing case.

This establishes an environment-dependent failed startup alongside passing
complete-logon controls, not a universal product freeze. It is consistent with
the already source-proven immutable COMMAND EndInit/EnvSiz discarded-INIT
limitation retained in TODO and the S3 writer audit. This new run does not
recapture that writer, inspect the owner's actual CMD environment or bisect
the first bad binary. Those limits prevent declaring the owner's particular
failure causally solved. No guest patch, environment truncation, workaround,
new production fix or publication is claimed.

### Execution-frame ownership review during grouped acceptance

Execution-build-r1 and its r2 no-work refresh exit0. Acceptance-r2 passes
original task-order/persistent eligibility and native identity/reply/seen
before starting the retained full native cohort. That cohort exposes a
source-level ownership mismatch in the staged077 implementation: resource
fixtures legitimately bind an original THREADINFO projection without creating
another lifecycle-private task. Native begin now requires current_task from
that private list and terminates before the removed-message assertion. The
retained rows remain failures, not omitted or weakened assertions.

The existing runtime_bind contract accepts a bound original thread independently
of private lifecycle allocation. Execution readiness describes that bound
thread's stack, not its allocator. Therefore the finite correction is to keep
the single borrowed LIFO top on the existing wow_task_order_thread carrier.
Both real original task selection and native/callback scopes consume that same
field directly. Remove the added host readiness vtable leaf, lifecycle-private
duplicate pointer and current_task lookups introduced for this purpose. No
second task record, test-only production branch or fabricated InitTask is
needed. Real task retirement still withdraws the borrowed top only after all
refusing cleanup edges, under the existing owner lock. Native resource-only
bindings without an original pwpi own no cooperative execution to release;
original TIF_16BIT task bindings retain the actual scheduler prerequisites.

This is the same077/123 registered execution boundary and unchanged five
original selection operands, not another transport, callback policy or semantic
waiver. The current frozen cohort completes before these source/test-input
edits; then the entire affected group is rebuilt and accepted together. No
failed candidate is published and actual Win16 M1--M6 proof remains required.

Acceptance-r2 finishes the retained20-case native cohort with10 failures and
10 passes; it stops the outer acceptance before original/guest floors, exit1.
All old evidence remains. Source review additionally identifies native-nested's
WM_APP+71 test as a terminal ExitThread from inside its callback, but its direct
runtime binding never sets production session_finish's terminal intent.
The fixture now supplies terminal_exit explicitly before unbinding; ordinary
active-frame unbind refusal remains required and unchanged. The model still
performs real thread exit during a native SendMessageTimeout and retains
receiver death, sender return/error/owner and retained-borrow assertions.

The whole correction is implemented after that terminal run: readiness is
directly on the bound original-thread carrier, the extra host vtable and
private-task pointer/search are removed, and the scheduler fixture drives the
actual shared readiness predicate rather than a mock host readiness leaf.
The original five selection conditions, counts, priority/task-lock algorithms
and modern message owner are unchanged. Full replacement-group verification
is pending; no passing result or new publication follows from these edits.

Execution-build-r3 passes with candidate WOW32
D53DE4E497F508C7A268CF8C6AD5330D8394F95B0177AEDC2665575F13C90C4B;
worker stays P22-identical. Acceptance-r3 passes19 of the20 retained native
rows, including native nested/receiver-terminal exit and original modal/style
prerequisite. Graph-negative alone rejects held=1 property/enumeration callback
locking. Source attribution is finite: that fixture intentionally has no
original TDB, so native_call_begin's old pre-InitTask early return skips the
lock-release that the replaced enumeration-local pair used to perform.
The newly shared execution call boundary must distinguish data locking from
the optional original task, not skip both because no TDB exists.

The correction now pairs caller-held data locking for every bound runtime;
only a committed original task gets a stack-readiness frame and scheduler
transition. Actual native return/unwind restores the entry lock state. Existing
graph-negative held=0/1, atom/string/signed-HWND/stop/exception/removal/invalid
assertions are retained. Queue creation uses this same common branch, deleting
its redundant local pre-InitTask lock-release/restore pair. There is no fabricated
task, skipped callback assertion or new original-mirror diff. Full rebuild and
whole-group acceptance are required again. The message-guest-group-r1
supervisor correctly refuses to start its runtime because acceptance-r3 failed.

Independent actual USER16 message inputs are now authored separately:
win16_message_probe.asm, wow_message_peer.c and verify-win16-message-probe.ps1.
They cover two real same-worker Win16 tasks, nested actual/native sends,
early/duplicate reply results, post versus received state, queue filtering,
no-remove/removal/thread post/quit, bidirectional text conversion and original
task retirement. The native peer records real PID and distinct thread identities;
it supplies no task/scheduler/converter double. These are unverified authored
inputs, not accepted capabilities. Cross-worker/external-native-process/bitness,
implicit focus, full converted structures, SMTO_BLOCK and pending receiver
death remain in the unchanged M1--M6 denominator. No guest or publication changes.

### Actual-message witness source/owner review

The new witness is one complete same-worker/native dependency group, not a
replacement queue test. MSGSEND uses original KERNEL WinExec to admit MSGRECV;
USER16 GetWindowTask records distinct16-bit identities. The loaded independent
MSGHOST peer checks both guest HWNDs belong to its actual worker PID, records
their distinct actual native threads and its third native thread, and uses
only native USER delivery. It never binds a fake WOW thread, supplies callback
conversion, reads private structures or calls the adapter scheduler itself.

| Fixed group | Actual selected chain exercised by these inputs | Still not established by these inputs |
| --- | --- | --- |
| M1 | USER16 SendMessage/PostMessage/PostAppMessage/GetMessage/PeekMessage/DispatchMessage -> original wumsg and message converters -> native USER. Repeat PM_NOREMOVE, one removal, exact filters, task post and filtered WM_QUIT. | Cross-worker and broader queue/order/external-native boundaries. |
| M2 | Original InSendMessage/ReplyMessage in actual16-bit window procedures, nested same-thread receive-state inheritance, first versus duplicate reply result and actual native peer sending back into the pending Win16 sender. | Implicit focus receiver, external native process and all mixed nesting forms. |
| M3 | Real two-task WinExec admission, original Yield inside reply processing, product native/callback scope and original taskman; pure native peer never becomes a WOW task. | Every lock/wait/timeout/death permutation in actual guest code. |
| M4 | WM_SETTEXT/WM_GETTEXT input/output through both Win16 task conversion and native-to-Win16 conversion; exact strings/results asserted. | Full structures, movement, timeouts and exception buffer-retirement closure. |
| M5 | Normal receiver window/task retirement, dead HWND failure, sender cleanup, exactly one original retirement record per actual16-bit identity, native peer thread/window exit. | Pending receiver death/cancellation and every failed converted call. |
| M6 | All three style policies against the coherent candidate on a private unswitched desktop, complete supported logon block, independently authored immutable inputs and normal launcher completion. | This bounded group does not replace retained S3/application/DOS/ordinary-desktop floors or the rest of S4. |

Input/lifecycle review: NE procedure thunks and classes retire after their
windows; the peer DLL is freed only after its native thread has exited. Test
reports contain result counters, task/native identities and environment shape,
not environment values. Compiler outputs stay under build; runtime probe copies
stay in the owned short-root stage's tests directory. Existing package cleanup
pins actual paths and identities; forced cleanup cannot establish a normal
exit. The run's retained limitations explicitly distinguish the known original
large-environment failure from complete-logon-block acceptance.

Execution-build-r4 exits0. Candidate WOW32 is
B56BFBD8D9705BF626104F0FDC142A4C4D2ACBD20125E7ABC8B82E969BD8779E;
NTVDM remains12DA2E8FCFFB35B724791F7356C1A9A87A5208442928C29127415AE8DC812705.
Acceptance-r4 is running in its owned hidden supervisor PID32500, with full
original/native/guest floors selected. Message-guest-group-r2 PID25984 waits
for that exact supervisor identity and success before starting its own serial
runtime group; build and probe source hashes are checked before/after.
Neither an active process nor these source-review conclusions is a test pass.

Acceptance-r4 subsequently passes the full20-case native/real-CCPU cohort and
both-width style boundary. The original30 boundary-fixture build then fails
LNK1120 only in wow_dialog_modal_entry_fixture: its whole wow_title_policy
object retains the unused ShellAbout/MessageBox native-call functions. This
does not indicate a production DLL link/runtime failure or a passed fixture.
New-T447S3CallBoundaryNinja now selects function-level COMDATs for that object
and OPT:REF for this isolated modal/context link. Used original modal/context
and all existing assertions remain; no fake successful native-call service,
test removal or production behavior change is introduced. Validate this whole
30-case fixture group and the remaining real-guest floors afresh. Reuse only
already-passing native/style rows whose exact source/artifact identities remain
unchanged; do not rerun them simply because a different fixture's link changed.

The source-led outgoing-call review also identifies a further finite M3/M4
dependency: original wkgthunk WK32ICallProc32 and wmmedia WMM32CallProc32 enter
caller-owned native code, while original wcall16 WOWCallback16/Ex and exported
WOWYield16/WOWDirectedYield16 form the return/yield directions. The original
wkgthunk comment explicitly relies on no16-bit memory movement during the
native thunk unless it calls back. Therefore blindly wrapping every extension
call as a descheduling native wait would change that original contract. Audit
the complete native-extension/callback/explicit-yield and pointer-fault return
closure together before choosing the smallest binding. This is already part
of fixed S4 execution/conversion scope, not new feature admission or authority
to alter original generic argument order, callpr32 stack cleanup or memory
movement. No extension-boundary replacement has been implemented or verified.

The function-level COMDAT/OPT:REF fixture-link attempt still reports the same
three unresolved native symbols (acceptance-r5, exit1), so that attempt is not
retained or described as successful dependency selection. The corrected unit
composition instead links wow_modal_native_unreached.c solely into the isolated
modal/context executable. Every entry immediately reports and exits97; it
cannot fake a native success or satisfy the unit if a native-dialog path is
actually invoked. This follows the unit's existing explicit original-manager/
execution doubles and claims no native acceptance. The actual native/CCPU and
guest gates still require the production owner, never these sentinels. The
new file is not selected by any of the old native20 compile/link commands or
the production manifest; all previous source/artifact hashes remain required
before reuse. No production source/image change is needed for this test-only
link repair. Complete original30 and fresh real-guest rows remain mandatory.

### Verified message baseline and finite identity/retirement correction

Acceptance-r6 completes original30 and all12 actual S3 guest groups, with
explicit hash-checked reuse of acceptance-r4's passing native20/style and
focused message/source rows. No failed row is reused. The production image
remains build-r4 B56B..., with no new publication. Message-guest-group-r4 fails
NASM before any runtime (% adjacent to512 parsed as a macro token); r5 corrects
that lexical error and runs the authored message group. The probe's DOS header
also has an extra zero word, placing40h in e_ovno rather than e_lfarlc. It still
loads as NE, but that authored input defect must be corrected, not retained.

Actual r5/Off sender phases0--3 have zero assertion errors: same-worker nested
Win16 messages, native peer both directions, first/duplicate reply results,
post/thread-post/no-remove/filter and text copyback execute. Receiver reaches
phase6 with zero errors, but its module cleanup rejects status6 and aborts the
worker (launcher0x42b). This is failed lifecycle acceptance, not a pass for the
whole group. GetWindowTask also reports927 (WOWEXEC fallback) rather than the
receiver's self-reported6031; sender identity is6079. The unchanged final
cross-record identity and retirement assertions remain mandatory.

Source trace identifies two already-present, but unconnected boundaries:

1. Original WU32UnregisterClass calls public UnregisterClassA, bypassing the
   already selected wow_class_client_unregister -> original ntcftxt.h ->
   original _UnregisterClass -> class storage owner. Native removal succeeds
   while its owned class-list/backing record remains; later module cleanup
   encounters that stale registration. Reconnect the existing original client,
   including its original captured-name/menu-free ordering; do not implement
   another removal algorithm or suppress cleanup failure.
2. Original WU32GetWindowTask calls GetWindowThreadProcessId on HWND32, which
   walias.h deliberately sign-extends from16 bits. Canonical winmgrc.c uses
   private ValidateHwnd/PTI/shared data unavailable on this host. The existing
   original handles.c GetFullUserHandle already supplies the missing alias
   input. Normalize there, then use native USER as the sole actual PID/TID
   authority; preserve ThreadProcID32toPTD and WOWEXEC fallback for real
   non-WOW windows. Do not substitute the lifecycle's task list.

Native-handle-contract-r1 is an independent owned x86 native-API diagnostic,
not a WOW capability pass: full HWND035311F6 returns TID16320/PID12624; unsigned
and signed short11F6 both return TID0/error1400 and leave the PID sentinel
cccccccc unchanged, although IsWindow accepts each. This directly verifies
the missing host input and argues against blanket rewriting of all HWND APIs.
Canonical winmgrc.c SHA256 is
26F0174E6A0DB53D942506CBDC12EFBFEBC34A943D780170E7BF9D95775514A4;
ntcftxt.h is E7F3D42CE8939BBE0B14EA34BC693107334C19F8B2D7862EC15B75A501D11DAB
at the pinned OpenNT revision. Its invalid ValidateHwnd branch returns before
writing PID too, so do not invent a failure-output overwrite.

Register ADAPTER-WOW-078 before implementation. First usable reuse is the
already selected complete class client and original full-handle algorithm
plus a finite public native query. Reintroducing NT4 PTI/shared native heap
for its trivial fast path would duplicate modern USER's real identity owner;
no imported algorithm is replaced by another task/identity registry. No
additional original/guest diff is needed. Pair original public unregister,
normal/module retirement and post-native failure with real USER16 identity
and native unsigned/signed/full/dead query assertions as one bounded group.

The078 source review also pairs the irreversible-release branch with an
owned negative child: register a native class through production class storage,
publish its real WND/class association, destroy the native WND while deliberately
retaining that test publication, then enter production native unregister.
The original page-domain owner must reject the stale class projection after
native removal; the existing worker-failure owner must terminate, never return
an ordinary retryable failure. The parent requires the actual child exit,
pre-entry event and production traces for both native-unregistered=1 and
release-class-after-native-unregister. No guest modification, production fault
hook or cleanup bypass is involved. Full/signed/unsigned live/dead thread-query
rows preserve native PID output, last error and inherited lock state. These
are source/assertion dispositions, not runtime passes until the fresh group
finishes. The old r4 package remains immutable and unpublishable as the current
source078 candidate.

### Native-extension boundary: resolved source composition

This is the next finite M3/M4 dependency group, not additional T scope and not
yet a runtime implementation. Pinned original hashes: wcall16.c
4875844ABFB8D22025480C14174481A194779D83759CE3A696E978B68C55A9F0;
wkgthunk.c787DFD9D5135B71CDFF6CA2BCC8145BD3EDF5E2EA26B7E9C87B14176F82EFEEC;
wmmedia.cFBC4695791937FEA487C91A0BA964E19625EBF4793C0123238100628FA95FD08.

| Complete entry family | Retained original owner | Required bounded seam and rejected alternative |
| --- | --- | --- |
| CallProc32W/ExW, multimedia native thunk | WK32ICallProc32 / WMM32CallProc32, original callpr32.asm argument copy, order, convention and stack recovery | Inline native execution scope retains inherited execution; only actual callbacks/yields can move guest memory. Per-unit entry renaming can preserve these bodies and the byte-exact assembly. A descheduling wait around these arbitrary callees violates the explicit original memory contract. |
| LoadLibraryEx32, FreeLibrary32, GetProcAddress32 | Existing wkgthunk bodies, including x87 fsave/frstor and pointer resolution/free order | Per-unit underlying native API aliases let the scope enter at the actual native call, inside the original FPU protection. Do not move scope work before the original fsave or replace the loader/argument conversion. |
| All original CallBack16 consumers, including same-unit memory helpers and public WOWCallback16/Ex | Original CallBack16 performs guest frame preparation, original CCPU reentry and return restoration; original public Ex argument validation remains | Acquire READY execution before original GETFRAMEPTR/CBFRAMEPTR, pair on exceptions and return. A definition-only rename plus one original-shaped adapter entry is smaller than wrapping20 same-unit public/internal callers. Whole-unit CallBack16 renaming alone is rejected: it also renames those20 calls and bypasses the adapter. Gating host_simulate is too late, after guest frame writes. |
| Explicit native WOWYield16 / WOWDirectedYield16 | Original WK32Yield / BlockWOWIdle / DirectedYield pair | READY callback scope encloses the original yield and restores the outer inline-native state. No second scheduler or invented runnable event. |
| Fault/resource reentry and teardown | Existing original force-segment callback and wow16cal.apfnWOW16Func; existing early resource-cleanup owner | Review the actual RET target, not a numeric range or a claim that every callback is a resource helper. Terminal application callbacks remain rejected; trusted memory/resource operations require the already-live early cleanup scope. Never make public WOWCallback16 an arbitrary-code exception during teardown. |

The guest-side wow16cal.asm table is read-only source evidence: it explicitly
distinguishes application procedure dispatch, foreground-idle/winsock hooks,
task exit, kernel memory/resource operations and ForceSegmentFault (IsBadReadPtr).
No guest build/edit is required. Existing callback guards outside CallBack16
remain necessary: window/dialog/message conversion can touch guest state before
reaching that central frame builder. This composition cannot remove those
earlier boundaries merely because a central guard is introduced.

Review must pair real native-extension callbacks/yields, argument/result and
error preservation, invalid public Ex arguments before guest entry, exceptional
return, cleanup refusal and real application floors. Existing MSGHOST's native
thread/message witnesses do not establish exported WOWCallback16 or explicit
native Yield coverage. These remain unverified, not inferred from successful
generic loading or a native-message pass. Exception registration and complete
group source/tests precede any next runtime build.

### Fresh078 build and native acceptance

Execution-build-r5 completes with WOW32
F3FD174A25B72EC17274979B6F24A1C5C5314A9E025BD233847356F9F45680F2;
NTVDM remains the published12DA... identity. Execution-acceptance-r7 runs the
entire fresh group without reusing r4 native results. Its native-core summary
records20 cases, zero failures. Graph-negative logs contain all four
WOW_WINDOW_THREAD_QUERY_CCPU held0/1 and dead0/1 rows, including full/unsigned16/
signed16/native-output assertions. The owned class-failure child records
ClassRetirement native-unregistered=1, then the exact
release-class-after-native-unregister status5023; the parent verifies
STATUS_ASSERTION_FAILURE c0000420 and the expected production traces.
This proves the deliberately stale-publication terminal boundary, not that
ordinary unregistration should fail.

The two-width style boundary also completes; original30, remaining real S3
guest floors and actual-message group-r6 are still pending at this checkpoint.
The latter pins supervisor43396/start2026-10-18T03:50:14.8492640Z and requires
success before starting. Product-gate-r1 pins message supervisor29848/
start2026-10-18T03:50:33.9078295Z, requires its success, then selects the retained
WINMINE/SOL/WRITE and Console17/Window17 floors. These are serial prerequisites,
not passing runtime claims. Execution-review-r2 passes documentation governance,
diff whitespace and selected PowerShell parser checks. No publication or new
commit is claimed; guest/profile/ZIP and the installed coherent13 remain intact.

Acceptance-r7 subsequently exits0: original30, resource guard, MENU and all12
actual S3 guest groups pass afresh, not via reused native evidence. Actual
message-guest-group-r6 also exits0 for Off/Win16/All. Each mode records sender
6079, peer lookup6031 and receiver self6031; all guest/native assertion counters
are zero, sender nested2, native received4, destroyed1 and observer exit0.
The runner separately requires exactly one original complete/status0 retirement
per actual task in the verified native worker. This corrects r5's927 fallback
and failed module retirement through the selected original owners; no assertion
or required phase was removed. WINMINE and SOL gameplay/normal close pass all3
policies in product-r1; WRITE startup, DOS and ordinary-logon groups are still
pending at this checkpoint. Full S4 external-process/cross-worker/implicit
message/converted-lifetime and native-extension obligations remain open.

### Final review: direct native-modal coverage and restored baseline

Product-r1 completes all6 WINMINE/SOL gameplay cases, WRITE's four startup
samples per policy and Console17/Window17 with zero Expected/Actual mismatches.
Ordinary complete-logon gameplay passes3 policies (window-ready5156/5875/5750ms,
then board-ready1032/1000/1234ms; phase timings, not a shell-to-first-frame or
performance-improvement claim). Delivery-r1 backs up and publishes coherent13,
hash-verifies both destinations and passes deployed-style plus all3 installed
complete-logon gameplay cases. Neither publication nor recovery closes an
existing owner session: both stopped-process inventories are empty.

The second-role review then identifies a missing direct assertion: changed
wow_title_message_box_a / wow_style_shell_about_a pair native execution scopes,
but original30's isolated modal unit explicitly cannot enter those native
paths. Add independently authored win16_native_modal_probe.asm and the native
dialog peer/runner. Actual USER16 MessageBox ordinal1 and SHELL16 ShellAbout22
must return IDOK/success1 into guest checkpoints0/1/2, then retire exactly once.
Sources are original wudlg.c::WU32MessageBox, wshell.c::WS32ShellAbout and
wow16/user/user.def, shell/library/shell.def; no original guest changes.

Native-modal-group-r1/Off fails: peer exits8 after one posted OK and a5-second
close deadline; guest report remains one14-byte phase0 record and observer
reports timeout0x53504354. The observer process exit0 is not guest success.
R2 adds visible/enabled/responsive prerequisites and fails peer6 (no ready
dialog in20sec). A pinned read-only snapshot before watchdog cleanup shows
the real MessageBox/OK and WOWEXEC windows; it does not prove their earlier
readiness or modal-loop progress. R3 aligns the new peer with established
gameplay's DESKTOP_WRITEOBJECTS/SetThreadDesktop protocol, logs actual ID1 and
enabled states, but still lacks a completed return. These are failed witness
runs, not accepted runtime rows or proof of a specific product root cause.

Restored-execution-r1 validates all27 recovery targets/backup hashes and
restores the exact previous coherent13/manifest at both destinations. Published
WOW32 is again3D2CEB931C6E43E097FA72185049B626332F346407A94A007ADDB7259411D3A9;
F3FD... remains only in the sealed candidate. No commit/push or guest/profile/
ZIP change follows the rejected supplemental gate. The independent
native_modal_control.c now exercises the same two public native APIs, captions,
buttons and desktop/peer protocol without WOW/style/adapter code; its owned
run must distinguish native-host/probe prerequisites from the actual adaptation
before any runtime correction is selected. Do not weaken the guest return or
retirement assertions, or infer a cause from merely seeing the dialog.

### P2 native-modal proof, final delivery and second-role review

Native-control-r1/2 are failed probe-protocol controls, not proof that native
USER is broken: this host's MessageBox does not expose a direct IDOK HWND and
does not honor the assumed internal WM_COMMAND path. Native-control-r3 uses
the public WM_CLOSE route and records MessageBox=1/ShellAbout=1 plus normal
exit; native-modal-group-r4 uses exactly that verified protocol on the sealed
WOW candidate. All3 policies pass: actual task6079 records phases0/1/2,
message=1/about=1/errors=0, both real dialogs close in the same actual native
thread, launcher returns0 and original task retirement completes exactly once.
No product patch was needed for these probe failures. This proves native modal
entry/guest continuation/return and retirement, not physical OK-button input,
all native control layouts or remaining mixed-message families.

The reproducible pure-native reference is now
tests/observation/verify-native-modal-control.ps1 with mandatory Observer,
CompilerDriver and fresh build-owned BuildRoot. Native-control-r4 passes that
committed-form runner, explicitly x86/MT/O2; native_modal_control.c and the
shared wow_native_modal_probe.c never load WOW/style. The actual guest runner
is tests/observation/verify-win16-native-modal-probe.ps1 with PackageRoot,
Observer, CompilerDriver, the actual-message group's LogonHelper assembly,
fresh BuildRoot and WindowStyle Off/Win16/All. Each root records exact inputs,
image hashes, real owned processes/deadlines, guest returns and retirement.
Both runners keep failed native or observer outcomes distinct from success.

Delivery-r2 requires the added direct-modal and native-control gates, repeats
coherent13 backup/publication/hash checks, then passes deployed style and all3
installed complete-logon gameplay cases. Both destinations again contain
WOW32 F3FD174A25B72EC17274979B6F24A1C5C5314A9E025BD233847356F9F45680F2;
the other12 images retain their previous hashes. No existing project session
is closed. This supersedes the temporary recovery, not its retained evidence.
Original guest/profile/ZIP remain unchanged; the owner's particular inherited-
shell WINMINE failure is not claimed solved by complete-logon controls.

Second-role scope review: original taskman retains counts/priorities/list/lock
and wait policy; its only new mirror mechanics are five registered persistent-
readiness predicate operands/local comments (OPENNT-HOST-123).077 removes the
independent nominated-target/psmsSent pseudo-identity, rather than retaining it
beside native USER.078 reuses the already imported original unregister client
and full-handle algorithm; no additional mirror/guest diff, registry or native
private object decoding is introduced. Selected-source audit reports278 files,
192 byte-exact/86 registered content differences and zero gaps. This is not an
added-file/diff count or proof of untouched branches. Native extension/callback/
explicit-yield composition remains the next finite, source-defined S4 group;
cross-worker/external-native/implicit/converted-lifetime coverage and S5/S6 are
not closed or transferred to manufacture P2/T completion.
