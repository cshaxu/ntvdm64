# T447 S1 execution baseline

## Question, inputs and method

Does the inherited early-reply handoff failure still exist in current source,
and what do the three ordinary applications currently reach? Owner requests
single-person implementer/reviewer execution toward working WOW32. This is
S1 baseline evidence, not complete WOW32 or S1 closure.

Repository baseline is70da27e83; runtime remains accepted ca0abb7c4.
build/M0-T446/S1/package is checked against all13 release-manifest identities
before staging. Original SYSTEM.INI and guest inputs are unchanged. The
background runner baseline.ps1 records PID26476, a900-second bound, and
separate outputs below build/M0-T447/S1. observe-wow-frontiers.ps1 uses the
retained console-startup-observer and worker-window-snapshot on an unswitched
private desktop with WaitTarget. Its explicit cleanup is not normal guest exit.

## Current app evidence

frontiers completed in65007ms. Four timed window observations per live app
are retained under frontiers/current-*-windows.txt:

| App | Actual current visible content | Interpretation |
| --- | --- | --- |
| WINMINE | Minesweeper main window | Startup retained; this particular observation does not exercise gameplay. T446 gameplay remains separate accepted evidence. |
| SOL | Out of memory dialog | Initialization remains broken; not proof that allocation is the first failed provider. |
| WRITE | Not enough memory for Write to complete this operation | Initialization remains broken; no normal document window established. |

These independently reproduce the known application frontiers. No dialog
text is treated as a provider diagnosis. The first failed initialization
provider still requires evidence, with W03 the inherited diagnostic receiver
and explicit promotion if a different owner is proved.

## Finite execution matrix

| Stable row | Production caller/binding | Existing test and evidence limit | Required next proof |
| --- | --- | --- | --- |
| C01 send/reply/reentry | mvdm/wow32/wumsg.c WU32SendMessage/SendDlgItemMessage/ReplyMessage; wow_user_private_access.h; wow_user_task_lifecycle.c native call begin/end | verify-wow-task-lifecycle.ps1 Case native-send/native-reply/native-foreign; real native USER with test task setup, not real guest callbacks | Current early-reply failure attribution, both Win16/native directions, duplicate replies and real guest execution ordering. |
| C02 blocking receive/wait | WU32GetMessage/PeekMessage/WaitMessage -> WOW task lifecycle message/wait bindings | Same fixture and existing lifecycle cases are partial; current frontend observations do not prove cancellation | Native-only target and modal wait, timeout/cancel, sender/receiver loss and execution restoration. |
| C03 callback frames | wow_user_registration_bridge.c and wow_window_words_binding.c -> wow_task_callback_enter/leave; original callback/CCPU path remains owner | Current fixture covers task handoff, not all real CallBack16 frame/lease operations | Recursive guest entry/return, exceptions/destruction and exact frame/lease release. |
| C09 init/yield/hung | wow_user_task_lifecycle_initialize/register/init_style/yield/directed_yield/wait and selected original task-order algorithms | Case task-order; historical E93 source-order spies remain mock evidence | Real multiple-task initialization/yield and hung registration/failure behavior. |
| C10 execution cleanup | Callback unwind and task/thread retirement at existing lifecycle owner | Existing lifecycle fixture plus original S3 checklist; app observer forcibly cleans owned package | Interrupted waits/callbacks, task loss and exactly-once cleanup without retiring another live task. |

Read the inherited T422 S3 checklist for every normal/failure/cleanup case;
these rows do not collapse its unchecked obligations into a pass. W02 object
semantics and W06 dialog/input behavior remain distinct consumer ownership.
Every future implementation delivery still requires C11 edge/diff review
and C12 actual package build/regression/publication.

## Source observation and probe status

Current WU32ReplyMessage still calls ReplyMessage directly. The private access
header binds SendMessage/SendMessageTimeout and Get/Peek/Wait, but no reply
wrapper. native_call_end still resolves the receiver, calls
DirectedScheduleTask then xxxSleepTask before restoring psmsSent. This is
consistent with inherited E92, not by itself current runtime proof.
The callback boundary must not obtain fake receive identity from an outer
native-call record; modern USER remains the only message transport.

The first fixture attempt failed because environment sanitization removed
Ninja from the outer PowerShell PATH. VsDevCmd's subprocess could build
objects, but the following direct Ninja command could not run. This is a
test invocation failure, not a product failure. fixtures.ps1 waits for that
runner to end and passes an explicit resolved Ninja path; it uses retained
S6/provider and S6/r003 caches without changing released binaries.
The already-started runner has terminated (fixtures.exit is0, indicating
runner completion only). fixture-results.json records native-send passed
(67234ms), native-reply failed (53822ms), native-foreign passed (47618ms),
and task-order passed (51238ms). Bounded review of r2-native-reply.failure
and r2-native-reply.txt confirms exit95: early=0 returned114 normally;
early=1 reports native transport return114, then WOW_NATIVE_REPLY_STALLED
with native_returned=1, scheduled/owner equal to receiver and current=NULL.
This matches the inherited handoff boundary, not a guest acceptance result.
Link /FORCE parent substitutes and native task fixtures are
not real guest acceptance. No original or project production code changed.

## Source-first implementation inventory, initial pass

This pass reads the selected production bodies at70da27e83, not fixture
substitutes. It is a partial inventory, not S1 closure or a complete repair
design. sources and New-T404S5Wow32ProviderNinja.ps1 select the lifecycle
and message bridge; registration_bridge publishes the actual callbacks.

| Edge | Source finding | Disposition |
| --- | --- | --- |
| C01 reply and receive identity | wumsg.c WU32ReplyMessage calls native ReplyMessage; private_access has no reply binding. lifecycle native_call_begin sets only sender psmsSent and two peer pointers in a stack record. No assignment to psmsCurrent exists in current src C/H files. taskman xxxSleepTask reads psmsCurrent and reply/receive flags for its cross-scheduler handoff. | Confirmed missing inputs to that original branch, not a missing function name. A reply wrapper alone cannot supply legitimate receive identity. Preserve native transport; do not populate psmsCurrent from the outer-call record. Design send/receive/reply ownership together. |
| C01 external native peer | native_call_begin transfers execution only when the HWND maps to an enrolled WOW task. Unknown/native targets retain execution and only release USER data protection. | Present deliberate branch, not complete cross-native semantics. Trace induced callbacks and nested returns before deciding its required adaptation; do not equate dropping the data lock with yielding guest execution. |
| C02 queue wait | wait_for_task_or_message observes the native queue, records runnable events, and returns to original taskman. message_bridge receive uses non-removing native PeekMessage and never creates an SMS receive list. | Existing production route; not an unimplemented wait API. Queue wake cannot provide the missing C01 receive identity. Audit cancellation/termination separately. |
| C03 callback lifetime | window_words_binding dispatch and registration_bridge tagged CallWindowProc enter task/borrow scopes, use finally to restore callback page view and release borrows, then leave task scope. | Existing scoped route, not grounds to add another callback manager. Continue original CallBack16 and exception/unbind audit; ignored restoration return values need explicit failure-contract review. |
| C09 task registration/init | registration_bridge publishes hung handlers, init, directed yield, yield and wait. lifecycle_init_style calls original queue.c xxxInitTask; that body inserts TDB and enters the original WOW critical section. | Production-connected, not success-only placeholders. No task-scheduler rewrite justified. Init failure cleanup remains a separate missing edge below. |
| C10 application-name lifetime | queue.c xxxInitTask allocates pti->pstrAppName with UserAllocPoolWithQuota (mapped to HeapAlloc). lifecycle_init_style failure cleanup frees task but not that allocation. retire_task calls DestroyTask then frees task; selected taskman DestroyTask frees TDB/waiter, not pstrAppName. No other selected name-release consumer exists. | Confirmed missing project lifecycle release on post-name-allocation failure and ordinary retirement. Recover the original thread-cleanup ownership before implementation. This does not establish SOL/WRITE OOM causality. |
| C10 input attachment lifetime | original input.c AddAttachment shares pqAttach and increments cThreads. lifecycle retirement unlinks PtiLink and frees task without an explicit attachment-release step; destroy_queue only checks cThreads. | Open lifetime integration gap: trace original detach/reference ownership, including embedded task queue versus heap queue, before specifying a fix. Do not simply free a possibly shared queue. |
| C10 normal versus abnormal retirement | ModuleUnload callback performs cleanup without destroying THREADINFO; runtime_unbind invokes retire_current_thread. Live enrolled windows explicitly reject retirement with ERROR_BUSY. | Normal ownership separation exists. Follow session hook failure and native-window teardown to establish whether abnormal retirement eventually completes; forced test cleanup cannot prove it. |

Independent review of this pass distinguishes proven absence from incomplete
analysis: the missing name release and missing receive-state producer are
source facts; input-attachment and abnormal-retirement repairs are not yet
designed. The sender record is explicitly labelled an outer native call in
production, so treating it as an original received SMS would violate its own
contract. E94/E95 already reject that shortcut and a duplicate SMS transport.

Next source reads must cover original thread teardown/queue detach and the
selected session-unbind consumer, then complete callback frame and W01
dependency mapping. Repair acceptance cases follow that inventory; no new
runtime experiment is needed to establish the above missing release.

### Original teardown and attachment follow-through

Read-only canonical inputs are O:/repos.external/OpenNT/windows/core/ntuser/kernel:
queue.c SHA-256 0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8;
input.c SHA-256 B7656BCA55E892A42BA825BE0324211450AD298C40717E1AE5A889D084467529.
These were read, not copied or compiled. The comparison identifies original
owners rather than authorizing a wholesale USER server import.

- Original queue.c DestroyQueue (1511 onward) decrements cThreads before
  deciding whether the queue survives; surviving queues transfer references
  to departing threads, while final disposal waits for cLockCount as well.
  Current lifecycle destroy_queue only checks for an already-zero count;
  it does not implement the decrement expected by imported AddAttachment.
- Original input.c ReattachThreads (5702 onward) does more than
  RecalcThreadAttachment: it consumes the pending pqAttach plan, clears that
  field, and calls AttachToQueue to install the result. Current reattach
  only computes the plan and returns whether pqAttach is non-null. Thus the
  mismatch precedes teardown; it is not just a missing free. The current
  embedded per-task queue can remain referenced by other tasks' pqAttach
  after its containing task is freed. This is a source-derived lifetime risk,
  not a claimed observed application crash.
- Original queue.c xxxDestroyThreadInfo cleans windows before SendMsgCleanup,
  removes task scheduling, releases queue membership with temporary queue
  locks, then unlinks desktop membership. DeleteThreadInfo (1685 onward)
  performs final event/app-name/queue-lock release. The task-name free is at
  1710--1712. Current DestroyTask alone is not this entire thread lifecycle.
- Current runtime_unbind correctly refuses to erase binding state when
  retire_current_thread fails. session_thread_unbind propagates that failure
  before decrementing counters. But thread_start_compat.c opennt_exit_thread
  ignores it and unconditionally calls ExitThread; normal start thunks also
  ignore the unbind result before returning. Therefore the ERROR_BUSY
  live-window refusal has no retry at that terminal callsite. Trace native
  destruction notification before choosing how the original two-phase
  lifetime contract should be connected; do not simply suppress the refusal.

Recovery ladder for this group: original queue/input owners are identified;
the currently selected subsets omit required lifecycle/attachment phases.
Whole original functions also own kernel message lists, focus/capture,
desktop locks and ETHREAD object references, which the current finite views
do not represent. Their complete direct composition has not been established.
Next design must identify the smallest original slice and same-shaped host
binding for the fields actually owned here, while leaving native input with
USER. No new cleanup algorithm, duplicate queue, mirror change or imported
body is admitted by this finding. A two-line local name free would address
one leak but would not close this original-owner lifetime group.

### C03 callback-frame source comparison

Read original base/mvdm/wow32/wcall16.c from the same canonical tree,
SHA-256 4875844ABFB8D22025480C14174481A194779D83759CE3A696E978B68C55A9F0,
and current wcall16.c, wow32.h, wow32.c, wow_callback_frame_lease.c,
wow_user_thunk_scope.c and session guest-memory release boundaries.

The original CallBack16 algorithm is still selected: frame construction,
stack selection, host_simulate recursion, return extraction and stack pop.
WND, dialog, hook, enumeration, timer and resource callers still converge on
this owner (wmdisp32, wudlg, wowhooks, wuwind, wutmr, wres16). They do not
justify writing a new callback dispatcher. W32Dispatch and W32TryCall both
use the selected finally-based thunk borrow scope.

DIV-178's added failure paths require correction as one frame-lifetime group:

- At current wcall16.c 779--780, release of the caller and callback frames is
  combined using short-circuit OR. If the first fails, the second is never
  attempted. Original source executes FREEVDMPTR, FLUSHVDMPTR and FREEVDMPTR
  in sequence. Lower release can fail when the session is no longer active;
  no claim is made that this causes ordinary startup failure.
- Callback-frame acquisition can fail after vpCBStack and the initialization
  flag have been changed; that branch releases the caller lease and returns
  without restoring the pre-entry callback-stack state. The added failure
  route needs an explicit pre-entry rollback, without replacing successful
  original stack/CPU ordering.
- Original GETFRAMEPTR resolves through GetPModeVDMPointerAssert, which can
  invoke RET_FORCESEGMENTFAULT for a valid nonpresent selector. The current
  fixed-frame lease goes directly through mvdm_softpc_effective_address.
  Equivalence of that path's nonpresent handling has not been established.
  Trace the existing address/segment owner before retaining a duplicated
  resolver or reverting the bounded-memory seam. This is an open semantic
  comparison, not proof of SOL/WRITE causality.

Original pointer macros currently expose raw aliases and FREEVDMPTR is a
release-build no-op; simply reverting DIV-178 would remove the bounded lease
contract. The required next decision is whether the original macro-shaped
seam can supply the selected bounded access without reproducing its segment
resolution policy. If composable, replace/delete the autonomous equivalent
in that repair. Existing lease fixture tests the helper, not the actual
CallBack16 failure/rollback branches, so it cannot certify this group.

Follow-through establishes the address-contract difference in source:
mvdm_softpc_effective_address.c selects real-mode arithmetic when PE is clear
or VM is set; its protected branch reads descriptor.base without invoking
WOW's demand-load callback. Original GETFRAMEPTR always routes through
GetPModeVDMPointerAssert (wow32.c1774 onward), including its existing
RET_FORCESEGMENTFAULT path, before obtaining the protected alias. CPU mode
selection is therefore not an equivalent replacement for that WOW contract.
This is not a claim that an ordinary callback actually arrives with PE clear.

Current production uses of wow_callback_frame_acquire_vp are confined to the
three fixed-frame acquisitions in CallBack16. wow_task_parent already uses
acquire_protected_vp, as does the separate resource boundary. Consequently
the callback resolver can be corrected without changing their contracts.
The proposed repair must invoke the already-selected original resolution
owner, retain the bounded copy/lease interval, and remove the callback-only
mode-dependent resolver path. It must also preserve both release attempts
and roll back pre-entry state on the newly fallible lease boundary. Do not
duplicate the original present-bit/selector checks in a new helper.

This narrows a concrete C03 implementation seam. Direct raw-pointer reversion
is still not accepted as bounded-memory recovery; exact same-shaped binding
and failure ownership require the implementation brief and review. One
read-only shell attempt timed out in permission review before process creation;
the permitted single narrowed retry confirmed the consumer inventory. No
application/test run was launched to derive this finding.

Additional ABI review: original wow32.h declares GetPModeVDMPointerAssert
FASTCALL, with an extra Count parameter only in DEBUG. It returns the
FlatAddress result. Current dpmiselr.c publishes FlatAddress through original
GetDescriptorMapping, not necessarily a simple IntelBase addition. A repair
must not call the original resolver merely for its fault side effect and
then silently discard a distinct mapping result. The current bounded lease
uses SAS-linear copies; prove mapping equivalence or preserve the original
resolved mapping through the admitted memory boundary. This remains an
implementation-design requirement, not permission for a generic pointer
manager. guest_memory_lease_end already frees outstanding bounce allocations;
the short-circuit release defect is not proof of a persistent leak after
complete session teardown.

## Next action and review boundary

The owner corrects the method: source tracing identifies recovery gaps;
tests are acceptance after connecting new code, not a discovery loop.
Collect terminal outputs from already-started runs without adding trial
runs. First complete the original-owner to production-binding implementation
and cleanup inventory, classify actual missing/divergent edges, and order
their dependencies before admitting repairs. Existing observations remain
auxiliary evidence. Do not presume the reply boundary explains SOL/WRITE
initialization. Keep the whole WOW recovery goal active; no functional
completion or new release is claimed.

## S1 review conclusion and implementation order

The single-person second-role review compared the source inventory against
the original C01--C03/C09/execution-C10 checklist and actual production
selection. S1 establishes a bounded recovery-gap inventory, not exhaustive
proof of all runtime branches. Earlier sections marked initial/partial are
the investigation chronology; their unresolved implementation questions
remain explicit below rather than converted into passes.

1. Connect original callback address semantics and correct the added frame
   failure lifecycle (C03 prerequisite for C01 reentry). Keep original
   CallBack16 ordering, no substitute dispatcher; remove the autonomous
   mode-dependent callback resolver. Verify the real selected function,
   not just the lease helper, after implementation.
2. Complete message receive/reply/execution handoff design and implementation
   (C01/C02). The original scheduler's missing receive identity cannot be
   filled with a guessed outer-call record. Native USER stays the transport.
   Early/duplicate replies, foreign peers, native-induced calls, timeout and
   loss all remain required; a callback-frame repair does not close them.
3. Recover task/thread lifetime phases (C09/execution C10), including the
   name allocation, input-group plan/reference completion, and terminal
   unbind failure. Reuse original queue/input/task owners where composable.
   Promote any actual W02 object/input dependency explicitly before changing
   that owner's behavior; never silently import the full USER server.
4. Complete actual package acceptance and all retained application frontiers.
   SOL/WRITE startup causes remain unattributed and open under their recorded
   owners. No source finding here is an excuse to mark either functional.

Review found no production edit or demonstrated ordinary-application cause
in this S. The main next implementation candidate is a source-identified
callback dependency, not the next failing test. Runtime package stays at the
accepted T446 identities. Governance/link/diff delivery is still required
before this research-only S1 can be reported as delivered.

Delivery review uses build/M0-T447/S1/review-docs.ps1 as an owned hidden
background runner, with a120-second governance-child deadline. The gate
includes relative Markdown links; git diff --check is separate. Initial
runner issues (PowerShell process ExitCode retention and stderr warnings
treated as exceptions) are runner failures, not waived checks; corrected
runner output and terminal status are retained before delivery. Source
comparison and the second-role review above do not claim runtime completion.
