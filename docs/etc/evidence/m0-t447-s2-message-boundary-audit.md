# T447 S2 message boundary audit

## Inputs and method

Source-only follow-through on f950ffd38 and the uncommitted S2 callback
candidate. No new runtime experiment or production release is claimed.
Read selected wow_user_task_lifecycle.c, wow_user_message_bridge.c,
wow_user_registration_bridge.c, wow_window_words_binding.c,
wow_task_order_bindings.h and original selected taskman.c. Compare canonical
O:/repos.external/OpenNT/windows/core/ntuser/kernel/sendmsg.c, SHA-256
AF68BADCB022EFCB3947968FC6E3DBF148B697072EBEB2898F97FBCB774039F3.
Use bounded Get-Content ranges and rg symbol/consumer searches; retain
[S1 findings](m0-t447-s1-execution-baseline.md) and T422 E94/E95 stopping
boundaries. This is a live audit checklist, not proof of exhaustive coverage.

## Systematic audit boundary and inventory

Owner now requires the whole execution graph before further patches. This
section is the coverage denominator, not a declaration that every row is
audited. The message detail below is one part of it. Read selected build
definitions, not a directory-wide symbol list: New-T404S5Wow32ProviderNinja
selects the 77 original WOW C bodies and explicit support/USER bodies;
New-T310OriginalSoftpcNinja selects worker-local wow_user_runtime and
wow_user_session_binding. A source's existence alone does not establish
selection or complete reachability.

| Phase / stable scope | Production entry and original mechanism | Required forward and reverse audit | Current coverage |
| --- | --- | --- | --- |
| Session bootstrap / C09,C10 | ntvdm-exe session hooks -> wow_user_session_attach -> worker-local runtime TLS; WOW DLL imports worker current-thread operations | Hook registration, attach failure, bind/context installation, each unbind caller, DLL/session teardown ordering | Build ownership and bind/unbind bodies read; complete caller/failure graph still open |
| USER registration / C09 | UserRegisterWowHandlers -> init, hung, yield, wait and cleanup callbacks; original queue/taskman/exitwin selected in DLL | Every published handler, precondition, failure unwind and pairing with guest caller | Registration table read; individual consumer reconciliation open |
| Task creation / C09,C10 | registered_init_task -> lifecycle init -> original xxxInitTask | Name allocation, task/queue/list insertion, lock acquisition, partial-init reverse order | S1 identified missing name cleanup and attachment completion; full original-slice design open |
| Outbound calls / C01,C02 | WU32 message thunks -> private_access -> native call begin/end; modern USER is sole transport | Same thread, shared WOW, foreign/native, implicit calls, timeout, errors, task lock, restoration | Source-derived gaps below; receive association unresolved |
| Receive and waits / C01,C02 | Get/Peek/Wait bindings -> lifecycle -> original taskman -> message bridge/native queue | Posted versus synchronous sends, filtering, modal entry, wake/cancel, unexpected status and shutdown | Existing routes read; complete blocking-call census and modal callers open |
| Callback families / C03 | WND wmdisp32, dialog wudlg, hooks wowhooks, timer wutmr, enum wuwind, resources wres16 -> original CallBack16 | Every selected callback family, thunk/page borrow, execution-owner acquisition, CPU recursion, unwind/restoration | Family anchors confirmed; no claim all families use the WND task scope; per-family paths open |
| Frame/address domain / C03 | Original GetPModeVDMPointerAssert and CallBack16; bounded lease/SAS/segment providers | Demand loading, remapped storage, nested entry, frame commit, failures and lease end | S1 gaps confirmed; mapping equivalence unresolved; local edits frozen as candidates |
| Reply and resume / C01,C02 | Original sendmsg reply contract -> selected taskman; current native reply and sender completion | Exact normal/early/duplicate reply, nested previous state, live receiver after timeout, one execution owner | Detailed source mismatch established below; integrated design open |
| Yield / C09 | Registered YieldTask/DirectedYield/WaitForMsgAndEvent -> lifecycle -> original taskman | Scheduling versus USER data lock, event accounting, task locks, hung registration and teardown | Bodies selected; all edges and failure behavior not yet certified |
| Retirement / C10 | ModuleUnload cleanup distinct from runtime unbind -> task retirement -> session/thread exit | Windows/objects before task free, queue attachment references, waiter/name release, refusal/retry and worker loss | Known source gaps retained from S1; complete reverse graph open |

Three distinct lifetimes must remain explicit: worker SRW USER data protection,
original taskman WOW execution permission, and guest frame/page borrow epoch.
Releasing the first does not release the second; a finally block does not
prove successful restoration of either. Worker runtime unbind can refuse
retirement and leave TLS intact; audit all terminal callers rather than
assuming they honor that failure.

Repair groups will be finalized only after the above paths reconcile with
the inherited C01--C03/C09/execution-C10 checklist. Each group must name its
original reusable slice, unavailable boundary, autonomous code to delete,
state/failure dependencies and exact acceptance cases. Do not declare these
rows complete from a green focused test. W02 object/input and W03 startup
provider boundaries stay explicit, not silently excluded from dependency
analysis or silently absorbed as implementation scope.

## Detailed message obligations

| Edge | Original contract and current binding | Status / required repair proof |
| --- | --- | --- |
| Send versus reply transition | sendmsg.c xxxInterSendMsgEx calls DirectedScheduleTask(sender, receiver, TRUE, exact SMS) after publishing the send. Current native_call_begin only deschedules; native_call_end calls the FALSE direction with an outer-call record. | Exact send/receive association remains unresolved. Do not move TRUE before native publication or pretend the outer record is received SMS. |
| Early and duplicate reply | Original _ReplyMessage rejects absent/already-replied SMS, records reply, wakes sender, schedules receiver-to-sender, and sleeps the receiving Win16 task only when sender still waits on that SMS. Current WU32ReplyMessage calls native ReplyMessage without that WOW execution edge. | Missing receiver-side handoff confirmed. Native return and guest execution permission are distinct; a generic yield is not proof of original equivalence. |
| Ordinary receiver return | Original xxxReceiveMessage restores previous psmsCurrent, clears busy, and performs automatic reply only if not already replied; it also handles transferred freeing. Current callback_leave only deschedules a callback it resumed. | Neither callback scope nor outer-call completion supplies the complete reply identity/lifetime. Nested previous-receive state must not be guessed from HWND or message tuple. |
| Timeout / native failure | Original sender distinguishes timeout, restores psmsSent, and transfers SMS disposal when receiver remains busy. Current WOW_NATIVE_CALL uses the same native_call_end for SendMessageTimeoutA regardless of result; end receives no result, timeout flags or completion classification and may enter xxxSleepTask(TRUE,NULL). | Confirmed information loss at the adapter seam. A bounded native timeout is followed by an unbounded scheduler restoration path; actual stall on this branch is not claimed. Required design must distinguish return causes without inventing a second transport. |
| Native/unknown peer | begin does not transfer execution for an unenrolled target; only caller-held USER data lock is restored. Original send/reply taskman updates nSendLock/nRecvLock across WOW/native or separate scheduler boundaries. | Cross-native induced reentry still needs source-complete attribution. Native same-thread success cannot certify the cross-thread branch. |
| Implicit window operations | BringWindowToTop, SetWindowPos, MoveWindow, ShowWindow, EnableWindow, focus/activation, destruction, parent/text/placement and explicit sends share WOW_NATIVE_CALL. | Initial target HWND is not proof of the receiver of every induced nested message. Audit these classes before treating one target-aware bracket as a complete received-message binding. |
| Receive / queue observation | bridge receive performs native PeekMessage for synchronous sends, then sync performs another non-removing peek. Neither creates private receive identity. | Existing dispatch route, not a missing API. The second peek and callbacks inside both calls require reentry accounting, not a local SMS queue. |
| Task loss / unwind | end re-resolves receiver by thread ID and falls back to original DirectedYield if missing; finally performs restoration. Receiver callback finally discards callback_leave failure. | Missing peer, restoration failure and terminal cleanup remain open; do not count finally existence as successful restoration. S3 owns final task retirement, S2 owns its message-boundary behavior. |

## Callback census and failure-contract dependencies

Source census uses the same sources-manifest selection shape as the provider
generator: exclude the !IF 0 block, select C filenames, then find CallBack16
calls in those files. Exclude the function definition and commented examples;
translation-unit selection is not proof that every conditional body executes.
The selected files containing call sites are:

- wow32.c, walias.c, wcall16.c, wcall32.c, wdde.c, wres16.c;
- wmdisp32.c, wmsgem.c, wgdi.c, wgfont.c, wgmeta.c, wkman.c;
- wshell.c, wudlg.c, wumsg.c, wuser.c, wutmr.c, wuwind.c;
- wowhooks.c, wcuricon.c, wsubcls.c, wsext.c, wcommdlg.c, wdib.c.

wddeml32.c also contains a textual call but is not selected by this manifest;
do not count it as production callback coverage or add it just because it was
found. The complete family inventory exceeds the initial WND/dialog/hook/
timer/window-enumeration list: GDI object/font/metafile/line enumeration,
printing abort, properties/gray-string, common-dialog hooks, word-break,
Winsock blocking hook, subclass thunk creation, module/resource/memory/DIB
services, foreground-idle/forced-exit, and exported WOWCallback16/Ex all need
an explicit execution-contract disposition. Their subsystem behavior remains
with its own owner; the shared callback entry/return obligation belongs here.

Concrete failure-contract findings from selected bodies:

| Caller | Current source fact | Integrated design obligation |
| --- | --- | --- |
| wcall16.c WOWCallback16Ex | Initializes output to argument size, calls CallBack16 without checking BOOL, then returns TRUE. Exported in wow32.def. | New adapter acquisition/release failures cannot be certified by testing CallBack16 alone; define truthful public result and output contract through original-compatible recovery. |
| wuwind.c W32EnumWindowFunc | Local fReturn is not initialized before unchecked CallBack16 and is returned afterward. | Added early failure can leave the result unwritten. Trace all result-consuming callers, not a one-off initializer patch. |
| wutmr.c W32TimerFunc | Resolves timer, constructs parameters, directly calls CallBack16 with no return output. | Timer dispatch does not pass through the WND task-scope wrapper by virtue of sharing RET_WNDPROC. Prove inherited execution ownership from actual dispatch. |
| wumsg.c direct 16-bit CallWindowProc branch | Calls CallBack16 directly after thunk discrimination. | Distinguish guest-initiated synchronous reentry from native callback entry; blindly adding task acquisition to all callers is not justified. |
| wow_user_invoke_thunk | Enters a WND/CLS borrow scope around the original thunk and leaves in finally. | This object borrow is not the callback task scope or guest-memory lease. It cannot certify execution ownership for all native callbacks. |

The first two facts identify an interaction between newly fallible adapter
boundaries and inherited callers, not a demonstrated defect in unchanged
guest code. Do not globally initialize outputs or alter all original callers
before tracing original exception/return policy and choosing the smallest
coherent failure seam. Both pre-entry and post-CPU-return failure paths must
be included. Original mapping demand-load may itself reenter CallBack16, so
its active leases and caller state belong to the same dependency group.

Second-role review of this census rejects the inference that two wrapped WND
gateways cover all callbacks. Next audit step is to classify each listed
caller's inherited execution context, checked/unchecked result, memory
retention and unwind, with original-source alternatives. This census closes
the missing family-list question, not the per-family audit or runtime proof.

Follow-through of additional selected callback bodies confirms that the
failure-contract issue is shared, not specific to window enumeration:

| Family and selected registration | Callback/result behavior | Required disposition |
| --- | --- | --- |
| W32EnumFontHandler -> EnumFontFamilies/EnumFonts -> W32EnumFontFunc | CallBack16 result is unchecked; iReturn controls another callback and enumeration continuation | Preserve synchronous caller-owned execution; verify frame failure cannot expose an unwritten result or spuriously repeat a callback |
| WG32EnumMetaFile -> EnumMetaFile -> WG32EnumMetaFileCallBack | Unchecked result, then guest record/handle-table copyback and return of local iReturn | Callback failure and copyback are one transaction; returning FALSE from the new frame layer alone is insufficient |
| Winsock blocking-hook registration -> WWS32CallBackHandler | Unchecked callback writes local ret, whose low byte is returned; default hook instead peeks/translates/dispatches native messages | Blocking/modal execution ownership and callback failure must be reconciled together; selection of wsext.c alone does not prove installed Winsock provider availability |
| WU32DispatchMessage -> native timer dispatch -> W32TimerFunc wrappers | Timer callback has no output result and does not use a WND execution scope | Explicit guest dispatch has an inherited execution owner; native modal dispatch must be considered separately, not inferred from this route |
| GetStdClassThunkWindowProc in wsubcls.c | Callback failure is checked only by assertion before later processing | Retail failure behavior must be reviewed; a checked BOOL is not automatically a recoverable failure contract |

These rows classify selected source paths and local result/copyback behavior;
they do not yet certify every entry's runtime context. No modifications to
original font/metafile/Winsock behavior are proposed. The preferred repair
remains at the added frame/address boundary if the original failure semantics
can be retained there, rather than scattering new checks across original
callers. The complete registration/context proof remains required.

## Review and next action

### CCPU thread context and terminal ordering

Follow-through confirms selection of ntthread.c via the original ccpu386
sources manifest consumed by New-T310OriginalSoftpcNinja. c_main.c invokes
ccpu386SimulatePtr/Unsimulate; ThreadSimBuf stores per-thread recursive
simulation and exception jmp frames through TLS. host_CreateThread in
nt_thred.c creates suspended, marks potential new CCPU thread, then resumes
as requested. W32Thread obtains original task execution permission through
InitTask, selects its initial VDM stack and calls host_simulate. These owners
already exist; no new TLS callback engine or CPU context manager is justified.
Per-thread native return stacks are distinct from a separately owned virtual
machine register set, and do not independently serialize guest execution.

The reverse edge constrains the cleanup design: original host_ExitThread
calls ccpu386exitthread BEFORE ExitThread, whose project redirect enters
opennt_exit_thread/session_thread_unbind. ccpu386exitthread frees its thread's
simulation buffer. Thus moving guest-callback-capable native window cleanup
into the current unbind location without reviewing this ordering would allow
callbacks after the original recursion context has been released. This is
a design hazard derived from source, not an observed crash or authorization
to edit original CCPU internals. Required pre-exit cleanup and post-execution
retirement must be distinguished; do not weaken the live-window guard or
retry guest work after freeing its execution prerequisites.

The source also has allocation/depth failure returns and cleanup details in
unchanged CCPU code. Those are not automatically project defects and are not
new repair targets from this read. Apply source policy and existing original
owners; the currently proved integration obligation is ordering, not rewriting
the emulator. Original ThreadStartupRoutine's ordinary return and exception
path must be reconciled with explicit host_ExitThread before claiming all
terminal paths share the same release behavior.

### Execution exclusion is not the USER data lock

Read selected userk.h EnterWowCritSect/ExitWowCritSect, taskman scheduling,
worker runtime SRW binding, CallBack16's CCPU entry, nt_cpu.c and c_main.c
c_cpu_simulate. The selected EnterWowCritSect records CSLockCount and
CSOwningThread; it does not acquire a host mutex. The retail diagnostic
macros in wow_task_order_bindings.h do not enforce exclusion. Correct taskman
selection/wait ordering is therefore a prerequisite, not merely bookkeeping
that can be repaired after a native call returns. The inspected host_simulate
entry invokes original CPU simulation; it is not a second WOW task arbiter.
This is an entry-path observation, not a claim that all deeper CPU context
code has been audited or that concurrent CPU execution occurred in a run.

Consequences for the integrated design:

1. Preserve original taskman as the sole selection owner. Adding a mutex
   around CallBack16 would not recover original cooperative scheduling,
   nested send/reply ordering or modal yielding, and could deadlock reentry.
2. Native-boundary adaptation must classify entry, suspend, callback resume
   and return under the same execution contract. Do not independently patch
   ReplyMessage, modal waits and hook callbacks with unrelated yields.
3. Original frame/memory mapping and error handling must remain valid during
   that handoff; an object borrow or SRW lock is not a guest execution lease.
4. Terminal cleanup must retire message/callback references before removing
   scheduler and queue membership. A separate cleanup patch cannot certify
   execution safety while receive identity/lifetime is unresolved.

The emerging repair groups are thus (A) original callback address/frame and
failure contract across all callers, (B) native call/wait/callback/reply
execution integration, and (C) task initialization/retirement with shared
queue/object lifetime. They are design groups, not new admitted S tasks or
permission to implement independently. B depends on A and C's lifetime
contract; C must consume B's cancellation/peer-loss contract. Final review
must resolve these cyclic lifetime requirements before coding, with explicit
deletion of the superseded outer-call scheduling policy and incomplete
cleanup bindings where original composition is proved. Actual replacement
interfaces and original subset selection remain open, not preapproved by
this grouping.

### Blocking and modal entry classification

Read the private USER redirect header, wow_native_process.h, lifecycle wait
bindings, wow_public_user_facade.c, wow_title_policy.c, selected WU32 dialog/
menu thunks, common-dialog callers and WK32Yield/WinOldApp wait. Source facts:

| Class | Actual selected path | Execution audit disposition |
| --- | --- | --- |
| Guest Get/Peek/WaitMessage | Explicit private-access redirects to lifecycle/taskman/native message bridge | Existing scheduler boundary, with receive/reply and cancellation obligations already listed. |
| WinOldApp process wait | Original wkman.c loop calls MsgWaitForMultipleObjects; wow_native_process.h maps this to wow_user_msg_wait, which deschedules before native wait and restores through DirectedYield afterward | Already has execution handling; do not add a duplicate wait scheduler. Result/error/unwind and callback timing still need acceptance. |
| Task creation rendezvous | Original WK32Yield resumes new task then waits for creation event/thread before pfnYieldTask; original xxxInitTask forces the new task active | Not interchangeable with generic message wait. Preserve this original handoff ordering; do not blanket-wrap every WaitForSingleObject search hit. |
| Modal guest dialog | WU32DialogBoxParam's modal branch calls DialogBoxIndirectParamAorW -> wow_title_modal -> native DialogBoxIndirectParamA/W with original W32DialogFunc | Distinct from registered modeless ServerCreateDialog binding. Style creation scope supplies appearance, not task scheduling or full WND enrollment. Trace native modal callbacks and original private USER dependencies separately. |
| MessageBox / ShellAbout | Style wrappers surround native APIs only with appearance scope | No demonstrated WOW execution transfer in these wrappers. Native nested loop cannot be assumed to call the project's GetMessage redirect. |
| Popup menu | WU32TrackPopupMenu calls native TrackPopupMenu; not in private-access scheduling redirects | Audit native induced callbacks and blocked sibling tasks; absence of redirect alone is not runtime proof of deadlock. |
| Common dialogs | wcommdlg.c calls native APIs and registers original WCD32DialogProc for applicable hooks | Callback family and modal-loop ownership must both be covered. Template/resource semantics remain their existing owner's responsibility. |
| Helper/serial/hung waits | Additional waits occur in wkman.c, wow32.c and wucomm.c | Classify native helper thread versus guest execution before assigning a missing handoff; do not change them based only on API name. |

The macro boundary redirects calls compiled in this provider, not calls
inside USER32/COMDLG32. Therefore native modal loops are not automatically
covered by the guest Get/Peek bindings. This is a source-established coverage
boundary, not yet a chosen replacement loop or permission to implement one.
The unified repair design must explain how original task execution ownership
is maintained across every listed native call class while retaining native
transport. No new modal message pump is admitted.

Original follow-through: canonical client/dlgmgr.c DialogBox2 (989 onward),
SHA-256 8E2383F52E4E3F65F656BD492143D7A8ED5D0A2526AD8E6AF10D242465FF8D7A,
uses PeekMessage, WM_ENTERIDLE, NtUserWaitMessage and normal dialog dispatch.
It directly depends on private PWND/PDLG, end/result flags, validation and
focus/owner state. Original kernel/queue.c xxxSleepThread reaches
xxxSleepTask on the TIF_16BIT branch (3259); thus original scheduler handoff
is below the modal loop. Copying only DialogBox2's message loop is neither a
complete original composition nor a solution for native menus/common dialogs.
The smallest reusable contract to recover is the execution boundary shared
by native waits and callbacks; choosing its modern carrier remains open.

Adjacent original idle branch must be included: selected taskman.c checks
IsHooked and calls the foreground-idle hook with the task temporarily selected,
then clears selection and rescans because the hook may schedule new work.
Current host_ops idle_hooked always returns FALSE on the stated premise of
no registration. However selected wkman.c WK32WowSetIdleHook registers
WH_FOREGROUNDIDLE, stores hIdleHook, and cleanup unhooks it; wktbl2.h exposes
the thunk. The callback directly enters CallBack16(RET_FOREGROUNDIDLE).
Registration can fail, so this is not proof a live hook existed in prior runs.
It does disprove treating permanent absence as a source-established invariant.
Audit native-hook execution context and registration state together; do not
simply set idle_hooked TRUE, invoke a hook twice, or install another global
hook to compensate. This belongs to the same wait/callback ownership group.

### Initialization and terminal-owner follow-through

Source reads now span worker begin/cleanup, session bind/unbind/dispose,
worker USER binding, DLL registration/lifecycle, selected original
queue.c/exitwin.c and W32Thread's init retry. This identifies the following
whole-lifetime obligations rather than separate speculative leak patches:

| Phase | Observed ownership and mismatch | Repair-group constraint |
| --- | --- | --- |
| Worker attach | Worker binds the session, attaches USER hook, then starts guest-memory service. USER runtime/TLS lives in NTVDM, not DLL. | Preserve one TLS/runtime owner and account for each partially completed acquisition. |
| Hung-handler registration | Original exitwin.c allocates WOWPROCESSINFO, references the event and links shared WOW state; failed event reference frees its allocation. | Already original-owned; do not replace this normal/failure algorithm. Final release must pair with this registration. |
| Task initialization | DLL inserts its task and desktop membership before xxxInitTask. Original allocates app name, updates process startup state, allocates TDB, attaches queues, inserts scheduling state and enters WOW execution. | Rollback must distinguish which original phase completed. S1's app-name/queue gaps are part of this group, not sufficient standalone fixes. |
| Post-init notification | registered_init_task can return FALSE after successful lifecycle init if OpenNtBaseClientWowStarted fails. Original W32Thread retries pfnInitTask; current lifecycle rejects an already-bound thread, then W32Thread calls W32DestroyTask and host_ExitThread after retries. | Audit notification failure as a committed-task failure, not equivalent to pre-init allocation failure. Do not silently declare retry successful or reinitialize live task state. |
| Thread unbind | session_thread_unbind stops at a failed hook and keeps binding counts/TLS. runtime_unbind retains state when task retirement refuses live windows. opennt_exit_thread and four normal start thunks discard unbind results. | Join original window/task teardown with terminal thread lifetime; failure cannot be treated as completed release. Partial hook rollback and thread death require explicit disposition. |
| Worker cleanup | Guest-memory service ends before USER detach. Detach and session unbind results are discarded and worker flags cleared. session_dispose may reject remaining binding counts with ERROR_BUSY, yet worker bookkeeping is reset afterward. | Trace all cleanup consumers that still need guest state; preserve failure identity and required state until ownership ends. No runtime failure on this ordering is claimed yet. |
| Lifecycle final disposal | Search of src finds wow_user_task_lifecycle_dispose only at declaration/definition, not a production call. It would close WOWEXEC event/free process and object storage only when task list is empty. Original DLL detach unregisters base callbacks and destroys hWOWHeap, not these added allocations. | Distinguish process-exit OS reclamation from supported explicit detach/reuse. Do not call a blocking cleanup under loader lock or label process-exit reclamation a persistent leak. Establish the actual product shutdown owner before wiring disposal. |

Normal module cleanup is not terminal thread retirement. Likewise a failed
session dispose is not proof of a safely reusable worker. The bounded
mvdm_softpc_execution_run_until_return helper explicitly documents its
fixture-only CPU entry; its ignored unbind results must not be used as proof
of the ordinary host_main product path. Other search hits in console and
redirector threads require their own task-context classification, not blanket
changes merely because they call session_thread_unbind.

These findings extend the matrix's reverse graph through the product cleanup
caller. Final original W32DestroyTask/window-destruction sequencing and the
finite original thread-cleanup slice remain to be reconciled before the
systematic audit can close. No production implementation resumes here.

### Task cleanup is not native window destruction

Follow-through reads selected wkman.c W32DestroyTask, wuser.c ModuleUnload,
original cleanup.c, wow_cleanup_bindings.c and wow_window_native_proc against
canonical queue.c xxxDestroyThreadInfo (especially 1870--1995).

- W32DestroyTask performs WOW-side task bookkeeping, ExitVDM notification,
  optional pfnWOWCleanup, task-list removal and thread-handle close. It does
  not implement the original USER thread-destruction sequence.
- ModuleUnload marks TDF_TASKCLEANUPDONE before pfnWOWCleanup and may then
  issue a separate module cleanup. Both this path and W32DestroyTask ignore
  its BOOL result. The current adapted cleanup has fallible allocation,
  native class release and window-tree construction; failure propagation
  therefore belongs to the same integration audit as added callback failure,
  not merely a local missing free.
- Original PseudoDestroyClassWindows explicitly neutralizes procedures, not
  windows. The current binding DOES expose that changed procedure to native
  dispatch: wow_window_native_proc reads cleanup.lpfnWndProc. Thus this is
  not a disconnected copy of WND state and must not be replaced with a new
  neutralization algorithm. Normal WM_NCDESTROY detaches the enrolled binding
  in finally; dialog creation has a corresponding detachment path.
- Original xxxDestroyThreadInfo orders native thread objects/windows before
  SendMsgCleanup and scheduler DestroyTask. Current retire_current_thread
  rejects still-enrolled windows before native thread exit. Calling only
  W32DestroyTask or _WOWCleanup cannot establish the absent window-destruction
  prerequisite: neutralized but live windows still hold task references.

This separates an already-connected original operation (procedure
neutralization) from an unresolved lifetime edge (native terminal window
destruction followed by provider retirement). Do not remove the live-window
guard, manually erase enrolled windows, or assume native ExitThread will call
the project's missing post-destruction retirement hook. The integrated design
must identify a real destruction/retirement owner and preserve the original
window-before-message-before-task ordering without importing native USER's
private SMS lists. Object-specific destruction remains a W02 dependency;
execution retirement cannot claim completion while this prerequisite is open.

Second-role review distinguishes proven adapter facts from runtime claims:
the timeout result is genuinely unavailable to native_call_end, and original
reply scheduling is receiver-side. These facts extend S1's missing-state
finding; they do not prove a new application failure or SOL/WRITE causality.
The current frame-only candidate does not repair any row in this table.

Do not add a ReplyMessage wrapper as an isolated fix. First establish an
observable native receive/reply boundary and its nested lifetime, then map
the original scheduling contract through that boundary. E94/E95 forbid
guessing private SMS identity or importing an entire USER server; the owner
has rejected local SMS transport. If that boundary cannot express a required
original contract, record the exact unavailable input rather than substitute
a convenient scheduler. Callback resolution remains a separately identified
prerequisite, not a reason to omit these message obligations.

### Native observation contract and terminal-message closure

Source review on 2026-10-14 additionally follows canonical sendmsg.c
SendMsgCleanup, ClearSendMessages and ReceiverDied, selected taskman.c
DirectedScheduleTask/xxxSleepTask/DestroyTask, and the current native-call
and retirement bindings. These are source conclusions, not new runtime
results or attribution of SOL/WRITE initialization failures.

The modern observation alternatives have now been checked against their
documented contracts, rather than assuming a hook supplies original SMS:

| Interface | Available information | Missing input for the original scheduler contract |
| --- | --- | --- |
| InSendMessageEx | Current cross-thread send kind and replied state | Neither the sender identity nor a stable send identity |
| WH_CALLWNDPROC | Whether the current thread sent the message; CWPSTRUCT contains HWND/message/wParam/lParam | No originating thread identity for a foreign send; no identity distinguishing nested identical sends |
| ReplyMessage | Publishes the native reply and lets the blocked sender continue; receiving thread also continues | Does not transfer this project's WOW execution ownership or report the matching original task/SMS |
| Current native_call_begin/end | Sender and initially enrolled target, previous outer call, native return boundary | No association with each actual receive; end has no success/timeout/exception classification |

Primary contracts: [InSendMessageEx](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-insendmessageex),
[CallWndProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/callwndproc),
[CWPSTRUCT](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-cwpstruct),
[ReplyMessage](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-replymessage).
Canonical client/clmsg.c InSendMessage itself only queries thread state;
it does not provide the missing association. This rejects these specific
interfaces as a complete SMS binding; it is not a claim that all possible
native integration boundaries have been proved impossible.

Original taskman does not use SMS merely as an optional diagnostic label.
DirectedScheduleTask compares identity against the sender's psmsSent to
raise priority, and updates cross-scheduler nSendLock/nRecvLock using flags
on that same message. xxxSleepTask examines psmsCurrent and its sender,
receiver and reply/busy flags to avoid a documented deadlock. Consequently
assigning a new record to psmsCurrent without proving its association would
change execution policy, not just fill an unused field.

The required message-state closure is finite and must be designed together:

| Event | Original owner and effect | Current integration disposition |
| --- | --- | --- |
| Send publication | xxxInterSendMsgEx publishes the SMS before directed send scheduling | Outer native call is not publication; retain the rejection of pre-publication synthetic handoff |
| Receive entry / nested exit | xxxReceiveMessage installs and restores exact psmsCurrent | No production assignment found in WOW/worker providers; receive association remains unresolved |
| Explicit early reply | _ReplyMessage wakes the exact sender and performs receiver-side scheduling | Native ReplyMessage only; no equivalent receiver-side execution transition |
| Ordinary reply | xxxReceiveMessage replies only when not already replied | Generic callback leave does not distinguish this from early reply |
| Timeout while receiver runs | Sender marks reply/disposal ownership without assuming receiver returned | Current end loses cause and still restores through a generic scheduler wait |
| Window invalidation | ClearSendMessages distinguishes queued versus busy receives and schedules affected WOW senders | Native window transport cleanup cannot by itself update provider taskman state |
| Thread death | SendMsgCleanup routes exact messages through SenderDied/ReceiverDied before task destruction | Provider retirement goes from object cleanup to DestroyTask without this message-state closure |

Selected DestroyTask removes TDB scheduling/event records and wakes remaining
runnable tasks. It does not enumerate outstanding sends or perform
SenderDied/ReceiverDied. Current retire_current_thread checks live window
entries, but an absence of windows does not establish absence of outstanding
message/execution references. The native_call_end relookup avoids directly
using a freed receiver pointer; it does not prove that earlier scheduling
counts or the actual received-message relationship were settled. Do not
import gpsmsList or add a local message transport to fill this gap.

Second-role disposition: reject three tempting partial fixes -- a standalone
ReplyMessage wrapper, matching receives by HWND/message/parameters, and
relying on DestroyTask alone for message retirement. They each omit a
required state transition above. This closes those design alternatives, not
the message repair. Repair group B and group C must share an explicit
completion/cancellation boundary before either may be called complete.

The next design decision must establish a source-backed execution boundary
that supplies these required transitions without a second transport or
scheduler. If it cannot preserve exact original SMS-dependent behavior,
record the precise unavailable contract and seek a bounded architecture
decision; do not quietly substitute generic yielding or call a fixture pass
equivalence. Independent remaining audit work is callback-family ownership
and failure propagation, plus original initialization/queue retirement.

### Original callback return and mapping contract

Follow-through compares canonical base/mvdm/wow32/wcall16.c CallBack16
(581--819) with the selected body, wow32.h GETFRAMEPTR/FRAMEPTR,
wow32.c GetPModeVDMPointerAssert/W32Exception, dpmiselr.c descriptor
publication/GetDescriptorMapping, and the callback lease/SoftPC memory
providers. The original CallBack16 body has only its final TRUE return.
FALSE returns elsewhere in wcall16.c belong to other functions. The new
acquisition/release FALSE exits therefore add a recoverable-return contract
that its original consumers did not have; their unchecked BOOLs are not by
themselves evidence of an inherited original defect.

| Boundary | Original source behavior | Current adaptation / design consequence |
| --- | --- | --- |
| Callback address | GETFRAMEPTR calls protected-mode GetPModeVDMPointerAssert irrespective of current CPU mode | acquire_vp instead uses effective_address, which chooses real/VM versus protected mode. Replace this autonomous choice, not the original helper's protected-mode contract |
| Missing segment | Original resolver can invoke RET_FORCESEGMENTFAULT before returning the mapped pointer | Numeric protected_address only reads the descriptor base; simply changing effective_address to protected_address still omits original demand loading |
| Remapped selector | FlatAddress is populated through original GetDescriptorMapping, which checks selector range and matching LDT base, and removes stale mapping records | SAS numeric acquisition is not proof of access to the same storage; invoking the original resolver and discarding its result is not restoration |
| Commit before reentry | Original flush/free points precede stack selection and host_simulate | Keep the bounded no-alias-across-reentry invariant; raw-pointer restoration without its lifetime proof is not admitted |
| Post-return frame | Original reacquires returned frame, copies result, restores vpStack and succeeds | Current read/release failure may return FALSE after guest effects occurred. A pre-entry rollback cannot repair this case |
| Exception alternative | W32Dispatch/W32TryCall use W32Exception; its KillTask branch itself resolves vpStack and writes RET_FORCETASKEXIT | A generic RaiseException on all lease refusals is not yet a source-equivalent remedy, especially when the stack mapping is the failing resource |

The selected descriptor publisher explicitly separates native FlatAddress
from the guest-linear flat-address array (DIV-310). The page-domain code
documents original DIB aliases and deliberately avoids subtracting IntelBase
from arbitrary mapped pointers. This supplies a concrete rejection of that
shortcut for the callback repair as well. It does not assert that an actual
application uses a DIB mapping as its callback stack.

Second-role review decision for group A: the retained rollback candidate
does not meet the original return/mapping contract and must not be promoted
on focused success tests. The repair must first compose the original
resolver with a bounded lease of its actual resolved storage, or prove a
restricted frame-address equivalence for every admitted callback frame.
Its failure/unwind contract must cover both sides of guest execution. Do not
scatter default return values through original callbacks, introduce an
unbounded alias, or substitute an unproved exception policy. This remains a
design dependency, not a claim that the original resolver is uncomposable.

### Existing lease backend: reuse boundary and hidden CPU reentry

Read session/guest_memory_lease.c and .h, session.c memory entrypoints,
softpc/mvdm_softpc_guest_memory.c, mvdm_softpc_physical_mapping.c,
original ccpusas4.c and nt_mem.c VdmAddVirtualMemory, and wdib.c mapping
callers. The active provider generator selects CPU_40_STYLE/CCPU and does
not define the original lowercase i386 switch in its compiler flags.

The existing session lease is a copied buffer, not a pin of an arbitrary
resolved native address: acquire takes a numeric address, always reads it
(including WRITE acquisition), allocates from eight session slots, and
release writes back through the context's backend. There is no existing
resolved-host-pointer lease entry in this interface. Do not reinterpret an
x86 pointer as its numeric guest-address argument or replace the session
backend globally to accommodate one callback.

Conversely, the SAS backend is not limited to plain contiguous RAM. Its
original c_sas_loads -> bios_read_byte -> phy_r8/c_GetPhyAdd path reaches the
existing physical mapping adapter. Original VdmAddVirtualMemory installs
that mapping through VdmSetPhysRecStructs; the DIB callers also publish
VdmAddDescriptorMapping for FlatAddress. This is a concrete existing reuse
route, not a reason to invent a second alias manager. Exact equivalence of
the callback span still requires descriptor, paging and lifetime proof;
the two mappings' existence alone is insufficient.

An additional shared-lifetime dependency is now established by source:

1. guest_memory_lease_acquire chooses an inactive slot and stores its malloc
   buffer, then invokes context->read before setting the slot active.
2. The actual backend calls c_sas_loads; bios_read_byte performs xtrn2phy
   when paging is enabled. On a failed translation it can enter biosDoInst.
3. biosDoInst invokes host_simulate. Thus a backend read is not guaranteed
   to be a non-reentrant host memory copy.
4. If guest handling reenters a lease consumer, the still-inactive slot is
   eligible for selection again. The outer acquisition has not reserved
   ownership across that call. Actual application-triggered slot reuse has
   not been demonstrated, and is not asserted here.

The write backend likewise reaches bios_write_byte; source-level CPU reentry
must therefore be considered at acquisition and commit, not solely at the
explicit CallBack16 host_simulate statement. Existing outer leases, epoch
teardown and exception unwinding are part of this obligation. The eight-slot
limit is another added refusal source; increasing it does not establish the
original failure contract or repair reservation ordering.

Group A design disposition: retain the existing session-owned memory and
original SAS/mapping mechanisms; reject a separate callback mapping manager.
Before changing the mirror, reconcile resolver demand loading with backend
page-fault reentry and reserve/release/unwind ownership in the shared lease
boundary. Acceptance must cover nested acquisition during a backend call,
preexisting outer leases, backend failure, commit reentry and teardown,
alongside ordinary callback results. These are tests of the source-derived
contract after implementation, not exploratory application runs. The
specific minimal interface and original exception disposition remain open;
no production change is admitted by this finding alone.

### Lease consumer closure and acceptance coverage

The shared lease change is not WOW-only. Direct source consumers are
session memory entrypoints, mvdm_softpc_guest_memory copy/acquire/release,
mvdm_guest_location, and wow_callback_frame_lease. Their downstream callers
include DEM DTA/file/error operations, COMMAND environment/command records,
EMS copies, XMS reads, redirector transfer buffers and debugger reads.
These are impact boundaries, not authorization to rewrite their algorithms.
Preserve their numeric address and commit/discard contracts and include the
existing DOS/redirector regressions when this shared dependency changes.

Read tests/session/guest_memory_lease_test.c,
guest_memory_lease_boundary_test.c and the T335 callback-frame fixture.
Existing tests cover ordinary hydrated WRITE, discard, overflow, invalid
access, foreign/forged lease pointers, read/write failure, slot exhaustion,
end and epoch wrap. Their backends are memcpy implementations. The callback
fixture also substitutes selector resolution. None of these proves original
demand loading, SAS page-fault reentry or the full original CallBack16 path.
No tests were executed to discover these source gaps.

The proposed shared-boundary repair contract, pending whole-audit review:

| Transition | Required invariant | Existing test coverage / additional acceptance |
| --- | --- | --- |
| Acquire enters backend | Slot and buffer have a unique owner before any potentially reentrant read | Existing success/failure coverage retained; add nested acquisition from the read callback with distinct live buffers |
| Acquire fails or unwinds | Release only its own allocation; do not alter a nested successful acquisition or publish half-initialized output | Existing FALSE backend test is insufficient; include nested failure and the chosen exception-unwind contract |
| Release enters backend | Commit buffer and slot cannot be reused or freed during the write operation | Add reentrant write callback and nested release rejection; preserve commit/discard behavior |
| Context ends during backend execution | In-flight bytes cannot be freed underneath the backend; an ended epoch cannot publish a successful lease | Existing tests end only after acquire returns; define terminal ordering with group C before coding |
| Context/slot reuse | An old reference must not release a newly acquired object | Existing epoch checks inspect the current slot, not a caller-carried generation; determine whether caller lifetime prevents reuse or the interface needs a bounded identity correction |
| Callback integration | Demand loading and backend reentry cannot leave stale frames or a second execution owner | Test actual original resolver/CallBack16 after connection; memcpy-only fixtures remain auxiliary |

The source API returns a pointer to a reusable slot, so its stored epoch
does not by itself authenticate the generation held by a stale caller.
This is a limitation of the current handle representation, not proof that
ordinary callers actually retain one. The implementation decision must
either prove that complete caller lifetime excludes stale use, or explicitly
repair the identity contract; merely incrementing context->epoch is not a
solution. Similarly, marking active earlier addresses reservation only,
not teardown, commit reentry, original callback failure or generation reuse.

Single-person review disposition: the dependency is a shared added-adapter
contract and therefore belongs in group A's complete design, not a new
mirror patch or a new memory manager. Group C must establish shutdown order
before accepting its terminal behavior. Full execution audit remains open.

### Terminal-path split and repair ordering

Source follow-through now reaches the actual product caller, not only
cleanup helpers: obj.vdm/ntvdm.c main installs the worker termination escape
and returns mvdm_standalone_worker_finish's result. Termination calls
host_applClose, then session_terminate_current marks SESSION_STATE_COMPLETED
and longjmps to that finish label. The ordinary memory acquire/release
entrypoints require SESSION_STATE_ACTIVE. host_applClose stops event and
heartbeat producers and closes host devices; it does not enumerate and join
all WOW task threads or perform their message/task retirement.

This narrows the previous teardown finding: finish is a terminal process
path in the inspected product, not a proven reusable-worker loop. Do not
justify a reusable-session framework from its flags or claim that retained
process-exit allocations necessarily leak across later launches. Conversely,
the terminal path does not prove outstanding threads are already quiescent.

| Exit class | Required ordering and current source evidence | Repair boundary |
| --- | --- | --- |
| Normal Win16 task exit | WK32KillTask calls W32DestroyTask, RemoveTaskSharedList, host_ExitThread. host_ExitThread frees CCPU thread simulation state before adapted ExitThread unbinds USER state | Window/message/task cleanup requiring callbacks must precede CCPU destruction. Do not insert it into the existing later unbind callback |
| Failed task initialization | W32Thread failure also reaches W32DestroyTask/host_ExitThread, but original initialization may be partial or committed before notification failed | Use initialization phase ownership, not unconditional normal-task cleanup |
| Ordinary support-thread return | Four project start thunks unbind after the start routine returns; some have no WOW task at all | Preserve taskless unbind. Only an enrolled task needs the task retirement contract; no blanket guest cleanup on every worker thread |
| Whole-worker termination | Completed state and longjmp precede finish; current finish ends guest memory before USER detach, ignores detach/unbind failures, then may report ERROR_BUSY from session disposal | Final host-only disposal must not attempt new guest execution. Resolve active task shutdown before the terminal transition, or explicitly use terminal process reclamation without claiming successful reusable disposal |
| Bootstrap failure | begin goes to cleanup with only some flags/resources installed | Keep reverse initialization cleanup independent from live-task termination |

Group C design requirements are consequently concrete:

1. Establish the normal task pre-exit owner while guest memory and CCPU
   thread state remain usable. Recover the original window-before-message-
   before-task ordering through the admitted bounded owners; a move of
   runtime_unbind after/before one free is not that complete operation.
2. Preserve the existing fallible detach/unbind contract: only clear
   success flags and binding counts after actual completion. Terminal
   abandonment, if required, must be explicitly distinct from reusable
   successful cleanup. Do not remove the live-window guard.
3. Keep final worker finish non-reentrant into guest. Merely moving
   guest_memory_end below USER detach cannot make the already-completed
   session accept memory access, nor prove all other task threads stopped.
4. Join message cancellation from group B before freeing a task carrier;
   selected DestroyTask alone is not that cancellation operation.
5. Use the source-defined terminal escape to bound lease teardown analysis:
   the abandoned stack is not expected to resume. Reentrant backend calls
   that DO return still need group A's reservation/unwind proof. Do not
   invent resumed execution after process termination as a product feature.

Single-person review rejects a blanket cleanup reordering and a generic
thread-unbind destructor as complete repairs. The exact pre-exit seam and
native window/message retirement remain unresolved dependencies shared with
W02; the audit has not authorized importing the original USER server. This
section settles the lifetime classes and ordering requirements, not their
implementation or application acceptance.

### Original object-destruction slice and current enrollment boundary

Read canonical queue.c xxxDestroyThreadInfo and handtabl.c
DestroyThreadsObjects (1597 onward)/HMDestroyUnlockedObject (1437 onward),
selected cleanup.c, and current wow_cleanup_bindings,
wow_user_object_bindings and wow_window_words_binding. The original
thread-destruction closure is not contained in selected _WOWCleanup:

- DestroyThreadsObjects first handles DC-cache/thread locks, then scans the
  current handle table, excludes process-owned/already-destroying objects,
  and dispatches destruction for objects owned by the exiting thread.
- HMDestroyUnlockedObject marks destruction and dispatches by type. Its
  window branch uses original ownership transfer and xxxDestroyWindow;
  other branches cover hooks, menus, DDE objects and additional native types.
- _WOWCleanup's HMDestroyUnlockedObject call is instead in its process-owned
  task-object loop. Its class-procedure neutralization is already connected
  and remains distinct from destroying the thread's windows.

Current cleanup_destroy_object is intentionally an error-only boundary for
unenrolled process-owned types; it is NOT a working generic destructor that
can be reused for the missing window loop. Current enrollment publishes
TYPE_WINDOW and native HWND identities. Normal WM_NCDESTROY reaches
detach_matching -> wow_user_window_retire -> guest handle retirement,
then removes the property, drops the class association and releases the
binding. That reverse path is existing reusable mechanics and must not be
replaced by manually erasing table entries.

The required source-recovery disposition is consequently:

| Candidate | Disposition and specific dependency |
| --- | --- |
| Whole xxxDestroyThreadInfo/DestroyThreadsObjects import | Not directly composable at the current boundary: kernel thread locks, DC cache, complete native handle table/type destruction and private SMS cleanup are not supplied by the current carrier |
| Original owner-filter/destruction-order slice | Candidate for bounded recovery only with an explicit W02 object-owner contract; current enrollment covers windows, not the whole original object universe |
| Existing _WOWCleanup/class neutralization | Retain; it is already connected original behavior, not a replacement for native window destruction |
| Existing WM_NCDESTROY retirement | Reuse after genuine native destruction; preserve identity, class reference and guest backing release ordering |
| Synthetic successful destructor or direct table clearing | Reject: leaves native windows/references live and defeats the current task-retirement guard |

Group C's missing prerequisite is now named at the concrete function level:
same-thread native destruction must complete through the enrolled reverse
path before provider task retirement. Handle reuse, child destruction and
callback reentry prevent retaining unprotected table pointers across that
operation. Full type/object support remains W02-owned; do not silently
claim it from a window-only loop. This audit finding identifies the minimal
cross-owner contract needed for the integrated repair, not approval for a
new generic destructor or broad USER-server import.

### Registered execution entrypoints and hung-handler consumers

Read the actual UserRegisterWowHandlers output assignments and their
registered_* bodies, selected wkman.c/wkgthunk.c consumers, lifecycle
yield/wait/register operations and original exitwin.c registration. This
closes the enumeration of execution-facing output slots, not their runtime
acceptance. The other output slots (creation/classes, DDE, resources,
handle queries and drawing) retain their explicit object/provider owners;
their callbacks remain in the cross-family execution audit.

| Published execution slot | Guest-side consumer and selected owner | Disposition |
| --- | --- | --- |
| pfnInitTask | W32Thread -> registered_init_task -> lifecycle init -> xxxInitTask | Wired; committed-init versus broker notification failure and rollback remain group C gaps |
| pfnDirectedYield | wkman/wkgthunk task-yield callers -> lifecycle -> xxxDirectedYield | Original selection body wired; retain, with task-lock/peer-loss acceptance rather than replace with a local scheduler |
| pfnYieldTask | WK32Yield and other original callers -> lifecycle -> xxxUserYield | Wired; native receive reentry and idle-hook state remain dependencies, not absent registration |
| pfnWowWaitForMsgAndEvent | WK32WowWaitForMsgAndEvent -> lifecycle -> xxxSleepTask | Wired; original FALSE means message/task work and TRUE means interrupt event, not generic failure/success |
| pfnRegisterUserHungAppHandlers | WK32InitializeHungAppSupport -> lifecycle register -> xxxRegisterUserHungAppHandlers | Original allocation/event reference/storage wired; storing the exit callback does not connect every original notification producer |
| pfnWOWCleanup | Original module/task cleanup -> lifecycle exit -> _WOWCleanup | Wired class/resource operation; not terminal window/message retirement |

In particular, WK32WowWaitForMsgAndEvent brackets the wait with BlockWOWIdle
and returns its BOOL to WOWEXEC. The adapter's FALSE must not be globally
converted to a failed operation. Error propagation needs a separate
entry-failure distinction while preserving the original result meaning.

The hung-handler registration's lpfnWowExitTask has no consuming reference
in the current provider beyond its storage/declaration. Canonical
kernel/queue.c exports it through UserThreadWOWInformation; canonical
server/exitwin.c queries that information and uses
InternalCreateCallbackThread with the exact task ID, falling back to process
termination on failure. Those private USER/CSR facilities are not selected
product bindings. Thus registration alone is not end-to-end hung-task support.

However, hung notification is NOT wholly absent: selected nt_bop.c resolves
the exported W32HungAppNotifyThread, and nt_error.c calls it with zero on
the WOW error-panel abort path. Preserve this distinct existing path; do
not add another notification thread merely because lpfnWowExitTask has no
consumer. Product task-end requests require their own source-backed route
to the original callback and remain part of the cancellation audit.

Second-role review preserves the connected original yield/wait/register
bodies and rejects both blanket rewrites and the claim that all six are
complete merely because function pointers are non-null. The remaining
work is in state/consumer closure, not registration-table population.

### Product management close is not a hung-task callback

Follow-through of ntmon-exe/main.c close_selected_node, common RPC
management.c, ntsrv-exe/main.c and opennt/source/lifecycle.c establishes the
existing management route. A worker selection is generation/instance-checked
and resolves a retained process handle. Non-native workers are terminated
as a process; native workers have their separate stop/closed acknowledgment.
The monitor confirmation explicitly says worker for this category. There is
no call from this route to W32HungAppNotifyThread or lpfnWowExitTask.

This is not evidence that management close is broken: killing the selected
worker is its current contract. It must not be counted as per-Win16-task
graceful cancellation or as a test of original hung-task dispatch. Do not
reinterpret its worker identity as a guest task ID merely to exercise the
missing USER notification path. The owner's subsequent explicit approval
admits targeted task deletion in S3 alongside worker termination; see the
working plan. It does not make the currently absent route functional.
Normal guest WK32KillTask, host error-panel abort and forced worker death
remain three different acceptance paths with different cleanup guarantees.

### Remaining audit decisions, reconciled with inherited checklist

The T422 S3 C01--C03/C09/execution-C10 obligations remain intact. Their
current unresolved decisions are recorded explicitly to prevent repeatedly
rediscovering the same boundary or calling more documentation a repair:

| Scope | Decision required before integrated implementation | What is already established |
| --- | --- | --- |
| C01/C02 | A real receive/reply association and cancellation boundary expressible without private SMS guessing or another transport | Exact original transitions, current information loss, rejected public-query shortcuts, modal/wait categories |
| C03 | Original resolver/storage composition and the added lease failure/reentry contract; complete per-family context classification | Original success-only CallBack16, demand loading, existing alias route, shared lease consumers and untested reentry cases |
| C09 | Initialization commit/notification rollback, input-attachment completion and idle/hung consumer ownership | All six registered execution output entries traced; original yield/wait bodies already connected |
| Execution C10 | Normal task pre-exit and same-thread native object destruction before message/task retirement; terminal worker disposition | Original destruction slice, existing WM_NCDESTROY reverse path, task versus worker termination distinction |
| C11/C12 | Review exact retained/deleted code against the completed design, then run production acceptance and delivery | No source-only audit or native fixture substitutes for guest/runtime gates |

This table is a finite design-decision inventory, not a complete audit or
permission to skip remaining caller tracing. The production candidate stays
frozen. The next implementation proposal must resolve these decisions or
identify a precise owner decision needed; it must not restart a series of
isolated fixes under the same task name.

### Initialization failure cut points

Read the complete selected xxxInitTask and lifecycle_init_style against the
registered_init_task caller. Normal original failure returns are limited to
name allocation/copy and subsequent TDB allocation. After assigning
pti->ptdb, the ordinary source path attaches queues, inserts the task,
sets compatibility state, selects the new task, enters WOW execution and
returns STATUS_SUCCESS. Exceptions in called adapters are distinct from
these explicit NTSTATUS returns and still require unwind analysis.

| Cut point | State actually reached | Required handling |
| --- | --- | --- |
| Before original call | Project task/event/message view and list/context publication may exist | Undo this invocation's adapter resources and restore truthful context; do not run completed-task destruction |
| Name allocation/copy failure | No TDB; original frees the name on copy exception | Do not double-free original cleanup; retire remaining project publication |
| TDB allocation failure | App name exists; original already changed process startup fields/list membership | Release the task-owned name through its original lifetime owner; do not invent rollback of shared startup policy that original failure did not perform |
| Original STATUS_SUCCESS | TDB is scheduled and WOW execution ownership has been entered | Keep the task as committed; failure handling now requires actual task retirement, not freeing the carrier as if never initialized |
| BaseSrv startup notification failure | Happens after that successful original commit | Current FALSE triggers W32Thread retry while binding->thread prevents reinit. Choose notification retry or committed-task failure through the original lifetime contract, not a fabricated successful reinitialization |

Original ReattachThreads' BOOL is not tested by xxxInitTask. Consequently
the adapter cannot rely on its current reattach(FALSE) return to prevent
init success when it only calculated pqAttach without completing attachment.
This is another reason to complete that dependency before accepting task
initialization, rather than add a new original failure branch opportunistically.

Second-role review narrows the repair: preserve the source's shared startup
effects and successful scheduling algorithm. Repair project-owned resource
closure and post-commit notification semantics; do not replace xxxInitTask
with an invented transaction or blindly call DestroyTask for a NULL/partial
TDB. The concrete notification disposition and queue attachment owner must
be settled in the integrated design before implementation.

### Approved targeted termination: identity and completion boundaries

Source-only follow-through, 2026-10-14: inspected canonical OpenNT
base/mvdm/vdmdbg/vdmdbg.c VDMTerminateTaskWOW and selected wkman.c
W32Thread/W32HungAppNotifyThread/W32DestroyTask/WK32KillTask, plus current
NTSRV management.c service_append_worker and registration_bridge.c
registered_init_task. No runtime or production repair was performed.

The original debug API starts the original notification routine with a
guest task handle; its TRUE means a remote thread was created, not that
the task exited. The product has no corresponding task-close binding.
The existing monitor row is keyed by service epoch, worker generation and
label identity, but its task field is WOWRECORD.iTask. W32Thread separately
sets td.htask16 from pFrame->wTDB and td.VDMInfoiTaskID from iW32ExecTaskId.
registered_init_task receives both task_id and shared_id, yet startup
notification currently sends shared_id alone. This is the exact place to
audit identity association, not permission to conflate these namespaces.

W32DestroyTask signals the shared ghevWaitHungAppNotifyThread before
W32FreeTask, ExitVDM, WOWCleanup, WK32DeleteTask and thread-handle closure.
Therefore that event cannot prove target-specific cleanup completion and
concurrent requests cannot simply share it as their completion receipt.
Original termination also permits escalation after warning when another
task owns execution, then whole-process termination after a second timeout.
Preserve original execution mechanics while explicitly bounding the new
product operation's authorization. Do not silently drop warnings or infer
that the original function guarantees isolation under every hang.

Required integrated decisions: live task registration including guest-started
tasks; stale-handle protection; serialized original hung-operation ownership;
worker-local bound notification-thread entry; cleanup-complete observation;
explicit escalation results. Reuse the original task list and lifecycle,
not a second execution registry. This extends C09/C10 and S3 acceptance;
it does not reopen the rejected private USER/CSR transport or alter DOS.

### Startup notification decision: reuse the original bounded retry

Follow-through reads registered_init_task, lifecycle_init_style,
W32Thread's complete InitTask loop, base_rpc_client_launch_style.c
OpenNtBaseClientWowStarted, and base_service.c OpenNtBaseServiceWowStarted.
The native RPC wrapper returns actual transport/server failure. The server
validates worker generation and dispatched WOWRECORD, matches the parent's
receipt, sets its start event and marks wow_started. Repeating notification
for the same still-live record is harmless; there is no new task allocation
in this operation. Missing parent after a valid record is already success.

Design decision for the integrated C09/C10 repair: preserve W32Thread's
existing bounded InitTask retry loop and xxxInitTask unchanged. At the
project registration/lifecycle boundary, distinguish an uninitialized task
from this same task's committed initialization awaiting notification. On a
retry of the latter, validate current bound task identity and the original
shared request identity, and retry only OpenNtBaseClientWowStarted. Do not
repeat style lookup, queue attachment, allocation, insertion or scheduling.
Reject a different task/request rather than treating every existing binding
as success. Keep any necessary commit/notification state with the existing
task lifetime, not a global last-ID cache or a new retry service.

The original caller remains responsible for retry count/delay and eventual
W32DestroyTask/host_ExitThread. A persistent notification failure must not be
converted to success. That terminal branch requires C10's full committed-task
retirement first; it cannot free the carrier as a pre-init failure or leave
the scheduler holding a dead task. Ordinary pre-commit failures continue
through the original retry with invocation-local rollback. Guest-started
and separate tasks without a shared launch notification remain distinct.

Deletion/replacement set: replace the current registration comment and
control flow which returns FALSE after commit but then unconditionally
re-enters lifecycle_init_style on retry; retain lifecycle's general rejection
of unrelated duplicate initialization, original bounded caller and original
BaseSrv startup receipt validation. Do not add a mirror retry or reset
shared startup policy. This is a design decision, not implemented behavior.

Acceptance cases: initial success; failure before original commit; first
notification failure followed by success; response lost after server event
set; persistent notification failure with complete retirement; mismatched
task/request retry; parent disappearance; no-notification guest/separate
task. Assert one original task initialization and unchanged scheduler/event
accounting across post-commit retries, not merely final BOOL success.

### Message-boundary sufficiency check

Re-reading DirectedScheduleTask and xxxSleepTask confirms that preserving
these original bodies requires more than a sender runnable flag. The exact
send identity controls priority (psms == ptiNew->psmsSent), cross-scheduler
send/receive counters and early-reply deadlock prevention via psmsCurrent.
The current outer-call record cannot establish any native receive identity.

A discriminator required of any proposed binding is two live sends with
identical HWND/message/parameters, including nested reentry and an early
reply. Matching tuples or selecting whichever task is currently waiting is
not unique; a callback may instead be caused by an implicit native operation.
Native reply success alone supplies no identity with which to update the
original counters. Therefore importing more taskman bodies or moving the
existing FALSE DirectedScheduleTask call to ReplyMessage cannot close C01/C02.
The unresolved decision remains a proven native receive association boundary,
not an invitation to add a heuristic, second queue or replacement scheduler.

### Input attachment repair must include queue storage ownership

Read canonical input.c AttachToQueue/ReattachThreads (5493--5880),
queue.c DestroyQueue's decrement-and-survivor handling (1511 onward),
selected input.c AddAttachment/Recalc2/RecalcThreadAttachment, and current
lifecycle allocation/reattach/retire_task. This identifies a storage contract
which prevents the tempting partial repair of merely assigning pti->pq.

Current lifecycle embeds one wow_task_order_queue inside each task and sets
both pcti and pq to that object. Recalc2 can nominate that embedded queue
as another task's pqAttach and increment its cThreads. Current reattach
neither consumes/clears those pending references nor performs the native
input attachment. Current destroy_queue does not decrement cThreads; it
only HeapFree's a zero-count argument. retire_task frees the containing task
without releasing its pq/pqAttach references. Thus an embedded per-thread
object is being offered to an original algorithm expecting independently
reference-owned shared queues. Assigning pqAttach to pq or adding a decrement
alone would expose dangling references or an invalid free of embedded storage.
This is a source-derived lifetime defect, not a claim of an observed crash.

Original AttachToQueue additionally moves active/focus/capture/caret/mouse
tracking state and redistributes native input. Original ReattachThreads clears
pqAttach before each commit, handles journal/foreground queues, and states
that no application callback may occur during this operation. Its private
Q/PWND/RIT fields are absent from the finite binding. Copying that entire
function is not currently composable; copying just its pointer assignment
would omit its purpose. Native USER must remain the input-state owner.

Integrated group requirements: separate per-thread wake observations (pcti)
from shared input-group lifetime (pq); use independently owned group storage
when retaining the original grouping algorithm; preserve original reference
increments/decrements, clear pending attachment references, and retire both
committed and pending memberships before freeing the task. Establish the
native attachment boundary for actual focus/capture/input sharing and audit
its failure, callback and thread-exit semantics before declaring equivalence.
Do not confuse an in-memory grouping view with an attached host input queue.

Deletion set: replace the recalc-only reattach success condition and the
zero-count-only destroy_queue substitute, together with embedded shared-group
storage assumptions. Retain original grouping bodies where their contract can
be supplied. Native attachment and object state are a bounded W02 dependency
of C09/C10; this is not authority for an independent synthetic input queue.

Acceptance must include two and three tasks, repeated regrouping, departure
of the task that originally supplied the group, pending-attachment failure,
the last member, and focus/capture behavior. Observe that surviving tasks'
per-thread wake state remains independent and all group storage is released
exactly once. These dependency-complete checks precede single-task Delete
acceptance; a successful initialization alone cannot establish this closure.

### Native input boundary: reuse retained evidence, do not rediscover it

Reconciled the above storage audit with T420 S40's existing native input-group
and focus evidence in [the USER profile](m0-t420-s40-wow-user-profile.md),
sections Native input-group mechanism and peer-thread exit, Input-group
lifetime: middle exit followed by a new member, Native input attachment
record ownership, and Conflicting native input state and GUI-thread lifetime.
Read the current wow_native_input_group_fixture.c,
wow_native_input_focus_fixture.c and wow_input_membership_fixture.c sources.
No fixture was rerun and historical artifacts were not re-certified.

The existing negative is decisive for design: A-B/B-C links can leave A/C
apparently shared after B exits, but adding D via A-D then splits C away.
Explicit surviving A-C membership before joining D retains sharing. Thus
neither a fixed root nor a chain whose middle may retire is a complete binding
of original same-process TIF_16BIT grouping. Original Recalc2 derives that
group regardless of surviving explicit links. Preserve that source owner;
the adapter must reconcile native owned attachments for the live desired
membership, not infer it from a one-time successful attach or keyboard read.

The retained duplicate-pair case observes two attachments require two
detachments; a third removal fails. Therefore track exactly the native
attachments owned by this binding, not all attachments the application or
another subsystem might create. Reapplying the same desired group must not
silently accumulate native references. Group reconciliation and task exit
must be one serialized owner operation, with successful edge additions and
failure unwind accounted for. Connecting a new task to each existing member
of its implicit WOW group is a candidate without a fragile anchor; explicit
cross-group links and regrouping still require the original component result.
This candidate is not asserted as implemented or fully reviewed.

Microsoft's current public contract also confirms that AttachThreadInput
shares input state, requires both message queues on the same desktop, can
fail, and resets keyboard state. See
[AttachThreadInput](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-attachthreadinput),
read 2026-10-14. Do not patch reset behavior by restoring a saved keyboard
table: original queue creation also initializes key state, and historical
evidence does not establish that such restoration is correct.

The historical focus fixture covers private-desktop focus/caret/capture and
counts no callbacks in those tested attach/detach cases; it does not prove
universal callback absence, foreground precedence or pending-input equivalence.
Its intermediate desktop-cleanup failure remains a non-pass observation.
Reuse its exact cases for acceptance after the binding is implemented, adding
actual lifecycle membership/failure/retirement tests. These native tests do
not close the original private AttachToQueue contract or real Win16 gates.

Second-role disposition: public USER is the viable native input-state owner;
do not import/rebuild Q/PWND/RIT storage or duplicate input delivery. Original
membership computation, independently owned group views, owned native links,
and task retirement form one repair group. Pending final design items are
partial-reconciliation failure and source-backed callback/foreground bounds,
not another round of exploratory keyboard-state tests.

### Callback scope starts before CallBack16 for several families

Read wowhooks.c W32StdHookProc/ThunkCallWndProcHook, wcommdlg.c
WCD32CommonDialogProc, wkman.c WK32ForegroundIdleHook, wgdi.c
W32AbortProc/EnumObjects/LineDDA and the existing window dispatch scope.
These paths rule out a universal execution-acquire patch placed only inside
CallBack16, independently of the unresolved native SMS association.

| Entry family | Work before/around guest CPU entry | Scope consequence |
| --- | --- | --- |
| WND dispatch | Existing wow_window_dispatch_bound enters task scope before borrow and DispatchClientMessage; finally restores callback page, borrow and task scope | Retain this order while repairing actual message ownership; ignored leave failure still needs terminal disposition |
| CallWndProc hook | ThunkCallWndProcHook prepares/thunks message, stackalloc16, writes and flushes guest CWPSTRUCT before CallBack16, then unthunks/frees afterward | Any required execution acquisition must precede original hook marshalling, and survive through copyback/free; wrapping only guest CPU entry is too late |
| Common-dialog hook | WCD32CommonDialogProc uses CURRENTPTD and guest OPENFILENAME storage before CallBack16; message-dependent data is copied back after it | Modal ingress and full original thunk lifetime are one boundary, distinct from a general window procedure callback |
| Printer abort | W32AbortProc reads current task's guest TDB abort procedure via SEGPTR before CallBack16 | Guard/validate at the native callback entry when needed; do not attach a foreign thread by guessing a current task |
| Foreground idle | WK32ForegroundIdleHook directly calls CallBack16, separately from taskman's idle hook slot | Reconcile native hook delivery with original scheduling; do not double-deliver idle by filling both slots blindly |
| Synchronous GDI enumeration/LineDDA | Original guest thunk supplies native callback and per-call parameters | Preserve inherited execution on the synchronous path; no new task-selection step merely because the function is a callback |

This classifies concrete source paths, not all runtime invocations of these
families. A complete entry binding must know whether native USER has resumed
a suspended thread or execution is already inherited, and must preserve
original guest-memory lifetime before and after CallBack16. It must not
acquire/release execution independently around every nested memory helper.
The two existing WND task scopes cannot be advertised as universal coverage.

Repair placement decision: keep original thunk bodies and callback algorithm;
select the smallest callback-registration/entry seam that precedes their first
guest access. Original mirror hooks are considered only where the registration
binding cannot supply that seam, with exact provenance and minimal diff.
The frame resolver/lease fix remains shared but cannot substitute for these
execution-entry obligations. Acceptance observes preparation and copyback as
well as guest entry/return, including exception and nested callback paths.

### Message architecture decision boundary, not another missing wrapper

Reconciled current message binding with T422 E95, which explicitly records
the owner's rejection of a local SMS delivery owner. That rejected route is
not a pending approval and must not be proposed as an already-admitted fix.
Rechecked the published InSendMessageEx, CallWndProc and CWPSTRUCT contracts
on 2026-10-14: they expose send/reply category and message parameters, not the
exact received SMS or foreign sender identity required by the inspected
original DirectedScheduleTask/xxxSleepTask coupling. The prior private
server-to-client callback audit likewise does not carry that identity.
This establishes insufficiency of the inspected interfaces, not a universal
claim that no undocumented or future interface can exist.

The current design cannot truthfully promise unchanged original SMS-directed
scheduling through those interfaces. Another tuple lookup, callback mutex,
fabricated SMS or whole USER import would violate established constraints.
Do not use further peripheral audit edits as evidence this core is resolved.

Owner decision requested before changing this contract: permit a design-only
evaluation of retaining original task selection while adapting execution
handoff to actual public native callback/wait/reply events, without a local
message transport or fabricated receive identity. Such a design may differ
from exact SMS-directed priority/order and is NOT yet proven equivalent or
authorized for implementation. It must enumerate that difference, nested
and foreign-peer counterexamples, cancellation and single-owner invariants;
approval to investigate is not proof of correctness. If that relaxation is
not acceptable, exact message association remains a blocking architectural
dependency under the currently demonstrated boundaries. Other audit gaps
and all guest acceptance remain open; no T/S closure is claimed.

### Target-only termination: escalation and shared LockTDB ownership

Question: can the approved target-only operation reuse W32HungAppNotifyThread
unchanged behind existing bindings? Source-only inspection on 2026-10-14 used
wkman.c W32HungAppNotifyThread/W32EndTask, vdmdbg.c VDMTerminateTaskWOW,
wow16/drivers/keyboard/keyboard.asm keybd_int, kernel31/tasking.asm
SwitchTask/ExitSchedule, kernel31/schedule.asm LockCurrentTask, and the selected
WOW Ninja generator and wow_title_policy binding. No runtime test or production
edit was performed for this conclusion.

The original first phase sets LockTDB and requests a task switch. SwitchTask
checks the newly current task against LockTDB before its original DOS exit;
ExitSchedule clears the matching LockTDB. The later interrupt phase is
different: keybd_int tests and clears VDM_WOWHUNGAPP, then executes int 21h
4CFF without checking the requested HTASK. The subsequent timeout branch can
call ExitVDM(ALL_TASKS) and ExitProcess. Blocking only ExitProcess is therefore
insufficient to guarantee target-only deletion.

The selected wkman build has the native-process binding, not the MessageBox
style binding used for wudlg. Even inserting a generic MessageBox substitute
would be unsuitable: the first escalation prompt is conditional on the
current task differing from LockTDB; matching tasks skip it. Both original
prompt branches stop only on IDCANCEL, so a failed dialog returning zero is
not a safe denial. Neither global MessageBox nor ExitThread interception is
an acceptable policy seam for this operation. W32EndTask is a current-task
callback path, not a drop-in remote-target alternative.

Cancellation also has a shared-state obligation. LockCurrentTask writes the
same LockTDB word. Unconditionally clearing or restoring that word when a
management request times out can overwrite an application's intervening lock
operation. Checking only equality cannot prove ownership after the value is
changed and restored. Serialization of management requests alone does not
serialize application use. The design must specify whether a committed
target-exit request remains pending or can be safely cancelled at a proven
execution boundary; timeout must not silently mean cancellation or completion.

Review disposition: unchanged wholesale invocation is rejected for the new
target-only contract. Preserve the historical callers' escalation behavior.
A scoped policy seam before irreversible escalation is a candidate mirror
adaptation, but it is not yet a complete design or an exact diff budget:
target lifetime and shared LockTDB ownership must be resolved together.
Do not replace this original algorithm with a copied autonomous implementation
merely to report zero mirror diff. Guest source/binaries remain unchanged.

## Connected callback path: initial production acceptance

The restored original callback resolver now builds and links the complete
WOW32 DLL in the retained x86 /MT CCPU40 provider. The candidate package is
a copy of the hash-checked T446 package with only WOW32.DLL replaced;
identities are in build/M0-T447/S2/callback-package/candidate-hashes.json.
Neither assets/release nor the configured deployment was modified.

The first observation accidentally omitted S1's environment sanitization.
All three guests showed an illegal-instruction dialog; the observer report
did not show candidate WOW32 loaded. That observation is failed, not a pass
and not evidence attributing the failure to the resolver. Its runner exit 0
means observation completed only. The same already-built candidate was then
tested with the retained S1 environment allowlist on an unswitched isolated
desktop (process 19528; build/M0-T447/S2/callback-controlled). Results:

- WINMINE: visible Minesweeper main window in all four samples.
- SOL: original observed frontier remains the Out of memory dialog.
- WRITE: original observed frontier remains its insufficient-memory dialog.

Thus startup parity is established for this limited observation; neither
SOL/WRITE initialization nor callback edge cases are fixed/proved by it.
The particular inherited environment variable responsible for the first run
is not identified. Do not introduce a runtime workaround from that result.
Stages were identity-checked and removed by the existing cleanup helper;
logs and the build-owned candidate package remain available.

The subsequent dialog-boundary implementation was not included in this
candidate. It adds the existing task callback scope before original W32InitDlg
and the supplied DLGPROC, makes terminal cleanup unconditional through finally,
and releases creator references on exception. Mirror diff for that group is
zero. Its focused native fixture requires an explicit guest-publication seam:
the retained old fixture creates no CCPU page domain, so production handle
publication correctly rejects it before callback entry. The fixture now
checks paired simulated publication/retirement, retains original W32InitDlg
and lifecycle bodies, and does not claim guest page-domain acceptance.

Hidden dialog fixture process 4208 completed with exit 0 and
`WOW_USER_TASK_DIALOG_LIFECYCLE errors=0 init=2 terminal=2`, followed by
`WOW_FIXTURE_OK case=dialog-lifecycle`. Evidence is under
build/M0-T447/S2/dialog-entry (launcher-r2.log, stderr.txt, build.log).
The checks cover normal creation/destruction, a callback that recursively
destroys its dialog, original current-task ownership on callback entry,
no exclusive lock held during the procedure, and paired resource withdrawal.
Exception propagation, denied-entry cleanup, surviving native windows during
creation unwind, and real guest dialog acceptance remain unproved. This is
partial C03/C10 progress, not S2 closure or publication.

## Native dispatch and original Hook entry connection

Original client.c::DispatchClientMessage consumes WW and pfnWowWndProcEx
only in its WNDPROC_WOW branch; native WNDPROC is called directly. The outer
project gateway previously entered WOW scheduling and published guest window
state before either branch. It now retains the existing window reference for
both branches but skips that guest-only setup for native dispatch, retaining
the original dispatcher as the branch owner. No original-source diff is added.
Process 25704's native-dispatch-gate fixture passed with
`WOW_NATIVE_DISPATCH_GATE errors=0 native=2 guest-entry=1`. It links the actual
gateway/dispatcher and deliberately denies guest admission: native success
and exception unwind still execute; tagged WOW dispatch is refused before
its callback. This is a boundary test, not scheduler acceptance.

Original wowhooks.c::WU32StdHookProc reads CURRENTPTD and dispatches message
conversion/guest stack allocation before CallBack16. Its one definition is
renamed under DIV-332; all 32 original stub calls retain WU32StdHookProc and
reach the new scoped project wrapper. The original function body is unchanged.
Formal DLL map evidence puts `_WU32StdHookProc@16` in wow_hook_callback.obj
and `_WowOriginalStdHookProc@16` in wowhooks.obj. The mirror delta is +2/-1
(one definition replacement and its comment); LF line endings were restored
after the edit tool initially converted this -text mirror to CRLF.

Process 23708 completed the full provider link and real-wrapper fixture with
`WOW_HOOK_CALLBACK_SCOPE errors=0 calls=4 denied/normal/nested/exception checked`.
The fixture substitutes task/borrow operations and the original body solely
to witness ordering/unwind, not original hook behavior. Evidence is in
build/M0-T447/S2/hook-callback; production selection is recorded in both
wow32-dll/sources and New-T404S5Wow32ProviderNinja.ps1.

Process 16108 then built the latest combined provider and observed all three
immutable applications in the S1 controlled environment. The resulting
callback-gates-package evidence again has a visible WINMINE main window and
the unchanged SOL/WRITE memory-error frontiers. Thus there is no demonstrated
application initialization improvement yet. Real guest hook invocation,
mixed-message identity, early reply and full runtime acceptance remain open.
No deployed or release binary has been replaced.
# WRITE class-menu creation gap (source follow-up)

Source tracing after the combined callback-gates candidate identifies a
concrete missing creation dependency, not a heap diagnosis. In that retained
candidate's `frontiers/restored-write-windows.txt`, `MSWRITE_MENU` exists hidden
while its expected child controls are absent. Original
`mvdm/wow16/write/initwin.c::MmwCreate` first requires `GetMenu(hWnd)`, then
`FInitMemory`, then creates its scrollbar/page-info children. This observation
is consistent with the first check failing, but does not independently prove
which instruction failed or that this is WRITE's only initialization defect.

The production gap itself is directly established:

- `wow_class_client_bindings.c::publish_native` explicitly clears native
  `lpszMenuName`, correctly preventing public USER from loading a packed WOW
  module, and promises loading through the recovered resource client.
- `wow_window_creation_binding.c::create_bound` passes the caller's `hMenu`
  unchanged. Neither this path nor `wow_user_create_window_ex` supplies the
  retained class menu when a top-level caller passes NULL.
- Original `windows/core/ntuser/kernel/createw.c` lines636--650 loads that
  class menu with `xxxClientLoadMenu(pcls->hModule, ...)`. This happens after
  the CBT hook (line394), before WM_NCCREATE (line841) and WM_CREATE (line963).
  Loading failure does not itself abort window creation. Child IDs and
  explicit caller menus do not take this branch.
- Original `inc/ntcb.h::ClientLoadMenu` calls the menu resource client.
  The already-selected `opennt-host/.../client/clmenu.c` retains LoadMenuA/W
  as `wow_user_load_menuA/W`, CommonLoadMenu and the original parser.
  `W32LockResource` retains the Win16 menu conversion. No new menu parser or
  guest modification is needed. The current provider map retains the parser
  but not the unreferenced LoadMenuA/W entry points.

The bounded repair must reconnect that existing resource client using the
class module/name, not the creating module by assumption. Preserve the CBT
ordering and make both GetMenu and the creation message's hMenu agree before
guest WM_CREATE. Retain class/name lifetime across resource callbacks; clean
up newly loaded menus on rejected creation without deleting explicit caller
menus. Verify null/name/ordinal resources, child exclusion, explicit menus,
CBT rejection and nested creation before the real WRITE acceptance run.
Do not prematurely load before the native CreateWindow call: that would move
guest resource callbacks ahead of the original CBT boundary. This group has
no demonstrated need for additional mirror diff. The gap is open, not repaired
or published by this audit entry.

Implementation follow-up: the enrolled creation now calls the existing
`wow_user_load_menuW` at its first WM_NCCREATE, after CBT, attaching the menu
with public USER ownership. It supplies the resulting current menu to both
WM_NCCREATE and WM_CREATE structures. The class reference is retained by the
already-enrolled window borrow. Only project creation/gateway files changed;
this group adds no mirror diff or guest changes.

The extracted-production-body fixture `wow_creation_menu_fixture.c` uses
native USER windows and menus, substituting only resource lookup. It caught
that USER provides distinct CREATESTRUCT instances for the two messages;
the first candidate incorrectly updated only WM_NCCREATE. After correction,
`build/M0-T447/S2/creation-menu/launcher-r3.log` records
`WOW_CREATION_MENU errors=0 loads=4`. Seven cases cover named/ordinal menus,
load failure, absent class menu, rejected WM_NCCREATE, explicit menus and
child IDs. Attached menu destruction and creation-message/GetMenu equality
are checked. This is not the complete CBT publication chain, guest resource
conversion, nested creation or exception acceptance.

The earlier full provider candidate in `class-menu-package` linked and ran
the three unchanged guests, but WRITE/SOL still showed their previous error
frontiers and WINMINE retained its main window. That run predates the second
CREATESTRUCT correction. It proves neither WRITE restoration nor the exact
remaining failure instruction. Release directories remain unchanged.

The corrected candidate `class-menu-messages-package` subsequently completed
the full DLL build and the same isolated three-guest observation (launcher
PID17872, terminal result0). WINMINE still reaches its main window; SOL and
WRITE retain the initialization-error frontiers, including WRITE's hidden
MSWRITE_MENU without expected children. Thus the corrected source connection
has not demonstrated improved application initialization. The next resource
boundary acceptance must establish whether the actual class menu was found,
converted and attached, rather than inferring that from the native fixture.
`class-menu-review/review-docs.exit` is0 and documentation governance passed;
the working-tree diff check also passed. No publication or S2 closure.

Real menu witness and ordering correction: `class-menu-witness` PID25360
completed with result0. Its window trace proves WRITE's enrolled parent had
menuChecked=1, menuAttached=1 and a nonzero actualMenu (worker3872,
window006E0306), despite the retained initialization error. This establishes
real resource lookup/conversion/attachment, not successful application init.
The same trace also revealed the SetMenu candidate delivering window-position,
move and size messages inside WM_NCCREATE. Original createw.c sends its normal
size/move notifications after WM_CREATE (lines963--994); this difference is
not accepted merely because the menu handle was correct.

A bounded native experiment (`cbt-menu-boundary`, PID5048) disproved changing
CBT_CREATEWND.lpcs->hMenu as an alternative: the next WM_NCCREATE and GetMenu
both saw NULL, and the created menu was not owned/destroyed by the window.
The experiment reports errors=3, early=0; it is not product code.

Original `kernel/getset.c` case GWL_ID, lines422--451, already provides the
needed non-child menu association without frame recalculation; contrast
`kernel/mnapi.c::xxxSetMenu`, which adds xxxRedrawFrame. The project boundary
now calls SetWindowLongPtrA(GWLP_ID) and verifies the resulting GetMenu value,
reusing that original-shaped operation rather than suppressing messages.
The source-extracted production fixture now rejects size/move/window-position
changes before WM_CREATE. PID13060 completed with result0 and
`creation-menu/launcher-r4.log` records errors=0, loads=4. All seven previous
cases and the added ordering assertion pass on this host. No mirror diff is
added; no claim is made about untested hosts or complete guest behavior.

The corrected real WRITE observation (`class-menu-order-package`, PID26184)
completed with result0. The trace no longer has the SetMenu-induced
window-position/size/move recursion inside WM_NCCREATE; class menus remain
attached. WRITE still shows its initialization-error dialog and hidden main
window. This is a verified ordering correction, not initialization recovery.

Next source frontier: `MmwCreate` requires GetMenu, then `FInitMemory`, before
creating scrollbar/page-info children. `initmmw.c::FInitMemory` covers local
page-buffer/reserve allocation, FCB allocation, `FSetScreenConstants`, document
and property setup, scratch-file creation and emergency save storage. In
`scrnchng.c::FSetScreenConstants`, explicit failures include CreateBitmap(1,1),
GetDC(parent), and allocating printer-name strings. An absent configured
printer is expressly successful; it must not be treated as a missing-driver
diagnosis. `FInitFiles` creates a scratch file, so an initialization error is
not restricted to memory/GDI. The possible modern GetVersion mismatch was
checked against `kernel31/context.asm::GetVersion`: the unchanged guest uses
its own winVer, not host GetVersionEx. There is no evidence to patch that
version branch. No remaining branch is yet identified as the actual failed
instruction, and guest changes remain prohibited.

## Proven client-menu projection dependency

The next acceptance boundary changed the investigation, rather than providing
another guessed heap patch. `class-menu-guest-return` (PID9524) uses the same
candidate and the existing bounded dispatch witness. Matching its thunk RVAs
against that candidate's WOW32 map shows WRITE's main-window creation, but no
WU32GetMenu call in its WM_CREATE. This absence is not itself a failed-call
diagnosis: original `wow16/user/usercli.asm` explicitly selects a local client
implementation of GetMenu, GetMenuItemCount/ID/State and GetSubMenu.

The pinned immutable USER.EXE `_GetMenu@4` reads WND+104; for a non-child it
rebases that MENU pointer through WND.head.pSelf and reads MENU.head.h. The
existing page-domain publisher zeroes each WND and writes handle/self/class,
rectangles and styles, but never writes offset104. Its refresh also omits
this field. Consequently the original local GetMenu necessarily sees no
menu for an enrolled top-level window even after native GetMenu succeeds.
This is a concrete missing producer for an unchanged guest consumer, not a
reason to redirect or patch the guest. It directly matches MmwCreate's first
failure check; subsequent initialization failures remain untested.

`verify-wow-user-profile.ps1` now additionally pins GetMenu's menu/control-ID
loads and rebase, MENU type/count lookup and GetSubMenu's handle load. The
selected immutable image hash and all profile checks passed under PID25800;
`build/M0-T447/S2/menu-profile/result.exit` is0, with profile.json retained.
Both the original generated assembly and actual selected binary agree on
WND.menu=104, MENU.count=40, MENU.items=56 and ITEM.stride=64. Existing checks
cover ITEM type/state/ID/submenu offsets0/4/8/12.

Repair closure promoted from MENU-01/DATA-07/08/09/10/13 into active S2:

- Preserve the original local readers and native USER menu policy. Publish
  real guest-owned original-layout MENU/ITEM views and HANDLEENTRY identity;
  never expose host pointers or make a fake header containing only an HMENU.
- Include unattached CreateMenu/CreatePopupMenu, resource parser roots and
  submenus, GetSystemMenu, window/class attachment, and control IDs (numeric,
  not menu pointers). Retain handle type and uniqueness validation.
- Include Append/Insert/Modify/Change, Check/Enable/Hilite, SetMenu,
  SetMenuItemBitmaps and popup callback transitions. A creation-only snapshot
  would leave original local reads stale after the first mutation.
- Distinguish RemoveMenu (detach without destroying submenu) from DeleteMenu
  and DestroyMenu, window destruction, task retirement and native handle
  reuse. Reuse the existing session/page-domain allocation and publication
  boundaries; no second menu-selection or command-routing engine.
- Review original MENU/ITEM/PROCOBJHEAD layouts for bounded import/assertions
  before implementation. Current selected mirror contains WND/CLS, not the
  complete MENU/ITEM bodies; expected added mirror material is declarations,
  not edits to guest algorithms. Exact line budget follows that selection.

The prior FInitMemory exploration is not evidence that execution passed
GetMenu. Scratch-file creation failure calls DiskError(IDPMTSDE), setting
vfDiskError and selecting IDPMTCantRunF, whereas the observed string exactly
matches IDPMTCantRunM in write.rc. No TEMP/configuration workaround is applied.

### Original menu declaration closure

Selected the unchanged `_PROCOBJHEAD`, `tagITEM` and `tagMENU` bodies from
`O:/repos.external/OpenNT/windows/core/ntuser/inc/user.h` (source SHA-256
`ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1`).
They join the existing OPENNT-HOST-063 opt-in client layout subset in the
same original-relative owner. The only added dependency is an opaque
PROCESSINFO pointer declaration inside the original body; no server code,
native object pointers in guest state, or menu policy is introduced.
The bodies, including conditional fields and comments, match upstream after
line-ending normalization. Mirror budget for this step: +60/-0 declaration
and explanatory lines, zero algorithm changes. Copyright remains Microsoft's.

The owned layout header now asserts PROCOBJHEAD=24, MENU=64, ITEM=64 and
the handle/self/count/items/type/state/ID/submenu offsets consumed by the
unchanged guest. The existing layout fixture reports MENU and ITEM sizes.
The WOW32 provider does not compile this fixture implicitly: its successful
link alone is not layout evidence. The separate worker fixture was built
and executed explicitly.

Background verification in `build/M0-T447/S2/menu-layout/` completed with
exit 0 (launcher-r3, PID 20892): exact-body comparisons for all three types,
incremental WOW32 link, existing x86 client-layout fixture, and pinned
immutable USER.EXE profile verification. The prior runner's PowerShell
stderr promotion was corrected by capturing fixture stderr inside cmd;
that runner failure was not a failing assertion. No production publication.

Runtime closure remains open: the current object table publishes only
TYPE_WINDOW companions; the menu parser's destroy leaf directly calls
native DestroyMenu. A MENU graph must therefore join both publication and
retirement, not merely the class-menu loader or WND+104 assignment. This
declaration step supplies checked original layout only, not menu recovery
or WRITE/SOL acceptance.

### Child-ID branch and menu lifetime closure

The same WND+104 union has a distinct, source-proven WS_CHILD branch in the
pinned `_GetMenu@4`: it returns the numeric value without MENU dereference.
The page-domain producer now samples GWL_ID for that branch on publication
and refresh, including callback refresh. Zero is valid; GetWindowLong failure
is checked before writing the guest snapshot. It never relocates or aliases
the numeric ID. This is project-owned code only, with no new mirror diff.

`tests/mvdm-host/wow_page_domain_fixture.c` now creates an actual native child
and checks initial 0x8123, updated 0xfedcba98 through callback publication,
zero through refresh, and unchanged zero top-level menu field. The existing
CCPU/SAS domain fixture runs these checks with real page-domain storage,
then its existing geometry, borrowing and retirement checks.
Background run `build/M0-T447/S2/child-id/launcher-r3.log` (PID 2860)
completed with exit 0 and `WOW_PAGE_DOMAIN_LIFECYCLE_OK`.

The fixture initially failed to link: its shared native-child closure had
not acquired the Hook installer/context and launch-style client dependencies.
The generator now includes those real libraries together with their core,
worker RPC stub, binding/transport and common-RPC closure; the memory-test
link rule includes rpcrt4. No fake implementation was added. The existing
fixture's `/force:multiple` retains its pre-existing worker-I/O seam symbols
when the full RPC library is linked; this bounded native/CCPU fixture is not
evidence of broker I/O or whole-runtime acceptance. No production publication.

Further source review confirms original `cleanup.c::_WOWCleanup` already
iterates process-owned objects and selects their original hTaskWow. Its
selected body is present, but lifecycle `cleanup_destroy_object` explicitly
has no process-owned producer yet. Menu registration must populate that
existing owner and provide its destruction leaf before enrollment; do not
add an independent task-menu sweep. Native DestroyMenu/DeleteMenu recursively
retire submenus, whereas RemoveMenu must keep detached submenus alive.
Original `rtl/wow.c::MNLookUpItem` recursively reads ITEM.spSubMenu and
prefers matching leaf command IDs before a popup-handle fallback;
`_GetMenuState` additionally reads submenu.cItems. Keep those guest algorithms
unchanged and project their complete graph, not a single menu header.

### Menu query graph storage implementation (not activated)

The existing worker page-domain now provides publish/update, retire and
window-attachment operations for MENU query views. These three local worker
exports are added to the existing generator export list; no RPC or guest ABI
changes. Native menu creation/selection/destruction and process/task ownership
are deliberately not implemented by this storage layer. Original guest
MNLookUpItem/GetMenuState/GetSubMenu remain unchanged.

The native USER owner must supply copied type/state/ID/submenu identities,
publish submenus first, and finish all affected nodes and attachments before
CCPU entry. The carrier writes the original 64-byte ITEM stride and MENU
head/self/task/capacity/count/items fields. Other server-only drawing/locking
fields remain unexposed; this is the complete graph required by the audited
local query readers, not a substitute native USER MENU. The existing original
layout fixture additionally asserts task offset 20 and capacity offset 36.
This boundary is necessary because original server allocation produces host
addresses, whereas immutable USER16 must traverse CCPU guest-linear storage.
No usable original allocator can replace the existing VDM memory carrier.

Before replacing an item array, validate the full menu identity, immutable
creator task, bounds and every referenced submenu identity, then allocate and
populate replacement storage. Rejection leaves the prior graph untouched.
Window and menu slots reject type collisions. Child control IDs cannot be
overwritten through menu attachment. Stable MENU backing survives item-array
replacement; stale identities cannot retire a replacement at the same index.
Storage addresses used for freeing are worker-private, not read back from
guest-visible pointer fields. This also permits teardown after leaving the
protected page domain. Retirement does not invent recursive menu policy:
the native USER owner must detach/update references and invoke the required
retirements as one non-guest-executing boundary.

Dual-role review caught and removed a candidate teardown read through SAS
after protected-mode exit; teardown now uses retained worker-private item
allocation addresses. No released/runtime package contained that candidate.
`menu-carrier/launcher.log` under `build/M0-T447/S2` (PID 11092) completed
with exit 0 and `WOW_PAGE_DOMAIN_LIFECYCLE_OK`, including nested graph fields,
mutation to empty, detached-submenu retention, cross-type collision, stale
identity/task mismatch, count overflow, self-reference rejection, unchanged
old graph on rejection, window attachment and slot reuse. This fixture's
existing worker-I/O link seams remain as disclosed above. The original-layout
and immutable-profile run `menu-layout/launcher-r4.log` (PID 25708) also
completed exit 0; source whitespace checks passed.

Activation remains pending the native menu producer/mutator wrappers,
callback refresh and original _WOWCleanup destruction leaf. This storage
implementation alone cannot repair WRITE, and no such claim or publication
is made. Allocation-failure injection, whole-graph rollback/reentrancy and
real guest menu interaction remain acceptance gates for the complete group.

### Native menu companion and original cleanup composition

`wow_menu_client_bindings.[ch]` now supplies the existing USER object table's
TYPE_MENU companion, with process owner and creator hTaskWow retained. It
samples native GetMenuItemCount/GetMenuItemInfo into the audited query graph;
native USER remains the creator, mutator and selector. Existing nodes retain
their creator task when another task refreshes them. Full identities and
known short aliases are distinguished; recursive traversal rejects a node
already being visited. Native object operations are not copied into the
mirror, and no second task-resource registry is added.

The old lifecycle `cleanup_destroy_object` unsupported placeholder is removed.
The original cleanup callback slot now uses `wow_menu_client_cleanup`.
Original `_WOWCleanup` and handtabl.c select process-owned menus by hTaskWow;
the supplied leaf invokes native DestroyMenu and withdraws recursively
invalidated companions. RemoveMenu-detached, still-valid submenus stay alive.
The adapter records a release failure and returns FALSE after the original
scan rather than silently accepting its void destruction leaf. This changes
no original algorithm or guest code.

Review caught an important phase distinction: `_WOWCleanup` returns before
process-resource destruction for the non-NULL hInstance task/ModuleUnload
branch. Resource destruction occurs in final thread cleanup with NULL
hInstance. A candidate pre-retirement "live-menu means busy" check would
have prevented that original cleanup and was removed before publication.
The existing retire_task(NULL, task_id) edge remains the owner.

The real CCPU page-domain fixture now also links the selected original
cleanup.c, handtabl.c and ProbeForRead, plus the actual cleanup adapter and
menu companion. It proves early cleanup preserves both tasks' native menus,
final cleanup destroys only the selected task's menu and projection, and
the other task's menu remains valid until its own final cleanup. It also
checks native tree enrollment, checked-state mutation, short-alias resolution,
creator-task preservation, detached submenu retention and recursive native
destruction. `build/M0-T447/S2/menu-client/launcher-r4.log` (PID 17004)
completed exit 0 with `WOW_PAGE_DOMAIN_LIFECYCLE_OK`. The fixture compiles
original cleanup with its existing provider /Gz calling convention; no
source change was made to satisfy that ABI. The previous fixture assumption
that last_handle equalled the only window index was corrected to the exact
maximum of the already used menu indices and the window index.

Full worker/provider rebuilding after review completed exit 0 in
`menu-provider/launcher-r2.log` (PID 26264). The worker's VdmTib ownership
audit passed. These are local candidates, not a publication or application
acceptance. Mirror budget for this continuation remains zero additional
algorithm/declaration changes beyond the previously recorded MENU types.

The broader menu-cleanup lifecycle case was added with an explicit storage
seam for failure/retry testing, but its link currently fails with 18 unresolved
startup dependencies (including RegisterWowBaseHandlers, fast-BOP offsets,
ExpLdt, common-dialog/version imports and CsrPortHeap). It is NOT a passed
test, and the direct original-cleanup/real-storage result does not replace
that task-retirement/failure acceptance gate. No extra startup stubs were
added to conceal the failure.

Next required activation work remains native producer/mutator entry binding,
class/window and callback refresh, and failure-safe return-to-guest behavior.
The companion is selected by the build and original cleanup callback, but
ordinary guest menu producers are not yet enrolled automatically. WRITE/SOL
recovery, complete message semantics, publication and delivery remain open.

### Native menu API entry compilation and CCPU acceptance

The previously drafted `wow_menu_native_bindings.c` is now selected in both
the component sources and the provider generator. Its redirect header is
still NOT included at original callers: this is compiled candidate code,
not activation of guest menu routing. The background worker/provider build
in `build/M0-T447/S2/menu-provider/launcher-r3.log` (PID 10464, started
2026-10-14 14:16:24 local) completed with result.exit 0. The worker and
provider remain unpublished candidates.

Source review confirms `mvdm/wow32/walias.h::HMENU32` sign-extends the guest
handle. `wumenu.c` applies it to popup operands as well as root menus.
The native-entry fixture therefore exercises the existing runtime binding,
real native menus and real CCPU menu storage together, including a signed
short popup operand. It asserts guest root/count/submenu fields after append,
checked state after mutation, unchanged state after a nonexistent-item
failure, detached-child retention and destruction/handle withdrawal.
The fixture uses the production runtime implementation, not a TLS substitute.

The first fixture run failed at creation because the test had not assigned
its process object table before calling the new entry; the diagnostic repeat
identified ERROR_INVALID_STATE at that prerequisite. The fixture ordering
was corrected without changing the product contract. The background run
`menu-native/launcher-r3.log` (PID 21084, started 2026-10-14 14:20:04 local)
completed result.exit 0 with `WOW_NATIVE_MENU_ENTRY_CCPU result=1` and
`WOW_PAGE_DOMAIN_LIFECYCLE_OK` in `menu-native/test.log`. The retained fixture
still has its declared broker-I/O seam duplicate-link warnings; this does
not verify broker I/O or a real Win16 application.

Review gates remain open before enabling redirects: inherited USER-lock
release around native callouts, callback/implicit destruction refresh, and
failure coherence after a native mutation succeeds but guest publication
fails. In particular, returning API failure alone does not undo a native
mutation and cannot prove the guest view is safe. No additional mirror diff,
guest change, publication, commit, push or S2 completion is claimed here.

### Inherited menu-callout lock repair

Original `ntuser/kernel/mnapi.c::xxxSetMenu` associates the menu and calls
`xxxRedrawFrame`, including for hidden non-minimized windows.
`mnaccel.c::xxxHiliteMenuItem` reaches `xxxMNRecomputeBarIfNeeded` and
`xxxMNInvertItem`; `mndraw.c` reaches owner drawing. These operations must
not retain the project's USER data lock across native client callbacks.
The candidate wrappers previously avoided acquiring a new lock but did not
release an inherited one. They now pair the existing runtime callout
leave/reenter operations in finally, preserving the caller's lock state and
native error without changing WOW task ownership. No new scheduler, original
menu algorithm, guest patch or additional mirror diff was introduced.

`verify_native_menu_entry` now creates a real native overlapped window and
publishes it through the existing window owner. With the runtime lock held,
SetMenu must produce at least one WM_NCCALCSIZE, none with that lock held;
return must restore the lock and publish the actual WND menu pointer into
CCPU storage. Detach is checked with the same inherited lock. Successful and
nonexistent-item CheckMenuItem calls also preserve that lock. Finally, window
and menu records are retired through their real owners. This tests native
callout lock state, not guest callback execution or exception unwinding.

Hidden background fixture runs PID 16804 (14:22:22 local) and PID 14784
(14:23:14 local), 2026-10-14, completed exit 0. Retained final log:
`build/M0-T447/S2/menu-native/test-r5.log`, markers
`WOW_NATIVE_MENU_ENTRY_CCPU result=1` and `WOW_PAGE_DOMAIN_LIFECYCLE_OK`.
The declared broker seam warnings remain unrelated to these assertions.
Redirect activation, callback-view synchronization and post-native-mutation
allocation-failure coherence remain mandatory open gates, not waived by this
lock repair.

Full-provider composition initially rejected the two callout-named runtime
helpers because those trivial enter/leave aliases are not worker exports
(`menu-provider/launcher-r4.log`, PID 26092, link exit 1120). The binding now
uses the already exported `wow_user_runtime_leave/enter`, exactly as the
existing message bridge does; no duplicate exports or ABI expansion was
added. Full worker/provider build PID 25644, started 14:24:31 local,
completed result.exit 0 (`menu-provider/launcher-r5.log`).
The matching final-source CCPU/window regression PID 22512 (14:25:20 local)
also completed exit 0, with both native-entry and page-domain lifecycle
markers (`menu-native/launcher-r6.log`, `menu-native/test.log`).

### Callback-entry menu view and complete fixture linkage

The tagged WOW branch of `wow_window_dispatch_bound` now refreshes the
native window's menu immediately after publishing its existing callback WND
and before original DispatchClientMessage enters guest code. This follows
createw.c/mnapi.c's association-before-callout order. Failure returns without
dispatching that guest callback; the existing finally restores the callback
view and releases task/object borrows. The native branch remains unchanged.
This is not a solution to all post-mutation publication failures: ordinary
menu mutation redirects and their failure coherence remain unactivated/open.

Worker/provider build PID 16720 (2026-10-14 14:28:18 local),
`menu-provider/launcher-r6.log`, completed exit 0. Native-branch regression
PID 3752 (14:29:01), `native-dispatch-gate/launcher-r2.log`, completed exit 0
with `WOW_NATIVE_DISPATCH_GATE errors=0 native=2 guest-entry=1`.

The real CCPU window-borrow fixture now binds a real runtime/task owner and
attaches a native menu before dispatch. Its supplied callback checks the
guest WND+104 menu pointer, original menu identity and count before nested
window destruction. Both normal and raised-exception paths verify callback
view restoration and object/storage retirement. Run PID 15028 (14:31:19),
`window-menu/launcher-r2.log`, completed exit 0 with
`WOW_WINDOW_BORROW_CCPU errors=0 normal=1 exception=1` and
`WOW_PAGE_DOMAIN_LIFECYCLE_OK`. This exercises the production callback
gateway and real CCPU data, not an actual Win16 program's instruction stream.
An earlier compile attempt exposed the test's duplicate inclusion of original
wowuserp.h; the redundant test include was removed, not the original ABI.

The fixture runner's prior unresolved startup closure was repaired by
selecting existing production system import libraries, version/common-dialog
libraries, worker implementation archive and the otherwise unextracted
symbol-free production ABI directive objects. The sole static-import data
cell points to actual worker Ldt, matching `ExpLdt = Ldt DATA`; the interrupt
alias matches `call_ica_hw_interrupt = ica_hw_interrupt`. No replacement LDT,
interrupt function or successful startup stub was added. These are fixture
composition changes, not a new runtime ABI or mirror diff. Existing duplicate
broker fixture seams remain explicitly outside this acceptance claim.

The repaired fixture composition also unblocked the previously failed
menu-cleanup acceptance: PID 2324 (2026-10-14 14:32:43 local),
`menu-cleanup/launcher-r3.log`, completed exit 0 with
`WOW_MENU_CLEANUP errors=0`, `WOW_USER_TASK_LIFECYCLE errors=0 native_direct=4`
and `WOW_FIXTURE_OK case=menu-cleanup`. This case uses its explicit storage
failure seam to verify original early/final cleanup phases, failed retirement
retaining the task carrier, successful retry and other-task isolation; real
CCPU storage is covered separately by the window/menu fixtures above.

The actual-application candidate runner now builds and replaces BOTH ntvdm.exe
and WOW32.DLL over the hash-checked accepted package. The new page-domain
exports make a WOW32-only replacement invalid. The isolated application
frontier run is a separate acceptance step, not implied by fixture success.

### Actual application failure exposes conditional pointer rebase mismatch

Isolated frontier run PID 5376 (2026-10-14 14:33:53 local),
`menu-callback-apps`, completed observation exit 0, NOT application acceptance:
WINMINE had its main window; SOL still displayed Out of memory; WRITE now
displayed a page fault in USER.EXE at 0001:0222. The candidate was not
published. Its per-image identities are in candidate-hashes.json and exact
window text is under frontiers/restored-*-windows.txt.

Original `wow16/user/k/daytona/user.map` maps 0001:0222 to assembly line 483,
`_GetMenu@4`'s `mov eax,[ecx]`. The immediately preceding original code compares
the WND menu pointer to 0x7ffeffff and only rebases values above that threshold
using WND.head.pSelf. The project page domain used client_delta 0x70000000;
low allocated menu storage plus that delta remains below the original
threshold and is wrongly interpreted as a client pointer. Prior tests always
subtracted delta and therefore failed to exercise the actual consumer rule.

The owned page domain now uses 0x80000000 for server-form addresses, retaining
its existing checked addition/overflow validation. No guest or original source
change is made. The CCPU menu fixture now evaluates the original conditional
GetMenu transformation against actual WND/MENU fields before dereference,
rather than assuming every pointer must be rebased. PID 21988 (14:36:59),
`menu-native/launcher-r7.log`, completed exit 0 with native-menu and page-domain
markers. Actual application reacceptance is a separate new immutable package
run; the preceding WRITE fault is retained as a failed result, not hidden.

The actual unchanged USER binary also passed the added GetMenu threshold
instruction assertion `66-81-F9-FF-FF-FE-7F`: profile runner PID 21084,
2026-10-14 14:38:32 local, `menu-profile/launcher-r2.log`, result.exit 0.
This confirms the conditional-rebase contract in the selected media, not
merely the retained assembly listing.

Reacceptance observer PID 15592 (14:37:34), `menu-rebase-apps`, completed
observation exit 0. WINMINE still presents its main window (no gameplay claim);
SOL still shows Out of memory; WRITE no longer shows the USER 0001:0222 fault
and instead reports `Cannot find WINSPOOL.EXE`, with MSWRITE_MENU still hidden.
This is initialization progress, NOT successful WRITE acceptance. Exact
windows are in that run's frontiers directory, and candidate-hashes.json
identifies its coherent worker/provider pair. No candidate has been published.

Source attribution for the next dependency is explicit: original
`wow16/kernel31/ld.asm` lines 1176 onward redirect a WINSPOOL.DRV / extensionless
winspool request to the original 16-bit WINSPOOL.EXE facade. The isolated
package's system32 contains no *spool* file. The original mirror retains
`wow16/winspool/WINSPOOL.EXE`, assembly, map and resources; existence alone is
not a runtime admission. Audit its provenance, immutable-media identity,
caller/dependency closure and packaging selection before using it. Do not
change the guest loader, substitute a native spooler DLL or suppress the
original error to claim recovery.

### Immutable spool companion: bounded isolated acceptance

The retained 2,112-byte WINSPOOL.EXE matches canonical OpenNT, SHA-256
`e4e787340f124476090ed0f22837d7d7acab37fd7bbe49db46cae9ac4944d535`.
The T301 supplement manifest records it as already-tracked-identical; T414
retains WOW16 as load-only media. Its sole NE module dependency is KERNEL.
Original DeviceMode/ExtDeviceMode/DeviceCapabilities forward DGDI IDs
0x1c4/0x1c5/0x1c6 to the existing WOW32 print thunks. ExtTextOut retains its
original int 3 at code offset 0x41 after the entry prologue at 0x37.
No guest bytes, source ZIP or installed system32 were changed. This exercise
selects the original companion only in a build-owned isolated package;
default packaging and publication remain unaccepted.

`verify-wow-spool-profile.ps1` pins the hash, NE dependency and thunk bytes
before staging. The first two background attempts failed in this new verifier,
before application execution: unavailable Get-FileHash module, then an incorrect
assertion at the procedure entry rather than the int 3 instruction. Both were
corrected against the immutable binary and retained LST, not by changing media.
Run `spool-companion-apps-r3`, PID 24440 at 2026-10-14 14:46:08 local, passed
the profile and completed observation exit 0. WRITE no longer displayed the
missing WINSPOOL error but displayed `Not enough memory for Write to complete
this operation`; MSWRITE_MENU remained hidden. This is a failed application
acceptance, not evidence of actual heap exhaustion or successful WRITE recovery.
Candidate hashes, profile and window/file traces are retained in that run.

### Standard-class closure: source audit after the menu dependency

The r3 existing dispatch witness records WU32CreateWindow (RVA 0x25980)
returning zero at nested depth 2 inside WRITE's main WM_CREATE; the enclosing
creation returns the main HWND. Original write/initwin.c::MmwCreate tests
GetMenu, FInitMemory, then creates ScrollBar children before its page-info
window. The retained write.map places MMWCREATE at 0002:05b0; the nested
guest return offset is 0674. This supports investigating child creation, not
claiming actual heap exhaustion. The caller's class argument was not captured,
so the trace alone does not prove which child or rejection branch failed.

Source independently proves the following missing production closure:

| Boundary | Original owner / current producer | Disposition |
| --- | --- | --- |
| FindClass16 prerequisite | walias.c calls GetClassWOWWords before W32CreateWindow invokes pfnCsCreateWindowEx | Preserve original code; absent WC metadata prevents even native creation. |
| Named class lookup | wow_user_borrow_named_class -> wow_class_words_find_atom -> original GetClassPtr | Current producer only registers application classes. system_classes is read but never populated by production. |
| Standard control registration | client/clinit.c::RW_RegisterControls registers Button, ComboBox, ComboLBox, dialog, Edit, ListBox, MDIClient, Static (and conditional IME); kernel/init.c::LW_RegisterWindows includes ScrollBar and server classes | Native USER already owns these classes. Do not copy NT4 controls/server implementations, register replacement classes or maintain an invented alternate class namespace. Supply the unavailable WC/CLS metadata using the existing owner and native class identities. |
| Client-module fallback | kernel/init.c::InitializeClientPfnArrays sets hModClient=hModUser; class.c::GetClassPtr searches it before the system list | Project lifecycle left it zero; _InnerGetClassPtr explicitly interprets zero as unrestricted module. Correct independently below. |
| Native control creation | create_bound requires an acquired class and publishes WW before WM_NCCREATE | Metadata alone is insufficient: standard native procedures do not enter wow_window_native_proc. Retain native behavior and establish matching creation/terminal metadata boundaries; no successful naked HWND workaround. |
| Dialog-created controls | dialog_create_hook currently enrolls only the #32770 root | Include automatic children and ComboBox-owned nested controls in the same lifecycle review; root-dialog success does not prove their WW/CLS access works. |
| Subclass/previous-proc | original walias/wsubcls retains conversion; registration bridge calls native CallWindowProc for native/CPD tokens | Reuse this path; do not call opaque native previous-procedure values as arbitrary function pointers, or reinterpret them as guest tags. |
| Retirement | original cleanup scans owned class/resource lists; current process dispose frees no system metadata | Borrowed native classes must not be UnregisterClass'ed. Any added metadata needs explicit process lifetime and outstanding-window/borrow handling. |

Existing original-class-lookup fixture supplies a synthetic system list; native
creation fixtures register custom classes. Neither proves production standard
classes are connected. Required acceptance covers original-name and atom
lookup, module-private precedence, each standard control family, failed
creation, dialog automatic children, nested destruction, subclass/restore,
shared WC words, task cleanup and process disposal. Tests follow implementation;
do not continue running WRITE to choose one missing control at a time.

Independent reviewed correction: lifecycle initializes client_module with the
already-loaded USER32 module, mirroring InitializeClientPfnArrays without a
new DLL load or original-source diff. The lifecycle fixture exercises actual
initialized context with original GetClassPtr: same-module instances match,
an unrelated private module does not, and the USER-client fallback does.
Background run PID11624, 2026-10-14 14:51:37 local, `class-client-module`,
passed `WOW_USER_TASK_LIFECYCLE errors=0 native_direct=4` and
`WOW_FIXTURE_OK case=task-lifecycle`. This is not standard-control recovery
or full production acceptance; no candidate release is published.

The subsequent full worker/provider background build PID26552 (14:53:10)
finished at 14:53:24 with result.exit=0 and LINK wow32.dll. Existing duplicate
export/DESCRIPTION linker warnings remain; no new unresolved symbol is hidden.
Second-role review of this bounded correction: USER32 is already imported by
the provider, GetModuleHandle does not acquire a new lifetime reference, NULL
fails initialization before allocations, and the original class-lookup body
is unchanged. Tests use the initialized lifecycle context rather than manually
injecting a nonzero client_module, which was the older fixture's blind spot.

For standard-class identity, canonical ntcftxt.h::GetClassInfoEx returns the
original NtUserGetClassInfo result unchanged, and kernel/class.c defines the
result as ATOM. Modern public documentation exposes only BOOL success; a
future native binding must not silently assume its numeric return is a class
atom without an explicit supported boundary and verification. The original
walias.c atom lookup uses RegisterWindowMessage for named standard classes
and fixed predefined atoms for #327xx. Do not substitute Base local/global
atom tables, duplicate the standard-class policy table, or treat a CPD token
returned as lpfnWndProc as a directly callable guest/native address.

### CLS ANSI-name prerequisite (same standard-class closure)

The source audit found another concrete producer omission, independently of
any new application launch: pinned USER16 `_GetClassNameA@12` reads WND+76,
rebases it with WND+12, reads CLS+100 and rebases that pointer through the
same WND. It then scans/copies the ANSI string. The existing class publisher
zeroed CLS+100. `verify-wow-user-profile.ps1` already pins all three relevant
instruction sequences in the immutable USER.EXE, so this is a required
original reader contract, not a speculative crash workaround.

Original kernel/class.c::InternalRegisterClassEx, lines370--414, caches an
ANSI conversion of the captured Unicode class name; class destruction and
desktop cloning retain its paired lifetime. The owned class-client boundary
now calls the same native RtlUnicodeStringToAnsiString/RtlFreeAnsiString for
its captured name. The worker copies that counted NUL-terminated string into
the existing CLS allocation, immediately after its original 108-byte layout,
and sets lpszAnsiClassName to its server-form guest address. Class retirement
still frees one original allocation. No host text pointer is retained in
guest state, no new GetClassName implementation is added, and the original
USER16 reader remains unchanged. A layout assertion pins CLS+100.

The expanded private call is named
`mvdm_softpc_wow_page_domain_publish_named_class`; the old ten-argument
publish_class export is removed rather than silently keeping its name with
two extra arguments. Both generator and current generated DEF select the
one new implementation. A mismatched old provider must fail import binding
instead of reading absent arguments; guest/wire contracts do not change.

The real CCPU fixture validates NULL/zero/nonterminated/overflow input
rejection, the exact two-stage WND-relative rebase, source-buffer mutation
independence and existing paired class/window retirement. First run
`class-name`, PID26224 at 2026-10-14 14:58:35, passed. The full suite before
the export rename (`class-name-suite-r2`, PID18664 at15:02:12) passed worker
and provider builds, original class-client policy, real window/CCPU normal
and exceptional callback cleanup, and the immutable USER profile. Markers:
`WOW_CLASS_NAME_CCPU copied=1 rebase=1 bytes=14`,
`WOW_WINDOW_BORROW_CCPU errors=0 normal=1 exception=1`, and both fixture OK
markers. The final named-export suite `class-name-suite-r3` (PID9580,
15:05:10) subsequently completed with suite.exit=0. Its independent PE
inspection (PID13828) recorded `WOW_NAMED_CLASS_IMPORT_EXPORT_OK`: the worker
exports and provider imports the new entry, without the old entry. This adds
no mirror/guest diff and does not close standard-control enrollment.

Actual coherent-package reacceptance `class-name-apps`, PID23028 started at
2026-10-14 15:09:36 local, completed observation exit0 and process cleanup.
It includes the pinned immutable spool companion. WINMINE displayed its main
window; SOL still displayed `Out of memory`; WRITE still displayed its
initialization-memory error. The observer's per-app `result=timeout` is its
bounded observation endpoint, NOT application success. No gameplay or normal
SOL/WRITE initialization is claimed. Candidate identities and window samples
are retained beneath that run; no release or installed binaries changed.

### Standard-class integration review: lifecycle, not a name fallback

The next implementation must close these interfaces together, rather than
allowing a native HWND to escape without the original WOW metadata:

1. **Class selection and ownership.** Retain original `GetClassPtr` private,
   public, client-module and system-list ordering. Original
   `kernel/class.c::InternalRegisterClassEx` lines456--469 selects list
   membership using CSF_SYSTEMCLASS and registration flags, not merely the
   fact that a class is a familiar built-in control. `LW_RegisterWindows`
   registers ScrollBar with CSF_SERVERSIDEPROC, without CSF_SYSTEMCLASS.
   Therefore placing every borrowed native control in `system_classes` is
   not a source-equivalent general solution. Establish the original list
   disposition before publishing each admitted native class. Preserve the
   existing original `walias.c` class/thunk table; do not duplicate it as a
   separate routing policy.
   Follow-through confirms `ntcftxt.h::RegisterClassExWOWW` supplies dwFlags=0,
   and `RW_RegisterControls` supplies CS_GLOBALCLASS: those client controls
   belong to the process public list, not gpclsList. ScrollBar's server-proc
   registration also enters the public list. This corrects any inference
   that filling only `system_classes` would restore all standard classes.
2. **Early enrollment.** `create_bound` acquires WC before calling public
   CreateWindowEx and its CBT scope attaches WW before WM_NCCREATE. Retain
   this ordering for native controls. Dialog and control-generated children
   need the same enrollment, including children created after a parent API
   returns; enumerating children only after creation is too late for their
   initial callbacks and original direct USER16 readers.
3. **Procedure identity.** `wow_window_dispatch_bound` currently rejects
   WNDPROC_HANDLE, while public native procedures can be CPD values. The
   existing registration bridge already resolves native/CPD calls through
   CallWindowProcA/W and tagged WOW calls through the original callback.
   Reuse that boundary; do not cast CPD to a callable pointer. Native-only
   dispatch must not acquire guest execution ownership. Original WOW message
   conversion, task callback scopes and DispatchClientMessage remain owners
   of their respective paths.
4. **Subclass and restore.** `wow_user_set_window_longA` substitutes logical
   procedures only while the native WNDPROC is `wow_window_native_proc`.
   Merely attaching WW to an unchanged native control leaves its fallback
   calling SetWindowLongA with a guest-tagged value. Integrate the gateway,
   logical procedure reads/writes and native previous-procedure identity as
   one contract, including restoration. Do not register replacement native
   control classes or change their native class procedures globally.
5. **Terminal ownership.** Current WM_NCDESTROY cleanup runs in the gateway;
   native controls that never enter it cannot rely on that cleanup. Pair
   successful early enrollment with exact-instance terminal retirement and
   creation-failure rollback, including nested destruction. Borrowed native
   class metadata must not use `wow_class_words_destroy_native`, which calls
   UnregisterClassA. Process disposal must retire only the owned projection
   and references, after windows and borrowers have released them.

Independent review rejects a ScrollBar-only bypass, a duplicate standard
class table, blanket thread-window enrollment, opaque-procedure direct calls,
and an unpaired metadata cache. These would respectively leave the same
defect in other controls, duplicate original policy, claim unrelated native
windows, break A/W procedure conversion, or leak/retire live metadata.
The new bounded group is still under source design; no standard-control
implementation or acceptance is represented by the preceding class-name
tests. Expected additional guest diff is zero; no new original-source diff
has been made during this review.

Caller-side confirmation: `wuwind.c` lines833--886 places the entire
`pfnCsCreateWindowEx` call inside `if (pwc)` after FindClass16. A missing WC
therefore returns NULL before public native CreateWindowEx, even for a
standard control whose host class already exists. Integer guest class input
is resolved to a name earlier in this function; original GetClassWOWWords
uses string capture, and kernel `_GetWOWClass` uses FindAtomW. Its existing
named-query validation is not independently relaxed to accept integer
pointers as a supposed fix. Name/atom creation coverage must exercise the
original caller conversion, not bypass it with a different private contract.

### Native procedure prerequisite and automatic-child semantics

Original `createw.c::xxxCreateWindowExWOW` associates the current THREADINFO,
CLS and reference count before creation callbacks; it copies adwWOW only when
lpWOW is supplied. Original `walias.c::FindPWW` already fills an initially
unknown class through GetStdClassNumber and SETWL. Automatic native children
therefore need zero-initialized shared WW storage and a valid class owner,
not a new host reimplementation of WOW class initialization. Reuse FindPWW.
Thread-scoped enrollment still requires a proven current WOW task owner;
the earlier rejection of blanket enrollment means not claiming unrelated
native threads/windows, not excluding a native child merely because its
creation did not directly call WU32CreateWindow.

The next implemented prerequisite is CPD handling in window-owned procedure
storage. Original `xxxSetWindowData` resolves a CallProc handle before storing
WND.lpfnWndProc; original DispatchClientMessage assumes that resolved value
and tests WNDPROC_WOW. Modern public USER exposes CallWindowProc, not the
private resolver. Merely admitting FFFF into the original procedure field
would make dispatch/cleanup misclassify a native token as guest code.

The owned binding now keeps the opaque native CallProc separately and stores
a native forwarding address in the existing cleanup/dispatch field. That
entry borrows the existing window, snapshots the native token, releases the
publication lock, calls public CallWindowProcA, and releases the borrow in
finally. Reads and exchanges expose the original token, never the forwarding
address. Replacement by a guest or ordinary native target retains existing
behavior; restoring the token restores the forwarding path. Original
DispatchClientMessage and cleanup are unchanged. No public native class is
re-registered and no guest address is sent to native CallWindowProc.

Full worker/provider build and a real Unicode-window A/W CallProc fixture
completed under `native-callproc`, PID23292, 2026-10-14 15:19:58 local,
with suite.exit=0. Actual markers are
`WOW_NATIVE_CALLPROC token=FFFF07D9 calls=2 restored=1`,
`WOW_NATIVE_DISPATCH_GATE errors=0 native=3 guest-entry=1`, and
`WOW_FIXTURE_OK case=native-dispatch-gate`.
The fixture requires an actual FFFF token returned by USER, exercises ANSI
text delivered as Unicode, replacement/restoration, zero guest-entry calls,
and destruction during dispatch. The preceding native exception test remains.
Independent review confirms original `kernel/getset.c` lines552--577 resolves
CPD before storing the procedure and its A/W state. The new forwarding entry
keeps that unavailable representation boundary outside the mirror. The lock
protects the paired token/procedure update, never the native callback; borrows
survive the callback's property removal and DestroyWindow. The class-level
procedure contract is not silently relaxed by this window-level change.

Adjacent-test audit found the retained window-procedure fixture still asserted
unconditional CPD rejection and had no case in the current lifecycle runner.
It now uses an actual USER-produced A/W identity, keeps raw-CPD direct-dispatch
rejection, exercises guest-target replacement and token restoration, and has
an explicit runner case. Its task scope is an explicitly labelled mock for
procedure-storage testing, not scheduler evidence. Expanded background suite
`native-callproc-r2`, PID12524 at15:22:45, runs that fixture, the native-entry
fixture and real CCPU window-borrow acceptance. Its outcome is not inferred
from the first fixture's success.

That expanded run ended unsuccessfully at the window-procedure completion
marker. Its linked map resolves `_printf` to original SoftPC `host/src/fprt.c`,
whose PROD body returns without printing; both captured output files were
empty. This was not accepted as a test pass. The retained fixture now uses
CRT fprintf(stderr), and the runner records `fixture-exit.txt` before checking
markers so an abnormal exit cannot be concealed by a missing-marker report.
No product change was made to fix the test-output issue.

Final run `native-callproc-r3`, PID20412 at15:25:44, completed suite.exit=0.
It passed native dispatch, the restored old window-procedure fixture and
real CCPU window-borrow. The old fixture reports
`WOW_WINDOW_PROCEDURE errors=0 tagged_calls=3`, actual native token FFFF07EB,
four correct A/W text deliveries and fixture-exit=0. It includes guest
replacement and native restoration, not only native-to-native replacement.
The CCPU fixture retains `WOW_CLASS_NAME_CCPU copied=1 rebase=1 bytes=14`,
`WOW_WINDOW_BORROW_CCPU errors=0 normal=1 exception=1` and lifecycle OK.
This verifies the bounded CallProc prerequisite, not native-control class
publication, automatic-child enrollment or SOL/WRITE recovery. Mirror diff
counts are unchanged by this group; no publication, commit or push is claimed.

The standard-control audit also identifies an independent semantic boundary:
original `wmsgem.c::ThunkEMMsg16` packs EM_SETHANDLE with the application's
instance, and UnThunkEMMsg16 extracts EM_GETHANDLE's 16-bit handle. Original
EDIT `edecrare.c`/`editec.c` uses LOCALALLOC/LOCK/REALLOC with that instance;
`wcall32.c` supplies the guest local-heap callbacks. Native control creation
alone does not prove this contract, nor the tagged word-break callback in
the same original message converter. Keep these explicit in the control
message/data audit; do not call a visible native EDIT full Win16 EDIT
compatibility. This finding does not justify rewriting the original thunk or
prevent independently correct window/class storage recovery.

### Standard-class descriptor: original table, native identity check

The first storage-integration boundary now consumes the existing original
`walias.c::stdClasses` and `GetStdClassNumber`, rather than adding another
name/ordinal/procedure table. Its private five-field declaration is an x86
link ABI view of that original object, with size20 and offsets4/8/12/16
asserted. The existing linked map identifies `_stdClasses` and
`_GetStdClassNumber@4` in walias.obj. Original predefined atoms are retained;
named atoms use the original RegisterWindowMessage producer/cache. The
descriptor validates native GetClassInfoExA by both canonical name and atom,
checking procedure, instance, style and extra sizes. The BOOL return is never
interpreted as an atom. Failure does not publish the output descriptor.

FNID is a translation of the original WOWCLASS index to original
clinit.c/init.c registrations, not a second classifier. Thirteen exact
definitions from original ntuser/inc/user.h were added to the already
selected declaration subset (plus two comment lines). This is +15 lines of
additional declaration-only mirror material and zero original executable or
guest changes. The existing OPENNT-HOST-063 bounded-layout ownership remains.
Source identities, read-only canonical OpenNT:

- `windows/core/ntuser/inc/user.h`, SHA-256
  `adcf0ac89c4779d9b7908610976f4a47d2385403edc37496c8ab004b016cf6c1`.
- `base/mvdm/wow32/walias.c`, SHA-256
  `6118b6837c7ed291cc9d70446b7d691699d49c9ef6f2be60d8e0fac478d93b65`.

The unavailable original alternative is private USER class/atom storage;
full USER-server import is excluded. Reusing the already linked original
classification/data and querying public native class facts is the bounded
binding, not replacement control implementation. No new class registration,
window, guest object or independent identity namespace is created here.

Background build/fixture `standard-class`, PID18144 at2026-10-14 15:32:56,
completed full worker/provider linking and suite.exit=0, with
`WOW_STANDARD_CLASS errors=0 resolved=9 original_table=1` and fixture OK.
The nine families are Button, ComboBox, ComboLBox, Edit, ListBox, MDIClient,
ScrollBar, Static and dialog; each was queried by name and returned atom.
This confirms the original cached identities against actual native USER,
not a synthetic class registry. Independent declaration review also retains
the original `_stdclass` structure tag across translation units, rather than
using a differently tagged but layout-identical external declaration.
This is a descriptor prerequisite: its production consumer WC/CLS publication,
borrowed-class retirement and automatic-child enrollment are not connected
yet, and no application recovery is claimed from it.

### Standard-class connection review: lookup, procedures and retirement

The final descriptor run `standard-class-r2`, PID19168 (started at15:35:35),
is terminal: suite.exit=0 and `WOW_STANDARD_CLASS errors=0 resolved=9
original_table=1`. The final original `_stdclass` tag is therefore covered;
there is no remaining background wait or owner approval on this group.

Source tracing, rather than another application trial, establishes the next
connection group. Read-only canonical inputs and SHA-256:

- `windows/core/ntuser/kernel/class.c`:
  `82528443bc5f6c227e0e14fe8a9dbc928ba735f98cf3891744c20c8e1604939a`.
- `windows/core/ntuser/client/clinit.c`:
  `83c7e304b38501c9afd1251c2cf671f1bc0e799560af10b225cedee891e3e8a2`.
- `windows/core/ntuser/kernel/init.c`:
  `7489347051f1a4f5096d5fc162ce0eab6851510b9f1dca0b5b799febc3943bd6`.

`RW_RegisterControls` registers Button, ComboBox, ComboLBox, dialog, Edit,
ListBox, MDIClient and Static with CS_GLOBALCLASS. `LW_RegisterWindows`
registers ScrollBar with CSF_SERVERSIDEPROC. Original InternalRegisterClassEx
lines456--467 places either kind in the process public list unless the
explicit CSF_SYSTEMCLASS registration flag is present. In particular, a
server-side procedure does not imply system-list membership. Desktop and
IconTitle have additional system registrations; it is inaccurate to move
every predefined #327xx class into one system list merely from its name.
Retain the already selected original GetClassPtr; do not add a new native
lookup that bypasses its private/public/client-module/system precedence.

The source-defined end-to-end gaps are now:

| Boundary | Current code and required connection |
| --- | --- |
| Pre-creation WC | Original wuwind.c calls FindClass16 before any native creation. wow_user_borrow_named_class reaches only owned registrations today. A native class descriptor alone cannot pass this gate. Publish one shared WC binding in the correct original list, not one copy per window. |
| Class procedure identity | Original wuclass.c explicitly distinguishes FFFF CallProc handles from tagged guest procedures and uses original GetThunkWindowProc. The current class setter rejects every FFFF value; its private-access gateway test also falls through to native SetClassLong when a class is not gateway-registered. A standard class must not let that fallthrough install a guest address into USER. Logical class read/replace and native dispatch normalization must be paired, retaining original thunk conversion. |
| First callback | creation_hook currently enrolls only its requested root. A native control's nested children need the same existing task/WW ownership before guest-visible callbacks, not enumeration after creation. Preserve native previous procedures and existing subclasses; do not classify every native window on an unrelated thread as WOW. |
| Guest CLS | Existing publication accepts procedure and module fields but has no established native-procedure projection contract. Copying native WNDPROC/CallProc values into guest CLS, or silently substituting zero, is not an approved solution. Trace the original guest readers before activating this projection. |
| Retirement | wow_class_words_destroy_native unconditionally calls UnregisterClassA; lifecycle_dispose has no borrowed-class drain. Imported native class metadata needs explicit borrowed ownership, paired projection/storage retirement and live-window/borrow checks. It must never unregister the host built-in class as a side effect of task cleanup. |
| Control-specific data | EDIT's guest local-heap and word-break callback contracts remain separate required connections. Passing native creation does not close them. |

The procedure gap is not a request to rewrite wuclass.c: its CPD handling is
already present, including GetClassLong and SetClassLong restoration. Reuse
that code. The adapter must present the correct logical value while keeping
native-only callable state out of the original WOW-tagged cleanup fields.
Likewise, original walias.c::FindPWW already classifies zero-initialized WW;
do not duplicate its classification in a new control implementation.

Review conclusion: activate WC publication, native creation enrollment,
class-procedure access and borrowed retirement as one connected group, not
by enabling the descriptor fallback alone. Additional original executable
diff budget remains zero for these owned-storage boundaries. The existing
15 declaration lines are unchanged. Guest projection semantics and native
automatic-child ownership require source resolution within this group,
not an owner policy decision and not repeated SOL/WRITE trial launches.
No production connection, application success, publication or delivery is
claimed by this design review.

### Class CallProc restoration prerequisite

Original `wuclass.c::WU32GetClassLong` and `WU32SetClassLong` already recognize
FFFF procedure handles, preserve their distinction from WOW-tagged guest
procedures and use GetThunkWindowProc/IsThunkWindowProc. The owned
wow_class_words_exchange_procedure instead rejected FFFF unconditionally.
This also prevents an existing enrolled application's class from restoring
a previously obtained native A/W procedure, independently of the unconnected
standard-class factory.

The repair reuses the existing WND CallWindowProcA forwarding entry rather
than adding another dispatcher. Each class stores its native identity apart
from entry.lpfnWndProc; the latter contains the native forwarding address for
original cleanup. A per-binding SRW lock protects the pair on read/exchange,
and no callback runs under that lock. target() returns the logical native
identity to existing original callers and to window creation, where the
already verified WND binding snapshots it. Changing a class target does not
rewrite an existing WND. Original registration's CPD-resolution boundary is
unchanged; no new acceptance of an unresolved initial registration is implied.

The retained window-procedure fixture now restores a real USER A/W identity
through class storage, dispatches ANSI text to its Unicode procedure through
a window snapshot, changes/restores the class target, and checks that the
existing window remains native without acquiring guest execution ownership.
It also asserts the original cleanup field has no WOW tag. The runner requires
`WOW_CLASS_CALLPROC restored=1 isolated=1 errors=0` in addition to all earlier
window-procedure assertions. This storage test uses its explicitly labelled
task-scope mock, not application or scheduler acceptance.

Hidden background run `class-callproc`, PID13980 at2026-10-14 15:43:20, uses
the existing incremental provider/worker builds and runs native-dispatch,
window-procedure and real CCPU window-borrow cases. Full provider linking
completed; final fixture results are not assumed until suite.exit is present.
Additional mirror/guest diff is zero. This does not activate standard classes
or close their native private-access fallthrough, child enrollment or cleanup.

That same run is now terminal with suite.exit=0. Native dispatch reports
`WOW_NATIVE_DISPATCH_GATE errors=0 native=3 guest-entry=1`; class/window
storage reports `WOW_CLASS_CALLPROC restored=1 isolated=1 errors=0` and
`WOW_WINDOW_PROCEDURE errors=0 tagged_calls=3`. Six A/W text calls reached
the native Unicode procedure, including two through the class snapshot.
Real CCPU window-borrow retains normal and exception coverage, class-name
rebasing, native-menu entry and `WOW_PAGE_DOMAIN_LIFECYCLE_OK`.

Second-role review checked the actual paired-field diff, unchanged original
registration guard, recursion rejection, existing-window isolation, and that
the shared forwarder releases its window borrow in finally with no class lock
held across CallWindowProcA. No second procedure dispatcher was introduced.
The original mirror counts remain wcall16.c +18/-26, wowhooks.c +2/-1 and
the selected declaration-only user.h +75/-0; none changed for this repair.
Background governance PID10216 at15:44:54 reported verification passed.
No release binaries were copied, and no commit/push or S2 closure is claimed.

Additional source narrowing: usercli.asm's optimized local API list includes
GetClassName, but not GetClassLong; user2.asm retains UserThunk GETCLASSLONG.
Thus class-procedure API conversion remains owned by original wuclass.c,
not a newly invented guest CLS reader. The pinned binary and complete local
reader closure must still constrain any native CLS projection; this source
observation does not authorize publishing host pointers or a dummy procedure.

### Creation enrollment rollback before CBT veto

The standard-window lifecycle sweep found an existing error path, independently
of native class import. Both creation hooks can publish WW before creation is
accepted. Their outer cleanup runs after CreateWindow/CreateDialog returns,
but wow_window_words_detach_if first validates a live HWND. Native destruction
can therefore remove the property without releasing its owned heap allocation
before the adapter attempts rollback. Dialog enrollment additionally assigned
creation.window only after all publication succeeded, so an earlier failure
did not even retain the HWND needed to withdraw the attached words.

Original `windows/core/ntuser/kernel/createw.c`, SHA-256
`da4a0c0e0a85dbe88130f01ccceecf9da08a0febf3f841458d2a3cbd707a57d7`,
lines394 onward routes a nonzero HCBT_CREATEWND result to MemError; that path
marks destruction and calls xxxFreeWindow before returning NULL. The original
WND and its class reference are server-owned and freed together. This adapter
cannot depend on its later native procedure receiving a terminal message for
a window vetoed before normal creation. No original creation policy needs
replacement or mirror modification.

The owned hooks now withdraw their publication while HWND remains valid:

- Own owner-publication failure detaches before returning the veto.
- Dialog enrollment records HWND after WW attachment; failed acquisition,
  owner publication or dialog-property publication releases only its own state.
- A downstream CBT veto or unwinding exception rolls back in finally before
  returning control to USER. The downstream result/error is preserved.
- Creator and external storage borrows survive withdrawal and release at their
  existing endpoints. No original WNDPROC, task scheduler or guest changes.

The existing dialog-lifecycle fixture now also exercises ordinary bound window
creation using its real initialized task/class context. Its explicitly mocked
CCPU publication boundary rejects publication for one case in each path;
a real downstream CBT hook vetoes another case in each path. Checks cover
no guest initialization on rejection, cleared published-object identity,
class unregister after ordinary-window rollback, and actual WW heap allocation
release. For hook-veto cases a test borrow remains live across failed creation;
HeapWalk proves the allocation disappears only after that final borrow releases.
For failed publication the test observes the property allocation without
reentering the production publication lock, then verifies it is no longer
allocated. This is stronger than checking NULL HWND alone and is not a claim
to test actual CCPU allocation failure.

Background run `creation-rollback`, PID12632 at15:51:43, was explicitly stopped
after review found the first test injection would recursively acquire the
publication SRW lock. It has no accepted result. Its process tree was confirmed
absent before reusing the build cache. The corrected background run is
`creation-rollback-r2`, PID11300 at15:53:08, with mandatory marker
`WOW_CREATION_ROLLBACK cases=4 errors=0`, retained normal/nested dialog checks,
and the separate real CCPU window-borrow regression. Final results remain
pending until the runner's terminal record. Additional mirror/guest diff is
zero; standard-class publication and automatic-child enrollment remain open.

The corrected run's dialog-lifecycle case has now passed:
`WOW_CREATION_ROLLBACK cases=4 errors=0` and
`WOW_USER_TASK_DIALOG_LIFECYCLE errors=0 init=2 terminal=2`.
The ordinary-window cases use wow_window_create_boundA with the same real
initialized task carrier, not a second fake creation implementation. Its
native class is unregistered successfully after both rollbacks, covering the
class association count as well as physical WW lifetime. The CCPU case is
still running; this partial result is not its outcome or suite completion.

Adjacent class-API sweep adds GetClassInfo to the standard-class connection
checklist: original wuclass.c::WU32GetClassInfo calls native GetClassInfo and
classifies the returned procedure before GetThunkWindowProc. Current private
access redirection covers Get/SetClassLong but there is no equivalent logical
class view for GetClassInfo. For gateway-registered classes the native answer
is the gateway, not the logical guest procedure. The OLD_WAY FindClass16
fallback in that source is compiled out and cannot be cited as a recovery.
Future shared-class/procedure integration must cover this query and
superclassing together with Get/SetClassLong, keeping original WOW conversion
and class-extra accounting. Do not enable standard-class creation and silently
leave these two views inconsistent. This is a source-proven integration gap,
not an observed application failure attribution.

Final `creation-rollback-r2` result is suite.exit=0; PID11300 is no longer
running. The real CCPU case reports `WOW_WINDOW_BORROW_CCPU errors=0 normal=1
exception=1`, retained class-name/menu assertions and page-domain lifecycle OK.
Governance PID7136 (15:55:07) is terminal with verification passed; diff check
also passes. Second-role review confirms the veto value is returned unchanged,
cleanup preserves the downstream last error, rollback removes only its own
dialog property, and the creator retains its scope reference until native
creation returns. No lock is added around native callbacks. Actual fixture
allocation checks cover publication rejection and downstream veto, not all
possible native exceptions or failure of the page-domain retirement operation.
Those limits remain explicit rather than inferred from finally syntax.
This repairs the reviewed creation failure paths; it does not make the absent
standard-class WC/CLS factory, automatic-child enrollment or application
initialization complete. Accepted release binaries remain untouched.

### Original GetClassInfo client reconnection

The missing class query is now connected for the existing enrolled classes.
Original `inc/ntcftxt.h::GetClassInfo` composes with the already selected
capture/copy/cleanup macros; its complete body is imported unchanged into
that same subset and compiled in the existing original ANSI class client.
The source SHA-256 is
`e7f3d42ce8939bbe0b14ea34bc693107334c19f8b2d7862ec15b75a501d11dab`.
Actual additional mirror lines are +48/-0, including the subset comment;
there is no alteration inside the original function and no guest change.
OPENNT-HOST-046's README entry records this extension.

The unavailable private boundary is original NtUserGetClassInfo. Its server
body `_GetClassInfoEx` depends on kernel CLS/cursor/CPD owners and version
policy; it is not imported wholesale or replaced with a new class registry.
The bounded owned binding uses retained GetClassPtr for an enrolled class,
queries actual public USER fields by that exact atom and registration module,
and supplies the existing logical procedure and original client menu. NULL
module lookup uses the existing client_module, never the private-list wildcard.
This also preserves the original high-word module match when the caller uses
a different low-word instance. Unenrolled native queries keep native behavior.
The original client, not a duplicated wrapper, restores the caller's class-name
pointer and copies output only on success. Current class context and temporary
name/reference cleanup are restored in finally.

The production redirect in wow_user_private_access.h now connects original
GetClassInfoA callers, including WU32GetClassInfo, to that client. Original
WOW tagging, standard thunk selection and class-extra accounting in wuclass.c
are unchanged. GetClassInfoEx is not newly redirected. Original server
version-policy recovery and the standard-class factory remain separate open
requirements; this repair does not claim native default behavior equals every
NT4 server policy.

Background run `class-query`, PID18744 at2026-10-14 16:00:29, verifies the
selected GetClassInfo function body equals canonical source, then rebuilds
the full provider and runs class-query, window-procedure and standard-class.
`WOW_ORIGINAL_GETCLASSINFO_BODY_OK` and full linking have completed. The
new fixture already reports `WOW_CLASS_QUERY errors=0 queries=6 logical=1
native=1`: named/atom lookup, different low-word instance, changed logical
procedure, captured client menu, unchanged output on failure, absence of
NULL-module wildcard selection, native Button/external-class queries and
no-context passthrough. Original class registration and unregistration are
used; CCPU class projection is explicitly mocked in this fixture, so this is
not application initialization or guest projection acceptance. Remaining
suite cases are not assumed complete from that marker.

The first class-query suite completed with suite.exit=0, including retained
class CallProc/window snapshot and all nine standard-class identity checks.
Independent review then found a necessary negative: a deferred class can
still exist in native USER after original GetClassPtr rejects it. Native
fallback must not expose the gateway for that rejected registration. The
binding now returns ERROR_CLASS_DOES_NOT_EXIST when no selected metadata
exists but the native procedure is the project gateway. The fixture marks
the actual enrolled entry CSF_WOWDEFERDESTROY, proves native GetClassInfo
still sees it, and requires the product query to fail without changing output.
This retains the original lookup decision; it adds no new deferred-class policy.

Final run `class-query-r2`, PID25652 at16:04:12, rebuilds and verifies that
change with the same source-body, query, procedure and descriptor checks.
The product map `build/M0-T445/S6/provider/wow32.dll.map` contains
wow_original_get_class_info in class-client.obj, wow_class_client_query_native
in the owned client binding, and wow_user_get_class_infoA in the selected
private access object. This is a linked production path rather than an unused
test helper. Runtime app and full WOW acceptance remain outstanding.

The final class-query-r2 suite has now terminated with suite.exit=0. Its
query, class/window procedure and nine-standard-class markers all pass;
class-query-review/governance.log also reports success. These remain the
bounded contracts above, not standard-control creation or application recovery.

### Class registration: publish the completed original result

Source review of the standard-class dependency exposed a separate definite
ordering defect in existing application-class registration. The selected
original kernel/class.c::_RegisterClassEx first calls InternalRegisterClassEx,
then writes lpfnWorker, both client menu pointers, both adwWOW words and the
TIF_16BIT-dependent hTaskWow. The owned InternalRegisterClassEx callback
published the guest CLS before that original sequence completed. Consequently
the published adwWOW fields were the allocation's initial zeroes, even though
the native WC subsequently contained the original input. The callback also
duplicated hTaskWow selection without the original TIF_16BIT predicate.

wow_class_client_bindings.c now keeps the newly registered entry only in its
call-local publication record. After _RegisterClassEx succeeds, the existing
page-domain publication consumes the completed original entry. The duplicate
task assignment is removed; original class policy, copy and task selection
remain unchanged. Projection failure uses the existing native unregister
rollback before the original client frees menu names. No mirror, guest or
private export ABI changes are added. Later class-field mutation coherence
is a distinct unresolved shared-view requirement, not proved by this change.

Background run class-commit, PID15112, started2026-10-14 16:14:02. Its driver
is build/M0-T447/S2/verify-class-commit.ps1; output and logs stay in the
matching class-commit directory. It rebuilds the provider, runs class-query,
class-client and real-CCPU window-borrow, then documentation governance.
The query fixture's new publication observer checks both nonzero WOW words,
one publication per registration, original task identity, a failed projection
with no remaining native registration, and a subsequent non-WOW registration
with ptdb present but hTaskWow zero. This observer mocks only the page store;
it must not be cited as a real-CCPU registration-to-memory assertion.

### Standard-control closure review: implementation boundaries

The following connected groups remain necessary; a descriptor query alone
does not implement them:

| Boundary | Original evidence and required connection |
| --- | --- |
| Before explicit CreateWindow | wuwind.c calls FindClass16 before even selecting the standard-class WW values. The current GetClassWOWWords binding searches only enrolled lists. Native USER having a class does not populate those lists. |
| Standard-class ownership | clinit.c::RW_RegisterControls registers process-global controls; init.c::LW_RegisterWindows also registers ScrollBar without CSF_SYSTEMCLASS. Do not put every predefined or standard class into the system list. Retain class.c::GetClassPtr precedence. |
| Registration and removal | Native standard registrations already exist. Their metadata must not re-register the classes. Process metadata teardown and explicit guest UnregisterClass are separate operations; eager process initialization avoids a lazy miss silently resurrecting an explicitly removed class. |
| Explicit and internal creation | Current window_creation_binding matches only its requested root. dialog_creation_binding enrolls only its first #32770 root. Dialog children and ComboBox-created children need the same existing task/WW/CLS owner before guest-visible callbacks, not post-creation enumeration. |
| Procedure identity | A native class procedure is not the HWND's current subclass chain. Preserve the actual previous HWND procedure, original WOW classification and CPD normalization; never route a FFFF native identity through a WOW-tag branch. |
| Existing subclass chains | wow_user_private_access currently substitutes logical WNDPROC only when native USER's top procedure equals the project gateway. A later native subclass can sit above it; standard-control integration must review that chain rather than assuming the gateway remains topmost. |
| Guest projection | usercli.asm selects GetClassName as a local optimized reader; user2.asm retains GETCLASSINFO/GETCLASSLONG/GETCLASSWORD host thunks. This narrows required CLS readers but does not authorize placing native pointers in guest memory or declaring all native fields interchangeable. |
| Control-private data | Native Edit creation alone does not recover original wmsgem.c EM_GETHANDLE/EM_SETHANDLE and guest local-heap/wordbreak callback contracts. These are explicit source dependencies, not optional tests after declaring controls complete. |

This review selects no second standard-class table, no replacement control
renderer, no new scheduler and no guest rewrite. It also does not claim the
observed SOL/WRITE failures have a single established cause. Implement the
creation/storage/retirement connections as a group before another application
initialization acceptance run.

The first class-commit run completed with suite.exit=0 and governance pass.
Independent review requested stronger evidence than its mocked publication
observer: the existing window-borrow CCPU fixture now calls the actual original
class-client registration, original _RegisterClassEx, production owned binding
and selected worker page-domain publisher. It reads guest CLS offsets36/40/44
through CCPU and requires both input words and the original task decision;
private/WOW and public/non-WOW cases both unregister and restore the original
allocator's free-byte count. No WNDPROC callback is permitted. The runner now
requires WOW_CLASS_COMMIT_CCPU as well as the class-query commit marker.
Review also stopped a negative native query from overwriting the unrelated
saved Button query output used by a later assertion.

Final background run class-commit-r2, PID26392, began2026-10-14 16:18:05 with
the same driver and stable build roots. This is still host-driven original
registration in a real CCPU page domain, not immutable USER16 application
initialization. Its terminal result is recorded after observation below.

Further standard Edit source tracing establishes a concrete missing consumer,
not merely hypothetical incompatibility: original client/globals.h maps all
six LOCALALLOC/REALLOC/LOCK/UNLOCK/SIZE/FREE macros to callbacks supplied by
UserRegisterWowHandlers. edecrare.c allocates ped->hText through LOCALALLOC;
editec.c locks it through LOCALLOCK. The current bridge retains those six
pointers but only consumes the version and WNDPROC callbacks for class/task
construction. Original wmsgem.c passes a packed Win16 handle and instance in
EM_SETHANDLE and extracts the 16-bit identity in EM_GETHANDLE. Forwarding those
unchanged values to an ordinary native Edit does not establish the matching
local-heap owner. Recovery must audit the original edit-client package or a
source-shaped unavailable-boundary binding before claiming this control's
full semantics. Canonical read-only source identities:

- windows/core/ntuser/client/editec.c:13e3aa26649c7b21a49a3f3114a73bc06fdfc6472b782663bb10879cabc0ddb6
- windows/core/ntuser/client/edecrare.c:b50e0f8f305f6456b3a952ddd63c815357136f222328c6d83dd0e76e697b5634
- windows/core/ntuser/client/globals.h:fd78200232574074f7ec68d2f5b851c27ee9d5024427310d18ebbcb048336e97

Final class-commit-r2 terminated with suite.exit=0 and no remaining runner.
Full provider link passed. Query reports errors=0/queries=6; commit observer
reports errors=0/publications=3/rollback=1/original_task=1; original class-client
passes. Real CCPU reports WOW_CLASS_COMMIT_CCPU errors=0 cases=2
original_words=1 original_task=1 released=1, retains normal/exception window
borrow checks, and ends WOW_PAGE_DOMAIN_LIFECYCLE_OK. Governance also passed.
These results accept this registration ordering correction only. No SOL/WRITE
success, C01/C02 closure, release replacement or P delivery is implied.

### Standard-class shared storage and native subclass ownership

Background standard-storage, PID19360, started2026-10-14 16:28:21 and
terminated with suite.exit=0. The full provider build completed25 actions.
The standard-class fixture reports WOW_STANDARD_CLASS errors=0 resolved=9
original_table=1 and WOW_STANDARD_STORAGE errors=0 classes=9 precedence=1
retirement=1. The entrypoint is verify-wow-task-lifecycle.ps1 -Case
standard-class, called by build/M0-T447/S2/verify-standard-class.ps1.

The staged factory consumes original walias.c::stdClasses and original
GetClassPtr list precedence; it does not add a second class-name table or
register replacement controls. Each native control has one shared zeroed WC,
no invented Win16 task/procedure, and an explicit borrowed-native-registration
flag. Process disposal forgets that metadata, never unregistering USER's
control classes. Tests cover shared lookup, private override, app-public
preservation, live-window retirement rejection and retained storage borrowers.
This factory is intentionally not called from production lifecycle startup
until creation, guest projection and original cleanup form a complete group.
The test cannot establish control creation or guest application recovery.

Independent source review of NTW16S attachment.c confirms real
SetWindowSubclass use. That carrier can become the native top procedure above
the existing WOW gateway. Original getset.c's GWL_WNDPROC branch operates on
the logical client target; the project facade incorrectly identified that
target only while the gateway happened to remain topmost. The correction uses
the existing nonzero owned WND/CLS procedure, preserving any outer native
subclass chain. A tagged guest address without a logical owner is rejected;
ordinary native and CallProc paths remain USER operations. No hook-chain
scanner, alternate procedure table, guest change or mirror diff is added.

Recovery ladder: original getset.c retains private WND/process/zombie/CPD
policy but cannot directly consume a public HWND instead of NT4 private WND.
The existing ADAPTER-WOW-033 property-backed boundary is the smallest retained
seam for logical procedure identity; native USER continues native access and
CPD conversion. No original body is replaced or moved. The owning production
creation path already installs its gateway; this change neither installs it
for standard native controls nor claims to repair an unrelated native client
that deliberately replaces and discards the entire gateway chain.

Background native-subclass-review, PID20432, started2026-10-14 16:35:36 and
terminated with suite.exit=0. Driver verify-native-callproc.ps1 rebuilt the
provider and ran native-dispatch-gate, window-procedure and window-borrow.
Actual comctl32 SetWindowSubclass surrounds the gateway in the fixture:
WOW_NATIVE_SUBCLASS errors=0 forwarded=3 preserved=1 missing_owner_rejected=1;
WOW_WINDOW_PROCEDURE errors=0 tagged_calls=3; actual A/W CallProc restoration
also passes. The real CCPU fixture retains normal/exception borrow release,
both original class-registration task cases and PAGE_DOMAIN_LIFECYCLE_OK.
The procedure fixture mocks task scopes explicitly; it is not guest execution
or a substitute for immutable WINMINE/SOL/WRITE acceptance.

Second-role review distinguishes a metadata-only attachment (including the
current dialog path) from a procedure owner. Added assertions require native
get/set/restore to remain usable with a zero-target WW attachment. Conversely,
a gateway with attached metadata but no target must fail deterministically
with ERROR_INVALID_DATA, not report a stale success/error. Missing metadata
retains ERROR_NOT_FOUND. Background native-subclass-r2, PID22028, started
2026-10-14 16:39:13 to verify these additions with the same driver and cases;
its terminal result is not assumed here.

### Standard-control group: remaining source-defined initialization closure

The selected cleanup.c::_WOWCleanup initializes its original spfnwp table
once, on first call, from gpsi client procedures and four server-procedure
bindings. It later restores a departing module's subclassed standard class
by matching atomSysClass. Current lifecycle initialization fills only the A/W
DefWindowProc entries: the standard atoms and remaining procedure entries are
absent. Therefore importing nine WC records alone is insufficient and can
leave original cleanup restoring the wrong fallback or a missing procedure.
Before production enrollment, initialize the complete applicable original
restore view and account for CallProc normalization and the source's special
Edit-to-DefWindowProc restoration. Do not replace cleanup's selection algorithm
or fabricate task/module identities. Native borrowed class disposal remains
distinct from explicit guest unregister and module cleanup.

Original Edit dependency is present at creation, not only an optional
EM_SETHANDLE extension: edecrare.c::ECNcCreate assigns ped->hInstance from
pwnd->hModule, obtains expected Windows version and allocates ped->hText via
LOCALALLOC before returning WM_NCCREATE success. editml.c::EM_GETHANDLE locks
and terminates that same buffer; EM_SETHANDLE reaches MLSetHandle. Thus a
native modern EDIT window does not by itself demonstrate the original guest
local-heap identity contract. The six retained input callbacks, word-break
callback and edit-private storage must be classified together before selecting
the smallest original-client composition. No Edit rewrite or wholesale USER
import is admitted or implemented by this observation. Application testing
waits for this connected dependency group's implementation, rather than using
successive OOM dialogs to choose the next patch.

The original build package is client/sources.inc's seven edit units:
edecrare.c, editec.c, editml.c, editsl.c, edmlrare.c, edmlonce.c and
edslrare.c. editec.c::EditWndProcWorker requires ValidateHwnd, the original
FNID/size checks, EDITWND.ped/lookaside ownership, ANSI creator state and
CsSendMessage for A/W mismatch. These are concrete unavailable private-USER
dependencies of direct whole-unit composition, not a reason to rewrite the
edit algorithm. client/usercli.h::CALLWORDBREAKPROC selects the already
supplied W32EditNextWord callback for a WOW-tagged procedure. Retain that
decision rather than inventing a second word-break bridge.

The six original wcall32.c W32Local functions already distinguish native
instances (LOWORD(hInstance)==0) from guest instances and retain the packed
handle/DS identity. They are existing reusable algorithms, not missing
implementations. W32LocalLock's VDMPTR return and W32LocalReAlloc's
ParamMapUpdateNode add pointer-lease/reentry obligations to the composition
audit. Registering these callbacks alone cannot make native USER32 consume
them. Sources are research-only so far; no new original file is imported:

- client/sources.inc SHA256:e117cdc822944a8ef336065bd0c18a2636b668a445d2e5f632e484e0e3af5237
- client/usercli.h SHA256:11edc7c5b3f32ce40114bd0c4026e43381f2b131f036506376e23c8eb1db5bba
- client/edmlrare.c SHA256:fb9dd31919b7b90db2f713daa83684f1231558e7653ae72cd72458f1212000f1
- client/client.c SHA256:787eda33785a87ce9e71202c97b5913e2857f7106fed5bb97103049b1d5e3472

Final native-subclass-r2 terminated with suite.exit=0 and no remaining child
process. Full provider build, native-dispatch-gate, window-procedure and real
CCPU window-borrow pass. The added metadata-only native get/set/restore and
invalid gateway-target assertions are included in the window-procedure
errors=0 result. Original class-registration CCPU cases remain errors=0 and
normal/exception window borrows release. git diff --check exits0 (existing
LF/CRLF warnings only). This accepts the bounded logical-procedure repair,
not standard-control integration, application acceptance or S2 delivery.

### Standard-class original cleanup restoration binding

The admitted cleanup prerequisite now initializes the original atom-indexed
restore view for the same nine prepared controls. The WALIAS-to-ICLS mapping
contains ordinal translation only; class names/classification remain in the
original stdClasses/GetStdClassNumber owner. _WOWCleanup itself is unchanged,
including its module-selector decision and Edit-to-DefWindowProcW exception.
The four unprepared desktop/menu/switch/icon-title families remain unclaimed.
This initialization must run before _WOWCleanup's first call, because the
retained original caches its restore procedures on that first invocation.

Modern USER can supply a CPD rather than a directly callable standard
procedure. The existing owned class carrier now retains the original native
identity independently of subsequent subclass replacements. Its cleanup
restore entry is a bounded CallWindowProcA forwarding target; logical class
reads and new-window snapshots resolve that entry to the original identity.
It borrows the existing window/class storage across the synchronous call and
releases in finally. No native/CPD pointer is added to guest projection.
Native classes remain registered by USER, not by this adapter.

Recovery ladder: original cleanup.c is already selected and retains all
restore selection and mutation policy. Original private client/server entry
addresses cannot be called as modern public CPD handles; the smallest
same-owner storage/CallWindowProc boundary supplies that missing capability.
No executable original change or new mirror declaration is needed. No new
cleanup policy, global process table, guest patch or duplicate control name
table is introduced. A shared forwarding target is resolved using the
retained class storage, not a new dynamic thunk per control.

Initial background standard-cleanup, PID14844, began2026-10-14 16:46:59;
the25-action provider build and standard-class fixture passed, suite.exit=0.
WOW_STANDARD_CLEANUP reports errors=0/restored=9/original_policy=1; existing
descriptor/storage/retirement markers also pass. Tests invoke the selected
original _WOWCleanup, not a mock cleanup algorithm.

Independent review identified that original cleanup matches by atom even for
an application-private class shadowing a standard name. Accordingly ordinary
registration captures the native standard identity before registering such
a shadow, using the same original classifier/native descriptor; unknown
application names have no invented standard default. The strengthened fixture
registers a private BUTTON, subclasses all nine public records, unloads the
selector through original cleanup, and checks both public and private restored
identities. Native CPD replacement before the guest subclass must not overwrite
the immutable original identity. It also creates a real checkbox using a
restored target snapshot: state set/get dispatch and the native forwarding
entry remain correct after another class-target replacement, without guest
callback entry; terminal destruction releases the class association. This
post-creation fixture enrollment does not prove production CBT enrollment.

Background standard-cleanup-r2, PID3840, started2026-10-14 16:50:09 with
build/M0-T447/S2/verify-standard-cleanup.ps1. It rebuilds the full provider,
then runs standard-class, window-procedure, class-query and real CCPU
window-borrow via the tracked verify-wow-task-lifecycle.ps1 entrypoint,
followed by documentation governance. At initial observation, standard-class
passes including WOW_STANDARD_PRIVATE_CLEANUP errors=0; final suite outcome
remains subject to terminal observation. The factory/restore-table pair is
still not production-startup wired; the entire standard-control group remains
open until creation, projection and disposal are connected together.

### Next creation connection: preserve original lazy WW classification

Original wuwind.c calls FindClass16 and constructs explicit-create WW before
passing the three words to pfnCsCreateWindowEx. Original createw.c copies those
words only when lpWOW is non-NULL. dlgbegin.c creates template children through
NtUserCreateWindowEx with lpWOW=NULL, and retained walias.c::FindPWW classifies
their initially zero words lazily. Therefore internal-child enrollment must
provide zero WW storage, style/instance and shared CLS before the first guest
reader, but must not copy/rewrite FindPWW's classification algorithm. This
removes a proposed adapter-owned iClass initialization from the design.

dlgbegin.c calls WOWDlgInit after root creation and before template children;
the comment about messages does not mean before native root WM_NCCREATE.
Its CD_GLOBALEDIT path additionally calls GetEditDS once, shares that dialog
data segment across selected Edit children, and releases it with the dialog.
This is a concrete dependency beyond merely calling modern CreateDialog.
Audit that original client helper and matching release before claiming full
dialog/Edit composition. Do not infer that the existing WM_CREATE return
hook is wrong solely from W32InitDlg's abbreviated comment, nor implement
new guest DS allocation outside the retained original callback owner.

Terminal standard-cleanup-r2 result is suite.exit=0 with no remaining runner.
All four fixture cases pass. The standard-class test covers nine restores,
the private same-atom class and real native checkbox calls; window-procedure
retains its three tagged calls, native subclass chain and A/W identity checks;
class-query retains six queries and registration rollback/task markers; real
CCPU retains both original-registration cases and normal/exception release.
Documentation governance passed and diff whitespace checks pass. No existing
application success, standard-control production enrollment or release change
is inferred. This group adds zero mirror lines and zero guest changes.

The next dialog dependency's release owner is now traced too:
client/client.c::GetEditDS calls the registered pfn16GlobalAlloc with
GHND|GMEM_SHARE and256 bytes; ReleaseEditDS calls pfn16GlobalFree(LOWORD(h)).
client/dlgmgr.c's WM_NCDESTROY/WM_FINALDESTROY cleanup releases PDLG.hData when
DS_LOCALEDIT is absent and clears it. These small original helpers are
composable through the existing registered callbacks; a new allocator/free
implementation is not justified. Their full consumer still requires the
original dialog ownership and failure edges, so no unused helper import is
made at this stage.

### Shared native/application CLS projection: verified prerequisite

Background run native-class-projection used
build/M0-T447/S2/verify-class-commit.ps1 -OutputName native-class-projection,
PID604, started2026-10-14 16:59:29. Terminal observation confirms the PID is
absent, suite.exit=0 and documentation governance passed. The worker/provider
build result is0. Tracked verify-wow-task-lifecycle.ps1 cases class-query,
class-client and window-borrow all pass. The real CCPU fixture reports
WOW_NATIVE_CLASS_CCPU errors=0 projected=9 native_pointers=0 released=1,
alongside both original-registration cases and normal/exception window-borrow
release. These are host-driven CCPU assertions, not guest-app acceptance.

Independent code review: completed application registration and native classes
now share wow_class_words_publish_guest; the duplicated ANSI-name/projection
body is removed from the class client. Original _RegisterClassEx still commits
WOW words/task identity before projection, and failed projection retains its
native unregister/menu rollback. The helper copies the captured class name,
rejects duplicate publication without allocating, suppresses native/CPD
procedure values, and shares existing retirement. The fixture checks all nine
standard records, original name rebasing, shared lookup from another module,
duplicate rejection without allocator growth, and return to initial free
bytes. Encoded application WOW procedure preservation is asserted separately.
No additional mirror or guest diff. This proves initial publication only;
mutable class-field coherence and production factory wiring remain open.

### Production connection review: initialization is not sufficient

Current production wow_user_task_lifecycle_initialize initializes empty class
lists and only the DefWindowProc cleanup targets. Its dispose clears the owner
after thread retirement but does not yet call borrowed-class retirement.
The staged prepare, projection and standard cleanup helpers therefore must
enter and leave through this process owner together, not via lazy creation
fallback. Original wuwind.c invokes FindClass16 before the creation adapter;
preparing classes inside create_bound would be too late for that original
consumer. Projection additionally requires the active page domain; its
startup order must be established before selecting the initialization call.

Original kernel/createw.c establishes pcls and its reference at179/192, copies
optional lpWOW at232, and selects the procedure at307 before HCBT_CREATEWND
at394. Current creation_hook attaches metadata only for the one requested
window. Unlike application classes registered with the gateway, native
standard classes keep their native procedure and would bypass both logical
dispatch and the gateway's WM_NCDESTROY retirement. Merely publishing their
CLS or enabling the factory would leave this lifecycle leak uncorrected.
The gateway installation, own-veto/downstream-veto unwind and terminal release
must be reviewed and connected with factory startup; preserve native subclass
chains and original DispatchClientMessage rather than add a new dispatcher.

Current dialog_create_hook enrolls only its first non-child #32770 root,
without a class association. The same source review must cover template
children and internally created ComboBox children/popups, not just explicit
guest CreateWindow. Internal children retain zero initial WW, as original
dlgbegin.c's NULL lpWOW and walias.c::FindPWW require. Their ownership must
follow the active creation/root relation, not every native window on a thread.
No production startup-only switch is made while these paired edges are open.

### Production startup/teardown ordering traced after projection verification

The production forward chain is now explicit: nt_bop.c::MS_bop_1 enters
the worker page domain before loading WOW32 and calling W32Init. Original
wow32.c registers the input handlers at559 and invokes
WK32InitializeHungAppSupport at783. That original helper calls the registered
hung-handler entry synchronously; its name does not mean registration itself
runs on the eventual notification thread. registered_hung_handlers calls
ensure_lifecycle before registering the callback. UserRegisterWowHandlers
already rejects an absent guest shared view/client desktop. Thus the process
class setup can precede original FindClass16 without deferring to CreateWindow,
provided it retains the publisher's current-session/CR0/CR3 checks. No new
guest-entry assumption or thread-local allocator is needed.

The reverse edge is not symmetric today. Repository source search finds no
production caller of wow_user_task_lifecycle_dispose; runtime_unbind invokes
only retire_thread. mvdm_standalone_worker_cleanup ends guest memory before
wow_user_session_detach, then unbinds the session thread and disposes the
session. Adding a class-page release to the existing late detach would
therefore be too late to assume usable guest memory. A generic session
teardown registration is not by itself proof of correct ordering either.
More precisely, guest_memory_end closes the session lease interface; it does
not itself free all CCPU memory or prove a later class-page free fails.
session_dispose_with_reason requires zero thread bindings and invokes its
teardowns in reverse registration order. Class retirement has its own
current-session/page-domain prerequisites, so a post-unbind teardown cannot
be substituted for a still-bound release without auditing those conditions.
The process-owner release must occur while its required CPU/page/session
context is valid and after guest-capable task/window cleanup; failure must
not be silently discarded as the existing detach result is. This is a
source-established lifetime gap, not a measured leak or a claim that every
worker exit currently fails.

Next connection design must specify this paired release boundary together
with normal initialization, failure rollback, live-window refusal and final
native unregister policy. Do not import whole USER initialization/teardown or
invent a second scheduler to supply it. The six-stage replanning discussed
with the owner is still a proposal: current S2 and all its obligations remain
unchanged until explicit replanning disposition.

Review governance ran in hidden PID724, started2026-10-14 17:06:41; terminal
observation finds no process and the stdout pass marker, with empty stderr.
Whitespace diff check exits0 (existing CRLF conversion warnings only).

Second-role teardown review narrows the preceding conclusion: page_domain_dispose
already drains class_allocations, guest windows and menus as the worker's
exceptional backstop. Original nt_mem.c::VdmFreeVirtualMemory operates on its
allocation headers and does not require the guest CR0/CR3 checks imposed by
the ordinary class-publication API. Therefore an absent lifecycle-dispose
caller is not proof that guest class backing survives process teardown, and
must not motivate a duplicate final allocator sweep. The remaining normal
owner-release gap is the provider's metadata/associations and ordered,
fallible retirement while execution is still valid. Retain the existing
backstop and original allocator; distinguish ordinary object retirement from
whole-worker reclamation in implementation and tests.

### Menu connection review: compile presence is not production coverage

Source review after the cross-workstream draft confirms
wow_menu_native_bindings.c is selected by New-T404S5Wow32ProviderNinja.ps1,
but its redirect header is not included at original wow32/precomp.h or the
original menu parser compilation boundary. The only production header include
found is the owned window gateway, which calls refresh_window without enabling
the redirects. Original wumenu.c mutation calls and menu-parser creation thus
still reach raw native operations. Existing direct wrapper fixture success
does not prove those original callers are connected.

Do not solve this by enabling macros alone. MENU_MUTATION currently calls
native USER first, then returns the failure sentinel when menu_refresh fails.
At that point native state may already have changed while guest projection
still describes the earlier graph. WU32ModifyMenu at1342 retains a concrete
failure consumer: when the call fails with MF_POPUP and the supplied submenu
is invalid, it queries GetSubMenu and retries. This is original compatibility
logic to retain, not code to delete or emulate. An adapter publication error
must not be confused with an original native-validation failure; the branch
is evidence why simply overriding success is not a sufficient connection.
It does not prove this particular fallback always executes on allocation
failure (its invalid-submenu predicate remains required).

The data-layer distinction is explicit: publish_menu builds replacement ITEM
storage before replacing one MENU pointer, but publish_tree recursively
publishes children before the parent. A single-menu failure-preservation
assertion therefore does not establish whole-tree/native mutation coherence.
The next menu repair must cover original parser creation, native mutation,
callback-time visibility, failed publication and recursive destruction as
one reviewed group. Keep original wumenu conversion/fallback and parser
bodies, use the existing single object/page owner, and remove displaced
adapter paths in the same connection. This finding adds no mirror/guest
change and is not new application acceptance.

## Post-P8 callback acceptance source review

Baseline c44d33939 is delivered. No new production failure is inferred from
the remaining checklist. Review first rejects two tempting extra changes:
public SetClassLong already routes through wow_native_SetClassLongA and its
return graph update writes CLS style/window-extra; absence of publication in
the private getter is not proof that those native mutations are disconnected.
Synchronous W32EnumWindowFunc is called by its original enumeration thunk on
the initiating thread and inherits that execution context. It does not need
another scheduler simply because it calls CallBack16 outside the WND wrapper.
Private class WOW words/procedure direct-reader completeness and exceptional
callback failure contracts still require their existing broader audit; the
above observations do not certify them.

W32TimerFunc likewise needs path-sensitive reasoning. Explicit guest
DispatchMessage is selected through the target-aware native wrapper;
dialog creation passes NULL to native_call_begin, retaining execution while
dropping the data lock. TrackPopupMenu supplies its owner HWND. These facts
do not prove all modal timer callbacks lack execution ownership, nor do they
prove complete modal coverage. Do not add a timer wrapper on that assumption.
Original wutmr.c still owns timer slots, killed-timer lookup and callback
conversion; no replacement timer registry or mirror edit is proposed.

The next concrete S2 gap is evidence at the real guest entrance. Existing
hook fixtures use controlled original-call mocks and native creation fixtures
use real CCPU storage, but neither executes a Win16-created WH_CBT hook through
all original conversion and guest callback frames. An independently authored
disposable NE probe is admitted by the existing S2 real-guest acceptance
contract; it is not a replacement, rebuild or modification of USER/KERNEL.

Source-shaped probe boundary:

- user.def ordinals: InitApp5, CreateWindow41, DestroyWindow53,
  GetClassName58, SetWindowsHookEx291, UnhookWindowsHookEx292,
  CallNextHookEx293. kernel.def retains InitTask91, GetCurrentTask36,
  MakeProcInstance51, FreeProcInstance52 and original file-output APIs.
- wowusr.h SETWINDOWSHOOKEX16 stores f4 as HTASK16, f3 module, f2 far
  callback and f1 hook kind. WU32SetWindowsHookEx resolves THREADID32(f4),
  uses the original native stub/state table and returns MAKEHHOOK(index).
  HHOOK16 is DWORD, not WORD. Use the current task, never a global hook.
- Original ldaux.asm IMakeProcInstance emits mov ax,hInstance followed by
  a far jump. A probe callback must establish its data segment from AX,
  preserve callee state and obey the far Pascal callback stack contract;
  no guessed native pointer or host callback may bypass this entrance.
- Hook creation must query the new window from inside HCBT_CREATEWND,
  then recursively create/destroy one second window. A separate case vetoes
  creation and a final ordinary creation must still succeed. Chain unrelated
  hook codes through original CallNextHookEx. Unhook/free the instance thunk
  and destroy every accepted window before writing a completion record.
- Preserve explicit guest phase/result counters and expected assertions in
  the host runner. Check callback query data, nesting, veto, recovery, normal
  launcher completion and package/guest identities. A missing report, forced
  observer cleanup or only a visible window cannot pass. Use a fresh physical
  stage under O:/tmp and build-owned output, not the older directory probe's
  SUBST runner. No computer-use or original guest changes.

This is acceptance of P7/P8's connected group, not a speculative runtime
repair or an SOL diagnosis. If it fails, distinguish probe ABI correctness
from product conversion/entry failure by source before changing production.
Native same-slot handle reuse remains a separate open S2 acceptance item.
Source hashes read this pass: wuhook.c
A4F31EE8CC2258FB177471647066238F7BB406B2466DCA0249F4D6E6A98022B2;
wutmr.c E42BC0C7E9DC199AE19414461D65ECF86B7ACC55C6D7523792812D73F158BEF5;
kernel31/ldaux.asm
AAE64AC7B0DB29F348E43D04B220FDCB0324A7EAF66AE9BE316D4974F2037360.
No fresh application launch, runtime edit or new acceptance pass in this review.
