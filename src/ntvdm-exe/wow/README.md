# wow family

Native cleanup composition derives temporary tree links from current
GetAncestor(GA_PARENT) relationships before entering original WOWCleanup.
Native-only intermediate nodes are contracted out; bound descendants still
reach the original class/current-thread predicate. It does not alter native
parenting or sibling order; the temporary traversal order is not exported as
ordinary USER enumeration. Original PseudoDestroyClassWindows changes only
matching procedure fields and makes no callouts, so native sibling ordering
does not affect this operation's result. Required domain serialization and
non-callout cleanup operations remain caller obligations. A finally scope
restores the previous desktop root and clears derived links on success or
exception; no second persistent native topology or allocator is introduced.

ADAPTER-WOW-048 binds native windows to the cleanup object table using the
existing WOW 16-bit alias, not a generated handle. Original HANDLEENTRY-shaped
phead/pOwner/type/flags/wUniq fields reference the live WW binding; wUniq retains
the native high word. Occupied aliases are rejected without replacement; retire
requires both native identity and a still-borrowed matching object. There is no
extra object allocator, uniqueness counter or resource policy. The process USER
owner supplies zeroed table storage and serialization with cleanup, and retains
it until all objects retire. The x86 table is 64K entries at 12 bytes each;
native property publication/retirement updates it under the property lock.
Creation publishes before downstream hooks/callbacks, and rollback/destruction
retires before releasing the binding. This currently enrolls windows created
through the bound A/W entry, not every native helper or process-owned resource.
The table contains host views and is not yet a guest-visible shared USER table.

ADAPTER-WOW-048 binds native windows to the cleanup object table using the
existing WOW 16-bit alias, not a generated handle. Original HANDLEENTRY-shaped
phead/pOwner/type/flags/wUniq fields reference the live WW binding; wUniq retains
the native high word. Occupied aliases are rejected without replacement; retire
requires both native identity and a still-borrowed matching object. There is no
extra object allocator, uniqueness counter or resource policy. The process USER
owner supplies zeroed table storage and serialization with cleanup, and retains
it until all objects retire. The x86 table is 64K entries at 12 bytes each;
native property publication/retirement updates it under the property lock.
Creation publishes before downstream hooks/callbacks, and rollback/destruction
retires before releasing the binding. This currently enrolls windows created
through the bound A/W entry, not every native helper or process-owned resource.
The table contains host views and is not yet a guest-visible shared USER table.

Cleanup object classification now comes from the compiled original handtabl.c
gabObjectCreateFlags and original TYPE/OCF declarations, not a caller-supplied
table in the cleanup context. Native resource release bindings still belong
to their actual producers/owners. Native USER remains the allocator; importing
HMAllocObject unchanged would introduce another generated handle identity and
would require a different, comprehensive handle-conversion architecture.

Cleanup object classification now comes from the compiled original handtabl.c
gabObjectCreateFlags and original TYPE/OCF declarations, not a caller-supplied
table in the cleanup context. Native resource release bindings still belong
to their actual producers/owners. Native USER remains the allocator; importing
HMAllocObject unchanged would introduce another generated handle identity and
would require a different, comprehensive handle-conversion architecture.

ADAPTER-WOW-031 creation takes the actual USER thread, deriving class roots
from its original process view. The CBT scope publishes this same thread in
the live WND fields before downstream hooks or the first WNDPROC invocation.
No host-thread ID is converted into a fabricated USER task. The owner must
retain its thread while windows are published; terminal/rollback property
detach clears the retired window borrow's thread pointer. This is not full
worker current-thread registration, scheduler teardown or handle-table
membership. Bounded native fixtures supply explicit controlled thread owners.

ADAPTER-WOW-027 stores the logical WND procedure once, in the window-owned
cleanup field lpfnWndProc. Native dispatch and procedure read/exchange use that
same field; cleanup no longer needs a separate target snapshot or write-back.
The callback remains immutable private binding. Borrowed cleanup fields retain
the existing class entry and survive native destruction while the WW borrower
is live. Native attach initializes the class identity, not a fabricated USER
thread or object table; production membership remains the USER owner's duty.
This removes duplicate procedure representation, not a claimed net allocation
reduction: tree/thread/class fields are required original lifecycle state.

ADAPTER-WOW-047 supplies an explicit borrowed cleanup context to the recovered
whole original WOWCleanup owner. Existing task process/desktop views point to
the same class roots and desktop tree. Full live window/handle membership and
dispatch-target mutation are mandatory before production registration; fixture
views are not a substitute. Class removal uses one shared native primitive
with ordinary unregister, but no repeated ordinary class lookup: the original
lookup intentionally excludes deferred classes that cleanup must destroy.
False native release preserves metadata; the recovered cleanup stops with
FALSE instead of retrying the unchanged node. Completed earlier cleanup is
not rolled back. Menu/cursor/icon/accelerator native operations and ownership
must be bound by their actual resource owners, not generic success callbacks.

ADAPTER-WOW-046 supplies the exclusive process user-address ceiling from
GetSystemInfo and maps original probe exceptions to native RtlRaiseStatus.
Original ProbeForRead retains all range/alignment decisions. No VirtualQuery
scan, payload copy or access-right success assertion is invented. A successful
probe is not evidence that a later read is accessible; original SEH remains
required around that read. This does not replace guest-memory leases.

ADAPTER-WOW-030 stores the enrolled logical class target only in the original
CLS.lpfnWndProc field of the existing class entry; the former private target
procedure was removed. Only the immutable callback binding remains private.
Native USER retains the gateway, and each WND retains its own procedure snapshot.
This is a field-ownership consolidation, not a new procedure resolver or a net
storage-byte reduction. Unenrolled native classes are not synthesized here.

ADAPTER-WOW-045 routes class unregister through original _UnregisterClass.
Only native destruction, output-transfer gating and finite context remain in
the adapter; original private/public lookup and window-association rejection
are no longer copied there. Native failure must not release metadata or return
menu ownership. The source wrapper propagates a failed native destruction;
the adapter publishes its captured output only on success.

ADAPTER-WOW-044 binds original class.c _RegisterClassEx to an explicit current
thread and required registration/CPD-owner operations. The original wrapper
sets hTaskWow from TIF_16BIT plus TDB, otherwise zero. Existing class records
hold worker/menu pointers and two WOW words; former duplicate storage in the
private binding was removed. The class-client publish path now calls this
original wrapper before native publication. It requires an explicit thread
view and rejects CPD registration if no actual resolver is supplied; fixture
CPD resolution is not a modern USER CPD provider. The worker's top-level
20-slot USER registration and current-thread initialization remain incomplete.

ADAPTER-WOW-030/027 now publish cWndReferenceCount in the existing class
lookup entry when an enrolled window property is installed/removed. This is
separate from storage borrowers: failed attach and stale rollback do not
change it, and aliases surviving HWND destruction do not count as windows.
Native USER still owns actual class clones and native window counts. This
field covers enrolled associations only; it cannot claim coverage of native
windows outside the binding. Original task ownership must still be supplied
by the class registration caller's TDB, not fabricated from a native thread ID.
Original WOWCleanup is not yet installed or authorized to assume a complete
object table merely from this counter.

ADAPTER-WOW-043 supplies the original desktop list, explicit attachment edges
and queue reference fields to recovered input.c grouping. Existing task thread
views carry original PtiLink/rpdesk/pqAttach fields; no second task registry or
copied grouping algorithm is introduced. The original current-thread read is
an explicit argument; DestroyQueue is a required owner operation, not a stub.
This is a finite host view, not the NT4 binary layout or guest-visible data.
Its fixture uses original compiled bodies and observes queue releases, but
does not implement native input publication or production USER lifetime.
It also supplies the required queue allocation operation for original
RecalcThreadAttachment. Allocation failure is passed through unchanged; the
original void function stops traversal rather than returning a success flag.

ADAPTER-WOW-042 supplies only PMAP_COMPAT read-only machine-key access and a
validated entry into original FastGetProfileStringW. It selects the native
machine view explicitly, not the redirected 32-bit view, and creates neither
keys nor compatibility values. Original source declares PMAP_COMPAT as a
machine mapping, so user impersonation/profile caching is unnecessary for
this one family. NtQueryValueKey uses the original declaration ABI; RegCloseKey
releases the RegOpenKeyExW-owned handle. Caller-buffer/section validation is
outside the unchanged original reader. Zero capacity, overflowing allocation
size and a default longer than the destination are rejected before entry.
Native absent-key tests pass; no existing registry data has been changed to
manufacture positive query coverage. This function is available for the
InitTask profile operation, not yet installed into a production task context.

ADAPTER-WOW-041 (wow_task_init_bindings.h) provides the original initialization
field views and mandatory profile, journal/input attachment and cursor
operations. Original queue.c owns name capture, compatibility policy, TDB
publication and task activation. `wow_user_task_lifecycle` now supplies one
worker-local carrier for that selected original owner: a retained WOWEXEC
event, original registration/InitTask/yield/DirectedYield/DestroyTask calls,
and one public USER32 queue per bound native thread. The profile operation keeps its original
section/key/default/capacity arguments; it is not a new compatibility database.
The queue/client views are not guest ABI structures. Runtime initialization
must bind all reached operations, serialize the process domain, and publish
the resulting task version to class consumers. The tested observers are not
production implementations. Name storage is thread-owned; DestroyTask retires
TDB but does not free the thread name or process. Failure can leave original
startup/name state for caller-owned cleanup; no synthetic transaction policy
has been added. This carrier is linked into the provider composition, but the
standalone output-table registrar remains absent until all twenty original
output entries can be published together.

ADAPTER-WOW-040 in wow_task_event_binding.c replaces the reached kernel
event-reference operation for original USER process registration. It first
duplicates without adding access rights, queries SDK public object records,
requires original EVENT_ALL_ACCESS and native Event type, and returns an
independently owned handle. Every failure closes the duplicate/query buffer.
The original source passes a NULL object-type parameter but later uses
KeSetEvent; rejecting a non-event is an explicit modern binding safety check,
not a claim that original ObReferenceObjectByHandle checked event type.
Only the Boolean success result is consumed; no new error translation policy
is introduced. Process publication/zeroing/failure rollback stays original.
Required process teardown must release this handle after tasks and waits end.
No production USER callback registration has been enabled yet.

ADAPTER-WOW-039 supplies finite priority/list/process/thread/message views for
original InsertTask, WakeWowTask, DirectedScheduleTask and CurrentTaskLock,
plus the original process registration list/callback fields.
DestroyTask now uses the same domain, with process-heap task/waiter records
and independently retained native idle-event handles. The recovered original
pseudo-event macro distinguishes sentinel ON/OFF from an owned event before
signaling and closing it. A thread view is retired after destruction, not
reused with the original dangling ptdb field. No thread termination API or
new task-exit policy is introduced.
The view is neither a guest structure nor a second task manager. All process
views participating in a send/reply belong to the same shared event-counter
domain. Native events represent retained host objects, not kernel pointers;
the original signal call's return value is unused. The lock-cookie algorithm
is original, with its current-process input explicit instead of a fake TEB.
No independently invented task allocator, executor or scheduling policy is added.
The recovered original SleepTask now uses native alertable WaitAny and its
original five-slot wait-array allocation; original UserYield owns before/after
message ordering. ADAPTER-039 exposes required host operations for outer-lock,
message, death/APC and idle handling, with no default implementation. Native
wait return values preserve task vs WOWEXEC-event distinction. The historical
queue view uses original QS_INPUT/QS_ALLINPUT, not modern raw-input additions.
Original retail assertions remain elided; the worker-local lifecycle carrier
supplies synchronization for one task domain. It substitutes only a public
`MsgWaitForMultipleObjectsEx` queue wake for the unavailable kernel wait;
original task selection, event counts and dispatch ordering remain in
taskman.c. InitTask, sleep/yield, send/reply ordering and cleanup remain an
indivisible whole-provider requirement until W2 owns real task cleanup and
all callbacks are registered atomically.
Original DirectedYield now uses a required host-thread lookup under that same
domain lock. It does not use a replacement task-ID table or Sleep(0). The
borrowed target must remain live through the locked call. Native two-thread
handoff tests do not establish real WOW task registration or guest execution.

ADAPTER-WOW-038 composes the original RegisterClassWOWA/RegisterClassExWOWA
and UnregisterClass client with the existing native class-word owner. A
synchronous scope carries the caller's InitTask version, original module
version callback, class context and WOW callback. Finally restores the prior
scope on success, failure and exceptions. No default version is synthesized.
The caller still owns class-list serialization and context lifetime. Native
registration uses the gateway and omits automatic native menu loading from a
packed WOW module; retained original client menu names must be consumed by
the recovered class-menu creation path. No native window is created here.
This replaces test-only class-client backends in the new formal-object fixture;
USER output-table hookup, InitTask context ownership and class-menu creation
remain pending and must not be inferred from a nonzero native class atom.

ADAPTER-WOW-036 binds original CommonLoadMenu/LoadMenuA/W and
WOWFindResourceExWCover to the selected original W32 resource callbacks,
whose declarations come from mvdm/wow32/wres32.h. A/W entry names are private
to avoid overriding native USER32 imports. Unicode-name conversion still
uses original WCSToMBEx. ADAPTER-WOW-037 now binds recovered original
MenuLoadWinTemplates/MenuLoadChicagoTemplates/CreateMenuFromResource to native
menu object operations. The original TIF_16BIT condition is an explicit
invocation parameter: the three WOW-only consumers pass TRUE; tests also
exercise FALSE. This is not a global fake task flag or a public USER override.
ADAPTER-WOW-020, original WU32LoadMenuIndirect and the WOW resource client all
use this parser; native LoadMenuIndirectW had corrupted packed itemData.
The binding does not free guest resources: original CommonLoadMenu
deliberately unlocks without FreeResource. This finite resource client now
links but automatic class-menu invocation during creation remains unconnected.

ADAPTER-WOW-030 now retains the logical CLS procedure/callback by value for
explicit gateway-class registrations. ADAPTER-WOW-031 inherits that target
when creation supplies no explicit override. ADAPTER-WOW-033 returns the
logical class procedure for a native gateway GCL_WNDPROC result, rather than
exposing the gateway; other native class results are unchanged. Window-local
subclass exchange does not alter the class target. Raw CPD inputs remain
invalid at this storage boundary, pending original public-policy resolution.
Class-wide CPD/Unicode SetClassLong policy, standard-class enrollment and
production RegisterClassWOWA publication remain incomplete. This metadata
is the finite counterpart of original CLS.lpfnWndProc, not a new dispatch
algorithm: actual dispatch still enters original DispatchClientMessage.

For an enrolled ANSI gateway class, ADAPTER-WOW-033 now exchanges its raw
logical procedure while leaving the native gateway installed. ADAPTER-WOW-030
uses an atomic procedure snapshot/exchange and immutable callback identity;
ADAPTER-WOW-031 copies the target before native creation rather than borrowing
a mutable class-field pointer across callbacks. Existing windows retain their
own procedure. This recovers the selected storage/lifetime contract only;
it does not implement CPD resolution, Unicode mapping or standard classes.

ADAPTER-WOW-035, `include/wow_class_query_bindings.h`, supplies the finite
private class view for original GetClassWOWWords. The existing borrow scope
inherits an explicit class context; its owner retains that context and
serializes all list access. Named query reuses the native atom/Unicode
comparison and original GetClassPtr selection already present in ADAPTER-030.
It does not create a global class registry or a second atom namespace.
The fixed GCL_WOWWORDS expression maps to the same two-DWORD WC storage,
following `client/classc.c::_GetClassData` (OpenNT SHA-256
`1DA5A77B48A073A0C6001D7F56B530C941C90B55C8E995FD6C10914AC8E8D937`).
Zero client delta describes this native view, not a populated NT4 TEB/shared
heap. A missing scope/context or class fails; original GetClassWOWWords
retains its own ERROR_CLASS_DOES_NOT_EXIST mapping. Production class-context
publication, locking and USER output-table registration remain mandatory.

ADAPTER-WOW-030's existing class record also retains the original CLS pair
`lpszClientAnsiMenuName` / `lpszClientUnicodeMenuName`. Register failure leaves
them owned by original RegisterClassExWOW; successful unregister returns them
to original client cleanup. The call-local captured Unicode descriptor is
never retained. This removes test-only menu ownership from the native-backed
class-client fixture, not an algorithm from the mirror. The CLSMENUNAME
declaration is copied from OpenNT `windows/core/ntuser/inc/user.h`, SHA-256
`ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1`.
The ordering follows original kernel/class.c::_UnregisterClass: refuse while
windows exist, then return client pointers before destroying class metadata.
Native USER supplies the live-window check; no second reference-count policy
or menu-name conversion is introduced. Task teardown and public SetClassLong
menu replacement remain unconnected and unaccepted.

ADAPTER-WOW-034, `include/wow_user_capture_bindings.h`, binds the original
USER `RtlCaptureAnsiString` to existing `opennt_support_current_teb` scratch
storage, process heap and NTDLL conversion. Its IN_STRING declaration comes
from original `windows/core/ntuser/client/usercli.h` (SHA-256
`11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA`);
the static-buffer threshold comes from original `public/sdk/inc/ntpsapi.h`
(`CAFD48B3A9C672C6A18503D4565E7864A284EC029F63AE04FF221C58A2DB5148`).
The projection is not the modern native TEB. Retail RIP diagnostics remain
non-evaluating, and warning 4057 is suppressed for the original same-width
signed/unsigned conversion byte-count pointer. No string algorithm or extra
TLS owner is added. Full class-client service integration remains open.

ADAPTER-WOW-033, `wow_user_private_access.c`, binds GWL_WOWWORDS and the
three private window DWORD indices plus GCL_WOWWORDS and two class setters
to existing scope-retained WW/WC. Public indices call USER32; GWL_WNDPROC
queries that resolve to our ANSI gateway return its retained logical target
instead of exposing the gateway. Other native procedures/CPD tokens pass
through unchanged. Missing gateway metadata fails without exposing it.
Original GetWindowData/_GetClassData private field selection is retained;
shared class storage replaces per-clone replicated words. The original private
setter branches leave dwOld indeterminate; this boundary returns the actual
previous word, unused by selected SETWL/SETWC statements. It is not full
procedure/subclass policy, missing standard-class enrollment, or USER16 shared
object projection. Direct original kernel/client access to unavailable private
WND/CLS storage is the rejected dependency, not an excuse to copy their full
public API algorithms into this binding.

## Scoped private aliases (S40)

ADAPTER-WOW-032, `wow_user_borrow_scope.c` and its header, retains existing
WW/WC storage references for synchronous native WOW consumers. Original
USER private getters alias retained WND/CLS fields; modern USER exposes neither
those fields nor the original object-lock lifetime. A thread-local nested scope
deduplicates retained references and releases them in finally, without new
window IDs, task records or guest addresses. Getter use outside a scope fails;
class lookup retains its caller-owned synchronization requirement. Native
DispatchClientMessage binding enters/leaves a scope. `wow_user_thunk_scope.c`
also wraps both original W32Dispatch/W32TryCall invocations using the original
LPFNW32/PVDMFRAME declarations; exceptions propagate to the original owner.
ADAPTER-WOW-033 supplies the formal private-index redirection; this is not
a completed public GetWindowLong/GetClassLong replacement.

The WW binding also exposes atomic read/exchange of its opaque dispatch
procedure. Native gateway dispatch snapshots that value; original
DispatchClientMessage still chooses WOW versus native invocation. This is
storage for an already validated procedure, not full SetWindowLong policy:
CPD/A-W mapping, destroyed-window policy and class-procedure exchange remain
outside it. It does not update original WW procedure words implicitly.
The provider-facing ANSI getter now queries this target when native USER
returns our gateway. For that existing gateway, the setter exchanges the
validated raw target without uninstalling the native entrance; original WOW32
still owns selector encoding and subsequent WW updates. CPD conversion,
standard-window enrollment and the remaining subclass policy are not complete.
As in original xxxSetWindowData, CPD tokens must be resolved before this
storage/dispatch layer. Attachment, exchange and dispatch reject the reserved
FFFF CallProc tag rather than misclassify it as a WNDPROC_WOW guest target.
This enforces a lower-layer invariant, not completion of CPD conversion.

WW property access checks ownership through native GetWindowLongA(GWL_WNDPROC),
which retains original GetWindowData's same-process rule and accepts the
original short HWND form. GetWindowThreadProcessId cannot substitute for that
contract on this host: it rejects the short handle even while native window
and property queries accept it. Foreign-window queries are rejected before
the property pointer is read. No new handle IDs or lookup registry is needed.

## Native creation publication (S40)

ADAPTER-WOW-031 in `wow_window_creation_binding.c` binds original createw.c's
pre-callback WW/class publication to public USER's native creation. Original
USER object allocation, desktop heaps, thread locks and server creation engine
are unavailable; they are not imported. A scoped thread CBT hook publishes
owned WW/WC for the selected atom, instance and unchanged application parameter.
For WS_EX_MDICHILD it matches that parameter through USER's temporary
MDICREATESTRUCT.lParam wrapper, without changing or retaining the wrapper.
Native CreateWindowExA/W retains creation policy and callback order. Both
entry points share one publication/cleanup scope; the W entry copies the public
CREATESTRUCT representation without converting its string pointers. A TLS stack
supports reentry; success keeps window ownership, while failure unhooks,
restores the stack and removes only this attempt's borrowed publication.

The input is a complete WW constructed by the composition owner, not the
12-byte original lpWOW carrier. Original client validation/MDI/task policy and
guest procedure conversion remain outside this binding; it is not a direct
replacement for the full CsCreateWindowEx slot. `wow_window_native_proc` retains
the encoded procedure and callback, delegates to original DispatchClientMessage,
and detaches its own publication in WM_NCDESTROY's finally cleanup. A custom
native procedure must provide that detach itself. No lock may span callbacks.
The real native fixture
verifies early metadata, nested creation, NCCREATE rejection, CBT veto, later
successful creation, and unchanged opaque application parameters. Complete
provider output-table and real Win16 acceptance remain outstanding.

## Shared class WC storage lifetime (S40)

ADAPTER-WOW-030, `wow_class_words_binding.c` and its same-named header,
supplies native lifetime for original CLS.adwWOW's two DWORDs (the original
WOW32 WC). Original kernel/class.c remains the lookup owner. The unavailable
desktop CLS allocation/locking engine is not imported: register preallocates,
calls native USER and publishes only on success; unregister uses original
private/public lookup and removes ownership only after native success.
Acquire borrows the same shared words, and release defers freeing until the
last reader leaves. List insertion follows class.c's head
insertion; no new class selection, atom namespace or guest graph exists.
The original client's captured Unicode name is copied into the same allocation
and associated only with the native registration atom. Lookup uses NTDLL's
case-insensitive RtlEqualUnicodeString; integer identities pass through without
a pointer dereference. It does not use local/global Win32 atom tables. The
supplied name must be the capture of the same native class being registered.

The caller serializes transactions and all words updates. Prepare/publish/
detach are private implementation details, not an exposed mutation protocol.
Unregistration uses the original private/public lookup scope,
not GetClassPtr's broader client/system query fallback. Every context entry
must be created by register. Original client validation/capture and conversion
of tagged guest procedures to native callable procedures precede this boundary.
Original client menu ownership,
system-class population, task cleanup and raw-pointer call epochs are still integration
obligations; the helper is compiled but no incomplete output slot is installed.
The native lifetime fixture proves failure preservation, same-module aliasing,
public/private separation and reader survival after successful unregister.

## Class metadata lookup declarations (S40)

ADAPTER-WOW-028, `include/wow_class_lookup_bindings.h`, defines the finite
native list view consumed by original class.c's two lookup functions. It
adds no lookup body, native class registration, global USER process record,
guest CLS layout or automatic metadata lifetime. The leading next link is
required by the original pointer-to-pointer traversal. Registration scope
must populate the correct list and must not be inferred from a mutable
CS_GLOBALCLASS bit or GetClassInfo's caller-derived hInstance. The caller
must serialize lookup and retain every returned link for its use. Its
explicit context also supplies the system list and client-module identity.

## Window WW storage lifetime (S40)

The class-aware attach variant transfers one acquired class reference only
on success. Failed publication leaves ownership with the caller. Window
borrowers can obtain the same shared WC through their borrowed window binding;
the class reference is released only with the final window-storage reference.
Original createw.c assigns pwnd->pcls and calls ReferenceClass before callbacks;
this is its finite native-storage lifetime binding, not a copied WC or class
resolver. The existing attach API remains for paths without an owned class
record. The actual creation owner must select the class and publish before
callbacks; post-creation fixture attachment does not prove that ordering.

ADAPTER-WOW-027 in `wow_window_words_binding.c` supplies only native storage
for the original WW structure. Native properties bind it to an owned HWND;
acquire/release retains that storage across synchronous or reentrant callbacks,
and detach removes property ownership without invalidating outstanding borrows.
Original kernel/userk.h and handtabl.c use WND lock counts and per-thread
lock stacks for this guarantee; their kernel PHEAD/PTHREADINFO and destruction
engine are unavailable and are not imported or emulated. The native HWND
continues to own actual window destruction. No alternate HWND namespace,
class resolver, guest mapping, message thunk or scheduler is added.

The caller must attach the correctly initialized original WW before callbacks,
detach during creation rollback or WM_NCDESTROY, and release every borrow.
Publication locking does not serialize updates to WW fields or implement WOW
task scheduling. This mechanism is selected in the formal x86 WOW32 graph,
but no product window gateway attaches it yet. Its native destruction-during-
callback fixture uses the actual original DispatchClientMessage; poisoned-free
pages verify retention and exact cleanup, with allocation/publication failure
injection. Real guest callback/field publication remains unaccepted.

`wow_window_dispatch_bound` now acquires that same WW, calls the unchanged
original dispatcher, and releases in `__finally`. Before dispatch it refreshes
WW's style, exstyle and instance from the same native HWND through public
GetWindowInfo/GetWindowLongA. Original wowuserp.h aliases these fields to WND,
and original rtl/getset.c exposes the identical fields via public indices;
a creation-time snapshot is not equivalent. The three WOW-owned DWORDs and
private state flags are left untouched. Failed attribute queries neither
dispatch nor partially update the fields. This sampling does not provide
general direct-pointer synchronization, task scheduling or guest projection.
It neither masks exceptions
nor returns a successful dispatch when WW/callback/arguments are absent.
Native procedures do not require a WOW callback. A real SendMessage test
destroys its window inside the mock WOW callback; a separate raised-exception
test verifies cleanup and propagation. These enter the compiled binding,
not hand-retained fixture references. The product creation/registration path
still has to install it together with first-message WW publication.

## Original window dispatch binding (S40)

ADAPTER-WOW-026 is the declaration-only finite view in
`include/wow_window_dispatch_bindings.h`. It supplies a native HWND, a pointer
to the window-owned original WW storage, and the original-shape registered
WOW callback for one synchronous DispatchClientMessage invocation. It adds
no procedure classification algorithm, global current-window state, registry
or guest pointer. The original client.c owns tagged/native dispatch. The
caller must retain the window record/WW through nested calls, including a
callback that destroys its window; removing a native property cannot free
storage still borrowed by the call. Kernel PWND locking is not recreated.
The original body is linked and its native/tagged/nested branches pass the
isolated x86 fixture. The native window gateway and real guest callback have
not yet been connected, so this is not a working WOW window provider.


## Bitmap environment binding (S40)

ADAPTER-WOW-025 supplies the finite USER client environment for original
opennt-host `clres.c`/`cldib.c`: per-invocation compatible DC and screen
capabilities, original heap/macro shapes, and a shared recursive lock for
the original scaling scratch arrays. A thread-local binding is pushed and
restored at explicit enter/leave; no native pointer enters guest state.
`wow_bitmap_bindings_shutdown` is terminal teardown after every call has
returned, not a reset/restart API. Native isolated tests verify nested calls
and zero retained GDI objects, including four-thread and initialization-failure
tests. It is selected in the WOW32 provider graph; normal DLL detach calls
terminal shutdown after original W32DllInitialize. Actual DLL unload testing,
full resource leases and guest/output-table integration remain required. It owns no DIB
parser, color mapping or scaling algorithm. `wow_bitmap_load` retains the
original four-argument bitmap output-slot ABI and supplies that environment
only for resource-data conversion; native name/ID loads call the original
owner directly. The actual extracted WOW thunk, production lease, original
USER conversion and native GDI now pass a 96 KiB integration fixture.
Descriptor sourcing/handle aliasing are mocked there; production registration
and real Win16 consumption remain unaccepted.

This family preserves the historical WOW32/WOWEXEC host-product boundary.
The complete original WOW32 provider is selected on the x86 CCPU40 path. It
does not recreate NT4 USER, CSRSS, Win32k, a generic Win16 API, or a guest UI
server.

The prior copied callback transaction relied on retired machine-frame recovery
code. Its repository copy was removed; any retained comparison material is
external-only and is not a production source or build input.

## Registered bindings

The existing ADAPTER-WOW-001 lease binding also exposes an explicit protected
resource-address form. It reuses the worker's original CCPU descriptor lookup
and session lease, never the current PE/VM-dependent frame conversion. The
caller must resolve/lock its original resource first and supply its full
32-bit size. The 96 KiB and failure-cleanup fixture passes. WU32LoadBitmap now
uses it around the original output call after original selector resolution;
the extracted actual thunk passes ordering tests. This is not real Win16 or
output-table registration acceptance; names remain under the original macros.

ADAPTER-WOW-024: the original WOW32 DLL uses the NT x86 `/Gz` default,
not the standalone worker's `/Gd` default. Its three guest-memory/effective-
address imports explicitly retain the worker's cdecl ABI, including the local
fixed-width declaration in `wow_callback_frame_lease.c`. No pointer, mapping,
ownership or cleanup behavior changes. The provider build restores STD_CALL,
the original stdcall DLL initializer and the two private system import names.
The WOW-local `nt_vdd.h` shim qualifies three worker declarations while keeping
the imported ABI carrier byte-identical. DIV-294 records the original MVDM
worker header qualifications. This is not real guest acceptance.

ADAPTER-WOW-023 is retired with the entire five-function callback ABI bridge.
The original `/Gz` build supplies these calling conventions without wrappers;
the earlier `/Gd` mock stack experiment remains historical evidence only.

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-WOW-012 | RegisterUserHungAppHandlers also establishes WOWPROCESSINFO and the WOWEXEC event used by InitTask/SleepTask. | Original process/task state and event references are absent from modern USER's NT4 registration surface. | S40 removed the unbound non-null-arguments success stub. Restore registration, scheduling and process release as one contract; this is not merely optional hung-app notification. | OpenNT `ntuser/kernel/{exitwin.c,queue.c,taskman.c}`; S40 USER evidence |
| ADAPTER-WOW-022 | `WU32FillWindow` calls the private USER FillWindow output slot. | Modern USER32 has no exported FillWindow; original USER server PWND locks/GRE calls cannot be imported. | Public binding preserves parent-DC clipping, brush origin, same-process control-color callback and invalid-brush fallback. Native MM_TEXT pixel/cleanup tests pass; nondefault DC modes, callback destruction and real guest registration remain unaccepted. | `wow_private_user_compat.c`; original OpenNT `windows/core/ntuser/kernel/{paint.c,random.c}`; S40 USER evidence |
| ADAPTER-WOW-001 | `wow32/wcall16.c` obtains temporary `VDMFRAME` and `CBVDMFRAME` aliases with `GETFRAMEPTR`, flushes a written callback frame, then frees both aliases before calling `host_simulate`. | The NT4 `FlatAddress` form exposes process pointers; a native pointer cannot cross the current session boundary or the recursive CPU call. The original `vpStack` is a packed 16:16 address, not a SAS-linear address. | `wow_callback_frame_lease` first calls the selected CCPU40 `mvdm_softpc_effective_address` contract, then enters only the `ntvdm/softpc` bounded guest-pointer seam. That owner reaches the existing session guest-memory mapping instance. The original source retains task/frame algorithm, stack transfer, CPU resume, result and failure semantics; the selected `CallBack16` fixed-frame points now acquire, commit/release and reacquire at those original points. | `include/wow_callback_frame_lease.h`; `wow_callback_frame_lease.c`; `../softpc/{include/mvdm_softpc_guest_memory.h,mvdm_softpc_guest_memory.c}`; original target `../../mvdm-host/wow32/wcall16.c` |
| ADAPTER-WOW-002 | A late-loaded WOW32 DLL keeps the historical `Sim32FlushVDMPointer` function spelling after its selected CCPU profile makes that operation a no-op. | Under CPU40, original `softpc.h` defines this operation as `TRUE`; leaving the historical external declaration unresolved would incorrectly require a CPU30 pointer-cache provider. | `wow_sim32_pointer_compat.c` preserves the exact four-argument cdecl shape and always returns `TRUE`; it owns no pointer, mapping, cache or guest state. Callback-frame writes remain owned by ADAPTER-WOW-001. | `wow_sim32_pointer_compat.c`; original `../../mvdm-host/inc/softpc.h` CPU40 macro and `../../mvdm-host/softpc.new/host/src/sim32.c` declaration surface |
| ADAPTER-WOW-003 | `WU32RegisterClass`, `FindPWC`/`FindPWW` and `W32Win16WndProcEx` require NT4 USER private class/window words. | S40 native x86 probing found a readable -1 result with incompatible WW layout, rejected window-word setters, and no readable WC; low16 HWND validity separately passes one lifecycle test. | Historical `wow_window_projection` is absent from the current tree and is not a selected provider. Its sequential ID namespace and creation-time ordering cannot be adopted as completed recovery. The private-word binding and guest shared-object contract remain open in S40; no complete window/callback acceptance is claimed. | Original targets `../../mvdm/wow32/{wuclass.c,walias.c,wmdisp32.c}`; [current source/runtime evidence](../../../docs/etc/evidence/m0-t420-s40-wow-user-profile.md) |
| ADAPTER-WOW-013 | `WU32NotifyWow(FUN_FINALUSERINIT)` needs the private default-window-procedure bitmap and build field. | Modern USER32 does not populate the NT4 private output table; the earlier two-message subset incorrectly suppressed normal USER16 default processing. | Retain the complete original non-FE_IME `server.c` message union and `client.c::WowGetDefWindowProcBits` output contract, with checked buffer capacity. No window/server policy is reimplemented. S40 x86 host fixture passes; registration and real UI remain unverified. See [source and verification evidence](../../../docs/etc/evidence/m0-t420-s40-wow-user-profile.md). | `include/wow_private_user_compat.h`; `wow_private_user_compat.c`; original consumers `../../mvdm/wow32/{wow32.c,wuman.c}` |
| ADAPTER-WOW-014 | W32Thread requires original USER task initialization before guest resume. | Modern USER supplies no NT4 task owner. | S40 replaces the removed validation-only wrapper with a worker-local carrier that calls original registration, InitTask, yield/wait and DestroyTask through one USER32 queue boundary. It is deliberately not yet published: W2 cleanup/message ownership and the other output slots must complete before original all-or-nothing registration. | `include/wow_user_task_lifecycle.h`; `wow_user_task_lifecycle.c`; Original `../../mvdm/wow32/wkman.c`; OpenNT `ntuser/kernel/{taskman,queue,exitwin}.c`; S40 USER evidence |
| ADAPTER-WOW-015 | `WU32WOWGetIdFromDirectory` invokes the private `pfnWOWGetIdFromDirectory` output slot to select a grouped icon/cursor resource ID. | Current USER32 leaves the NT4 slot null even though its public resource selector is still present, causing an execute-at-zero fault once USER16 proceeds past boot. | `wow_private_user_get_id_from_directory` validates RT_ICON/RT_CURSOR and delegates only the directory selection to public `LookupIconIdFromDirectory`; it owns no USER state or resource lifetime. | `include/wow_private_user_compat.h`; `wow_private_user_compat.c`; original targets `../../mvdm-host/wow32/{wow32.c,wuser.c}` |
| ADAPTER-WOW-016 | `WU32LoadCursor` invokes `pfnServerLoadCreateCursorIcon` with materialized Win16 resources and Unicode names. | Modern USER32 does not populate the original table. | Original private and public clres.c entries share ConvertDIBIcon: use CreateIconFromResourceEx with the original default-size/shared/monochrome flags and resource-format version, not the application version. The no-data branch uses LoadImageW. Remove the handwritten DIB parser and invalid-data fallback. Native format/size/hotspot/pixel/failure tests pass; original wcuricon.c resource identity and real guest cleanup remain integration gates. | `include/wow_private_user_compat.h`; `wow_private_user_compat.c`; original `../../mvdm/wow32/{wucursor.c,wcuricon.c}`; [evidence](../../../docs/etc/evidence/m0-t420-s40-wow-user-profile.md) |
| ADAPTER-WOW-021 | `wmsg16.c` translates WM_MENUSELECT popup identity into an immediate-child index through pfnGetMenuIndex. | NT4 receiver locks and PMENU pointers cannot be used in this worker. | Preserve original validation-before-search, error 1401/zero versus valid-not-found -1, and immediate-item ordering through IsMenu/GetMenuItemCount/GetSubMenu. Preserve the original sign-extended WORD identity only after validation. Native positive/negative/lifetime tests pass; table registration and real guest WM_MENUSELECT remain open. | `wow_private_user_compat.c`; `include/wow_private_user_compat.h`; original OpenNT `windows/core/ntuser/kernel/{ntstubs.c,validate.c}` |
| ADAPTER-WOW-017 | Original yield/directed-yield/sleep coordinate task execution, message events and send/receive locks. | OS thread scheduling alone does not serialize the shared CCPU or implement USER task policy. | The lifecycle carrier calls original taskman.c and uses one `MsgWaitForMultipleObjectsEx` wake only where NT4 used a kernel queue wait; it does not create a second scheduler or synthetic SMS. Real message ownership, cross-task delivery and output-table publication remain W2/W7 gates. | `wow_user_task_lifecycle.c`; Original `../../mvdm/wow32/{wkman.c,wkgthunk.c}`; OpenNT `ntuser/kernel/taskman.c`; S40 USER evidence |
| ADAPTER-WOW-049 | Original USER enters its exclusive domain for server work, drops it around user-mode callbacks, then reacquires it before continuing. | The standalone worker has no NT4 USER resource, `gptiCurrent` source or CSRSS-created thread records. | S42 gives `ntvdm.exe` exactly one worker-local runtime and attaches it to the existing session thread-hook lifecycle. The late-loaded WOW32 DLL imports only current/enter/leave/set-context from that parent; it cannot acquire a second TLS/lock instance. This is synchronization and current-task binding only, not a replacement `SHAREDINFO` plane, USER server, task table, or guest ABI. | `include/{wow_user_runtime,wow_user_session_binding}.h`; `wow_user_{runtime,session_binding}.c`; `../softpc/mvdm_standalone_worker.c`; original OpenNT `ntuser/kernel/{validate.c,ssend.c}`; S42 W1/W2 ledger |
| ADAPTER-WOW-018 | WOW task/module cleanup releases original task-owned USER resources. | The previous success-only wrapper neither released its task events nor provided projected-window cleanup. | S40 removed the unbound no-op. Task/resource cleanup remains an explicit implementation and acceptance obligation. | Original `../../mvdm/wow32/{wow32.c,wuser.c,wkman.c}`; S40 USER evidence |
| ADAPTER-WOW-020 | `WU32LoadMenu` invokes NT4 USER's private `pfnServerLoadCreateMenu` output slot for a materialized Win16 menu. | Modern USER32 leaves that slot null; a packed WOW module is not a native PE resource module. Native indirect-menu parsing also misinterprets Win16 owner-draw pointers. | ADAPTER-WOW-037 routes the converted template through recovered original `CreateMenuFromResource`, with explicit Win16 invocation semantics. Automatic class-menu loading through `createw.c -> xxxClientLoadMenu -> LoadMenuW` remains a distinct integration requirement under ADAPTER-WOW-036. Earlier attribution to `W32CreateWindow` was incorrect. | `include/wow_private_user_compat.h`; `wow_private_user_compat.c`; `include/wow_menu_parser_bindings.h`; original `../../mvdm/wow32/wumenu.c` and OpenNT `ntuser/{kernel/createw.c,inc/ntcb.h,client/clmenu.c}` |
