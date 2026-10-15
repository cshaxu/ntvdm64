# T447 native execution boundary design

## Authority and evidence level

Latest owner clarification allows a clean project implementation where
original integration creates excessive architectural complexity or error
risk, with equivalent semantics mandatory. It resolves the implementation
policy question, not the technical equivalence proof. The earlier request
to waive native-versus-NT4 reply semantics below is superseded, not approved.
Retain original thunks and directly composable original algorithms; a new
adapter is justified by a documented boundary and replaces, rather than
duplicates, the superseded provider. No production design is certified here.

Latest decision: owner approves evaluating original same-worker WOW-to-WOW
SMS delivery. The previous native-only admission impasse is resolved for this
bounded research; the rejected candidates below retain their evidence value.
No implementation, full USER-server import or second scheduler is approved.
The next design must close parameter capture, dispatch, native-wait servicing,
reply/timeout/death and interoperability before providing its mirror budget.

Owner approves design research after the explicit private-SMS association
decision request. S2 stays active; production edits remain frozen pending
integrated review. This is a source-derived candidate model, not implementation
approval, behavioral equivalence or a runtime result. The
[execution audit](../evidence/m0-t447-s2-message-boundary-audit.md) and
[working plan](t447-wow-execution-plan.md) retain the complete scope.

Inputs: selected taskman.c xxxSleepTask/xxxDirectedYield/InsertTask;
wow_user_task_lifecycle.c native call, callback and msg-wait boundaries;
wow_user_message_bridge.c native receive; original sendmsg.c reply/timeout
ordering; existing wow_user_task_lifecycle_fixture.c early-reply case.
Read sources, not another runtime discovery experiment.

## Fixed ownership

- Modern USER remains the released delivery owner. The latest approved
  research may assign same-worker WOW sends to original SMS delivery, with
  explicit native/external interoperability. The initial no-wrapper candidate below is retained as a
  failed ordering design, not a prohibition overriding the owner's later
  envelope-research approval. Association-bearing envelopes may be evaluated;
  no envelope or local SMS implementation is accepted yet. Original local
  SMS delivery is now admitted for bounded design evaluation only.
- Original taskman remains the task priority/list/selection owner. No new
  runnable queue, guest executor or mutex standing in for its scheduler.
- The adapter can describe whether a native stack is actually able to enter
  guest code. It cannot invent the sender of a received message.
- USER data locking, guest execution permission and guest-memory borrows
  remain separate. Every native callback is gated before its first guest
  access, including preparation and copyback outside CallBack16.

## Candidate stack states and transitions

These are semantic states for review, not a proposed second task registry.
Nested scopes belong to the existing bound thread and are unwound in LIFO
order; an outer call is not a received message identity.

| Actual boundary | Required execution transition | Original mechanism / open issue |
| --- | --- | --- |
| Guest calls an operation capable of waiting or invoking a callback | Publish native-suspended state, release guest execution and then USER data lock before the native operation | xxxSleepTask(FALSE, HEVENT_REMOVEME) is an existing original external-wait boundary, but its exclusion is only local to that call; see eligibility counterexample below |
| Native invokes a callback with guest work | Validate bound task/lifetime; publish callback-ready scope; obtain original task selection before any guest access | Existing WND scope uses xxxDirectedYield(DY_OLDYIELD); other callback registration seams must provide the same pre-marshalling boundary |
| Callback inherits current guest execution | Retain ownership, add only nested lifetime bookkeeping | Do not reschedule a synchronous enum/helper merely because it is called a callback |
| Callback returns to suspended native operation | Complete copyback/borrow cleanup; release execution acquired for this callback; restore outer suspended state | No guest continuation occurs on this native return; never leave a blocked native stack selected as the execution owner |
| Native operation returns normally, fails or times out | Preserve result/error; publish ready continuation; reacquire through original task selection before guest return | Return proves this caller is ready, not which callback ran; remove target-based pseudo-reply scheduling |
| Guest calls ReplyMessage | Native USER decides whether a reply occurred; preserve that result | Successful reply may unblock a caller but supplies no exact peer. Ordinary yield alone does not prove original early-reply ordering |
| Task/worker ends | Cancel its eligibility, unwind original resources in valid order, wake real dependents and invalidate scopes | Depends on the integrated C10 retirement group; no completion merely from the early hung-event signal |

## Counterexample 1: transient exclusion is not native-wait eligibility

xxxSleepTask skips the current task when hEvent == HEVENT_REMOVEME, then
calls InsertTask and returns. InsertTask preserves its event count. A later
scan by a different task has a different hEvent/current task pair and can
select the native-blocked task if it still has nEvents > 0. Posted input
or nested execution can leave such events; an event count is not proof the
native call has returned. Selecting that blocked stack can strand the other
tasks. This is a source counterexample, not a newly observed product hang.

The candidate therefore needs persistent native-stack eligibility, integrated
with the original selection boundary. Do not zero event counts to hide the
problem: that discards actual work and corrupts accounting. A minimal original
selection predicate backed by the existing thread scope is a design option,
not an admitted diff. Every selection path (including task lock, task creation,
WOWEXEC wake and nested return) must respect it before it is sufficient.

## Counterexample 2: yield after early reply can select the receiver again

Sender A is suspended in native SendMessage. Receiver B holds guest execution
and calls ReplyMessage. Native USER makes A returnable, but A's host thread
has not yet run far enough to publish a ready continuation. B immediately
calls original OldYield. Only B may currently have schedulable work, so B
can be selected again. If B then waits in guest code for A's response without
yielding, A cannot execute. Original SMS-directed reply instead identifies
and promotes the exact waiting A before continuing B.

Therefore a plain ReplyMessage wrapper plus OldYield is rejected as a full
fix. Holding B until an arbitrary pending send returns is also rejected:
the actual sender can be an external native process, with no local return
notification, while an unrelated local send is pending. HWND/message tuple
matching cannot distinguish that case. Sleep, polling or a fixed timeout
does not establish the original relation or justify calling it equivalent.

The existing early-reply fixture demonstrates one required handshake, not
all of these interleavings. Do not weaken its assertions to make this design
pass. The design still needs a proven bounded continuation rendezvous or an
explicitly reviewed semantic difference; neither is supplied by this draft.

## Selection-entry review

Source enumeration of assignments to ptiScheduled in the selected original
queue.c/taskman.c finds the normal priority scan, idle-hook temporary owner,
WOWEXEC virtual-interrupt branch, task-init forced selection, and clearing
on sleep/destruction. DirectedScheduleTask changes events/priority, not the
selected pointer directly. No production implementation is changed by this
enumeration.

| Entry | Eligibility obligation |
| --- | --- |
| Normal scan | A queued event alone cannot make a native-suspended stack guest-ready; do not select it until an actual callback or native return is present |
| Idle-hook temporary owner | The hook itself can enter guest code; eligibility must precede invoking it, rather than checking only at the final CPU entry |
| WOWEXEC virtual-interrupt branch | The current WOWEXEC thread is at an actual original wait boundary; retain the dedicated hardware-work handling, not a general pending-message guess |
| xxxInitTask forced selection | Preserve original immediate new-task selection and W32Thread/creator rendezvous; a newly committed task is a real continuation, not an idle native stack |
| Task-locked owner | xxxSleepTask deliberately does not clear ptiScheduled when nTaskLock is nonzero. A readiness predicate must not silently override that policy or select a different task merely to avoid a wait |
| Destruction | Clear eligibility and scope references before storage release; original DestroyTask retains its task-lock reset and successor wake responsibilities |

This narrows a possible adaptation to an original-selection eligibility
predicate plus boundary state, not a replacement priority algorithm. But a
single predicate in the normal scan is insufficient unless the other entries
are proved safe by their source preconditions. In particular task-lock
deadlock caused by an application deliberately preventing task switching is
not authority to rewrite LockCurrentTask behavior.

## Original wake filtering is a missing contract, not just a counter

Read canonical queue.c xxxSleepThread (3096 onward) and SetWakeBit (3280
onward) together with sendmsg.c's wait loop. Original sends wait for
QS_SMSREPLY and optionally permit received sends; SMTO_BLOCK introduces
QS_EXCLUSIVE so the sender does not dispatch ordinary reentrant sends.
SetWakeBit records pending state, but increments a WOW task's nEvents only
when the new bit intersects that thread's current wake mask. The original
sleep loop distinguishes actual reply, received message and timeout before
returning to its caller.

Consequently the native-boundary adaptation must not reinterpret nEvents as
either the length of the native queue or the number of runnable guest
continuations. The host transport already owns SendMessageTimeout's actual
blocking/reentry choice. A scheduler adapter must not PeekMessage on a blocked
sender to manufacture a callback that SMTO_BLOCK intentionally excludes.
An actual callback arriving through USER is evidence of eligible callback
work; an arbitrary queued input bit is not. Preserve timeout/result and
callback suppression independently of guest execution reacquisition.

This also fixes the acceptance denominator: default SendMessage reentry and
SMTO_BLOCK non-reentry are separate required cases, with reply/timeout and
task-lock combinations. Native timeout is not permission to return directly
to guest while another task owns execution; distinguish transport completion
from waiting to regain the cooperative guest CPU. Do not promise that the
total guest-visible elapsed time is bounded solely by the native timeout.

## Scenario acceptance matrix

| Case | Required proof |
| --- | --- |
| Same-thread send | No unnecessary relinquish if there is no external/native suspension; nested scopes preserve the inherited owner |
| A to B to A nested send | Both native stacks remain able to receive callbacks; callback-ready can temporarily override outer suspension without resuming the wrong guest continuation |
| Foreign sender / identical message tuple | No attribution to an unrelated local pending call; original single-guest-owner invariant still holds |
| Early reply followed by another send | First reply is not mistaken for second-message completion; no implicit dependency on receiver callback return |
| Duplicate or invalid ReplyMessage | Native result remains authoritative; do not manufacture a new wake or release on failure |
| Timeout while receiver continues | Sender becomes ready by actual native return; receiver scope remains valid, with no pointer into a destroyed sender stack |
| Modal and implicit native calls | All guest-capable callback kinds enter the gate; initial HWND is not assumed to be the complete receiver set |
| Task lock and task death | Preserve original lock contract or document the precise required exception; no selected dead/native-blocked task and no borrowed task pointer after retirement |

## Removal set if a complete design is proved

### Callback storage: reject a resolver-call-only repair

Implementation update, 2026-10-14: the previously frozen +21/-2 rollback
candidate is superseded, not delivered. CallBack16 now consumes the original
GETFRAMEPTR/CBFRAMEPTR result at all three original resolution points. Only
the caller's wTDB and wLocalBP are copied before the second resolution; no
caller snapshot is committed. Preparation uses the actual native mapping,
then the original flush/free boundary precedes host_simulate. Return data is
read from a fresh original resolution. There is no extra flush-time resolver
call and no new FALSE-return transport failure path. The obsolete
current-mode acquire_vp helper and declaration are removed; protected resource
consumers remain. Existing resource lease tests are relabelled accordingly,
not claimed as callback tests. DIV-178 is updated in src/mvdm/README.md.

Source budget: wcall16.c is +18/-26 relative to HEAD (net -8 lines); no other
original C file is changed in this group. Three affected objects compiled in
the existing x86 /MT CCPU40 release provider; hidden process 9440 returned 0
with evidence in build/M0-T447/S2/callback-original. This is compile evidence
only. Original resolver mapping/nonpresent-segment behavior, nested callbacks
and real guest application acceptance still require verification. Message
entry ownership and other audited execution gaps are not closed by this edit.

The selected GetPModeVDMPointerAssert returns a native pointer from the original
FlatAddress array, after its original not-present-segment callback. Selected
dpmiselr.c publishes that host array through GetDescriptorMapping and IntelBase;
the PMODE32 guest-linear array is a different view. Calling the resolver only
for its side effect and then reacquiring through effective_address or a flat
IntelBase subtraction would still discard the original mapping decision.
The replacement must consume the actual resolved storage under a bounded
scope; no second selector/mapping registry is proposed.

The shared copied-lease mechanism has an independent reentry contract. Its
acquire currently allocates a slot buffer, calls the read backend, and only
then marks the slot active. Release calls the write backend before freeing
the buffer; end immediately frees every slot. Because SAS backend calls can
enter guest fault handling, reserving a slot before read is necessary but
not sufficient: teardown must not free an in-flight read/write buffer, and
an unwound operation must not publish a lease into an ended/restarted epoch.

Design constraints for this group:

- Reserve ownership before invoking a guest-capable backend; rollback only
  the reservation owned by that operation, not a nested borrow's storage.
- Keep the actual copy buffer alive until the backend returns or follows the
  admitted terminal unwind; reject new acquisition once teardown starts.
- Revalidate the same context incarnation before publishing or committing;
  a returned backend success alone does not prove the session survived.
- Permit disposal of outstanding borrows during retirement without permitting
  new guest work. The current session wrapper rejects release unless ACTIVE,
  so moving end later alone does not solve the terminal path.
- Do not claim stale-ticket protection from the epoch stored only inside the
  reusable slot. A stale slot pointer and a new pointer to that same slot are
  numerically identical; the caller's acquisition identity or a proved
  non-reuse lifetime must participate if that protection is promised.

These shared mechanics are project-owned session/SoftPC changes, not new
original-mirror algorithms. They must be reviewed with every existing lease
consumer and C10's pre-exit ordering. They do not yet settle the resolver's
bounded native-storage integration or original CallBack16 failure contract;
therefore the frozen +21/-2 callback candidate remains inadmissible on its own.

#### Callback-specific closure and removal boundary

The production call-site census distinguishes two interfaces in the current
helper. Only CallBack16 calls wow_callback_frame_acquire_vp (three sites).
wow_task_parent.c and wuser.c also use acquire_protected_vp; their resource
contracts are not permission to delete the entire helper translation unit.
Existing fixture substitutes for acquire_vp do not exercise the original
GetPModeVDMPointerAssert and cannot prove its replacement is connected.

The callback candidate to review uses the actual original resolved pointer,
rather than translating that pointer back to a guest address. Preserve the
original three resolution positions: caller frame, callback frame before
entry, callback frame after return. Snapshot the caller's required values
immediately; do not retain its mapped pointer across the second resolution.
Keep original callback-frame field construction, stack selection and return
extraction in CallBack16. The second resolved pointer may serve only the
straight-line preparation interval ending at the original release boundary,
with no guest-capable call or scheduling handoff in that interval. Discard it
before host_simulate. The third resolution reads the returned frame afresh.
Do not introduce a second descriptor map or a heap-backed frame registry.

Review correction: an earlier draft proposed resolving the destination again
at flush. Reject that extra resolution: it introduces a fourth potential
RET_FORCESEGMENTFAULT callback not present in the original ordering. Likewise,
never write back a whole caller-frame snapshot. Original kernel31/wow16cal.asm
WOW16Done (line 1029) explicitly writes ES into the caller's vf_wES before
rebuilding the callback return frame. That is a legitimate guest update, not
stale data to restore. Original CallBack16 only reads its caller frame and
releases it without a flush; preserve that asymmetry.

This requires explicit review of the second resolution's segment-fault
callback, nested stack restoration, preservation of untouched CBVDMFRAME
bytes, and original exception/terminal behavior. WOW16ForceSegmentFault
(line 985) calls IsBadReadPtr and returns through that same WOW16Done path;
it is not merely a host descriptor-table refresh. The bounded preparation
interval also depends on the integrated execution gate excluding another
guest task; a native memory view is not itself such a gate. It must not add another
unchecked FALSE return to original callers. A copy helper cannot report a
fault as success or silently replace the original resolver's fault handling.
The USER WND/CLS borrow scope is not a guest-memory pin and is not reused as
one. This remains a candidate until those ordering obligations are proved.

Source basis: canonical wcall16.c constructs the frame between GETFRAMEPTR
and FLUSHVDMPTR, releases it before host_simulate, then resolves again for
return extraction. dpmiselr.c VdmAddDescriptorMapping explicitly documents
that a mapped native address need not equal IntelBase plus the guest address;
GetDescriptorMapping also owns removal of obsolete descriptor associations.
The original resolver and descriptor-map bodies therefore require no planned
edits for this route. Its mirror change is confined to the existing wcall16.c
frame-access seam, with exact line accounting deferred until the candidate
is reviewed. Remove its mode-dependent acquire_vp implementation and its
obsolete tests in the same connected repair; preserve other live consumers.

The general SAS copied-lease reentry defects remain in the systemic repair
inventory, but are not a reason to route these resolved native frame snapshots
back through SAS or to make every callback depend on a new generic lease API.

Replace wow_user_native_call's fabricated message/previous psmsSent and
receiver-ID-based DirectedScheduleTask(FALSE) on arbitrary native return.
Remove the corresponding pseudo-SMS bookkeeping, not original reusable
taskman algorithms. Keep native results and modern USER transport. Replace
callback-only-WND coverage with registration-boundary coverage before guest
marshalling, and replace ignored restore failures with the integrated terminal
contract. Do not remove the old route until the replacement covers its whole
caller set and passes actual guest and lifecycle acceptance in the same repair.

### Adapter-owned USER call-outs are part of the repair boundary

The selected-original-source macro redirects are not a complete call-out
inventory. `WU32FillWindow` calls `pfnFillWindow`, whose registered adapter
calls `wow_private_user_fill_window`. That implementation calls native
`SendMessageA` for WM_CTLCOLOR, without the private-access header or a task
scope. The provider generator gives this adapter the ordinary support-source
flags; it does not force the original-source USER redirects into it.
Original paint.c::xxxPaintRect routes this operation through
random.c::xxxGetControlColor and xxxSendMessage. Thus the current adapter
bypasses even the existing native-call boundary. This is a source-proved
coverage gap, not proof that it caused an application's initialization error.

The bounded textual call-site census in wow32-dll/source also finds:

| Route | Existing scope | Required design disposition |
| --- | --- | --- |
| FillWindow color query | Neither task-call scope nor original-source redirect | Include in the common synchronous native-call boundary; retain native color-message delivery and brush validation |
| Native/CPD CallWindowProc branch | Deliberately delegates to native USER; tagged WOW branch has task scope | Preserve classification, but account for a native procedure reentering WOW; do not infer no callback from an untagged procedure |
| Window/dialog creation | Creation hooks and binding lifetime scopes | Keep these ownership scopes; they do not substitute for execution ownership during synchronous creation callbacks |
| Creation or style failure DestroyWindow | Raw native call in creation bindings and registration bridge | Keep destruction callbacks inside the same execution/lifetime contract, including exceptional failure paths |
| Native modal dialog and message box | Style scope | A style scope does not release or reacquire guest execution; include the native modal loop and guest callback boundaries |
| Detached WM_NCDESTROY default processing | Native DefWindowProc fallback | Preserve native terminal processing without demanding a retired guest context |

This census identifies direct calls, not every transitive callback-capable
USER/GDI API. It complements the registered callback-family inventory; it
does not replace it. The correction belongs in project adapters and requires
no new mirror edit for these call sites. Simply routing FillWindow through
the currently flawed pseudo-SMS wrapper is not an accepted standalone repair:
the replacement must cover this route together with the other send/wait
consumers, and remove the superseded pseudo-SMS implementation in that repair.

## Separate review conclusion

### Retirement correction: original PatchThreadWindows precedes destruction

The prior pre-CPU-release requirement applies to cleanup that can still enter
guest code; it is not proof that terminal window destruction should call the
exiting application's procedures. Canonical server.c PsW32ThreadCalloutExit
sets TIF_INCLEANUP before xxxDestroyThreadInfo. Canonical queue.c line 1815
then calls PatchThreadWindows before DestroyThreadsObjects. createw.c
PatchThreadWindows (line 2533) explicitly replaces exiting-thread procedures
with server-only procedures to prevent application callbacks during cleanup.
This required preparation was absent from the earlier retirement inventory.

The original function selects live, non-destroy-marked windows by owning
thread, handles the shared menu window separately, and selects a server
procedure from the window's actual function ID (falling back to the server
default). Its comment explicitly rejects choosing from class identity:
subclass/superclass and CallWindowProc usage can differ from the class.
createw.c and hooks.c additionally suppress cleanup-time client hooks; the
flag is not a standalone replacement for the procedure-neutralization pass.

Revised source selection: audit PatchThreadWindows as the original candidate
for the terminal preparation phase, using the existing owned-window table
and live procedure target. Do not author a competing loop based on class
names or unconditionally route all controls to DefWindowProc. Required
outgoing closure is the selected window/function-ID view, native system
cleanup target, shared-menu ownership disposition, cleanup flag and retained
window lifetime. Whole createw.c, private server procedures and kernel exit
callouts remain outside runtime admission. This is a candidate original
subset, not an import already proved composable.

Current wow_window_dispatch_bound always enters wow_task_callback_enter and
publishes the guest callback-window page even for a native procedure target.
Merely assigning a neutral native procedure therefore does not yet remove
guest dependencies. The terminal native-only dispatch path must avoid those
guest operations while preserving native cleanup and the existing
WM_NCDESTROY finally-detach. Keep the live-window guard until actual native
destruction and object-table removal are complete; deleting the guard is not
a repair. Native-only windows, native subclasses/hooks and sibling-owned
children require separate closure rather than assuming the enrolled table
is the entire native desktop.

Normal ModuleUnload, terminal native-thread cleanup, and whole-worker death
remain distinct. Original W32FreeTask has already freed resource aliases,
hooks and parameter maps before host_ExitThread; restarting ordinary guest
window callbacks at that late edge would use retired state even if CCPU TLS
were kept alive. This rules out a simple move of guest-capable destruction to
immediately before ccpu386exitthread as the complete design.

#### Dispatch seam without another procedure classifier

The selected original client.c DispatchClientMessage already tests WNDPROC_WOW
and calls its registered pfnWowWndProcEx only for that branch. The alternative
branch invokes the native WNDPROC directly. Preserve this original decision:
bind the existing WOW callback slot to a scoped adapter which acquires guest
execution before invoking the original registered W32Win16WndProcEx. Do not
duplicate the tag decision in another dispatcher or edit client.c.

Move guest callback-window publication and the complete guest-capable
marshalling scope into that callback adapter. Keep window/class storage
references around the whole outer native dispatch, including native calls
which can reenter another enrolled window. A nested WOW call still reaches
the same original tag decision and scoped callback. Preserve the registered
callback's six arguments, including NULL WW for the existing CallWindowProc
route; do not fabricate WW from a class lookup. Share that scoped invocation
with the already-tagged CallWindowProc branch rather than retaining two
independent gate/cleanup implementations. Failed entry must not invoke the
guest callback, and unwind must restore the nested scope on exceptions.

This removes unconditional guest entry and page publication from
wow_window_dispatch_bound without removing native lifetime protection or
WM_NCDESTROY detachment. Planned new mirror delta for moving this boundary:
zero files/lines. The gate's scheduling implementation remains dependent on
the message/eligibility design; moving its placement does not prove that
algorithm correct or complete coverage of non-window callback families.

PatchThreadWindows is not yet composable from present fields. The current
wow_cleanup_window has no window fnid. wow_class_lookup_entry does have a
class fnid, but original PatchThreadWindows explicitly forbids using class
identity in place of the actual window procedure family. Original
NtUserSetWindowFNID and dialog-function initialization are evidence for that
separate owner. Do not add a field populated from class fnid or zero and call
it restored behavior. Resolve the native cleanup/control-family binding
before admitting the original subset or quoting a final import-line budget.

#### Dialog route and exceptional destruction are separate live consumers

Source census also reaches wow_dialog_creation_binding.c dialog_proc, not
DispatchClientMessage. Original wudlg.c supplies W32DialogFunc as the native
DLGPROC; the adapter stores it in creation->procedure and calls it directly.
Changing cleanup.lpfnWndProc therefore does not neutralize this callback.
The window-tag callback adapter alone is incomplete: dialog entry needs its
own existing-registration-shaped scope before W32DialogFunc's guest reads
and message thunking, with terminal disposition supplied by the same task
lifecycle, not another dialog task registry. Keep original W32DialogFunc
unchanged and keep native dialog-manager behavior distinct from WNDPROC
return semantics.

There is also a concrete exception-lifetime defect in that adapter:
dialog_proc retains creation before invoking the application callback, but
its WM_NCDESTROY property removal/window detachment occurs after that callback
inside the try body. Its finally only leaves the borrow scope; the retained
creation release follows the try/finally. An exception can consequently skip
both terminal detachment and the retained creation release. The repair must
put terminal cleanup and invocation-reference release in guaranteed unwind
paths, retaining the reference until nested destruction has completed and
removing the property only once. A failed borrow entry on a terminal message
also must not strand the property/reference. Ordinary and exceptional
WM_NCDESTROY need the same lifetime outcome, not necessarily a fabricated
successful message result.

This is project-adapter repair with zero planned original wudlg.c changes.
Tests must exercise the real dialog binding, a callback exception, nested
destruction and terminal entry failure; a window-procedure fixture does not
cover this separate dispatch route. The existing normal-window finally
detachment is a reusable pattern, not permission to convert DLGPROC into
WNDPROC or assume their return contracts match.

### Initialization notification: concrete adapter transaction

Keep the source-derived retry decision in the execution audit, with these
implementation boundaries. The original TDB is not sufficient notification
storage: queue.c xxxInitTask initializes ptdb->pwti to NULL and only fills it
when an existing waiter matches idTask. The live task's guest handle is
ptdb->hTaskWow; it is not the launch request number. Do not create a synthetic
waiter or add request fields to the original TDB to bridge this difference.

The existing wow_user_task_lifecycle_thread carrier needs one DWORD launch
request identity and one pending-notification flag. No additional registry,
timer or retry thread is required. Native current-thread/runtime binding plus
the live TDB supplies the task identity. The two new fields are adapter state,
not a wire record, guest state or original scheduler field.

Registration entry order:

1. Ensure the existing lifecycle owner and inspect the current bound task
   under its existing runtime lock, before any style/parent lookup.
2. No bound task: continue original initialization. Publish the request and
   pending flag only after xxxInitTask succeeds. Pending is required only for
   shared_id other than zero and DWORD(-1).
3. Bound task with pending notification: require the same hTaskWow and launch
   request. Retry only OpenNtBaseClientWowStarted; do not re-enter InitTask,
   refresh parent/style policy, allocate a queue or reinsert the task.
4. Bound task without pending notification, or mismatched identity: retain
   duplicate-initialization rejection rather than returning generic success.
5. Notify outside the USER data lock. Clear pending only on actual success,
   validating the same live binding again before updating it. On failure,
   return the actual error to W32Thread's unchanged seven-attempt loop.

Do not keep a raw task-carrier pointer across the RPC merely because the
request ID was validated before it. The current binding/lifetime must still
be valid on return. Terminal notification failure continues into original
W32DestroyTask; C10 must retire the committed task, not the pre-init rollback
path. The carrier's normal final release also releases this adapter state.

Concrete removal is the unconditional post-commit re-entry into
lifecycle_init_style and its misleading rejection comment in
registered_init_task. Original wkman.c retry, queue.c xxxInitTask, and BaseSrv
generation/dispatched-record/parent-receipt checks remain unchanged. Planned
mirror delta for this transaction is zero files and zero lines; runtime
acceptance still requires the C09/C10 group and is not claimed here.

### Mirror budget baseline and accounting

Source comparison on 2026-10-14 repeats the selected-source parser in
New-T404S5Wow32ProviderNinja.ps1 (including exclusion of !IF 0 table sources),
then compares the 77 selected WOW32 C bodies with their canonical OpenNT
base/mvdm/wow32 counterparts after newline normalization. Ten differ:
wow32.c, walias.c, wcall16.c, wmdisp32.c, wkman.c, wkfileio.c, wkernel.c,
wuman.c, wuser.c and wdos.c. This is a bounded WOW32 baseline, not the total
product mirror count. The T422 S1 six-file result remains historical evidence,
not the current denominator. No source was rewritten by this comparison.

| Item | Measured baseline / design disposition |
| --- | --- |
| Existing uncommitted callback candidate | wcall16.c +21/-2 relative to HEAD, within existing DIV-178; frozen and not a final budget |
| WND callback diagnostics | wmdisp32.c differs from canonical by 60 added lines only: raw-procedure capture and pre/post callback trace bodies. Shrink into bounded owner-local instrumentation if equivalent evidence is retained; do not promise all 60 lines disappear |
| File-open diagnostic | wkfileio.c differs by three added lines: include plus two-line trace call. Candidate diagnostic retirement only after checking acceptance evidence; not a proved functional recovery |
| Initialization notification retry | Design uses existing registration/lifecycle adapter; no new W32Thread mirror change is presently required |
| Task identity projection | Existing registered InitTask supplies guest task and request identities; management binding belongs outside mirrors. This does not establish the termination algorithm's mirror budget |
| Native execution eligibility | taskman selection predicate remains a candidate changed original seam; number of sites cannot be fixed until all selection entries and early reply are resolved |
| Target-only termination | wkman policy seam remains a candidate. Unchanged wholesale invocation and generic MessageBox/ExitProcess interception are rejected; lifecycle/shared LockTDB proof is still required |
| Guest media/code | Zero modifications permitted |

The WND trace is consumed by verify-wow-user-network-profile.ps1 for completed
WM_NCCREATE/WM_CREATE callbacks and verify-wow-user-destruction.ps1 for completed
WM_DESTROY/WM_NCDESTROY callbacks. A native gateway return is not automatically
equivalent to the inner CallBack16 success value. Do not retire these assertions
or replace them with mere dispatch evidence to obtain a smaller diff.

Final review must report separately: baseline-versus-upstream differences,
proposed change-versus-HEAD, changed mirror files, semantic insertion points,
and autonomous adapter deletions. Deleting project code outside mirrors cannot
be counted as removing an original-source divergence. The current measurements
are not a net reduction promise; unresolved groups prevent a truthful total.

The approved research yields a narrower, testable problem than private SMS
reconstruction, but the simplest adaptation is not sufficient. Persistent
native-stack eligibility and early-reply continuation ordering are distinct
obligations. No production implementation is recommended from this draft.
Resolve these finite counterexamples first; do not return to peripheral
patches or claim that publication of this design completes C01/C02.

Owner now approves design research of explicit association-bearing envelopes
for controlled WOW calls while retaining native USER as delivery owner, with
standing authority for similar necessary design-research decisions. No repeat
approval is needed for that research. This does not demonstrate a solution or
admit a replacement scheduler/transport. The requested integrated brief and
mirror budget still precede production implementation.

### Envelope research scope and first exclusion matrix

The selected explicit SendMessageA/SendMessageTimeoutA bindings and enrolled
window gateway are potential attachment points, not coverage of every send.
An initial HWND identifies a window owner, not every receiver of an implicit
USER operation. Retain the original sendmsg.c source ownership of nested
send/receive association, early reply, sender/receiver death and two-sided
record lifetime when evaluating a finite reusable slice.

| Route | Design obligation before claiming coverage |
| --- | --- |
| Same-worker controlled WOW to enrolled WOW window | Carry an explicit invocation identity, validate live task/window incarnations, and retain association until both sender and receiver release it; a stack pointer cannot survive early reply or timeout |
| Same-thread direct callback | Preserve synchronous original behavior; do not create an inter-task send merely to attach metadata |
| External native sender to WOW | No envelope is presumed. Never attribute this arrival to an unrelated local invocation with matching HWND/message/parameters |
| WOW to native or another process | Do not replace an arbitrary application's message with a private identifier it cannot decode, and do not pass process-local pointers as cross-process envelope identities |
| Implicit sends from SetWindowPos, focus, destruction or modal APIs | An explicit-send wrapper does not intercept their internal transport. Find an original/publicly composable boundary or record the remaining coverage gap |
| Hooks, native subclasses and message conversion | Changing the outer message can change observations or bypass original per-message handling. Gateway decoding alone is not transparency proof |
| ReplyMessage, timeout and nested callbacks | Preserve native completion and original scheduling as distinct transitions; bind exact nested receive scope, not the outermost API call or receiver thread alone |

This first matrix rules out treating a registered private message plus a
payload as a universal drop-in replacement. Association storage must not
become another delivery queue or scheduler. Reuse decisions and any required
original-source adaptations will be costed only after this coverage is proved.

### Original association lifetime versus message-parameter lifetime

Source trace: sendmsg.c xxxInterSendMsgEx restores psmsSent after reply and
transfers freeing to the receiver for timeout/early reply while reception is
still busy. xxxReceiveMessage installs/restores nested psmsCurrent, clears
SMF_RECEIVERBUSY, and obeys SMF_RECEIVERFREE. UnlinkSendListSms releases the
retained window, captured parameter storage and SMS. These are two-sided
lifetime decisions, not permission to free at native SendMessage return.

The proposed association therefore cannot be the current stack-allocated
wow_user_native_call.message. A received invocation needs an owned lifetime
through receiver exit after sender completion, with task/window incarnation
validation and nested scope restoration. Ownership storage is distinct from
the original delivery list: do not import gpsmsList as a second queue merely
because original cleanup traverses it.

There is a separate parameter hazard. Selected WU32SendMessage builds MSGPARAMEX
and calls ThunkMsg16 before native SendMessage, then calls lpfnUnThunk16 after
that native call returns. Retaining only an envelope's numeric wp/lp fields
does not retain any converted buffers those fields reference. The original
server also owns pvCapture and per-message conversion. Consequently generic
shallow-copy envelopes are rejected even if their association record itself
is reference-counted. A proposed boundary must preserve each reached original
parameter capture/copyback contract, including early reply and timeout, without
moving sender copyback to an arbitrary receiver thread or writing after the
sender has released its destination.

The enrolled window gateway currently acquires task execution before
DispatchClientMessage and releases its window/borrow scope afterward. An
association must be decoded and validated before that execution acquisition,
then installed for the actual callback and restored afterward. Installing it
only inside CallBack16 is too late for scheduler entry and thunk marshalling.
This identifies the adapter seam, not proof that all native subclasses/hooks
or implicit sends pass through it.

Reuse review: _ReplyMessage's duplicate-reply and directed-task transition,
xxxReceiveMessage's nested scope, and SendMsgCleanup's sender/receiver-death
cases are original candidate owners. Their current server-only transport,
window locking, captured-data and global-list dependencies prevent wholesale
composition as-is. Before selecting a true subset, account for every excluded
callback/notify/broadcast branch and bind transport completion exactly once;
do not implement independent guessed flags under the original function names.

The original cross-WOW route does not exempt parameters from native capture:
sendmsg.c performs its per-message capture before the TIF_16BIT-directed
scheduling branch. The latter distinguishes 16-to-16, 32-to-16, 16-to-32 and
32-to-32 execution, not alternative parameter lifetimes. A same-worker WOW
envelope therefore cannot justify shallow parameters merely from shared
address space.

### Parameter reuse decision: no new universal marshaller

Further source inspection narrows the preceding hazard; it does not justify
copying every message parameter. wparam.c is already selected, and its
AddParamMap/GetParam16/DeleteParamMap maintain reference-counted parameter
associations. WM32SetText first uses GetParam16 and otherwise creates its
original guest string; its unthunk releases that map or allocation.
UnThunkWMMsg16's WM_SETTEXT branch consults DeleteParamMap before releasing
the sender-side pointer. Preserve these original paths, including their
alias handling, instead of adding another string/parameter registry.

This is not a universal buffer pin: the default parameter-map build does not
enable MAPPARAM_EXTRA, and a mapping reference does not establish arbitrary
application storage lifetime. WM_GETTEXT is a separate output contract:
WM32GetText prepares a guest buffer and copies it to the native destination
during unthunk. Original server xxxInterSendMsgEx separately owns its captured
output and copies it to the sender only when !fTimeOut. WM_COPYDATA has another
existing original data-node/allocation path. Treating these as interchangeable
opaque lParam values loses source-defined capture and copyback behavior.

Decision: retain existing original per-message thunk and map owners unchanged
unless the route audit proves a missing binding. No generic envelope copier,
new parameter cache, or unconditional deep copy is planned. For each envelope
route, identify the original capture layer it bypasses and the exact retained
owner that supplies that layer's contract. The scalar/no-thunk case is only one
case, not permission to ship a scalar-only recovery. This decision adds zero
planned mirror edits to wparam.c/wmsg16.c; the overall transport and mirror
budget remains unresolved.

### Envelope capture boundary: original client and server layers are distinct

Read canonical client/clmsg.c SendMessageWorker/SendMessageTimeoutWorker and
kernel/sendmsg.c xxxInterSendMsgEx together. Original client SendMessageWorker
routes inter-thread sends, DDE and installed call-window hooks through
CsSendMessage. ANSI/Unicode conversion can also require that server route.
The client-side same-thread shortcut is therefore not an original precedent
for bypassing capture on a cross-task envelope. Timeout and broadcast also
have separate original FNID paths.

The server capture switch consumes already-transformed values in some cases:
WM_SETTEXT uses LARGE_STRING, WM_MDICREATE uses MDICREATESTRUCTEX, while fixed
structures have already been captured by the preceding message thunk. List
and combo string capture also depends on the window's owner-draw/HASSTRINGS
style: the same message number can carry an integer item value rather than a
string pointer. Importing that switch alone and feeding raw SendMessageA
arguments would therefore misinterpret its input. The selected ntstubs.c is
only the GetClassWOWWords subset; its presence does not mean the general
CsSendMessage conversion family is already connected.

Design disposition: replacing every original message number with a private
envelope at wow_native_SendMessageA is not an accepted universal boundary.
It would remove native message-specific conversion before the target gateway
could decode the payload. Reusing WOW's 16/32 thunks alone does not restore
the separate native client/server layer. A reusable source slice must include
its exact input representation and outgoing capture/copyback dependencies,
not just familiar switch labels. The current candidate therefore has no
justified universal-envelope implementation or finite mirror-line estimate.
Continue source selection at the conversion boundary, not by inventing a
message-number table or claiming scalar-only tests establish full recovery.

### Conversion-entry disposition

Canonical client/ntstubs.c fnINSTRING (line 1828), fnINSTRINGNULL and
fnOUTSTRING (line 1986) confirm the outgoing dependency: the former builds a
LARGE_STRING and invokes NtUserfnINSTRING; the latter describes the caller's
output buffer, invokes NtUserfnOUTSTRING and handles the zero-result string
termination case. inc/messages.h selects these separate operations for
WM_SETTEXT and WM_GETTEXT. They are private message-call front ends, not
self-contained capture implementations callable on an envelope payload.

Disposition: do not import these client stubs as the missing envelope
marshaller. Redirecting their NtUserfn calls back to public SendMessage with
the unchanged message merely retains the original association problem;
redirecting to the private envelope still bypasses the message-specific
capture. This rejects that proposed reuse slice, not all possible association
bindings. The private USER-server conversion closure remains a stopping
boundary, not a reason to reimplement it in an adapter.

Budget consequence for this rejected slice: zero new mirrored files and zero
added/deleted mirror lines. This is not the total S2 budget. Existing native
SendMessage routes and original WOW per-message thunks remain unchanged while
the association design is unresolved. An integrated design must identify a
boundary that preserves native capture, rather than reopening this same
client-stub import as if its outgoing dependency were still unknown.

The complete representative string chain has now been followed beyond the
client stub: kernel/ntstubs.c MESSAGECALL(INSTRING), line 8669, probes the
LARGE_STRING and explicitly delegates buffer capture to xxxInterSendMsgEx;
MESSAGECALL(OUTSTRING), line 9219, likewise delegates output allocation and
requires return-side probing. inc/ntcb.h SMESSAGECALL(INSTRING), line 2403,
then performs the receiver-side ANSI/Unicode capture and invokes the native
callback transport; its RECVCALL counterpart dispatches the converted
message. These are distinct validation, retained-send-storage and
receiver-conversion stages. Selecting the kernel entry stub alone is no
more a self-contained parameter owner than selecting the client stub.
No complete conversion subset has been admitted by this trace, and it does
not authorize importing the full USER server or replacing native delivery.

### Identity-query alternatives and the transport stopping point

Further original-source checks close two concrete metadata alternatives:

- client/clmsg.c::GetMessageExtraInfo calls the thread-state query;
  kernel/ntstubs.c::UserThreadStateExtraInfo reads ptiCurrent->pq->ExtraInfo.
  kernel/input.c::_SetMessageExtraInfo writes that queue field, and queue.c
  updates it when taking queued input. It is not a per-synchronous-send
  identifier installed/restored with psmsCurrent. Using it as an invocation
  token would overwrite application-visible state without proving causality.
- kernel/sendmsg.c::_QuerySendMessage, line 2783, copies hwnd, message,
  wParam, lParam and time from psmsCurrent, then returns TRUE. Despite its
  old "return the sender" comment, it returns neither ptiSender nor an SMS
  identity. Its private entry cannot be admitted as an identity provider on
  the strength of that comment. client/client.c::ReplyMessage itself merely
  forwards to NtUserCallOneParam; it contains no separate usable association
  algorithm.

The original transport boundary is equally concrete. In xxxInterSendMsgEx,
the same allocated SMS is appended to ptiReceiver->psmsReceiveList, installed
as ptiSender->psmsSent, used to wake QS_SENDMESSAGE and to call
DirectedScheduleTask, then waited on through xxxSleepThread until reply or
timeout. xxxReceiveMessage consumes that original receive list. Importing
this full path would therefore install an original local delivery mechanism,
not merely attach metadata to modern USER delivery. It is outside the
current sole-native-delivery admission even though its code is original.

Design disposition: do not claim a complete implementation plan or a finite
total mirror budget from these rejected alternatives. A separately reviewed
boundary change would be needed before implementing an original local SMS
delivery slice. Such a change must still prove its finite dependency closure,
native-window interoperability and capture/lifetime behavior; it does not
authorize the full USER server. Alternatively, retaining sole native delivery
still requires an as-yet unproved association mechanism or an explicitly
accepted scheduling-semantic difference. Neither can be silently selected
as part of the standing research authorization.

### Minimal SMS subset review: no independently complete candidate admitted

The function-level lifetime review gives the following dispositions. These
are dependency conclusions, not a claim that all possible designs are
impossible.

| Original sendmsg.c owner | What the body actually owns | Why it is not an independent native-delivery identity adapter |
| --- | --- | --- |
| AllocSMS / FreeSMS (150 / 193) | Lookaside-zone/pool allocation | Does not supply sender identity or association; porting only this allocator would retain no missing execution algorithm |
| _ReplyMessage (227) | Exact current SMS, reply flag/result, sender wake, directed scheduling and optional callback reply | Requires an already correct psmsCurrent; cannot create the missing association, and its callback-reply branch calls xxxInterSendMsgEx |
| xxxInterSendMsgEx (1235) | Parameter capture, receiver-list insertion, sender-stack association, delivery wake, wait, timeout and copyback | The substantial original behavior includes delivery/wait ownership explicitly excluded by the current native-only transport rule |
| xxxReceiveMessage (1876) | Removes from the original receive list, installs nested current SMS, calls hooks/procedure, replies or transfers freeing | Cannot merely wrap a native callback: it consumes its own pending message and also determines whether the message should execute |
| SendMsgCleanup / ReceiverDied / SenderDied (2283 / 2453 / 2599) | Global live-SMS traversal, receive-list unlink, synthetic reply/wake, task scheduling and lifetime transfer | Depends on the same send state machine and delivery lists; a separate native delivery must not receive a second independently synthesized completion |
| UnlinkSendListSms (2689) | Live-record unlink, window unlock, captured-parameter release and allocation release | gpsmsList is a lifetime registry, not itself a delivery queue; nevertheless this function assumes the original window/capture/allocation ownership and does not solve receive association |

Consequently neither "copy only lifetime helpers" nor "import the complete
send/receive functions unchanged" is a complete candidate within the current
approved transport boundary. The former leaves the causal association gap;
the latter changes that boundary. Deleting the existing adapter before a
replacement is proved would remove functionality rather than satisfy the
owner's same-repair replacement requirement.

The next implementation decision requires boundary review, not another
application-discovery test: either admit a bounded original same-worker WOW
SMS delivery design for evaluation (native destinations and external callers
must still have an explicit interoperability design), or retain native-only
delivery and supply/approve a different proved association/ordering contract.
Do not silently weaken early-reply assertions, introduce a second scheduler,
or treat a proposed boundary expansion as an approved source import.

No new mirror file or production edit results from this subset review.
The frozen wcall16 candidate remains +21/-2 versus HEAD; that is existing
unverified work, not this review's change or a complete S2 budget. A total
added/deleted mirror-line budget remains unavailable until this architectural
decision and its dependency-complete design are settled.

### Approved local-SMS candidate: dependency groups and first binding decisions

Owner approval now removes the native-only research restriction for
same-worker WOW-to-WOW SMS. It does not make the previously rejected native
wrapper an implementation candidate or authorize importing all of sendmsg.c.
The candidate must connect these groups together:

| Group | Original owner and existing entry | Binding/deletion requirement |
| --- | --- | --- |
| Admission and capture | WU32SendMessage's existing 16/32 thunk, then xxxInterSendMsgEx and its message-specific capture owners | Classify by retained live task/window ownership, not filename or message-number heuristics. Same-thread, native destination, broadcast and external callers need separate explicit dispositions. Do not use raw sender pointers after timeout/early reply |
| Send/receive identity | sendmsg.c allocation, psmsSent stack, psmsReceiveList and xxxReceiveMessage's psmsCurrent stack | Extend the existing finite thread/message carriers to original required fields; remove stack pseudo-SMS in native_call_begin/end in the connected repair, not retain both mechanisms for one send |
| Wait/wake | queue.c::xxxSleepThread / SetWakeBit and existing taskman selection | Original wake-mask and exclusive-wait rules own readiness. Bind unavailable event waits, not invent another runnable list or equate native queue presence with local SMS readiness |
| Dispatch | xxxReceiveMessage hooks and ScSendMessageSMS; selected DispatchClientMessage for actual WOW callback | Original logical window procedure and parameter representation must survive native gateway indirection. Native controls, subclasses and hooks prevent treating direct callback invocation as universal dispatch |
| Query/reply | WU32InSendMessage / WU32ReplyMessage, original current-SMS query and _ReplyMessage | Select the reply owner from the actual nested receive context, not the destination of an outer native call; native arrivals must not inherit an unrelated local psmsCurrent |
| Completion/retirement | SenderDied / ReceiverDied / SendMsgCleanup / UnlinkSendListSms | One completion/lifetime owner per send; preserve early-reply receiver lifetime, timeout copyback suppression and task/window retirement without an independent native synthetic reply |

Concrete source-backed seams:

- wumsg.c::WU32InSendMessage and WU32ReplyMessage currently call native
  InSendMessage/ReplyMessage. precomp.h already includes
  wow_user_private_access.h with WOW_USER_PRIVATE_REDIRECT. Adding the
  query/reply binding there can leave these original thunk bodies unchanged:
  planned additional wumsg.c/precomp.h mirror edits for this seam are zero.
  WU32ReplyMessage's warning about invalidated flat pointers after scheduling
  remains part of the callback/borrow acceptance, not a reason to move copyback.
- wow_user_message_bridge_sync currently clears QS_SENDMESSAGE after native
  PeekMessage, and receive only asks native USER to dispatch. This must be
  replaced as a whole for the new local receive state; retaining it would erase
  the pending bit while psmsReceiveList still contains work. Native queue state
  and original local pending sends are distinct facts, even if both wake the
  same host thread.
- queue.c::SetWakeBit updates pending/change bits and increments original WOW
  task events only when the bit intersects the current wake mask. The existing
  host wait's native-message event increment is not a substitute for this
  original algorithm. Recovery requires its actual input/idle dependencies to
  be closed, not a new local counter policy.
- xxxSleepThread checks local receive work unless QS_EXCLUSIVE is requested.
  Its dependency set includes system-hook expunge and input-idle processing
  as well as event waiting; none may silently become a success-returning stub.

Native-loop servicing is a remaining design requirement, not a test-only
detail. An event can wake the project's MsgWait loop but does not by itself
make an arbitrary native modal loop or blocked native SendMessage call drain
the local SMS list. A posted wake notification alone is therefore not a
complete servicing design. Any native notification carrier must preserve
exclusive waits and must not become the guest-visible current message or
cause ReplyMessage to acknowledge the notification instead of the real send.
Likewise, an external native callback nested inside a local receive requires
its own receive-origin scope; a process/thread-level "local mode" flag is
insufficient. These interoperability obligations remain open, with no
production import or complete mirror budget claimed by this group map.

### Local receive/native hook interoperation: actual original dependency

The receive-side hook boundary is not just a scalar notification. Canonical
sendmsg.c::xxxReceiveMessage builds CWPSTRUCTEX/CWPRETSTRUCTEX with the current
psms before invoking the original hook owner. kernel/srvhook.c::xxxHkCallHook
reads that private field, suppresses the hook if SMF_SENDERDIED or SMF_REPLY
is set, and routes the actual message through ScSendMessageSMS with
SCMS_FLAGS_INONLY. client/cltxt.h::fnHkINLPCWPSTRUCT then constructs the public
CWPSTRUCT for DispatchHook. The SMS identity does not pass to the public hook;
it protects parameter lifetime and conversion before the hook runs.

This disproves a proposed easy dispatch binding: implement ScSendMessageSMS
as a same-thread native SendMessage and assume native hooks supply the entire
original hook contract. For example, a guest callback can reply through the
local original SMS and continue. The local SMS is now replied, but the
independent native invocation has not necessarily been replied. The original
srvhook guard would suppress its hook call; the native hook owner has not
been given that local completion state. Invoking both owners can instead
duplicate hooks. This is a source counterexample, not a runtime observation.

Selected WOW consumers are real: wuhook.c::WU32SetWindowsHookEx registers the
thunk with native SetWindowsHookEx, while WU32CallNextHookEx reaches
wowhooks.c::WU32StdDefHookProc using the existing hook-state record. A local
dispatch design must account for installation, chain continuation and
unregistration together; directly calling one stored hook procedure would
not preserve that chain. This bounded dependency concerns message-dispatch
hooks; it is not admission of the whole separately queued hook capability.

For native receive-origin research, the public
[CallWndProc contract](https://learn.microsoft.com/en-us/windows/win32/winmsg/callwndproc)
exposes whether the send originated on the current thread, but not an exact
sender identity. This can inform a local-versus-native nested-scope candidate;
it does not certify a balanced before/after lifetime or original hook
parameter conversion. The inspected NT4 client uses CI_INTERTHREAD_HOOK;
do not copy its Boolean polarity into the modern public callback binding
without reconciling the different contracts. Hook order, callback exceptions,
native subclasses and guest work occurring inside a hook before WND dispatch
must be included before choosing this carrier.

Disposition: keep original message conversion/hook lifetime owners in the
candidate dependency graph. Do not add a SendMessage-based dispatch stub or
install a new hook as a purported complete origin solution yet. Next source
selection must resolve this hook-chain boundary and the previously identified
native-wait servicing together; neither is satisfied by a tagged window
callback or an ordinary local-message success case alone.

### Candidate size and hook-chain closure inventory

Source-only inventory `build/M0-T447/S2/audit-sms-candidate.ps1` extracts the
named candidate definitions with balanced braces after masking comments and
strings, retains SHA-256 per original file and lists call-like tokens. The
successful background result is in
`build/M0-T447/S2/sms-candidate-inventory-r2/` (PID 26452, exit 0). The first
run exited 1 because its PowerShell lacked Get-FileHash; the corrected reader
uses .NET SHA256. This is not a runtime or compiler test.

| Named candidate family | Functions inspected | Physical function-span lines |
| --- | ---: | ---: |
| sendmsg.c allocation, send/receive/reply and cleanup | 10 | 1589 |
| queue.c sleep/wake | 2 | 220 |
| hooks.c call/chain owner | 2 | 329 |
| srvhook.c hook invocation/conversion owner | 1 | 376 |
| Total named candidate, not transitive closure | 15 | 2514 |

There are 147 caller/token pairs, including macros, casts, assertions,
conditional compilation and indirect calls. They are review inputs, not 147
proved runtime dependencies. Function boundaries were checked against the
previously read original send/receive/lifetime definitions and the newly read
hook bodies. These counts include comments/blank lines, exclude signatures
on preceding lines, and are neither executable-line counts nor a proposed
mirror diff. No source import is justified solely by this inventory.

The hook-chain trace closes an important false simplification. Selected
wuhook.c registers a native HHOOK through SetWindowsHookEx; WOW's existing
state table retains that HHOOK plus the guest procedure and thunk index.
ThunkCallWndProcHook16 ultimately calls native CallNextHookEx with the same
HHOOK. Explicit unhook, module cleanup and task-owned cleanup all return to
native UnhookWindowsHookEx. This table is an alias/registration owner, not
an original USER hook chain that can be invoked independently.

Canonical hooks.c::xxxCallHook2 instead depends on the original current-hook
stack, hook-object locks and destruction state, task/global chain traversal,
WOW-process filtering, optional module loading and WH_DEBUG interception.
Its substantive behavior cannot be replaced by iterating WOW's alias table.
Conversely, copying all 319 lines without resolving those dependencies does
not create a finite binding. Keep that original algorithm as the reuse
candidate; classify which branches are required by the admitted same-worker
domain and which remain native/external owners before selecting its slice.
Do not delete the original WOW alias table: a correct replacement of a native
hook-chain binding would still need the table's guest identity and cleanup
consumers. Dispatch, CallNextHookEx and all three unhook routes must choose
the same owner, rather than registering one hook into two independent chains.

### Hook scope and deferred-destruction boundary

The original chain has a finite, explicit traversal policy: PhkFirst starts
with the thread's list, then the desktop-global list; _PhkNext crosses from
the end of a non-global list to the global head. xxxCallHook2 additionally
filters WOW-installed non-inter-send hooks to 16-bit threads in the same
process. A worker-wide hook registry alone must not discard the distinction
between task-targeted and global hooks, or admit those hooks into unrelated
native processes.

UnlinkHook and FreeHook prove a necessary lifetime case: an executing hook
may unhook itself and subsequently call CallNextHookProc. UnlinkHook removes
the list membership without clearing sphkNext. FreeHook marks destruction,
defers final freeing while the object is locked, and releases the retained
next-hook reference only on final destruction. xxxCallNextHookEx uses the
current hook scope, not an independently guessed next alias-table entry.
This is required original behavior, not optional defensive bookkeeping.

Current wow_user_object_bindings.h is explicitly a native-window companion
table. Its implementation publishes/retires TYPE_WINDOW entries and relies
on native object identity; it supplies neither HMAllocObject(TYPE_HOOK) nor
the original mark-destroy/thread-lock/deferred-free contract. Do not reinterpret
that table as a complete USER object manager or add a hook object with plain
HeapFree at UnhookWindowsHookEx. The minimal original handle/object lifetime
subset is now an explicit dependency of the local-hook candidate, subject to
the same finite-slice/no-full-server admission.

The expanded source inventory at
`build/M0-T447/S2/sms-candidate-inventory-r3/` (PID 26416, exit 0) adds original
registration, traversal, unhook and thread-cleanup consumers: 24 named
functions, 3186 physical function-span lines, 205 call-like token edges.
The prior 15-function count remains an initial candidate, not a claim of
closure. Neither number includes the unresolved parameter-conversion/object
lifetime dependencies or represents a production import or mirror-diff
budget. The next review must classify that object lifetime subset and the
native-loop/receive-origin boundary before freezing an implementation set.

### Hook object lifetime: algorithm versus storage boundary

Canonical handtabl.c now resolves the deferred-destruction dependency more
precisely. HMMarkObjectDestroy (757) marks HANDLEF_DESTROY and, while locked,
clears HANDLEF_INDESTROY and returns FALSE. HMUnlockObjectInternal (1006)
dispatches final destruction only for a marked object not already being
destroyed. HMDestroyUnlockedObject (1437) sets the reentrancy guard before its
TYPE_HOOK branch calls FreeHook. This is a reusable original lifetime
algorithm candidate, not evidence that the existing adapter supplies it.
HMAssignmentLock also locks the replacement before unlocking the old object
and treats self-assignment as a no-op; retaining only a decrement-and-free
helper would lose these necessary chain-reference semantics.

HMAllocObject (314) and HMFreeObject (575) are a different dependency class:
global handle pages, shared handle entries, handle uniqueness, desktop heap
allocation and desktop references. Hook allocation takes the desktop-heap
branch; freeing advances handle uniqueness before returning the slot to the
free list. They cannot be bound to bare HeapAlloc/HeapFree while claiming
unchanged original handle validation or stale-handle behavior. Importing the
whole allocator would expand beyond the admitted finite candidate.

Design disposition: retain original mark/unlock/assignment/free-hook behavior
as candidates; do not import the general allocator. A bounded hook storage
binding remains to be specified together with its handle validation and all
registration/CallNext/unhook consumers. This is an unresolved implementation
boundary, not an approved new handle namespace. Native window identities
remain native-owned and must not be inserted into a replacement USER table.
The source scan of current wow32-dll/opennt-host finds no implemented HM
mark/unlock/assignment binding; wow_window_words_binding.c's lifetime analogy
is not such a provider. Native-loop reception and implicit native sends are
still separate open dependencies. No finite total mirror-diff budget or
runtime repair is claimed by this classification.

### Native receive servicing: notification candidate and implicit-send witness

Source review separates two problems previously grouped as native-loop
interop. A local SMS needs a receiver-thread entry; an implicit native send
needs a truthful receive origin. Solving the first does not solve the second.

Canonical sendmsg.c::xxxSendNotifyMessage (968) delegates to
xxxSendMessageCallback without a result callback. Cross-thread notification
passes a null ptiSender to xxxInterSendMsgEx, marks SMF_RECEIVERFREE, wakes the
receiver and returns without a reply wait. _ReplyMessage (227) does not wake
a sender for this notification form. The public
[SendNotifyMessage contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendnotifymessagea)
likewise distinguishes synchronous same-thread invocation from asynchronous
cross-thread invocation. This supports evaluating a receiver-owned private
notification window as a wake carrier, not as the local guest-message owner.

The finite candidate must satisfy all of these obligations before admission:

- Enqueue/capture/retain the original SMS before notification; carry no raw
  SMS pointer whose lifetime depends on eventual native delivery. The receiver
  drains its original list, not a second delivery queue.
- Acknowledge only through the original receive/reply algorithm. The private
  notification is never the guest's InSendMessage/ReplyMessage context.
- Invoke original receive processing only after original task selection and
  with its required USER lock. Same-thread notification cannot accidentally
  create another guest execution owner.
- Preserve exclusive waits. A notification callback must not bypass local
  QS_EXCLUSIVE or force an extra PeekMessage during a native blocked send.
  A deferred pending list must be reconsidered when exclusivity ends.
- Retire the notification endpoint with its thread and validate liveness on
  delivery/failure. A late notification cannot resurrect a retired task.

No claim is made that every native modal loop services this endpoint: modal
loop source and supported native API contracts still need to establish that
coverage. This is a candidate wake mechanism, not a completed implementation.

The implicit-send witness is concrete: focusact.c::xxxSendFocusMessages
(222 onward) takes the old focus window from pti->pq->spwndFocus and sends it
WM_KILLFOCUS before sending WM_SETFOCUS to the requested window. It permits
reentrant focus changes and rechecks the new focus before the latter send.
Current wow_user_native_call_begin instead determines the supposed receiver
only from the wrapper's input HWND; SetFocus(NULL) has no such receiver at
all, although the original path can still send WM_KILLFOCUS to the old one.
For attached input queues the old/new owners need not be the same task.
Thus merely adding local explicit-SendMessage delivery leaves this confirmed
coverage defect in place. Do not infer an actual send/reply association from
the SetFocus argument, even when the argument belongs to the caller itself.
This is a source-level contract counterexample, not a newly reproduced
SOL/WRITE failure. It fixes the next design question: native-origin guest
callbacks require their own execution/receive scope independently of the
local-notification scope and independently of the outer API's target HWND.

### Receive inheritance and bounded callback-entry budget

Original sendmsg.c::xxxSendMessageTimeout's same-thread branch (after the
thread comparison at 712) invokes hooks and the client procedure without
replacing pti->psmsCurrent. xxxReceiveMessage instead saves and replaces it
at 1955--1956 and restores it at 2194. Therefore a nested same-thread send or
CallWindowProc must inherit the enclosing received message. A universal
"native window callback means new native receive" scope is rejected: it
would redirect a valid reply away from an enclosing local SMS. Conversely,
letting every native entry inherit local state would misroute a genuinely
nested foreign receive. Execution ownership and received-message ownership
are distinct; the common execution gate does not solve receive attribution.

The source-backed execution entry set is now narrower than a new family of
thunks. These are proposed integration locations, not implemented providers:

| Entry | Earliest required boundary | Additional mirror budget for this boundary only |
| --- | --- | --- |
| Registered WOW window callback, including CallWindowProc to a WOW procedure | Wrap the registered pfnWowWndProcEx before original message thunking; leave DispatchClientMessage's original WOW/native branch intact | 0; adapter registration/call sites only |
| Dialog initialization and DLGPROC | Enter before W32InitDlg in dialog_return_hook and before creation->procedure in dialog_proc; terminal cleanup remains exception-safe independently of guest entry | 0; both entry owners are adapters |
| Native Hook callbacks | All 32 WU32SubStdHookProc stubs call WU32StdHookProc; acquire execution before its shared state and CURRENTPTD access, retain the original type switch and thunk bodies | Candidate +1/-1 in wowhooks.c: rename only the central definition; keep original call sites targeting an adapter-owned wrapper under the old symbol |

The hook candidate avoids adding 32 wrappers or changing each thunk. The
adapter declares/calls the renamed original body with the same four-argument
APIENTRY ABI and releases its scope in a finally block. A global preprocessor
rename is not equivalent: it would also redirect the 32 callers around the
wrapper. Source search finds the central definition and 32 stub calls in
wowhooks.c; its declaration remains under the original wrapper symbol.
The original function and canonical counterpart both begin at line 458.
ThunkCallWndProcHook already performs WM32 conversion and stackalloc16 before
CallBack16, so gating only CallBack16 is too late.

This is a local, reviewable mirror budget, not the whole S2 budget. It does
not admit implementation, change original Hook policy or repair original
shared Hook-state behavior. Reuse assessment: the original thunk bodies are
directly retained; the missing host execution boundary is adapter mechanics;
the one-line definition seam is a proposed minimal intrusion, not permission
to replace the original dispatcher. Native receive-origin discrimination,
local SMS parameter conversion and persistent scheduler eligibility remain
unresolved parts of the integrated design.

### Public receive-origin probes do not establish a complete scope

The current public
[InSendMessageEx contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-insendmessageex)
returns SEND/CALLBACK/NOTIFY/REPLIED flags, not a receive identity or nesting
token; its parameter is reserved and must be null. Canonical
client/clmsg.c::InSendMessage (1902) similarly delegates a Boolean thread-state
query to the server. Neither is a direct replacement for the original
psmsCurrent save/replace/restore boundary.

There is a concrete ambiguity for the private-notification candidate. While
handling a local SMS under its native NOTIFY carrier, an ordinary same-thread
native callback must inherit the local receive. A nested foreign NOTIFY
receive must instead hide that local receive. A NOTIFY flag by itself cannot
identify which native receive is current. Matching HWND/message/arguments
does not add causal identity, and the notification's own identity does not
identify subsequent nested native receives.

Public WH_CALLWNDPROC provides a current-thread-versus-other-thread flag,
but installing an observer hook does not create an unconditional receive
boundary. The
[CallWndProc contract](https://learn.microsoft.com/en-us/windows/win32/winmsg/callwndproc)
allows a hook not to call CallNextHookEx, in which case later hooks do not
receive notification; the message is subsequently passed to the window
procedure. Original hooks.c::xxxCallNextHookEx (711) likewise traverses only
when called. A prior hook can therefore suppress our proposed origin observer
without suppressing the window callback. No uninterruptible before/after
scope is established by that observer. Exception-safe scope pairing would
also still require proof independently of the normal return notification.

Disposition: reject flags-plus-observer as the complete mixed local/native
receive-origin binding. This does not prove that every possible public
interop design is impossible, nor does it justify restricting other hooks,
reading undocumented modern USER internals or accepting a semantic change.
The execution-entry seams above remain independently useful candidates; do
not confuse their single-guest-owner protection with received-message
identity. The private notification remains only a wake candidate, not an
accepted complete transport. No production implementation is admitted by
this negative result.

### Integrated-review decision correction

An implicit-native-send-only scheduling exception is not a sufficient design
decision: the mixed local/native receive-origin ambiguity above remains even
if that ordering difference is accepted. Do not present that answer as an
implementation unblock. The earlier owner question is superseded.

A native-only transport/reply design would avoid needing to choose between
two current-receive owners, but the early-reply counterexample at the start
of this record remains an observable difference from original WOW. Owner
has therefore been asked explicitly whether that difference may apply to
all transport/replies. No answer is assumed. An approval would permit
revising that acceptance contract and completing the native-only design;
it would not prove persistent eligibility, callback/lease cleanup, application
recovery, or any production gate. Original taskman remains the scheduler,
and the pseudo-SMS adapter remains a deletion target, not an accepted bridge.
Without approval, retain the original behavior requirement and do not ship
the relaxed native-only candidate. This is a material product-contract
choice, distinct from the standing authority for source research.

Precision correction after second-role source review: do not describe the
original contract as an unconditional guarantee that the sender executes
before ReplyMessage returns. _ReplyMessage calls DirectedScheduleTask when
a sender exists and either peer is 16-bit. It calls xxxSleepTask only when
the receiver is 16-bit and the sender's psmsSent is this exact SMS.
DirectedScheduleTask adjusts event counts/list position and conditionally
raises the sender's priority; it does not itself assign ptiScheduled.
xxxSleepTask retains task-lock and priority-selection rules. Thus the required
contract is message-associated conditional handoff, not unconditional peer
execution. The earlier two-task, unlocked early-reply counterexample remains
a required case, but must not be generalized to task-locked, older-message or
native-receiver cases. The owner-facing decision wording overstated this
guarantee and has been corrected in the report; no acceptance or semantic
exception may be inferred from an automatic goal continuation.
