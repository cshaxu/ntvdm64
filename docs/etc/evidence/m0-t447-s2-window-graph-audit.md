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

### Post-P13 returned-object production closure audit

Baseline cf2ac9520 is pushed and published. This continuation is source
research, not another runtime test loop. The preceding E group is progress;
neither its12 fixtures nor the three applications prove all HWND producers.

Inputs: selected src/mvdm/wow32/wuser.c, wuwind.c, wudlg.c, wuser31.c,
wuclip.c, wusercli.c, wmdisp32.c, wmsg16.c, wowhooks.c, wstruc.c; current
wow_user_private_access.h/.c, wow_user_discovery.c and
wow_user_task_lifecycle.c. Canonical OpenNT revision remains
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Read-only original references:
windows/core/ntuser/kernel/capture.c (SHA-256
4381EC478161E8F74E2AC6E86F662AB5919FEBAF02A8C8C44C4C197B64AEC335),
rtl/getset.c (4A2CB2067F5DD776676D7F808F40F6468082DB018E16BCF9A77DAF00BDDB34C9),
and kernel/focusact.c xxxSetFocus. No source is imported.

| Complete return/callback family | Actual producer and disposition |
| --- | --- |
| Pure HWND queries and window enumerations | P12 discovery seam plus P13 graph owner; retained original thunks convert the native result. Keep that implementation. |
| Parent/owner-valued GetWindowLong/Word | WU32GetWindowLong/Word reaches wow_user_get_window_longA, which currently returns public indices directly. rtl/getset.c GetWindowData deliberately returns owner for top-level windows and parent for children. Preserve that native policy; enroll only the HWND-valued result of GWL_HWNDPARENT, never arbitrary LONG/extra words or procedure tokens. |
| SetCapture/ReleaseCapture | wuser.c calls native APIs without the selected native-call boundary. capture.c xxxCapture can notify the OLD capture window; the new capture target is not that receiver. Pair both entries with the existing native-call ownership/restoration seam, using the previous capture as its target. Preserve native queue restrictions and native result; no substitute capture manager. |
| SetFocus/SetActiveWindow/SetParent and parent-index setter | Already use native_call_end, but return the previous HWND without explicitly rooting it. Refreshing current relationships does not prove every previous result is represented. Extend that same return transaction, not a second native call or duplicate graph pass. |
| EnumProps callback | W32EnumPropsFunc in wuser.c truncates hwnd then calls original CallBack16. WU32EnumProps owns string allocation, atom/string conversion and the original callback state. Missing explicit target publication at this native enumeration boundary; do not replace its property policy, string conversion or RET_ENUMPROPSPROC body. |
| PMODE32 direct local readers | wusercli/usercli queries consume the graph; adding same-name host wrappers would not wire their actual path. No change. |
| Message/hook/structure HWND payloads | wmdisp32/wmsg16/wowhooks/wstruc contain distinct payload converters, including HWND-valued message results. Keep them in their explicit callback/message owner audit; this return group cannot certify their complete conversion coverage. |
| Clipboard, common-dialog, multimedia/fax returns | Preserve W10/W18/W16/W15 boundaries and original converters. The shared-view prerequisite remains recorded, not family acceptance or permission for blanket handle rewriting. |

Important original lifetime evidence: xxxSetCapture saves the previous HWND,
performs capture change, then RevalidateHwnd before returning it. xxxSetFocus
likewise revalidates the previous HWND after callbacks and ThreadUnlock.
Therefore the input HWND, current GetCapture/GetFocus result and actual
returned previous HWND are different values. Never replace the returned value
with a later getter. A destroyed/reused native return must not manufacture a
new guest object or alias another generation.

The next complete S2 group will pair these query, mutation-result and
property-callback entrances with required local publication, error/result
preservation, implicit callback reentry, cancellation, exception restoration
and terminal/reuse tests. Reuse original thunk/conversion bodies and the
existing E graph transaction; expected additional mirror/guest diff is zero.
The selected native USER queue/server implementation remains the stopping
boundary: capture.c depends on private PQ/PTI, locks, tracking and server sends,
not an independently composable algorithm to duplicate in WOW32.

This audit proves missing selected seams, not a failure attribution for SOL
or WRITE. No production source or released binary changes in this audit.

### Returned-object group implementation and verification in progress

Starting source is6275809e0 (owner-approved unchanged ZIP committed after the
fe68af10a audit); published runtime remains P13 cf2ac9520. The whole identified
group now has candidate source, not a delivered runtime. Existing call_end
receives the actual previous HWND in its stack record and includes it in the
same graph transaction. A dead full identity is not republished. Only the
parent/owner-valued GetWindowLong index uses discovery; numeric indices retain
native behavior. Capture calls use the old capture's execution receiver.

EnumProps retains WU32EnumProps/W32EnumPropsFunc and original guest allocation,
atom/string conversion and callback. The native ExA form carries only a
stack-local callback context, not new enumeration policy. Canonical reference:
windows/core/ntuser/client/clenum.c, same pinned OpenNT revision, SHA-256
4EA923B41085E2FEFBF0C65A18F4AF7ECBB60D2E0F0DC652302BA31AB668C1FF.
Its A/ExA entries use the same InternalEnumProps; that body requires private
NtUserBuildPropList/global atom facilities, so native USER remains its existing
stopping boundary. No original function is copied or modified. The original
callback's documented no-yield restriction remains, not a claim to support
recursive use of its static guest callback state.

The existing CCPU fixture now asserts parent/owner/numeric distinctions,
previous capture/parent publication, dead return withdrawal, property string/
atom/data forwarding, signed last callback result, stop, exception restoration,
callback-side removal and invalid-window results with both entry lock states.
The independently authored NE discovery probe additionally invokes original
EnumProps/CallBack16 on a native peer's property and performs USER16's direct
GetClassName reader inside that callback. It is a test input, not a modified
original guest. All added marker assertions are required by their gate scripts.

Background returned-object-r1 (supervisor21712, start17:22:47Z) runs the full
12-case foundation group after worker/provider incremental builds. Those
builds and graph-snapshot/window-borrow have passed at this checkpoint.
returned-object-apps-r1 (supervisor32132, start17:25:29Z) waits for that exact
owner's successful terminal result before staging and running guest/application/
DOS gates. Remaining results and delivery are pending. No mirror/guest changes
or new public/worker export ABI; no publication or S2 closure claimed.

r1's fixture completed window-borrow with zero errors, but its gate then
incorrectly required property markers outside the graph-snapshot compilation
profile. The suite correctly failed and apps-r1 refused staging. Restricting
those property assertions to their actual graph-snapshot case corrects the
test selection without changing or dropping any assertion. Production source
and fixture bodies are unchanged. returned-object-r2 (supervisor15504,
start17:26:59Z) reruns the complete group; apps-r2 (supervisor32032,
start17:27:17Z) waits for it. These are pending, not passing delivery evidence.

r2 subsequently passes all12 foundation groups. apps-r2 fails the new real
guest property gate: errors=0, matched=1, queries=2, freed=2, properties=0,
property_result=0xffff. Publication correctly refuses it. One bounded existing
callback-trace run (returned-property-diagnostic-r1, supervisor31416,
start17:41:30Z) reproduces the missing property callback; it is diagnostic,
not ordinary acceptance or a new production trace hook.

Source attribution: walias.h HWND32 is USER32, a sign-extension of SHORT,
not FULLHWND32. Native property enumeration echoes its HWND argument into the
callback. The new publication entrance consequently received a short HWND,
unlike EnumWindows' full identities, and could reject it as a conflicting
zero/FFFF generation. Under the existing USER lock, discovery now restores
that input through retained GetFullUserHandle and the existing shared metadata
owner before graph publication. Full inputs remain unchanged. No new alias
table, native enumeration policy, original converter or mirror edit is added.
The CCPU property test now includes the original sign-extended input and an
explicit negative assertion against treating it as a full graph root.

returned-object-r3 (supervisor1232, start17:44:52Z) rebuilds and reruns the
entire group with that source-derived correction. This checkpoint is not a
successful real-guest result or publication; the failed r2 evidence remains.

r3 finishes exit0 at17:58:00Z (about787s): both builds and all12 foundation
groups pass, including the explicit short-carrier negative/normalized-positive
case. apps-r3 fails only its supervisor's terminal race before staging: the
owner writes exit0 and exits between the loop's file check and PID inspection.
Both terminal files have17:58:00Z timestamps. Re-read the terminal file after
process inspection before treating absence as failure. apps-r4, supervisor27952
start18:00:08Z, reuses that successful unchanged candidate and runs only the
pending product gates; no repeated foundation build/test is required.

P14 completion: apps-r4 finishes exit0 at18:04:04Z on2026-10-15, about236s.
Real Win16 discovery reports errors0, matched1, queries3, freed2, properties1,
signed property result65529 (-7). Real guest CBT passes query, nested creation,
veto/recovery, unhook and launcher completion. SOL and WINMINE pass actual
interaction and normal exit in Off/Win16/All modes; WRITE exposes its three
expected visible classes in each mode, not editing acceptance. Console17 and
Window17 both match all17 expected exit codes.

returned-object-publish-r3 (supervisor27224, start18:06:50Z) completes exit0.
Its input seals, backups, unchanged-inputs and verified-manifest records prove
all13 images match assets/release and O:/winnt/system32 with correct PE widths.
Only ntvdm.exe and WOW32.DLL change; other11 host and46 guest/config inputs
match the prior baseline. Candidate SHA256 identities:

- ntvdm.exe: 3A72ABDDE933EFC391728EC81E4F5741FED3BEF1CAABB1AB44B798F0859A7C46
- WOW32.DLL: E6EF8D78F61FDD504473A969B7F0A9BCD08A6B8C60F22AEDE6EB15F8E1EA703B

Separate source review confirms no new graph owner, property registry,
scheduler, guest conversion or native enumeration policy. Added bindings are
project-owned and selected through the existing provider header. Zero changed
paths in src/mvdm or src/opennt-host; no guest image is rebuilt or patched.
The owner-approved ZIP remains exactly the committed6275809e0 payload.
This closes only the returned-object group, not S2 or T447; message payload,
reply identity and WRITE editing remain outside this delivery's acceptance.
Final documentation/link, diff, PE/hash and mirror-expression checks precede
the reviewed commit/push. This is a usable P3 baseline, not S2 completion,
SOL recovery, WRITE editing acceptance or full message-contract recovery.

### Post-P3 source review: transaction and terminal ordering

Input baseline is e15193485, synchronized with origin/main; only the excluded
ZIP was dirty. This is source review, not another runtime experiment. Original
reference remains pinned OpenNT windows/core/ntuser/kernel/createw.c:
xxxDestroyWindow unlinks the parent chain at1569, preserves owner at1587,
and enters xxxFreeWindow. xxxFreeWindow destroys children before sending
WM_NCDESTROY at1872; after that callback its2312ff cleanup clears child/owner
and substitutes the desktop parent for retained WND storage. The callback
therefore must not see an already-cleared owner. Conversely, retaining its
old incoming parent link after terminal completion is not equivalent.

The current source review identifies the following complete transaction
boundary, beyond P3's already-verified physical-free mechanics:

| Current site | Proven gap | Required placement/contract |
| --- | --- | --- |
| wow_user_window_project_graph descriptor/observe loop | Each observe publishes immediately; a later descriptor/native recheck failure leaves earlier publications changed. The preliminary collector can also retire objects before snapshot success. | Prepare all missing WND/CLS and generation replacements before publishing any handle or relation; abort keeps old identities/edges. Remove the sequential publish/collect path when the transaction replaces it. |
| observe_window existing-slot branch | A different full identity in the same low-word slot is rejected, even when the old native identity is dead. | Resolve snapshot edges against prepared new backing while keeping old backing for outstanding readers. Never overwrite the old allocation or free it to make preparation possible. |
| wow_window_dispatch_bound finally then wow_window_native_proc finally | Task callback leave runs before the outer terminal detach. | Terminal copied-state work must be placed while the appropriate original execution context is still established; an outer finally is not evidence of guest-access authority. Preserve native-only dispatch's no-WOW-entry branch. |
| dialog_proc finally | Its terminal detach precedes task leave, unlike ordinary windows, but property and creation references are dropped even when detach fails. | One common terminal contract, with idempotence under nested destruction and an explicit retained cleanup owner on failure. |
| rollback_enrollment | Restores the native procedure before ignoring detach failure. | Creation veto cannot depend on a later callback reaching a gateway it has already removed. Pair unlink/withdrawal and restoration while HWND remains valid. |
| wow_user_invoke_thunk and WU32StdHookProc | Existing borrow scopes cover aliases, not graph synchronization. | Include pre-guest hook entry and thunk/native return in the group, preserving original conversion, dispatch and exception policy. |

Execution evidence for the placement constraint is
wow_user_task_lifecycle.c::wow_task_callback_leave: a resumed callback may call
original xxxSleepTask before restoring the suspended native wait. The page
domain checks session binding, CR0.PG and CR3; publication_lock alone is not
that authority. Do not move all work into a generic scope destructor or add
another lock/scheduler to bypass the original handoff.

Four-rung disposition remains: original native USER already owns destruction
and linking; importing createw.c's body would require its private server heap,
thread locks, input queues and desktop ownership, duplicating the existing
native owner. Keep original guest readers and original WOW task ordering;
the minimal adapter supplies only the unavailable copied shared view. No new
mirror expression is required. Explicit transaction scratch is call-local,
not a second persistent object registry.

Grouped verification must distinguish callback-time owner visibility from
post-terminal retained backing; ordinary/dialog terminal callbacks, denied
entry, callback exception, nested destroy, CBT veto and native-only forwarding
are separate rows. Transaction negatives include second-node preparation
failure, native revalidation failure, stale linked generation, surviving
incoming edges and retained readers. P3's dead-observation test alone cannot
prove these production boundaries. Do not activate project_graph until this
whole closure is implemented. No runtime source or published binary changes
are made by this review.

### Post-P3 implementation: whole-snapshot preparation

The subsequent candidate replaces project_graph's preliminary collector and
sequential observe loop with one worker-owned snapshot operation. Provider
sampling still supplies copied class descriptors and full native identities;
it does not publish intermediate nodes. The worker prepares missing classes
and WNDs under existing borrow records, retains existing backing, and rejects
replacement of live or managed generations. Native revalidation and control-ID
sampling precede the shared non-failing copied-field commit.

The existing relationship transaction now resolves edges against its
call-local prepared rows before the old published table. Old observed dead
generations are withdrawn without physical freeing; fresh handles publish
only after that withdrawal. Existing callbacks can retain the old allocation.
No second persistent table or native relationship policy is introduced. The
old relationship-only API delegates to the same transaction without prepared
rows. Native geometry/ID querying is separated from copied field writes so
the commit itself cannot fail on a native query.

Abort consumes temporary borrows, marks unpublished fresh allocations retired,
and uses P3's reclamation owner and existing class reference checks. Existing
window words, class links and published relationships are not updated during
preparation. The heap-containing interval may grow as allocations are reserved,
as it already did for ordinary allocation; that is not node publication.
Successful logical commit does not become a reported failure merely because
subsequent physical reclamation is deferred.

Added CCPU fixture assertions for a second-descriptor failure after first-node
preparation and for native relationship revalidation failure after both
preparations. Both require unpublished identities and exact allocation
recovery. These tests are authored, not yet executed. Successful fresh/reused
generation coverage and all terminal/callback integration rows remain part
of this same unfinished group. No incremental compile/test or publication
has been performed; P3 remains the installed baseline. Mechanical diff check
passes, which is not compilation or runtime evidence.

### Snapshot acceptance and terminal-owner follow-through

Added graph-snapshot as a separate production-object CCPU case. It runs the
existing fixture body on a dedicated thread attached to a newly created,
unswitched private desktop; after thread termination the fixture closes that
desktop. The test does not manipulate the owner's visible desktop. It calls
the real provider snapshot entry, creates a native parent/child pair, verifies
parent/child guest rebasing and numeric control ID71, destroys the native
pair, publishes a new desktop snapshot, and checks that the held old parent
backing survives handle withdrawal until its last release. This is not yet
executed and does not substitute for same-slot generation-reuse coverage.

Further original-source review examined createw.c::UnlinkWindow itself
(2439ff), not merely its callers. It walks the original parent/next chain and
updates links through original Lock/Unlock. It cannot be called directly on
the copied guest WND: those links are server-form guest addresses, not native
PWND pointers. Translating an entire parallel native WND tree merely to call
it would add a second graph owner. Native USER already executes its native
equivalent. Keep the adapter limited to sampling that result; do not author
a competing native unlink policy or import the kernel destruction package.

The terminal failure path is still a required unresolved part of this group:
moving detach earlier alone does not solve callback-entry denial or allocation
failure, and restoring the native procedure before a failed CBT rollback can
remove the only future gateway. Existing WW/property/object-table ownership
must retain a concrete cleanup obligation until logical withdrawal completes.
The publication SRW lock is not a replacement for the original task execution
context. No terminal integration or runtime acceptance is claimed yet; P3
remains installed and the full S2 objective remains open.

### Owner-directed P4 delivery boundary

The owner requires build, tests, publication, commit and push before further
wiring. Freeze this delivery at the prepared snapshot transaction and terminal
storage ownership; do not activate graph projection at additional production
callback/return sites. Same-slot generation-reuse acceptance and complete
pre-guest synchronization remain open, not silently claimed by this P.

The terminal implementation now gives the existing object-table publication
one WW storage reference. Its cleanup view points back to that same storage;
there is no additional pending registry. WM_NCDESTROY and creation rollback
remove the property even if guest withdrawal is temporarily unavailable.
The table retains the failed obligation, class association and thread identity
until its existing cleanup/scope boundary can retry successfully. Successful
withdrawal consumes the table reference exactly once. Ordinary tagged-window
dispatch performs terminal withdrawal before callback/task leave; the outer
finally covers denied entry. Dialog and CBT rollback use this same owner.
Native-only dispatch still bypasses WOW callback entry.

Retired guest backing with incoming copied links stays in the existing borrow
list until those links disappear; it is never freed out from under a reader.
This retention is not proof of graph synchronization: that still-unconnected
production contract remains a subsequent group. No original source or guest
image is changed in this candidate.

Build/test run roots are graph-batch-r1 and delivery-r4. The first group builds
the affected worker/provider closures using the existing validated Ninja
caches, then exercises eleven foundation cases. A supplemental window-borrow
run adds the explicit terminal failure test: leave protected execution,
withdraw the property, fail collection, release the caller, destroy the native
HWND, reenter protected execution, collect through the retained table and
require exact guest allocation recovery. Repeated collection must be harmless.
This is real CCPU-backed host-fixture evidence, not Win16 application acceptance.
All runs are hidden background processes with recorded PID, deadline, logs
and exit status; the user's desktop is never switched or manipulated.

Publication is conditional on these fixtures and independent ordinary
WINMINE/SOL/WRITE frontiers, WINMINE gameplay in all three style modes and
both seventeen-case DOS frontend matrices. Reuse only P3's unchanged eleven
host images and original package inputs; rebuild NTVDM/WOW32 together. Keep
the unrelated assets/winnt.zip outside the delivery.

Grouped verification result: graph-batch-r1 built both production images and
passed all eleven cases: window-borrow, window-reclaim, graph-snapshot,
process-retirement, dialog-lifecycle, menu-resource, thunk-scope, menu-cleanup,
native-send, native-nested and task-lifecycle. Its initial window-borrow and
graph-snapshot binaries precede the supplemental terminal assertion; do not
attribute that later assertion to those earlier runs. The final checked-in
window-borrow source additionally passes in delivery-r4/terminal, with
WOW_TERMINAL_RETRY_CCPU errors=0 and exact allocation recovery after the native
window and property are gone. Production inputs did not change between runs.
The complete worker and provider links pass; retained fixture link warnings
are not represented as warning-free builds. Actual elapsed times are derived
from each process.json start and its corresponding exit-file timestamp.

### P4 verification and publication result

On2026-10-15, the grouped worker/provider build passed in40.9seconds;
the eleven foundation cases took587.8seconds in total. Supplemental terminal
coverage passed in50.8seconds. Each case retains its own PID, start/deadline,
stdout/stderr, link log and exit code; no build was truncated by a foreground
tool observation deadline. Reproduction uses
tests/observation/verify-wow-task-lifecycle.ps1 with the recorded Case,
ProviderBuildRoot=build/M0-T445/S6/provider,
WorkerBuildRoot=build/M0-T445/S6/r003 and a fresh BuildRoot under build.

The sealed graph-apps-r1/package is derived from P3's sealed package, never
from an unrestricted installed-directory copy. Its83.8second isolated
frontier run independently reproduces WINMINE's main window, WRITE's visible
MSWRITE_MENU/MSWRITE_DOC/MSWRITE_PAGEINFO windows and SOL's inherited Out of
memory dialog. WRITE editing/saving and SOL normal initialization remain
unverified/failed respectively, not newly passing capabilities.

delivery-r4/checks passed in123.6seconds: verify-wow-mine-gameplay.ps1 in
Off/Win16/All modes checks flag/counter, reset, an open cell, WM_CLOSE and
launcher completion; Verify-CommandExitStatus.ps1 checks all17 existing
text-gated routes separately for Console and Window. Both matrices contain17
rows with expected=actual; no earlier application frontier regressed.

The publication gate validates all preceding results and package identities,
backs up both complete destinations and the manifest, and publishes the same
thirteen host images to assets/release and O:/winnt/system32. Both locations
match the sealed candidate. Only these two images change from P3:

| Image | P4 SHA-256 |
| --- | --- |
| ntvdm.exe | BCC9AB026DE1333A3D934082FEA3FA662D7725F2CFA6001E05510908126A448E |
| WOW32.DLL | D1BC524B72D72F79A23BC5BCE0A66C4A7305BDA6C4275356D36EEAF0838AF433 |

delivery-r4/publication retains coherent P3 recovery copies and the verified
manifest. No installed project process required termination. Original
WINSPOOL.EXE retains E4E787340F124476090ED0F22837D7D7ACAB37FD7BBE49DB46CAE9AC4944D535.
No guest, original-source mirror, wire protocol or user configuration changes.
The independent review checks property/table/caller reference consumption,
failure-retained class/thread association, non-failing graph commit, rollback,
and the unchanged original-owner boundaries. Remaining production graph entry/
return synchronization and real same-slot-generation reuse remain explicit
next work, not hidden P4 completion claims. Final governance/link/diff and
three-location PE/hash checks precede the reviewed main commit and push.

### Post-P4 source boundary review: creation roots and failure semantics

Verified baseline is cef8ce2cb, matching origin/main; only the unrelated ZIP
was dirty before this review. No new experiment, compilation or production
activation is used to discover the following gaps.

Pinned original createw.c sends WM_NCCREATE at841, calls LinkWindow at945,
then sends WM_NCCALCSIZE at957 and WM_CREATE at963. Its earlier xxxAdjustSize
can send WM_GETMINMAXINFO before WM_NCCREATE. The failure path explicitly
preserves owner until WM_NCDESTROY because clients call GetParent in callbacks.
Consequently a desktop-root-only snapshot is insufficient during creation or
terminal dispatch: an enrolled, still-live callback window can be unlinked.
Use the existing managed object-table identities as additional sample roots,
alongside the desktop and explicit callback HWND where applicable. Do not
manufacture a parent-child edge to force an unlinked window into the tree.
The native sample must retain the actual intermediate state. Dead pending
terminal owners are cleanup obligations, not native query roots.

Original rtl/wow.c GetPrevPwnd/_GetWindow (493--604) follows parent->child and
sibling links, while _GetParent (616ff) selects child-parent versus popup-owner
and preserves tiled-window behavior. Retain these readers and their rebase
rules; neither filtering external siblings nor replacing them with host query
thunks is an equivalent repair. The existing sampler already supplies their
four distinct fields; production activation must supply complete roots.

| Boundary | Required synchronization and ownership |
| --- | --- |
| Tagged dispatch_bound | After original task entry and bounded alias entry, before callback-window publication/DispatchClientMessage; include the actual callback window, not just desktop children. Keep native-only forwarding outside WOW admission. |
| dialog_return_hook/W32InitDlg and dialog_proc | Same pre-guest synchronization at both existing entrances; W32InitDlg has private WW reads even though W32DialogFunc owns CallBack16. Preserve terminal-before-task-leave ordering. |
| WU32StdHookProc wrapper | Synchronize after the existing task/alias entry but before original hook conversion and guest-stack allocation. Do not decode individual hook message layouts or replace original stubs. |
| wow_user_native_call_end | Restore original sender execution ownership first; synchronize copied window state before menu refresh/retirement and before releasing the caller's USER protection. Cover both same-task and resumed cross-task branches. |
| Creation veto and WM_NCDESTROY | P4 retains logical withdrawal responsibility. Subsequent successful publication must remove stale incoming links before guest readers resume; physical retention alone is not that publication. |
| wow_user_invoke_thunk | Audit direct mutations not already enclosed by native-call/creation bindings before adding a blanket return synchronizer. This wrapper has no universal BOOL failure result. |

The last distinction is a source-proved integration constraint, not permission
to ignore a failed snapshot. Original wow32.c W32TryCall initializes its API
result to zero and delegates exceptions to W32Exception; the latter's Ignore
choice can fail the API and continue, while Close sets RET_FORCETASKEXIT.
Therefore inventing a generic exception after an already successful mutating
API neither rolls back the operation nor guarantees that stale guest readers
cannot resume. Likewise, returning zero for every thunk would corrupt varied
API return contracts. Do not add either shortcut. Complete the actual mutation/
callback boundary inventory and retain each source-defined error owner.

The next integrated fixture group must assert early unlinked creation,
WM_CREATE linkage, callback-time owner visibility, native reparent/z-order
mutation, nested destruction, failed entry, veto, exception unwind, pending
terminal collection and same-slot generation replacement. Compare native
query state with the actual CCPU-backed fields at each boundary, not merely
the final window count. P4's published artifacts remain unchanged while this
source-defined activation/failure contract is completed.

Mutation sweep found two concrete omissions in the existing native-call
closure, not a speculative need to intercept every thunk:

- Original wuwind.c WU32EndDeferWindowPos (1088ff) calls EndDeferWindowPos
  directly. wow_user_private_access.h has no redirect and the lifecycle
  native-call group has no wrapper. Begin/Defer only construct the batch;
  End applies the geometry/z-order mutations and is the required callout/
  return boundary. Reuse the existing begin/end adapter with no target HWND,
  preserving the original HDWP conversion and BOOL result.
- Original WU32SetWindowLong and the word-sized setter's negative-index
  branches reach wow_user_set_window_longA, but its public-index branches
  return native SetWindowLongA directly. Style, ExStyle, child ID and owner
  changes therefore bypass the existing native return synchronization.
  Keep logical WNDPROC and private WOW-word handling exactly where it is;
  wrap only the already-native operation through the existing task callout.
  Do not add per-application tests, alter original selector conversion or
  route positive extra-byte storage through a new policy engine.

These bindings belong to the same pending graph activation group. Their
source owners and API results provide explicit error boundaries, unlike a
blanket arbitrary-thunk return override. Check the remaining original USER
mutation imports against the actual redirect selection before claiming the
inventory complete. No new runtime implementation is claimed by this audit.

### Post-P4 activation candidate and delivery gate

Owner requests a compiled, exercised, published, committed and pushed grouped
baseline before further recovery. The candidate connects the existing complete
snapshot at tagged window callbacks, dialog initialization/dispatch, the common
original Hook entry and both native-call return branches. Roots comprise the
desktop, explicit callback HWND and existing managed-table windows; no second
registry, foreign-window subclass or invented parent link is introduced.
EndDeferWindowPos and the already-native SetWindowLongA branches join the
existing callout/return owner. Logical WNDPROC and private WOW words are unchanged.

The first graph-batch-r2 build passed, but three CCPU cases stopped at the
native menu test. graph-focus-r1's refined failure label identifies direct
fixture retirement, not menu data corruption: it used a NULL retained owner
despite newly published incoming links. The fixture now consumes the same
retained-backing contract as production. Graph tests run on an unswitched
private desktop. Fixture native destruction while bound now crosses the
production native-return wrapper; bypassing it left stale incoming references.
Allocator assertions remain exact, not relaxed or removed.

graph-focus-r4 then passed menu/creation, all nine native class projections,
observed promotion/retirement and the new child-ID/reparent/deferred-geometry
checks, but found final nested-callback reclamation incomplete. Source review
confirms the last dispatch-owned WW was released after alias scope leave, and
that scope collected retired WND but not their now-unreferenced sampled CLS.
Release dispatch's reference inside its existing scope and use the existing
observed-class collector (which includes retired-WND collection). Dialog
dispatch already releases its window borrow before scope leave. Native-only
dispatch retains its separate, no-WOW-entry branch. One existing worker-local
collector is added to the private host export selection; rebuild both host
images. No new lifetime list, guest change or original-source mirror diff.

graph-batch-r3 was cancelled during review; graph-batch-r4 failed linking the
newly reached collector before its export selection was added. Neither is
acceptance or published output. graph-batch-r5 is the frozen full build and
eleven-case acceptance run. The tests retain native-only forwarding, creation
veto, exception, denied entry, pending terminal retry and precise allocator
checks. Actual same-slot USER generation reuse and the remaining direct-reader
mutation inventory are still open; this bounded baseline is not S2 closure.

### P5 verified baseline

Frozen production build: graph-batch-r5/run.ps1, supervisor PID11852,
build PID31524, MSVC x86 /MT CCPU40, dependency-selected worker/provider caches
under build/M0-T445/S6; build exit0 in40.6s. Full group took612.9s, all11
case exits0: window-borrow, window-reclaim, graph-snapshot, process-retirement,
dialog-lifecycle, menu-resource, thunk-scope, menu-cleanup, native-send,
native-nested, task-lifecycle. Entrypoint remains
tests/observation/verify-wow-task-lifecycle.ps1 with -Case and the two build
roots; case evidence is beneath build/M0-T447/S2/graph-batch-r5/<case>.
The empty failures pipeline produces no failures.json; individual exits and
suite.exit=0, not that file's absence, establish the passing result.

delivery-r5/prepare.ps1 supervisor PID8184 then ran the supplemental terminal
fixture PID19356 (46.3s, pass), candidate application observation PID33156
(84.0s), and checks PID29280 (124.6s, pass). Candidate graph-apps-r2/package
starts from the hash-verified sealed P4 graph-apps-r1/package, replacing only
the freshly built NTVDM and WOW32; it does not copy the installed O:/winnt
tree or consume the unrelated modified ZIP. Probes use the existing isolated
package/desktop ownership and clean only their own processes.

| Gate | Actual result and limit |
| --- | --- |
| Graph/native mutation | Child ID91, actual reparented WND.parent and all four deferred-position rectangle coordinates match real CCPU memory. Normal/exception nested destruction, retained-reader removal, retry and exact class/window allocator balances pass. |
| WINMINE | Main window preserved; Off/Win16/All each pass flag/counter, reset, open cell, WM_CLOSE and launcher completion. |
| WRITE | MSWRITE_MENU, MSWRITE_DOC and MSWRITE_PAGEINFO remain visible as in P4. This is not editing/saving acceptance. |
| SOL | Existing Out of memory dialog reproduced, still unresolved; no functionality pass claimed. |
| DOS | Console17/17 and Window17/17 expected/actual text-gated results match, including interactive and nested COMMAND/MEM/EDIT routes. |

Publication supervisor PID31604 / child22936 exits0. Both assets/release and
O:/winnt/system32 match the tested thirteen-image set. delivery-r5/publication
retains coherent P4 backups and verified-manifest.json. No installed project
process required termination. Changed identities:

- ntvdm.exe:188CAF403E6612B43A5EF60AA21982B4E9F6E25C63125F34562C94560271CAC4.
- WOW32.DLL:8CE7B2D66C599EE459C4E483719A82898CE49E796D5B5E2E13559778E76D5B98.

The other11 images retain P4 identities. Original WINSPOOL.EXE remains
E4E787340F124476090ED0F22837D7D7ACAB37FD7BBE49DB46CAE9AC4944D535.
Original guest/configuration and mirror code are unchanged. Independent
review checks native-only dispatch exclusion, original task/alias ownership,
no partial snapshot commit, managed versus observed retirement, final borrow
release before scope cleanup, original mutation return shapes and private
collector export linkage. Existing generic-thunk failure/foreign-tree race
limits are not waived. Full original-reader mutation closure and real native
same-slot reuse remain future S2 gates. Final governance/link/diff and all
three-location PE/hash checks precede the reviewed P5 main commit/push.

### Post-P5 native mutation inventory (source review, not acceptance)

Input is committed P5 984ee0441 and the same pinned read-only OpenNT tree.
No application run selected these findings. Trace original WOW thunks through
their native USER owners and compare the selected private redirect header,
native-return wrapper and worker snapshot commit. This is an incomplete
inventory for the next coherent group, not a declaration of S2 completion.

| Original caller / native owner | Proven effect and current disposition |
| --- | --- |
| wuwind.c WU32CloseWindow / kernel/showwin.c xxxCloseWindow; wuser.c WU32OpenIcon / xxxOpenIcon | Original owners use xxxShowWindow to minimize/restore. These two direct native calls bypass the existing native-return wrapper even though ShowWindow itself is redirected. A redirect at a caller does not intercept USER's internal calls. Retain original thunks and native return distinctions; connect their existing callout boundary. |
| wuwind.c WU32ArrangeIconicWindows / kernel/icons.c xxxArrangeIconicWindows | Operates on the parent's minimized children and their positions, not just the passed HWND. Needs the existing whole-graph return update, not a target-only rectangle copy. Zero is also the legitimate no-icons result. |
| wuwind.c WU32ScrollWindow; wuser31.c WU32ScrollWindowEx / client/client.c and kernel/scrollw.c xxxScrollWindowEx | SW_SCROLLCHILDREN reaches OffsetChildren and subsequent child move messages. Child geometry therefore changes even though no selected MoveWindow thunk ran. Preserve the original flag/rectangle conversion and native implementation; connect both entry names. ScrollWindowEx has a region-complexity result, not a BOOL contract. |
| wuwind.c WU32ShowScrollBar / kernel/sbapi.c xxxShowScrollBar | SB_CTL calls xxxShowWindow; ordinary bars directly change WS_HSCROLL/WS_VSCROLL and redraw the frame. WND style/client geometry publication must follow the complete call, including callback paths. |
| wuser.c WU32SetScrollRange / client/sbapi.c SetScrollRange and kernel/sbctl.c xxxSetScrollBar | SIF_RANGE can change scrollbar presence, clear/set WND flags and redraw the frame. This is not merely thumb-position storage. Preserve the original thunk's void guest return and native range validation. |
| wuser.c WU32SetScrollPos | client/sbapi.c selects SIF_POS and SBSetParms does not change scrollbar presence for that mask. However SB_CTL reaches xxxSendMessage(SBM_SETSCROLLINFO), so retain it in the callout audit rather than classify every branch as painting-only. |
| wuser31.c WU32EnableScrollBar / kernel/sbctl.c xxxEnableScrollBar | SB_CTL with ESB_DISABLE_BOTH calls xxxEnableWindow; restoring both arrows can enable the entire control. Other changes send SBM_ENABLE_ARROWS. Thus it can change WND disabled state and requires the same boundary as EnableWindow, which caller-level macros cannot intercept inside USER. |
| wuwind.c WU32SetWindowWord | Negative instance/parent/ID indices already call SetWindowLong and thus the selected P5 wrapper. Positive extra-word writes remain a distinct extra-storage contract; do not replace the original thunk with a second index dispatcher. |
| wuclass.c WU32SetClassLong / WU32SetClassWord | GCL_STYLE and GCL_CBWNDEXTRA reach SetClassLong through the existing private access facade, whose public branches still call native USER directly. This is a separate shared-CLS data obligation, not fixed merely by wrapping the API. |

The last row exposes a second concrete gap: wow_user_window_project_graph
samples class_style/window_extra, but
mvdm_softpc_wow_page_domain_project_window_graph uses those descriptors only
when allocating a missing class. Existing WND.pcls is reused without updating
the existing CLS fields. Thus simply adding SetClassLong to the return-wrapper
list would still leave stale shared class data. Audit original class setters,
guest direct readers and managed/observed class ownership together before
designing that update; do not allocate replacement classes on each mutation or
overwrite logical procedure/private WOW words with native carrier values.

Recovery ladder: retain the already composed original wuwind/wuser/wuser31/
wuclass bodies. The unavailable boundary is native USER's private shared WND/
CLS storage, not the public mutation algorithm. Reuse the admitted callout and
copied-state boundary; importing kernel showwin/icons/scrollw/sbctl would cross
the registered USER-server stopping boundary and require private desktop,
window locks, drawing and positioning machinery. No such import, new window
policy, mirror intrusion or guest change is proposed. New runtime edits and
tests are not yet made; P5 remains the only published baseline.

#### Group implementation admission

The next group connects all ten audited public mutation/callout entries plus
the already-native SetClassLongA branches through the existing task-return
owner. Original guest thunks and their BOOL/void/region/previous-value conversion
remain unchanged. Private WOW words and logical class-procedure mutation keep
their existing owner, not the native public setter. The worker stages and
revalidates numeric class style/window-extra with the existing window graph,
then updates these two existing CLS fields only at the common commit. Retain
the allocation, name, procedure, task, WOW words and retirement obligations.
Original kernel/class.c InitClassOffsets/xxxSetClassData establish those
numeric fields and clone-wide mutation; native USER still performs that policy.
No additional mirror diff or guest modification is required.

Acceptance must inspect actual CCPU WND/CLS fields after scrolling children,
minimize/restore, arranging, bar show/range/position/enable changes and class
style mutation across existing windows. Include unchanged/invalid operations,
preserved class identity and cleanup. Then rerun the complete P5 group and
application/DOS frontiers before publication. No new P is delivered yet.

Completeness review of the selected wuwind/wuser/wuser31 mutation calls adds
WU32ShowOwnedPopups: original showwin.c xxxShowOwnedWindows sends WM_SHOWWINDOW
to the owned popup set; native default handling changes visibility. Native-only
popup handling need not enter a WOW callback, so the original API return also
needs the same whole-graph boundary. Verify an actual owned popup hidden and
shown and its CCPU style after each operation. SetSysModalWindow's apparent
entry is inside NEVER and returns zero; Begin/DeferWindowPos only prepare the
batch whose End boundary is already connected. ScrollDC affects DC pixels,
not child WND geometry. Redraw/UpdateWindow painting and capture/input callbacks
remain distinct from this numeric window-mutation group; do not claim the
entire message/input contract audited or closed.

mutation-batch-r1 compiled successfully and all11 cases passed, including the
new numeric mutation checks. Source completeness review found the popup entry
before delivery; its waiting delivery supervisor33352 was stopped by exact
owned command-line identity before staging. This earlier result is retained
as partial evidence only. The popup-inclusive source requires a new frozen
group run; neither version has replaced the published P5 package.

### P6 verified mutation baseline

The subsequent frozen mutation-batch-r2/run.ps1 (supervisor24460,
build33088) passed the full x86 /MT CCPU40 worker/provider build in49.2s.
The unchanged valid dependency caches remain build/M0-T445/S6/r003 and
provider. All11 cases pass with suite.exit=0 and failures.json=[]:
window-borrow, window-reclaim, graph-snapshot, process-retirement,
dialog-lifecycle, menu-resource, thunk-scope, menu-cleanup, native-send,
native-nested and task-lifecycle. Build plus group completed in619.5s.
The strengthened runner requires the new mutation marker, including popups=1,
for all four real-CCPU window cases, not merely the preceding P5 marker.

The custom native-class fixture checks existing class identities and numeric
fields across two windows, ScrollWindow/Ex child coordinates, scrollbar range/
position/presence and disabled state, owned popup visibility, minimize/restore,
arrangement snapshots and invalid/no-op calls against real native state and
actual CCPU memory. Existing exceptional callback, retained-reader, failed
preparation, retirement/retry and exact allocator tests remain enabled.
This is boundary/CCPU fixture evidence, not a claim that every application
has passed all original guest thunks or that native concurrent snapshots are
equivalent to holding USER's private desktop lock.

mutation-delivery-r2/prepare.ps1 (supervisor8396) then ran application observer
4060 and checks26592. Candidate mutation-apps-r2/package was copied from the
manifest-verified sealed P5 graph-apps-r2/package, replacing only the two
freshly built host images. No installed-tree or modified ZIP staging input.
Application observation completed in86.8s and gameplay/DOS checks in123.3s.

- WINMINE Off/Win16/All each passes flag/counter, reset, open cell, WM_CLOSE
  and launcher completion.
- WRITE preserves visible MSWRITE_MENU, MSWRITE_DOC and MSWRITE_PAGEINFO;
  editing and saving are still unverified.
- SOL retains the previously observed Out of memory failure, not a functional
  pass or proof of its cause.
- Console17/17 and Window17/17 match expected guest output, including the
  retained interactive/nested routes. None was accepted by exit code alone.

Publication supervisor9504 / child19484 and publication/exit.txt report0.
The full13 images are hash-verified in the sealed candidate, assets/release
and O:/winnt/system32; coherent P5 backups and verified-manifest.json remain
under mutation-delivery-r2/publication. Changed images:

- ntvdm.exe:70B0AB1270DDA65CCFA6B981C9D8C1E602E68BC9C911E01772D6B2BC5506449F.
- WOW32.DLL:A4CDE8418BC095B50DA341A14136AC0FDEDB4F66390DE67D0AC4F7F69F5AD4C4.

The other11 runtime images and all40 other system32 package files match P5.
WINSPOOL remains the original hash recorded above. No guest, configuration,
mirror, private cross-component ABI or public CLI changes. The independent
review retains original thunk conversions and native failure/previous-value
results; public setters use the existing task-return owner, while logical
procedures/private words keep their prior owner. Class field writes occur only
after graph validation, use the existing allocation, and cannot overwrite its
procedure, task, name or WOW words. No new object registry or native mutation
algorithm is introduced. Governance/link/diff and final PE/hash checks gate
the reviewed P6 commit/push. This is not S2 closure or full WOW32 recovery.

### Post-P6 creation-boundary reconciliation

Input is delivered e97345404; the worktree initially contained only the
preserved unrelated ZIP. Source review, not a new application failure, selects
the following remaining work. Earlier pending-startup statements in this
chronological record must not be interpreted as the current implementation.

| Responsibility | Current source / evidence | Disposition |
| --- | --- | --- |
| Standard-class startup and cleanup | wow_user_task_lifecycle.c initializes the original-table-derived metadata with wow_standard_classes_prepare and binds its matching cleanup table before publishing runtime->lifecycle. | Connected; do not redo the old startup repair. Full standard-control behavior remains S3 work. |
| Explicit creation and initial native children | create_bound installs creation_hook, and dialog creation installs dialog_create_hook. Both forward otherwise-unclaimed children to wow_window_creation_forward_internal. | Connected only while those temporary scopes exist; not a universal native creation boundary. |
| Existing object updates | P5 callback/native-return graph publication and P6 mutation/CLS updates. | Delivered, with bounded snapshot/failure limitations retained. |
| Native same-slot generation reuse | wow_page_domain_fixture's window-links-reused-slot explicitly publishes synthetic 0x34560005/0x34570005 identities into real CCPU storage. | Real storage proof, not actual native allocator reuse; acceptance still missing. |
| Message-driven application-class creation | Original MDI client creates its child while handling WM_MDICREATE, outside an explicit W32CreateWindow request. Existing internal enrollment additionally rejects non-borrowed application classes. | Source-confirmed missing enrollment closure; design it as one creation group, not an MDI-specific route. |

Concrete original chain: wumsg.c WU32SendMessage uses retained ThunkMsg16;
wmsg16.c ThunkWMMDICreate16 converts the MDI structure; native USER receives
SendMessage. Pinned OpenNT windows/core/ntuser/client/mdiwin.c, WM_MDICREATE
at1563, calls CreateWindowExA/W with WS_EX_MDICHILD and the selected application
class. That internal call does not reenter WOW's pfnCsCreateWindowEx binding.
The current source selects no general thread creation hook: the only WH_CBT
installations under wow32-dll are inside explicit create_bound and dialog
creation. Even if another creation scope is active, forward_internal rejects
a class for which wow_class_words_native_registration is false. A registered
WOW application class uses wow_window_native_proc, which rejects an HWND
without its WW binding before dispatch. Thus merely having a valid native
class registration or post-call shared snapshot cannot supply the missing
pre-callback owner and private storage.

Original kernel/createw.c provides WND/WOW storage before HCBT_CREATEWND
(hook call at394); its later xxxAdjustSize/WM_GETMINMAXINFO is around796--800.
Existing original walias.c FindPWW initializes classification in available
WOW words but does not create missing storage. Retain that initializer rather
than invent another native-versus-Win16 classifier. For implicit creation,
original lpWOW may be NULL; do not fabricate explicit W32CreateWindow's WW
flags or copy a parent's procedure merely to make a callback execute.

Next group must reconcile explicit requests, native-created children and
owned popups, both registered application and borrowed standard classes,
first callback/CBT order, veto/exception unwind and terminal cleanup through
one enrollment owner. Thread-lifetime versus bounded-native-call hook placement
still needs review: a permanently installed older CBT hook alone does not
prove metadata exists before a subsequently installed guest CBT hook executes.
Do not silently accept that ordering change. No filename, WM_MDICREATE-only
replacement path, second class registry, rewritten MDI implementation or
guest mutation is admitted. Original MDI/message conversion remains selected;
the missing seam is private window-storage ownership before original callbacks.
This does not establish causality for SOL's OOM or complete S2/S3. No new
runtime code, build, test or publication is part of this audit continuation.

### Creation callback ordering and existing original hook identity

Further source tracing at 527984cbf resolves one proposed design dependency,
without changing the delivered P6 runtime. Inputs are the same pinned OpenNT
tree and current wow_window_creation_binding.c, wow_dialog_creation_binding.c,
wow_hook_callback.c, wow_user_task_lifecycle.c and original wowhooks.c/.h.
Procedure: inspect the creation hook, original hook dispatcher and converter,
then enumerate all native-call scopes and compare original createw.c ordering.

- Original wowhooks.c already exposes W32GetHookStateData. Its caller supplies
  iIndex; the original function checks the original hook-table bounds and
  returns HOOKSTATEDATA, including InUse and iHook. The existing project
  WU32StdHookProc entrance receives that same index. A future creation guard
  can therefore distinguish an actual WH_CBT/HCBT_CREATEWND using the existing
  original accessor and declaration, without another hook registry, guessed
  message identity, direct unchecked array access or a new mirror edit.
- Original ThunkCbtHook's HCBT_CREATEWND branch reads GWL_WOWWORDS and explicitly
  returns FALSE without calling the guest when that storage is absent. Its
  WM32Create conversion and subsequent CallBack16 must remain unchanged.
  Publishing only at the first WNDPROC or native return is too late. Merely
  adding a long-lived CBT hook does not establish ordering against newer hooks.
- Original createw.c copies lpWOW at232, initializes the module/procedure
  around298--307, and calls HCBT_CREATEWND at394. Its comment about an unset
  procedure describes the older Windows bug, not the NT4 implementation.
  Preserve class-procedure selection before CBT; do not postpone it based on
  that comment. lpWOW being NULL is not permission to omit window storage.
- Existing native-call scopes already cover SendMessage, DispatchMessage,
  DefWindowProc, DefMDIChildProc, DefFrameProc, queue peeking and explicit
  window/dialog creation. They are useful operation boundaries, not proof
  that every native-origin callback necessarily has such a caller scope.
- Explicit creation installs its CBT hook before native_call_begin; dialog
  creation does likewise. Adding a hook in native_call_begin without changing
  that ownership order could enroll their root HWND first and prevent the
  existing explicit WW input or dialog initialization property from attaching.
  The correction must share enrollment selection, not stack two competing
  root owners. A borrowed standard class must retain its native procedure;
  an application class must retain its already registered logical procedure.
- A third existing scope dependency is wow_window_creation_menu: it does
  nothing unless current_scope owns the HWND. General enrollment alone would
  still omit an implicit application-class window's original class-resource
  menu. Include this in the same group; retain the existing original resource
  client/parser and native menu association, not a new menu loader.

Implementation review requirements are now concrete: one idempotent enrollment
selection shared by explicit creation, dialog roots and implicit known-class
creation; an original-hook entrance guard before conversion regardless of
hook installation order; one ownership-aware veto/exception rollback; matching
first-callback menu handling and terminal release. A guard must not call the
next hook twice, retire another scope's attachment, classify by HWND spelling,
or enroll arbitrary unrelated host windows. Reuse W32GetHookStateData rather
than alter the original dispatcher. Whether the generic native observer lives
for the thread or the bounded call remains a lifecycle design decision; neither
placement alone substitutes for the original-hook entrance guard.

The coherent acceptance group must cover explicit custom/standard windows,
dialog root plus controls, message-created application windows, child/owned
popup creation, both hook-installation orders, hook veto/exception, early
WM_GETMINMAXINFO/WM_NCCREATE, nested creation, class menus and destruction.
Verify real CCPU-visible owner/class/words before guest hook conversion as well
as at later callbacks. This source audit identifies requirements, not passing
tests, an SOL cause, or permission to change guest binaries. Additional mirror
diff remains zero; production implementation and its grouped acceptance remain
pending.

### Creation-group implementation in progress

After the ordering audit in 3f55fbce9, implementation starts in project-owned
creation/window-word bindings only. This is an unverified worktree, not P7 or
a replacement for the published P6 package. No compile/test/publication has
been run for these partial changes; the owner requires group acceptance.

The class-menu helper now receives the enrolled window's retained class and
per-window checked/attached state. This removes the explicit-creation TLS
dependency while keeping the original resource client and native association.
The window callback already retains that binding across resource-loading
reentry. Attachment resets the state, including observed-storage adoption.
The former creation-scope menu state and duplicate ownership are removed.

Internal enrollment and its ownership-aware finish are separated from native
CallNextHookEx. Known application classes can use their retained class target;
borrowed native classes still require a managed parent/owner. Existing
attachments are not stolen and downstream veto/exception retires only the
borrow acquired by this enrollment. No explicit W32CreateWindow WW flags are
fabricated for implicit creation. This does not yet supply a general observer
or resolve explicit/dialog priority before a newer guest hook.

The menu fixture follows the new per-window state contract. The native creation
fixture adds actual WM_MDICREATE-driven creation using the production internal
enrollment helper through a fixture CBT observer, with its application target
registered on the class. That is deliberately not claimed as production
observer-lifetime or guest-hook ordering proof. Next: connect those remaining
entrances, strengthen veto/nested/dialog and real CCPU coverage, review the
whole group, then perform one build/test/delivery cycle. Original guest and
mirror files remain unchanged; assets/winnt.zip remains excluded.

The next implementation pass connects the task-owned CBT observer and original
hook entrance to that transaction. Explicit and dialog hooks now delegate to
the shared entrance, which first checks existing ownership, then asks the
pending explicit/dialog creator, then considers known implicit classes. The
creator retains its original WW input/dialog property; the hook transaction
holds a separate borrow for veto/exception rollback. Native procedure restore
also checks that the current property is still the same borrowed identity.
The hook wrapper includes the original precomp declarations and uses unchanged
W32GetHookStateData before the original converter, not a copied hook table.
Task initialization installs a thread-only observer, failed initialization
unhooks it, and retirement unhooks after original object cleanup before freeing
the task carrier. The test scope fixture adds enrollment rejection, graph
failure, CBT veto/exception and non-CBT overlapping-code assertions.

Whole-group build/regression has started under creation-batch-r1/run.ps1:
supervisor33752, build child29392, started2026-10-15T09:20:55Z with1800s build
deadline. Existing11 boundary cases follow serially with600s per-case bounds.
No result is claimed at launch. New-case runner integration and lifecycle
failure review remain required before publication; ordinary app/DOS delivery
gates still follow. This is not a delivered P and no installed image changed.

creation-batch-r1 built successfully but failed window-borrow/window-reclaim.
The real CCPU fixture reached WOW_DIALOG_CACHE_CCPU with one error: its dialog
creation at wow_page_domain_fixture.c:384 returned NULL/error5023. Earlier
native creation, class, graph, menu and mutation markers were all zero-error.
Source review identifies the introduced prerequisite error: generic enrollment
required ppi->classes before asking the pending dialog creator, while original
dialog-root storage needs only its existing object owner. Move that requirement
after explicit/dialog claiming and retain it only for generic class lookup.
Do not weaken the fixture or add a class table just to satisfy the new gate.

Stopped the failed r1 supervisor33752 and its identified child tree. taskkill
reported an exit race for5616/33724; elevated CIM recheck found neither process
nor supervisor alive before reusing build caches. r1 remains failed/aborted,
not an11-case pass. No installed or release artifact was replaced.

New tracked verify-wow-creation-boundaries.ps1 runs the production hook wrapper
with ordering-only mocks and the exact production menu-helper prefix with a
resource-only mock; its scope limitations are explicit. The lifecycle runner
adds native-creation, linking the production owner objects. creation-batch-r2
supervisor26436 started2026-10-15T09:26:41Z: full affected build, previous11
cases plus native-creation/hook/menu (14 total), same bounded background model.
Runtime results, hook installation-order and callback-exit failure review,
ordinary app/DOS gates and publication are still pending.

While r2 runs, source review keeps two ordering obligations explicit rather
than treating green fixtures as whole-group closure. Pinned createw.c loads
the class menu at640 and associates it at664--670, before xxxAdjustSize /
WM_GETMINMAXINFO around800 and WM_NCCREATE at841. The existing helper's
WM_NCCREATE entrance supplies the menu before that callback, but does not
prove menu visibility during an earlier sizing callback. Moving state to the
window removes the scope dependency, not this inherited adapter timing gap.
Do not claim exact earliest-callback equivalence from the menu fixture alone.

Also review task-scope failure after a successful CBT chain: current wrapper
finishes its enrollment before task_callback_leave, yet changes the result to
a creation veto if leaving fails. That last failure would need ownership-aware
withdrawal too; do not release the identity and then guess it from a possibly
reused HWND. Preserve final-borrow/alias retirement ordering while resolving
this, and verify the observer does not unnecessarily acquire WOW execution for
an otherwise native-only callback. These are source-review obligations, not
newly observed app failures or reasons to modify the guest.

Original createw.c's MemError path at843--873 calls xxxFreeWindow, whose body
sends WM_NCDESTROY at1872 before releasing the object. This supports retaining
terminal ownership through normal post-CBT creation failure; it does not remove
the explicit veto cleanup requirement at the public hook boundary. At the last
r2 observation, window-borrow, window-reclaim, graph-snapshot and
process-retirement all exit0; supervisor26436 and dialog-lifecycle child29200
remain the same active batch, not a restarted run.

r2 is terminal: all previous11 cases and the new hook guard passed; failures
are native-creation and menu. The hook marker proves enrollment5/accepted1/
rollback3, including failure, veto and exception, not real guest execution.
The menu runner truncated the production source before claim_explicit's
definition (C2129); it now includes the exact translation unit with function
section elimination rather than copying a truncated algorithm. Native-creation
returned3 without diagnostics because its printf resolves to the linked
original host output route. Its checks now use stderr, like the other combined
fixtures. Its cause remains unproved; no assertion or runtime behavior is
changed to make it pass.

The post-r2 review corrections retain the creation borrow until the task-scope
return determines the final CBT veto. A nested finally still finishes that
transaction before alias collection, including task-leave failure/exception.
The scope fixture now asserts the leave-failure rollback (enrollment6,
accepted1, rollback4). Ordinary native creation observation no longer enters
WOW scheduling; original guest hooks and tagged procedures own their existing
task entrances. The observer retains only its bounded private-data borrow.

New enrollment arms per-window menu initialization; ordinary existing-window
attachment starts already checked so a later sizing query cannot replay
creation. The original helper is invoked before the first WM_GETMINMAXINFO,
using the retained explicit menu input, and still propagates the associated
handle to fresh WM_NCCREATE/WM_CREATE structures. Association failure remains
latched so an early sizing failure rejects the later creation callback.
Loading is now inside the selected native/tagged dispatch branch; guest resource
conversion therefore has the existing task/alias scope before it runs. The
menu fixture checks early sizing and subsequent native ownership, including
explicit menus, child IDs, missing resources and rejected creation.

creation-batch-r3 supervisor28696 started2026-10-15T09:43:28Z for the same14
cases after r2 was confirmed terminal. Build and tests remain pending at launch.
No candidate image has been published or committed; P6 remains the release.

Owner requires delivery of this reviewed group before further wiring. r3
finished with the existing11 cases and hook guard passing; native-creation
and menu failed. The former old native-only fixture omitted the now-required
real page domain; wow_user_window_publish correctly rejected publication.
After sharing the existing CCPU setup, r4 reached enrollment/veto but still
rejected callbacks: wow_menu_native_refresh_window requires a bound task even
when the window has no menu. The fixture now uses the existing callback-borrow
test's task/runtime setup and actual class projection, not success-only storage
stubs. Its WM_MDICREATE case also installs the production creation observer.
No production check was relaxed. The menu harness's whole translation unit
still required unrelated external symbols; it now extracts only the exact
standalone production menu function, with checked delimiters and resource mock.

creation-batch-r5 supervisor724 started2026-10-15T10:01:42Z. Only the two
corrected test cases rerun; production inputs remain unchanged from r3's full
build and12 passes. The affected native case includes the real CCPU foundation
checks too. Application/DOS gates and publication remain required; neither
failed harness run changes installed binaries or proves S2 completion.

r5 stopped at a fixture-only duplicate wowuserp.h inclusion after lifecycle
types were added; remove that redundant include, not the original header.
r6 supervisor31092 started2026-10-15T10:03:03Z and passed both corrected
cases. Native creation uses real CCPU storage and the production observer:
errors0, callbacks51, nested1, rejected1, vetoed1, destroyed7, table_empty1.
The shared CCPU lifecycle and earlier foundation assertions also pass in that
same executable. Menu ownership/ordering reports errors0, sizing callbacks6,
resource loads4. Together with r3's unchanged-production12 passes, the group
has14 passing cases. No failed run has been relabelled as a pass.

Delivery supervisor26020 started the sealed P6-based candidate/application
stage at2026-10-15T10:04:11Z (child21024,900s deadline), followed by three-mode
WINMINE gameplay and both DOS17 matrices. Evidence root is
build/M0-T447/S2/creation-delivery-r6; candidate is creation-apps-r6/package.
Only NTVDM/WOW32 are eligible replacements; the other11 images and original
companions must match P6. This launch record is not a publication claim.

### S2 P7 creation baseline delivery

The grouped runtime gate completed successfully under creation-delivery-r6:

- r3's11 original boundary cases and hook guard plus r6's real CCPU native
  creation and native menu ownership cases pass (14 total). Reproduce through
  tests/observation/verify-wow-task-lifecycle.ps1 with the recorded Case and
  provider/worker roots, and verify-wow-creation-boundaries.ps1 for hook/menu.
- Ordinary isolated WINMINE, SOL and WRITE observations are retained under
  creation-apps-r6/frontiers. WRITE retains visible MSWRITE_MENU, MSWRITE_DOC
  and MSWRITE_PAGEINFO; editing/saving remains unverified. SOL still shows
  Out of memory and is explicitly not a functionality pass.
- verify-wow-mine-gameplay.ps1 passes Off, Win16 and All: flag/counter, reset,
  open cell, WM_CLOSE and launcher completion. Each records exact input hashes.
- Verify-CommandExitStatus.ps1 passes Console17 and Window17, each17 rows,
  zero expected/actual mismatches. Source/fixture/runtime timeouts remain
  unchanged; no failed gate was waived or replaced by an exit-code-only claim.

Publication child27808 started2026-10-15T10:08:39Z with180s deadline and
exited0. Its publication/exit.txt is0. Candidate, assets/release and configured
O:/winnt/system32 agree for all13 images. Only these two identities change:

| Image | SHA-256 |
| --- | --- |
| ntvdm.exe | 792A0DED9DE25381B4AA02CEA54CF9E23A39AC0FD50C6C265B4FD303424389EB |
| WOW32.DLL | 186BCACC3B357AB99B02281356AE34C56548B6E534D618E91F0029DF0FDD281E |

The candidate is built from the sealed P6 package, never a wholesale installed
tree copy. All40 non-release system32 companions retain their P6 hashes.
publication/backups.json retains prior asset/system32 identities and recovery
copies. No original guest source or binary was modified/rebuilt. The unrelated
assets/winnt.zip keeps SHA256539A910247C5478BDE19FB2A431CD4C5AB26355CDC46F8160F49B23F060B495B
and remains outside this delivery.

Second-role review checked shared explicit/dialog/implicit enrollment, existing
owner preservation, native-chain restoration by matching identity, callback
exception and final task-return veto cleanup, native-only observer scope,
original W32GetHookStateData identity and per-window menu lifetime. Production
changes are299 added/117 removed lines across eight owned files; zero additional
mirror/guest diff. No second hook table, scheduler or class registry is added.
Original hook conversion, class selection, resource conversion and MDI behavior
remain their retained owners. The hook guard uses ordering mocks, while native
creation uses real CCPU storage and production observer with controlled task
and callback inputs; neither is proof of arbitrary original guest CBT programs.
Those broader coverage obligations and full S2 closure remain open. This P
preserves a usable reviewed baseline before additional integration work.

Final identity/PE checks, documentation/link governance and diff checks pass
(creation-delivery-r6/final-checks.exit0). All40 original companion identities
are recorded in original-companion-identities.json. Commit scope is the eight
owned production files, their checked-in regression sources/runners, current
state/evidence and coherent release assets/manifest; the unrelated ZIP is
excluded. Commit title: Deliver T447 S2 P7 implicit creation baseline.

### Post-P7 production standard-class projection audit

Input is delivered ff6d74dba; only the preserved unrelated ZIP was dirty.
This is source tracing, not a fresh application experiment. The next complete
dependency group is prepared standard-class metadata -> first managed HWND ->
shared CLS identity -> observation promotion -> normal/veto retirement.

Concrete production gap:

1. wow_user_task_lifecycle_initialize calls wow_standard_classes_prepare and
   binds cleanup. prepare imports nine native class records; import_native
   sets the original FNID/procedure/module metadata but does not publish CLS.
2. The only production caller of wow_class_words_publish_guest is
   publish_completed_class in wow_class_client_bindings.c, reached by original
   application registration. No production standard-class caller exists.
3. enroll_created_window passes its acquired class to WW attachment, then
   wow_user_window_publish forwards object->pcls->guest_server to the worker.
   For those prepared controls that field is therefore zero.
4. project_window_graph compensates for a zero WND.pcls by allocating an
   observed class from native name/style/extra, with FNID and WOW words zero.
   That is an appropriate read-only observation for an unowned foreign HWND,
   not the prepared managed class identity. Distinct windows may obtain
   separate observed CLS allocations instead of their existing shared owner.
5. If discovery already published the HWND, publish_handle's promotion branch
   requires an existing non-observed next_class. A zero class_server cannot
   satisfy it; promotion fails rather than preserving the existing alias.
   This is a source-derived failure condition, not an attributed SOL cause.

Why existing coverage did not establish production connection:
verify_native_class_projection explicitly calls wow_class_words_publish_guest
inside its nine-class loop before verify_native_class_creation. It proves the
storage implementation, field layout and retirement, not the missing caller.
Keep those assertions and add production-path acceptance without that manual
preparation; do not weaken the publisher or permit zero managed CLS identity.

Pinned original createw.c at150--192 selects GetClassPtr, stores pwnd->pcls and
references that selected class before callbacks. Preserve that ownership;
original class selection, native registration and native control procedures
are not candidates for replacement. The existing class publisher already
copies the name, original metadata and encoded guest procedure policy, while
forget_native already owns paired projected-class retirement. Reconnect those
owners, not a new atom table or per-window substitute class factory.

Implementation boundary under S2: ensure the selected prepared native class
has its shared projection before managed enrollment publishes any WND. Reuse
the existing class publisher and preserve already-published application
classes. Lazy publication is a candidate because process preparation also
runs in native-only fixtures before a CCPU domain exists; unconditionally
publishing during that preparation would change its contract. Resolve domain
admission and metadata serialization first. wow_user_runtime_enter is
non-recursive and callback/native-call boundaries can already hold or release
its lock: preserve caller lock ownership and never hold it across CallNextHook
or guest callbacks. No new scheduler or generic publication subsystem.

The verification group must cover multiple windows sharing one prepared CLS,
correct original FNID/name/words, observed-to-managed promotion retaining WND
aliases, explicit/internal creation, allocation failure before publication,
CBT veto, terminal cleanup and final borrowed-native-class retirement without
UnregisterClass. Remove the test-only manual preparation from at least one
real CCPU production-entrance case. Then run the existing coherent delivery
gate once after the entire group is connected. Expected additional mirror and
guest diff: zero. No runtime modification or build is admitted by this note
alone until the source/lock review is completed in the current packet.

Rejected premature lead: task->classes.resolve_callproc is NULL, but original
WU32RegisterClass always encodes a Win16 callback, and WU32GetClassInfo returns
the original guest/native thunk representation. The original server CPD branch
therefore cannot be declared this ordinary registration failure just from the
NULL callback. Native/CPD restoration is already separately handled by the
window/class procedure owners. Do not invent a CPD registry or claim this
unsupported native-only input is the observed application root cause.

### Standard-class admission implementation review

Source/lock review resolves the candidate above in favor of task admission,
not per-window lazy publication. wow_user_task_lifecycle_init_style already
holds the runtime exclusive lock and validates the active page domain through
set_expected_windows_version before xxxInitTask. publish_named_class requires
that same session owner, CR0 paging and CR3 directory. Initialization of the
process remains native-only; no hook or guest callback is called by the class
publication loop. This avoids adding lock recursion or storage mutation in a
native creation hook which can run outside the guest execution epoch.

The existing public class list is the sole owner. Publish only borrowed native
entries missing guest_server; application registration is unchanged. On a
partial allocation failure, task admission fails and existing task cleanup
removes its context/hook. Successfully projected classes remain process-owned;
retry fills missing members. forget_native is already paired with process
disposal after all tasks/windows, refusing live associations. Worker domain
teardown is its exceptional backstop. No new rollback registry, class table,
native registration or guest image mutation is needed.

Implementation adds this bounded call before xxxInitTask. Real CCPU coverage
now calls the same group publisher instead of manually publishing each class,
checks inactive-domain rejection, idempotence and two native BUTTON windows
sharing the selected CLS, retaining original layout/creation/veto/retirement
checks. Native lifecycle fixtures explicitly mock only the storage leaf and
exercise failure on the third class, removed task context, and retry without
republishing the first two. Those mocks do not prove CCPU or guest behavior.
Grouped build/runtime gates and publication are still pending; P7 remains the
published baseline. This is not evidence that SOL's failure has been repaired.

### S2 P8 standard-class admission delivery

The preceding pending statement describes the pre-verification state. The
complete group is now built and tested under standard-class-r1; supervisor
29788 started 2026-10-15T10:29:14Z, with each child command/PID/deadline and
exit recorded. Build and all14 cases return0: window-borrow, window-reclaim,
graph-snapshot, process-retirement, dialog-lifecycle, menu-resource,
thunk-scope, menu-cleanup, native-send, native-nested, task-lifecycle,
native-creation, hook and menu. No failed row was waived. The task marker is
WOW_STANDARD_CLASS_ADMISSION projected=9 calls=10 retry=1 errors=0; real CCPU
reports WOW_NATIVE_CLASS_CCPU errors=0 projected=9 native_pointers=0 released=1.
The ten calls include the deliberately failed third allocation. Existing
native creation reports callbacks51/nested1/rejected1/vetoed1/destroyed7,
table_empty1/errors0; those are not a full original guest-hook acceptance.

standard-class-apps-r1 seals the candidate derived from the exact P7 package,
not the user's installed tree. Ordinary WINMINE retains its main window;
WRITE retains visible MSWRITE_MENU/DOC/PAGEINFO; SOL still shows Out of memory.
The SOL root cause remains open and WRITE editing/saving is not verified.
standard-class-delivery-r1 records three-mode WINMINE flag/counter/reset/open
cell/WM_CLOSE/run16 completion PASS and Console17/Window17 each17 matches,
zero mismatches. Application supervisor33056 and checks30204 have recorded
commands, deadlines and successful exit codes; preparation also returns0.

Publication supervisor596 starts at10:45:03Z and completes with exit0. Its
recoverable backup/verified manifest records coherent publication to both
assets/release and O:/winnt/system32. Changed image SHA-256 identities:

- ntvdm.exe: 558BC70257B7187F96C99CA921335D43580717BF53D489CF94876236AF15C711
- WOW32.DLL: 47192B315AB6543E3A16EE1E4349F901D3E8CA9CA94D81E91B5253411E30A585

The other11 release images retain P7 identities. Production changes are only
the standard-class binding source/header and task-lifecycle call site:
30 additions/2 removals, including comments/declarations. No mirror or guest
source/binary change; existing class owners displace the wrong observed-class
fallback for managed controls, while foreign native observation remains.
The same implementer reviews separately: lock ownership stays at admission,
no lock is added around native hooks/callbacks, process-owned partial success
has one retirement owner and retry skips existing projections. Final gates
cover documentation/links/diff,13 PE/hash identities and unchanged companions.
Unrelated assets/winnt.zip stays excluded. Commit title: Deliver T447 S2 P8
standard class admission baseline. S2 remains active; no whole-WOW closure.

### Post-P8 real native slot reuse acceptance

Source review distinguishes shared-view publication from managed creation.
Original OpenNT windows/core/ntuser/kernel/handtabl.c HMFreeObject clears the
entry and advances uniqueness; HMMarkObjectDestroy retains locked objects.
The retained GetFullUserHandle reads type/uniqueness, not cleanup ownership.
Current page_domain_project_window_graph prepares a new backing before
withdrawing a dead observed generation. Existing borrow records retain the
old backing. This calls for real native reuse acceptance, not another handle
registry or a change to the original resolver.

The checked-in graph-snapshot case now runs verify_native_slot_reuse in
tests/mvdm-host/wow_page_domain_fixture.c. It projects a real STATIC HWND,
borrows its CCPU WND, writes a sentinel WOW word, destroys the native window,
and creates/destroys hidden windows until native USER returns the same slot
with a different full handle. Both131072 attempts and30 seconds bound this
witness acquisition; absence is a failed assertion, never a passing skip.
The old publication stays intact until the normal graph projector replaces
it. Assertions require current GetFullUserHandle identity, refusal of a fresh
old-handle borrow, separate backing, preserved old sentinel, zero new words,
and release of both generations after native destruction. No synthetic
handle is substituted for this case.

Run: build/M0-T447/S2/native-reuse-r1, graph-snapshot through the existing
verify-wow-task-lifecycle.ps1 entrypoint, provider cache M0-T445/S6/provider
and worker cache M0-T445/S6/r003. Background supervisor17592 owns child32800,
started2026-10-15T11:13:59.3867306Z with600-second outer deadline; fixture has
its existing60-second deadline. Both return0. Marker:
WOW_NATIVE_SLOT_REUSE_CCPU errors=0 attempts=1 old=029c0d64 new=029d0d64
old_backing=00225000 new_backing=0022a000. The complete graph-snapshot fixture
also returns WOW_PAGE_DOMAIN_LIFECYCLE_OK. This is real native USER plus
real CCPU storage evidence, not a Win16 guest-program reuse witness.

Separate review keeps the managed-creation case open: CBT enrollment calls
publish_owner before native_call_end refreshes the graph. A stale observed
generation can therefore still be present when an internal creation needs
that slot. The passing graph-return case does not prove that earlier entrance
or justify adding a graph refresh inside a callback without its lock review.
Next acceptance must cover observed-to-managed reuse inside a native call,
including existing borrows and rollback. No production/mirror/guest source
changed in this test delivery; P8 remains the published runtime baseline.

### Managed reuse source design after a0cfa71b3

The collision is source-proven: both ordinary and dialog CBT enrollment call
wow_window_words_publish_owner, then wow_user_window_publish, then the worker
publish_handle. Its existing-slot branch accepts only identical full identity;
the graph replacement which can retire the old observed generation occurs at
native_call_end, too late for this entrance. borrow_enter does not refresh it.
Do not insert a callback-wide graph refresh: dialog roots deliberately have
no managed CLS, and observed-to-managed promotion requires a managed CLS.

Selected repair stays in the existing worker publication owner. Validate full
new identity and native liveness as before. A conflicting slot may be replaced
only if its class identifies an observation and its old full HWND is dead.
Prepare new WND/words first; retire the old slot with the existing retained
backing operation; commit the prepared new WND; release both temporary
references. Existing graph edges and reader references defer old physical
release until the ordinary graph return and borrow cleanup. A failed prepare
or retirement leaves the old publication intact. Never discard a live or
managed identity. A null class on a fresh dialog remains valid, unlike an
attempt to promote an already observed identical HWND without a class owner.

Recovery ladder: original handtabl.c HMMarkObjectDestroy/HMFreeObject and
createw.c provide the lifetime/order contract but operate on kernel-owned
handle pages, pool and USER locks, which cannot replace public USER's native
allocator. The existing registered page-domain storage seam retains copied
WND/handle layouts and original guest readers. Reuse that seam's retirement
and borrow operations; no new allocator, registry, policy interface or mirror
intrusion. Zero additional mirror/guest diff. Gate the whole group with real
native managed reuse, null-class publication, retained old readers, collision
refusal and existing creation/veto/rollback/graph tests; then the full runtime
P gates and coherent publication. Candidate changes are not a delivered P.

Implementation is24 lines in the existing worker publisher, using existing
prepare/commit/retire/release functions and no new private ABI. Separate review
checks that fresh storage is allocated before old publication changes; failed
retirement marks only the fresh temporary allocation retired; after successful
withdrawal the remaining commit cannot allocate or call native USER. Existing
incoming links keep old physical storage until normal graph synchronization.
No new lock, callback, subclass, class registry or original-source diff.

Grouped acceptance managed-reuse-r1 runs under supervisor19040; build child
32952 starts2026-10-15T11:22:23.0287133Z with1800-second deadline and returns0.
The first window-borrow case returns0: managed creation reuses039400da as
039500da and the null-class storage case reuses039600da as039700da, each on
attempt1. Marker WOW_MANAGED_SLOT_REUSE_CCPU reports errors0, creation1,
null_class1, retained1, released1. The null-class case is explicitly storage
acceptance, not a real guest-dialog witness. The other foundation cases and
runtime publication gates remain pending at this checkpoint.
Preparation supervisor12672 waits for the complete14-case suite, then runs
managed-reuse-apps-r1 against sealed P8 inputs and managed-reuse-delivery-r1
gameplay, real guest CBT and DOS gates. It does not publish automatically.

All14 foundation cases complete with exit0 and an empty failures.json.
Application child19484 starts11:34:22Z and returns0 with the P8 frontiers:
WINMINE main window, visible WRITE menu/document/page-info, SOL Out of memory.
WRITE editing/saving and SOL successful initialization remain unverified.

The first gameplay gate in managed-reuse-delivery-r1 fails: Off mode probe
returns6 after posting a right-button click; the sampled covered-cell hash
remains3cc8733b for5 seconds. Board readiness and native frame checks succeeded.
The runner then cleans its own package; its forced cleanup is not a normal
application-exit result. This failed attempt is retained, not waived or
described as a repaired defect.

Bounded follow-up uses identical sealed P8/candidate packages and unchanged
probe assertions. managed-reuse-gameplay-review runs both with existing thunk
tracing enabled: both pass. Because tracing changes timing, a predeclared
six-run clean-environment sequence follows without tracing, under supervisor
15248: baseline, candidate, candidate, baseline, baseline, candidate. All six
pass full flag/counter/reset/open/close/launcher assertions. The prior timeout
remains unexplained; these observations establish neither an inherited defect
nor a repaired one. WOW32 .text is identical in the two packages:371712 bytes,
SHA256 8DACFC82F3A9DBFCF2BF7CB8D888CC133BCA1777A0DF42684BF631C3833FB4A6.

A single complete untraced gate, managed-reuse-delivery-r2, starts under
supervisor17184 against the unchanged candidate (no rebuild or weakened
assertions). Its three WINMINE modes and real Win16 CBT pass. DOS and final
publication checks remain pending at this checkpoint. Preserve the first
failure and exact source/input evidence alongside the final gate; do not
claim the intermittent observation is solved by the slot-reuse repair.

### S2 P9 managed reuse delivery

The complete r2 gate (child7420, started2026-10-15T11:41:42.7291916Z,
1800-second deadline) returns0. Console17 and Window17 each contain17 matched
results; all three WINMINE modes and the real guest CBT report PASS. This
supersedes only the pending gate statement, not the retained failed attempt
or the remaining application/coverage limitations above.

Publication supervisor19108 / child28956 starts11:45:07.6269866Z and returns0.
managed-reuse-delivery-r2/publication retains recoverable P8 backups and the
verified13-image manifest for assets/release and O:/winnt/system32. No existing
project session needed closure. Changed image SHA256 identities:

- ntvdm.exe: 884B275A704A02F312B7F3137E45E790935354CBF228BF67E4928D2D25CFBBA4
- WOW32.DLL: D4EA48E50362CD320CBF2F7C6C4AC61BF5BABBA8F2BADEE04B43F051C14787BB

The other11 host images are unchanged. Source change is confined to24 added
lines in the project-owned page-domain publisher; WOW32's unchanged code was
relinked against the rebuilt worker import library. No original mirror or
guest source/binary change. Existing retired backing and graph-reference
owners prevent reuse from overwriting live aliases; the real native test
exercises both creator and null-class storage forms. This is not proof of all
guest callback families or closure of S2/SOL initialization. Final governance,
relative-link, diff and complete package/companion identity gates precede the
P9 commit; assets/winnt.zip remains an unrelated excluded modification.

### Post-P9 source review: production bitmap registration gap

The next group is selected by the production callback graph, not another
application retry. WU32LoadBitmap calls pfnOut.pfnWOWLoadBitmapA; registration
currently selects registered_load_bitmap, which refuses a NULL
wow_bitmap_current. The only production initializer of that TLS context is
wow_bitmap_bindings_enter, called by wow_bitmap_load; the rejecting wrapper
does not call it. A source-wide reference search finds no enclosing production
initializer. In contrast, wow_bitmap_thunk_integration_fixture.c explicitly
sets its private pfnOut slot to wow_bitmap_load. Its successful conversion
therefore did not verify actual production registration.

Read-only static inspection of the sealed P9 SOL.EXE (SHA256
68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31)
narrows the application consequence without modifying/running the guest.
NE startup 8:0083 calls movable entry85, resolved to1:0000. Its first-instance
initialization at1:00f0 calls movable entry12 (10:0000); that routine calls
USER ordinal175 (LoadBitmap) for resources53,67,68 at10:0017/0029/003b,
returns zero if any handle is zero, and cleans the acquired handles. The caller
then takes1:0121 ->1:0144 ->1:0404. Other failures (GetDC, RegisterClass,
CreateWindow, SetTimer) also join this exit; it calls entry15 (2:07c1), which
passes the string buffer at DS:0534 to2:05cc's MessageBox call. That buffer
was loaded with string resource300 by1:010b. A subsequent read-only NE resource
table inspection resolves RT_STRING block19/index12 to `Out of memory`.
Thus bitmap refusal is a concrete startup error path, but its runtime execution
has not yet been independently verified by this static record. No general
heap-failure attribution or successful SOL initialization is claimed.

Existing inspect-win31-ne-segment.ps1 produced build/M0-T447/S2/
sol-static-entry-r2 and sol-static-segment1-r1, segment2-r1, segment10-r1,
segment11-r1 (each latter name prefixed sol-static-). All exit-code.txt values
are0. Background PIDs19868,15248,29100,29028,29392 began between
2026-10-15T11:56:53Z and11:58:34Z; commands select immutable SOL, the named
segment and fresh build output, with a120-second observation deadline. Earlier
sol-static-entry-r1/PID32780 is retained as failed: Windows PowerShell lacked
Get-FileHash; PowerShell7 completed the unchanged script. These are static
analysis results, not product runtime tests. Unrelocated operands are resolved
using the recorded NE relocation chains and movable entry table, not treated
as executable native addresses.

Recovery decision: retain selected original clres.c::WOWLoadBitmapA and its
ConvertDIBBitmap algorithm (OPENNT-HOST-033), using existing ADAPTER-WOW-025
for unavailable native conversion environment. Direct original global USER
scratch/DC state is not supplied by modern USER32; the already selected scoped
adapter supplies that finite dependency. No additional mirror intrusion or
new converter is justified. Delete registered_load_bitmap and bind the existing
wow_bitmap_load directly. Protect its acquired context with finally so original
conversion unwind cannot leave TLS pointing into an expired stack or leak its
DC/lock. The registration transaction, input lease and alias conversion remain
their original/existing owners. Additional mirror and guest diff: zero.

Candidate edits implement that connection and exception unwind; the existing
failure fixture now injects a converter exception and verifies outer TLS
restoration and subsequent conversion. Production-registration acceptance,
full bitmap group build/test, ordinary three-application comparison and
coherent publication remain pending. No candidate binary is published.

Background bitmap-registration-r1 supervisor31832/build29560 starts
2026-10-15T12:04:21Z; worker/provider builds finish0. The first native fixture
link fails1120 on printf and the unrelated W32FindResource dependency in the
shared clres translation unit. Preserve bitmap/failure.txt. The test runner
now links the standard legacy_stdio_definitions library and compiles the
identical clres source with /Gy for test-only function-section elimination;
no missing provider stub or product build change is introduced. Continuation
supervisor13736 starts12:06:45Z after the first supervisor exits. It reuses the
completed product build, runs fresh bitmap-r2 tests, then the14 existing
boundary cases serially. Pending results are not passes. The tracked
verify-wow-bitmap-bindings.ps1 also checks the actual registration assignment;
this static guard supplements, not replaces, ordinary guest acceptance.

bitmap-r2 still fails linking W32FindResource despite function sections;
bitmap-r3 uses the already existing WOW_BITMAP_FIXTURE_BINDINGS selection but
fails compilation because that selection expects its caller to supply the
bitmap environment header. Neither failure changes the production candidate.
bitmap-r4 supplies that same production header explicitly and retains the
existing bitmap-only selection; it is not a new mirror conditional or stub.
Supervisor30924 starts2026-10-15T12:08:43Z after19208 has exited. Fresh run
directories retain all failed attempts. Subsequent14-case execution is serial
and starts only after the bitmap group passes.

bitmap-r4 completes0: WOW_BITMAP_BINDING_OK records30 nested conversions,
4x50 parallel conversions, two named/ordinal resources, two missing resources
and unchanged GDI count0/0; WOW_BITMAP_BINDING_FAILURE_OK records locks2 and
screen acquisitions/releases10/10 including the injected converter exception.
The registration assignment audit reports21 input/20 output slots. These
are native/structural acceptance, not guest execution proof. Supervisor30924
continues the14 boundary cases. Application supervisor29340 starts
2026-10-15T12:10:49Z, waiting on that exact process and successful group result
before sealing bitmap-apps-r1 from P9 with the new worker/provider images.
Delivery supervisor21632 starts12:14:49Z and waits for29340 before running
bitmap-delivery-r1's three-mode gameplay, real guest CBT and DOS17 matrices.
Neither supervisor publishes; each consumer rejects a failed prerequisite.

The14-case continuation completes0. Application observation completes0:
bitmap-apps-r1 records visible Solitaire/Stat windows at all four samples,
with no Out of memory dialog; WRITE retains MSWRITE_MENU, MSWRITE_DOC and
MSWRITE_PAGEINFO. This improves SOL's initialization frontier, not proof of
card gameplay or normal SOL shutdown. The observer's final cleanup remains
explicit fixture cleanup. The unchanged guest's static bitmap failure path,
the production registration defect and this bounded before/after result
support attribution of that initialization failure to the missing bitmap
context binding, not to guest heap exhaustion.

bitmap-delivery-r1 checks child13736 starts2026-10-15T12:21:53.8661284Z with
1800-second deadline. All three WINMINE modes pass flag/counter/reset/open-cell,
WM_CLOSE and run16 completion. Real Win16 CBT probe passes. DOS matrices and
publication remain pending. Candidate images are ntvdm.exe SHA256
5F069FB4CBADC9CB14C4F70B80574AE655082DC5B1ABEB4C8858BE90A4370027 and WOW32.DLL
5B0D03A9DB0295E9961999F20391339A49FCC72EDDD5AC24670593527E74D2CA. The worker
build recompiles generated base_vdm_config.c and relinks; its1579520-byte .text
section matches P9 byte-for-byte. No worker production source was changed.

### S2 P10 bitmap registration delivery

bitmap-delivery-r1 checks.exit and run.exit both finish0. Console17 and
Window17 each have17 expected/actual matches. The original CBT guest probe
and all three WINMINE gameplay modes pass. The static registration assertion
is explicitly only a wiring check; ordinary SOL's improved main-window
frontier supplies independent execution evidence through the real registrar.

Publication supervisor32240 starts2026-10-15T12:25:32Z and completes0, as does
publication/exit.txt. Its recoverable backups and verified13-image manifest
cover assets/release and O:/winnt/system32. Candidate hashes above are the
published identities; the other11 host images remain unchanged. Worker .text
SHA256 is11ED6E602660B1EDC05DE1B62FC03A141FDD2B1E3668E7D284E600CD679F3C26 in
both P9 and P10. All source changes remain in the two owned WOW32 files;
git diff lists no mvdm/opennt-host mirror changes. Final governance, relative
links, full guest/configuration identity and source review precede commit.

Independent role review: the obsolete rejecting wrapper is removed, not
retained as a fallback; the selected original WOWLoadBitmapA/ConvertDIBBitmap
still owns bitmap interpretation. Existing TLS nesting and critical-section
ownership are paired with finally, without catching or translating original
exceptions. Native no-resource loading retains its original separate path.
The retained fixture failures are harness configuration failures, not waived
runtime failures. No guest binary/source, graphics algorithm, allocator or
message policy was changed. Remaining T447 obligations are not closed by this
P: SOL gameplay/normal exit, WRITE editing/saving, EDIT's original memory and
word-break consumers, full message/reply semantics and individual task exit
retain their planned acceptance gates. The unrelated assets/winnt.zip change
is excluded from delivery.

Final supervisor23396 completes0: documentation governance (including relative
links), diff checks, all13 PE/hash identities in tested/release/system32 paths,
and all46 remaining original guest/configuration files compared to P9 pass.
CURRENT remains below32KiB. assets/winnt.zip retains the excluded SHA256
539A910247C5478BDE19FB2A431CD4C5AB26355CDC46F8160F49B23F060B495B.

Separate EDIT audit confirms another unmet contract, not the SOL attribution:
wmsgem.c encodes EM_SETHANDLE as guest DS/handle and EM_SETWORDBREAKPROC as a
WOW-tagged procedure. Current standard-class enrollment retains modern native
EDIT's procedure. Original client globals.h LOCAL* macros invoke registered
W32Local* callbacks; usercli.h CALLWORDBREAKPROC invokes W32EditNextWord for
tagged targets. No such consumption follows merely from copying registration
inputs. Treat original EDIT storage, callbacks, dialog DS and lifecycle as one
dependency group; do not bridge only one message or infer WRITE usability.

## P10 SOL operation acceptance supplement

This test-only continuation retains published runtime5edea33bb. The purpose
is to distinguish a visible main window from actual deal/draw/close behavior;
it does not close S2 or change the runtime. Entrypoints are
`tests/observation/verify-wow-sol-gameplay.ps1` and its
`wow_sol_gameplay_probe.c`. The runner compiles only the probe, uses a fresh
owned physical stage and isolated desktop, and verifies Run16's actual exit
before fixture cleanup. No guest memory access, patch, injected procedure,
desktop switch or computer-use is involved.

Inputs: sealed `build/M0-T447/S2/bitmap-apps-r1/package`, existing observer
`build/M0-T445/S5/console-startup-observer.exe`, x86 MSVC compiler driver
`build/M0-T445/S6/provider/msvc-x86.cmd`. Probe flags are /O2 /MT /W4 /WX.
The machine is the unchanged x86 CCPU40 product. SOL SHA256 is
68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31.
Final runners record all13 host-image and SOL hashes in inputs.json. Product
source, build inputs and release artifacts are unchanged, so the P10 build,
DOS17, WINMINE and other boundary evidence remain applicable; this supplement
does not claim a new production build or repeat unchanged publication.

### Static acceptance contract

Read-only NE inspection in sol-static-segment1-r1 and sol-static-entry-r2
retains raw disassembly, relocation chains and exact input hash. SOL's
1:0462 window procedure routes WM_COMMAND1000 through1:06e0 to1:09b4;
the same command initializes the game at1:03fa. WM_INITMENU (0116) reaches
1:075e and updates command1000 from its busy state. The test checks this
native menu state before issuing the command. WM_LBUTTONDOWN/UP at1:0833/
084c retain original capture and game callbacks. WM_CLOSE uses the default
close path, with WM_DESTROY timer/procedure cleanup and PostQuitMessage.
The test checks a changed tableau, changed waste pile, destroyed main window,
and `result=exited`, `exit=0x00000000` from the waiting launcher. These are
bounded ordinary-message checks, not physical-pointer or full-game acceptance.

The first probe assumed immediate redeal must change cards. Relocations prove
1:09c8 calls movable entry11, segment8:00fc; this time routine reads DOS AH2A/
2C and uses DH seconds, discarding DL hundredths, before converting the date.
Entry10, segment8:01ca, seeds the original random state. Two deals in one
second legitimately repeat the board. The corrected probe waits1100ms between
the initial sample and redeal; this is a fixture input precondition, not a
product timing change or a relaxed pixel assertion. A menu-ready check alone
does not solve the equal-seed test error.

### Runs and results

All roots below are under build/M0-T447/S2. Each background supervisor records
the actual child PID/start/command/deadline in its `.process.json`; child bounds
are150s, observer bound60s, individual image/window checks10s.

| Run | Background supervisor / start UTC | Result |
| --- | --- | --- |
| sol-gameplay-r1 |33788 /12:40:22 | Probe6, redeal image assertion timed out; retained, not a pass. |
| sol-gameplay-r2 |9180 /12:42:13 | Deal/draw/WM_CLOSE and launcher0 pass; diagnostic-only change, no runtime change. |
| sol-gameplay-r3-Off |15528 /12:44:31 | Probe6 despite menu enabled; visible Solitaire, no modal confirmation. Group stopped before style cases. |
| sol-gameplay-r4-Off |25764 /12:46:40 | Corrected seed precondition: all four assertions pass. |
| sol-gameplay-r4-Win16 |same supervisor | Probe4, expected main window absent; group stopped before All. |
| sol-gameplay-r5-Win16, -All |4976 /12:48:39 | Both probe4; diagnostic enumeration proves visible class #32770 titled SOL (330x146), WOWExec still alive, no visible Solitaire main window. Neither is a gameplay pass. |

Off r4 has ready/deal hashes76ea77d5/97359d34, waste65174fed/b644f519;
its main client is585x384. The r2 and r4 BMPs show actual cards and waste
updates. They are not a pixel-correctness or complete drag/drop-game verdict.
Raw failures, probe stdout, observer reports and captures are retained; no
failed run was overwritten or relabelled as success.

### Review and next owner boundary

The two failed immediate-deal assertions are consistent with the now-proved
seconds-granularity seed contract; no trace captured their seed values, so
the precise per-run seed equality remains an inference. Corrected Off passes.
Style-enabled startup is a separate unresolved result, not that seed issue:
it fails before the test sends a deal command. Do not claim P10 universally
fixed SOL initialization or infer a heap problem from the dialog's existence.

Source review identifies the bounded propagation to audit next:
registered_create_window_ex wraps native/original creation in the existing
WOW style scope; any style-scope failure destroys an otherwise created HWND
and returns NULL. NTW16S attachment.c accumulates per-window installation
failures, while deliberately unsupported custom children are nonfatal.
Review that full creation/return/cleanup group and the original SOL startup
failure joins before selecting a repair. The current evidence does not yet
identify which operation failed or establish a regression-introducing commit.
No new mirror diff, runtime patch, guest change or style workaround is made.

## SOL style creation-order diagnosis and admitted group

Post-c5c09ea26 source review and one bounded Off/Win16 creation-boundary
comparison refine the failure: the dialog says an application error occurred,
not Out of memory. `sol-style-boundary-r2` supervisor32844 started
2026-10-15T12:57:19Z and completes0 (observation only, not gameplay). Off shows
Solitaire/Stat and a successful final WindowCreate; Win16 enters WM_CREATE,
then additional WINDOWPOSCHANGING/NCCALCSIZE/WINDOWPOSCHANGED/MOVE/SIZE. SIZE
never returns before the application-error dialog, while the main window
remains hidden and the final WindowCreate record has not occurred.

The preceding r1 supervisor19056 completed observation but both modes failed
earlier with illegal instructions and no creation trace. Unlike P10 it retained
the agent's inherited environment. Keep that non-comparable failure; do not
attribute it to style or silently turn it into a pass. R2 reuses the same
explicit standard-environment whitelist as the P10 baseline, with only the
two existing trace variables added. Environment sensitivity remains unresolved.

Static proof uses the immutable SOL and retained relocation-aware segments in
sol-creation-source-r1 (background PID26552,12:59:23Z): after CreateWindow at
1:0352 returns, 1:03a2 calls entry18 (3:0263), whose relocation026e calls
entry36, the game initialization. Segment4:0020 publishes DS:0028 only during
that initialization. Earlier WM_SIZE calls entry22 (4:01c8), which reads that
game pointer and traverses its card arrays unconditionally. The style engine
invokes frame_changed/restore_client from its WM_CREATE-return attachment;
these issue SetWindowPos and force the premature SIZE before initialization.
The defect is project-added presentation timing, not a reason to patch SOL.

Recovery choice: original application/USER creation and later ShowWindow
ordering stay unchanged. NTW16S is already an admitted autonomous local
presentation engine, not an alternative original WOW implementation. Keep
its resource allocation/subclass attachment at the existing creation boundary,
but defer forced geometry for an automatically attached initially hidden
normal frame until its first show. Explicit install on an already constructed
window retains its immediate contract; initially minimized/maximized paths
retain native placement. Removal/destruction before first show must not force
an otherwise unnecessary resize. No timer, posted-work queue, process-name
test, guest patch, mirror change or new exported ABI is needed.

This bounded prerequisite is admitted under S2's first-callback ordering
contract. The complete group covers creation attachment, first-show/client
preservation, hide/show reuse, hidden removal/destruction, reentrant destruction
and existing initial-state/nested-Off behavior. Add a native caller whose data
becomes ready only after CreateWindow returns; it must reject premature SIZE.
Then verify the immutable SOL's operation/normal exit in all three styles,
retained WINMINE/WRITE frontiers and the standard coherent publication gates.
Neither this design nor the observation run claims the repair is implemented.

## Creation-order repair acceptance

The candidate implements the admitted boundary in NTW16S only: automatic
initially-hidden normal frames retain eager allocation/subclass attachment,
but their forced frame geometry activates on first WM_SHOWWINDOW. Explicit
installation remains immediate. Pending removal skips geometry restoration;
existing callback lifetime pins protect first-show reentrant destruction.
Initially minimized/maximized creation keeps its previous placement contract.
There is no new exported API, guest change, OpenNT mirror diff, timer, posted
work item, application-name branch or swallowed attachment failure.

Read-only NE entry-table verification additionally confirms entry18=3:0263,
entry22=4:01c8 and entry36=4:0000. This resolves the initialization target
directly rather than guessing from unrelocated far-call operands.

Background `style-order-build-r1` (PID27516,2026-10-15T13:10:08Z) builds both
Release/O2 widths and passes the extended actual-DLL attachment fixture.
`tests/app/style_attachment.c:verify_creation_order` tests native negative
space, no premature styled SIZE, preserved first-show client dimensions,
repeat hide/show, unseen detach/destruction and destruction during first SIZE.
Existing nested-Off, failed creation, initial minimized/maximized placement,
30 lifetimes and GDI balance also pass in both widths.

`style-order-checks-r1` (PID21748,13:12:15Z) runs that new fixture against
both published old DLLs: each fails exactly the early_sizes assertion at
line58, whereas both candidates pass. Actual DLL binding/unloading,
opposite-width rejection, 28-page demo, title marker, default button and
dialog/control tests pass in x86 and x64. The unchanged immutable SOL passes
deal, stock draw, window destruction and launcher exit0 in Off/Win16/All;
WINMINE likewise passes its gameplay and normal-exit checks in all modes.
Exact inputs, captures and output are retained in the per-mode directories.
These bounded actions do not establish arbitrary gameplay or WRITE editing.

The same supervisor then stopped on a missing WRITE log directory, before
that observation started. Its exit1 and error remain intact. It is neither
a runtime failure nor a passing complete suite. `style-order-remaining-r1`
(PID14080,13:15:36Z) resumes only WRITE observation, real guest CBT and the
two DOS17 matrices with a fresh log directory and the same sealed package.
Both runs use the previously recorded controlled environment; the unrelated
full inherited-environment startup failure remains unresolved.

The remaining supervisor completes0: WRITE retains visible MSWRITE_MENU,
MSWRITE_DOC and MSWRITE_PAGEINFO (observation, not editing acceptance), real
Win16 CBT completes0, and both Console/Window matrices pass all17 expected
results. No guest image is rebuilt or changed. The reviewed production diff
is three NTW16S files (+33/-9 lines), plus the49-line native ordering fixture;
original-source/mirror additional diff is zero. No other runtime fix is mixed in.

P11 publication `style-order-publish-r1` (PID32840,13:18:08Z) completes0 with
recoverable backups and all13 hashes equal across the sealed candidate,
assets/release and O:\winnt\system32. Only ntw16s32.dll and ntw16s64.dll change:

- x86 SHA256 C635871FD6E1A3CC3376D8200C8E4246591CA4119301FB7920E88162D03AF74D
- x64 SHA256 DC9A1B8195C3DB880E149106C8067F6E0454915234FEAA1996F6489560D00CB3

All57 other package files remain identical to P10: eleven product images plus
46 original guest/configuration companions. W16Style.exe is unchanged; its
28-page thin-client test passes against each newly built DLL. The publication
does not replace unrelated assets/winnt.zip. This is a bounded S2 baseline,
not S2/T447 closure, complete WOW recovery, or proof of WRITE editing/saving.

## Post-P11 discovery-to-direct-reader audit

Baseline7c01a09be is published. This pass performs source tracing only, not
another application failure search. Re-reading the post-P8 callback review
avoids repeating its rejected timer/enumeration scheduler proposal: ordinary
enumeration inherits the initiating execution context. The corrected
CallBack16 already uses the original protected resolver and no longer has
the obsolete acquire_vp failure branch; do not repair that historical finding
again. Native modal timer coverage remains path-dependent and unproved.

A separate object-publication boundary is still absent in current source:

| Native discovery producer | Selected path | Missing proof/edge |
| --- | --- | --- |
| EnumWindows, EnumChildWindows, EnumThreadWindows | Original wuwind.c thunks call native USER with original W32EnumWindowFunc. That callback truncates hwnd via GETHWND16 and calls original CallBack16. | No publication of the just-delivered native window before guest callback/direct reads. Inherited task ownership is not publication. |
| FindWindow | Original WU32FindWindow retains its Progman compatibility and calls native FindWindow; registered_find_window separately returns raw FindWindowA. | Neither path projects a newly discovered HWND before returning it to USER16. Keep the original compatibility logic; repair a shared boundary rather than duplicate the search. |
| Point/capture/focus/popup discovery | ChildWindowFromPoint and WindowFromPoint in wuwind.c; GetCapture, GetFocus, GetLastActivePopup in wuser.c | Same native HWND-to-GETHWND16 return pattern; classify together rather than patching FindWindow alone. |
| Dialog navigation | GetNextDlgGroupItem/GetNextDlgTabItem in wudlg.c | Native internal children may have changed since a previous graph snapshot. Include discovery/direct-reader visibility in the same census. |
| Already wrapped mutations | SetActiveWindow, SetParent, SetFocus and window-word setters | Existing native-call end refresh must be reviewed, not duplicated. A returned previous HWND needs explicit lifetime/return-value analysis. |

Source facts: walias.h USER16/GETHWND16 is a numeric WORD truncation, not an
enrollment call. wow_user_invoke_thunk establishes a borrow scope only;
wow_user_borrow_enter does not refresh the graph, and leave explicitly avoids
rescanning relationships. wow_user_window_refresh_graph is connected at WND,
dialog, hook and wrapped native-return boundaries, but the discovery APIs
above are absent from wow_user_private_access.h. Therefore an intervening
native/external window creation can be discovered after the previous snapshot
without a producer at its first guest-visible handle boundary. This establishes
the missing connection, not a measured application failure or a claim that
every discovery presently fails; existing snapshots can mask the omission.

Original owners remain canonical OpenNT base/mvdm/wow32/wuwind.c
(SHA256 AB270E2669CFD383DA4BAAC471BB63E7C9C603581F4F7592AD447E8A783953DC)
and wutmr.c
(E42BC0C7E9DC199AE19414461D65ECF86B7ACC55C6D7523792812D73F158BEF5).
Production /Gz/STD_CALL selection preserves W32EnumWindowFunc's callback ABI;
its absence of an explicit CALLBACK spelling is not an ABI defect.

Next dependency-complete group: enumerate all native HWND discovery and
callback returns, retain original search/enumeration/conversion algorithms,
and connect the existing single graph publication owner before the handle
becomes observable. No new window registry, low-word identity heuristic,
guest GetClassName replacement, scheduler or blanket CallBack16 interception.
Prefer owned native-call seams (expected new mirror diff zero), preserving
callback stop/result/last-error behavior and exception unwind. Before code,
resolve vanished-window/reuse races and publication failure disposition for
each return shape; do not invent successful NULL results or replay callbacks.
Acceptance must include native-created windows absent from the last snapshot,
guest direct queries inside enumeration, nested creation/destruction, early
stop, discovery failure and retirement. Existing CBT acceptance alone does
not prove these routes. This audit changes no runtime or guest and does not
claim the new group implemented.

### Discovery group implementation contract

Use an owned native-call seam for the three synchronous enumeration APIs and
the HWND query families, including active/foreground fallback producers.
The original thunks and W32EnumWindowFunc remain unchanged. Each enumeration
callback first publishes the exact full HWND through the existing graph
owner, then invokes the original callback exactly once with its unchanged
parameter. Do not infer guest execution ownership from the enumerated window:
the enumerating task owns execution. Acquire/release only the existing USER
data lock around graph publication, never around the guest callback.

Native NULL/failure stays native NULL/failure with its last error; no invented
window is published. Successful discovery must publish before returning. If
publication fails, return the API failure value with the publication error;
enumeration stops without calling that guest callback, and a remembered
publication failure overrides EnumChildWindows' native return convention.
Do not retry native enumeration or replay guest callbacks. Callback exceptions
propagate without a persistent callback context (the context is call-local).
On normal enumeration return refresh the graph to retire objects removed by
callbacks; preserve native result/error when publication succeeds. Existing
full-identity sampling rejects observed disappearance/reuse with no partial
graph commit. This does not promise a cross-process native USER tree lock.

The registered WOWFindWindow slot must reuse this same seam instead of keeping
its raw FindWindow wrapper. No original algorithm is replaced or duplicated;
modern USER remains native search/enumeration owner, original WOW owns its
conversion and compatibility cases. Mutation wrappers already doing return
publication stay separate and unchanged. The complete group requires native
and guest callback/direct-reader acceptance before any publication.

### Discovery candidate and first grouped acceptance

Owned wow_user_discovery.c supplies query/enumeration publication seams;
the private header selects them for original thunks, registered FindWindow
reuses the seam, and the provider build selects the new unit. No original
mirror or guest changes. The graph-snapshot CCPU fixture adds discovery
after a prior snapshot, child-ID reads inside callbacks, nesting, early stop,
exceptions, missing-owner publication failure and recovery. Native callbacks
are publication witnesses, not real Win16 discovery acceptance.

Hidden supervisor26204 (2026-10-15T13:33:55Z;45-minute deadline) owns
discovery-group-r1. Build child11080 completes0. Graph-snapshot child31936
reports WOW_DISCOVERY_CCPU errors=0 and WOW_PAGE_DOMAIN_LIFECYCLE_OK,
including retained native-slot reuse. Adjacent checks continue under the
same supervisor; no runtime candidate is published or committed yet.

Second-role source review requires another entry-state case: enumeration
with exclusive_held already TRUE. The current publication helper preserves
that incoming lock, but native/guest callout must release and restore it,
including exceptions. Reuse existing leave_for_callout/reenter_after_callout
or the established native-call boundary, not another lock or task selection
based on the enumerated window. Add held/unheld entry tests to this group.
The passing unheld fixture does not establish that additional contract.

Real-guest acceptance source now pairs independently authored
win16_discovery_probe.asm with wow_discovery_peer.c and the hidden runner
verify-win16-discovery.ps1. After InitTask/InitApp, the guest creates READY.DAT
and waits using original KERNEL _lopen (ordinal85), not a USER message loop.
Only then does the native peer on the observer's private desktop create a
STATIC window and signal GO.DAT. The guest uses original FindWindow50,
EnumWindows54 and GetClassName58, checks callback LPARAM and class bytes,
stops at the matched window, frees its MakeProcInstance and emits a14-byte
report. The runner requires zero errors, one match, two queries, cleanup and
normal launcher completion. Neither source edits nor replaces guest media.
This test is prepared, not yet accepted; the CCPU negative-space assertion
remains necessary because unrelated runtime activity might independently
refresh a desktop snapshot before a real-guest query.

The first supervisor completes0: all12 selected boundary cases pass, including
graph-snapshot, window-borrow/reclaim, process-retirement, dialog-lifecycle,
menu-resource/cleanup, thunk-scope, native-send/nested, task-lifecycle and
native-creation. Only after its terminal result are shared inputs revised.
Enumeration now brackets the complete native call with the existing runtime
callout unlock/restore pair, using finally for normal and exceptional returns.
The three APIs share that bounded spelling; they neither switch tasks nor
interpret callback handles as execution owners. Query publication preserves
its caller's existing lock. The CCPU fixture runs both held and unheld entry
states and asserts callbacks are unlocked and return/exception restores entry.

Second hidden supervisor29740 starts2026-10-15T13:48:10Z with45-minute bound
at discovery-group-r2. It builds the complete revised candidate, repeats the
affected real-CCPU graph group, then runs real Win16 discovery and three-mode
SOL/WINMINE acceptance. Package construction retains P11's other twelve
images and original companions, replacing only WOW32.DLL. Runtime delivery
is still pending; no stage or test outcome is inferred from launching it.

R2 stops at provider link: leave_for_callout/reenter_after_callout are worker
internal spellings, absent from its DLL exports. Their bodies simply call
the existing exported runtime leave/enter functions. The correction uses
those existing functions directly, preserving the same finally-based scope
without adding ABI exports or another implementation. No guest ran in r2.
R3 supervisor26428 starts2026-10-15T13:51:05Z with the same complete test
sequence and a fresh evidence directory; its build completes0. R2 failure
logs remain unchanged. Remaining acceptance and publication are not claimed.

R3 graph-snapshot passes both held=0/1 with errors0, including callback
unlock, normal/exception restoration, nested discovery, failure and recovery.
Real Win16 discovery also passes: report errors0, found1350, matched1,
queries2, freed1; native peer creates full HWND01290546 only after READY,
and guest/launcher complete normally. This adds a real original USER16
FindWindow/EnumWindows/GetClassName witness to the CCPU publication tests.

The same sealed candidate passes SOL Off/Win16 and WINMINE Off. WINMINE
Win16 then fails probe9: right-click changes cell/counter, but F2 reset keeps
the flagged cell hash62c0567b rather than covered3cc8733b for five seconds.
All-mode cases have not run because the supervisor stops at that failure.
Observer later reports exitFFFFFFFF, but runner finally forcibly cleans the
owned package after the failed probe; that is not independent crash evidence.
Do not attribute the timeout to a particular query without source/trace or
controlled baseline evidence. The complete application gate fails and the
candidate remains unpublished. P11 remains the runtime baseline.

### Owner-directed discovery delivery review

Owner requires preserving a compiled, tested, published and pushed baseline
before the next wiring group. Controlled comparison discovery-compare-r1
(hidden PID17992) runs the unchanged gameplay probe against sealed P11 and
the sealed r3 candidate with the same environment whitelist and Win16 mode.
Both pass flag/counter, F2 reset, cell opening and normal launcher exit;
reset takes62ms and63ms respectively. No production or probe change is made
to obtain these results. Original WU32TranslateAccelerator still forwards
the converted MSG to native TranslateAccelerator; the style menu filter only
consumes left/right navigation during active menu tracking, not F2.
These facts do not explain the earlier timeout or establish its root cause.
Retain that failure rather than describing a repaired accelerator defect.

Fresh hidden supervisor27256 starts2026-10-15T14:02:07Z, with45-minute bound,
at discovery-delivery-r1. It repeats all12 boundary cases on the revised
held/unheld implementation, then all three SOL/WINMINE style modes, three
WRITE initialization frontiers, real guest CBT and both DOS17 matrices.
The sealed r3 package is the only candidate; neither subsequent cache objects
nor diagnostic images replace its binaries. Acceptance/publication results
are recorded below only after terminal completion.

Second-role scope review: the three enumeration seams retain native callback
selection, original WOW conversion and task ownership. Only the existing
shared-view publisher is inserted before returning an HWND to USER16.
Stack-local callback state cannot survive the synchronous native call;
finally restores an incoming data lock even when a callback raises. Native
failure and adapter publication failure remain distinct, with no callback
replay or invented success. The registered FindWindow consumer is included.
No mirror, guest, wire or worker-export change is present. The independently
authored NE is a disposable test, not a replacement guest image.

This bounded P does not certify every HWND-producing API. In particular,
SetCapture's previous-handle result and GetWindowLong(GWL_HWNDPARENT) need
their mutation/relationship-owner audit before extending this seam; neither
is silently marked covered by a FindWindow test. Full callback-family,
WRITE editing/saving and later message/task obligations remain open in S2
and the existing working plan. Do not expand this delivery while its gates run.

The fresh twelve-case boundary sequence completes with exit0 for every case.
Graph-snapshot again reports errors0 for held=0/1, and its original CCPU
backing/reuse/retirement checks pass. Candidate WOW32.DLL is x86 Release
/O2 /MT /Gz, SHA256
5E782BC02845ABC20A908CB79C03A18EC851A3E68B0FA7537913A95391191A16.
The shared cache is reused under Ninja dependencies; the r3 sealed DLL,
not a later incidental link, remains the application/publication input.

The application sequence stops at SOL Win16 readiness (probe5): native menu
command1000 is enabled, but the sampled tableau never reaches the required
stable patterned pixels. SOL/WINMINE Off already pass. Keep this failed run;
the subsequent forced fixture cleanup does not prove a guest crash. The SOL
probe now logs last sampled hash/pattern and captures a readiness-failure
bitmap; success assertions, time bounds and input sequence are unchanged.
No production code is changed in response to either intermittent failure.

Controlled SOL comparison discovery-sol-compare-r1 (hidden PID10340) runs
the same diagnostic probe against P11 and sealed r3, both Win16 mode: both
pass readiness, deal, stock draw and normal exit. This is a successful
comparison, not a causal explanation or proof that intermittent failure is
fixed. The earlier WINMINE F2 and SOL readiness failures remain unresolved
acceptance reliability observations inside active S2; neither is attributed
to the original guest or waived as an original-media limitation.

Remaining checks run serially in discovery-remaining-r1 (hidden PID33192,
15-minute bound). All-mode SOL and WINMINE pass gameplay and normal exit.
The final gate composes unchanged Off results from discovery-delivery-r1,
Win16 results from the two explicit baseline/candidate comparisons, and All
results here, checking each run's input hashes against the same sealed r3
package. It does not relabel either aborted suite as an all-pass run, remove
failure logs, or claim a deterministic repeated-run reliability guarantee.

### S2 P12 publication and acceptance boundary

Remaining supervisor33192 completes apps-result0: All-mode games pass;
WRITE Off/Win16/All expose visible MSWRITE_MENU, MSWRITE_DOC and
MSWRITE_PAGEINFO; real guest CBT completes0. Console17 and Window17 each
contain17 expected/actual matches. WRITE is an initialization frontier,
not editing/saving or normal-exit acceptance. All twelve revised boundary
cases and real guest discovery retain the evidence described above.

Hidden publication supervisor33696, discovery-publish-r1, completes0 after
independently checking those results, input identities and documentation.
It preserves recoverable backups, copies the complete thirteen-image set to
assets/release and O:/winnt/system32, and verifies every SHA256 and PE machine.
WOW32.DLL is the sole changed image (5E782BC0...191A16 above); the other twelve
images and all46 original guest/configuration companions match sealed P11.
The release manifest records the full set. assets/winnt.zip remains unchanged
from the owner-owned dirty input and is excluded from this delivery.

P12 is a bounded discovery/publication baseline, not S2/T447 closure. The
two intermittent application-test failures remain explicitly unresolved;
the successful controlled comparisons and final per-mode results do not prove
their absence. Subsequent source-led work must preserve this baseline while
addressing the retained callback/object inventory and unexplained failures.

### Post-P12 producer and callback-failure audit

Baseline0581fe60e is published and pushed. Read-only source tracing, not a
new application test loop, classifies remaining HWND producers:

| Producer | Current path | Disposition |
| --- | --- | --- |
| SetFocus, SetActiveWindow, SetParent, SetWindowLong | Original thunk calls existing WOW_NATIVE_CALL; native_call_end refreshes the graph and menus. | Do not add a duplicate return wrapper. Review previous-handle lifetime separately from current graph reachability. |
| SetCapture | wuser.c WU32SetCapture directly truncates native SetCapture's previous HWND; no existing native-call seam selected. | Missing return-publication boundary; capture-change implicit callbacks also require owner review before wiring. |
| GetWindowLong/Word parent index | wuwind.c calls GetWindowLong(GWL_HWNDPARENT), then GETHWND16; owned private-access function passes public indices straight to native USER. | A returned external owner can be absent from the current view. Bind only this HWND-valued index, not arbitrary LONG values or procedure identities. |
| GetDesktopWindow/GetDlgItem/GetParent/GetWindow/GetTopWindow/GetNextWindow | wusercli.h selects LOCALAPI under PMODE32; usercli.asm uses original client code. | A host same-name wrapper is not a repair for these direct readers. Keep relationship producer completeness as their dependency; do not revive fallback thunks. |
| Clipboard owner/viewer/open-window and viewer-chain return | wuclip.c/wuser31.c also use GETHWND16 on native results. | Record shared-view prerequisite for W10; this is not clipboard-family acceptance or permission to replace its policy. |
| HWNDs inside messages/hooks/structures | wmdisp32, wstruc, wowhooks and other family owners convert them. | A query-only sweep cannot certify callback payload closure. Retain the owner-specific inventory and entry-boundary gates. |

More urgent source finding: callback delivery currently depends on a fresh
whole-desktop graph, not just the live target. In wow_user_object_bindings.c,
refresh_graph always roots the desktop; sample_graph follows parent, sibling,
child and owner edges, then rechecks every row. A changed sampled edge yields
ERROR_RETRY; disappearance or failed native metadata query also fails the
whole publication. This conservative behavior protects graph consistency,
but it is not a native desktop-wide lock.

In wow_window_words_binding.c dispatch_bound, refresh failure returns FALSE
before retained DispatchClientMessage. wow_window_native_proc ignores that
BOOL and returns its default result (zero, except WM_CREATE=-1). Its later
trace still labels the event WindowDispatch regardless of whether the
original callback executed. The hook entrance similarly skips the original
hook body on failed refresh. Original client.c DispatchClientMessage contains
neither this desktop sampling prerequisite nor this suppression policy.

Thus the code demonstrably permits a transient unrelated-window graph change
to suppress a valid target callback, and the present trace cannot distinguish
that outcome from a legitimate original callback returning zero. This is a
project integration/failure-contract risk, not an original guest defect.
It is a plausible contributor to the retained F2/readiness failures, **not
their established cause**: no failure-time error/branch witness was captured.
P12 remains the release baseline; no new runtime patch is made in this audit.

Next complete group must resolve publication consistency, transient failure,
callback execution and observation together before expanding the query seam.
Trace success must mean actual original callback execution, with failure stage
and error distinguished. Acceptance must force the failure boundary and prove
no replay, stale-generation access, silent callback loss or changed creation
veto/terminal cleanup. Native and guest branches, hook entry, native-return
and queue-publication consumers must be reviewed together. Do not fix this
by ignoring all publication failures, delivering arbitrary stale pointers,
blind message replay, fixed sleeps or unbounded sampling retries. The existing
graph failure-atomicity tests only prove that invalid snapshots are not
committed; they do not prove preservation of the pending original callback.
Original USER lock/shared-memory ownership is the reference contract; its
unavailable private server boundary must remain explicit in the design.

### Failure-contract implementation prerequisite

Canonical kernel/ssend.c LOCKPWND retains the target and callback-window slot;
MAKECALL releases the USER critical section during KeUserModeCallback and
restores it afterward. It does not require whole-desktop quiescence. Do not
equate a sampled-graph race with a failed original callback or solve it by
holding the project data lock across arbitrary guest execution.

Before selecting delivery recovery, restore meaningful failure evidence as
one owned provider/worker group. project_window_graph currently preserves
possibly stale last-error values on its Boolean validation branches; the
provider then overwrites every worker failure with ERROR_INVALID_STATE.
Classify invalid input, allocation failure, observed native changes and
structural/publication failures without changing atomic commit or retrying
native calls. Preserve the actual error through cleanup. Native window trace
must distinguish callback refusal from an executed callback returning zero.
Deterministic rollback tests assert both error class and unchanged storage.
This prerequisite changes neither scheduling nor failure recovery; it cannot
be reported as fixing the two application timeouts or the suppression risk.
No mirror, guest or new public/private export is needed. Complete this group
before grouped compilation; P12 remains the published runtime meanwhile.

The owned candidate now assigns explicit worker graph failures, preserves
them through provider rollback and callback cleanup, and emits WindowRejected
instead of WindowDispatch when the original dispatch did not execute.
An executed callback returning zero retains WindowDispatch and its own error.
The real-CCPU fixture forces a missing graph owner: callback count stays zero,
the window remains live, and ERROR_INVALID_STATE survives cleanup. Restoring
the owner invokes the zero-return callback exactly once; its0x55bb error
survives. Descriptor failure and stale relationship batches assert distinct
INVALID_PARAMETER/RETRY results with unchanged allocation/publication.

Hidden supervisor28692 starts2026-10-15T14:35:43Z with45-minute bound at
failure-contract-r1. Combined worker/provider compilation completes0;
graph-snapshot completes0, including both discovery lock states, rollback,
callback ownership and reuse. Adjacent boundary cases continue under the same
live supervisor; no restart or completed-suite claim is made. No candidate is
published or committed, and neither original mirror has changed.

Second-role recheck corrects the preceding allocation-review note: both HEAD
and the current provider already set ERROR_NOT_ENOUGH_MEMORY when descriptor
array allocation fails. No additional source change or rebuild is needed for
that branch. Preserve the larger callback-recovery decision: truthful rejection
evidence alone does not prevent transient publication failure from suppressing
a callback.

### Queue consumer and original-reader cross-check

Source review also identifies a post-operation failure boundary in
wow_user_task_lifecycle.c:672--722. peek_message_published executes native
PeekMessageA, possibly with PM_REMOVE, before native_call_end refreshes both
window and menu views. If that restoration fails, lifecycle_message returns
FALSE (Peek) or -1 (Get) even when native USER already removed a message and
wrote it into the supplied MSG. This is a source-proved loss-of-result path,
not a runtime witness attributing the earlier application timeouts. Retrying
PeekMessage is not recovery of that same operation; it can consume the next
message. The native-call macro similarly substitutes its failure value after
an already executed native operation. Window and menu failure must remain
distinguishable from the native operation's own result.

Canonical OpenNT windows/core/ntuser/kernel/input.c xxxInternalGetMessage
passes fRemove to xxxReadPostMessage, then invokes WH_GETMESSAGE and returns
the selected message (with WM_QUIT's distinct GetMessage result). There is no
subsequent whole-desktop snapshot gate. rtl/wow.c _GetWindow directly follows
the requested shared relationship and rebases it; _GetParent selects the
parent/owner by original style. Neither establishes a desktop-wide immutable
snapshot. These bodies are reference evidence, not permission to remove
generation checks or publish dangling copied relationships: original server
storage/lifetime is precisely the unavailable dependency.

The next recovery design must cover pre-callback admission and post-native
completion separately, retain the original operation exactly once, and retain
safe guest-visible object lifetime. It must not create a second message queue,
replay native operations, silently convert graph errors into successful calls,
or claim whole-desktop sampling provides original USER locking. Current tests
validate rejection/error reporting only; they do not establish this recovery.

The direct refresh consumers have now been reviewed together (owned sources,
not a claim that every message payload conversion is closed):

| Consumer | Operation boundary and current failure effect | Recovery constraint |
| --- | --- | --- |
| Window dispatch_bound | Refresh before original DispatchClientMessage; rejection substitutes zero or creation failure. | Retain target/borrow and terminal cleanup; no replay of the original procedure. |
| dialog_return_hook | After successful native WM_CREATE, refresh can prevent W32InitDlg and record creation failure. | Initialization must remain once-only; native creation already happened. |
| dialog_proc | Refresh before supplied original procedure; failure returns FALSE. | FALSE is also a legitimate unhandled-dialog result, not evidence of successful delivery. Preserve independent WND borrow and callback slot. |
| WU32StdHookProc | Refresh before retained hook body; ordinary failure returns zero, CBT creation failure vetoes. | A zero may affect hook-chain/notification behavior; preserve the existing enrollment rollback and real hook result separately. |
| publish_discovery/discovered | Native query already selected a full HWND; publication failure returns NULL. | Do not publish a truncated or unregistered HWND; distinguish absence from publication failure. |
| discovered_callback/enumeration_return | Entry can stop before one callback; final refresh can return failure after earlier callbacks already ran. | Never restart enumeration as though earlier callbacks had no side effects. |
| native_call_end and WOW_NATIVE_CALL_TARGET | Native operation has completed; window/menu refresh can substitute the wrapper's failure value. | Restore execution/lock ownership independently from data projection; neither implies permission to repeat the native operation. |
| peek_message_published | Native queue operation may already have removed MSG before the same return boundary. | Preserve exact message/result semantics without re-posting or a second queue. |

The existing callback scopes already retain owned WND/backing across nested
destruction. That proves local storage lifetime only, not that an arbitrary
last-good desktop graph is semantically current or safe for every direct
USER16 relationship reader. The recovery cannot simply remove all refresh
calls or downgrade every error to success. First separate required target and
payload publication from unrelated desktop reconciliation, with generation and
relationship lifetime accounted for; keep the original procedures and USER16
readers, not replacement per-message behavior.

The follow-up poll confirms the same boundary supervisor28692 remains live,
with ten completed case exits0: graph-snapshot, window-borrow, window-reclaim,
process-retirement, dialog-lifecycle, menu-resource, thunk-scope, menu-cleanup,
native-send and native-nested. Build/result exits are not additional test cases;
the earlier conversational count included the build and was one case too high.
The follow-on hidden supervisor23436 starts2026-10-15T14:45:28Z under
failure-contract-apps-r1 with a90-minute child bound. It waits for the existing
suite's successful terminal result without restarting it, then seals P12 media
plus the rebuilt ntvdm.exe and WOW32.DLL. Planned gates are real guest discovery,
SOL/WINMINE Off/Win16/All gameplay, WRITE's separate three-mode frontier,
guest CBT and both DOS17 routes. It neither publishes nor commits on success;
results, coherent image identity and the original failures still require review.

Boundary suite failure-contract-r1 is now terminal with exit.txt=0; all twelve
case exits are0, including task-lifecycle and native-creation. The checked trace
contains both WindowRejected/message8046/error5023 and the later actual
WindowDispatch/message8046/result0/error21947, matching the forced refusal
and executed-zero fixture assertions. This proves the diagnostic distinction,
not callback recovery. The follow-on application runner has advanced to package
staging; no application pass, runtime publication or commit is inferred yet.

Application suite failure-contract-apps-r1 is now terminal1 (supervisor and
child result agree; both process identities are absent). Real Win16 discovery
passes, as do SOL Off deal/draw/close and WINMINE Off flag/reset/open/close.
SOL Win16 fails the unchanged readiness assertion with probe exit5:
owned window00570DEC, PID18204, client585x384, enabled Deal menu;
phase=ready TIMEOUT pixels=1f13dc85 pattern=0. The retained
sol-Win16/sol-ready-failure.bmp shows a green client area with the bottom
Score/Time strip but no dealt cards. This is not evidence of a completely
uncreated window or a missing menu. No guest patch or probe weakening occurred.
Later modes, WRITE, CBT and DOS17 in this suite were not executed and are not
passing gates. Preserve the failure instead of restarting the same suite.

The candidate package differs from sealed P12 only in SYSTEM32/ntvdm.exe
(34b622deef8b5fb38064e7e41c3227c4ae07443adb6c1b7b8560e8b075d6b197)
and SYSTEM32/WOW32.DLL
(450f1548fc4bf4b9076db718bbd929653070e2cccd54cc683be945496c391690).
The remaining images/media/configuration compare identically. This failure
resembles the retained P12 styled SOL readiness failure, but similarity is
not attribution and no callback-rejection trace was enabled in this ordinary
application gate. P12 remains published. A subsequent diagnostic comparison
must hold environment/trace conditions explicit and capture actual callback
outcomes; a later pass cannot erase or explain this failed gate.

One bounded diagnostic pair failure-contract-sol-trace-r1 is now terminal0,
supervisor3144 (start2026-10-15T14:51:32Z), child9584. This is diagnostic
completion, not conversion of the ordinary failed gate into a pass. Both
sealed P12 and candidate complete the unchanged styled SOL gameplay probe.
Both use the same explicit standard-environment whitelist and the identical
MVDM_WOW_WINDOW_TRACE_PATH text; each resulting file is retained separately.
The added tracing changes timing/environment relative to the failed ordinary
run, so it is not evidence that the failure disappeared or was repaired.

Candidate PID8092 records95 WindowDispatch outcomes and one real rejection:
WindowRejected hwnd015804EC message0018 result0 error1237. The rejected HWND
is not the main game HWND005A0260 identified by the probe. Message0018 is
WM_SHOWWINDOW; error1237 is ERROR_RETRY. Earlier/later messages for015804EC
execute successfully, including creation, positioning, erase, paint and
terminal destruction. Its class is not captured by this trace; do not invent
an identity. Source search locates explicit ERROR_RETRY production in graph
sampling/revalidation. This is the first retained real-application witness of
the already source-proved callback suppression risk, even though this gameplay
run passes. The exact changed edge/stage and causality for the previous blank
tableau remain unproved. A passing visual probe therefore cannot certify the
callback delivery contract. Do not add more retries of the ordinary gameplay
gate as a substitute for repairing that contract.

Style-source review also confirms the P11 delayed-frame branch remains in
NTW16S: initially hidden normal frames postpone forced positioning until
WM_SHOWWINDOW, then activate_frame performs frame_changed/restore_client and
forwards. No new early-WM_CREATE positioning or SOL-specific branch was added
by this candidate. Those nested calls expose the existing callback boundary;
this review is not proof that all style ordering is correct or that the
recorded ERROR_RETRY originated from that particular nested call.

Second-role inspection of the final page_domain_replace_window_links boundary
finds an incomplete error contract in the candidate: validation/allocation
failure and cleanup in that helper can still leave stale last-error, even
though its caller now preserves errors. The owned source now classifies
malformed batches as INVALID_PARAMETER, absent/conflicting publication or
unsafe retirement as INVALID_STATE, and host allocation failures as
NOT_ENOUGH_MEMORY, preserving the chosen error through rollback. Success
restores the incoming error. The caller's obsolete blanket error seed is
removed. Retirement and atomic commit policy are unchanged; stale native
sampling remains a separate ERROR_RETRY category, not a generic recoverable
classification for generation/ownership failures.

The existing dead-graph CCPU fixture now asserts live-retirement rejection,
stale generation, duplicate rows, retained incoming edges and successful
commit error preservation alongside its allocation/alias atomicity checks.
No new test run is started for this isolated review correction: collect the
remaining complete failure/delivery group before rebuilding. The prior twelve
passing cases and sealed candidate describe the earlier inputs, not this new
source revision. No original mirror or guest changes are introduced.

### Reconciliation design review: reject an unsafe shortcut

Original client/wow.c GetWindow validates the input handle, calls retained
rtl/wow.c _GetWindow and converts its returned object to a handle. There is
no whole-desktop equality check. However GetPrevPwnd walks parent.child and
then next until NULL or the sought object; it has no cycle bound. Removing
the sampler's revalidation without replacing its consistency protection is
therefore not justified: independently sampled sibling rows can form a frozen
cycle which the unchanged guest reader cannot escape. Original shared native
storage and a copied guest snapshot are not interchangeable lifetime models.
Do not implement a blanket ignore-ERROR_RETRY or remove all edge checks on
the strength of the no-desktop-lock observation alone.

Original enumeration provides a more precise comparison boundary:
client/clenum.c BuildHwndList obtains a copied handle list from
NtUserBuildHwndList; its bounded retry is exclusively buffer sizing.
InternalEnumWindows revalidates each full handle immediately before invoking
its callback and skips disappeared objects. It does not demand unchanged
z-order after building the list. kernel/enumwin.c InternalBuildHwndList walks
the server child/next tree while ntstubs.c supplies the server receive scope,
then copies out only HWNDs. Neither callback replay nor a desktop-stability
retry policy appears in that contract.

Recovery ladder disposition at this point: the original client list iteration
is reusable for enumeration, but it does not supply parent/owner metadata or
the unlinked creation/terminal WNDs required by direct USER16 readers. The
server builder requires private PWND, desktop and USER serialization and is
outside the admitted server-import boundary. A public native enumeration
binding can supply a handle list, but cannot yet be claimed to supply an
atomic complete relationship graph. Investigate that narrower list boundary
before any sampler replacement; do not invent a second authoritative window
tree or silently drop callback targets absent from enumeration. This rejects
an unsafe candidate design, not the already admitted need to repair delivery.

Read-only source identities, relative to canonical windows/core/ntuser:

- client/wow.c: 1ece9db0e35238434843bb73ece92dd66000a44bc6b1148a08bcabf1329f8d9a
- client/clenum.c: 4ea923b41085e2fefbf0c65a18f4af7ecbb60d2e0f0dc652302ba31ab668c1ff
- kernel/enumwin.c: 1a28d712de37da806ad532ec5f60b08dda5af94abd29db3f32f15e55ce30664c

### Commit-level attribution of the added failure boundary

Git patch inspection, not a runtime binary bisect, identifies the relevant
introduction points:

- 6096f1b2c (S2 P2) already contains sample_links/sample_graph edge-change
  rejection. This identifies the sampler's origin, not yet the production
  callback gate.
- cef8ce2cb (S2 P4) moves descriptor preparation and final revalidation into
  the worker's atomic graph transaction. A pickaxe hit here for
  sampled_links_match reflects removed calls, not that helper's first origin.
- 984ee0441 (S2 P5) explicitly adds
  `if (!wow_user_window_refresh_graph(window)) return FALSE;` inside
  dispatch_bound before original dispatch. Its native_call_end changes also
  prepend graph refresh to both menu-publication paths. This is the direct
  introducing diff for graph-publication failure gating callback/native-result
  delivery; P12 adds discovery consumers but did not originate that gate.
- 32876c415 (S2 P1) introduces peek_message_published and its dependence on
  native_call_end after PeekMessage. At that point the publication failure
  already includes menus; P5 subsequently adds graph failure. Removing only
  P5's graph calls would leave the same post-consumption result-loss pattern
  possible on menu failure.

These are precise source attribution boundaries, not a claim that a particular
historical binary reproduces SOL's blank tableau. The original callbacks,
query readers and guest are not the added policy's owners. The complete repair
must separate restored execution ownership, safe view publication and completed
native operation results across both window and menu consumers. Reverting P12,
adding a SOL name check, or dropping only one callback gate cannot close it.

Menu-side closure review extends the direct-consumer inventory: the tagged
CallWindowProc branch in wow_user_registration_bridge.c bypasses
dispatch_bound and gates the original pfnWowWndProcEx on
wow_menu_native_refresh_message. Its native branch uses native_call_end.
wow_menu_native_bindings.c MENU_MUTATION and SetMenu similarly combine an
already completed mutation with publication/retirement in their return status.
wow_menu_client_refresh_all scans managed window menu associations, live menu
trees, then retires destroyed views; its failure need not precede all writes.
Thus the existing window graph's atomicity cannot be generalized to menu
publication, and a completed mutation cannot safely be replayed on FALSE.
The recovery review must include this direct subclass-call path and distinguish
menu association/tree commit from retirement, not only native_call_end.

The unresolved design boundary is now explicit: modern public USER calls do
not expose original desktop-wide lock/shared-heap retention. The current copy
producer cannot promise both a globally current native graph and unconditional
finite callback admission while unrelated processes mutate that graph. A
replacement that uses a structurally coherent older external-window snapshot
would need an explicit observable contract for those external readers; it must
not be silently described as identical to original live shared-memory reads.
Same-process creation, mutation, callback payload and terminal identity must
remain synchronous regardless. No relaxation or snapshot fallback has been
implemented or authorized by this review. Original guest readers remain intact.

The owner has been asked whether a coherent external-window snapshot may
replace exact live external relationship visibility while same-worker state
remains synchronous. Automatic goal continuation is not that choice. Pending
the answer, do not implement this semantic relaxation or claim it necessary
before exhausting equivalent bindings. Independent error-contract verification
continues without changing the pending choice.

The complete error-propagation revision, including final transaction rollback
and malformed-input assertions, is launched as hidden failure-contract-r2,
PID11364 start2026-10-15T15:02:14Z. Earlier suites are terminal before this
launch. Its owned children have30-minute build/10-minute per-case bounds;
the recorded150-minute supervisor budget covers the12-case sequence. It reuses
the existing dependency-tracked worker/provider caches, retains fresh evidence
and does not overwrite the sealed prior package. The new suite is not yet a
pass and does not publish. Shared build inputs remain frozen during execution.

### Original callback failure contract clarification

Read-only expansion of canonical ntuser/inc/ntcb.h SfnDWORD, together with
kernel/ssend.c and kernel/userk.h, distinguishes a real callback transport
failure from the added graph-stability prerequisite. SfnDWORD executes
LOCKPWND, MAKECALL, UNLOCKPWND, then CHECKRETURN. MAKECALL leaves the USER
critical section for KeUserModeCallback and reenters afterwards; UNLOCKPWND
restores CallbackWnd and releases the retained window before status checking.
CHECKRETURN rejects unsuccessful transport status, a wrong result size or an
unreadable result. ENDSEND(DWORD,0) supplies zero on that failure. Capture
variants also have allocation-failure and paired pool-release paths.

Consequently, original USER does not guarantee callback execution despite
resource/transport failure, and zero alone cannot prove execution. This does
not justify rejecting a callback because unrelated desktop relationships
changed: the inspected original path has no such prerequisite. Keep actual
zero results, structural/resource failures and added ERROR_RETRY conflicts
distinct. Do not replay callbacks or turn every failure into success. This
is source-contract evidence, not attribution of SOL's blank-tableau failure.

Procedure: inspect ntcb.h lines619--666, ssend.c LOCKPWND through
ENDSENDCAPTURE and userk.h CallClientProcA/W/ScSendMessageSMS. Same canonical
OpenNT revision as above; SHA-256 identities:

- inc/ntcb.h: eb09b59f4ab36bfbdda4f579a91f255ce8b011d533bb197e3e9c5ae95111adad
- kernel/ssend.c: e4b9600381683902a05ff8ba3e7e0f6876246a97960482f8b0bf6e51da601844
- kernel/userk.h: 9260e0ba3cc89974e10b3804ed66d18b9864f4ae7d1175cce6dde8ee79d66e8c

No original code is imported or modified by this clarification. The pending
external-view contract remains unresolved; this evidence does not approve a
stale-view fallback or weaken the required safe guest-reader lifetime.

### Owner-requested design decision audit: reject query-only refresh

Owner requires the hard dependency to be resolved before approving a repair,
not a design with direct-reader lifetime and failure behavior left for later.
The following findings invalidate the earlier proposed query-only refresh
as a complete equivalent solution; they do not authorize implementation of
an external-snapshot relaxation.

Source inspection: wow16/user/usercli.asm USER16CLIENTTHUNK emits a WOW32
fallback only when its callserver macro argument is present. The local
GETWINDOW, GETPARENT, GETTOPWINDOW, GETDLGITEM, GETMENU, GETSUBMENU,
GETMENUITEMCOUNT/ID/STATE and ISWINDOW declarations have no such argument.
The existing _wow16CsrFlag therefore cannot redirect them. PMODE32 is a
compile-time branch, not an available runtime switch. Historical generated
wowasmk.asm corroborates the local ValidateHwnd -> _GetWindow -> head.h path;
this additional inspection is source evidence, not a fresh guest disassembly.

Original rtl/wow.c further establishes that GetPrevPwnd traverses the
parent's child/next chain; _IsChild follows parents; _IsWindowVisible walks
parents until FNID_DESKTOP. Thus publishing only the callback WND is not a
closed reader dependency. A callback can perform these local reads without
another transition into WOW32. No callback-specific query list can predict
which foreign windows an arbitrary guest application will inspect.

The copied data necessarily has a visibility interval: if another process
changes a window after the last host observation and the guest next reads
only local memory, no existing transition communicates that change. Removing
the global recheck fixes one rejection policy but does not supply the missing
observation or retention capability. Calling this fully equivalent live data
would be incorrect. Likewise, borrowed projected memory does not pin native
HWND lifetime, and repeated IsWindow checks cannot supply that guarantee.

Modern API checks, read 2026-10-15:

- [EnumWindows](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows)
  is preferable to repeated GetWindow traversal; it does not provide a
  documented atomic parent/owner/class/menu graph or retain every HWND.
- [EnumChildWindows](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumchildwindows)
  handles reordered children and excludes children destroyed before their
  enumeration or created during enumeration. This is list-enumeration
  behavior, not a live shared-memory contract.
- [IsWindow](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-iswindow)
  explicitly warns of destruction after the check and recycled handles.
- [SetWinEventHook](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook)
  out-of-context delivery is asynchronous. It cannot close the direct-read
  visibility interval; adding it is not an equivalence proof.

Design dispositions: reject simple global-check deletion, query-wrapper-only
refresh, blanket ERROR_RETRY suppression, repeated native-operation execution,
and treating WinEvent delivery as synchronous shared memory. Original server
tree/locking is still outside the admitted composition boundary. A different
guest build or redirecting its compiled local queries violates guest
immutability. A snapshot design must explicitly specify its observation
semantics, including stale IsWindow results; excluding only relationship
freshness would understate the difference. It is not approved by an instruction
to finish this audit. The earlier no-new-thread/query-only outline is not an
implementation-ready design and must not be used as one.

Independent verification has now completed: failure-contract-r2 exit.txt=0,
build.exit=0 and all twelve named fixture exit files=0. Supervisor11364 is
absent on the elevated process check. This verifies the error-classification
candidate, not callback suppression repair or ordinary SOL non-regression.
The existing failed ordinary application run remains a failed gate; no new
runtime publication or delivery is made by this result.

### Comparative architecture research requested by owner

Question: resolve the alternatives rather than repeat that further design is
needed. This is an architecture feasibility report, not approval of changed
external-observation semantics or a runtime implementation.

Additional source evidence:

- Canonical ntuser/kernel/desktop.c, desktop-open mapping around1970--2041,
  maps pdesk->hsectionDesktop into the client with MmMapViewOfSection and
  records ulClientDelta. The original client is not periodically reconstructing
  a public-API copy. Private desktop section ownership and original layout
  are prerequisites, not simply a missing MapViewOfFile call.
- Current wow_user_client_view_layout.h asserts NT4 x86 WND176, CLS108,
  MENU64 and ITEM64 and exact field offsets. The page-domain converts object
  references into guest server-form addresses. Reusing arbitrary modern USER
  pages would not establish this ABI, even if those pages were readable.
- Original ccpu386/ccpusas4.c contains phyR/phy_r32, linear byte/word/dword
  reads, cross-page splitting, block loads and raw physical-address access.
  c_mem.h also routes CPU dword reads through phy_r32. Intercepting only the
  host c_sas_dw_at helper would not establish complete guest-read coverage.
  The existing SAS video dispatch is a device path, not an original USER
  metadata provider. Reusing its type for USER would conflate unrelated owners.
- Current lifecycle peek_message_published runs PeekMessage before
  native_call_end. Its caller returns FALSE/-1 when restoration/publication
  fails, even after PM_REMOVE. Retaining an already obtained message/result
  and refusing replay is mandatory independently of the selected view model.

Route dispositions:

| Route | Feasibility and decision |
| --- | --- |
| Original desktop heap mapped directly | Original algorithm is real, but depends on private server-owned section, object layout and lifetime. No supported same-shaped mapping provider has been established for this host. Reject as current implementation plan; do not claim all hypothetical private Windows mechanisms impossible. |
| Existing USER16 fallback/configuration | Source proves the needed local queries lack a callserver branch. No runtime toggle supplies it. Rebuilding/patching the guest is prohibited. Reject. |
| Intercept USER memory reads in CCPU | Can in principle observe accesses without editing guest bytes, so the earlier blanket impossibility claim must be limited to existing WOW entrypoints. It would require field/range conversion, scalar/block/cross-page/raw-access coverage, coherent split reads, lifetime, recursion and allocation-failure contracts. This is a new memory-provider profile, not a small callback adapter; no complete reusable original USER provider was found in the inspected SAS path. Do not admit under the current zero-mirror-diff repair. |
| WinEvent/polling refresh | Can accelerate observations, not guarantee synchronous direct-read visibility or HWND retention. Adds machinery without resolving the hard contract. Reject as correctness foundation. |
| Boundary-updated copied view | Compatible with the existing guest ABI and page-domain ownership, with zero intended mirror/guest changes. Recommend only as an explicit bounded compatibility model, not as identical live shared memory. Requires owner decision on that semantic model before implementation. |

The recommended model's declared differences must include direct-read
existence, relationship, geometry/style and menu observations of external
objects, not only window order. Freshness cannot be promised in milliseconds
without an additional execution/notification mechanism. A guest loop making
only local reads can remain on the same published external view until its
next serviced boundary. Public enumeration is neither an atomic full graph
nor a documented universal z-order producer; it must not be advertised as
solving that problem by itself.

Classification must use actual window/thread ownership and relation scope,
not the current observed-versus-managed storage flag. A native control in our
process can be observed; a mixed-process sibling list cannot be split into
independent authoritative trees. Same-thread owned state at synchronous
creation/mutation/callback boundaries is distinguishable from cross-thread
or external mutation. A promise that every same-process property is always
current would be too broad without all producer coverage.

For any future snapshot design, structural consistency means no dangling
guest addresses, valid representation/identity rules and terminating reader
chains. It is not proof that all fields coexisted at one native instant.
Safe copied-memory retention does not pin native objects. Actual operations
must still go through native USER and accept its invalid-handle result;
full HWND values help detect reuse but are not immortal identity tokens.
Any merge/reconstruction policy, external observation failure policy and
post-side-effect unsafe-state policy must be explicitly included in its
implementation admission, not hidden behind the word snapshot.

Research recommendation: reject the prior claim of a simple semantically
equivalent callback-only repair. Choose the bounded observation model rather
than broaden into private USER/server or CPU memory virtualization if the
owner accepts its explicit differences. Preserve original guest readers,
WOW conversion/scheduling and current backing lifetime mechanisms. No code
is changed by this comparative report; the equivalence requirement has not
been silently waived and S2 remains open.

### Route E approved and implementation begun

Owner now explicitly approves E's external observation semantics and directs
grouped implementation/test/publication/commit/push. The complete group design
is recorded in the existing T447 working plan under Approved E observation-view
repair group; Current's files/ABI field admits it. This supersedes pending
approval statements above, not retained failed evidence or the guest prohibition.

First connected structural boundary validates exact target identities and
acyclic next/parent/owner chains before guest allocation/commit. It does not
require symmetric parent/child membership. A new private copied-view export
reads the existing committed page-domain into caller-owned transaction scratch,
including retained historical target identities, class names and numeric
geometry/styles. It creates no persistent cache or native ownership. The
export is selected in the worker generator and paired fixture assertions cover
two-pass sizing, untouched insufficient output, values and error preservation.
The local/external merge and menu/result consumers are not yet connected;
this is not a functional E implementation or delivery. No incremental build
is launched for these substeps; rebuild/test the completed group together.
Existing r2 results apply to the preceding candidate, not these changed inputs.

### E connected group: initial verification

The earlier partial-implementation paragraph is superseded: window capture/
merge, copied-view commit, menu-tree preparation and post-native result
handling are now connected in the host candidate. No guest or original
mirror source changed. Complete design and file/ABI disposition remain in
the working plan's Approved E section.

`observation-e-r1` (supervisor19780, start2026-10-15T16:11:47.3938708Z)
completed with exit1. Both production builds and graph-snapshot passed.
Window-borrow's executable returned success with zero assertion failures,
but its PowerShell gate still expected the removed `stale_snapshot` marker.
The new contract tests missing desktop/structural rejection instead of later
native freshness; its emitted `missing_desktop` marker did not match that
old gate. Preserve this failed run; it was a checker migration defect, not
evidence of partial publication or a passing whole suite.

`observation-e-r2` (supervisor1956, start2026-10-15T16:19:05Z) rebuilds the
affected inputs with that gate corrected, plus independently asserted
external-churn and consumed-message failure cases. Its graph-snapshot case
passes:32 local position updates during a separate native process's window
creation/destruction; real low-word handle reuse with retained old backing;
and a child-process failure after exactly one PM_REMOVE consumption. The
last case deliberately removes required publication context after the native
operation and requires worker exit0xc0000420, not an ordinary returned false,
task-only exit or message replay. The normal path verifies restoration and
that the message is absent after return. These assertions exercise the
selected real worker/provider boundary, not replacement storage stubs.

The remainder of r2 and the separately owned, serial-dependent
`observation-e-apps-r1` product suite are pending at this checkpoint. No
publication or new P is claimed. Earlier ordinary styled-SOL failure and
diagnostic paired passes remain unresolved evidence, not silently erased
by these focused checks.

### E grouped verification follow-up (not a delivery)

r2 subsequently failed process-retirement's null-root error assertion: the
provider consulted an inactive page domain before validating its input.
Input validation now precedes that lookup. Its dependent apps-r1 correctly
refused staging. r3 failed the new retained-observation PID/TID assertion:
last reader release freed the identity record while WND remained published.
The existing backing record now lives until withdrawal, then follows the
same delayed-free owner. apps-r2 refused staging. r4 exposed a consequent
allocation-balance failure: zero-reader retirement deferred reclamation until
an unrelated class collection. Retirement now attempts eligible reclamation
immediately, retaining failures without reversing logical withdrawal.
apps-r3 refused staging. These failures are preserved under build/M0-T447/S2.

r5 (supervisor32756, start2026-10-15T16:47:03.6784226Z) rebuilds and runs the
whole boundary group with those changes and completed menu observation/
retirement handling. Dependent apps-r4 (supervisor30384,
start2026-10-15T16:47:20.5072627Z) only stages after boundary success. Results
are pending here. The new CCPU menu fault test distinguishes logical update/
retirement from failed physical reclamation and verifies retry releases all
storage. An external-parent/local-child copied menu test verifies safe null
edge withdrawal; existing local incoming-reference tests retain ERROR_BUSY.
No guest or original mirror diff and no publication is claimed.

### E boundary-group completion and second-role source review

observation-e-r5 now terminates exit0: worker/WOW32 incremental production
builds and all12 boundary cases pass. Reproduce with
tests/observation/verify-wow-task-lifecycle.ps1, ProviderBuildRoot
build/M0-T445/S6/provider, WorkerBuildRoot build/M0-T445/S6/r003, a fresh
BuildRoot and each Case below. Production uses MSVC Win32/x86 /MT CCPU40;
the selected generator/build logs and commands are retained in r5.

| Case | Observed boundary evidence |
| --- | --- |
| graph-snapshot | Separate-process churn/local32 updates; captured external objects destroyed before commit; actual slot reuse; structural failure rollback; PM_REMOVE exactly once and explicit post-native fatal boundary. |
| window-borrow | Native/control shared backing, nested/exception borrow, original class words and allocation balance. |
| window-reclaim | Fault-injected physical window/menu free: consumed once, logical commit intact, retry and complete reclamation. |
| process-retirement | Startup rollback, task/object/class refusal/retry, event and native release. |
| dialog-lifecycle | Selected dialog callback ownership, nesting and cleanup. |
| menu-resource | Original resource/menu producer, tree rollback, native tracking, system-menu observation and copied attachment retirement. |
| thunk-scope | Selected retained callback thunk and scope cleanup. |
| menu-cleanup | Original task/process cleanup ownership; borrowed observations not destroyed. |
| native-send | Native result114 and original task ownership restoration. |
| native-nested | Nested native-send restoration without duplicated dispatch. |
| task-lifecycle | Standard-class admission/retry and native-direct lifecycle. |
| native-creation |51 callbacks; nested/rejected/vetoed creation;7 destroyed; object table empty. |

These cases are host/CCPU foundation evidence, not substitutes for real
guest acceptance. apps-r4 has begun after r5 success; product publication
remains gated on its results. Documentation governance and diff/mirror check
pass in observation-e-docs-r2; r1's sandbox supervisor disappeared before
starting the check and is explicitly recorded as a failed launch.

Separate review pass checked the actual source diff: normal WND/dialog,
hook, enum/discovery, tagged procedure and native-return callers converge
on the existing graph boundary. The obsolete global freshness sampler is
removed, not retained behind a mode switch. Captured view and menu batches
prepare before non-fallible commit; identity responsibility remains distinct
from native destruction ownership. Deferred frees use backing lifetime
records rather than phantom reader references. Original algorithms, guest
code, message transport and task scheduler are unchanged. New private worker
exports require paired ntvdm/WOW32 deployment; no broker/public CLI ABI change.
Additional mirror diff remains zero; unrelated assets/winnt.zip is excluded.

### S2 P13 E delivery

Baseline runtime is P12 (0581fe60ec31f2f21697dcdd873650ebe3ceecec), with
subsequent design-only HEAD c685b1bc0. Complete group builds/verification:

- observation-e-r5 starts16:47:03Z, ends16:59:15Z (about732s), supervisor32756,
  exit0. Both production builds and all12 boundary cases pass. All compiler,
  linker and case invocations are background-owned with individual deadlines.
- observation-e-apps-r4 waits for that exact successful owner, then tests its
  sealed59-file package; exit0 at17:03:17Z, about242s after boundary completion.
  Real guest discovery and CBT both pass. SOL Off/Win16/All verifies deal,
  stock/waste mutation and WM_CLOSE; WINMINE Off/Win16/All verifies flags/
  counter, reset, open cell, WM_CLOSE and launcher completion. WRITE in all
  three modes retains visible MSWRITE_MENU/MSWRITE_DOC/MSWRITE_PAGEINFO,
  not proven editing. Console17 and Window17 each have17 expected=actual rows.
- No production tracing variables are enabled in this application suite.
  Its recorded sanitized child environment and physical O:/tmp stages match
  the previous controlled acceptance conditions; user/global environment is
  untouched. Existing intermittent failed SOL evidence is not erased or
  assigned a new causal explanation merely because this candidate passes.
- observation-e-publish-r1, supervisor29980 start17:04:03.7865598Z, exit0,
  validates source/package hashes and individual runtime gates, backs up the
  complete release set, publishes both assets/release and O:/winnt/system32,
  and verifies13 SHA-256/PE identities. The unchanged57 package inputs comprise
  11 host images and46 original guest/config companions. Recoverable backups
  and verified-manifest.json are retained. Documentation/link/diff gates pass.

Changed release identities:

| Image | SHA-256 |
| --- | --- |
| ntvdm.exe |0A8D766FE358CFF42BF947C0993B8355B831D4772B30C0838D64CE8BAAC50B2D|
| WOW32.DLL |C373342072F546DC716BD28EA0012D17666C7366AED60DD574067FCF8BDFB6EE|

The full design is the working plan's Approved E section, adopted by the
architecture/rules contract. No original mirror or guest change; only the
private paired page-domain interface changes. Original USER16 readers and
WOW conversions/scheduler are retained. External observations explicitly
remain boundary-updated rather than original live shared-heap equivalents.
This is the bounded E group delivery, not S2 or T447 closure. The unrelated
assets/winnt.zip modification remains excluded and unchanged at SHA-256
539A910247C5478BDE19FB2A431CD4C5AB26355CDC46F8160F49B23F060B495B.
