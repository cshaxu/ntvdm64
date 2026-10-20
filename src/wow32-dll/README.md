# WOW32 DLL target component

ADAPTER-WOW-103 completes CallMsgFilter's existing native execution boundary
for both ANSI and Unicode consumers. The original ANSI WUMSG entry currently
imports CallMsgFilterA directly, while the existing W wrapper is bound only
by the control-client facade. Reuse the same WOW_NATIVE_CALL_TARGET owner/
return mechanism and expose both mappings through wow_user_private_access.h.
Keep native A as A and W as W, target HWND and result/error semantics; do not
route ANSI through Unicode, add a scheduler, clone messages or reinterpret
native private state. This is an existing S4 M1/M4 dependency, not new scope
or proof of DDE packet retirement. Include both ABI variants, original caller
bindings, normal/nested/failure execution return and the whole DDE cohort
before product delivery. No mirror, guest, public ABI or wire-layout change.

ADAPTER-WOW-102 handles one specific precommit native-observation race.
Public HWND queries do not acquire NT4's USER lock, even for windows in this
worker. Exact-source/pure-native next/parent/owner witnesses prove that two
valid reads can form a transient local cycle. Mark only that normalization
outcome, discard the uncommitted transaction, and reacquire once. Keep all
existing identity, mandatory-root, graph and commit validation; other errors
and a second raced acquisition retain the original failure boundary. Restore
entry LastError before reacquisition. No sleep, stale local fallback, native
mutation, persistent state, retry manager, mirror/guest or ABI change. This
repairs acquisition, not an expanded snapshot compatibility contract, and
does not by itself attribute the earlier Shell Ignore/error13 failure.
Require immutable-source counterfactuals, all three real native interleavings,
normal/error/second-race/commit disposition and full composed runtime floors.

ADAPTER-WOW-101 pairs the finite call-owned conversion group (mirror349).
Existing WM32MSGPARAMEX scratch retains completed original heap/global keys;
normal releases and existing abort finalizers consume those same keys before
calling original free16/GlobalLock16/GlobalUnlockFree16. CREATE/MDICREATE keep
their exact nested class/title/record owners; clipboard temporaries distinguish
locked from already-unlocked handles. No output copyback/result translation/
foreground activation is replayed during abort, and DDE/COPYDATA retain their
separate original protocol ownership. A call-local TD cursor/INIT snapshot
covers constructor failure before CallBack16; only failed active same-task
invocations restore it, preserving normal callbacks and terminal SignalProc.
No new allocator/registry, task state, message transport, ABI layout, guest
change or private modern USER object interpretation. The existing original
conversion/stack owners cannot be replaced by a lower allocation wrapper;
the adapter records acquisition/consumption only, not control/window behavior.
All actual window/dialog/hook/nested callers and delegated control heap owners
are one source-derived group, with original-body and real-guest verification
required before a changed runtime is published. Native capture28 is a design
prerequisite, not runtime acceptance for this correction.

ADAPTER-WOW-100 completes the S4 callback-abort cohort's reached native
hard-error boundary. The existing worker dialog's initialization SetFocus can
synchronously enter another WOW task before first-pass exception unwind pops
the current ready callback. Reuse blocking wow_user_native_call_begin/end in
wow_user_thunk_scope around WOWSysErrorBox; preserve parameters, response,
LastError and exception propagation. Bind all three actual WOW consumers
(wow32.c, wuser.c, wudlg.c) through their compile-time service name. Worker
presentation, original nt_error.c and DOS callers stay unchanged. No new
window/thread/manager, invented response, task-lock override or guest change.
Existing nTaskLock/native-call policy remains; no new compatibility waiver.
Original348 captures bounded control/cursor state before its first resolver,
since error UI can dispatch other tasks before a preparation-fault unwind.
Complete service/failure/actual-guest/style proofs before the composed rebuild.

ADAPTER-WOW-099 supplies only the original callback's bounded control-context
facade and dispatch-filter binding. Capture code CS/IP and PE/VM/CPL in native
call-local storage; abandoned-call restoration reloads CS/SS through original
CCPU setters against CURRENT descriptors and restores the caller's SP/IP.
Do not restore CR2/CR3, descriptor tables, general registers, device/paging/
NPX/A20 state or guest bytes. The existing worker-active query distinguishes
terminal abandonment before any guest/CPU restoration. Broken required
selector restoration uses the existing no-resume adaptation-failure boundary,
not false API success. Existing W32Exception chooses the disposition; its
filter temporarily binds the known API entry, then restores the in-flight TD
pointer until ordinary unwind finishes. No new TLS manager, queue or scheduler.
The worker private import list exposes eight already selected original CCPU
mode accessors, not new implementations or a guest/wire ABI. Mirror348 keeps
actual frame and simulator ownership. Whole-group normal/fault/nested/terminal
proof and retained13/installed acceptance remain mandatory before publication.

ADAPTER-WOW-098 extends the existing message-lifetime finally owner for only
the original input-only text/CB_DIR acquisition records (mirror347). Invoke
their original pure-release unthunkers only while this call still owns a
reference/view/buffer. Do not add a map, allocator, output-copyback engine,
callback/frame restorer, DDE transfer rule or new export. Normal paths clear
progress before reentrant releases; exceptional cleanup preserves LastError.
No guest or public ABI change. Full M4 and callback-frame closure are separate.

ADAPTER-WOW-097 pairs all seven original GUI COMMDLG native leaves with the
existing borrowed execution scope. Original wcommdlg conversion, COMMDLGTD,
Hook/resource cleanup and results remain unchanged. Only that original owner
receives SDK aliases; the adapter alone calls the seven real ANSI APIs.
FindText/ReplaceText publish their returned window through the existing scope.
GetFileTitle/CommDlgExtendedError are not GUI dispatch boundaries. Preserve
LastError, nested execution and exceptional unwind; no new scheduler, helper
window, mirror or guest change. This is a release-blocking execution repair,
not evidence that COMMDLG causes WINMINE's residual cell-click latency.

ADAPTER-WOW-095 (bounded S4 P13 delivery) supplies finite finalizers:
invoke the original selected unthunker on exceptional exit, release an owned
filter MSG16 and restore original private recursion/mode snapshots. Original
COPYDATA conversion, node identity, native/guest allocation and normal cleanup
remain in their selected MVDM owners (MVDM-HOST-DIV-344). Existing call scratch
tracks owned versus borrowed payload; no allocation registry, transport or
callback engine is added. Per-thread scoped flags are operation-private, not
window facts or source-of-truth duplication. Guest/media remain unchanged.
The original thunk and Hook entry scopes restore the original TLS modes and
Hook context; each original message caller finally delegates only outstanding
COPYDATA acquisitions back to its original unthunker. Native procedure/input
conversions remain original. No copied-payload ownership escapes a filter
callback: an absent original native pair was a NULL dereference, not a valid
native MSG transfer. Reject that modified-output conversion, retain the input
MSG and roll back its new allocation. Actual pointer-changing posted-message
support is not inferred. Normal same-active-pair forwarding remains supported.

ADAPTER-WOW-094 removes an overwritten first sample of exact required roots
inside the existing copied-window transaction. Its temporary index marks full
identities only during observation compaction, then is cleared/rebuilt before
normal row lookup. Every required root still receives its ordered final fresh
native capture; other local rows, payloads, generation conflicts, validation,
failure atomicity and original guest-return boundaries remain unchanged.
No persistent cache, allocation, mirror/guest change or speedup claim is added.

ADAPTER-WOW-093 receives native sent notifications at the scheduler wait
boundary, before task selection can gate their arrival. Existing native
frames and original callback scheduling retain execution ownership. A native
wait continuation is not a guest return; selected guest returns keep the
existing full publication only if an omitted native-wait publication remains
outstanding. A successful ordinary return discharges it; failure/unwind does
not. No-wait or already-published scheduler returns do no extra work.
Initialization uses the existing input-plan lifetime as its no-receive
boundary, without maintaining a duplicate initialization flag.
Receive synchronization no longer does a second unfiltered native peek.
Posted/quit messages, original scheduling and task locks remain with their
existing owners. No hidden window, parallel queue, mirror or guest change.

ADAPTER-WOW-087 restores the original per-thread client projection when
taskman/InitTask transfers execution (OPENNT-HOST-124), not when a native
thread merely acquires the USER resource lock. Original pClientInfo owns
version; the inactive callback pair is a non-owning projection whose existing
scope retains backing. Initial/failed initialization and unrelated task
retirement no longer overwrite the running task's global guest TEB. A same-
task entry retains its live callback view. Terminal abandonment discards only
that dead thread's inactive snapshot without reading a withdrawn CPU view;
the next execution owner restores before guest resume. The parent validates
and exchanges only version/callback fields, preserving shared desktop state.
This adds no scheduler, object owner, hidden HWND, guest patch or semantic
waiver; the old unlocated0102 witness remains separately unproved.

ADAPTER-WOW-084 removes retained synthetic input queues from task carriers.
Original input.c AddAttachment/Recalc2 remains the exact membership algorithm;
preallocated call-local Q labels are its scratch graph, not sampled or invented
native USER queue state. RecalcThreadAttachment's private kernel-Q allocation
strategy cannot describe public USER resources and is no longer a production
initialization provider; its original unit coverage remains. Native USER owns
input/focus/capture via AttachThreadInput. This owner records only its acquired
pair increments and maintains every pair in each computed component, avoiding
an anchor/middle-exit dependency. Repeated membership does not attach twice.
Per-task wake observations remain separate; no label pointer survives commit.

Original input.c AttachToQueue/ReattachThreads/queue.c DestroyQueue additionally
own unavailable kernel message lists, PWND/RIT state, journal and foreground
queues. Importing those entire functions would require a second USER subsystem;
copying only a pointer assignment would omit native input sharing. Reuse their
component owner, supply native resource operations, and delete embedded shared
queue/zero-count-free/recalc-only substitutes together. Prepare all fallible
plan storage before original InitTask; preserve its original scheduling and
notification retry. Partial native mutation failure uses existing terminal
adaptation failure, never replay or false success. Retire only owned links
before closing the task's native thread capability/freeing the carrier.
No guest, mirror, message-transport or scheduler change is introduced.
Verification must cover complete native membership/focus, every departure,
rejoin, plan failure, owned versus third-party link references, native failure,
no scratch escapes and current full lifecycle/guest/application gates.

ADAPTER-WOW-082 applies the already approved route-E external observation
boundary to repeated publication. A committed, live external full HWND with
a complete descriptor reuses its existing page-domain snapshot instead of
re-querying every property after each local/native operation. Local worker
objects, cold rows, explicit roots/typed payloads and newly reached identities
still capture native state before the same structural validation and atomic
commit. Dead full identities are still withdrawn; copied retention does not
pin a native HWND. No new cache, freshness timer, state owner or mirror/guest
diff. Native USER remains the authority and required native calls still use
its real identities/results. The existing call-local boundary also retains
the explicit operand's known full HWND and publishes it on native return;
this target is not optional background observation. Verification must show stale external observation
does not hide local updates, explicit external discovery refreshes, and dead/
reused identities and nested readers remain correct, plus paired input timing.
This registers a candidate within the existing approved semantic boundary.

ADAPTER-WOW-079 pairs original native-extension entry and callback return.
The existing native/callback LIFO scopes remain the sole execution carrier;
there is no new task table, borrowed-pointer manager or scheduler. Inline
scopes enclose original WK32ICallProc32/WMM32CallProc32 without changing raw
argument order, callpr32 assembly or its convention/stack recovery. Native
LoadLibraryExA/FreeLibrary/GetProcAddress aliases are restricted to wkgthunk.c;
LoadLibraryEx remains inside its original x87 save/restore. Original exported
Yield/DirectedYield bodies run inside a READY scope and restore the caller's
outer native state. DIV343's original-shaped CallBack16 facade admits before
the first original guest frame access and always leaves on unwind. Existing
earlier pre-marshalling callbacks keep their guards. During established dying-
task cleanup only explicit kernel memory/resource RET entries may use the
existing early resource-live admission; application/Hook/WinExec callbacks
retain ordinary rejection. Normal RET behavior is unchanged. First usable
recovery rung is original bodies through these bounded services; frame-only
definition rebinding is the necessary minimal mirror seam, not a new callback
engine. Whole-group unit, actual guest extension and retained runtime floors
remain required before publication.

ADAPTER-WOW-078 reconnects the selected original class-unregistration client
and the historical HWND input of the native thread/process query. Original
wuclass/wuwind/walias bodies remain unchanged: UnregisterClassA enters the
already imported ntcftxt/class removal chain, including client-menu release,
instead of deleting only the native registration. GetWindowThreadProcessId
normalizes short inputs with the existing original handles.c owner, then asks
native USER for real identities; it neither invents task IDs nor walks a new
registry. Caller data locking/last-error/output failure behavior is preserved.
Published class-release failure after native unregister is terminal, not an
ordinary retry of a consumed operation. Source audit and native-ABI evidence
are in the T447 S4 record; whole-group verification/publication are pending.

ADAPTER-WOW-077 stages the complete native-stack execution boundary: the
existing bound THREADINFO carrier retains only a LIFO pointer to call/callback
stack scopes, independently of its lifecycle allocator. No readiness vtable
or second private task lookup is retained.
The same call boundary restores caller-held USER locking even before InitTask;
no guest execution frame is fabricated for resource-only entry. Queue setup
uses this common path instead of retaining another lock-release pair.
Native USER is the sole message/reply owner; no scope claims received SMS
identity. Original taskman consumes the persistent eligibility input through
OPENNT-HOST-123 and retains its original scheduling/event/lock algorithms.
Native return and actual callback entry make that stack ready; callback return
restores the previous native state. Synchronous same-thread calls inherit
execution, while an actual different-task callback can park an owner already
inside a native frame through original nonblocking SleepTask. The USER data
lock protects all transitions; no pointer survives the matching call/retirement.
Remove the old nominated-receiver/psmsSent pseudo-reply implementation in the
same group. Pair nonblocking queue/enumeration/next-hook scopes, native modal
calls, wait, exceptions and terminal cleanup. Original conversions are retained;
there is no universal marshaller, hidden window, timer or new transport.
Original sendmsg's private kernel delivery cannot be imported without creating
a second transport; directly selected original taskman plus this finite host
input is the first usable recovery rung. Group verification and publication
are pending, not a proven S4 capability or new semantic exception.

ADAPTER-WOW-032 / MVDM-HOST-DIV-340 relocates the existing default-off
W32WndProc entry/return diagnostics into wow_user_thunk_scope.c. The original
caller retains only raw scalar capture and two calls at the same boundaries.
Keep exact text, environment opt-in, success/result observation and LastError
on disabled/failed-output paths. No callback, policy, heap or lifetime owner
is added; the existing original procedure and scopes remain authoritative.
Direct production-owner diagnostics and real Win16 Hook/window callbacks are
required alongside the complete affected cohort before publication.

ADAPTER-WOW-076 canonicalizes the original signed16 HWND carrier at shared
original-dialog/control entries and immutable guest entries using the exact
binding's retained identity. Ordinary guest windows without original host
state use their existing published window
owner's full identity instead; no original state is fabricated. Native
property lookup validates the input first; strict graph/callback publication
still consumes full generation identities and rejects stale full handles.
This covers original EndDialog, dialog/default handling, dialog API callers
and saved control/guest procedure invocation. Native-procedure pass-through
keeps its caller's original argument; only retained behavior entries normalize
without changing original bodies, native fallback, original scheduling or
the lifetime owner. No new table, handle policy or guest/mirror change.

ADAPTER-WOW-075 retains a trusted resource-only destroy association in the
existing exact HWND binding. Original Find/Replace callback initialization
registers its existing DIV335 wrapper; normal destruction consumes it, and
neutralized task-terminal WM_DESTROY invokes it once under early resource
cleanup permission, outside publication locks. It is not a second COMMDLGTD
registry or a saved application-procedure fallback. Original resource/PDLG
cleanup stays with its existing owner; no guest or mirror delta is added.

ADAPTER-WOW-074 binds NTW16S's optional ComboBox information query to a scoped
copy of the actual original CBOX. Native USER's GetComboBoxInfo owns only native
control data and cannot supply this recovered control's children or rectangles.
Original editrc/buttonrc and retained spwndEdit/spwndList remain authoritative;
no new control algorithm, persistent projection or native-private cast exists.
The query rejects cleanup, stale child identity and foreign GUI threads, restores
task/borrow/error state and invokes no application callback. Untouched native
controls keep GetComboBoxInfo. Bind the additive DLL export before style readiness;
retain WOW's code for the existing DLL/callback lifetime. Mirror/guest delta0.
Whole control/template/native/style acceptance remains required before delivery.

ADAPTER-WOW-073 binds canonical terminal cleanup to the matching NTW16S
Ntw16sWindowTerminal endpoint after original control/dialog finalization.
Recheck the exact binding/property generation after those reentrant finalizers;
never hold the publication lock over DLL resource release. The DLL shares its
existing same-thread disposal owners without application notifications or
appearance restoration. Off/unloaded style has no DLL dependency. No original
mirror or guest changes are required. Native USER still owns actual window
destruction and native callback exception policy; project service exceptions
which propagate back retain the existing original execution/unwind contract.
Uncaught native WNDPROC exceptions are not a supported outer-catch guarantee
on WOW64. Do not change process exception policy or hide guest/adapter errors
to make a native fast-path-specific test pass. Real-DLL, native callback and
controlled service-unwind tests remain required; this row is registration,
not acceptance or publication.

ADAPTER-WOW-072 selects the missing native x86 entry for an immutable original
WOW procedure/callback pair in wow_window_procedure_entry.c. Core r3 builds;
CPD/native-subclass, class query and native task/send rows pass. Full grouped
acceptance remains pending. Original DispatchClientMessage,
W32Win16WndProcEx and CallBack16 remain the behavior owners. The entry uses
the native stdcall stack shape and an immutable descriptor; its identity does
not depend on an HWND's later procedure selection. Windows owns the actual
WND procedure and its native CPD normalization. Original CLS owns class
behavior; its native registration is a creation/resource carrier. No native token is decoded or
retained by this entry cache, and no auxiliary HWND is allocated.

Source reuse disposition: original USER's CALLPROC_WOWCHECK/DispatchClientMessage
can consume an encoded guest target, but modern USER cannot call that word as
a native function. wsubcls.c emits a16-bit guest thunk, not a native x86 entry;
MFC's HWND lookup is not immutable saved-target identity; APIMON's entry and
tracing stack do not provide the required WOW ABI. The smallest missing seam
is therefore a12-byte native stack adapter plus immutable target allocation.
RW construction, RX publication and instruction-cache flush precede exposure.
Allocation/protection failure must precede any native WND mutation. Entries
last for the existing worker/provider lifetime, as saved original procedure
values may outlive a particular window/class; there is no fixed-capacity limit.

This staging is not a CPD repair or native-slot migration delivery. Typed
guest/native services and original get/set callers, guest registration,
original class selection, creation/adoption and task-terminal observation are
now wired in source. Original get/set return paths retain decoded guest origin
as well as its value (DIV-339), including selector7fff versus native CPD.
Thread admission/rollback/retirement owns the terminal hook; retained exact
window storage survives downstream hook reentry. Creation rollback records
the actual installed procedure and does not overwrite an unrelated replacement.
The old WND-current cache, mutable native forwarder and fixed current-target
dispatcher are deleted; fixtures now use actual slots and explicit origin.
Original zero-window class restoration is retained rather than replaced by
an HWND-dependent native setter. OPENNT-HOST-122 connects original cleanup's
five storage writes and one typed WND selector guard. Canonical finalizers own
their existing task/borrow scopes; native-only retirement does not acquire
guest execution. Core r3 retains failed test composition/observability rows
and the native callback-policy finding; their coherent remediation is pending.
Do not interpret historical r15/app results as evidence for these changed inputs.

ADAPTER-WOW-063 dialog initialization now restores the actual native dialog
procedure in finally, including exception propagation. Initial mutation
failure returns without consuming the slot; any restoration failure uses
the existing terminal adaptation boundary. The original creator and
native slot authority are unchanged. The supplemental initialization fixture
uses real A/W subjects with explicit setter/scheduler fault doubles; the
separate native-owner probe retains actual USER calls. Neither is guest proof.

ADAPTER-WOW-071 stages the native address-owner boundary required by original
kernel/class.c::_RegisterClassEx and identity classification at class/WND
replacement and CallWindowProc. A scoped native class registration/query
validates the token without an HWND, private USER layouts, persistent token
cache or encoding inference. Class setters retain the resolved address and
their ANSI entry flag, matching original _SetClassData; WND forwarding still
retains an opaque token and its source-independent lifetime remains open.
Unrecognized WOW-tagged words retain original dispatch. The call-local target
snapshot projects whether the stored target is WOW-encoded, including the
selector7fff/ffff collision; it is not a second persistent state owner.
Resource/query/cleanup failure blocks mutation. Existing original registration,
lookup, cleanup and dispatch bodies are unchanged. Source and native evidence:
docs/etc/evidence/m0-t447-s3-control-source-audit.md (class-address-r1).
This staged group has no new mirror/guest diff; integrated validation and
coherent product delivery remain pending.

S3 full-link closure selects OPENNT-HOST-079's original clres template/modal
entry as a separate object; wow_title_modal invokes its private body name,
not its own public redirect. Existing task/borrow/context ownership remains.
The build also wires the two existing parent callout-transition exports and
the native RTL size/DOS-name imports; no substitute implementation is added.
These changes resolve full-link-r1's omissions; full-link-r2 now links both
current worker and provider without force-multiple. ADAPTER-WOW-063 also
routes MapDialogRect/four ANSI DlgDir APIs through existing task/borrow/context
ownership to privately named original bodies. Unowned native windows retain
native behavior; no native PCBOX/PDLG reinterpretation. r44 passes29 fixtures,
including arguments/result/error/exception tests of the five recovered entries.
Real WINMINE/SOL gameplay works but normal completion fails0x42b in
current-apps-r1/r2. These candidates are not accepted or published.
Failure-only r3 traces prove native class unregister succeeds with zero live
windows but a retired WND still pins copied CLS. Its release now transfers
storage to the existing parent collector instead of demanding immediate free;
class lookup removal/native ownership and original cleanup remain unchanged.
Whole-product and retained-reader reclamation revalidation is pending.

ADAPTER-WOW-063 private-word composition now selects original client
_SetWindowLong and RTL _GetWindowLong alongside xxxSetWindowLong. Their
production callers retain local original storage; TestWindowProcess uses
the existing exact-allocation ownership predicate, not a sampled PID or
foreign resource view. Original ntsend call macros are reused. This graph
addition compiles in production-cohort r5 but is not yet product-linked.
Source/export audit proves the internal
getter is called only for nonnegative indices; its production selection
rejects negative indices rather than importing unused GetWindowData. Public
negative access stays in the existing facade. No dummy provider or inferred
private flags are added.

ADAPTER-WOW-063 additionally owns wow_dialog_procedure_bindings.c/.h:
the approved actual-HWND procedure/result slots, scoped native notification
dispatch and native terminal cleanup. Original PDLG retains behavior/resource
roles; no native CPD layout, helper window, guest mutation or persistent
procedure/encoding cache. Normal destruction and task exit retire native
storage separately from original finalization. This staged integration still
requires whole-product/lifecycle acceptance; native probes are supplemental.

ADAPTER-WOW-063 original private-word setter composition: select the unchanged
kernel/getset.c xxxSetWindowLong body in the production provider. FCallerOk
is bound only in this finite translation unit to the existing retained local
state ownership check, not to a guessed NT4 system/logon privilege. Original
CBFNID control extents, private-index guards, old values and writes remain
original; the existing xxxSetWindowData continuation owns approved GWL_STYLE
host mutation. This does not implement incoming CPD normalization or complete
the separate client/RTL getset selection. No new mirror or guest difference.

ADAPTER-WOW-058/063 production composition corrections: the shared original
control precompile boundary includes limits.h, as pinned OpenNT client/precomp.h
does, for ListBox signed bounds. The timer entry includes the existing retained
WND layout explicitly before inspecting fnid. These are declaration dependencies,
not new state, algorithms, native-private casts or original mirror changes.

ADAPTER-WOW-063 style service: the reached xxxSetWindowData edge accepts
GWL_STYLE only, after original client validation and xxxSetWindowLong private
access checks. Retain its exact PWND allocation before the existing native
SetWindowLongA callout/return boundary; do not assign the requested value into
the projection after callbacks. Native USER owns the resulting style and
notifications, including the owner-approved pre-4.0 difference. No helper,
message filter or additional mirror diff. CPD/private-precomp closure remains
separate; fixture success is not full product acceptance.

OPENNT-HOST-108 restores original direct/descendant dialog lookup,
default-button transfer/removal and style/version-dependent rectangle
arithmetic. The adapter adds no lookup/button policy: existing SendMessage
handoff and retained graph serve original workers. Creation and modal entries
now install/restore the existing default-geometry context along with their
dialog inputs; ordinary cleanup does not acquire new geometry dependencies.
The original bodies are staged for the complete control graph, not yet
production-selected. Source matches are not runtime acceptance.

ADAPTER-WOW-063 / OPENNT-HOST-107: original ComboBox help inheritance now
has its original rtl/help.c owner. Only the private-property leaf changes:
the existing page-domain generation stores the native context-help scalar
captured with its window observation. The leaf pins that exact allocation,
reads it under the existing execution owner and releases it before return;
it never looks up a retired HWND. Missing owner/storage is an invariant
failure, not the valid zero meaning no local help ID. No second property
registry, inherited-ID cache or modern native ancestor walk is introduced.
Source selection and assertions are staged, pending grouped verification.

ADAPTER-WOW-063 general ValidateHwnd/NoRip now uses the existing page-domain
callback pair and current-generation storage resolver, under the existing
USER lock/borrow owner. The control precomp binds the two original names;
RevalidateHwnd remains the original NoRip alias. The NT4 shared handle table,
desktop mapping and CallbackWnd private storage are not imported/cast onto
modern USER. This is identity/lifetime adaptation, not new control behavior.
Private word/dialog access retains its separate owned-state validator.
Source tracing and staged actual-domain assertions are in the S3 ledger;
complete body/link/runtime acceptance remains pending.

ADAPTER-WOW-057 control composition also exposes the original NTDLL
RtlRunEncodeUnicodeString/RtlRunDecodeUnicodeString ABI to unchanged
ECLock/ECUnlock. EDIT retains seed, lock nesting and text-state ownership;
the existing NTDLL import/alias generator supplies only the two @8 imports.
The shared control precomp restores original windowsx.h font macros using
the matching SDK definitions, not replacement font functions. Export
availability is checked; vector and full-control acceptance remain unrun.
See the S3 control-source ledger for exact source and binary identities.

ADAPTER-WOW-063 dialog system-input closure: original dlgbegin/dlgmgr read
the message font, screen/work rectangles, snap-to preference, system font
dimensions and initialized dialog class atom. A per-entry dialog context
obtains those native resources/preferences and restores nested TLS ownership.
The temporary message font is only a source for original CreateDlgFont's
clone and is released after creation returns or raises. Finalization and
EndDialog require no new preference/font acquisition. Original parsing,
font selection/cloning, positioning and cursor policy stay in original code.
The context is a finite input projection, not another SERVERINFO owner.
Whole dialog body/build selection and real lifecycle acceptance remain open.

ADAPTER-WOW-063 declaration composition: wow_shared_user_abi.h is the sole
guarded inclusion owner for original unguarded wowuserp.h used by cleanup
and client-layout views. The original-layout THREADINFO pointer is scoped
as PWOW_CLIENT_THREADINFO during layout inclusion, not confused with the
input-group adapter's PTHREADINFO. No structure fields or original ABI header
are changed. This addresses the grouped boundary-build declaration failures;
rebuild and runtime verification remain required.

ADAPTER-WOW-063 class identity projection: recovered dlgmgr.c's superclass
notification reads only CLS.atomClassName through WND.pcls. The existing
class-registration binding now owns an original-layout, immutable identity
view initialized at that same registration publication. WND attachment uses
that view while its existing class reference retains it, including after
unregister/detach. This is not a native CLS cast or another class registry;
other CLS fields are not admitted inputs. Class-less observed HWNDs still
require their separate graph-owned identity construction before selection.
Original dlgmgr code remains unchanged. Lifetime assertions are staged,
not runtime evidence.

ADAPTER-WOW-069 sampled-borrow identity correction: cached observed WW must
match the current page-domain backing generation, not HWND alone. An outer
scope may legitimately retain a destroyed window while its handle is reused.
Promotion selects the binding returned by the generation-checked lookup;
existing aliases to the retired allocation remain valid until scope exit.
The existing page-domain identity and scoped references are the only owners;
no second registry or mirror change is required. Deterministic nested-reuse
acceptance remains required before delivery.

ADAPTER-WOW-063 / OPENNT-HOST-103: the admitted EDIT default-color edge
uses wow_edit_default_control_color, preserving original recipient/thread
selection. Exact recipient/source borrow retention precedes native entry;
only absent creation facts (ERROR_NOT_FOUND), not lookup failures, permit
the approved WM_CTLCOLORSTATIC host default. Native input receives source
HWND, never original PWND. The existing native-call owner restores execution
and publishes return state. No callback is sent to the parent application.
Tests remain staged; this does not establish original EDIT build selection.

ADAPTER-WOW-068 now uses OPENNT-HOST-102 for implicit control-brush recipient
selection. wow_control_brush_bindings provides only scope retention and
PWND-to-HWND conversion to the existing explicit color call boundary.
Original selection/validation remain in the mirror. Native implicit brush
resolution is removed from the product; reference-only tests may inspect it.
This staged chain still requires group2 relationship lifetime and group6
real-control acceptance; no new runtime pass or publication is asserted.

ADAPTER-WOW-063 nonclient exception: owner approves native ordinary-frame hit
testing and existing NTW16S geometry for styled frames, without guaranteeing
every NT4 border compatibility result. FindNCHit binds through the existing
scoped native boundary, not recursive SendMessage. The original default and
direct ListBox callers share this binding. Original-body selection and grouped
acceptance remain pending; see the S3 source audit for the recovery ladder.
The same state owner now routes public recovered control extra words and
GWL_STYLE writes through original getset, preserving its ListBox style guard.
This replaces the dialog-only selection, not the logical WNDPROC owner.

ADAPTER-WOW-063 send-boundary lifetime extension: SendMessageWorker's host
binding now obtains an exact-state borrow before native A/W dispatch and
captures HWND before reentry. It uses the existing scope/property owners,
not a new PWND map, and fails before sending if the state cannot be retained.
Existing enclosing scopes retain already-retired exact aliases; a live HWND
replacement must not satisfy an old state's lookup. This does not construct
observed WNDs or establish full relationship publication. Supplemental
argument/retention-rejection assertions remain unrun.

ADAPTER-WOW-070 binds the original ListBox ordinary timer family to native
SetTimer/KillTimer, separate from EDIT's system timers. NULL callbacks preserve
WM_TIMER delivery; the only admitted non-NULL original callback is
xxxLBoxCaretBlinker. Its native trampoline enters the existing scoped control
owner, refreshes (never creates) retained state and requires an initialized,
not-cleaned FNID_LISTBOX before invoking the unchanged callback. No timer map,
custom timer loop or replacement caret drawing is added. Callback registration,
queued-after-destruction, nested callback and real list selection acceptance
remain required and unrun; this source binding is not runtime proof.

ADAPTER-WOW-058/067 internal-creation input closure: the shared control-call
snapshot copies atomSysClass from the existing initialized lifecycle cleanup
table. It validates the same process owner and the original EDIT/ComboListBox
slots before original control entry; there is no name-based re-resolution or
additional class registration. The original-client facade now maps
_CreateWindowEx to the bounded existing wow_control_create_window_ex input
binding. Generic implicit native creation still lacks captured creator facts;
this change covers the two explicit original comboini.c calls only.

ADAPTER-WOW-058 collaboration-entry extension uses one scoped entry owner
for the original ComboBox, ListBox and ComboListBox A/W procedures as well as
EDIT. wow_control_client_procedure selects only this admitted FNID set; class
publication, native-target matching, creation preservation, task-exit patching
and original module-cleanup restoration all consume it. Native HWND/class
identity remains borrowed; recovered control state never returns to a native
control procedure during class restoration. Original EDIT's cleanup-specific
DefWindowProc restoration remains unchanged. The group is not yet link-complete
or tested; no new release capability is asserted.

ADAPTER-WOW-059 finalizer extension: the existing scoped terminal owner now
selects original EDIT, ComboBox or ListBox cleanup from the initialized WND
FNID before identity withdrawal. The shared entry is named
wow_control_finalize_bound/window; no parallel cleanup algorithm is added.
Original combo/list WM_FINALDESTROY handles child notification, lookaside
release and CLEANEDUP marking. ComboListBox initializes FNID_LISTBOX through
the same original worker. Body/private-precomp selection and grouped lifecycle
acceptance remain pending; the supplemental selector double is unrun.

ADAPTER-WOW-069 stages host-storage retention on the existing page-domain WND
generation record. A single-assignment host-only pointer/disposer follows
live, borrowed, retired and physically reclaimed generation lifetime; it is
not a second HWND map, task owner or guest pointer. The consuming binding
must not hold a reciprocal backing reference. The disposer performs only
nonthrowing host deallocation and must remain loaded through domain teardown.
Ordinary borrow records now carry transient generation pins, paired with the
scope rather than stored in the binding. Exact backing plus host-storage
identity is required; no HWND re-resolution can select a replacement
generation. Existing property-only borrows upgrade on subsequent exposure
after generation attachment, only for their matching HWND. Attachment validates the live HWND/property
association for property-only as well as managed storage; exact-state scope
borrowing checks the supplied HWND against retained head.h without querying
live USER after retirement. These are identity preconditions, not new object
policy or an additional map. The class-lifetime fixture stages mismatched
attachment/borrow negatives and retained-after-destroy positives; unrun.
Sampled records retain their existing pin and
are not double-counted. This is staged/unrun and does not yet publish host
relationship pointers or establish the complete graph lifetime.
Observed WW now acquires that generation-owned storage, retaining one scope
reference rather than reallocating it each time. Promotion reuses the same
storage even after a prior scope ended. Managed retirement releases the
reciprocal retained_guest_backing owner before dropping property/table refs;
the final generation callback performs only host deallocation. Host WND
construction for observations and relationship publication remain unwired.
Initially managed client-state attachment now retains the exact published
generation before exposing PWND; failure leaves guest identity unchanged.
Supplemental lifecycle assertions are unrun.

ADAPTER-WOW-068 binds native NtUserGetControlColor (four x86 arguments)
through the existing once-only USER export owner. Original GetControlBrush
and xxxGetControlBrush now supply implicit-recipient selection; the native
implicit service is no longer a product dependency. The explicit-parent
color edge uses the existing exception-safe native-call handoff. No brush
validation/fallback algorithm is duplicated. Full relationship lifetime
and real control acceptance remain required before delivery.
These are staged dependencies, not proof of runtime drawing restoration.

The staged native combo-destruction bridge is superseded by the approved
OPENNT-HOST-096 collaboration group. EDIT's direct ComboBoxWndProcWorker call
is no longer macro-redirected to CallWindowProcA on modern USER. The bridge
and its now-unused standard-target lookup are removed, not kept as fallback.
Original edecrare.c/lboxrare.c notifications and combo.c's state transition
remain unchanged. Complete original control dispatch, retained relationships,
terminal cleanup and group link/guest acceptance are still required before
publication. The removed bridge's mock tests are not evidence for that group;
normal destruction, partial creation rollback, subclass bypass, owner-draw
reentry and task-exit retirement remain mandatory acceptance cases.

ADAPTER-WOW-063 dialog finalization retains the original partial-creation
case: VALIDATECLASSANDSIZE may set FNID_DIALOG before PDLG allocation fails.
When both WFDIALOGWINDOW and PDLG are absent, the terminal binding applies
xxxFreeWindow's final CLEANEDUP mark without allocating a new dialog/pool.
Mismatched flag/pointer and already-deleted unclean states still fail. The
fixture includes idempotent empty cleanup, no-pool and mismatch negatives;
these are staged, not real original allocation-failure acceptance. No mirror
or guest diff is added.

ADAPTER-WOW-063's local scalar/name publication now prepares all enrolled
bindings and name buffers before the existing page-domain graph commit.
The same field writer serves individual refresh and group commit; original
private flags/FNID/control data remain untouched. Publication-lock validation
precedes guest-view commit; prepared swaps add no fallible post-commit query.
Scratch references/buffers are released outside that lock on return/exception.
This staged correction does not complete host relationship/cached-PWND lifetime
integration. Failure assertions are staged in wow_edit_state_fixture; full
CCPU commit/rollback and lifecycle acceptance remain required and unrun.
No additional mirror or guest diff is introduced.

ADAPTER-WOW-067 adds the finite original comboini.c child-creation boundary
to the existing creation owner. Both original calls have atom classes, null
titles/WOW words and only tool/client-edge styles. Their producer supplies
ANSI versus Unicode, module and child identity. The binding captures the
original WORD module version once and delegates to the existing counted
creator/enrollment, without another creation registry or callback hook.
The version query retains usercli.h GETEXPWINVER's null-module substitution
with the current executable module; it does not inherit a parent's version.
The staged native-creation fixture includes a query-failure assertion for
that substitution before enrollment, alongside the existing A/W creations.
Original ntstubs.c _CreateWindowEx's string/MDI/legacy-exstyle branches are
unreachable for these two calls; unsupported shapes fail rather than silently
omitting those policies. The general WOW creation entry is unchanged.
No mirror diff is needed. This is staged binding preparation; original combo
compile-environment selection and full grouped acceptance remain pending.

ADAPTER-WOW-022's autonomous FillWindow implementation is removed. The WOW
registration callback now uses the same ADAPTER-WOW-061 scoped FillWindow
edge as recovered EDIT, forwarding to the admitted native operation without
duplicate clipping, brush-origin or CTLCOLOR policy. Its void output-table
ABI stays unchanged. Existing standalone pixel/identity witnesses now call
the native leaf explicitly; they have no WOW task and do not establish the
registered callback's ownership contract. The scoped call fixture and real
guest registration path remain mandatory grouped acceptance. No tests or
new release have run for this staged change.

ADAPTER-WOW-013's copied default-window message arrays and hand-written merge
are removed. OPENNT-HOST-094 now supplies original WowGetDefWindowProcBits;
the existing output-table slot keeps its ABI and only validates the buffer,
initializes the original tables and establishes the zeroed-buffer precondition.
The provider Ninja graph selects a distinct original client.c object. This
is staged S3 recovery, not a published change. The retained bitmap fixture
also checks repeated publication and untouched trailing bytes.

ADAPTER-WOW-066 pins original combo/list private messages in the shared
wow_control_message_abi.h declaration carrier. Original wmsgcb.c/wcntl32.c
and recovered USER message tables must agree with wmtbl32.c's fixed 0x163/
0x164 slots. The previous host facade evaluated CBEC_* against the modern
SDK CB_MSGMAX, which can move those values. No original thunk/table behavior
is rewritten. This does not admit forwarding NT4 private control messages
unchanged to arbitrary modern controls. Full recovered-group routing and
real guest verification remain open; no runtime pass is claimed.

ADAPTER-WOW-065 stages the ListBox-only native double-click reset leaf in
wow_list_native_bindings.c. Original lboxctl2.c's WM_LBTRACKPOINT return-2
branch reaches kernel/rare.c _ResetDblClk, which clears the current queue's
double-click time. The installed named NtUserResetDblClk export supplies this
operation without reading private state or using an NT4 service number.
The source/ABI evidence is in the S3 control ledger. Resolution is lazy and
local to this leaf, so EDIT does not acquire an unrelated readiness gate.
Missing export/failure is returned, never represented as a successful no-op;
the original-call binding now enforces that failure before continuing the
unchanged ListBox branch. Symbolic NtUserCallNoParam dispatch binds this reset
and the separately scoped dialog queue reset without old service numbers.
The staged provider graph now includes the leaf; original-body selection and
runtime verification are still pending. Added mirror lines: zero. Acceptance
remains grouped with ComboBox/ListBox recovery, including the shell tracking
return-2 behavior; export presence alone is not behavioral acceptance.

This directory owns the target-local binding edge of `WOW32.DLL`.  It is not
an OpenNT mirror root: the selected 77 original WOW32 C bodies, `wow32.def`,
`wow32.rc`, and their original manifest remain under `src/mvdm/wow32/`.
Original USER support bodies remain in their original `src/opennt-host/`
paths.

`source/` contains the 21 finite bindings selected by the formal WOW32 link.
They live with the late-loaded DLL because they service original WOW32 and
original USER calls in that DLL address space.  `include/` contains their
target-local declarations.

## Pending original EDIT integration

OPENNT-HOST-092 now selects original clglobal.c's complete gabThunkMessage
constant table. Original EditWndProcWorker still owns its A/W and WM_USER
guards; no new conversion-message classifier is introduced. The source table
is identical after line-ending normalization and the current Release branches
contain1024 entries. The original EDIT bodies themselves remain unselected.

Its shared screen rectangle now follows original clinit's DISPLAY IC
DESKTOPHORZRES/DESKTOPVERTRES input. Combo placement and dialog clipping use
the same invocation-owned record; the earlier dialog-only SM_CXSCREEN/
SM_CYSCREEN sampling and separate rectangle were removed. The dimensions
are host resource inputs, not an alternative window/desktop authority.

ADAPTER-WOW-058 stages the finite system-input context read by original EDIT:
SYSTEM_FONT dimensions, wheel line count, original double-click-derived drag
scroll interval and the two desktop overlap values. Original
FastGetProfileIntW owns parsing/default behavior; ADAPTER-WOW-042 admits only
the additional read-only desktop key. Existing task compatibility lookup
remains restricted to PMAP_COMPAT. Entry TLS is restored on normal/exceptional
return and destruction adds no new font/profile acquisition dependency.
Original body selection, callout-refresh acceptance and grouped runtime
verification remain pending; this does not expose native SERVERINFO storage.

ADAPTER-WOW-051's existing scoped native-message mechanism now also supplies
SendMessageW and DefWindowProcW. Original control A/W selection must not be
forced through an ANSI-only boundary. The same macro retains execution owner,
native result publication, error and unwind handling; no second message
dispatcher is added. Private PWND worker callers still require their separate
argument/layout audit and are not globally aliased to these public HWND APIs.
The same native-call macro now supplies the original EDIT paint, scroll-info,
caret, cursor and invalidation leaves. ADAPTER-WOW-061 maps their original
NtUser names only in the recovered EDIT compile environment; no general
redirect is installed for all WOW thunks. BeginPaint can synchronously erase
or paint the frame, and SetScrollInfo(SB_CTL) calls the control procedure.
The existing CCPU mutation group now asserts those callbacks see no held USER
domain, preserves the native return/error, checks paint/caret lifecycle and
reads committed geometry afterwards. These additions remain unrun.

ADAPTER-WOW-064 supplies the unavailable NtUserInitBrushes environment for
the unchanged OPENNT-HOST-077 DrawFrame. The original kernel/init.c
LW_BrushInit monochrome8x8 pattern is retained through public GDI bitmap/brush
creation; system brushes are borrowed from native GetSysColorBrush, border
metrics from GetSystemMetrics, and drawing uses ADAPTER-WOW-057 PolyPatBlt.
The original server-wide initializer cannot be imported without display/server
ownership and the installed win32u lacks its export. This finite process-life
resource binding has no renderer, guest data, system mutation or mirror-body
diff. Initialization publishes only a complete brush set, deletes its temporary
bitmap, and retains the one gray brush until process exit like the control
lookaside lock. Failures are returned before entering the original editor.
Build/runtime acceptance remains pending with the complete control group.

ADAPTER-WOW-063 dialog creation, modal and default/finalization entries now
require the same OPENNT-HOST-074 lookaside-lock initialization as EDIT before
calling original control code. Original clinit.c initialized gcsLookaside
before either control family; original dlgmgr.c still owns DialogLookaside.
Initialization failure returns before task/borrow acquisition or allocation.
Original editec.c owns EditLookaside and dlgmgr.c owns DialogLookaside; the
adapter's duplicate EditLookaside definition has been removed. The standalone
allocator fixture uses its own private pool rather than supplying a production
control global. The adapter owns only the shared host lock initialization.
No second lock, pool or allocator is introduced. Entry failure/order fixtures
are staged but unrun; complete group selection and acceptance remain pending.
The production generator now names these staged bindings and selects original
rtlinit.c's lookaside subset as a distinct control-lookaside object, retaining
its existing string-capture object. Original EDIT/dialog/getset body selection
and private-precomp wiring are still pending: the intermediate source graph
is not a link-complete or validated candidate.

The EDIT-private menu compile seam binds original hmodUser to the existing
WOW32 DllInitialize module handle, where OPENNT-HOST-087's menu is linked.
It selects the original Unicode menu loader and existing menu publication,
focus and SendMessage scopes. The shared menu owner now carries the Ex
tracking argument list as well, preserving TPM_RETURNCMD's numeric return.
No ECMenu command/enablement/position/cleanup logic is rewritten. The binding
fixture is supplemental and unrun; full original EDIT selection and real
popup/guest validation remain pending.

ADAPTER-WOW-063 generalizes the staged EDIT-only retained carrier to one
original WND plus class-extra allocation in the existing HWND binding.
Its control-state call seam now joins original SetWindowState/ClearWindowState
to the audited native leaves and refreshes that same retained view on return.
The void original contract cannot report an adapter failure: failed readiness
or refresh uses the existing invariant-failure boundary, never silent stale
success. No additional style message or independent flag mutation is added.

The window owner retains and frees that one allocation; EDIT and dialog
views are casts, never separately registered identities. Canonical FNID,
not the mere presence of storage, selects EDIT terminal/early-exit cleanup.
The existing terminal owner sets original WFDESTROYED on retained host state
before canonical resource finalization, matching createw.c's final-release
precondition. This does not yet prove the earlier child/WM_NCDESTROY timing;
that ordering remains part of the full lifecycle acceptance.
The state is host-only and must never replace the176-byte guest projection.
Existing callback-entry WW refresh now updates the retained client WND's
geometry/style/module from the same native sample and publication lock.
The separate client refresh helper delegates there, retaining no duplicate
field-copy policy. Private flags, FNID and control pointers remain untouched.
The same projection retains numeric GWL_ID for child windows in original
WND.spmenu. A child ID is not a menu pointer or a translated handle. Initial
creation and later refresh both read it; a transition out of child style
clears the former numeric value rather than treating it as a MENU address.
Real top-level MENU pointer projection remains a separate unfinished edge.
Ordinary canonical native EDIT/dialog entries refresh an existing state via
that owner as well: they bypass DispatchClientMessage's WOW-procedure branch.
Final destruction still consumes retained state without requiring a new
native geometry query. The state fixture covers repeated entry after resize
and style changes without replacing private control data; it remains unrun.
The existing committed graph refresh now feeds its same-process observations
to that same update owner on native return. External observations never enter
this path; unenrolled local windows remain unenrolled. No second geometry
sampling/enumeration is introduced. Module identity is read through native
USER, not inferred from the current task. Group verification remains pending.
This is a storage seam for the existing original createw/class-extra and
miscutil/PDLG contracts; no new control algorithm or second cleanup scheduler
is admitted. Complete creator/private-precomp wiring and grouped tests remain
pending, so this does not admit a selectable partial implementation. The staged terminal
edge now selects FNID_DIALOG before identity withdrawal and calls original
DefDlgProcWorker with WM_FINALDESTROY under retained task/resource and HWND
borrow scopes. It checks original pointer/flag cleanup, not the ambiguous
zero return, and never allocates a PDLG merely to release it. Normal A/W entries
now share those scopes and invoke the same original worker. FNID_DIALOG class
imports select that entry; the WOW DefDlgProc adapter selects it only for an
already recovered dialog, preserving untouched native ownership otherwise.
The existing canonical-procedure matcher now recognizes both A/W dialog
entries, just as it recognizes recovered EDIT. The original module-cleanup
table restores the recovered Unicode dialog entry rather than modern USER's
unrelated dialog state. Standard-class fixtures cover initial targets,
canonical matching and the retained original cleanup owner; they are unrun.
Early exit now patches recovered dialogs to DefWindowProc, as original
PatchThreadWindows does for the client-only FNID_DIALOG; final resource
release stays after child destruction. The creation entry now calls original
InternalCreateDialog; private-precomp and complete group link remain open.
The staged control-client/precomp.h now composes the existing admitted
layout, original callback-memory macros, native service and drawing bindings
for that group. Dialog units also select the shared control-client binding
profile, then override only their call-scoped system-input view. It contains
no replacement control algorithm. This header is not yet selected by the
production generator: remaining symbol/identity closure and whole-group
verification still gate selection. Its presence is not a build/pass claim.
Its dialog-state leaf validates that same retained class-extra capacity and
calls unchanged SetDialogPointer. It owns no PDLG allocation, reference table,
native USER pointer or alternate state transition. Original ValidateDialogPwnd
and DefDlgProcWorker remain the allocation/cleanup policy owners.
Positive-index getters/setters now select original _GetWindowLong and client
_SetWindowLong for an already recovered dialog, preserving native ownership
otherwise. A thin continuation connects the client to original
xxxSetWindowLong on the same retained WND. Original zero/error returns cannot
fall through to unrelated native dialog state. Native CPD and full group
private-precomp/build wiring remain mandatory before this is selectable.
The staged owns-state predicate reuses native same-process validation and
requires the exact enrolled allocation, rejecting copied/sampled carriers.
It preserves last-error for the original denial owner. It is only a local
recovered-state boundary, not a general implementation of system/logon
privileges. Original FCallerOk/TestWindowProcess build mapping remains pending;
the supplemental state fixture is unrun.

ADAPTER-WOW-062 stages the x86 native creation leaf for original dialogs.
It also resolves the named zero-argument NtUserZapActiveAndFocus export for
original EndDialog's queue reset. Current x86 USER's caller and win32u stub
confirm the zero-argument ABI; no NT4 service selector or private address is
used. The original operation is VOID; the binding's BOOL reports dependency
resolution only. The staged native fixture checks cleared active/focus state,
no activation/focus notifications and repeated clearing. It remains unrun;
original EndDialog's private-precomp/call-boundary wiring is still required.
The corresponding wow_dialog_call_bindings.c now stages the existing
native-call begin/end scope around this VOID leaf, with finally-based unwind
and existing fail-fast handling if entry, service resolution or restoration
fails. The original caller cannot observe an ordinary FALSE after mutating
dialog state, so failure is not silently ignored. Its supplemental scope
fixture covers each failure and native exceptions; it is unrun. Private
precomp mapping and full group selection remain pending.
It accepts only an already resolved native class atom and original counted
title. Original USER's14-slot call cannot be cast to current win32u's17-slot
export. The adapter uses the inspected current native dialog call shape:
class/original-version class are the same resolved atom, optional slots are
zero, and native version uses its low word; full module facts remain with
the original caller. Original ANSI-creator and Unicode-string encoding stay
independent. No class lookup, activation-context resolver, direct private-code
address or guessed native WOW pointer is introduced. This is not a generic
CreateWindow replacement. The source-shaped dialog entry now surrounds this
leaf with the existing class resolution, CBT enrollment, native-call scope
and rollback. Explicit version facts do not require another version-query
callback. Native-leaf and enrollment fixtures are staged, not run. It remains
unselected/unverified pending the complete dialog owner and native/guest
acceptance; existing creation consumers gain a pending link dependency.

OPENNT-HOST-081 restores the original WOWDlgInit callback dispatcher alongside
GetEditDS/ReleaseEditDS. ADAPTER-WOW-056 maps pfnInitDlgCallback and
pfnWowSetFakeDialogClass to the existing registered input table. The staged
creation entry now calls original InternalCreateDialog, which calls that
dispatcher at its original point. The ADAPTER-WOW-052 native approximation
(per-dialog property, CBT/return hooks, callback and rollback state) is removed,
not retained as fallback. Roots and children use existing explicit creation
enrollment. New original/private-precomp dependencies are not yet selected in
the production graph. No second callback table or hardcoded initializer is
introduced; full modal/default/creator group acceptance is still required.
The WOW modal facade no longer substitutes native DialogBoxIndirectParamA/W:
it passes complete flags through the style/task/borrow scopes to original
clres.c. The existing provider redirect also binds EndDialog to the recovered
state owner, with native ownership retained only for untouched native dialogs.
The original dlgend.c is selected with WOW_ORIGINAL_DIALOG_END so its body
defines wow_original_EndDialog, not the adapter's wow_user_end_dialog.
Only that translation unit receives the override; other original callers
still enter the task/borrow-scoped adapter. The production and supplemental
generators share this selection. Complete product linking remains pending.
Likewise only dlgmgr.c selects WOW_ORIGINAL_DIALOG_DEFAULT: its A/W public
wrappers have private wow_original_DefDlgProcA/W names. They cannot define
the native ownership adapter's wow_native_DefDlgProcA or a local SDK API.
Production recovered dispatch retains its scoped DefDlgProcWorker call;
untouched native dialogs retain the native service path. This is a name
binding correction, not replacement default-dialog behavior.
EDIT private pointer publication and dialog result/user writes now share the
same original xxxSetWindowLong receive edge. The autonomous EDIT pointer
assignment is removed; the original live-pointer overwrite restriction and
destruction exception are retained, with supplemental unrun source cases.

ADAPTER-WOW-061 binds original EDIT FillWindow to installed NtUserFillWindow,
using the existing native-call ownership boundary for the brush-window
CTLCOLOR callback. Original client/ntstubs.c and kernel/paint.c establish
the four-argument call, null-DC rejection, parent fallback, clipping, brush
origin and control-color semantics; current x86 USER calls the same four-slot
service. No private painting algorithm or USER server is copied. The ninth
native leaf extends ADAPTER-WOW-057; its scoped consumer remains unselected
and unverified pending the complete EDIT group.
The same finite call binding exposes the existing current task's client-info
record to original GetAppVer. It does not use the module-version callback,
WND compatibility bits or a cached process-wide version. Original xxxInitTask
remains the sole initializer; a missing current-task record is an invariant
failure, not a fabricated version. Supplemental call/macro fixtures remain
unrun with the rest of this group.

ADAPTER-WOW-061's former native ComboBoxWndProcWorker notification substitute
has been removed: recovered EDIT, combo and list use the original direct
collaboration calls and retained PCBOX/LBIV. No native-PCBOX overlay or second
notification policy remains. This source integration is still unverified.

The shared state-call boundary keeps dlgmgr.c's temporary WFWIN40COMPAT
color-selection bit in the retained original WND read by clmsg.c, not in
modern USER private state. Original client.c guards and dialog set/call/clear
ordering remain unchanged; native style mutations retain their separate
resource path. The source ledger registers the finite correction and unrun
no-native-call, restoration, last-error and missing-state assertions. No
original mirror or guest change is required.

ADAPTER-WOW-057/061 additionally bind original lb1.c NtUserAlterWindowStyle
through the named x86 native HWND/mask/value leaf and required existing WND
refresh. Original list code retains its multicolumn/version mask policy;
there is no SetWindowLong fallback or added style notification. The S3 ledger
records pinned native call-site/stack evidence. Native mutation/notification
and mocked readiness/refresh failure assertions are staged, not run.

The same original-control facade maps existing enable/move/position/parent/
activation/scroll services to the common scoped native-call owner and modal
WaitMessage to the existing task-aware wait. ShowWindow uses the same boundary:
original SHOW_OPENWINDOW is the public value1/SW_SHOWNORMAL alias, not a private
operation. DC release stays with native GDI/USER. These are build bindings, not
new control policies or proof that the full original group is selected.

ADAPTER-WOW-062 binds original dialog NtUserSetThreadState through the named
native two-argument queue operation. Original QF_DIALOGACTIVE activation and
cleanup ordering remain in dlgmgr.c; native input remains the message-bit
producer. No process-local queue cache or synthetic character-message flag is
introduced. The S3 ledger records current native operand/stack evidence and
the unrun argument/failure fixture; real input and cleanup remain acceptance
requirements.

ADAPTER-WOW-062 also stages the original dialog-system-menu outgoing edge.
The unchanged creator's SFI symbol is a compile-time function binding, never
a numeric NT4 operation passed to modern USER. The named one-HWND native
service owns the actual menu resource; the existing native-call owner handles
entry, return publication and unwind. Preserve its BOOL result and last error;
missing service is an adaptation failure, not a successful no-op. No second
menu cache, original desktop structure or guest change is introduced. The S3
ledger records the native call/stub evidence; command contents, lazy mutation
and actual menu lifetime still require the unified group acceptance.

ADAPTER-WOW-061 completes the reached dialog public-service facade for native
foreground activation, context-help IDs and cursor position/display count.
These use the existing execution/publication owner, not new dialog policy or
copied resource state. NtUserGetSystemMenu uses the existing menu identity and
publication owner. Original foreground-queue predicates remain separately
unresolved; binding SetForegroundWindow does not replace those conditions.
Original ListBox NtUserDragDetect likewise uses scoped native DragDetect:
the original user32.def exports these as the same operation. Keep the POINT
unchanged and leave selection/button-up/WM_BEGINDRAG policy in lboxctl2.c;
the adapter does not add an input loop or synthesize drag notifications.

ADAPTER-WOW-063 binds original SFI_SETDIALOGPOINTER to the existing retained
dialog-state owner. The original VOID call receives a checked UINT-to-PDLG
boundary; a failed validation is terminal instead of being ignored. Both
initial installation and final clearing reach unchanged SetDialogPointer,
including its FNID/WFDIALOGWINDOW transitions. No old pointer is passed to
modern USER and no duplicate dialog record is allocated.

The same call boundary now covers original public UpdateWindow/RedrawWindow
and Unicode SetWindowText/SetDlgItemText calls. The existing native-call
owner releases/restores execution permission and publishes return state;
SetDlgItemText targets the child selected by its native item lookup, not the
dialog's thread by assumption. The original-control facade explicitly selects
the redirects, including the existing SendMessageW wrapper, without depending
on MVDM's separate precomp defining WOW_USER_PRIVATE_REDIRECT. No original
paint, text, dialog or control algorithm changes. Supplemental real-HWND
paint/text/nesting/failure checks in wow_user_task_lifecycle_fixture are staged,
not run; original dialog-precomp selection remains an open group dependency.

ADAPTER-WOW-051's message boundary now has a Unicode entrance for the
recovered USER dialog island. Original dlgmgr.c:DialogBox2 uses Unicode
PeekMessage/DispatchMessage; WOW thunks retain ANSI. One lifecycle_message
body owns the unchanged publication, queue scans, yield and wait sequence;
only native PeekMessageA/W selection differs. Registration-bridge W entries
and scoped DispatchMessageW reuse the existing owner. No second queue,
text codec or scheduler is introduced. Real-HWND WM_CHAR, PM_NOREMOVE,
empty filter, WM_QUIT and invalid-output assertions are staged, not run.

ADAPTER-WOW-058 also captures the existing default geometry context before
entering original controls. lboxrare.c:xxxLBSize calls original GetWindowBorders
directly, outside DefWindowProcWorker; a default-message-only TLS snapshot
was insufficient. Both snapshots are installed only after successful capture
and restored together in finally. Destruction does not acquire new metrics.
No original border arithmetic or native metric policy is replaced. Nested
default-worker restoration checks are staged; full list resize is unverified.

`wow_edit_native_bindings.c` and its header are the unselected ADAPTER-WOW-057
dependency group for the original EDIT consumer. They resolve nine installed
USER/GDI services without replacing editing, timer or A/W conversion policy.
Their [source/ABI admission](../../docs/etc/evidence/m0-t447-s3-control-source-audit.md#adapter-wow-057-edit-native-dependency-binding-admission)
records the original owners, private dependency boundary and zero mirror-diff
budget. `wow_edit_native_fixture.c` is supplemental native boundary test source,
not guest acceptance. These files are not yet built, tested or selected by the
production graph; the original editor's full lifetime group is still pending.
`wow_edit_memory_bindings.h` uses the existing registration table's const
view for the original callback names. OPENNT-HOST-071 stages original
GetEditDS/ReleaseEditDS in client.c without an additional allocation policy.
The production generator now selects that subset with WOWDlgInit as a distinct
dialog-memory object. The supplemental memory-callback fixture now calls all
three actual retained bodies, including failed allocation/initialization and
ReleaseEditDS's original LOWORD conversion. These assertions are unrun; real
guest and dialog lifetime acceptance remains required before delivery.
`wow_edit_entry_bindings.c` (ADAPTER-WOW-058) retains existing window words
and task/borrow scopes around the unchanged original A/W entries. It has no
control-state registry or message converter. Its terminal resource permission
is limited to current-task windows in the existing early-resource phase.
This source is likewise unselected and unverified; entry wiring does not
establish completed original EDIT storage or final-destroy integration.
ADAPTER-WOW-059 (`wow_edit_state_bindings.c`) exposes only the existing
scope-retained EDITWND and its offset-zero PED word to original control code.
Original OPENNT-HOST-076 owns the FNID transition; the binding does not repeat
its policy. No native USER private bytes or guest projection receives PED.
Creation/refresh and terminal integration remain pending, so this is not yet
a production-selected state provider. Its supplemental fixture is unrun.
The shared ADAPTER-WOW-063 allocation reserves WND plus the captured class extra
extent (at least EDITWND for pre-validation safety), matching original
createw.c rather than claiming more cbwndExtra than physically allocated.
The same binding owns the single allocation and last-reference release;
no separate extra-byte heap or dialog identity is added. Creation, validation
and refresh now live in wow_window_client_state_bindings.c; this EDIT unit
retains only its PED publication and canonical finalizer.
The same ADAPTER-WOW-063 owner captures native stored window names through
the SDK-declared InternalGetWindowText, not application WM_GETTEXT. The
original clmsg.c name readers require retained Unicode length/buffer state;
modern USER does not expose its WND allocation, and importing its server
heap is excluded. Creation/refresh/free now pair that buffer with the retained
WND. A per-binding short read lock protects replacement; it must never span
application/native callouts. Original default-worker name-reader wiring is
still pending and cannot use the raw buffer without that lock. Supplemental
long/empty/error/no-callback fixture cases are staged, not run. No guest or
original algorithm changes are introduced by this storage binding.
ADAPTER-WOW-061 also binds the two original EDIT SendMessageWorker calls
through wow_window_client_access_bindings.c. It converts only PWND to HWND
and selects the existing scoped native A/W send. Original ECGetControlBrush
still chooses same-thread send versus default-brush handling. Native USER
retains Hook delivery/conversion under the approved message-transport contract;
the unavailable NT4 private fast-dispatch tables are not reconstructed. The
default-worker algorithm is not replaced by this binding. Its argument/return
fixture is staged and unrun; original EDIT selection remains group-pending.
Its terminal adapter retains the scoped EDITWND, uses original FNID cleanup
status rather than PED nullness, and invokes the canonical original entry
through the task-scoped wrapper before HWND retirement. It frees no editor
memory itself and supplies no late-cleanup fallback. Original xxxFreeWindow
final-control dispatch is the source contract; importing the full USER server
is excluded. Terminal-owner wiring and grouped verification remain pending.
ADAPTER-WOW-060 (`wow_edit_conversion_bindings.c`) binds only original
EDIT's CsSendMessage/FNID_CALLWINDOWPROC conversion branch. Native USER owns
the CPD and A/W message conversion; the existing scoped CallWindowProc bridge
and recovered EDIT entries retain execution/lifetime boundaries. No string
converter, temporary window procedure replacement or message rewrite is added.
This source remains unselected pending the complete EDIT group.
Canonical EDIT selection reuses the existing class record and original FNID,
not another class-name table. GetClassInfo and original WALIAS caching consume
the same logical target as creation. Native registration remains borrowed;
the original native procedure is retained only as registration metadata, not
as a competing recovered EDIT implementation. This source wiring is pending
the same full build/creation/state/retirement acceptance group.
Explicit product creation retains module version, task compatibility and
class extra-byte size in the existing window record, alongside actual A/W
provenance. An application subclass's later first call into original EDIT
uses those captured facts to establish its carrier; it does not query the
current task or invoke a delayed guest version callback. Missing provenance
fails explicitly. Internal/template producer wiring remains pending.

`ntvdm.exe` retains `wow_user_runtime.c`, `wow_user_session_binding.c`, and
their headers in `src/ntvdm-exe/wow/`: those two files own the sole worker
TLS/session binding.  WOW32 imports only the narrow parent ABI
`current/enter/leave/set_context/worker_active`; it does not link or import a
raw session API, `session.lib`, a CCPU executor, or a second worker state
instance.

`include/nt_vdd.h` is a WOW32-only declaration wrapper for three cdecl worker
imports under the original `/Gz` target convention.  The byte-exact original
ABI carrier remains `src/opennt-abi/source/public/sdk/inc/nt_vdd.h`; ordinary
MVDM consumers continue to include that original carrier directly.

## USER message and task boundary

ADAPTER-WOW-031 internal enrollment and completion acquire the existing USER
domain only while changing shared class/window metadata, preserving the
caller's entry lock state. Hook-chain forwarding releases that lock and
reacquires it before completion, including veto/exception cleanup. This
also covers completion after a guest Hook has left its task callback scope;
it introduces no guest execution acquisition, new lock or task owner.
The supplemental creation fixture checks downstream-Hook lock state and
rollback with initially held/unheld ownership; grouped execution is pending.
Creation's untransferred class reference and temporary parent admission borrow
are released on exceptional exits as well as ordinary failures. The existing
native-creation runner has a separate `-CreationUnwind` fault row for outer
scope/TLS/hook/borrow restoration; ordinary coverage retains the production
object unchanged. Both staged rows still require grouped execution.

ADAPTER-WOW-031/063 creation enrollment preserves both recovered EDIT and
DIALOG procedure targets instead of replacing DIALOG with the native class
procedure. All explicit original creators carrying captured version/facts
now establish their shared WND before the first application callback. This
includes custom dialog classes: InternalCreateDialog validates that WND
directly after creation, independently of whether the class procedure calls
the default dialog handler. No PDLG is fabricated here; original
ValidateDialogPwnd still owns promotion. Generic implicit creation without
captured facts remains unresolved rather than inheriting parent provenance.
The native creation fixture checks WND availability during WM_NCCREATE and
retained A/W/version state after the counted creator returns; it remains
unrun pending the dependency-group build.

The staged S3 observed-WW reader consumes words, style, extended style and
module from the same retained page-domain backing. Module is host metadata,
never a native pointer written into the guest WND. Capture and graph commit
carry it alongside PID/TID; ordinary observation refresh updates it with the
copied fields only after native sampling succeeds. This extends the existing
bounded observation owner, not a second HWND registry. The paired NTVDM export
requires a live borrow, permits retained reads after retirement, and leaves
output unchanged on failure. Enrollment remains separate from observation.
Promotion, native mutation/refresh, retirement and final-release cases are
staged in wow_page_domain_fixture.c, not yet run. This does not complete the
host relationship/state producer or establish S3 runtime acceptance.

`wow_user_private_access.h` binds the selected original WOW32 ANSI
GetMessage/PeekMessage/WaitMessage calls to `wow_user_task_lifecycle.c`.
ADAPTER-WOW-051 retains the native USER queue and the recovered original
`taskman.c` scheduler; it does not create a second message queue or scheduler.
The native queue wake observation is retained only across the original task
wait so WaitMessage does not confuse a scheduler wake with new input.
The source-recovery rationale, tests and remaining cross-task send/activation
verification are in the [S2 ledger](../../docs/etc/evidence/m0-t422-s2-user-client-lifecycle-ledger.md#e70-message-api-facade-candidate-not-acceptance).
These bindings are production-wired but do not establish complete S2 acceptance.

The native queue bridge preserves original `ssend.c` MAKECALL ordering: release
the worker USER data lock while native PeekMessage may invoke client code,
then restore it before updating the queue view, including on unwind. This
does not release or replace taskman's separate WOW execution ownership.
The red/green reentrant receive witness is recorded in the S2 ledger (E76).

The native queue bridge preserves original `ssend.c` MAKECALL ordering: release
the worker USER data lock while native PeekMessage may invoke client code,
then restore it before updating the queue view, including on unwind. This
does not release or replace taskman's separate WOW execution ownership.
The red/green reentrant receive witness is recorded in the S2 ledger (E76).
