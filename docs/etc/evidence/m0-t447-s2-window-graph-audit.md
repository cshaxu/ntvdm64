# T447 S2 window relationship publication audit

## Question and source evidence

After the process-retirement group, trace the shared window data required
by original direct USER16 readers before enabling further control support.
This is source evidence, not an application-failure attribution.

Canonical OpenNT windows/core/ntuser/rtl/wow.c SHA256 is
71E5B523BD44077AB4E86FFC6093544922C815C759BB753C32B4155E3206691D.
Its _GetWindow (line 531), _GetParent (618), _IsChild (682) and
_IsWindowVisible (716) consume parent/child/next/owner relationships, with
desktop FNID termination. Preserve these original reader algorithms.
The mirrored historical generated wow16/user/k/daytona/wowasmk.asm
_GetDlgItem body (line 338) reads child at +28, sibling at +20 and ID at
+104. This generated source corroborates the source contract; it is not
by itself a fresh disassembly of the currently staged immutable USER.EXE.
The existing wow_user_client_view_layout.h asserts the original WND offsets:
next 20, parent 24, child 28, owner 32 and fnid 122.

## Producer and update findings

| Surface | Current source evidence | Disposition |
| --- | --- | --- |
| Window creation | mvdm_softpc_wow_page_domain_publish_handle zeros the complete WND, writes handle/self/class and calls page_domain_window_info. | No relationship publication: four links remain zero. |
| Geometry/style refresh | page_domain_window_info writes rectangles, styles and child ID only. refresh_window has only callback_window as its production caller. | Refresh does not repair relationships; no general post-mutation refresh proved. |
| Desktop | page_domain_create_desktop_root writes root identity, bounds and WS_VISIBLE but not FNID_DESKTOP. | Original graph traversal's desktop termination condition is not represented. |
| Native mutations | wow_user_task_lifecycle.c WOW_NATIVE_CALL wrappers for SetParent/SetWindowPos/ShowWindow/EnableWindow preserve execution ownership and invoke native USER. | They do not explicitly publish affected graph state after return. Callback refresh alone is insufficient proof. |
| Destruction | Window property/object retirement removes the individual publication and retains borrowed backing until final release. | There are currently no links to repair; adding links requires removing incoming references before freeing/reusing storage. |
| Standard children | Native/internal child enrollment is still incomplete, as recorded in the checkpoint. | A graph cannot pretend unprojected children are absent; reconcile this prerequisite with the S3 control boundary before claiming graph completeness. |

Confidence: missing writes are confirmed in the selected producer source.
No claim that this alone causes SOL/WRITE OOM, nor that all consumers of
every WND field have now been audited. Native public USER remains the
relationship owner; guest projected fields cannot contain native pointers.

## Dependency-complete repair direction

Keep original query/traversal code. Do not replace GetDlgItem/GetParent with
new per-API thunks or patch immutable USER16. The missing contract is the
shared-data producer, not a missing lookup algorithm.

Before implementation, resolve the full publication set and identity rules:
which existing enrolled records can be referenced; how required native
children and desktop are represented; how an unrepresented relationship
fails without falsely publishing NULL. Public GetParent is not a universal
parent-field accessor because it returns popup owners; distinguish native
parent and owner semantics explicitly.

One repair group must cover initial creation before first callback,
parent/child/sibling/owner publication, reparenting and z-order updates,
callback-time visibility, failed creation, destruction and handle reuse.
Gather/validate the related records before exposing edges, then publish in
the existing guest execution ownership interval. Do not introduce a second
window manager, scheduler, unrelated HWND enumeration cache or kernel USER
server. Do not copy original server link-management algorithms into a second
authoritative tree: native USER already owns the actual tree.

Required acceptance: real CCPU direct-reader checks of parent/owner,
first/next/previous/last child order and GetDlgItem; hidden-ancestor visibility
and desktop termination; reparent/reorder; creation veto; retirement/reuse;
allocation/lookup failures without dangling or silently missing edges.
Then production guest callback/query evidence and ordinary application
non-regression. Existing field-layout or callback fixtures alone do not pass.

Expected new mirror/guest diff is zero if the existing projection seam can
carry the complete graph. Implementation scope and precise producer ABI must
be recorded in Current before changes. This audit does not admit unfinished
S3 controls wholesale or declare the S2 foundation complete.

## Creation and destruction ordering constraint

Canonical kernel/createw.c SHA256:
DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7.
The original ordering is not a single final-tree publication:

1. HCBT_CREATEWND is called at line 394 before parent assignment.
2. Non-child owner resolution occurs at 711; its parent is then replaced
   with the desktop. Parent assignment is at 752.
3. WM_NCCREATE at 841 runs before LinkWindow at 945.
4. LinkWindow publishes sibling membership before WM_NCCALCSIZE/WM_CREATE.
5. Destruction unlinks at 1569 while retaining owner through WM_NCDESTROY;
   the original comment explicitly requires that owner remain readable.

Consequently, do not derive a permanently symmetric parent/child graph from
the request in the CBT enrollment hook. A valid intermediate window can
have a parent without yet appearing in its parent's child list. Sample the
actual native relationship at the callback boundary and after mutations;
creation enrollment alone supplies storage/identity, not final membership.
Do not clear outgoing owner links prematurely merely to eliminate incoming
references. This ordering constrains the graph transport and its tests.

## Desktop prerequisite implementation

The existing desktop producer now writes the original WORD FNID_DESKTOP
(0x029d) at WND offset 122. No new traversal or server algorithm is added.
The checked-in tests/mvdm-host/wow_page_domain_fixture.c asserts that field,
neighbouring words, NULL root parent and WS_VISIBLE in real CCPU backing.
It is exercised by tests/observation/verify-wow-task-lifecycle.ps1 with
Case window-borrow, ProviderBuildRoot build/M0-T445/S6/provider,
WorkerBuildRoot build/M0-T445/S6/r003 and BuildRoot
build/M0-T447/S2/desktop-root/window-borrow.

Hidden background driver build/M0-T447/S2/verify-desktop-root.ps1 PID25868
started 2026-10-14 17:52:00 local. Logs are under
build/M0-T447/S2/desktop-root; diagnostic review deadline 18:05 local.
Worker/provider full build completed with result.exit=0. The fixture result
must be checked separately; this prerequisite is not full graph acceptance.

Fixture completed at approximately 17:53 local with
WOW_DESKTOP_FNID_CCPU value=029d adjacent=0 root=1,
WOW_WINDOW_BORROW_CCPU errors=0 normal=1 exception=1,
WOW_PAGE_DOMAIN_LIFECYCLE_OK and WOW_FIXTURE_OK case=window-borrow.
Adjacent class/menu projection markers also passed. This proves the corrected
root field in CCPU-backed storage and retained fixture behaviour, not execution
of the immutable guest's complete relationship readers. Product publication,
DOS17, application acceptance and graph integration remain pending.

## Staged relationship transport

The page-domain header/source now accept a call-local batch of full window
identities and four links. Resolve every identity/generation into existing
guest backing before any edge write. An allocation/lookup failure leaves the
old graph unchanged; publication contains only server-form guest addresses.
This transport intentionally does not impose symmetric membership: the
original pre-WM_CREATE intermediate state must remain representable.
Retirement refuses incoming references from other published nodes, including
the desktop, instead of freeing referenced backing. Native enrollment and
discovery are not connected, and no new worker export is added yet.

The checked-in CCPU fixture now exercises server-pointer rebase, stale
generation rejection after an earlier valid batch element, unchanged graph
on failure, pre-link intermediate state, incoming-reference refusal and
slot reuse. These are synthetic identities in actual CCPU storage, not native
window enumeration or real guest execution. The observation script requires
the exact WOW_WINDOW_LINKS_CCPU marker in addition to existing assertions.
Background driver build/M0-T447/S2/verify-window-links.ps1, PID12256,
started 2026-10-14 17:55:33 local; run root build/M0-T447/S2/window-links;
diagnostic review deadline 18:10 local. Full worker/provider build result=0;
fixture outcome is recorded separately when completed.

Second-role review found the remaining borrowed-window lifetime prerequisite:
createw.c lines 2312--2331, after WM_NCDESTROY, clears child/owner references
and substitutes the desktop parent for a non-desktop WND retained by locks.
Thus preserved WND backing must not retain outgoing references to independently
freed windows. Current staged transport protects published incoming references
only. Before wiring it, pair that original post-destroy field disposition with
the existing final-borrow owner; do not invent a second retired-object graph.
This is an explicit incomplete edge, not a full lifetime-safety claim.

The first transport run subsequently completed with suite.exit=0, all
WOW_WINDOW_LINKS_CCPU assertions and adjacent class/menu/borrow markers.
The second-role finding above is now addressed in the staged retirement
producer: after incoming-reference validation and after the terminal callback,
retained backing has NULL next/child/owner and the original desktop parent.
Its existing WW borrow still owns final physical release. No second registry,
refcount graph, guest algorithm or mirror edit was introduced. The fixture
now retains a retired node, checks those outgoing fields and frees it through
the existing VdmFreeVirtualMemory owner. Native relationship discovery remains
unconnected; this is not proof of complete native destruction ordering.

Revalidation uses the same checked-in window-borrow entrypoint and a fresh
build/M0-T447/S2/window-links-retirement run root. Hidden driver PID19168
started 2026-10-14 17:57:36 local, deadline 18:10 local. Prior transport logs
remain unchanged. No production package is published from either run.

The retirement rerun's window-borrow fixture also passed with the exact
WINDOW_LINKS, WINDOW_BORROW, PAGE_DOMAIN and class/menu markers, including
the retained-node outgoing-field assertions. Both builds completed. These
remain focused transport tests, not native discovery integration.

Adjacent callback audit: wow_window_dispatch_bound publishes/restores the
page-domain callback HWND/PWND cache and refreshes its view before the
original WOW window procedure. In contrast, wow_dialog_creation_binding.c
dialog_proc enters task/borrow scopes but calls creation->procedure without
that publication/restore pair. Its W32InitDlg boundary likewise has scopes
without that cache operation. This difference is confirmed at the selected
call sites; trace their original consumers before deciding the shared binding.
It belongs to S2 first-callback coherence and must not be hidden by testing
only the ordinary WNDPROC path or postponed as menu styling/control visuals.

### Dialog caller trace and correction

Following the actual originals narrows that finding: wcall32.c W32InitDlg
only initializes WOW words and returns dwUserInitParam; it does not call
CallBack16 and therefore does not need a new guest-cache scope. Leave it
unchanged. wudlg.c W32DialogFunc does call CallBack16 at line 105.
Original kernel/ssend.c LOCKPWND/UNLOCKPWND save/set/restore CallbackWnd
while retaining WND storage. Current project dialog_proc enters the task
and borrow scopes but lacked the equivalent existing page-domain operation.

The project entrance now acquires a local WW borrow, invokes the existing
callback_window operation before creation->procedure, and restores the saved
cache in finally before releasing that local borrow. A nested terminal call
can clear creation->borrow without freeing the outer callback's backing.
Failed publication does not call the guest-capable procedure; terminal
ownership cleanup remains in finally. No original source/guest change or new
scheduler/cache implementation is introduced.

Background driver build/M0-T447/S2/verify-dialog-cache.ps1 PID18612 started
2026-10-14 18:01:52 local, completed with suite.exit=0. Full worker/provider
build and checked-in dialog-lifecycle case pass, including the explicit
WOW_DIALOG_CALLBACK_CACHE errors=0 paired=1 exceptions=1 assertion. Its
page-domain boundary is mocked: this proves pairing, publication refusal,
nested destruction and an actually raised exception, not guest memory.

A complementary real CCPU test is added to wow_page_domain_fixture.c. It
creates an empty native dialog through the production provider, observes
the actual guest TEB cache and retained backing during nested DestroyWindow,
then verifies cache restoration, handle retirement and allocator balance.
Its callback is an explicit native witness, not a Win16 execution substitute.
The window-borrow observer requires WOW_DIALOG_CACHE_CCPU errors=0
nested_destroy=1 released=1. Hidden driver PID21972 started 18:03:53 local;
root build/M0-T447/S2/dialog-cache-ccpu, deadline 18:15 local. Outcome is not
assumed from the prior mocked run. No new release or application acceptance.

The real CCPU run completed with suite.exit=0 and the required
WOW_DIALOG_CACHE_CCPU errors=0 nested_destroy=1 released=1 marker, alongside
all window-links/class/menu/borrow assertions. Documentation governance also
passed. Production dialog entrance plus actual CCPU backing is now exercised;
the callback body remains a native witness, so immutable guest execution is
not inferred from it.

The follow-up three-app observation driver PID17060 started 18:04:46 local,
waited for PID21972's successful completion, then uses the existing isolated
package observer with SpoolCompanion under build/M0-T447/S2/dialog-cache-apps.
Candidate hashes and frontiers are recorded there; review deadline 18:15
local. No published assets or O:/winnt images are changed by this observation.

The three-app observation completed with result.exit=0: WINMINE has a
visible Minesweeper window; SOL still displays Out of memory; WRITE still
displays Not enough memory for Write to complete this operation and has a
hidden MSWRITE_MENU window. These match process-retirement-apps, not normal
SOL/WRITE initialization or playable-WINMINE acceptance. The cache repair
does not establish an improved application frontier.

## Native-class enrollment dependency closure

The next production wiring cannot consist solely of enabling prepared class
metadata. Source tracing identifies the following dependent edges:

| Edge | Selected source and fact | Required paired treatment |
| --- | --- | --- |
| Initialization | registration_bridge ensure_lifecycle passes original WOW callbacks to lifecycle_initialize. That initializer leaves class lists empty. Both standard_classes_prepare and standard_classes_bind_cleanup have test callers only. | Bind both initialization and cleanup only with the complete native-control enrollment path; unwind partial metadata on failure. |
| Original creation consumer | wuwind.c W32CreateWindow calls FindClass16 before pfnCsCreateWindowEx; missing class storage returns NULL. | Preserve original lookup/classification; never manufacture per-application fallback classes. This is not proof that a particular SOL/WRITE error came from this call. |
| Name resolution | wow_user_create_window_ex resolves through the provider's class lists, then create_bound snapshots the selected class target. | Preserve private-before-public selection and integer atoms; standard metadata must exist before this lookup. |
| Native entry | creation_hook publishes WW/object data but does not install wow_window_native_proc for a borrowed native class. Native CreateWindowEx still calls that class's original procedure. | Publication alone does not route terminal callbacks to WW retirement. Preserve the actual native/CallProc target and existing subclass chains when installing an enrollment gateway; do not replace native control policy. |
| Internal children | dialog_create_hook only claims the top-level #32770 dialog. Native control children bypass wow_user_create_window_ex. | Define the original task/class owner for internal children before publishing their relationship links; do not omit them or assign them an arbitrary task. |
| Retirement | window_words detach removes associations and process disposal rejects live ones. | Native terminal notifications must reach this existing path; otherwise enabling metadata creates permanently live associations. Retain original cleanup restoration tables. |

This is why a successful nine-class metadata fixture cannot justify a
production initialization toggle. The dependency is shared with planned S3
control work, while S2 relationship publication requires its enrollment
foundation. Finish the bounded creation/update/destruction binding together;
do not silently count native EDIT/local-memory or all controls as completed.
No production initialization toggle was made during this audit.

### Native gateway and existing subclass ownership

Additional source constraint: ntw16s-dll/attachment.c and controls.c use
SetWindowSubclass/DefSubclassProc/RemoveWindowSubclass. WOW's title policy
enters their creation scopes around native USER calls. A native-control
enrollment change must preserve that existing chain, including callbacks
added by later CBT hooks. Merely replacing GWLP_WNDPROC with the class's
snapshotted procedure can skip already-installed per-window subclasses;
rolling back a veto cannot blindly restore an old pointer over a newer one.

Consequently no raw SetWindowLong gateway replacement was implemented.
The next implementation decision must compare a chain-preserving subclass
with thread-owned native notification enrollment/retirement. Neither may
pretend lifecycle observation also implements guest procedure replacement:
GWL_WNDPROC/SetClassLong logical targets, original CallProc tokens and direct
calls outside an active subclass invocation still need their existing
contracts. DefSubclassProc cannot replace CallWindowProc outside its dynamic
subclass call. Required cases include native target forwarding, guest target
replacement, style subclass before/after enrollment, CBT veto and nested
terminal callbacks. This prerequisite is source-proven, not a new observed
application regression or authorization to replace the styling component.

### Selected early creation boundary

Further source tracing resolves the project's actual style ordering:
registration_bridge registered_create_window_ex begins the style scope before
entering create_bound. NTW16S attachment.c creation_before only records pending
windows/theme state; creation_return installs subclasses after successful
WM_CREATE (or WM_INITDIALOG). create_bound installs its CBT hook immediately
around CreateWindowEx and publishes the WND before those messages. Therefore
the existing WOW entrance can be installed at that early boundary, below the
subsequent style chain, without another permanent observer or dispatcher.

The creation binding now queries the existing class binding's immutable
native_registration_borrowed ownership flag, not a class-name allowlist.
For such a registration it retains the actual per-window native predecessor
for an unchanged native target, publishes WW/object state, then installs the
existing wow_window_native_proc before calling later CBT hooks. Guest logical
class targets are not replaced by the native predecessor. Original
DispatchClientMessage remains the native/guest dispatch owner; native CallProc
tokens use the existing forwarding binding. Native control code is unchanged.
On a CBT veto it restores the previous native procedure only if its entrance
still owns the current slot, then withdraws its object association. Later
subclasses are never blindly overwritten. This does not prove every foreign
hook's mutation during a veto; retain that as an explicit interoperability case.

The real CCPU window-borrow fixture now creates BUTTON through create_bound
using prepared native class metadata, checks the installed entrance and native
text forwarding, installs a real comctl32 subclass above it, destroys through
that chain, and checks native-CBT veto withdrawal. Tests use the actual class
and window provider plus CCPU storage; they are not guest control acceptance.
The observer requires WOW_NATIVE_CREATE_CCPU errors=0 forward=1 subclass=1
veto=1 retired=1. Hidden driver PID20396 started 2026-10-14 18:12:54 local;
run root build/M0-T447/S2/native-create, review deadline 18:25 local.
No production standard-class initialization or internal-child toggle yet.

At 18:14 local the native-create driver completed with suite.exit=0.
WOW_NATIVE_CREATE_CCPU errors=0 forward=1 subclass=1 veto=1 retired=1 passed,
as did native class projection, dialog cache, window graph transport,
exception/normal window borrowing and page-domain lifetime assertions.
The second-role diff review confirms the ownership test is a binding flag,
not a control-name or application special case; native messages still use
the original DispatchClientMessage branch, and standard registrations are
not modified or unregistered. No newly admitted mirror/guest diff.
This proves the selected create_bound path only. Internally created controls
and production class bootstrap remain unconnected and block full capability
acceptance; ordinary package validation/publication is still pending.

### Internal-control creation and relationship mutation audit

The next source pass identifies why creation-only enrollment is insufficient.
Read-only canonical references (no source imported in this pass):

| Source | SHA-256 | Relevant boundary |
| --- | --- | --- |
| OpenNT windows/core/ntuser/client/dlgbegin.c | C3DF77A2987E97DF8872C3DAB8F6E7C10A08850C5EBCB439C0EC1DC88B0CDC44 | Lines737--827: global EDIT DS, WOW creation-parameter unwrapping, control creation with NULL lpWOW. |
| OpenNT windows/core/ntuser/client/comboini.c | B54A3787073F51E0578ACF2AA52AFEC144ABDE1C367F30DDBA8855810D5C4EC8 | Lines164--237: create list before edit, both with NULL lpWOW; a droppable list is subsequently hidden and reparented to NULL. |
| Selected src/mvdm/wow32/walias.c | 9BFA0B0412CA6193E4D9D2851BDA0E19214F3E57EDF47DCB5235CB9D07577C81 | FindPWW: lazily classify unknown WW through the existing GetStdClassNumber and set WWSTATE_ICLASSISSET. |

Original createw.c copies the three WOW DWORDs only when lpWOW is supplied.
Internal controls must not inherit their parent's iClass, procedure or guest
instance prefix. Keep the original FindPWW classifier; no second standard
class table or per-application fallback is justified.

Current dialog_create_hook stops enrollment once creation->window is set;
creation_hook stops claiming once scope->claimed is true. Thus these scoped
hooks do not enroll the native-created children. The explicit native-create
test above cannot certify their metadata or retirement. The same audit also
finds SetParent in wow_user_task_lifecycle.c uses WOW_NATIVE_CALL: it pairs
native execution ownership but does not publish relationship changes.
The staged page-domain relationship batch still has no production caller.

The original combo path is a concrete counterexample to publishing only the
creation tree: its list is created with WS_CHILD and the combo as parent,
then NtUserSetParent(hwndList, NULL) changes that relationship. Enrollment
cannot be removed merely because the list is no longer a descendant.
This original path is evidence of the required mutation contract, not proof
of the implementation details inside modern USER32. Wrapping the project's
SetParent alone also cannot observe USER32-internal calls.

Implementation review constraints for the pending paired group:

1. Reuse the existing WW/class/object owners and terminal gateway for internal
   windows; creation scopes supply the actual creating task, not the parent's
   inferred task. Nested explicit creation must not double-enroll an HWND.
2. Preserve original zero-prefix/lazy classification and the actual native
   procedure chain. Do not manufacture guest EDIT DS or transform arbitrary
   application creation parameters in this metadata helper.
3. Synchronize relationships at guest-observable boundaries and after native
   mutation; native USER remains the relationship authority. Unknown native
   nodes require an explicit disposition, never silent NULL substitution.
4. Pair creation failure, downstream CBT veto, reparenting and terminal
   withdrawal with generation-safe storage. Do not enable standard-class
   bootstrap until this complete producer path is connected.

Separate S3 integration obligations remain visible: original dlgbegin.c
unwraps a Win16 template's extra creation DWORD and obtains shared EDIT DS;
the current public CreateDialogIndirectParam path does not itself establish
equivalence for either. Successful child enrollment will not prove those
contracts or normal SOL/WRITE initialization. This pass changes evidence only,
adds zero mirror/guest diff and makes no new runtime or publication claim.

### Shared internal-control enrollment implementation

The explicit creation entrance and dialog CBT entrance now share
enroll_created_window through wow_window_creation_forward_internal. This
factors the existing WW/class/object publication and native gateway install,
not native control behavior. It claims only a previously unenrolled HWND with
an enrolled parent and already-prepared borrowed native class metadata. A
nested explicit scope's existing association is left untouched. Internal WW
starts zero; original FindPWW still owns later classification. The creating
scope supplies the task, independent of the parent's task. No permanent hook
or new object registry is introduced, and no native class registration changes.

The local CBT borrow protects metadata while downstream hooks run. Veto or
exception restores the preceding native procedure only when our gateway still
owns the slot, withdraws our association and releases the borrow. On success
the existing property owns metadata until the normal WM_NCDESTROY gateway.
The same helper is used for the explicit path, removing its duplicate attach,
owner-publication and gateway-install sequence. Unknown/unprepared classes
remain unhandled by this staged helper: it is not permission to expose an
incomplete graph or activate production standard-class bootstrap.

Hidden driver PID21640, started 2026-10-14 18:22:24 local, completed full
worker/provider build successfully. Its actual CCPU probe reports zero errors
including internal_combo=1 and final WOW_PAGE_DOMAIN_LIFECYCLE_OK. The outer
suite failed because its already-loaded observer expected the old anchored
marker without internal_combo. This is recorded as a failed suite, not a
passing delivery; the checked-in observer now requires the expanded marker.

The follow-up adds both internal-entry families: actual native COMBOBOX
creates EDIT/list windows and forwards item operations, while the production
dialog provider creates a BUTTON from a real dialog template. Assertions check
original zero-prefix where applicable, creating-task/class ownership and
complete object retirement. A downstream CBT hook specifically vetoes the
combo list; final class-reference and CCPU allocator checks detect leaked
publication. This extends the existing real CCPU fixture, not a guest test.
Hidden driver PID25792 started 18:25:21 local, review deadline 18:40 local;
run root build/M0-T447/S2/internal-native-create-complete. The driver includes
full build, CCPU window-borrow, dialog-lifecycle, window-procedure and
standard-class regression plus documentation governance. Results pending.

At 18:29 local PID25792 had exited; suite.exit and complete.exit both contain
0. The expanded WOW_NATIVE_CREATE_CCPU marker reports errors=0 with both
internal_combo=1 and internal_dialog=1. The final CCPU allocator/class release
checks pass. Dialog-lifecycle reports rollback cases=4 errors=0 and paired
callback cache/exception cleanup errors=0. Window-procedure reports tagged
calls=3, native-subclass forwarded=3 preserved=1 and errors=0. Standard-class
reports restored=9, resolved=9, classes=9 and errors=0, including private-class
cleanup. Documentation governance passes. Logs are preserved in the run root
above. This is local group verification, not DOS17, three-app guest acceptance,
coherent publication or S2 completion.

Second-role source review: shared enrollment still delegates procedure
selection to original DispatchClientMessage; native callbacks do not acquire
guest execution merely for enrollment. Native USER retains create/order/
reparent/destroy behavior. This change does not intercept later arbitrary
native-created windows or synchronize graph mutations, and does not solve
dialog EDIT DS/template-parameter equivalence. Those producer and S3 gates
remain explicit. No new guest or original-source edits and no publication.

Relationship sampling constraint from the retained rtl/wow.c _GetParent:
for a child it returns spwndParent, but for a popup it returns spwndOwner,
and for a tiled top-level window it returns NULL. A producer cannot populate
WND.spwndParent with public GetParent blindly. _GetWindow(GW_HWNDFIRST)
instead follows structural parent then its first child, and other commands
walk next/owner edges. The two fields must remain distinct; HWND-parent
changes and owner changes cannot share a guessed interpretation. External
or unrepresented sibling/owner nodes still need a source-backed projection
contract before connecting the staged batch to these original guest readers.

### Actual local readers and external-window ownership boundary

The next pass checked the selected compilation path, not just function names:
wow16/user/usercli.asm selects GETDLGITEM, GETPARENT, GETWINDOW, GETTOPWINDOW,
ISCHILD and ISWINDOWVISIBLE as local PMODE32 client thunks. The corresponding
wusercli.c fallback implementations are under `#ifndef PMODE32`. Connecting
those fallback functions in WOW32 cannot repair this immutable x86 guest's
direct reads. The existing verify-wow-user-profile.ps1 already checks actual
selected USER.EXE bytes for GetDlgItem child/next/ID and GetParent parent/owner,
in addition to the desktop FNID and handle/cache layout. The earlier generated
assembly qualification is retained, but this existing binary check is stronger
evidence and must be included in the graph group's acceptance.

Additional canonical sources inspected read-only:

| Source | SHA-256 |
| --- | --- |
| windows/core/ntuser/rtl/getset.c | 4A2CB2067F5DD776676D7F808F40F6468082DB018E16BCF9A77DAF00BDDB34C9 |
| windows/core/ntuser/kernel/cleanup.c | DFB0FB8D81AFEB555B3631ADC5456F4CEB13142F2E954A6F2B22631C49142E5C |

GetWindowData explicitly rejects GWL_WNDPROC across process boundaries, but
its GWL_WOWWORDS and GWL_WOWDWORD branches do not contain that same check.
Do not generalize procedure privacy to every original WW read. The shared
graph readers likewise have no owning-process filter: a native sibling or
owner outside the worker cannot be silently skipped. Original _WOWCleanup
uses task/process ownership when selecting windows, so visibility in the
shared graph does not confer destruction or procedure-replacement authority.

This rules out reusing wow_window_words_attach_target indiscriminately for
all HWNDs found by native enumeration. That API installs a native property,
associates class/object cleanup and, at creation, installs a procedure gateway.
Foreign/native unowned views need read-only shared publication, distinct from
managed callback/cleanup ownership. The existing page-domain guest handle and
WND storage remains the candidate shared-view owner; do not introduce a second
authoritative HWND tree. Merely setting a managed object's pOwner to NULL is
not a completed design: original cleanup has typed owner dereferences and
current process disposal rejects any still-live object-table entry.

Remaining closure decisions for this same graph group are now explicit:

| Required boundary | Constraint / next implementation obligation |
| --- | --- |
| Native sampling | Structural parent, owner and sibling order are separate; public GetParent cannot supply all of them. Include native-created, external and retained intermediate windows without inventing task ownership. |
| Public view | Reuse guest-owned WND/CLS publication, containing no native pointers or executable targets. Preserve original class-name and shared-handle consumers for unowned nodes as well. |
| Private WW access | Audit foreign/native FindPWW consumers separately from native procedure privacy; a public read-only graph is not a substitute for the original private accessor contract. |
| Update point | Synchronize before guest-visible callbacks and after native mutations, including internal USER changes not intercepted by the SetParent wrapper. No permanent creation-only tree. |
| Retirement | Remove incoming edges before releasing backing; distinguish retiring our window from retiring a sampled view of somebody else's live window. No foreign DestroyWindow, subclass or ownership assignment. |
| Execution reuse | Retain guest local walkers and original lookup/classification/cleanup. Public USER replaces the unavailable server's storage producer, not those algorithms. |

This source pass does not enable a partial graph or attribute SOL/WRITE's
remaining failures to an untested cause. It changes the next implementation
boundary: the missing closure is shared-view ownership for unowned nodes,
not another control-name special case or another GetWindow thunk.

Verification: the first hidden profile runner (PID24816, 18:35:48 local)
failed before inspection because Windows PowerShell inherited a module path
without Get-FileHash. The replacement driver confines PSModulePath to its
own PSHOME/Modules; it does not change the user or machine environment.
PID19376 started 18:37:16 and completed with result.exit=0 under
build/M0-T447/S2/window-reader-profile-modules. The unchanged accepted-package
system32/USER.EXE and the mirror both match SHA256
6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C.
All 58 source-and-binary consumer checks pass with profile PMODE32; governance
also passes. This is binary ABI evidence, not a new execution or application
acceptance result. No runtime files changed in this source-audit pass.

### Reuse existing unowned publication and protect referenced CLS

Inspection of the actual page-domain boundary narrows the implementation:
publish_handle already samples GetWindowInfo and numeric child ID without
SetProp, subclass installation, task assignment or native destruction. The
managed WW/object-table layer is separate. Therefore external public views do
not require a second HWND registration system merely to acquire guest WND
storage. Use the existing window and named-class allocations; keep discovery,
ownership transition and refresh/retirement disposition explicit above them.

Original kernel/class.c (SHA256
82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A)
rejects UnregisterClass with live cWndReferenceCount and asserts zero at
DestroyClass. Managed class bindings already maintain that ownership, but the
lower page-domain retire_class previously freed CLS without checking whether
a published WND still pointed to it. The retirement boundary now rejects that
operation before changing allocation state. This is a projection lifetime
guard, not a replacement class policy or a new native reference registry.
Outstanding unpublished WND borrows still require their existing owner to
retain the class; scanning published WNDs does not replace that obligation.

The real CCPU fixture samples the native desktop HWND as a stable foreign
process witness, publishes a temporary WND/CLS/name view through the existing
APIs, checks original pointer rebasing and absence of native procedure/module/
task values, and requires early class retirement to fail. It then withdraws
the view and class, verifies allocation balance, and verifies the original
desktop HWND remains alive with no WOW property attached. It never changes
that HWND. This temporary test record does not change the production desktop
mapping: native desktop discovery must still resolve to the existing guest
desktop root, not create duplicate production roots.

Hidden driver PID14732 started 2026-10-14 18:42:17 local, review deadline
18:55 local, run root build/M0-T447/S2/unowned-window-view. Full worker/provider
build and real CCPU window-borrow suite run in the background; results pending.
No ABI, original-source or guest changes. This validates a needed existing
storage boundary and its lifetime repair, not completed external graph
discovery, foreign private-WW semantics or production graph activation.

The driver completed with suite.exit=0. WOW_UNOWNED_VIEW_CCPU reports
errors=0 foreign=1 class_guard=1 native_untouched=1 released=1. All existing
CCPU class, internal-control, dialog-cache, relationship transport and window
borrow markers also pass, ending in WOW_PAGE_DOMAIN_LIFECYCLE_OK. Worker and
provider builds and documentation governance passed. The header contract is
clarified to describe projection lifetime rather than imply every caller
must own a native HWND. No native registration/ownership was added.
This remains local verification; no package publication or S2 closure.

### Menu ownership and producer connectivity audit

Read-only source tracing, not another application-failure experiment, exposes
two prerequisites for extending window graph discovery to unowned windows.
Canonical OpenNT inputs under windows/core/ntuser/kernel are handtabl.c
(SHA256 F0B56314A93423599F06645D17104391F19A0CA07C3944853B065D8866493F7C),
mncreate.c (C351C9C262CD78EEEFCB9A6A375B72CE66E55C2C8146B21C0C7EC54857A719B2),
and mnsys.c (DDC24C1DAF539EFCB7225BD3231E4B6B88E7130E43D826918753D50DDCFC0D07).

HMAllocObject records the allocating thread's process and, for a WOW thread,
its hTaskWow. InternalCreateMenu reaches that allocator. Original _WOWCleanup
selects that creator identity, not the task that subsequently queries a menu.
Current menu_refresh instead passes the current task to publish_tree; the
first publication creates an ownership entry and recursively gives previously
unseen submenus that task. Refreshing an existing entry preserves its creator,
but first observation is not proof of creation. Applying this path to foreign
window discovery would incorrectly grant cleanup ownership.

The other prerequisite is actual producer connectivity. The original clmenu
parser is retained, but wow_menu_parser_bindings.h maps NtUserCreateMenu to
CreateMenu and NtUserDestroyMenu to DestroyMenu. Neither parser/resource header
includes wow_menu_native_bindings.h. The generated provider graph compiles
clmenu with the ordinary cc rule; its forced include is nt.h, not the menu
redirect header. Repository include tracing finds that redirect header only
in its own implementation and wow_window_words_binding.c. Neither inclusion
selects WOW_ORIGINAL_MENU_CLIENT or WOW_USER_PRIVATE_REDIRECT. Therefore having
the wrapper implementations linked is not evidence that original menu
producers call them. Resolve the actual imported-call binding as part of the
repair; do not add another parser or claim wrapper fixtures prove connection.

Important qualification: _GetSystemMenu may load a private menu lazily through
xxxClientLoadMenu, so it is incorrect to classify every query-returned menu as
unowned. Distinguish actual creation from observation, including lazy native
creation, existing system menus, resource submenus and borrowed native menus.
Do not guess ownership from the window's owner or simply use task zero.
The current process disposer requires all object entries retired; changing
only task attribution would leave borrowed entries blocking disposal.

Required repair closure: connect original creation/parser and failure cleanup
at their existing bounded native interface; separate observation from creator
registration; retain original task cleanup selection; retire borrowed views
without destroying native menus; cover nested resources, cross-task queries,
system-menu reset, native recursive destruction and process disposal. Keep
the window-graph production activation pending this closure. No new guest or
original-source changes are needed merely to establish these source facts.

Owner reiterates that guest images, including USER.EXE, must never be modified
or rebuilt. verify-wow-user-profile.ps1 reads hashes, bytes and the retained
map only; its previously recorded passing result is ABI inspection, not a
guest build. This audit adds no runtime change, test pass or publication claim.

First connection repair includes the existing native menu binding at the
original-parser adapter header, where WOW_ORIGINAL_MENU_CLIENT already scopes
it. This connects allocation, item insertion and failure destruction together,
without modifying clmenu.c or any guest file. The real CCPU menu fixture now
calls CreateMenuFromResource, checks creator identity and both levels of the
published MENU/ITEM tree, recursive destruction, and invalid-template rollback.
The observer requires its new completion marker. This does not yet repair
first-observation attribution or connect every original WOW menu thunk.
Former parser-only native fixtures do not establish this runtime-associated
contract; their standalone harness coverage must be reconciled before delivery.

Background verification: PID26372 started 2026-10-14 18:56:56 local with
verify-window-links.ps1 -RunName resource-menu-connection; evidence root is
build/M0-T447/S2/resource-menu-connection, review deadline 19:10 local.
Worker and provider builds completed successfully. The CCPU fixture and
governance result remain pending at this record point. No published images
are replaced by this build.

The driver completed with suite.exit=0 and governance passed. The required
WOW_RESOURCE_MENU_CCPU marker reports registered=1 nested=1 rollback=1;
WOW_NATIVE_MENU_ENTRY_CCPU result=1 and the existing class, dialog, internal
control, window-link and borrow checks pass, ending in
WOW_PAGE_DOMAIN_LIFECYCLE_OK. This uses the actual provider parser object and
CCPU memory, not a test replacement parser. It is still not a guest app
acceptance result. Independent review confirms the production change is only
the adapter-header inclusion (three lines); original parser/source and guest
images are unchanged. Query ownership, ordinary thunk connection and the
standalone native-only harness migration remain outstanding before delivery.

The corresponding ordinary-thunk connection is in the existing owned
wow_user_private_access.h redirect block, already selected by the original
WOW precomp boundary. Include the same native menu bindings there; no new
precomp.h mirror change is necessary. The CCPU fixture calls the actual
WU32CreateMenu and WU32CreatePopupMenu functions, verifies their returned
16-bit aliases resolve to creator-associated published menus, then releases
them. This closes producer connectivity, not first-observation ownership.
All ordinary mutation wrappers are selected by the same header, so their
short-handle operands and query-only registration remain in the review scope.

The first ordinary-entry runner PID26476 (2026-10-14 19:00:33 local,
build/M0-T447/S2/original-menu-entry-connection) built the worker/provider,
then failed fixture linking: its handwritten __fastcall declarations requested
@WU32CreateMenu@4 and @WU32CreatePopupMenu@4, whereas the selected provider
defines _WU32CreateMenu@4 and _WU32CreatePopupMenu@4. wow32.h leaves FASTCALL
empty without i386 and the selected compile uses /Gz. Correct the fixture to
WINAPI; do not change the product calling convention to accommodate a test.
PID24808 started 19:02:44, review deadline 19:12, under
build/M0-T447/S2/original-menu-entry-abi, reusing the just-built unchanged
provider/worker and rebuilding the fixture. Result pending.

Retirement review additionally traces canonical kernel/mndstry.c
(SHA256 09B507CC0D0797080BB99B5DF158A059DA6B0E2C6A86D53390EC4947C8362E3F):
_DestroyMenu calls HMMarkObjectDestroy and returns without freeing a locked
menu. The current guest-view retire_menu immediately frees its MENU and ITEM
allocations, so incoming WND/menu links and retained callback views must be
accounted for before extending discovery to borrowed menus. Preserve the
native destruction policy; do not emulate an entire server object manager.
This is part of the pending ownership/retirement closure, not a passing claim.

The ABI-corrected runner completed with suite.exit=0. Actual original entry
calls report WOW_ORIGINAL_MENU_ENTRY_CCPU create=1 popup=1 registered=1;
the parser's registered/nested/rollback marker and every existing CCPU marker
pass through WOW_PAGE_DOMAIN_LIFECYCLE_OK. Governance passed. Review confirms
the connection is an adapter-header include, not a replacement original thunk
or changed calling convention. No guest, mirror-source or release mutation;
this does not close the separate ownership/retirement obligations above.

Ownership repair now separates register_created(process, task, menu) from
publish_tree(process, menu). Only a proved allocation may create an owned
entry; attempting to re-register an existing view is rejected. Recursive
observation preserves known creators but registers unknown menus with no
cleanup owner. The existing original _WOWCleanup process/task selection then
skips those borrowed views without a replacement cleanup algorithm. No extra
registry is introduced. The native creation wrapper alone uses registration;
mutation, window refresh and GetSystemMenu use observation. A queried system
menu remains native-managed: this no longer invents its original allocator's
identity. Exact lazy-system-menu task cleanup equivalence is still under
review and is not claimed by this narrower ownership-safety correction.

Process disposal, after stopping all task producers, preflights the existing
object table and refuses any owned or non-menu entry before withdrawing
borrowed menu projections. It never calls DestroyMenu on borrowed objects;
partial storage-release failure remains retryable. Normal native destruction
also retires borrowed views. This does not yet solve retained incoming menu
references while guest execution continues; that gate remains open.

CCPU checks cover creator preservation, original task cleanup ignoring queried
native menus, recursive borrowed submenu views, refusal to convert observation
into ownership, last-owner refusal with owned objects, view release and native
menu survival. The existing menu-cleanup and process-retirement fixtures must
also pass with the revised entry points. No guest or mirror-source changes.

Hidden verification driver PID9256 started 2026-10-14 19:08:16 local with
verify-menu-ownership.ps1, evidence root build/M0-T447/S2/menu-ownership,
review deadline 19:20 local. Full worker/provider build and CCPU window-borrow
case passed; WOW_MENU_OBSERVATION_CCPU reports unowned=1 cleanup_skips=1
native_survives=1 released=1. The separate menu-cleanup and process-retirement
cases and final governance are pending at this record point.

PID9256 completed with suite.exit=0. The separate original menu cleanup case
reports errors=0, the process-retirement case reports errors=0, and governance
passes. The real CCPU test proves borrowed native menus survive original task
cleanup and explicit view withdrawal; the process fixture independently tests
the production disposer refusal/retry path. It does not claim that fixture's
mock menu storage proves real CCPU disposal. Exact native lazy-system-menu
ownership and active incoming-reference retirement remain open as above.

Post-connection ordinary-application observation uses PID27400, started
2026-10-14 19:11:31 local, review deadline 19:23, with
verify-callback-package.ps1 -RunName menu-ownership-apps -SpoolCompanion.
Its immutable candidate lives under build/M0-T447/S2/menu-ownership-apps;
only newly built ntvdm.exe and WOW32.DLL replace accepted host files there.
The existing original WINSPOOL companion is copied byte-exact after its
pinned-hash check, never compiled or patched. Temporary runtime staging is
under O:/tmp and cleanup is scoped to those exact package processes.
No installed or release package is replaced. Observation is pending and
cannot establish gameplay or normal-exit acceptance.

PID27400 finished with result.exit=0 (observation/cleanup completed, not three
applications passed). WINMINE again has its visible Minesweeper main window.
SOL still shows Out of memory; WRITE still shows its memory-error dialog and
hidden MSWRITE_MENU window. These match the prior dialog-cache-apps startup
frontiers and do not demonstrate new playable behavior. No inference is made
that either OOM message proves actual heap exhaustion. The source-proven
window graph/standard-class and message boundaries remain open; this menu
ownership repair is not claimed as their root-cause fix or as S2 closure.

### Native destruction remains the owner; projection ordering is separate

Further source tracing narrows the proposed lifetime repair. Canonical
kernel/handtabl.c::HMMarkObjectDestroy sets HANDLEF_DESTROY but returns FALSE
without freeing when cLockObj is nonzero. rtl/wow.c::ValidateHandleMacro
checks index, uniqueness and type, then returns the desktop-mapped object;
it does not reject HANDLEF_DESTROY. Therefore a successful DestroyMenu does
not by itself prove the original menu handle or backing is already dead.
Do not introduce a parallel destroy flag policy merely because an API returns
success. The existing public IsMenu check remains the native lifetime witness;
modern boundary equivalence still needs acceptance, not an assumption that
this flag ordering automatically describes every modern implementation.

Canonical kernel/mnapi.c (SHA256
6358AED5425F56ED9C74CD93E1849106868803BAE3A90C382FDFE307B71222C7)
xxxSetMenu locks the new menu into WND before xxxRedrawFrame. The retained
guest wowasmk.asm::_GetMenu@4 then reads WND+104 and MENU.head.h directly,
without validating that menu through the handle table. This supports the
existing before-callback publication placement, but also means cached WND
and ITEM edges cannot outlive the MENU allocation they reference.
Current menu_publish retires invalid native records before updating a live
tree, and refresh_window writes its WND menu edge only after that operation.
That ordering, plus failed refresh and native reentry, must be addressed at
the shared-view producer; copying HMMarkObjectDestroy or a USER object manager
would not repair stale copied edges. No additional original-source import is
justified by this finding.

System-menu ownership also has two different original cases, not one rule:
GetSysMenuHandle in mnsys.c and xxxSetDialogSystemMenu explicitly clear pOwner
on the shared default root and first submenu, while _GetSystemMenu can load a
private copy. Thus neither assigning every observed system menu to the caller
nor claiming every copy was originally system-owned is valid. The borrowed
projection is a conservative ownership boundary, not proof of exact private
copy cleanup equivalence. Keep that distinction in the remaining design.
All source/guest inputs in this pass were read only; no runtime changes or new
test/publication result are claimed.

### Owner-directed complete menu batch (2026-10-14)

The owner rejects per-entry implementation/build cycles. The remaining menu
work is one dependency-complete source/implementation batch, followed by its
integrated verification. Earlier narrow test passes remain evidence for their
exact scopes, not permission to omit any row below or close this batch.

| Surface | Reached original source and current boundary | Batch obligation |
| --- | --- | --- |
| Allocation and resource loading | wumenu.c CreateMenu/CreatePopupMenu; retained client/clmenu.c, selected through wow_menu_parser_bindings.h | Keep original thunk/parser, register only proved allocation, cover ordinary/extended templates and recursive failure rollback. |
| Ordinary mutations | wumenu.c Append/Change/Check/Delete/Destroy/Insert/Modify/Remove/SetMenu/SetMenuItemBitmaps/Hilite; wusercli.c EnableMenuItem | The private-access header now selects adapters; verify full and short root/submenu operands and preserved return/error conventions for the complete family. |
| Guest-local queries | wusercli.h PMODE32 selects LOCALAPI for GetMenu/GetMenuItemCount/GetMenuItemID/GetMenuState/GetSubMenu; wowasmk.asm reads WND/MENU/ITEM | Wiring host query functions alone cannot fix these calls. Keep guest untouched and publish their exact graph before guest execution. |
| Conditional mutation fallback | wowasmk.asm EnableMenuItem reads ITEM state; when it differs, sets wow16CsrFlag; wutbl2.h selects WU32EnableMenuItem | Do not replace the original fast path or misclassify it as a guest-side state mutation. Its state observation and host fallback must see the same menu. |
| Native query results | wusercli.c non-PMODE32 queries; wumenu.c GetMenuString and ModifyMenu's original fallback GetSubMenu | Distinguish selected guest-local paths from alternate host paths. Do not add duplicate query algorithms or delete the retained original compatibility branch. |
| Menu messages | wmtbl32.c selects wmdisp32.c WM32InitMenu/WM32MenuSelect/WM32MenuChar | WM_INITMENU and WM_INITMENUPOPUP carry a menu in wParam; WM_MENUSELECT and WM_MENUCHAR carry one in lParam. Original conversion truncates the handle without registering its view. Current window-only refresh does not cover a detached popup or native system-menu message. Publish valid borrowed message menus before conversion/callback, preserving terminal/no-menu and invalid-input semantics. |
| Popup tracking and painting | wumenu.c WU32TrackPopupMenu and WU32DrawMenuBar still call native USER directly | Audit the same inherited-lock/callback boundary used for Hilite/SetMenu; include guest mutation or destruction during tracking and cancellation, not only a successful selection. Do not replace native menu tracking. |
| Class-menu association | wow_window_creation_binding.c loads through the retained resource client, assigns GWLP_ID before WM_NCCREATE | Preserve original ordering, explicit menus and child IDs. Failure destruction currently calls native DestroyMenu in this owned file; the completed retirement path must cover it rather than rely on a later unrelated mutation. |
| System-menu copies | kernel/mnsys.c GetSysMenuHandle/_GetSystemMenu and mnapi.c dialog-system-menu setup | Keep shared defaults distinct from lazy private copies. Observation is not proof of allocation ownership. Include revert, window destruction and task cleanup; do not claim borrowed projection alone restores original ownership. |
| Identity conversion | walias.h HMENU32/GETHMENU16 use USER32/USER16; current menu_resolve handles enrolled full/short identities | Root and MF_POPUP operands, native message handles and resource-created full handles must converge without turning item IDs into handles or adding a second alias registry. |
| Graph retirement | page-domain MENU/ITEM and WND+104; native DestroyMenu/RemoveMenu; original _WOWCleanup | Update/withdraw incoming edges before freeing copied backing, include reentry and failed refresh, retain native USER lifetime authority and original cleanup selection. No second USER object manager. |
| Verification closure | Current real-CCPU lifecycle fixture plus older standalone original-resource/thunk fixtures | Migrate resource cases requiring a bound runtime into the connected harness; retain ownerdraw, MENUEX, nested invalid-template and resource callback-failure coverage. Do not add an unbound production fallback merely to keep an old fixture green. |

These rows come from source tracing, not new failing application runs. In
particular, wmdisp32.c lines1407--1460 and its table entries establish an
additional incoming-menu path, while wow_window_words_binding.c currently
refreshes only GetMenu(window) before DispatchClientMessage. This is a
concrete missing publication boundary, not evidence that every observed
application failure has the same cause. No batch build or acceptance is
claimed until implementation and independent review cover the whole table.

Batch implementation in progress (not built or accepted): the owned native
menu boundary now publishes valid WM_INITMENU/WM_INITMENUPOPUP wParam menus
and WM_MENUSELECT/WM_MENUCHAR lParam menus before the retained message
conversion. Both the ordinary WOW window callback and original-dialog callback
carrier select this boundary. Terminal/no-menu and synthetic invalid-handle
messages retain dispatch and LastError; observing a menu never assigns a
creator. This is shared-view publication, not a rewritten message thunk.

TrackPopupMenu and DrawMenuBar now select the same inherited-data-lock release
and finally-restoration discipline as SetMenu/Hilite. GetSystemMenu also uses
that native callout boundary. Popup cancellation still refreshes surviving
state because callbacks may mutate/destroy menus even when tracking returns
FALSE. Native USER continues to own tracking, drawing and system-menu creation.
No original source or guest image changes are introduced by these connections.

The real-CCPU fixture source now includes detached, initially unregistered
native menu trees delivered through all four message forms, borrowed ownership,
terminal/invalid message passthrough, and the older parser fixture's MENUEX,
empty/nonempty ownerdraw, unknown-version and repeated invalid nested-template
cases. These are test definitions only, not test results. Remaining batch work
includes incoming-reference retirement, callout/reentry acceptance, complete
resource-callback fixture migration and private-system-menu lifetime review.
No build, test run, publication or closure was performed for this partial edit.

Retirement source review additionally reaches canonical createw.c's terminal
non-child spmenu unlink before _DestroyMenu, and mnchange.c::MNFreeItem's
FreePopup followed by Unlock(spSubMenu). The page-domain retire boundary now
refuses ERROR_BUSY before any free if a copied non-child WND or ITEM still
points to the MENU. It scans the existing graph without a new reference
registry. A retained terminal WND clears only its non-child menu edge, matching
the reached terminal unlink; child IDs remain numeric data.

Completed live-tree publication now precedes dead-menu collection, and window
refresh publishes WND.spmenu before collection. Recursive native destruction
can invalidate a parent and child in arbitrary handle-index order: the copied
storage collector therefore retries referenced candidates only after another
candidate was actually released, refusing when no progress is possible. It
does not call native DestroyMenu, choose task owners, or replace original
cleanup. Borrowed-view process disposal reuses the same release ordering after
its existing whole-table owner preflight. Failure leaves referenced backing
intact rather than silently clearing a live edge. Fixture source now checks
WND and ITEM refusal preserves all fields, followed by ordinary unlink/release.

This is still unbuilt batch work. In particular, a native-destroyed menu still
reported by a live native WND cannot be declared equivalent merely because
the guard safely refuses reclamation. Callback failure propagation and this
native lifetime boundary remain part of the batch review, together with the
system-menu/private-copy obligations; no runtime success is claimed here.

The next source pass finds a terminal producer that must be in this batch:
wow_native_DestroyWindow previously used the generic native-call wrapper and
returned without collecting menus destroyed by native USER after the final
window callback. Its owned wrapper now retains the same begin/end scheduling
boundary and collects dead menu views after successful native completion,
before returning to guest-local queries. Unbound native passthrough remains
unchanged. Collecting at WM_NCDESTROY alone would be too early: canonical
createw.c releases spmenu/spmenuSys later. Automatic destruction inside other
native calls and failure propagation still require review; this edit is not
proof of complete terminal coverage.

System-menu ownership review must account for locks, not only allocation:
mnsys.c locks the private root into WND.spmenuSys; its returned submenu is
also retained by that tree. Original _WOWCleanup can mark a creator's menu
for destruction, but mndstry.c cannot physically release those locked objects
until native unlink/revert/window destruction. This supports leaving public
native system-menu lifetime with USER rather than assigning the observing
task as creator. It does not yet prove every cross-task or detached-submenu
case equivalent. There is no selected WOW SetSystemMenu API consumer; the
search covered retained wow32, owned wow32-dll and selected ntuser sources.

Fixture source now verifies repeated private-system-menu observation from
different task IDs does not change owner, and native revert removes both the
native handle and copied view. The observer runner requires this marker and
the new message-publication marker; neither has been run yet. The old
resource-callback fixture still needs a properly bound execution route without
overriding the product's W32 resource implementation for unrelated tests.

The resource fixture is now wired as a separate menu-resource case in the
existing lifecycle observer. It uses the real CCPU page-domain harness and
bound USER runtime, with controlled W32 resource callbacks linked only in
that test executable. The ordinary window-borrow executable retains production
resource callbacks. The controlled FindResource entry rejects use outside the
explicit resource test scope, so accidental unrelated use cannot silently pass.
Original resource clients/parser stay selected; native menu destruction in the
fixture now goes through the connected retirement boundary. Standalone unbound
execution explicitly fails instead of implying a supported production fallback.
The case requires both the sixteen resource-client cases and ownerdraw markers,
plus completion of the bound native menu case. This is not real guest-resource
acceptance and is not a test result: the new case has not run.

The observer also stops directly recompiling the lifecycle source for every
case: it requests its existing Ninja target so unchanged dependency inputs are
reused. This does not skip affected rebuilds or authorize running the unfinished
menu batch. The old thunk-conversion fixture's controlled guest-memory seam
still requires bound-harness migration; its earlier pass is not reused as
evidence for the current connected parser.

The thunk-conversion cases are now also selected by menu-resource while the
CCPU/USER runtime is bound. The existing extraction helper verifies the exact
retained and canonical wumenu.c/wres16.c hashes before emitting original bodies
under build/. In the fixture translation unit only, the extracted functions,
guest-memory input seam, allocation seam and pfnOut carrier have distinct names;
they cannot replace production symbols in the provider archive. The observer
requires all six original-thunk cases and zero outstanding controlled
allocations in addition to resource/ownerdraw completion. Unbound standalone
execution refuses. Output-path admission now explicitly requires repository
build/. These source/runner edits are not executed verification.

Independent review also keeps an important terminal-coverage gate open:
explicit DestroyWindow is not the only way native USER can destroy a window
or adjust menu state. Native default processing and dispatch can do so inside
other calls. The completed batch must prove the host-to-guest publication
boundary covers those returns, not add a succession of command-specific
destroy wrappers or assume a later unrelated menu API will collect them.
Existing wow_user_invoke_thunk is a shared original-dispatch scope, but adding
publication there requires preserving original exception/result semantics and
covering nested guest callbacks separately. No such broad behavior has been
implemented or implicitly approved as complete by the present narrow wrapper.

Shared-boundary tracing now distinguishes three paths rather than assuming
one callback gateway: W32Dispatch and W32TryCall invoke wow_user_invoke_thunk;
ordinary window/dialog callbacks use their owned publication carriers; tagged
WOW CallWindowProc uses call_previous_window_proc and directly calls the
registered pfnWowWndProcEx with NULL WW. The latter now performs the same
message-carried-menu publication without requiring a window binding or
changing its original NULL-WW subclass contract. Native/CPD targets remain
native passthrough. No mirror edit is needed for this missing edge.

Do not install menu policy in host_simulate: canonical nt_cpu.c also serves
BIOS/DEM recursive CPU calls, not just USER callbacks. Likewise, simply
returning zero from the common thunk scope on synchronization failure would
overwrite arbitrary original API result contracts. W32Dispatch writes the
returned value to AX/DX and W32TryCall leaves exception policy to W32Exception.
Those facts constrain a common publication solution; they do not justify
manufacturing a successful result or silently translating a bookkeeping
failure into a different API's error. Original WU32DefWindowProc and
WU32DispatchMessage still call native USER directly and remain named terminal
coverage edges for the batch review. No new build/test or publication occurred.

The batch fixture now exercises native popup tracking without mouse input:
its native WM_INITMENUPOPUP callback verifies the data lock is released,
changes a known item's checked state through the connected boundary and calls
EndMenu. The enclosing TrackPopupMenu must report cancellation, restore the
inherited lock and retain the published mutation. The ordinary CCPU observer
requires the cancellation/reentry/publication marker. This is a test definition,
not a successful run; the background runner's existing timeout remains binding.
The review specifically rejects treating FALSE as proof that no callback or
mutation occurred. No production API-result or exception-policy change has
been made at the common thunk boundary while terminal coverage is unresolved.

The subsequent source implementation replaces the temporary DestroyWindow-only
collector with synchronization at the existing typed native-call return scope.
Same-thread calls now retain their binding even if entered without the data
lock, so the return can publish and restore the original lock state. Original
cross-task handoff remains in that scope. Default window/dialog/MDI processing,
DispatchMessage and native/CPD CallWindowProc now use it; tagged WOW procedure
dispatch retains its original callback path. This does not alter W32Dispatch,
W32TryCall or the CPU entry boundary. A publication failure propagates through
the typed wrapper's existing failure result; native side effects are not rolled
back, and that failure case must not be described as native API equivalence.

GetSystemMenu, TrackPopupMenu, DrawMenuBar and SetMenu now share this same
return scope instead of maintaining separate callout-lock/partial-refresh
paths. The tracking fixture additionally changes a different detached menu
through native CheckMenuItem inside the callback. After cancellation its guest
query view must reflect that change, without a later WOW menu query to repair
it. This specifically distinguishes whole-return publication from refreshing
only the tracked root. These remain unbuilt candidate changes. Creation
rollback, all remaining callback-producing mutation entries, stale incoming
references and publication-failure behavior still require batch review before
the consolidated build; neither guest execution nor S2 completion is claimed.

The same scope now also covers the entire menu-mutation wrapper family,
including HiliteMenuItem's window-targeted owner-draw callout. It synchronizes
after completed native calls regardless of their success result; failed native
calls may already have entered callbacks. The runner requires the additional
detached_published marker rather than accepting the older tracked-root-only
case. Creation rollback is still a distinct boundary to review: native creation
can fail and destroy windows internally before its caller receives a handle.
Adding a DestroyWindow wrapper only at the explicit rollback line would not
cover that path. No builds/tests were launched for these intermediate edits.

Creation-boundary source review confirms canonical createw.c sends
WM_NCDESTROY at line1872, then removes/destroys spmenu and spmenuSys around
lines2086-2099. Both owned window and dialog creation now bracket the whole
native creation/rollback operation with the existing native-call scope. Their
finally blocks first withdraw failed enrollment and release creation hooks,
then complete return publication. Thus an internal native failure that never
returns an HWND is covered as well as explicit rollback. Failed publication
of a successful creation invokes the existing typed destruction boundary and
returns failure; this is not a claim that native side effects can be undone
under arbitrary allocation failure. Original guest and createw.c are unchanged.

The original-cleanup menu destruction leaf also refreshes surviving native
links before retiring copied objects; it no longer assumes a destroyed menu
has no live parent just because _WOWCleanup selected it. Task/creator selection
remains in original cleanup.c. This is source implementation evidence only.
Batch acceptance still needs explicit failed-creation/menu lifetime cases,
review of refresh failures and reused identities, then one consolidated build.

The real CCPU fixture now defines a failed-creation case using the owned
class/window gateways: a native logical WNDPROC publishes its attached menu
during WM_CREATE, then rejects creation with -1. Both initially-held and
initially-unheld USER lock modes must return no HWND, leave native window/menu
identities invalid, remove both projected handles and restore the original
lock state. The observer requires its distinct result/modes marker. This is
new test source, not a pass or a substitute for immutable-guest acceptance.

Queue retrieval is also a native callback-producing edge: the existing
two-pass PeekMessage path can dispatch sent messages without returning a
queued message. Both passes now use the same native-call return publication.
The helper separates native message availability from synchronization success,
so a failed publication cannot become an endless GetMessage empty-queue loop.
The original filter flags, between-pass yield and sleep ordering are retained.
No guest/mirror source or release image changed; consolidated execution has
not started.

Consolidated candidate acceptance started 2026-10-15T03:10:19Z, owned runner
PID26792, root build/M0-T447/S2/menu-batch-20261014. It rebuilds the mutable
worker/provider once, then runs window-borrow, menu-resource, menu-cleanup,
process-retirement, dialog-lifecycle, native-send, native-nested, task-lifecycle
and governance sequentially. Child identities, commands, start/deadline and
exit records are written in that root; build deadline1200s, each fixture600s,
governance120s. A failure stops the suite. This is candidate acceptance, not
publication or S2 closure; the broader original-WOW restoration remains open.

Before this run, source review removed unnecessary page-domain ITEM rebuilding
when all consumed fields and identities are unchanged. It compares against
the existing view after generation/submenu validation, without adding a cache
or dirty registry. The fixture requires unchanged publication to preserve its
ITEM backing address. Existing stale-generation and referenced-retirement
cases remain selected. Allocation failure after a committed native mutation
is still not native rollback equivalence; these gates must not overclaim it.

That run is terminal: suite.exit=1/build.exit=1, before any fixture execution.
Worker compilation/link and VdmTib verification passed. Provider compilation
reported C2011 in the window/dialog creation units: adding lifecycle.h already
imports wowuserp.h through cleanup_bindings.h, while both units also included
the original unguarded header directly. Removed only the now-redundant owned
includes/typedef wrappers; no change to the original ABI header. This correction
has not yet been rebuilt. Retain this failed run rather than overwrite its logs.

The corrected batch retry started 2026-10-15T03:12:19Z as runner PID27832,
root build/M0-T447/S2/menu-batch-r2, with the same selected cases, mutable
Ninja roots and per-stage deadlines. The prior runner was absent and its
suite.exit=1 was inspected before this retry; no overlapping build was started.
Read this retry's exit/results before treating the include correction as verified.

Retry r2 completed build.exit=0, then stopped suite.exit=1 at window-borrow.
The real CCPU fixture reached original menu creation but failed
original-resource-failure-cleanup with error1400. Source confirmation:
clmenu.c sets ERROR_INVALID_DATA before NtUserDestroyMenu rollback; the newly
shared call-begin queried a NULL window and leaked its invalid-window error
into successful menu destruction. The common begin now restores incoming
LastError after successful preparation on both scheduling branches, retaining
its own errors only on failure. No parser change or weakened assertion.

Retry r3 started 2026-10-15T03:15:10Z, runner PID13332, root
build/M0-T447/S2/menu-batch-r3, same case/deadline sequence. r2 was terminal
and PID27832 absent before restart. The parameterized owned runner preserves
each earlier output directory. Results are pending; no publication occurred.

r3 is now terminal, suite.exit=1. Build passed; window-borrow emitted all menu
markers, including detached callback mutation, native default/dispatch close
and both failed-creation lock modes. The executable then exited 0xC0000005
before native-class enrollment completion. Source review finds the later
internal-dialog fixture supplied runtime/thread/task but no lifecycle owner;
the new native-call begin dereferenced that NULL owner. Added the fixture's
single-task lifecycle carrier and an explicit production INVALID_STATE check,
not an unbound success fallback. The native-menu case now also asserts this
rejection leaves the caller unlocked. Crash attribution is source-backed;
the next full run must verify the repair, not just reuse menu markers.

r4 started 2026-10-15T03:18:06Z as PID5776, root
build/M0-T447/S2/menu-batch-r4, same case sequence/deadlines. r3's exit and
missing runner were checked before launch. Results pending; no publication.

r4 completed with suite.exit=1: menus and native-class/internal-control cases
passed, but the separate callback-borrow fixture's empty dialog was rejected
with ERROR_INVALID_STATE (not an access violation). Its hand-bound task also
lacked a lifecycle owner. Audited all runtime_bind sites in this CCPU fixture
and adapter WOW fixtures: full task/dialog/window suites already initialize
lifecycle; class-query/runtime-only fixtures do not invoke these callouts.
Added the remaining single-task carrier to callback-borrow; production's
missing-owner rejection remains unchanged. This corrects test prerequisites,
not an application workaround or a relaxed assertion.

r5 started 2026-10-15T03:20:30Z as PID18228, root
build/M0-T447/S2/menu-batch-r5, same cases/deadlines after r4 was terminal.
No new production edit or release accompanies this retry. Results pending.

r5 window-borrow completed with runner exit0 and fixture exit0, including
WOW_PAGE_DOMAIN_LIFECYCLE_OK, errors=0 native/internal-control/class cases,
normal/exception callback-borrow and nested dialog destruction. All newly
required menu markers passed in this same run. Runner PID18228 continued to
menu-resource; remaining cases and overall suite result are not yet accepted.
This proves the connected CCPU fixture scope, not SOL/WRITE initialization or
production Win16 execution.

r5 subsequently stopped at menu-resource linking (suite.exit=1), before that
fixture ran: unresolved _MBToWCSEx from the namespaced extracted LoadMenu
body. The runner had replaced the provider /Gz with /Gd for this translation
unit; original rtl/chartran.c and its unannotated prototype use the selected
provider /Gz ABI. Kept /Gz for the extracted original thunk fixture; its main
entry is explicitly __cdecl. No helper stub, alias or original-source edit.

r6 started 2026-10-15T03:24:29Z as PID8112, root
build/M0-T447/S2/menu-batch-r6. It continues menu-resource and remaining
cases with the same deadlines; SkipWindowBorrow explicitly reuses r5's full
pass because only the separate thunk fixture's compile option changed, not
its code or production inputs. r5 was terminal before this continuation.
Results pending; no production publication or overall acceptance claimed.

r6 menu-resource linked and ran with fixture exit0 and full CCPU markers,
but the observer rejected missing resource/thunk completion markers and stopped
suite.exit=1. The map proves both extracted fixture entries are linked and
_printf resolves to original-softpc-host-fixture-roots:fprt.obj, whose debug
output is not the captured CRT stream. Changed only these fixtures' diagnostic
prints to fprintf(stderr), matching the connected CCPU witness. Marker/count
requirements remain unchanged; exit0 alone is not accepted as their proof.

r7 started 2026-10-15T03:27:36Z as PID15504, root
build/M0-T447/S2/menu-batch-r7. It continues the same resource-and-later cases
with the same deadlines and retains r5's unaffected window-borrow pass. r6
was terminal before restart. Production source/build inputs are unchanged by
this diagnostic-output correction. Results pending; no release was made.

r7 menu-resource completed observer exit0 and fixture exit0. Captured stderr
proves resource_cases=16, parser_cases=7, invalid_repeats=64, ownerdraw errors=0,
and original thunk cases=6/errors=0/allocations=0, followed by the connected
CCPU lifecycle markers. The retained original parser/conversion bodies execute
under the bound runtime; controlled resource inputs remain fixture-only.
The runner continues to cleanup/lifecycle/message cases. This closes the
missing-marker verification gap, not the whole S2 or real-application gates.

r7 menu-cleanup and process-retirement also completed observer/fixture exit0.
Menu cleanup reports errors=0 with task-lifecycle native_direct=4; process
retirement reports errors=0 across task/object/class refusal, retry, event and
native-registration lifetime checks. These retain original cleanup selection
and verify the owned release binding rather than replacing it with silent
discard. Dialog and native messaging cases remain in the live sequential run.

r7 dialog-lifecycle and native-send completed observer/fixture exit0.
Dialog coverage reports creation rollback cases=4/errors=0, paired callback
cache with exceptions/errors=0, and init=2/terminal=2. Native-send receives and
returns result114 twice with lifecycle errors=0/native_direct=4. These confirm
the selected ordinary native-call regression, not all C01/C02 message contracts.
Native-nested, task-lifecycle and governance are still pending in this runner.

r7 is terminal with suite.exit=0: native-nested, task-lifecycle and governance
also passed. Nested send reports scheduled=1/owner=1 and return114; ordinary
task lifecycle reports no errors. Together with r5 window-borrow this completes
the selected menu-batch fixture suite on the current candidate sources. It
does not complete S2: real WINMINE/SOL/WRITE and other foundation obligations
remain open. Real-application observation is staged separately under
build/M0-T447/S2/menu-batch-product, using the existing accepted package plus
the tested host worker/provider and hash-verified unchanged guest companion.
No assets/release or installed-system publication is performed by that runner.

Product observation started 2026-10-15T03:36:19Z, runner PID28988, with a
900-second owned child deadline. Exact command/child identity and output
locations are recorded in that run root. The previous fixture runner was
terminal before this build-root user started. Application results pending.

Product observation is terminal with runner/observation exit0, but this is
not application acceptance. Four snapshots show WINMINE's visible Minesweeper
window. SOL still shows an Out of memory dialog; WRITE shows Not enough memory
and an invisible MSWRITE_MENU window. Observer target timeouts are observation
bounds, not successful application exits. The menu batch therefore does not
establish recovery of SOL/WRITE. No publication/closure follows these results.

### Next foundation batch: activate paired standard-class ownership

Source recheck confirms prepare and bind_cleanup still have no production
callers. The earlier initialization row deliberately deferred them until
native/internal control creation and retirement were connected; r5/r7 now
verify that prerequisite. Reuse those existing functions at lifecycle_initialize
before publishing runtime callbacks/initialized, with the supplied original
WOW callback. Prepare borrowed class metadata and the matching original-cleanup
restore table together. On failure withdraw unpublished metadata/object storage;
never RegisterClass or modify native control policy. Last-owner disposal already
uses forget_native. This consumes the admitted W02 prerequisite rather than
adding a second class table or changing immutable guest USER.

Candidate wiring is now in place; process-retirement verifies initialization
owns the public metadata and rejects duplicate prepare instead of preparing it
only in the test. Initialization-failure coverage, other class-list assumptions
and the remaining shared-window graph ownership still require source review
before this next batch is built. No new application run was started for it.

Paired teardown review found dispose previously removed borrowed public class
metadata before checking for remaining private/application classes. With real
startup metadata that makes refusal partially destructive. It now preflights
private/system lists and public owned/live-associated classes before releasing
menu or class views. The process-retirement fixture requires all nine borrowed
standard classes at startup, zero creator-task ownership, duplicate-prepare
rejection and unchanged public-list identity after object/private/live-control
refusals. This preserves retry state without replacing original task cleanup.
Source anchor: canonical client/clinit.c RW_RegisterControls is invoked from
client initialization; the existing adapter reuses walias.c classification and
native USER registrations rather than reimplementing control procedures.
This next batch remains unbuilt; initialization-failure and shared-graph
coverage are not waived by the prior menu-batch passes.

Startup failure coverage now uses the original walias table in the fixture
process only: temporarily replace the COMBOBOX descriptor name with a missing
native class, after the earlier BUTTON import can succeed. Restore the name
in finally. Require partial class/object storage withdrawal, no published
runtime callbacks, and successful initialization retry with the same owner.
The same failure must leave the cleanup restore table and scroll procedure
byte-for-byte unchanged. No product fault-injection switch, guest-memory
change or native-class replacement is introduced. These assertions are added
but not run; the complete dependency batch remains the next build unit.

Graph producer recheck: wow_cleanup_bindings.c build_tree deliberately
contracts native-only intermediate nodes for PseudoDestroyClassWindows and
reverses enrollment traversal into its temporary child lists. That tree is
valid only for its documented cleanup selection, not a source of sibling
order or parent links for original PMODE32 readers. Do not repurpose it as
the guest-visible graph or change its cleanup ownership to enroll foreign
windows. The staged publish_window_links still has no production caller;
native relation sampling and reference withdrawal must be connected together
before that capability can be claimed.

### Shared-window consumer closure (source recheck)

Canonical rtl/getset.c GetWindowData applies TestWindowProcess only in
GWL_WNDPROC; its GWL_WOWWORDS branch returns adwWOW without that restriction.
Current wow_user_private_access.c routes the latter through borrow_window,
wow_window_words_acquire and own_window, whose GWL_WNDPROC check therefore
narrows a different original contract. Merely removing own_window is unsafe:
GetPropW would then expose another process's pointer as a local binding.
Keep this protection on managed property/procedure operations. A sampled
shared view needs local retained words, not dereferencing foreign properties,
attaching a procedure, or assigning a cleanup task.

This is not solely a GetWindow traversal dependency. Original walias.c
FindPWW reads those words, classifies with GetClassName/GetStdClassNumber and
writes iClass/flState through SETWL. Original wmsg16.c ThunkMsg16 uses that
classification for messages at or above WM_USER; a missing PWW instead
selects class zero. Returning temporary all-zero words on every read would
discard the original cache and setter contract. Preserve FindPWW and the
original thunk tables rather than copying classification into the adapter.
This establishes a concrete missing consumer, not the cause of either
application's memory-error dialog.

The complete next group must therefore satisfy these boundaries together:

| Boundary | Required producer/consumer pairing |
| --- | --- |
| Managed WND | Existing WW borrow/property and task ownership remain authoritative; sampled data must not create a second owner. |
| Native-only WND | Publish a locally retained read view in the existing object domain, including its class/name and original classification words; no native property pointer, subclass or cleanup-task adoption. |
| Relationships | Sample actual parent, owner, first child and next sibling separately; resolve complete identities before exposing server-form edges. Cleanup's contracted tree is not an input. |
| Callback entry/return | Refresh before original readers execute and after native mutations, not merely at creation. Keep current callback borrows valid across nested destruction. |
| Retirement | Refresh/withdraw incoming references before freeing view storage; preserve retained callback backing and class lifetime. Reused handles cannot inherit previous words or relationships. |
| Startup/disposal | Prepare borrowed standard metadata before publication; initialization refusal and final-disposal refusal retain retryable state. |

Original kernel/createw.c also refines the terminal ordering: xxxDestroyWindow
unlinks from the parent's child chain before xxxFreeWindow sends WM_NCDESTROY,
but deliberately retains the owner through that callback. Thus retirement
cannot indiscriminately zero all relationships before the terminal callback,
or wait until after freeing WND storage to remove incoming edges. The existing
managed NCDESTROY detach must be paired with this synchronization, not simply
left unchanged when publish_window_links becomes live. No new build/test is
started until that pairing and sampled-word lifetime are implemented/reviewed.

The existing object binding now contains a call-local native relationship
sampler, not a second registry or tree policy. It uses GA_PARENT separately
from GW_OWNER and retains native sibling order. Desktop parent/owner/next are
terminal; child still comes from native USER. It refuses observed identity/
style/relation changes and writes the output only after successful sampling.
Public queries cannot provide NT4's desktop-wide lock: no global atomicity is
claimed, and the graph publisher must still resolve each complete identity.
No guest publication or lifecycle owner is installed by this staged function.

The process-retirement fixture adds owned-popup versus child-parent checks,
actual sibling reorder, reparenting, desktop termination and unchanged output
on invalid input. These are native fixtures, not guest graph acceptance; they
remain unrun with the rest of the next batch. Full view enrollment, private
word borrowing, production synchronization and paired retirement are still
required before building or reporting the graph as connected.

The sampler now collects the complete reachable next/parent/child/owner
closure from explicit roots. Its low-word traversal index and dynamically
grown rows are call-local only; duplicate full identities merge, conflicting
generations fail, and all temporary allocations are released on failure.
It resamples relations before returning, detects observed changes with
ERROR_RETRY, and never returns a partial closure or loops indefinitely to
obtain one. This remains a public-USER sample, not a claim of an atomic NT4
server-locked snapshot. Publication still requires the paired existing-view,
borrowed-view and terminal-reference rules described above.

The relationship fixture now creates its own non-visible desktop, restores
the prior thread desktop and closes the owned desktop after destroying its
windows. It checks duplicate roots, complete reachability after reparenting,
preserved native ownership and unchanged outputs on failed collection. No
SwitchDesktop/computer use or interaction with user windows is needed. Code
and assertions remain unbuilt pending the full group; no runtime result is
claimed from this review.

Live WW audit found an adjacent producer gap within the same view group:
wow_window_dispatch_bound already refreshes style/ExStyle/hModule before
guest callbacks, but wow_user_borrow_window returned creation-time fields
when no such callback preceded a query. Original wcntl32.c, wmsgcb.c and
wmsglb.c consume dwStyle to select owner-draw/string conversion. The original
wowuserp.h WW fields alias live WND storage, not a creation snapshot.

Extracted that existing refresh into wow_window_words_refresh and reused it
from dispatch and the private getter. It samples native fields before writes,
checks the borrowed property identity under the existing publication lock,
preserves original classification/procedure words, and does not reacquire
native window ownership or alter original thunk selection. Existing thunk
scope tests now mutate native style/ExStyle without any WOW callback and
require the next borrowed read to see them at the same address, while nested
destruction still retains its original words. Existing poisoned-lifetime
negative paths remain applicable. This is candidate code, not tested evidence;
the complete window-view group remains the next build unit.

### Original short-handle query recovery

Source tracing the reverse direction found a distinct contract mismatch:
canonical client/handles.c GetFullUserHandle returns the input WORD when
the shared-table row is absent/non-window. Its comment explicitly preserves
this for DDE termination after peer destruction. The registered provider
instead called the strict managed-window lookup and returned zero. Original
FULLHWND32 consumers in wmsg16.c/wumsg.c therefore lost an intended value.

OPENNT-HOST-069 imports only this unchanged function under its original path.
Source revision/hash and excluded DDEML manager are registered in the mirror
README. The finite binding supplies the existing TYPE/uniqueness table and
original 16-bit index mask; no lookup policy is reimplemented. The callback
now uses the retained body. WOWEXEC registration explicitly keeps strict
lookup, as do menu attachment callers: an intentionally stale DDE value is
not a live registration identity. The prior callback replacement is removed,
not left as an alternate route. The shared-view enrollment gap remains open.

Read-only source comparison after CRLF normalization found both function
bodies and established exact equality (775 characters). Canonical user.h
defines HMINDEXBITS as 0x0000FFFF, matching the WORD-index binding. Fixture
assertions cover live type/uniqueness, freed/non-window rows, zero/FFFF and
the distinction from strict lookup. They are unrun until the integrated
batch. This is one host-source subset addition with zero original-body edits,
not a guest import/rebuild, runtime pass or release.

### Retirement review: do not insert unowned WNDs into cleanup rows

Review of retained cleanup.c establishes a concrete constraint on the next
connection. Its task-cleanup TYPE_WINDOW branch dereferences pOwner as a
PTHREADINFO to test TIF_16BIT/ptdb; a NULL-owner sampled window in that table
would fault. Giving it the current task or a fabricated native thread would
instead invent ownership. Neither is an acceptable way to expose native
neighbors. The earlier read-view requirement must retain the distinction
between the existing guest page-domain projections and managed cleanup rows.
No original cleanup conditional is added to accommodate a fabricated row.

The bounded integration direction is therefore to retain sampled WND/CLS
storage with the existing page-domain producer and expose its queried words
under a synchronous borrow, without adding a TYPE_WINDOW cleanup row. The
three original classification/procedure words must persist with that view;
a per-call zeroed substitute is not sufficient. Managed WW/property borrows
remain unchanged. Retirement must account for these outstanding borrowed
views before freeing WND and CLS. The full-handle query also needs that
complete shared-view input, not just managed cleanup rows; recovering its
original fallback does not by itself complete external-window resolution.
This is a dependency/design result, not a claim that those connections exist.

The finite sampled-data fields have now been checked against original WOW32
consumers: wcntl32/wmsgcb/wmsglb use dwStyle for owner-draw/string conversion;
wmsg16/wmdisp32 use dwExStyle for MDI creation conversion; wmsgem/wmdisp32
use hInstance. No active consumer of dwUserSrvState/state2 was found in the
WOW32 source search. This does not authorize inventing the latter flags or
loosening original procedure/process privacy. Storage, immutable-guest reader
layout and these host consumers must agree before activating the graph.

### Shared-view allocation borrowing

The page-domain carrier now tracks only outstanding synchronous WND borrows
by their existing allocation address. It introduces no native object, handle
ID, task owner or second handle table. Releasing publication removes the
handle entry as before, while outstanding references retain its old WND
backing. A new generation can occupy that slot without aliasing the old
allocation. The existing managed WW transfer has a paired retained-owner
release, so either release order (WW first or shared-view first) defers free
until both owners finish. Class retirement also checks these retained WNDs;
worker teardown drains retired allocations as its existing exceptional
backstop. Original client/guest algorithms remain unchanged.

The native WW final-release caller now uses that paired worker entry rather
than directly freeing transferred VDM storage. The worker export list includes
the three borrow/release functions. CCPU fixture assertions cover repeated
borrow, stale-generation refusal, retirement while borrowed, retained-owner
release before the last reader, slot reuse with distinct backing, final free
and duplicate reader-release refusal. They remain unrun. The sampled external
query/word consumer is not yet connected, and this infrastructure alone does
not establish the complete graph or application acceptance.

Follow-up release review removes the unchecked retained-address free path:
even an exclusively managed WW transfer now records its retained allocation
owner before unpublishing the handle. Unknown or duplicate release tokens are
rejected instead of being passed to VdmFreeVirtualMemory. Retained-class
checks cover both release orders and the managed-only transfer.

The three original WW words have finite read/exchange transport at their
original WND offset144, asserted against the retained user.h/wowuserp.h
layout. Reads require an outstanding shared borrow and remain valid after
retirement; setters additionally require a live publication. Neither API
classifies a control or supplies a guest procedure. Native style/module values
remain host samples, not native pointer writes into guest WND storage.
The CCPU fixture and required observer marker now cover persistent words,
retired read/set distinction, bounds, class retention, both release orders
and duplicate release. These additions are still unrun; original FindPWW
integration and native-view enrollment are the remaining consumer side of
this same batch, not deferred acceptance waivers.

### Original FindPWW consumer connection

The private WW getter now keeps its existing managed-property route, and
only for a missing/foreign property considers an already published shared
WND. Other failures (including allocation/invalid scope) cannot silently
fall back. Resolve short/full identity against the page-domain publication,
sample native style/ExStyle/hInstance into host-only WW fields, and retain
the shared backing until the outermost consuming scope leaves. Nested scopes
reuse the same live snapshot; no persistent host classification registry or
foreign property access is introduced.

Private SETWL now pairs with that getter: managed words retain their existing
InterlockedExchange; sampled words use the bounded shared-WND exchange and
update the same borrowed snapshot. Original walias.c FindPWW still selects
GetStdClassNumber and sets iClass/flState; no classification body is copied
or bypassed. Original procedure/process privacy is unchanged. This only
connects consumers for published views: automatic external-view production,
relationship publication and mutation/retirement coordination remain open.

The CCPU unowned-view case now calls the retained FindPWW against its sampled
foreign desktop record, verifies classification is stored at WND+144, checks
nested pointer identity and native attributes, retires publication inside
the outer borrow, and requires class retention until that borrow ends. No
native window is destroyed or subclassed. These assertions are added but
not run; the whole batch remains unbuilt and unpublished.

### Owner-requested integrated checkpoint, 2026-10-14

This run supersedes the preceding unbuilt/unrun status for the exercised
inputs, not the explicitly unconnected producer/mutation boundaries. The
owner clarified that group-level verification must not be postponed until
every remaining integration is finished. No production source changed during
this checkpoint, and no guest image was modified or rebuilt.

Procedure: background runner `build/M0-T447/S2/menu-batch-r2/run.ps1`
with `-OutputName menu-batch-r10`; after its fail-fast window-borrow result,
background `menu-batch-r10/run-remaining.ps1` ran eight independent cases
without masking failures. Root runner PIDs were15824 and31684. Exact child
PIDs, UTC start/deadline, commands and exits are in each `*.process.json` and
`*.exit`. Build deadline1200 seconds; case deadline600 seconds; fixture
execution has its existing60-second limit. Sources remained frozen during
the suite. Earlier r8/r9 stopped during generation for missing explicit Node
path and legacy PowerShell Get-FileHash availability; they are not code-test
results. r10 uses existing PowerShell7 and an explicit installed Node path.

Both affected production host images built successfully using the existing
x86 MSVC /MT /O2 CCPU40 caches in `build/M0-T445/S6/r003` and `provider`.
The worker graph was regenerated for the added private exports; WOW32 was
then regenerated and linked against that worker import library.

| Case | Current result |
| --- | --- |
| window-borrow | Failed: fixture exit -1073741819 (0xC0000005), before WOW_UNOWNED_VIEW_CCPU marker. |
| menu-resource | Failed: same exit and last completed native-creation marker; includes the shared window fixture. |
| menu-cleanup | Passed: WOW_MENU_CLEANUP errors=0 and observer success. |
| process-retirement | Passed: startup rollback, cleanup atomicity, refusal/retry and native ownership checks. |
| dialog-lifecycle | Failed:25 assertions, init=0 and terminal=0. |
| native-send | Passed: result114, lifecycle errors=0. |
| native-nested | Passed: scheduled=1 owner=1, lifecycle errors=0. |
| task-lifecycle | Passed: errors=0 native_direct=4. |
| thunk-scope | Failed: fixture exit -1073741819 (0xC0000005). |

The first two cases reached passing markers for shared borrow retention/words,
menu creators/resources/messages/system menus/tracking/rollback/cleanup and
native control creation before crashing. Those markers do not pass either
whole case. Source inspection identifies the dialog fixture's single
published_index mock as incompatible with newly enrolled internal controls;
this is a concrete test-boundary defect, not proof that production dialogs
pass. The access violations remain unattributed pending exception evidence.

Follow-up: resolve this bounded failure group and repeat affected acceptance
before adding more connections. Do not infer SOL/WRITE improvement from host
fixtures. No real-application rerun, DOS17 acceptance, coherent release,
commit/push or S2 closure is claimed by this checkpoint.

### Failure attribution and grouped re-verification

The r10 diagnostic observer (owned PID30828,60-second per-target deadline)
replayed the already built window-borrow and thunk-scope fixtures without
changing their exception disposition. Its source and logs are retained under
the same build root. Windows Application1000 records independently identify
the original failures. Mapping exception/stack addresses against fixture.map
places the window failure in class_is_referenced -> c_sas_dw_at -> phyR,
called by wow_class_words_forget_native, before the unowned-view case itself.
This refines the preceding marker-based location rather than attributing the
failure to the new FindPWW consumer.

Three test-contract corrections form one batch:

- The earlier geometry case still directly called VdmFreeVirtualMemory on
  retained WND backing. It bypassed the paired owner release and left a
  borrow record pointing at freed memory. Use release_retained_window, as
  the production WW final-release caller already does. Later class scans
  then exercise the actual retained-object contract rather than stale test
  bookkeeping. Other remaining direct frees are ordinary allocations, not
  transferred WND backing.
- The thunk fixture supplied LPFNW32 callbacks compiled /Gd, while the selected
  production header/profile and provider inherit /Gz. Its first thunk call
  corrupted the stack (exception replay shows execution at an unmapped address
  returning to fixture main). Compile this fixture with the production /Gz
  command and make incompatible-pointer diagnostics errors. Preserve explicit
  __cdecl main. No original header or production calling convention changes.
- The dialog's explicit page-domain mock admitted only one published HWND.
  Newly connected internal controls legitimately publish more than one. Track
  full HWND values per original index and a live count in the test, rejecting
  duplicate publication and unknown retirement. Retain exact per-window menu
  assertions and zero-live-object checks after rollback/destruction; do not
  remove the negative/callback assertions or pretend this mock is guest proof.

Background `build/M0-T447/S2/menu-batch-r11/run.ps1` (PID3988) reran all four
failed cases together. Each exited0, and suite.exit=0. Window/resource cases
now pass through the previously unreached unowned shared view, nine standard
classes, dialog cache and final page-domain lifecycle markers. Dialog reports
errors=0 init=2 terminal=2 and exception=1; thunk reports errors=0 calls=4
exception=1. Its unchanged production inputs are recorded in
unchanged-product-inputs.json: NTVDM SHA256
D07FED81F6FCE5E34EAD5A061413AA163C85E764F18DE8C9C9D912FF0F67F4FF;
WOW32 SHA25646F26846027D23440D2C1109D9FD655D1CF77FE8E51784B0434C166BCA0A1F36.

The five r10 passing cases are unaffected: no production source changed and
the changed /Gz selection applies only to thunk-scope; the other changes are
confined to the two failing fixture sources. Thus the nine-case group has
passing evidence across r10/r11, not a claim that all ran under r11. This
repair adds no original-source/guest diff. Automatic external-view discovery,
complete relationship/mutation wiring and real application acceptance remain
open; this is not a published delivery or S2 completion.

### Real application observation after the connected startup group

Question: does the fixture-verified menu/standard-class startup group change
the actual independent application initialization frontiers? This is grouped
acceptance of connected code, not a repeated trace-based patch search.

Background runner `build/M0-T447/S2/menu-batch-product-r2/run.ps1` PID32992
launched observation PID7748 with a900-second deadline. Both terminated with
exit0. The retained verify-callback-package.ps1 stages the accepted T446
package, validates release identities, replaces only the tested NTVDM/WOW32,
checks/copies the immutable original WINSPOOL companion, and uses an owned
short physical stage plus an unswitched private desktop. Each application
has a20-second observation window with4/8/12/16-second window snapshots.
No user desktop interaction, guest compilation or guest-byte patch is used.

The current system32 manifest versus the preceding menu-batch-product manifest
differs only in NTVDM/WOW32 hashes, now the exact r11 hashes above. Other
system32 entries, including guest modules, match. All three applications
were run separately, with owned cleanup between cases. Final process inspection
found no remaining run16/ntvdm/ntsrv processes. Forced observer cleanup is not
proof of normal application exit.

| Application | Previous menu-batch-product | Current product-r2 observation |
| --- | --- | --- |
| WINMINE | Visible Minesweeper main window | Visible Minesweeper main window; no gameplay claim. |
| SOL | Out of memory dialog | Still Out of memory; not functional acceptance. |
| WRITE | Not enough memory dialog, MSWRITE_MENU hidden | MSWRITE_MENU visible with Write - (Untitled), visible MSWRITE_DOC/MSWRITE_PAGEINFO and two ScrollBar windows. No memory-error dialog in the captured snapshots. |

The WRITE improvement is a new required startup frontier, not proof of
typing, menu actions, saving, drawing fidelity or normal retirement. Its worker
PID23736 was sampled alive at4062ms and subsequent scheduled samples; the
application remained running until the observer deadline. A diagnostic
timeout does not count as application success or a natural exit. Exact logs
are product-r2/frontiers/restored-{winmine,sol,write}-windows.txt, the paired
launcher logs and candidate-hashes.json. This establishes a batch-level
improvement without claiming any one changed function as the sole cause.

Continue the already-source-proved external shared-view/relationship closure;
preserve this WRITE frontier in subsequent changes. SOL's error remains
unattributed and cannot be called genuine heap exhaustion from its wording.
No assets/release or configured system32 publication, DOS17, commit/push or
S2 closure has occurred in this observation.

### Owner-directed S2 P1 baseline delivery

The owner directs build/test/publication/commit/push before further wiring.
Freeze the above r10/r11 and product-r2 production inputs; this delivery does
not close S2 or claim automatic external-window graph integration. The
observe/forget/collect observed-window helpers remain dormant, not a proved
production ownership path. Their missing final-borrow collection/promotion
and complete mutation wiring remain follow-up work, not runtime acceptance.

Background delivery-r1/run.ps1 PID26380 supervised checks PID8976 with an
1800-second deadline; runner.exit and checks.exit are0. Exact entrypoints:

- tests/observation/verify-wow-mine-gameplay.ps1 with the sealed product-r2
  package, retained S5 observer, provider/msvc-x86.cmd and WindowStyle
  Off/Win16/All. All three prove flag/counter change, F2 reset, opening a
  cell, WM_CLOSE and normal zero-status run16 completion. This preserves
  historical gameplay, not merely window creation.
- tools/audit/Verify-CommandExitStatus.ps1 with OrdinaryFrontend, retained
  S5 observer/G7.COM, private physical stage, milestone input and separate
  Console/Window input modes. Both summaries contain17 expected=actual
  outcomes and captured guest-output assertions, including EDIT and MEM.
  Nonzero expected application statuses remain exact, not forced to zero.
- tools/governance/Verify-DocumentationGovernance.ps1 passes. No production
  source changed after the r10 build or the r11 fixture corrections.

The old Product-suite frontier matcher requires WRITE's historical error
dialog literally, so it is not reused as an improvement acceptance test.
Independent product-r2 observations above establish each actual frontier;
the unchanged DOS matrix and stricter existing gameplay test provide the
remaining per-P runtime gates. SOL failure remains explicitly non-pass.

Background publication runner PID30436 supervised PID28972 with a180-second
deadline. Publication and runner both exit0. Exact candidate is
build/M0-T447/S2/menu-batch-product-r2/package/system32. All13 images match
between candidate, assets/release and O:/winnt/system32; only NTVDM/WOW32
change. delivery-r1/publication retains27 prior files, backups.json,
verified-manifest.json and original-companion.json. No live installation
process needed termination. The missing WINSPOOL.EXE is copied from the
verified original companion, SHA256
E4E787340F124476090ED0F22837D7D7ACAB37FD7BBE49DB46CAE9AC4944D535;
it is neither recompiled nor patched, and its original ExtTextOut limitation
is not declared fixed. Other guest files/configuration are untouched.

Second-role diff review covers callback lock restoration, menu creator versus
observer ownership, parent-before-child retirement, rollback, borrow storage,
process teardown ordering, fixture ABI and full-handle fallback. The only
new original-host subset is OPENNT-HOST-069, unchanged GetFullUserHandle;
original function-body edits and guest source/image edits are zero. Remaining
graph/semantic gaps stay open. This is a usable incremental baseline, not
SOL acceptance, WRITE editing/saving acceptance or full WOW recovery.

### Post-P1 source review: shared-view integration must precede activation

P1 is32876c415, pushed to origin/main; both release locations and the clean
worktree were verified. This next review changes no runtime or guest image.

Re-read canonical rtl/wow.c GetPrevPwnd, _GetWindow, _GetParent, _IsChild and
_IsWindowVisible alongside the current page-domain producer. These original
queries traverse copied links directly; they need not invoke a WOW32 thunk.
_IsWindowVisible follows parents until FNID_DESKTOP, and does not accept a
missing parent as a valid visible-window terminal. Therefore refreshing only
the named host query APIs, or only at thunk entry, is insufficient. Do not
replace these existing guest algorithms with host query wrappers. Publish
their data at every applicable return-to-guest boundary instead.

The concrete integration obligations are now:

| Boundary | Current evidence | Required connected behavior |
| --- | --- | --- |
| Identity/root | create_desktop_root reserves synthetic handle00010001/index1; native sampler returns GetDesktopWindow's real identity; publish_handle rejects index1. | Resolve the existing root consistently in both graph edges and handle conversion; never publish native desktop as an ordinary observed node or invent a second root. Audit possible native low-word collision before selecting the binding. |
| Projection versus cleanup | guest_windows contains shared backing; objects.entries is task cleanup ownership. Original cleanup dereferences TYPE_WINDOW pOwner. | Keep observed windows out of task cleanup. Full-handle conversion must consume the complete shared-view identity domain while strict WOWEXEC ownership lookup stays strict. |
| Creation and promotion | observed publication occupies guest_windows[index]; managed publish_handle rejects any occupied slot. | Observed-to-managed transition must preserve the same live identity and outstanding words/borrows, replace the class association safely and reject a different generation. Merely calling observe before the existing creator would break creation. |
| Callback and native return | Window, dialog, tagged CallWindowProc and hook paths enter task/borrow scopes; original CallBack16 ultimately calls host_simulate. | Review each guest-capable boundary, including non-window hooks and implicit native callbacks. Publish before the immutable guest can traverse relations, not only after a native operation completes. CPU mode restoration alone is not a USER graph boundary. |
| Terminal unlink | createw.c unlinks from parent before WM_NCDESTROY; preserves owner during that message; clears owner/child and substitutes desktop parent only afterward. | Publish native intermediate links without normalizing them to a guessed symmetric tree; retain callback backing through nested destruction and pair final withdrawal/release. |
| Observed class release | forget_observed_window collects unreferenced classes, but a retained WND borrow postpones eligibility; final release currently does not trigger collection. | Complete a paired final-class collection boundary before enabling discovery. Do not mistake process-exit reclamation for completed per-view retirement. |

Exact original teardown references are kernel/createw.c lines1558--1569,
1584--1592,1750--1764,1872 and2310--2338 at the pinned revision. They show
why owner cannot be cleared at WM_DESTROY entry and why a borrowed retired
WND can still need its class. Current producers reviewed are
mvdm_softpc_wow_page_domain_publish_handle/retire_handle,
observe_window/forget_observed_window/release_window_backing,
wow_user_window_publish/retire and wow_user_borrow_enter/leave.

The current native sample_graph fixture proves a call-local closure on an
isolated desktop; the CCPU links fixture proves a manually supplied copied
graph. Neither proves the missing producer-to-consumer wiring above. The
unowned-view test likewise does not call observe_window; its prior pass must
not be reused as automatic-discovery acceptance.

Next build unit remains the complete shared-view group, not six independently
published fixes. Add assertions for desktop mapping, observed-to-managed
promotion, native handle reuse, callback-time relation reads, owner retained
through WM_NCDESTROY, final-borrow class release and failure rollback, using
the existing real CCPU fixture. Preserve P1 gameplay/DOS17/WRITE startup.
No guest rebuild, new scheduler or copied replacement traversal algorithm is
authorized by this review. Root mapping and guest-resume synchronization are
not yet implementation-complete; do not activate the dormant helpers early.

### Shared-view candidate: native desktop identity

Read-only confirmation in USER16 k/daytona/wowasmk.asm GetDesktopWindow
shows it follows CLIENTINFO.pDeskInfo->spwnd, rebases, then returns WND.head.h;
it does not require handle00010001 or slot1. The prior synthetic identity is
therefore removed from the owned page-domain carrier. Domain initialization
now samples GetDesktopWindow once, publishes that identity at its real low-word
slot, and all window/menu retirement and lookup guards reserve that actual
slot. Shared backing stays guest-linear; no native pointer enters the WND.
Existing root storage and FNID_DESKTOP remain, with no second translation
table or changes to the original getter, handle algorithm or guest module.

The CCPU fixture follows the actual root slot, asserts duplicate publication
and retirement rejection, and verifies slot1 is ordinary when it is not the
desktop. Relationship fixtures now use that same native root identity. The
old unowned-view test fabricated a second desktop WND; it now checks the
foreign desktop as the real root and exercises ordinary unowned projection
with a separate native STATIC window. Its marker explicitly distinguishes
foreign-root identity from local unowned borrowing. This is not yet proof of
ordinary foreign-process window borrowing; add that coverage with the full
discovery group, rather than relabel the local window as foreign.

No build/test/publication is performed for this partial edit. Promotion,
complete shared-table handle lookup, root class/query fields, callback-return
synchronization and final observed-class collection remain in the same
uncompleted build unit. P1 release images are unchanged. The externally
modified assets/winnt.zip is preserved and excluded from these changes.

### Shared-view candidate: observed-to-managed backing handoff

The existing publish_handle boundary now distinguishes an observed class
projection from an already managed WND. It permits adoption only when the
full native HWND, supplied index/uniqueness and retained WND.head.h agree,
and the replacement class is an existing non-observed allocation. Native
geometry sampling precedes writes. The same WND allocation and relationships
survive; its three stored WOW words and outstanding backing borrows are not
reinitialized. Only the existing managed caller installs cleanup ownership.
Duplicate managed publication, stale uniqueness and an observed replacement
class are rejected. The original guest algorithms and native HWND remain
unchanged. The superseded query-class descriptor stays in the existing
allocation list until the observed-class sweep can release it; no second
allocation registry is introduced.

The real CCPU fixture now calls observe_window itself, checks idempotent
observation, borrows the WND, writes a sentinel WOW word, rejects stale and
invalid-class adoption, adopts the identical window, checks retained backing
and words, rejects observer withdrawal/duplicate adoption, then verifies
class guarding and exact guest free-memory recovery after final release.
This supplements the earlier manually published view case, rather than
calling that older case automatic-discovery proof. The verifier requires a
separate WOW_VIEW_PROMOTION_CCPU marker with zero errors.

These edits remain unbuilt parts of the same integration group. The managed
host WW versus existing sampled scope representation still needs its caller
review before discovery is activated; this backing test alone cannot prove
that higher-level handoff. Final observed-class collection, complete handle
lookup, callback publication and native destruction ordering remain open.

### Shared-view candidate: separate withdrawal and collection completion

Source review of selected original nt_mem.c VdmFreeVirtualMemory confirms a
real STATUS_UNABLE_TO_DECOMMIT_VM return when VirtualFree fails, in addition
to invalid-allocation failures. Collection cannot therefore be treated as an
infallible suffix to a consumed reference. The former observer withdrawal
returned `retire_handle && collect_observed_classes`: collection failure could
report failure after withdrawal had already succeeded, although retrying that
same withdrawal necessarily fails. The owned observer boundary now reports
withdrawal only. Its header requires explicit collection after withdrawal and
the final borrow release; failed class frees remain owned by the existing
allocation list. No extra retry registry, original allocator edit or guest
change is introduced. Production discovery remains disabled until its caller
implements that separate result/cleanup boundary.

Added a real-CCPU fixture case for two simultaneous borrows of an automatically
observed native STATIC window. It destroys the native window, withdraws the
view, rejects duplicate withdrawal, sweeps while each borrow still protects
CLS, reads retained words, releases the final borrow, rejects double release,
then sweeps twice and requires exact free-memory recovery. The verifier now
requires WOW_VIEW_RETIREMENT_CCPU with zero errors. This is unbuilt candidate
coverage, not a passing result or injected allocation-failure proof.

Callback review also confirms that the original nt_cpu.c host_simulate wrapper
is shared with BIOS/DPMI consumers, while CPU_40_STYLE header callers can use
the vector macro. It is not a suitable universal USER synchronization hook:
instrumenting just that external wrapper would miss macro callers and burden
non-USER execution. Existing WOW thunk, hook and callback boundaries remain
the integration owners to inspect. No new CPU-loop or mirror hook was added.

### Shared-view candidate: original full-handle query reads shared identity

The P1 facade pointed original GetFullUserHandle at objects.entries, which is
the managed cleanup table. An observed native window and the desktop can have
a valid shared HANDLEENTRY while that ownership table correctly remains empty.
Source audit therefore rejects populating fake cleanup rows as a remedy.

OPENNT-HOST-070 changes one original executable line: acquiring the selected
entry address becomes WowSharedHandleEntry(index). The original bounds/type,
uniqueness reconstruction and missing/dead-handle fallback remain unchanged.
The owned facade reads one metadata DWORD from the actual CCPU shared table,
decodes it into a synchronous local entry and invokes the retained body. It
never copies phead/pOwner or inserts the entry into cleanup. A failed carrier
read returns ERROR_INVALID_STATE/zero before invoking the algorithm; it is
not reported as the original successful short-handle fallback. The worker
export list includes the new numeric metadata read. Strict WOWEXEC ownership
lookup remains untouched.

Alternatives reviewed: an unchanged body over a lease of the entire table
would copy 1 MiB per query through the existing copy-backed guest-memory seam;
a persistent duplicate query table would introduce another synchronization
owner. Rewriting the original predicate would duplicate connectable logic.
The one-line, registered storage accessor avoids all three. Same canonical
source provenance as OPENNT-HOST-069; zero guest edits. Original function-body
identity assertions used for P1 must not be reused unchanged for this candidate.

The process-retirement fixture now explicitly mocks the shared metadata source,
not cleanup entries; it covers free/live/non-window/retired and read failure.
The real CCPU observation-retirement case checks live observed and desktop
conversion with an empty cleanup table, strict ownership rejection, then
short-handle fallback after withdrawal. No test has run yet. Production
discovery, callback synchronization and complete observed-class finalization
remain open in the same build unit; publication remains P1.

### Shared-view audit: WW alias coherence is a prerequisite, not a copy fix

Question: does preserving the observed WND allocation also preserve the
original PWW alias contract when managed enrollment occurs inside a borrow?
Source inspection says no. Current wow_user_borrow_window first tries the
managed property, and only otherwise searches enclosing sampled borrows.
An outer call can therefore retain &sample->words while a nested call returns
&binding->words for the identical HWND. The managed SETWL path changes only
binding->words via InterlockedExchange; the sampled path changes the copied
guest WND and sample->words. Neither path updates the other existing alias.
Managed publish_owner also does not publish its input adwWOW into that guest
WND. Stable backing/handle identity alone does not fix these three views.

Original evidence, canonical revision already pinned above:

- kernel/createw.c lines229--234 copies lpWOW into pwnd->adwWOW, before
  HCBT_CREATEWND at line394. The caller's initial WOW fields are authoritative
  creation data, not optional values to overwrite with a discovery snapshot.
- wow32/walias.c FindPWW keeps its returned PWW across SETWL and returns that
  same alias. wow32/wumsg.c SetFakeDialogClass also retains PWW while writing
  classification/procedure words through SETWL. Those routines must continue
  to see the same storage semantics; replacing these original algorithms with
  copied classification logic would violate the recovery objective.
- Current wow_window_dispatch_bound directly passes binding->words.adwWOW to
  retained DispatchClientMessage. Correcting the private getter alone would
  leave this callback consumer divergent.

This refines the earlier promotion candidate: preserving backing and words
is valid for the low-level storage handoff test, but is not acceptance of the
complete creation contract. Do not make guest copied words override the
creator's lpWOW, reject a valid promotion merely because an outer alias exists,
or solve it by adding a persistent per-HWND shadow registry. The next binding
change must carry one retained WW identity through observation/enrollment,
private getters/setters and dispatch, then publish its numeric words at the
guest boundary. Existing scope borrows must retain that same storage after
window retirement. No new original-source or guest edits have been made by
this audit; discovery must remain inactive until this dependency is connected.

Required integrated regression is now explicit: acquire an observed PWW in
an outer scope; enroll the same native window with creator-supplied WOW words;
query/set inside a nested scope; verify outer PWW, nested PWW, callback input
and copied guest fields agree; destroy in the nested scope, retain the outer
alias through return, and release the final storage exactly once. Include
creation veto/rollback without inventing a second classification policy.

### Shared-view candidate: reuse the existing WW binding on enrollment

Removed the separate WW value from wow_sampled_words. A sampled scope now
owns one reference to an unpublished wow_window_words_binding, alongside its
existing guest-backing borrow. Nested scopes find that same storage. Attachment
looks for this existing scoped binding before allocating; only successful
property publication takes its own reference and installs creator-supplied
words/class/procedure. Duplicate attachment fails before mutating fields.
An ever-enrolled marker prevents recycling a detached binding retained by an
outer alias into a different attachment. This is a lifetime flag, not another
window registry or handle identity. Foreign observation still attaches no
native property, class association or cleanup entry.

Getters and DispatchClientMessage now naturally address the same WW value
once attachment succeeds, rather than copying updates into old snapshots.
The original FindPWW/SETWL classification and callback algorithms are retained.
Scope release drops its existing binding reference; property detach and final
borrow release retain their separate reference ownership. No mirror edits
were added for this change.

New WOW_VIEW_WORDS_CCPU candidate coverage observes a real native STATIC,
holds an outer PWW, attaches creator fields, checks identical nested PWW,
sets a word, rejects duplicate attachment without overwriting creator fields,
detaches/destroys inside nested borrowing and reads the retained outer alias
before final release. Exact guest-memory recovery is required. This fixture
tests WW attachment/alias lifetime, not managed task enrollment or completed
guest word synchronization. It has not run yet. The remaining integration
must publish creator/subclass words into the guest WND and handle final class
collection at actual production boundaries before discovery is enabled.

### Shared-view candidate: publish creator words with the WND

The existing publish_handle private ABI now accepts an optional three-DWORD
input. All validation/allocation occurs before publishing; both fresh WND
creation and observed-to-managed adoption copy explicit creator fields into
the same guest WND. NULL remains the query-only/storage-test input and leaves
existing words intact. The retained managed WW exposes its words through its
existing cleanup companion to the publication caller, not through a second
copy or registry. No host pointer is written into guest state. Updated every
direct fixture call and the explicit dialog publication mock for this private
ABI; both worker and provider must be rebuilt together.

Managed SETWL now pairs a temporary borrow of its already published WND with
the existing numeric guest-word exchange, then changes the canonical host WW.
A failed guest update does not change the host value. Unpublished native-only
bindings retain their host-only behavior. Sampled scopes still use their
already retained backing and shared WW. Original classification/subclass
algorithms are unchanged. The adoption fixture now checks all three explicit
creator words replace observation values and survive retirement; its earlier
word-preservation assertion applied only to the NULL-input storage contract.

These are unbuilt changes. The managed setter's new backing dependency needs
coverage in native lifecycle mocks as well as the real CCPU test. Callback
graph synchronization, root descriptor completeness and production collection
remain open; no claim of complete integration or fresh release is made.

### Shared-view candidate: query the existing desktop root

The normal refresh/borrow paths now resolve the native desktop identity to
the existing root allocation, including callback-cache publication. Observation
of that root adds its missing copied class/name through the same class producer
as other observations; it never creates a second WND or handle. Subsequent
observation refreshes native geometry/styles without allocating another class.
The existing class reference scan protects the root's descriptor and ordinary
retirement still rejects the desktop. Its allocation survives until the page
domain teardown, matching the root's lifetime rather than individual scopes.

Added WOW_DESKTOP_QUERY_CCPU assertions for the original two-step WND.pcls /
CLS.lpszAnsiClassName rebase, unchanged identity/class on repeated observation,
borrow release without root destruction, and rejection of premature class/root
retirement. Root initialization precedes per-window free-memory baselines so
the test does not incorrectly require desktop-lifetime storage to disappear
when a shorter-lived window is released. The fixture remains unrun.

Also corrected the sampled SETWL return boundary after host enrollment: return
the canonical WW's previous value, not an older copied guest value. The numeric
guest update must still succeed first. The alias fixture now checks that exact
creator-value return before proceeding with nested updates. No original body
or guest image changed in this step. Automatic graph discovery and its complete
callback/withdrawal/collection wiring remain the next integration work.

### Shared-view candidate: bounded graph publication operation

Connected the existing native relationship sampler to the query-node producer
and numeric relationship publisher in wow_user_window_project_graph. It first
collects the reachable closure, captures native class descriptors for every
node, then prepares existing/new query projections and rechecks every native
edge before publishing links. The final producer resolves all identities into
guest addresses before changing any edge. A shared sampled_links_match helper
replaces duplicated sampler checks. Added worker exports for the two producer
operations now directly referenced by WOW32; no native pointer is serialized.

All scratch rows/descriptors/links are call-local and released on success or
failure. No task cleanup membership or native ownership is acquired. A failed
preparation can leave newly allocated query nodes in the existing page-domain
allocation lists, but never claims that the relationship update succeeded;
rollback/withdrawal belongs to the still-open complete synchronization owner.
This operation does not yet retire nodes outside its closure or run at a
production callback boundary. Do not enable it there until stale-generation,
terminal unlink, retained-borrow and collection handling are paired.

Public USER calls cannot establish a global desktop lock. Double checking
detects observed races, not atomicity against concurrent native processes.
The desktop parent may expand a closure to other desktop windows; acceptance
must include that scope/cost and race behavior, not only a three-window fixture.
The existing isolated relationship test now rejects a NULL-root projection
before any producer call. Real CCPU graph publication remains untested and
requires an isolated desktop plus paired withdrawal, not the active desktop.

### Shared-view candidate: collect confirmed dead unlinked observations

Added a sweep over the existing guest_windows allocation domain, not a second
registry. It selects only observed-class nodes whose exact native HWND is no
longer valid and whose backing has no incoming published relationship. Live
windows and managed task-owned windows are never removed. Existing retirement
retains backing/class storage while outer borrows exist; class collection runs
after the sweep. Failure preserves the allocation that could not be freed and
can be retried without recreating a successfully removed node.

Graph preparation calls this reclamation before preparing/publishing the new
closure, keeping cleanup failure separate from successful graph commit. The
real CCPU retirement case now verifies a sweep preserves a live observation,
automatically withdraws it after native destruction, protects the class during
two borrows, and releases the class on the sweep after final release. No build
or test has run for this candidate.

This intentionally does not clear incoming edges merely because their target
is dead. Doing so before a successful replacement snapshot would turn a failed
sync into a partially rewritten graph. Consequently dead nodes still linked
from an old snapshot, including a reused low-word slot, remain an explicit
integration gap: they require replacement-edge publication and generation
retirement in one bounded operation. The sweep is not claimed as complete
handle-reuse repair, and production callback activation remains pending.

### Shared-view batch r1: first grouped verification

After the accumulated storage/identity/root/reclamation and projection-interface
changes, froze runtime/test sources and started the hidden supervised batch at
build/M0-T447/S2/shared-view-batch-r1. It regenerates and incrementally builds
both existing x86 Release caches, then runs nine foundation cases sequentially.
Supervisor PID19216 and build child7796 were verified live; each stage records
its actual PID, command, deadline, stdout/stderr and exit. Build deadline is
1800 seconds, each fixture deadline600 seconds, with owned-subtree cleanup.

Worker and provider builds/link both pass (build.exit0, result.exit0). The real
CCPU window-borrow case also passes (window-borrow.exit0): desktop query,
observed promotion, borrowed retirement and same-WW enrollment markers all
report errors=0, followed by WOW_PAGE_DOMAIN_LIFECYCLE_OK. Existing menu,
native-control and callback borrow markers pass in that same executable.
These supersede the earlier unrun status only for the exercised paths; graph
publication/generation replacement and production callback activation are
not covered by those markers. The remaining eight cases are still running
under the same supervisor; no full-suite, real-guest or publication success
is claimed. Release assets and installed binaries remain the P1 baseline.

### Shared-view batch r1: process pass and dialog boundary failure

process-retirement.exit is0 with WOW_PROCESS_RETIREMENT errors=0. The dialog
case exits1: exactly four failures at fixture lines243/245 (two dialogs), which
assert classification/state and dialog-procedure words after WM_INITDIALOG.
Original wcall32.c W32InitDlg lines244--246 sets those fields through SETWL.
The current owned setter first borrows/writes the published guest WND, but this
native-only fixture mocks publish_handle/retire_handle and stores only HWNDs;
it supplies no corresponding borrow/read/write storage. The real page-domain
fallback is inactive in this fixture and the errors are5023 (INVALID_STATE).
This establishes an incomplete mock at the changed boundary, not a passing
dialog regression. Real CCPU callback markers alone do not prove this native
case's original W32InitDlg assertions.

Correction required after the frozen batch: the explicit dialog mock must
retain initial three-word inputs per published identity, implement paired
borrow/exchange/release for those same identities, and assert reference/word
cleanup at retirement. Preserve the existing WM_INITDIALOG expectations and
failure/veto cases. Do not add a production fallback that skips synchronization
when the real page domain is absent. Remaining cases continue under the same
supervisor; source/test files have not been edited during this run.

### Shared-view batch r1: further gates and isolated guest observation

menu-resource, thunk-scope and menu-cleanup also exit0. With window-borrow
and process-retirement, five of nine foundation cases have passed; dialog
remains a recorded failure and the last three cases continue. Runtime and
fixture sources remain frozen.

Prepared an isolated candidate observer using the sealed S2 P1 package as
its input, instead of the obsolete T446 package or the live installed tree.
The first attempt, shared-view-apps-r1, stopped before guest execution because
the isolation guard detected installed broker PID31952. Read-only process
identity checks found only that NTSRV and its NTVWM PID26096 under the exact
O:/WINNT/system32 project paths. Under the owner's standing test-termination
authorization, closed those two verified processes and preserved their records
in closed-project-processes.log. No release files or guest binaries changed.

Retried in a fresh shared-view-apps-r2 directory (supervisor14360, child28896,
900-second deadline), retaining the failed attempt. Both the original fixture
supervisor19216 and the new observation supervisor were verified alive. Actual
WINMINE/SOL/WRITE results are pending; a successfully started observer is not
application acceptance or permission to publish the candidate.

### S2 P2 delivery freeze and grouped results

Owner directs build, test, publish, commit and push of the current progress
before further wiring. This freezes the shared-view foundation candidate as
S2 P2, not completion of S2 or activation of the full relationship graph.
The externally modified assets/winnt.zip is excluded and preserved.

The first foundation batch completed: eight cases passed; dialog-lifecycle
failed for the explicit mock mismatch recorded above. Once that case had
finished, its test-only provider was corrected to copy creator words and
model exact-identity borrow/exchange/release. Its original assertions were
retained, with new word-value and zero-borrow assertions. No remaining case
used that fixture; production sources stayed frozen. shared-view-batch-r2
then passed dialog-lifecycle: creation rollback cases=4 errors=0, callback
cache errors=0 paired=1 exceptions=1, lifecycle errors=0 init=2 terminal=2.
The accepted foundation result is eight r1 cases plus one r2 case, not a
claim that the original r1 suite passed. Both complete host links passed.

shared-view-apps-r2 completed with runner/result exit0. Independent windows
show WINMINE main, WRITE main/document/page-information, and SOL's existing
Out of memory dialog. These match P1's initialization frontiers; WRITE
editing/saving and SOL initialization remain open. Observer cleanup does
not establish normal application exit. No guest image was rebuilt or patched.

delivery-r2 runs the separate actual-gameplay/normal-exit gates and both
DOS17 input matrices against that exact sealed candidate package. Its
supervisor is26464, child2576, deadline1800 seconds, with captured logs and
exit files. No further implementation is admitted before this delivery.

Second-role diff review distinguishes active repairs (native desktop identity,
shared handle metadata and paired host/guest WOW words) from prepared graph
projection. Observation does not create task/class ownership; promotion keeps
the borrowed backing/WW identity. Withdrawal and class reclamation retain
separate results. Only OPENNT-HOST-070 changes original executable text:
one handle-entry-address expression, with the original type/uniqueness and
missing-handle return algorithm retained. Complete callback/return graph
synchronization, linked stale-generation replacement and final collection
remain open; the graph helper is not activated at production boundaries.

delivery-r2 completes with checks/runner exit0:17/17 DOS text/output gates
pass on each Console and Window input route. WINMINE passes flag/counter,
reset, open-cell, WM_CLOSE and launcher completion in Off, Win16 and All.
The unchanged original guest inputs and candidate image hashes are captured
by the individual observers. The wider SOL/WRITE limitations above remain.

Publication supervisor23404 completes with exit0. Recovery copies of both
13-image destinations and the prior release manifest are retained under
delivery-r2/publication. The candidate, assets/release and installed system32
match all13 SHA-256 identities. Only these two images change from P1:

| Image | P2 SHA-256 |
| --- | --- |
| ntvdm.exe | BD4AA3C1FD263C5DFE960E299863E5FC146F06129EFE99EBD39E5EAD721D30D3 |
| WOW32.DLL | 53183D2FBD03D9381464D2ED02D1781F7FF803D25380A88F545D08C71EDC07BC |

No running installed project process required termination during publication.
The immutable WINSPOOL companion and external ZIP remain unchanged by this
delivery. Final governance, mirror-expression, PE/hash and diff checks precede
the reviewed commit/push; this preserves a bounded baseline, not full S2 closure.

### P2 successor: terminal and final-release closure audit

P2 is committed/pushed as6096f1b2c. Rechecked the current source after delivery;
only the unrelated ZIP was dirty. The following findings are source-proved
integration requirements, not attribution of SOL's initialization failure.

| Boundary | Actual current behavior | Required grouped correction |
| --- | --- | --- |
| wow_window_native_proc terminal finally | Ignores detach_if failure, then releases its local binding. | Resolve incoming projected edges before terminal withdrawal; do not lose cleanup ownership if withdrawal fails. |
| dialog_proc terminal finally | Removes creation property, ignores detach_if failure, clears creation window/borrow. | Apply the same terminal contract as ordinary windows, including denied entry and callback exceptions. |
| rollback_enrollment | Restores native procedure then ignores detach_if failure. | Creation veto/rollback must share terminal release mechanics; it cannot rely on a later callback reaching the removed gateway. |
| release_window_backing | A failed final VdmFreeVirtualMemory preserves the numeric borrow reference for retry. | The upper owner must actually retain or transfer that retry obligation. |
| release_binding sampled branch | Ignores release_window failure and frees sample/WW reference. | Currently loses the only upper-level retry handle on that failure. Do not confuse allocation tracking with a reachable retry owner. |
| wow_window_words_release | At native reference zero, ignores release_retained_window failure and frees binding/class reference. | Same failure-contract mismatch for managed backing; repair both paths together. |
| wow_user_invoke_thunk | Releases private aliases, with no relationship synchronization. | Full return-boundary integration cannot be replaced by selected native API wrappers alone. |
| WU32StdHookProc | Has task/borrow scopes, but no relationship synchronization. | Preserve original hook conversion/dispatch; include this pre-guest entry in the same synchronization design. |

Original evidence: kernel/createw.c unlinks the parent chain before
xxxFreeWindow, retains owner through WM_NCDESTROY, then clears child/owner and
uses the desktop parent for retained backing. kernel/handtabl.c
HMMarkObjectDestroy sets HANDLEF_DESTROY even with outstanding locks and
returns false until final unlock. This separates logical destruction from
physical release; it does not permit forgetting a failed final release.
Public native USER owns these native algorithms already. Importing server
destruction/handle management would duplicate that owner and require private
desktop heaps/locks; the selected route remains a same-layout copied view
with existing project storage and borrow owners. No guest algorithm or
original source expression needs changing for these lifetime repairs.

The graph helper remains deliberately uncalled. Enabling it now would make
retire_handle's incoming-reference refusal reachable from the ignored-failure
paths above. Without relationships the refusal is generally absent; this is
not evidence of a newly observed P2 runtime leak or justification for reverting
the verified baseline. Likewise, failed physical free is a source-confirmed
negative path, not a measured normal-run leak.

Grouped implementation requirements, before the next build:

1. Prepare storage and replacement edges without exposing partially changed
   relationships; preserve exact full-handle generations. A linked stale
   observation cannot be reclaimed first merely to make a reused slot fit.
2. Commit replacement edges and logical withdrawal in the existing guest
   execution interval, with no callback/native query/allocation inside commit.
   Terminal owner stays readable through the callback; post-terminal backing
   receives the original cleared-link/desktop-parent disposition.
3. Define consumed-reference versus physical-free failure explicitly at both
   existing release callers. Reuse the existing backing allocation/borrow list
   for any deferred physical reclamation, not a second task/window registry.
   Final scope/process cleanup must have a concrete retry/drain owner.
4. Connect window/dialog/tagged-procedure/hook entry and thunk/native return
   boundaries as one group, including veto and exception unwind. No CPU-loop
   hook, arbitrary polling, global desktop lock claim or guest modification.
5. Extend the existing CCPU fixtures for failed final free, linked generation
   replacement and retained aliases; cover production terminal/veto paths,
   then run one grouped build/acceptance and the ordinary publication gates.

This audit narrows the next implementation to the complete dependency group;
no new runtime edits, tests or publication are represented by this section.

### Successor implementation: consumed borrows and deferred physical free

Implemented the first part of the admitted lifetime group without building
or publishing it. A valid page-domain release now consumes exactly one caller
reference. If the final decommit fails, its existing retired borrow record
remains with zero references/retained-owner; the domain, not a vanished WW or
sample, owns reclamation. Duplicate release and borrowed reads are rejected.
The collector traverses only this existing list and preserves failed entries;
no new registry or native-window enumeration is added. Scope leave retries
after aliases have been released, class retirement retries before checking
reachability, and full worker teardown remains the final domain backstop.
Scope leave failure does not mean that an already-consumed scope may be left
twice. These contracts are documented in both private headers.

Prepared a separate window-reclaim fixture selection: it compiles the same
page-domain source with only VdmFreeVirtualMemory redirected through a
test-owned exact-address failure leaf. Other frees still reach original nt_mem.
The ordinary window-borrow case retains the unmodified production object.
The negative case requires consumed-once, denied access after release, class
retention, failed collection, successful retry and exact allocation recovery.
Both sampled-reader and retained-WW-owner release modes execute that matrix.
No production fault switch or guest change is added. This test is not run yet;
the terminal/graph transaction still needs the rest of the grouped
implementation and acceptance before delivery.

### Successor implementation: relationship replacement and dead-node withdrawal

Extended the existing relationship producer with one bounded replacement
operation. The old publication entry delegates to it with no withdrawals;
there is no parallel publisher. Its caller supplies full identities of
confirmed-dead observations and replacement relationships. The producer
rejects managed classes, the desktop, live native windows, duplicate source
rows, duplicate withdrawals, missing generations and new edges into a
withdrawn node. It checks unchanged sources for surviving incoming references.
This is copied-view validation, not a reimplementation of native UnlinkWindow.

All edge resolution and missing borrow-record allocation precede commit.
Only after validation does it link the prepared retention records, write
replacement edges, clear outgoing edges of withdrawn dead nodes, and perform
the existing handle retirement. Every withdrawn node already has a retention
record, so that commit performs neither allocation nor physical free. The
collector added above owns subsequent reclamation. Existing outer readers
retain their original backing and the post-terminal desktop-parent state.
Failed preparation frees only call-local scratch and leaves graph/identity
publication unchanged. The producer does not claim a native desktop lock.

Added a real-window/CCPU fixture for two mutually related dead observations:
live-window refusal, surviving unmodified incoming-edge refusal, unchanged
graph on each failure, successful edge replacement plus both withdrawals,
retained outer-reader data and exact final allocation recovery. Acceptance
requires WOW_DEAD_GRAPH_CCPU errors=0; this candidate has not been compiled
or run yet. Same-slot old/new-generation storage preparation and production
terminal/callback integration remain unfinished. The replacement interface
is not yet exported to the production provider, and no new release is made.

Second-role review removed the transaction's ignored call to fallible
retire_handle. Both that public entry and the transaction now share one
non-failing copied-state withdrawal body. The public entry performs its
original validation/allocation/free first; the transaction uses its already
validated and retained nodes. This eliminates a misleading ignored failure
inside commit without duplicating terminal field policy. Added stale-generation
and duplicate-row negatives after native destruction, asserting both old
identities and the desktop edge remain unchanged on rejection.

Rechecked the actual original allocation owner, softpc.new/host/src/nt_mem.c
VdmFreeVirtualMemory: STATUS_UNABLE_TO_DECOMMIT_VM returns before header-list
coalescing, clearing HDR_ALLOC_FLAG and incrementing totalFree. Therefore the
decommit-failure allocation remains tracked for retry. The separately handled
HDR_REMAP_FLAG success path is not used for these ordinary allocated WNDs;
this is not a general remapped-memory reclamation mechanism. No original
allocator rewrite or assertion of real failure-injection execution is made.

### P3 delivery boundary and review

Owner directs delivery of existing progress before further integration. This
supersedes the earlier requirement to finish all successor wiring before the
next build, not the full S2 exit criteria. Freeze the current bounded lifetime
foundation; do not activate the graph helper or claim same-slot replacement.

Fresh WND allocation now prepares an unpublished backing under one temporary
reference in the existing borrow list, then publishes the handle slot and
releases that reference. Preparation failure marks it retired and consumes
the reference through the same deferred-free owner. This removes the former
ignored physical-free failure during allocation rollback. It prepares, but
does not yet implement, atomic replacement of a linked stale generation.

Second-role review: all changed production files are project-owned. There is
no new original-source or guest diff, new registry, scheduler or native window
policy. The replacement operation remains unexported to WOW32; production
relationship activation and terminal/veto integration are still open. Its
existing publication wrapper delegates without withdrawals. Actual production
changes are fresh-allocation ownership and final-release/collection behavior.

Build/test roots are reclaim-batch-r1/r2, reclaim-apps-r1 and delivery-r3 under
build/M0-T447/S2. Reuse the validated worker/provider Ninja caches from P2;
rebuild their dependency closure and seal new outputs before runtime tests.
The first window-reclaim run exposed a test-driver selection mistake: its
fault object was replaced by the generic parent-stub object list, producing
two unresolved fixture symbols. Exclude this CCPU case from that stub branch,
retain the failed log, and rerun in r2; no runtime code changed for this repair.
Other foundation cases continue in r1. Published P2 remains untouched until
all gates and independent application comparisons pass.

P3 runtime observations: reclaim-apps-r1 reproduces the P2 frontier independently
for each original guest. WINMINE has its visible main window; SOL still shows
Out of memory; WRITE has visible MSWRITE_MENU, MSWRITE_DOC and MSWRITE_PAGEINFO.
SOL is an inherited failure, not a functional pass; WRITE editing/saving is
still unproved. delivery-r3 then passes WINMINE flag/counter, reset, open-cell,
WM_CLOSE and launcher-completion checks in Off/Win16/All style modes, plus
Console17/17 and Window17/17 DOS text-gated routes. Observation uses isolated
stages and private desktops, no owner interaction or guest modification.

The window-reclaim r2 case passes the exact-address failure injection for
both sampled and retained-owner releases: consumed once, no read/duplicate
release afterward, CLS still retained, failed collection, successful retry
and exact allocation recovery. The ordinary window-borrow r1 case separately
uses the production object and passes dead-node graph atomicity and lifetime
checks. This controlled failure proves the release boundary, not a real OS
decommit fault or complete production relationship synchronization.

### P3 verified publication

All ten foundation cases pass: window-borrow, window-reclaim,
process-retirement, dialog-lifecycle, menu-resource, thunk-scope, menu-cleanup,
native-send, native-nested and task-lifecycle. Nine use reclaim-batch-r1;
window-reclaim uses corrected r2. The r1 suite failure is intentionally kept,
not rewritten as a passing first attempt. Full worker/provider builds pass;
the fixtures retain their established /FORCE duplicate-symbol warnings and
are not substituted for real product DLL acceptance.

delivery-r3 publication passes after the per-case, application-frontier,
gameplay and DOS gates. All13 candidate/release/installed hashes agree;
only the following images changed from P2:

| Image | P3 SHA-256 |
| --- | --- |
| ntvdm.exe | B4C7AA63078D2E0438750CC48064B828E45D08F550C758CFD539A7E76BAD5AAF |
| WOW32.DLL | 9FC7159AFBBEF7AAF7C3958B2B0F4A55DD386BBBD57F1652CBF4D71E0458184B |

Both previous13-image destinations and the release manifest have recovery
copies under delivery-r3/publication. No installed project process required
termination. The original WINSPOOL companion is unchanged. The unrelated
assets/winnt.zip remains excluded, SHA-256
539A910247C5478BDE19FB2A431CD4C5AB26355CDC46F8160F49B23F060B495B.
Final documentation/link, diff, PE/hash and mirror-expression checks precede
the reviewed commit/push. This is a usable P3 baseline, not S2 completion,
SOL recovery, WRITE editing acceptance or full message-contract recovery.
