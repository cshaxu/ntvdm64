# S14 WOW resource and callback ownership review

Research-only checkpoint against `732f4a23fcfa53aca579dbb965d1392d6c26ecc4`.
Question: which actual implementations behind the C-to-F edges can move to B,
and which contain original private state or product lifecycle decisions?
This record does not authorize a mirror increase or close dependency IDs.

## Inputs and method

Read the complete implementation bodies below in the current working tree;
follow named callers using `rg` over `src` and `test`. Inspect the associated
headers, `common/guest-memory/access.h`, `compat/include/guest_memory_access.h`,
`non-mvdm/windows/core/ntuser/inc/user.h`, and the abnormal-return portion of
`mvdm-host/wow32/wcall16.c`. Paths in the table are relative to
`src/wow32-dll/source`. This is source evidence, not a fresh compile, link or
runtime test. Earlier selected-graph inventory remains separately recorded.

## Implementation dispositions

| File | Observed responsibility and state | Proposed ownership and necessary follow-up |
| --- | --- | --- |
| `wow_bitmap_bindings.c` | TLS conversion context, serialized scratch/DC lifetime, screen capabilities; `wow_bitmap_load` calls original `WOWLoadBitmapA`. | Move independent scope/DC/lock mechanics to B. Keep original-call orchestration separate so B does not call C. Split C `newres.h`, original declarations and alias macros from the resource carrier. Preserve recursive calls and failure cleanup. |
| `wow_button_resources.c` | Caller-owned GDI atlas; create into a temporary resource object, publish only after rendering succeeds; restore selected object and release on failure. | GDI mechanics are a B candidate, but current metadata uses C `OEMBITMAPINFO` and `OBI_COUNT`. Resolve this representation boundary before relocation; moving its current header wholesale would introduce B-to-C dependency. Do not duplicate a button state engine. |
| `wow_button_lifetime.c` | TLS invocation stack, private `PBUTNWND`/`PLOOKASIDE`, deferred release at the oldest same-control invocation, native-window dispatch wrapper. | Private control-state lifetime belongs with C algorithms; native entry composition stays outer. Only independent resource leaves belong B. Exact original carrier and mirror cost remain to be established before editing C. |
| `wow_callback_frame_lease.c` | CPU continuation capture/restore; protected-address conversion; bounded copied-memory leases; worker-active check and fatal-selector boundary. | CPU and selector operations belong C, generic lease mechanics B, worker cancellation/terminal policy outer. Do not move the whole file to B or conceal active/fatal checks behind new callbacks. The D access header is only a forwarding include to B, but removing that include detour alone does not solve implementation ownership. |
| `wow_class_client_bindings.c` | Scoped class context; original registration; native class publication and procedure normalization; completed guest-class projection and rollback; optional CPU-position diagnostic. | Separate native resource publication from original class lists, task/procedure representation and guest projection. Original structures and algorithms remain C; product entry composition remains outer. Do not classify the whole implementation as independent B merely because it calls `RegisterClass`-related APIs. Trace class-word and window-creation providers before choosing the exact split. |
| `wow_cleanup_bindings.c` | Native window/procedure verification plus temporary reconstruction of private USER child/sibling lists; invokes `_WOWCleanup`, then restores root and clears links in `finally`. | Private cleanup projection and original invocation belong C; independent native operations may be B. Preserve no-application-callout procedure replacement and retained object identity. Native handle checks alone cannot replace original cleanup. Full object-retirement chain remains to audit. |
| `wow_user_message_bridge.c` | Native queue initialization, owner-thread checks, sent-message-only peeks, private queue wake-bit update and native-call release/reacquire hooks. | C owns original queue/task ordering; B can own independent native queue operations only if their reentrant call boundary is retained outside B. Preserve PM_NOREMOVE/PM_QS_SENDMESSAGE and the wait-specific PM_NOYIELD contract; do not introduce a second message loop. Native-call providers remain to audit. |

## Specific ordering evidence

- `wcall16.c` invokes `wow_callback_control_restore` only during abnormal
  unwinding. Its return value gates restoration of `ptd->vpStack`,
  `vpCBStack` and callback flags. The current provider checks whether the
  worker is still active before touching CPU state, reloads CS/SS against
  current descriptors, and does not restore CR3 or cached descriptors.
  A relocation must preserve both machine restoration and terminal-abandonment
  semantics; blindly copying CPU setters into B is not a solution.
- `wow_class_client_publish` calls `_RegisterClassEx` before
  `publish_completed_class`: the original algorithm supplies WOW words and
  task association after initial native publication. Projecting the initial
  allocation instead would publish incomplete state. Failure destroys the
  newly published native class through the existing class-word owner.
- `wow_user_message_bridge` wraps native peeks because they may invoke client
  code synchronously. Its ordinary return and scheduler-wait return use
  distinct existing end operations. These must be traced, not replaced by a
  plain public API call on the basis of matching function names.
- The button atlas header reaches `OEMBITMAPINFO` and OBI constants through
  `opennt_user_multipass.h`, which includes original C `user.h`. Defining
  `WOW_ORIGINAL_BUTTON_RESOURCES` in the wrapper is not proof of an independent
  B declaration closure.
- Source search finds `wow_bitmap_bindings_shutdown` calls in tests, but no
  production call in `src`; the production definition and declaration remain.
  Its current terminal-only INIT_ONCE/lock lifecycle must not be described as
  verified production teardown. Reconcile final link and unload ownership
  before deciding whether to wire, retain or remove it.

## Result and remaining evidence

Seven complete bodies classified; no implementation moved and no dependency
record closed. The historical direct ledger remains 150 eliminated / 418 open
of 618 total records. Mirror changes: zero points, zero lines.

This review gives concrete ownership constraints, not completed migration
designs for every chain. Next inspect class-word/window-creation, native-call
entry/return and USER retirement providers, then reconcile destination files,
source provenance, mirror cost and final-code validation. Full four-component
inventory and caller/lifetime verification remain unfinished. No guest source
or binary was changed; no tests or release package were rerun for this review.

## Runtime binding and retirement follow-up

Against P32 `be33fdff8`, read both complete NTVDM implementations
`src/ntvdm-exe/wow/wow_user_runtime.c` and `wow_user_session_binding.c`,
their headers and `src/common/wow/user_runtime.h`. Also read the selected
native-call, callback, publication, initialization and retirement functions
in `wow_user_task_lifecycle.c`; this is not a complete read of that large file.

The concrete C0618 path is original `taskman.c`/`queue.c` current-thread
access, expanded through the existing task-order bindings to
`wow_user_runtime_current`, resolved to the NTVDM provider. The ledger's
COFF/map evidence matters here: absence of the expanded symbol spelling in
the original C source is not absence of the dependency.

The runtime has independent SRW-lock/binding mechanics, but its unbind
operation invokes `retire_thread` before clearing TLS. Initialization in
WOW32 installs `retire_current_thread` and `dispose_process`. Retirement
performs original USER cleanup, refuses live owned windows, removes native
hooks and input associations, calls `DestroyTask`, and frees task resources.
These bodies cannot be treated as B-owned generic resource callbacks.

The migration design is to separate fallible retirement from the B binding
primitive. The composing outer boundary must validate the binding, perform
retirement while it is still current, and only then clear the B binding.
Failure retains the binding and provider state. Process disposal remains
after all thread retirement and before removal of session hooks. Production
unbind calls occur in `wow_user_session_binding.c`; direct test callers must
be divided into primitive tests and complete retirement tests, not weakened
to accept premature unbind. Reuse the existing binding; do not introduce a
second TLS current pointer. Final representation of C task/class references
and the destination of their algorithms remain part of the complete design.

The native-call return path reacquires the data lock, marks the suspended
scope ready, reacquires original execution ownership through
`xxxDirectedYield` if needed, pops the scope, then publishes native window/menu
results before guest continuation. A scheduler wait without execution
ownership defers publication until ownership is acquired. Callback entry
likewise uses original `xxxSleepTask`/`xxxDirectedYield`; these are C scheduler
semantics, not a generic B mutex operation.

Existing tests supply relevant contracts, not fresh acceptance:
`test/adapter-mvdm-host-out/wow/wow_user_runtime_fixture.c` checks lock
release/reacquire and rejects unbind while held. The lifecycle fixture in the
same directory checks that a live owned window makes unbind fail with
ERROR_BUSY while retaining the exact task and binding. Keep these negative
requirements after splitting the entrypoints. No production change, test run,
new exception, or dependency elimination follows from this source review.

## Complete lifecycle body review

Against P33 `f449bde29`, finish reading all of
`src/wow32-dll/source/wow_user_task_lifecycle.c`, including its generated
native wrapper definitions, initialization rollback, message loop, launch
notification and terminal-window destruction. Together with the preceding
reviews this covers ten complete implementation bodies (the original seven,
two NTVDM runtime/binding files, and this lifecycle file). It does not mean
all transitively called providers have been audited.

| Responsibility | Concrete implementation/state | Required disposition |
| --- | --- | --- |
| Product start notification | `notify_start`, `launch_request`, `start_pending`, `start_attempt` | Keep product request identity and RPC transaction outside B/C. Preserve the current call-frame attempt identity across reentrant notification, without retaining a task pointer across RPC. The exact carrier change must not create duplicate task identity/state. |
| Original task execution | `xxxInitTask`, `xxxSleepTask`, `xxxUserYield`, `xxxDirectedYield`, `DestroyTask`; private TDB/process/queue fields | C owns these algorithms and their original state. Host readiness and callback adapters must be traced to finite original owner seams, not moved as an arbitrary 1,800-line C insertion. |
| Data lock and thread binding | `wow_user_runtime_enter/leave/current/set_context` | Independent mechanics are B candidates; separate retirement as described above, and preserve one authoritative binding. |
| Native USER call wrappers | HWND conversion, result roots, native call begin/end, publication, destruction without publication | Mixed C representation/execution contract and native USER operation. They are not ready for whole-file B migration. Keep the single shared handoff implementation; adding one callback bridge per wrapper would preserve the outward dependency. |
| Task/module resource cleanup | `wow_user_task_lifecycle_exit`, original `_WOWCleanup` | Resource cleanup does not retire THREADINFO. Task-zero module cleanup must not destroy the task's native windows. |
| Native-thread retirement | `retire_task`, terminal window destruction, hook/input withdrawal, `DestroyTask` | Preserve destruction while guest resource callbacks are still usable, followed by original cleanup and final thread-carrier release. Retirement refusal retains ownership; product thread completion remains outer. |

### Start-chain evidence

`mvdm-host/wow32/wkman.c::W32Thread` retries `pfnInitTask` up to its existing
bounded loop before destroying a failed task. The selected
`wow_user_registration_bridge.c::registered_init_task` resumes an already
initialized task's pending view publication and notification instead of
calling `xxxInitTask` again. Its notification callback is
`common/base_srv/base_rpc_client_launch.c::OpenNtBaseClientWowStarted`, which
uses the authenticated execution connection and `Client_WowStarted` RPC.
This is concrete product-service knowledge, not an independent compatibility
operation or a guessed meaning of the callback name.

### Wait, callback and teardown evidence

`wait_for_task_or_message` uses native queue/event waits, then updates original
`ptdb->nEvents` and shared `nEvents` under the existing runtime lock. It
distinguishes native sent-message dispatch from guest execution ownership;
an input-plan transaction defers receiving until original InitTask releases
that plan. `client_context` exchanges debugger task identity and active WOW
page-domain client state. Moving only the native wait into B cannot transfer
these C responsibilities.

The native wrappers deliberately distinguish ordinary return publication
from DestroyWindow's no-publication return. Terminal cleanup first marks and
prepares owned windows under the lock, retains each window's storage, drops
the lock for native destruction/reentrant calls, and then reconciles surviving
relations and retired classes. Replacing this with generic resource freeing
would lose the required callback lifetime.

### Verification requirements for the eventual split

Preserve existing runtime/lifecycle fixtures and add assertions at changed
boundaries only: failed start notification does not rerun InitTask; module
cleanup leaves the task bound; retirement failure retains exact owner state;
native callouts do not hold the data lock; callback return reacquires the
original execution owner; destroyed HWNDs are not republished. Reuse existing
Win16 lifecycle/dialog acceptance for final code. No test was run in this
research checkpoint, and no mirror-increase candidate is approved by it.

## Class, creation and common-dialog follow-up

Against P34 `e53451ba8`, read five more complete WOW32 implementation files:
`wow_class_words_binding.c` (567 lines), `wow_window_creation_binding.c`
(836 lines), `wow_common_dialog_callback.c`, `wow_common_dialog_calls.c` and
`wow_clipboard_epoch.c`. This record now covers 15 complete bodies; it does
not count that as 15 migrated files or a complete transitive closure.

| Implementation | Evidence-based disposition |
| --- | --- |
| `wow_native_class_resolve_procedure` | Independent B candidate: only native class registration/query/unregistration, scalar procedure input/output and native error preservation. It creates no HWND and reads no session, C layout or guest state. Preserve failure to unregister as failure, and keep the non-FFFF fast path. Existing native-creation fixture has real token and procedure assertions; final relocated code still needs that test. |
| Remaining class-word storage | Mixed ownership, not a wholesale B candidate. A binding embeds B `wow_class_lookup_entry` plus original C `CLS` atom projection, selects original class lists with `GetClassPtr`, dispatches recovered control procedures and publishes/retires guest CLS through page-domain operations. Split native resource/storage mechanics from original layout and guest publication without adding B-to-C callbacks. |
| Window creation | Shared scope supports explicit original creators and native implicit creation. It composes class selection, WW metadata, task execution/borrow scopes, CBT publication, native creation, menus and rollback. Native hook mechanics alone do not describe the function's behavior; keep original conversion/policy at C and independent native mechanics at B. Resolve original owner placement and mirror costs before migration. |
| Common-dialog calls | Seven native COMMDLG operations already share one wrapper; all use the existing execution handoff. Moving that wrapper to B with calls back into the F lifecycle would not remove the dependency. Modeless Find/Replace use no-publication return, unlike modal operations. Preserve that difference while separating ownership. |
| Common-dialog callbacks | Original dialog callbacks are wrapped by task execution and copied-view borrow scopes. Find/Replace binds resource destruction at WM_INITDIALOG and consumes it at WM_DESTROY before original guest resource release. This is a C callback/lifetime boundary plus outer entry, not a generic B event callback. |
| Clipboard epoch | Native sequence/owner query is independent; the operation then calls original `W32EmptyClipboard`. Keep the invalidation decision with its C caller and use native query directly or an independent B leaf. Moving the whole operation to B would add a C callback dependency. No decision to change clipboard behavior follows from this classification. |

Class storage has distinct `references` (registration/borrowed storage) and
`cWndReferenceCount` (window associations). Original cleanup decisions use the
latter; do not collapse them into one count. Borrowed native standard-class
metadata is forgotten without unregistering USER's class. Owned application
classes are unregistered first; failure to release the already-unregistered
class's guest projection is terminal, not a retryable native-registration
failure. A proposed split must keep those different resource owners explicit.

The class lookup carrier header is already in B, but its original lookup
algorithm is C and its complete registration storage is still F. This is a
specific example of why header ownership is not implementation ownership.

Creation publishes ownership before downstream CBT or WNDPROC callbacks.
Rollback checks that the same binding and installed procedure still own the
window before restoring it. Class references transfer into the window binding
on attachment; remaining local references are released on failure/unwind.
The scope is popped and the hook removed before final retained-object release.
The native creator does not replace original guest menu loading: the original
resource path is used before WM_NCCREATE, without using SetMenu to introduce
premature size/move callbacks. These sequencing constraints prohibit a simple
replacement with an unwrapped CreateWindowEx call.

No production edit or test run occurred. Direct ledger and mirror-difference
counts remain unchanged. Next resolve window-word storage, guest projections
and provider cleanup against the proposed B/C split; independently movable
leaf identification is not permission to skip the complete migration ledger.

## Window storage and generation ownership

Against P35 `083c3ffe1`, complete the 1,536-line
`wow_window_words_binding.c` body review (16 complete bodies in this record).
Follow its generation binding into NTVDM's page-domain provider at
`bind_window_host_storage`, `retain_host_window`, `window_host_storage` and
`dispose_window_borrow`. These selected provider functions are not a fresh
complete audit of the page-domain file.

The storage record combines native property/refcount mechanics, original
`WW`/`WND`/`CLS` representations, USER cleanup ownership, native class
references, guest generation identity and resource-only destruction callbacks.
Its opening comment is not sufficient evidence of independent resource-only
behavior: actual final release can return retained guest backing, and
terminal retirement can invoke original control/dialog finalizers.

| Ownership domain | Current concrete behavior | Migration constraint |
| --- | --- | --- |
| Native property/storage | GetProp/SetProp/RemoveProp, publication lock, references, immutable native thread identity | Candidate B mechanism only after isolating typed original state and guest release; preserve exact-binding checks against HWND reuse. No generic callback bag that still invokes the outer provider. |
| Original state | WND allocation and promotion, FNID/private-state flags, WW words, CLS atom projection, original finalizers | C-owned representation/algorithm. Resource-only views cannot acquire control-private access merely by having a WND-shaped allocation. Promotion preserves previously borrowed addresses and graph edges. |
| Guest generation | Borrow/pin/release, guest WW updates, published graph and retained backing | C machine ownership with independent B bookkeeping only where actually separable. Preserve original SAS behavior and existing generation identity, not a new registry. |
| Product retirement | Runtime admission/cleanup scope, late callback rejection and terminal failure | Keep product policy outside B. C finalization and native deallocation need explicit ordering rather than calls back into product state from B. |

The generation owns one host-storage reference. A retired host binding can
also own a retained generation reference. `release_reciprocal_generation_owner`
clears the latter before release, outside the publication lock while other
references still keep the binding alive, to break the cycle. Conversely,
`dispose_window_borrow` unlinks the generation and clears its stored callback
before invoking `release_generation_storage`; that callback clears generation
and retired-backing fields before the final host release. Do not substitute
unconditional free or change this into two independent disposal registries.

Observed-graph publication first captures fallible native information and
names, then locks/revalidates exact properties and states, commits the guest
graph, and performs prepared local swaps. Terminal cleanup marks WFDESTROYED,
drops locks before original finalizers, rechecks whether nested cleanup already
retired the binding, and only then withdraws property/table ownership. Native
WM_NCDESTROY observation retains the old binding before forwarding other hooks;
the returned HWND alone cannot identify the generation to retire.

The native-procedure dispatch path does not acquire guest execution. Tagged
guest dispatch does acquire the task and borrow scopes, publishes callback
window/menu views, then releases the WND while those scopes are still live.
Keep that distinction; merging both paths around an unconditional CPU scope
would change native-only behavior.

Existing `test/mvdm-host/wow_page_domain_fixture.c` cases cover atomic link
publication, reused slots, retained class/window release ordering and duplicate
release rejection. They are candidate regression requirements, not a new pass.
No production change, mirror delta, test invocation or dependency closure is
claimed. The exact destination carriers and complete migration plan remain
unfinished; the next work is to reconcile these ownership domains with the
page-domain and object-table implementations.

## Scoped alias ownership

Against P36 `ba695335d`, read all 655 lines of
`src/wow32-dll/source/wow_user_borrow_scope.c`, its complete header, and
the selected thunk invocation and page-domain fixture consumers. This raises
this record to 17 complete implementation-body reviews, not 17 migrations.

The file is not a generic native resource scope. `wow_user_borrow_window_state`
resolves the retained callback generation before the current shared handle
table, preserves original short-handle uniqueness rules, and returns a private
WND only after pinning its reachable graph. `retain_window_generation` matches
both the full window and exact backing; its 512-slot index is transaction
scratch with collision fallback, not a second identity authority.

`wow_user_borrow_client_graph` transfers newly reached graph pins to the
outermost existing scope before publishing links. Suspended original code
holds those same WND addresses: popping the inner callback must not free the
new targets or restore obsolete links. Preserve this algorithm with the C
window representation and page-domain owner. Moving only the list/TLS to B
and calling F for release would retain the forbidden outward dependency.

The concrete split is: original handle/class resolution, callback-pair
restoration and typed graph projection belong to C; independent allocation
and reference mechanics may use B without callbacks into product ownership;
product entry/finally and fatal-error policy stay outside. The existing
`wow_user_invoke_thunk` enters the scope and leaves in finally without taking
over original W32Dispatch/W32TryCall exception policy. Do not create another
scope stack when changing ownership. The exact C carrier and mirror cost
still require the complete migration design before implementation.

Leave has two different failure contracts. A mismatched scope is not consumed;
a matching scope restores its callback, pops, releases pins and can then report
deferred collection failure. Retrying leave in the latter case is incorrect.
`wow_user_borrow_leave_required` enforces this distinction. The selected fixture
at `test/mvdm-host/wow_page_domain_fixture.c:2261` checks stable PWW identity,
nested borrowing and retained data across handle retirement. This is source
coverage evidence only; no test was run for this documentation delivery.

No product or mirror change and no eliminated dependency is claimed. The
object-table implementation has only been partially read and is not counted.

## Object publication and four callback consumers

Against P37 `fc28c99d5`, finish the 790-line
`src/wow32-dll/source/wow_user_object_bindings.c` and its header; read the
complete control-brush, dialog-keyboard, timer-callback and idle-hook binding
bodies. This record now covers 22 complete implementation bodies. The prior
partial object-table read is superseded, not counted twice. These are source
findings, not migrations or new runtime passes.

| Concrete functions | Actual responsibility and proposed owner |
| --- | --- |
| `wow_user_window_sample_links`, `capture_window` | Public USER property and identity sampling; candidate B with a finite observation value. No task lookup, guest mapping or original private-state inference is needed by these bodies. Preserve desktop parent versus owner and legitimate zero-valued properties. |
| `position_roots`, message/queue/return root extractors | Typed native message operands; candidate B without a provider callback. Keep posted-message and synchronous payload rules distinct, exclude menu handles and insertion constants, and do not recursively dereference arbitrary payloads. Original thunk conversion stays C. |
| `wow_user_full_handle`, native/public/owned/guest handle conversion | Original shared metadata versus cleanup ownership are different authorities. These operations require C-owned identity publication; moving the function names into a B header would not remove their page-domain dependency. |
| `project_graph_attempt`, refresh and observed publication | Mixed native capture and C generation/guest commit. Split capture from commit, with one transaction owner; do not put a guest commit callback into B. Native observations are not an atomic desktop snapshot. |
| `wow_user_window_publish`, retire, cancel-native-adoption | C typed object/generation ownership. Publish rolls back the provisional local entry when guest publication fails; retire and cancel clear the local entry only after page-domain success. Cancellation retains the observed generation and is not equivalent to destruction. |
| `wow_control_brush`, retain/color helpers | Original `xxxGetControlBrush` plus retained PWND and a native color call. C owns private state and original behavior; the bounded native color service can be B. Read native handles before reentrant USER calls, keeping both exact allocations retained. |
| `wow_dialog_messageA/W` | Original IsDialogMessage behavior wrapped in task execution, borrow and dialog context. Split the execution boundary from the original manager; retain context/borrow/task unwind ordering. Do not replace the original manager with native IsDialogMessage. |
| `W32TimerFunc` | C timer stubs call this F scope wrapper, which invokes renamed C `WowOriginalTimerFunc`. Original timer slot selection, queued callback after KillTimer and CallBack16 stay C. Scope ownership must move inward before removing this outward wrapper. |
| Idle registration and dispatch | `wow_user_idle_hook_registered` reads C TD nodes under C gcsWOW; belongs with that original state, not generic B. Native hook forwarding happens after callback scopes leave; direct taskman notification must not forward a native hook chain. |

The actual timer round trip is visible in `mvdm-host/wow32/wutmr.c`: the
27 stubs retain W32TimerFunc calls, while the original body at physical line
219 is named WowOriginalTimerFunc. `wkman.c:683` similarly retains the original
CallBack16 foreground-idle body under a renamed symbol. These are concrete
candidate C carriers, not authorization to insert an unmeasured mirror patch.
The scope implementation and complete call chain must be settled first.

`mvdm-host/wow32/wuclip.c:39` directly calls the F public HWND resolver.
Its fallback to an externally observed, committed window differs from owned
window cleanup lookup; deleting that distinction would break the public
clipboard boundary. Native capture can move independently, but eliminating
this edge requires moving the identity implementation, not just its header.

The graph collector uses temporary indexed rows, prepares local storage before
the irreversible guest publication, and retries only a detected local capture
cycle once. It does not retry commit failures or reuse an invalid old graph.
Those distinctions are migration invariants; this review does not claim that
every existing adaptation is minimal or that all original alternatives have
been proved unavailable. Destination carriers and mirror costs remain design
work. Direct dependency counts and production package identity are unchanged.

## Control state and default-window boundary

Against P39 `23035c8f6`, complete seven further bodies: control_draw_state,
control_fnid, control_state, default_message_bindings, default_native_bindings,
default_state_bindings and default_window_bindings (all under
`src/wow32-dll/source`, with `.c` suffix). This brings this record to 29 bodies.
Read the corresponding default/drawing headers, original server.c message-table
initialization, drawing precomp aliases and the selected control finalizer.
The larger edit_state_bindings.c is only partially read and is not counted.

| File | Concrete disposition |
| --- | --- |
| wow_control_draw_state.c | DC/bitmap/lock/atlas acquisition and disposal are candidate B resource mechanics, with screen bits/brushes supplied as values. Original ClientDrawState invocation and original-global binding remain a separate C boundary. The whole body cannot move B unchanged because it calls that C algorithm and reads the F edit context. |
| wow_control_fnid.c | Extracted original NtUserSetWindowFNID algorithm now exposed as wow_edit_set_window_fnid: validates PWND, reads GETFNID and mutates fnid. It belongs with C private window state, not B; original ntstubs retirement remains approved and is not silently reversed. A bounded C carrier needs an explicit mirror/provenance decision. |
| wow_control_state.c | Extracted original SetWindowState/ClearWindowState retain TestWF and private operation semantics. Preserve these original bodies while resolving their C state provider; public SetWindowLong is not an established substitute for internal WF bits. |
| wow_default_message_bindings.c | INIT_ONCE invokes C InitMessageTables, publishes FNID-indexed WNDMSG arrays, registers SHELLHOOK, and frees all partial allocations on failure. The table algorithm/state is C; native allocation and message registration may use B/public APIs. Moving the initializer B would introduce a C callback dependency. |
| wow_default_native_bindings.c | FindNCHit adaptation calls native DefWindowProcW for host-owned nonclient geometry, but first retains exact PWND and crosses the WOW execution boundary. Split those C obligations from the native call, preserving existing approved modern-host behavior. CsSendMessage admits only the explicit default-window operation and zero extra data. |
| wow_default_state_bindings.c | System color/border/display sampling is B-eligible finite data capture; initialization of original tables and current invocation storage is not. SPI_GETBORDER supplies logical border units; padded-frame pixel metrics are not interchangeable. Shell-message initialization must not call native SetTaskmanWindow. |
| wow_default_window_bindings.c | Original default processing, retained PWND, creation facts and stored-name locking remain C responsibilities. Only WM_GETTEXT/WM_GETTEXTLENGTH take the name lock, around the non-callback original branch. The WM_CTLCOLORSTATIC fallback for unavailable creator facts preserves the approved native-host boundary. |

The drawing lifetime requires special care without a second resource registry:
original scratch bitmap growth changes the owned bitmap through
wow_control_draw_select, while arbitrary application callback selections are
borrowed. Cleanup reselects the original bitmap before deleting the DC and
owned scratch; exceptional return restores the caller viewport and prior TLS
context. A B resource object can implement this without knowing Run16 or task
requests, but its interface must not call ClientDrawState or fetch F TLS.

`non-mvdm/windows/core/ntuser/kernel/server.c:676` is an existing original
InitMessageTables carrier. Its gSharedInfo mapping currently selects the F
message-state global. Resolving this means moving actual table storage and
initialization ownership, not merely relocating a declaration. The retained
original arrays and FNID indexing make wholesale substitution unnecessary.

The native default-window implementation is an approved stopping boundary,
not evidence that every omitted USER server algorithm must be reimported.
Conversely, moving an original control-private algorithm into compat would
create another B-to-C exception. The migration plan must preserve both rules.
These source conclusions identify destinations and necessary splits; they do
not approve a net mirror increase or establish final candidate equivalence.
No production edits, tests, publication or dependency elimination occurred.

## Dialog services and retirement

Against P40 `0ddef04bb`, read six complete bodies under wow32-dll/source:
wow_dialog_call_bindings.c, wow_dialog_context_bindings.c,
wow_dialog_memory.c, wow_dialog_native_bindings.c,
wow_dialog_procedure_bindings.c and wow_dialog_finalization_binding.c.
The complete-body count is now 35. Selected registration, original callback
assignments and existing B native-service headers were also checked, without
counting those partial reads as additional complete files.

| Implementation | Destination and required split |
| --- | --- |
| Native export resolver, system-menu/thread-state/reset services | Reuse B wow_native_controls.c as the native USER/GDI service owner. These leaves need no task/request/guest state. Keep availability failures explicit; do not add another service table or map original NT4 syscall numbers to modern calls. |
| Native dialog creation | The installed 17-argument service and 16-byte counted descriptor differ from the original declaration. B can own the resolved native call with already converted values. Resolve guest parent identity and translate original LARGE_STRING in C before calling B; do not let B call wow_user_window_owned_handle. |
| Context capture/release | Native work area, snap-to setting, DPI and optional font are B mechanics with caller-owned lifetime. Runtime validation and copying original class atoms belong to the C context owner. Publish output only after successful capture; font cleanup accompanies the resource, not a new lifecycle registry. |
| WOWDlgInit/GetEditDS/ReleaseEditDS | Original callback algorithms, not native memory allocation. wow32.c assigns W32InitDlg/W32GlobalAlloc16/W32GlobalFree16; the F registration bridge currently supplies that table. Reunite original registration/consumer ownership in C rather than move these callbacks into B. Preserve 256-byte GHND/GMEM_SHARE allocation and LOWORD release. |
| Procedure initialization and dispatch | Save/null/restore native DWL_DLGPROC prevents an extra application callback during native WM_NULL initialization. Procedure-token handling and native calls may use B, but execution handoff and guest procedure selection remain outside B. No replacement HWND or second dialog-procedure owner. |
| Dialog finalization and API routing | C owns original PDLG/FNID state and EndDialog/DefDlgProcWorker policy. Outer admission must distinguish ordinary callbacks from cleanup-only entry; split it from typed original finalization. Unowned native dialogs continue to native APIs, never reinterpret native PDLG as original state. |

Specific lifetime constraints from the bodies: failed original allocation must
not initialize a pool merely for teardown; WM_FINALDESTROY must reach the
original cleanup body without an application subclass; native WM_NCDESTROY
retirement happens while HWND remains valid and is not repeated by the later
private finalizer. The terminal native procedure clears the application slot
before native cleanup and consumes owned resource destruction once.

The context and procedure wrappers preserve original/outer TLS in finally.
The caller owns failure policy: existing native helpers that invoke
wow_user_native_call_failed need a finite status-return leaf before moving B,
not a B callback into F failure handling. This is a proposed split, not an
implemented or approved semantic change. In particular no silent false/zero
substitution is authorized for a failed void native operation.

Existing B wow_native_controls.h already defines the modern counted-text
descriptor and retains original-layout translation outside B. Extend that
same boundary rather than copy the F header with original C layouts. Native
service ABI comments identify historical inspection evidence; this source
review does not newly verify those exports on other Windows versions.
Mirror carriers/costs and final runtime validation remain outstanding. No
production change or dependency elimination is claimed by this delivery.

## EDIT calls, entry scopes and hook dispatch

Against P41 `2e09f3f8d`, complete wow_edit_call_bindings.c,
wow_edit_conversion_bindings.c, wow_edit_thread_bindings.c,
wow_edit_entry_bindings.c, wow_hook_callback.c and the associated
wow_control_extent_bindings.c under wow32-dll/source. Total complete bodies:
41. Original dlgmgr.c, editec.c and wowhooks.c call sites were checked; those
partial mirror reads are not included in the body count.

| Implementation | Ownership decision and preserved contract |
| --- | --- |
| EDIT native calls | Native text/style/color/fill providers already use B leaves. Their wrappers additionally retain original state, hand off execution and publish return state; do not move these wrappers wholesale B. A successful native text write followed by failed publication cannot resume with stale original text. |
| Set/ClearWindowState | WFWIN40COMPAT is C behavior state: dlgmgr.c temporarily toggles it around DefWindowProcWorker, and clmsg.c reads it. Other admitted flags use native state operations then refresh the projection. Preserve this ownership distinction; neither a universal native setter nor universal private-bit mutation is equivalent. |
| Client info and registered extent | C queue initialization owns TIF_16BIT and app-version state. Registered control sizes use exact WND/control layouts, not current HWND guesses. Keep these beside their C type/algorithm owners; no new B exception or duplicate flag writer. |
| A/W conversion | editec.c:2270-2274 admits only the opposite original EDIT procedure and FNID_CALLWINDOWPROC. Native CPD creation is B; original target selection and task/borrow entry are C execution concerns. Preserve the native class-owned opaque procedure token across window destruction. |
| Thread identity | Retain the exact original WND allocation and read its bound native thread identity. Do not fabricate THREADINFO or re-query a reused HWND. Typed retained-state access belongs with C window ownership. |
| Context capture | Display IC metrics, preferences and font measurement are candidate B input capture. Profile lookup must be resolved separately; current FastGetProfileIntW routing is not proof of B independence. Supply original class atoms from C rather than fetch F lifecycle state inside B. |
| Control entry and caret callback | One task/borrow/context boundary covers EDIT, combo, list and button entries. Cleanup avoids fresh metric acquisition; caret callbacks refresh existing state and reject cleaned-up list state rather than allocate it. Keep original control bodies and timer identities; do not expose raw original procedures without their retained execution contract. |
| Hook dispatch | Original wowhooks.c owns hook identity, conversion and guest stack. F WU32StdHookProc currently wraps the renamed C body; move required scope mechanics with C execution ownership, leaving only genuine product admission outside. Typed native payload extraction can be B with explicit hook type/code, not private HOOKSTATEDATA access. |

Hook creation has an ordering constraint beyond a simple finally pop. It
retains exact enrollment until callback completion and task-scope return both
accept creation; a failed return vetoes creation. Nested hook parameters,
current index and DDE/caller flags are restored before finishing enrollment,
then borrowed aliases retire. Replacing this with an unconditional finalizer
or identifying hooks by overlapping numeric codes would change behavior.

The control extent helper is another associated provider absent from the open
direct-symbol rows. Its original layout expressions can be evaluated by C;
the full original registration routine cannot simply replace it because that
routine would register host control classes. A C placement still needs a
measured mirror candidate, not implicit re-admission of retired mirror files.
No source migration or runtime pass is claimed. Existing dependency statuses
and mirror counts are unchanged; the crosswalk records coverage only.

## Menu lifetime and small conversion providers

Against P42 `5cc40c455`, complete eight bodies: wow_menu_native_bindings.c,
wow_menu_client_bindings.c, wow_message_lifetime.c,
wow_ole_descriptor_binding.c, wow_private_user_compat.c,
wow_public_user_facade.c, wow_shell_callback.c and
wow_sim32_pointer_compat.c (wow32-dll/source). Total: 49 complete bodies.

| Chain | Migration disposition |
| --- | --- |
| Native menu operations and client projection | Native USER owns actual menus. C must own copied MENU/ITEM generations, original task cleanup selection and identity translation. Split finite native observations from typed publication; no B callback into page-domain or task state. Preserve native operation return conventions, including TPM_RETURNCMD and minus-one results. |
| Menu capture and retirement | Capture the whole tree before guest commit, install cleanup ownership only after commit. Created-task ownership and locally refreshed observation are distinct. Publish surviving parent/window edges before retiring descendants; RemoveMenu detachment alone is not destruction. Retire copied parent views before referenced children, and validate no live owners before forgetting borrowed views. |
| Message lifetime | Captured original converter keys, PTD callback stack and original unthunkers belong C. Do not replay output conversion on failure, substitute a new heap registry, or restore a retired/reused TD. Worker-active admission currently comes from F; separate that product decision from C cleanup rather than relocate the predicate behind B. |
| OLE descriptor | ACP lead-byte initialization is native data mechanics; original ConvertObjDescriptor and GetPModeVDMPointerAssert are C algorithm/mapping paths. Whole-file B migration would retain C callbacks. Keep the original descriptor conversion and segment-fault owner, without OLE activation or new mapping. |
| Private USER resource helpers | Native icon/cursor creation and validated native menu-index lookup are B candidates; original default-message bitmap merge and menu template parsing remain C. Owner-draw template data can be a guest pointer, so public LoadMenuIndirect is not an established replacement for CreateMenuFromResource. |
| Public dialog facade | One forwarding call preserves all flags into the recovered modal manager. Reconcile this alias with the eventual C dialog entry and remove an unnecessary forwarding layer only after its callers/ABI are checked; do not restore the old native-dialog substitution. |
| Shell callback | The synchronous modern three-slot to original two-slot callback bridge retains nested TLS and delegates execution/current-directory semantics to original wshell. It invokes an original callback, so it cannot move B unchanged under the existing two-exception rule. Native module resolution alone does not make the entire bridge a B leaf. |
| Sim32FlushVDMPointer | Current body is a cdecl constant-success completion with no state. Candidate finite B ABI implementation after checking the pinned CPU40 contract and selected import/export shape; its comment alone is not proof. Current selected softpc.h exposes an extern declaration, so no restoration or removal is claimed from that comment. |

`wow_message_lifetime.c` clears captured keys before release and restores the
callback stack only when the current live TD and task16 still match. Its
worker-active provider is `ntvdm-exe/wow/wow_user_session_binding.c:95`, which
reads session_thread_current and SESSION_STATE_ACTIVE. This is concrete F
state dependence, not independent resource cleanup. Disposal ordering must
remove that dependency without attempting guest frees after machine retirement.

Correct the crosswalk's associated-body path for wow_user_session_binding.c
from the nonexistent wow32-dll/source location to ntvdm-exe/wow. The earlier
body review already identified the latter component; this corrects the mapping
artifact, not its count or a production file. Validate associated paths as
well as direct-target hashes. No dependency status, source or release changes.


## S14 P44: registry and task termination ownership

Baseline 375b7c6b6. Complete body reads: wow_registry_binding.c,
wow_task_cleanup_entry.c and wow_task_termination.c under wow32-dll/source.
Total: 52 bodies. Source review only; production and mirror delta zero.

| Chain | Ownership and migration constraint |
| --- | --- |
| Registry | DWORD token translation and locking qualify as B mechanics. The backing tree already resides in compat/mvdm_shadow_registry.c; New-T310OriginalSoftpcNinja.ps1 links it into NTVDM. Current EXE export lookup remains an outer image binding. Preserve one tree instance rather than linking a second copy into WOW32. |
| Registry lifetime | Keep the token lock through backing operations; failed token publication closes the new raw key; token wrap must not reuse stale authority. New-T404S5Wow32ProviderNinja.ps1 generates explicit-unload shutdown (reserved == 0) and force-includes the binding header for selected original callers. Cross-image ownership and unload order must be established before migration. |
| Incomplete startup | CURRENTPTD, gptdTaskHead, gcsWOW, htask16, absent hThread and native thread identity establish original task eligibility. These checks belong C; runtime binding admission remains outer. A stale TEB is not sufficient identity. |
| Resource cleanup | W32FreeTask prepares exit before WowOriginalFreeTask. Async socket users retire before guest segments are freed; original late cleanup still owns thread/socket release. Preserve both boundaries rather than relying on final session destruction. |
| Termination | Ticket bind/retire/failure RPC and NtvdmAuthorizeExit belong outer request policy. Original W32HungAppNotifyThread belongs C and exits its caller, hence the fresh local thread. Preserve serialization of its source-global completion state; do not move the ticket registry into B or introduce a B callback exception. |

Consumers checked by source references: wkman.c task binding/process exit,
wow32.c fatal exit, wow_user_task_lifecycle.c async retirement and
wow_user_registration_bridge.c incomplete-startup cleanup. These three bodies
add 17 covered symbol rows, not 17 eliminated dependencies. Exact migration
interfaces and runtime validation remain outstanding; no production relocation
or successful runtime test is claimed.
