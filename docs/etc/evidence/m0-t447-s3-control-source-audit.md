# T447 S3 control source audit

Admission follows continued owner execution after S2 P16 f47d951b0.
Single-person dual role; source-led dependency analysis before grouped wiring
and acceptance. Published images remain P16; no runtime source change or new
runtime test belongs to this initial audit.

Original reference: O:/repos.external/OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, read-only. Original guest media is
immutable. A registered callback or matching symbol is not runtime proof.

## Fixed source-audit groups

| Group | Original owner and current integration | Required disposition/proof |
| --- | --- | --- |
| C1 children | wow_window_creation_binding.c enroll_internal already handles explicit/dialog claims and standard classes with enrolled parents. | Trace class selection and child teardown; reuse ownership; cover direct/template/internal creation and rollback. |
| C2 EDIT | wow32.c registers original W32LocalAlloc/ReAlloc/Lock/Unlock/Size/Free and W32EditNextWord; wcall32.c retains memory callbacks. | Audit original EDIT consumer/package closure, actual callbacks and lifetime; no second heap or editor. |
| C3 dialog DS | Original client.c GetEditDS/ReleaseEditDS, consumed by dlgbegin.c/dlgmgr.c. | Shared/private EDIT data, allocation failure, unwind and teardown together with C2. Do not allocate unused segments just to satisfy tests. |
| C4 MENU-01 | Existing wow_menu_client/native_bindings and selected original clmenu parser; S2 shared-view foundations. | Distinguish production-selected from staged code; creation/resource/hierarchy/mutation/callback/retirement closure, no second parser. |
| C5 applications | SOL/WINMINE gameplay and WRITE initialization retained. | Trace actual consumers before repair; separate app frontiers, no inference of editing/saving/full exit from initialization. |

## Confirmed selection gap

wow_user_registration_bridge.c saves PFNWOWHANDLERSIN as wow_input_handlers.
Original wow32.c registering Local* and EditNextWord does not prove consumption.
Selected opennt-host client.c contains DispatchClientMessage and separately
compiled RegisterClassWOWA, not GetEditDS/ReleaseEditDS. The provider graph
does not select editec/editsl/editml translation units.

Original ntuser/client/globals.h routes allocations through pfnLocalAlloc and
peers; usercli.h routes WOW word-break through pfnWowEditNextWord. Original
client.c GetEditDS calls registered16-bit GlobalAlloc with GHND|GMEM_SHARE,
256 bytes; ReleaseEditDS calls registered GlobalFree with LOWORD(h). These
are source contracts, not permission to rewrite their algorithms locally.
Original dlgbegin.c consumes the allocation and dlgmgr.c releases it.

The missing consumer selection is established; its exact effect on WRITE is
not yet attributed. Native EDIT availability does not prove Win16 handle/DS
contracts, nor does this finding establish every ordinary edit operation fails.

## Before runtime edits

Enumerate original EDIT-family outbound dependencies, callback reentry,
shared/local allocation ownership and checked guest-memory lifetime. Evaluate
unchanged composition, then the smallest same-shaped facade; identify concrete
private USER/server stopping boundaries before considering exceptional code.
Register any mirror intrusion and remove superseded autonomous implementation
in the same group. No original guest source is a host-link input.

C1/C4 reuse established S2 boundaries; audit concrete functionality without
reopening the approved foundation scope. No mirror diff is admitted yet.
Grouped runtime acceptance follows dependency-complete wiring, not each leaf.

## EDIT and dialog contract traced beyond registration

The original host EDIT package has seven implementation units, not merely the
three main dispatch files: editec.c, edecrare.c, editsl.c, edslrare.c, editml.c,
edmlrare.c and edmlonce.c under windows/core/ntuser/client. ECNcCreate in
edecrare.c allocates text through LOCALALLOC using the original window module;
the same family owns release. MLCreate in edmlonce.c separately allocates
the native line-index array. Text storage and host bookkeeping therefore must
not be collapsed into one heap or subjected to a global LocalAlloc redirect.

Original wow32/wcall32.c documents the text handle contract: LOWORD is the
Win16 local handle, HIWORD is its data selector. W32LocalAlloc chooses native
allocation only when LOWORD(hInstance)==0; otherwise LocalAlloc16 supplies
the composite value. W32LocalReAlloc also updates an existing parameter-map
node when its optional pointer is present. W32LocalLock returns the mapped
guest buffer, not a native heap allocation. This rules out treating the value
as an ordinary modern HLOCAL or retaining a borrowed native pointer after its
callback/lease epoch.

Original wmsgem.c ThunkEMMsg16 constructs that composite value for
EM_SETHANDLE from guest wParam and the window instance selector. UnThunkEMMsg16
converts EM_GETHANDLE back through GETHMEM16. EM_SETWORDBREAKPROC tags the
guest procedure with WNDPROC_WOW; original usercli.h CALLWORDBREAKPROC selects
W32EditNextWord, which calls the original RET_SETWORDBREAKPROC callback.
No project-owned matching EDIT-message handler was found in wow32-dll/source
or ntvdm-exe. The original converter cannot simply deliver these encoded
values to a native control whose implementation does not consume that contract.
This is a source-established integration obligation, not a claimed observed
application crash or permission to add an API-specific workaround.

The dialog branch is conditional: dlgbegin.c sets CD_GLOBALEDIT when
DS_LOCALEDIT is absent, allocates one shared segment lazily at the first EDIT,
and passes that segment as the child's instance. Subsequent EDIT children share
it. dlgmgr.c releases hData on WM_NCDESTROY/WM_FINALDESTROY and clears it;
DS_LOCALEDIT avoids this shared allocation. Required negatives include no EDIT
child, allocation failure, multiple children, partial creation and repeated
terminal cleanup. The stale GetEditDS source comment naming DS_LOCALEDIT does
not override these executable branch conditions.

## Direct-composition boundary identified

An unchanged host EDIT unit expects precomp.h's private USER environment:
PWND/PEDITWND/PED storage, FNID/message masks, control lookaside initialization,
style mutation and CsSendMessage for ANSI/Unicode thunking. Public-looking
NtUser paint/caret/focus/capture leaves are not the whole dependency closure.
CsSendMessage and kernel-owned window internals cannot be satisfied by importing
USER server or by silently using a modern native EDIT's private storage.

The next design comparison must preserve the seven-unit original editing
algorithms while adapting the finite window/control carrier and API leaves,
and account for parent notification/reentry, combo internal edits and text
pointer lifetime. No imports or runtime edits are made until this closure is
enumerated. Importing only GetEditDS would create an unused allocation path,
not solve the reached storage/callback contract. Likewise, forwarding only
EM_SETHANDLE/GETHANDLE would introduce a second text-storage policy and is not
selected as a shortcut.

## Outbound dependency classification

Read all seven units' lexical call candidates, then resolve the non-public
families against their definitions and reached call sites. The lexical list
includes macros/declarations and is not by itself a complete call graph.
The following concrete boundaries govern the prospective facade; none is
permission to stub a private operation with unconditional success.

| Boundary | Source evidence | Required composition |
| --- | --- | --- |
| Text and bookkeeping heaps | ECNcCreate/ECNcDestroyHandler, MLCreate and original wcall32.c | Keep Local* callbacks for guest text and native UserLocal/UserGlobal bookkeeping distinct; retain realloc parameter-map update and nested lock ownership. |
| Control carrier | usercli.h ED/EDITWND; EditWndProcWorker uses PEDITWND.ped and INITCONTROLLOOKASIDE | Retain original ED algorithm state in the existing window owner; never reinterpret modern USER's private EDIT allocation or guest-visible WND as host ED. Prove failure and post-destroy access refusal. |
| Window reads and mutation | ValidateHwnd, TestWF/ClearWindowState, REBASEPWND, FNID and original lookaside | Reuse existing enrolled-window identity/borrow/retirement; audit every style write and cleaned-up marker. A sampled read-only graph is not writable native state. |
| A/W entry and conversion | EditWndProcA/W, FWINDOWMSG, CsSendMessage, gabThunkMessage | Preserve message translation and one execution of original edit logic; no fake CsSendMessage, raw pointer forwarding or blanket ANSI-only success claim. |
| Parent callbacks | ECGetControlBrush branches on PtiCurrent versus GETPTI; original SendMessageWorker and DefWindowProcWorker | Use established native-call/task scopes. Preserve the foreign-thread default-brush branch instead of sending a new cross-thread callback. |
| Combo internal EDIT | SLCreate locates CBLISTBOXID; ECNcDestroyHandler explicitly calls ComboBoxWndProcWorker with WM_PARENTNOTIFY/WM_DESTROY | Account for canonical parent control dispatch and once-only destruction notification. Native combo internals must not be interpreted as original PCBOX or silently replaced. |
| Clipboard cache | ECCopy calls pfnWowEmptyClipBoard when CI_16BIT after EmptyClipboard | Keep original callback and scope, not just native clipboard copy. This is the reached EDIT dependency, not admission of an entire clipboard subsystem. |
| Native leaves | NtUser paint/DC/caret/focus/capture/scroll/menu APIs and system timers | Map same-contract available APIs after signature/order review; system timer message kind and teardown need separate equivalence proof, not ordinary timer substitution. |
| Font/drawing helpers | GdiGetCharDimensions in ntgdi/client/cfont.c; GetCharWidthInfo in dcquery.c; CopyInflateRect in ntuser/rtl/rect.c; DrawFrame in rtl/draw.c | Prefer already selected original owners or finite original fragments; identify unavailable private glyph metric inputs before choosing a facade. No whole GDI import. |
| Optional language package | SUPPORT_LPK branches and charset callbacks | Determine actual original build selection before exclusion; unsupported locale behavior cannot be inferred from Latin fixture success. |

The original ED includes selection, wrapping, undo, font metrics, password
encoding and lock depth. Its native line/undo allocations are not a duplicate
guest heap. Scope review therefore rejects a native-EDIT text mirror as a
shortcut: synchronizing that second buffer would become a separate editing
implementation rather than composing the original algorithm.

Concrete next implementation admission requires resolving the A/W conversion,
system-timer and combo canonical-call boundaries above, plus the exact
registered state-layout bridge. These are finite dependencies of C2/C3, not
additional S tasks or a reason to expand to full USER/GDI. C1/C4 source audit
and existing selected-owner reuse can proceed independently meanwhile.

## C4 selected menu owners versus adaptation

Procedure: read the provider generator's source list and clmenu owner loop,
selected clmenu.c, wow_menu_resource_bindings.h, wow_menu_parser_bindings.h,
wow_menu_native_bindings.c and the capture/commit portion of
wow_menu_client_bindings.c; follow LoadMenu callers and callback refresh sites
with rg. This establishes source selection, not a new runtime pass.

- New-T404S5Wow32ProviderNinja.ps1 selects clmenu plus both menu binding
  translation units. The original MenuLoadWinTemplates,
  MenuLoadChicagoTemplates and CreateMenuFromResource own parsing; original
  CommonLoadMenu/LoadMenuA/W use W32 resource callbacks. The registered
  OPENNT-HOST-043/045 slices are production inputs, not staged-only code.
- wow_private_user_load_create_menu receives the template after original
  WU32LoadMenu/ConvertMenu16, then invokes that same original parser. Its
  explicit wow16 argument retains packed owner-draw guest data rather than
  treating it as a Unicode string. There is no second parser here to delete.
- Parser allocation/insertion/failure destruction macros reach the same
  task-associated native menu boundaries as direct WOW menu APIs. Modern USER
  owns actual menu mutation; capture_tree copies ID/type/state/submenu edges
  for original direct readers. Its copied graph is not a replacement menu
  mutation algorithm. Publication prepares the whole tree before committing
  cleanup ownership; failed preparation frees newly allocated records.
- Callback roots are not limited to GetMenu(window): refresh_message covers
  popup/system and owner-draw menu identities. Window/menu association is
  published before detached-menu retirement. These are S2 execution and
  object-lifetime foundations to retain, not a rationale to reimplement them
  during S3.

Existing wow_original_menu_resource_fixture.c explicitly substitutes W32
resource callbacks; its parser/owner-data/invalid-template checks do not prove
guest resource execution. wow_menu_index_fixture.c uses native menus and
checks immediate rather than recursive submenu index, narrow handles,
detachment and destroyed handles; it likewise is not guest acceptance.
C4 final acceptance must map production guest resource loading, owner-draw
data/callbacks, mutations and direct-reader visibility to actual guest probes.
Do not infer this coverage from the fixture names or the original source
selection alone. Remaining C4 audit includes those exact probe assertions
and retirement/reuse evidence, rather than another menu-parser import.

## EDIT system-timer boundary: concrete mismatch

Read-only source comparison resolves the purpose, but not yet the modern
binding: editml.c MLMouseMotion arms IDSYS_SCROLL while dragging beyond the
control, rearms its interval, and cancels on button release; MLEditWndProc
also cancels on WM_CAPTURECHANGED and consumes WM_SYSTIMER by calling the
original MLMouseMotion with saved mouse coordinates while fMouseDown is set.

Original ntuser/kernel/timers.c _SetSystemTimer calls InternalSetTimer with
TMRF_SYSTEM and checks the window's process owner. KillSystemTimer uses
KillTimer2(..., TRUE), while ordinary KillTimer passes FALSE. Expiry chooses
WM_SYSTIMER versus WM_TIMER from that flag. Thus a direct macro alias to
SetTimer/KillTimer is not equivalent: it changes message kind and timer
identity class, potentially sharing an application's timer identifier.
Importing the kernel timer scheduler is outside the admitted boundary.

The required finite binding must preserve window/task ownership, distinguish
control-private timers from application timers, rearm/cancel semantics and
post-destroy refusal, while leaving MLMouseMotion as the scrolling algorithm.
Acceptance must include a simultaneous application timer with the same
numeric ID, capture loss, release and destruction, not only visible scrolling.
No timer substitution has been implemented or represented as verified.

## C1/C2 creation and canonical procedure identity

Procedure: read wow_standard_class_bindings.c and the explicit/internal
creation enrollment in wow_window_creation_binding.c, then follow retained
native calls in wow_class_words_binding.c/wow_window_words_binding.c and
original walias.c/wmdisp32.c. Current standard-class preparation imports the
modern USER class descriptions using the original stdClasses classifier;
enroll_created_window snapshots the actual previous per-window procedure
before installing the common WOW entrance. That previous target may be an
opaque CallProc value. Rollback restores it only while this enrollment still
owns the window. These ownership mechanics are reusable.

There is another reached entry besides normal window dispatch:
wmdisp32.c W32Win16WndProc intercepts EM_SCROLLCARET and calls
GetStdClassWndProc(WOWCLASS_EDIT) directly, intentionally bypassing the guest
subclass because that message is Win32-only. walias.c caches the result of
GetClassInfo in the original stdClasses entry. wsubcls.c also compares these
canonical procedures when mapping subclass procedures. Therefore replacing
only per-window native dispatch would leave a route back to modern EDIT
against the wrong state. The original EDIT integration must give standard
class queries, cached canonical procedure identity, subclass restoration and
normal dispatch the same selected owner. No guest thunk patch is proposed.

The existing hook point for canonical queries is already present:
wow_user_private_access.h redirects selected GetClassInfoA calls to
wow_user_get_class_infoA, and wow_class_client_query_native substitutes the
registered binding's logical target before returning the class description.
Prefer extending that one owner when the EDIT provider is ready; adding a
second stdClasses table or per-message EM_SCROLLCARET special case would
duplicate original routing. Current native targets remain unchanged until
the state/creation/destruction group can move together.

Required checks include a guest-subclassed multiline EDIT receiving
EM_SETSEL/EM_SCROLLCARET, calling its old procedure, then restoring it; merely
showing text in a newly created non-subclassed EDIT does not cover this route.

## A/W conversion and combo destruction findings

Original clmsg.c CallWindowProcAorW only requests CsSendMessage conversion
for a validated tagged CallProcData handle and a message in gabThunkMessage.
An ordinary function address goes to CALLPROC_WOWCHECK. Consequently calling
CallWindowProcA with a raw imported Unicode procedure is not a general-purpose
A-to-W conversion facade. Existing project native-call wrappers correctly
retain actual native CallProc values; they do not prove that arbitrary imported
procedures acquire that capability. EDIT's own A/W entry conversion and the
creation encoding must be resolved together, including subclass calls.

Original comboini.c creates its EDIT with ES_COMBOBOX, CBEDITID and encoding
derived from the combo's WFANSICREATOR. edslrare.c finds CBLISTBOXID for that
edit. ECNcDestroyHandler directly invokes ComboBoxWndProcWorker, whose
WM_PARENTNOTIFY/WM_DESTROY case clears SEDITABLE, sets fNoEdit and replaces
spwndEdit with the combo itself (or clears the list pointer for list death).
This is internal parent-state repair, not just an optional application
notification. A generic SendMessage substitution could invoke a guest
subclass instead of that canonical owner and cannot be assumed equivalent.
Neither importing the original PCBOX layout over modern combo memory nor
silently leaving both EDIT implementations alive is an acceptable bridge.

The design must explicitly cover the modern parent's retained canonical
procedure and once-only child teardown, or compose the original combo owner
if its finite dependency audit permits it. This is still an unresolved
integration boundary, not an implemented solution or a new semantic waiver.

## Selected EDIT memory path, not a hypothetical lease provider

Procedure: trace W32LocalLock/Unlock through the selected wcall16.c
preprocessor branch, VDMPTR through wow32.h/wow32.c, and descriptor publication
in dpmiselr.c; compare original ECLock/ECUnlock/NextWordCallBack and
ParamMapUpdateNode. This refines the earlier generic lifetime obligation:
there is no evidence that W32LocalLock currently acquires a separate
project mapping lease which an EDIT adapter should duplicate.

wcall16.c unconditionally defines WOWFASTEDIT. The selected LocalLock16 and
LocalUnlock16 manipulate the original LOCALHANDLEENTRY for moveable handles,
including discarded and saturated-count cases; the RET_LOCALLOCK/UNLOCK
callback implementations are the unselected branch. Alloc/realloc/size/free
still use their original guest callbacks. Tests must assert the selected
branch, not require lock/unlock callback trace events that cannot occur.

W32LocalLock resolves the returned selector:offset through VDMPTR and
GetPModeVDMPointerAssert. That owner checks ExpLdt/presence, may force the
original segment fault, and consumes the native FlatAddress cache.
dpmiselr.c's existing DIV-310 publishes GetDescriptorMapping plus IntelBase
where applicable; the guest-linear FlatAddress view is distinct. Reuse this
selected address route. This finding does not prove arbitrary pointer
retention, segment remapping or cross-thread use is safe.

Original ECLock increments ED.iLockLevel. ECUnlock's last password-buffer
unlock takes an additional LOCALLOCK/UNLOCK pair while encoding, then releases
the ordinary lock and decrements ED.iLockLevel. NextWordCallBack retains its
text pointer across multiple CALLWORDBREAKPROC calls before ECUnlock;
W32EditNextWord can execute guest code during those calls. A wrapper scoped
only to the return of W32LocalLock would therefore be too short-lived.
W32LocalReAlloc retains ParamMapUpdateNode's existing pointer-alias refresh;
do not add a second parameter-map policy.

Integration keeps the original seven-unit EDIT algorithms and these memory
owners. Required grouped witnesses are fixed/moveable text handles,
discarded/refused lock, nested password locking, realloc success/failure with
parameter-map refresh, word-break guest reentry, and final release including
failed creation. Exact guest handle/count state and callback results are
needed; a native edit displaying copied text cannot establish this contract.

## Bounded A/W message inventory

Procedure: intersect names in kernel/server.c gawEditWndProc with TRUE entries
of client/clglobal.c gabThunkMessage, then resolve their inc/messages.h
dispatch families. The ten named matches below are from the pinned source,
not an invented list based on sample applications. The source arrays and
conditional compilation remain authoritative; this textual intersection is
not a preprocessed proof of every regional build.

| EDIT message | Original conversion family |
| --- | --- |
| WM_CREATE, WM_NCCREATE | INLPCREATESTRUCT |
| WM_SETTEXT, EM_REPLACESEL | INSTRINGNULL |
| WM_GETTEXT | OUTSTRING |
| WM_GETTEXTLENGTH | GETTEXTLENGTHS; FE_SB selects GETDBCSTEXTLENGTHS |
| EM_GETLINE | INCNTOUTSTRING |
| EM_SETPASSWORDCHAR, WM_CHAR | INWPARAMCHAR; FE_SB selects INWPARAMDBCSCHAR |
| WM_SYSCHAR | INWPARAMCHAR |

CsSendMessage is not an ordinary callable helper: usercli.h selects
gapfnScSendMessage for sub-WM_USER messages and NtUserfnDWORD otherwise.
The table resolves to private USER services. ntcb.h's INCNTOUTSTRING
send/receive pair demonstrates why directly importing that transport is not
the finite solution: it depends on capture buffers, locked PWND, kernel/client
callback dispatch, sender-death/reply flags and copy-out ownership. Those
server dependencies remain excluded. Its EM_GETLINE count-prefix and
non-NUL-terminated output contract must nevertheless be retained by the
eventual finite binding, including encoding-dependent capacity and result.

Original build lineage: client/dirs selects daytona by default and lists fe
and keizon as optional; daytona/sources includes client/sources.inc, whose
source list contains all seven EDIT units and whose C_DEFINES add UNICODE and
USER_C_DEFINES. No SUPPORT_LPK definition was located in the searched header,
sources/include and makefile inputs; inherited USER_C_DEFINES is not resolved
by that absence. Do not turn this search result into a claim that every
optional language branch is unsupported or safe to remove.

This inventory reduces the conversion audit to six family boundaries plus
the regional alternatives; it does not authorize a handwritten broad USER
message converter or classify S4's complete messaging contract as complete.

## Control-carrier placement and terminal contract

The selected ownership seam is the existing wow_window_words_binding, not a
second HWND map or modern EDIT's offset-zero storage. It already owns a class
reference, WW, cleanup membership and references that survive property detach.
The prospective original EDITWND/PED carrier must be associated with that same
identity and borrowed through the same dispatch lifetime. Native control
storage is private host state: it must not be written into guest WND pages or
the underlying modern class's reserved extra bytes. This is a placement
decision, not an implemented provider.

Original usercli.h supplies more than allocation macros:
VALIDATECLASSANDSIZE rejects wrong-FNID and CLEANEDUP/DELETED controls before
dereferencing PED. For a zero FNID it verifies the available extra-word size.
INITCONTROLLOOKASIDE lazily initializes the allocator, stores PED at the
original EDITWND offset, sets PED.pwnd, and on failure marks CLEANEDUP and
destroys the window. An adapter cannot replace either macro with a no-op.
NtUserSetWindowFNID in kernel/ntstubs.c ORs the status bit, accepting an
initial class only when GETFNID is zero; blindly overwriting the class would
lose the original repeated-destruction rejection.

EditWndProc sends both WM_NCDESTROY and WM_FINALDESTROY to
ECNcDestroyHandler; the validation/status contract prevents two releases.
Current wow_window_words_release runs only on the final native borrow and is
not a guest-capable cleanup entry. Original text/undo/line release therefore
belongs to the original terminal dispatch while WOW execution is valid, not
to a newly added final-reference destructor that might run after task exit.
The binding's final release may free only already-retired host carrier storage.

ECNotifyParent synchronously sends WM_COMMAND using the original child ID.
Several caller paths explicitly re-check IsWindow after a notification while
others resume reading PED. This source observation does not itself establish
a defect or justify changing all notifications. Group acceptance must destroy
the EDIT from EN_UPDATE/EN_CHANGE and a word-break callback, including nested
dispatch and creation refusal, and attribute any failure before correcting
original logic. Holding WW alone does not automatically retain separately
freed PED buffers. Do not claim reentrant EDIT lifetime is proved by the S2
window-borrow tests.

## Native drawing and timer export census

Read-only PE inspection used MSVC14.43.34808 Hostx64/x86 dumpbin.exe
(/exports, /headers and bounded /disasm:nobytes /range). No DLL was loaded by
a probe, modified or copied; these observations are availability/ABI evidence,
not runtime acceptance. Input SHA256:

- C:/Windows/SysWOW64/gdi32.dll, file version10.0.26100.8521:
  7C3CB17BC41A00E5167A82AEF67D0DA4A09E73E6DEBA9ADDCDC8B20A90B5186C.
- C:/Windows/SysWOW64/user32.dll:
  56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E.
- C:/Windows/SysWOW64/win32u.dll:
  31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C.

gdi32 exports GdiGetCharDimensions (RVA7C20), GetCharWidthInfo (5090) and
PolyPatBlt (89E0). The installed SDK10.0.22621.0 x86 gdi32.lib symbol census
does not expose those names. Original wingdip.h supplies the declaration and
CHWIDTHINFO/POLYPATBLT layouts; gdi32.prf identifies GdiGetCharDimensions@12
and NtGdiGetCharWidthInfo@8. Original daytona/gdi32.def aliases
GetCharWidthInfo directly to NtGdiGetCharWidthInfo; the apparent dcquery.c
wrapper is inside #if0 and must not be recorded as its selected producer.

Prefer an explicitly resolved native export boundary, with signature and
behavior verification, over importing font-engine state or reimplementing
GdiGetCharDimensions's 52-character rounding and font caches. The export's
presence alone does not prove cross-version availability or modern semantics.
GetCharWidthInfo requires outline-driver support and transformed metrics in
original GreGetCharWidthInfo; replacing it with constant zeros or unconditional
FALSE would silently discard the f40Compat/TrueType path. Original DrawFrame
can retain its four-rectangle ordering and brush/ROP calculation with the
native PolyPatBlt leaf. CopyInflateRect is independently composable original
rectangle arithmetic. Neither requires importing the GDI server.

user32 has no named SetSystemTimer/KillSystemTimer export in this census.
win32u does export NtUserSetSystemTimer (RVA69F0) and NtUserKillSystemTimer
(5DF0). At imagebase10000000 their entry stubs return with ret0Ch and ret8,
respectively. Thus the modern x86 set entry consumes three stack arguments,
not the four in original NtUserSetSystemTimer; a direct four-argument function
pointer alias would corrupt stack balance. Both reached original EDIT calls
pass a NULL fourth callback argument. This makes a finite native binding a
candidate, not permission to invent a syscall number or infer all argument
semantics from stack size. Verify the HWND/ID/interval contract, WM_SYSTIMER
delivery, ordinary-timer namespace separation and cancellation in the grouped
acceptance before selecting it as the production provider. No timer code has
been changed and no synthetic WM_TIMER route is selected.

### Modern system-timer callers

The same user32 image imports both services from win32u. Resolve its PE import
name-table entries directly, not by counting only Nt-prefixed lines in dumpbin:
that textual shortcut omitted other imports and initially produced incorrect
IAT addresses, so the resulting call-site matches were discarded. The name
table is RVA EFD74 and IAT RVA EE7A0. The adjacent sequence is
SetProcessUIAccessZorder, DestroyMenu, GetRawInputBuffer, ScrollDC,
KillSystemTimer, SetSystemTimer. Correct timer IAT addresses at base10000000
are 100EEB54 and 100EEB58.

At 10069447--1006944F the native caller pushes an interval derived from a
shared metric, ID FFFE, then the HWND, and calls the set import. At
1006A951--1006A958 it pushes the same ID and HWND and calls kill; other
callers at 10093E97--10093EA5 and 10093E31--10093E39 use the same pair.
Together with the win32u ret0Ch/ret8 stubs, this supports the candidate
signatures UINT_PTR WINAPI set(HWND, UINT_PTR, DWORD) and
BOOL WINAPI kill(HWND, UINT_PTR). These are inspected host ABI bindings,
not permission to bake addresses, ordinals or syscall IDs into the product.

Design disposition: resolve the named services from the installed system
module and retain their actual system-timer class; adapt the original EDIT's
always-NULL callback argument at one same-shaped facade. Do not create a timer
registry, worker scheduler or WM_TIMER-to-WM_SYSTIMER translation. Unsupported
export/ABI availability must be explicit, never silently switch semantics.
The grouped native/guest verification still must prove interval rearm,
same-ID application-timer independence, capture loss, cancellation and
destroyed-window behavior before this binding is released.

Later local Win2000SRC/winxpscodes ntuser.h declarations were read only as
comparison and still have four arguments; none is an imported or linked
dependency. A web search returned historical declarations and did not supply
modern ABI evidence. The retained modern conclusion rests on the named local
PE inputs and actual caller instructions above.

## A/W design: native CallProcData, not six new converters

Original kernel/getset.c GetCPD associates a conversion with a class/window,
deduplicates the target procedure plus options and lets USER own the resulting
CALLPROCDATA lifetime. NtUserGetCPD validates HWND/options and invokes that
owner. Original client/clmsg.c CallWindowProcAorW consumes the opaque handle
and selects conversion only for messages that need it. This is the mechanism
to reuse, not an invitation to recreate the six conversion families.

In the previously hashed current x86 win32u image, NtUserGetCPD is exported at
RVA1590 and its stub returns ret0Ch. Direct PE import-table reading identifies
user32's corresponding IAT slot100EEC28. Its helper1007E365 pushes procedure,
options and HWND before invoking that import. Callers at100269AD,
100276DC and1002953A compute respectively 81/82,41/42 and21/22 options,
matching original CPD_WNDTOCLS, CPD_DIALOG and CPD_WND with direction1/2.
This is stronger ABI evidence than the exported name alone, but not yet a
passing runtime contract test.

Selected finite design for EDIT's CsSendMessage mismatch branch:
resolve native NtUserGetCPD by name; request CPD_WND plus the conversion
direction for the opposite original EDIT entry supplied by that branch;
invoke the returned opaque value with CallWindowProcA or W according to the
incoming encoding. The opposite original entry then sees matching ED.fAnsi
and runs the editor once. No temporary GWL_WNDPROC replacement, synthetic
window, manual string converter, extra HWND registry or message reinjection
is required. Restrict this facade to the audited FNID_CALLWINDOWPROC branch,
not arbitrary USER send services. Do not reinterpret CPD values as pointers
or WOW-tagged guest procedures; reuse the existing native-call scopes.

Keep native failure and original call ordering explicit. Verify both
directions, the ten reached message kinds (including EM_GETLINE's count word),
nested guest subclass calls, target destruction and stale conversion use in
the grouped acceptance. Class-associated CPD ownership and module lifetime
must agree with the worker's existing class/DLL lifetime; no invented CPD
free API or process-wide window mutation is selected. Export absence or a
failed ABI witness cannot silently fall back to raw-pointer dispatch.

## Creation selection must preserve the recovered owner

Source inspection of wow_window_creation_binding.c, specifically
enroll_created_window and create_bound, establishes another required edge in
the same EDIT group. For a borrowed native class, enrollment currently replaces
an untagged selected procedure with GetWindowLongA(GWL_WNDPROC), before
attaching WW and installing wow_window_native_proc. This is intentional for
the current native-class baseline, but would discard a recovered original
EDIT entry even if class lookup had already selected it. Merely publishing
EditWndProcA/W in the class query cache is therefore insufficient.

The integration must distinguish the backing native registration from its
selected control implementation. Retain the existing native per-window target
for classes without a recovered owner; preserve the selected original EDIT
target for recovered EDIT. Do not infer the distinction from a pointer tag,
class spelling or the existence of a borrowed native registration. The same
class-owned selection must govern explicit and original internal child
creation, canonical GetClassInfo/GetStdClassWndProc queries and subclass
restoration. Native internal creators retain their native implementation;
mere identity enrollment is not original behavior ownership (see the
implicit-native-creation correction below).
The native HWND registration remains borrowed and must not be unregistered
by rollback. This is a required project-binding change, not a guest change or
a change to the original editor algorithm.

The CBT enrollment installs the entrance before ordinary creation messages.
Consequently original EDIT must own its creation and destruction as a pair;
initializing the native editor first and then swapping implementations would
leave two private state owners. The native extra bytes are not an original
EDITWND overlay. Keep original EDITWND/PED storage with the already retained
window owner, and let the original terminal procedure release its text and
bookkeeping before the host-only backing is finally reclaimed.

### Combo retirement: usable dispatch edge, not yet equivalence proof

Original edecrare.c ECNcDestroyHandler frees text/bookkeeping and PED, marks
FNID_CLEANEDUP_BIT, then directly invokes ComboBoxWndProcWorker for a combo
parent. Original combo.c WM_PARENTNOTIFY consumes the child HWND and updates
its private PCBOX edit/list membership. The order and direct canonical call
are significant; ordinary SendMessage could incorrectly traverse the guest's
parent subclass during this internal retirement step.

Current wow_class_words_binding.c wow_class_call_original_native already
retains the parent's window/class association and calls the saved canonical
native procedure through CallWindowProcA, without choosing the current guest
subclass. WM_PARENTNOTIFY carries no A/W string data. This is the existing
candidate dispatch edge to reuse, rather than introducing a second procedure
registry or a PCBOX-shaped overlay on a native combo. Its existing
wow_window_note_native_procedure call must remain within the established
native cleanup ownership contract.

This source observation does not prove that a modern combo updates its private
membership identically. Grouped verification must cover editable combo child
creation, explicit child destruction, parent destruction, rejected creation,
and subsequent parent messages, including a guest parent subclass that must
not receive an extra internal notification. Do not mark that boundary closed
from the existence of the forwarding function alone. No native combo policy,
guest image, source selection or published binary was changed by this audit.

## ADAPTER-WOW-057: EDIT native dependency binding admission

Implementation may now begin with the audited native dependency sub-group,
without selecting an incomplete editor in the production graph. Its owner is
wow32-dll/source/wow_edit_native_bindings.c and its target-local header. The
seven original EDIT units and the creation/storage/dialog group remain required;
these leaf bindings alone cannot establish recovery or justify publication.

Recovery ladder: unchanged EDIT algorithms remain the selected target.
Their original private GDI/kernel dependencies cannot be linked unchanged:
CFONT/DC caches, USER server timers and class-owned CALLPROCDATA require the
host's USER/GDI implementation. Reimporting those server packages is excluded.
The smallest facade uses the actual installed implementation's named exports,
with the previously recorded source and x86 PE ABI evidence. Original
wingdip.h supplies CHWIDTHINFO and POLYPATBLT; no duplicate structures or
new glyph-metric calculation are introduced. No original file is modified by
this sub-group, so its mirror-diff budget is zero.

Six exports form one readiness group: GdiGetCharDimensions, GetCharWidthInfo,
PolyPatBlt, NtUserSetSystemTimer, NtUserKillSystemTimer and NtUserGetCPD.
Resolve only loaded system dependencies, publish the complete table once,
and retain no caller-owned window, timer or conversion state. Preserve the
caller's last-error value on successful initialization. Missing module/export
returns explicit failure without partial publication; initialization can retry.
The target is x86 only, as are WOW32 and the inspected ABI. Do not retain
addresses, ordinals or syscall numbers from the research record in code.

The timer facade retains the original four-argument call shape, but accepts
only the source-proven NULL callback; the modern service receives HWND, ID
and interval. A non-NULL callback is an explicit unsupported request, not a
silently discarded callback. The CPD facade preserves HWND/options/target
and the opaque returned DWORD; conversion dispatch and its task/native-call
scope remain with the EDIT entry facade, not this address binding.

Verification, after the complete EDIT dependency group is wired: assert x86
structure widths and call conventions; exercise readiness, native failure,
font metrics and drawing results; verify system-timer rearm/namespace/cleanup;
verify both A/W conversion directions and nested/destruction behavior as
specified above. Missing-export and unsupported callback cases must not run
an alternative algorithm. Keep the leaf source unselected until the original
consumer and lifecycle owners are ready. No passing runtime claim follows
from this admission or from successful export resolution alone.

## Original dialog EDIT allocation body admission

OPENNT-HOST-071 retains GetEditDS and ReleaseEditDS verbatim in client.c under
a separate build selector; source identity is registered in the mirror README.
The direct original consumers are dlgbegin.c's shared EDIT segment creation
and dlgmgr.c's corresponding terminal release. Their callback-only outgoing
closure is finite; unrelated client.c server/CSR entrypoints are not imported.
This is an original subset with zero algorithm edits, not an allocator rehost.

ADAPTER-WOW-056's existing retained registration table supplies a const view
to the original memory callback macros. The accessor does not register,
allocate, copy or own callbacks, nor synthesize a registration on failure.
The original caller's initialization and WOW execution scope are prerequisites;
do not call these functions from final host-storage disposal after WOW exit.
Future EDIT compilation uses the same callback table for all six Local*
operations, word-break and clipboard notification. There is no alternate
native heap fallback in this binding. The input registration already supports
native-versus-guest allocation inside the original W32 callbacks.

The subset and its bindings remain unselected until the full consumer group
is ready. Verify 256-byte GHND|GMEM_SHARE allocation, selector-shaped result,
LOWORD release, allocation failure and original dialog shared/private lifetime
with real guest callbacks; a mock allocator alone cannot close this boundary.

Implementation review: the two staged function bodies were extracted from
the pinned upstream and local client.c using their full signature-to-closing-
brace ranges and compared after CRLF normalization. Both ranges were found
and both compare equal. This is source fidelity evidence, not execution
acceptance. The separately authored wow_dialog_edit_memory_fixture.c supplies
controlled registration inputs and asserts original allocation flags/size,
selector result, LOWORD release and allocation-failure propagation. It has not
been compiled or run; neither it nor the native-leaf fixture substitutes for
the pending production consumer and real guest lifecycle group.

## EDIT carrier and final destruction requirements

OPENNT-HOST-072 stages original UNDO, ED and EDITWND declarations, including
the original anonymous UNDO member and every state flag. ICH/LPICH remain in
their original inc/user.h owner. The exact declaration-only slice and hashes
are registered in the mirror README. No reduced or newly invented editor
state is selected. This header is not yet part of a compiled EDIT consumer.

Additional source proof: original createw.c xxxFreeWindow first destroys
children and sends WM_NCDESTROY, then uses the actually exercised FNID and
FNID_CLEANEDUP_BIT to deliver WM_FINALDESTROY to an uncleaned built-in control,
even when an application subclass swallowed WM_NCDESTROY. Its terminal branch
finally marks the control cleaned. ECNcDestroyHandler and the ECNcCreate
text-allocation-failure branch also set that bit after freeing PED. A non-null
old PED slot alone therefore cannot authorize reuse or a second free.

Modern USER cannot perform this fallback for a separately owned original ED.
The recovered control's finalization must occur before the existing native
window binding detaches, use the original FNID/cleaned checks and original
terminal procedure, and retain host backing through all active borrows.
Releasing host backing is not permission to call LocalFree16 after guest task
retirement. Normal and swallowed-destroy paths, create veto/allocation failure,
and nested destruction require separate assertions in the grouped acceptance.

Existing dispatch_bound skips WOW task acquisition for untagged native
procedures. An original host EDIT entry is untagged but can invoke guest
LocalAlloc16/LocalFree16/word-break callbacks. Its entry facade must acquire
the existing WOW task/borrow scope; do not globally make every native procedure
a guest callback. The existing resource_cleanup_live window and
wow_task_resource_cleanup_enter supply the bounded terminal resource phase.
Do not enable arbitrary guest application procedures during INCLEANUP.
Original PatchThreadWindows deliberately disables client procedures during
late thread cleanup; it is not authority to postpone EDIT resource work until
that phase. The current SG_EXIT preparation occurs before GlobalFreeAll and
is the relevant admitted resource boundary.

Source comparison of the staged UNDO-through-EDITWND range against the pinned
usercli.h found both ranges and matched all9,385 normalized characters. This
includes every ED field rather than a selected-field reconstruction. The
provider generator does not yet select the header/EDIT consumer or separate
dialog EDIT branch. This comparison is not a compiler/layout or runtime pass.

## Seven-unit source recovery and concrete window field boundary

OPENNT-HOST-073 stages all seven complete C units at their original relative
paths. Initial patch application preserved normalized text but changed line
endings. A guarded formatting-only normalization first required identical
normalized content, ASCII input and uniform upstream CRLF, then restored CRLF.
All seven SHA-256 values now equal the pinned upstream values in the mirror
README. No include, conditional, algorithm or function body was edited.
New-T404S5Wow32ProviderNinja.ps1 still selects only its explicit existing
clres/cldib/client/rtlinit/ntstubs/clmenu owner list, not these new files.

Direct field/call-site inspection of the recovered files narrows the carrier
contract beyond a generic private-WND requirement:

| Access | Original sites | Binding obligation |
| --- | --- | --- |
| style and module | ECNcCreate, ECSetFont, MLCreate, SLCreate, MLEditWndProc | Retain original style/module inputs; do not overwrite recovered state from a later native snapshot without reconciling original writes. |
| control ID | ECNotifyParent and ECNcDestroyHandler | Preserve the numeric child ID, not a menu-handle surrogate. |
| parent/owner and thread identity | ECGetControlBrush | Keep popup-owner versus child-parent choice and the foreign-thread default-procedure branch; do not turn it into an unconditional cross-thread send. |
| creation/class flags | EditWndProcWorker and ValidateClassAndSize macro | Distinguish original ANSI creator, class FNID and cleaned/deleted status from modern USER private state. |
| border/read-only/password/horizontal-scroll writes | ECNcCreate, ECSetPasswordChar, EM_SETREADONLY branch, MLCreate | Four flag families, not arbitrary mutable WND support; preserve native frame consequences and guest-visible style consistency without inventing additional app notifications. |

The exact mutation inventory is WFBORDER clear for original flat-border
painting, EFREADONLY clear for pre-3.1 compatibility and set/clear for
EM_SETREADONLY, EFPASSWORD set/clear, and WFHSCROLL clear for non-left-justified
multiline controls. Blindly mapping ClearWindowState to public SetWindowLong
is not yet justified: the original private bit update is not an instruction
to deliver new WM_STYLECHANGING/WM_STYLECHANGED callbacks. A copy-only update
is also insufficient for native non-client border/scrollbar consequences.
This remains a named carrier integration obligation within the same fixed
EDIT group, not permission to import USER server or waive style semantics.

### Style-state boundary resolved to the retained native operation

Further source tracing corrects the incomplete description of the original
client operation: client.c SetWindowState/ClearWindowState first test the
copied WND bits, then call NtUserCallHwndParam with SFI_SETWINDOWSTATE or
SFI_CLEARWINDOWSTATE. Kernel random.c applies SetWF/ClrWF directly. In
contrast classchg.c xxxSetWindowStyle deliberately sends style-changing and
style-changed messages for WFWIN40COMPAT. Thus public SetWindowLong is not
the appropriate replacement for this particular internal state operation.

The same previously hashed current x86 win32u image exports
NtUserClearWindowState at RVA4E10 and NtUserSetWindowState at RVA6B30; both
stubs return ret8. Direct import-table decoding of the hashed user32 image
locates their IAT entries100EEAF4/100EEAF8 (indices213/214). The inspection
helper is build/M0-T447/S3/read-native-imports.ps1. A text-only import census
again omitted entries and was rejected; only the decoded IAT values above
were used for call-site attribution.

Native USER helpers100BCF3E and100BEE28 test a packed flag against native WND
state, then pass HWND and the unchanged flag to these respective services.
The native editor's callers use the same four packed flags as original USER:
border0E80 at1008C10F, read-only0D08 at1008C14F and10090943,
password0C20 at1008CB1A/1008CB2C, and horizontal-scroll0E10 at10097F78.
This supplies actual argument and flag evidence, not just matching exports.
NtUserAlterWindowStyle also exists, but its distinct mask/value interface is
not needed and is not selected.

Extend ADAPTER-WOW-057's readiness table from six to eight named services,
adding only these two same-shaped HWND/UINT leaves. They are void operations
at the original client boundary; do not invent a BOOL success result. The
eventual original helper retains its TestWF guard and the carrier must refresh
the changed native state through existing publication ownership. No public
SetWindowLong wrapper, private WND write, global setting or suppression of
application style messages is introduced. Grouped acceptance must prove all
four native style results and absence of additional style notifications,
alongside guest-visible state and the original border/scrollbar painting.
These static ABI findings do not themselves prove the runtime assertions.

## ADAPTER-WOW-058: original EDIT entry execution scope

The canonical exposed A/W entries will be wow_edit_window_procA/W; they call
the unchanged original EditWndProcA/W bodies, not native EDIT. Raw original
entries remain available inside the editor for its synchronous A/W conversion
branch while the outer scope is still live. Do not globally alter native
CallWindowProc or the native fast branch of dispatch_bound to acquire WOW.

Recovery ladder: original EDIT remains unchanged, but the historical USER
execution environment is absent in this host. Reuse the already selected
wow_task_callback_enter/leave and wow_user_borrow_enter/leave pair instead of
importing USER scheduling or implementing another lock. Retain the existing
window words through the entire call, including nested destruction. Entry
requires the existing worker/task and enrolled-window carrier; it must not
invent a task binding from the target HWND or copy ED state into guest pages.
An ordinary explicit CallWindowProc must not switch the current WOW task to
the window's owner: original direct-call semantics use the calling context.

Only original terminal resource messages (WM_NCDESTROY/WM_FINALDESTROY) may
use the existing resource cleanup entry while INCLEANUP is set, and only for
the current task's retained window. That existing entry additionally requires
resource_cleanup_live. This extends its named original-resource consumers
from modeless common dialogs to EDIT; it does not allow application procedures
or arbitrary messages during task teardown. Failed entry does not invoke the
editor. Unwind releases window backing while the borrow scope is still live,
then restores borrow/execution scopes, preserving the original result and
last-error; restoration failure keeps the existing terminal invariant rule.

This is an unselected entry binding until state validation and canonical
class/creation/finalization selection are connected. Acceptance must exercise
ordinary and nested A/W calls, direct versus sent calls, allocation/word-break
guest callbacks, failed acquisition, destruction in callbacks and early task
resource exit. It is not validated by the leaf native fixture and introduces
no original body diff or guest change.

### Existing-window-owned EDIT backing

Extend ADAPTER-WOW-027's existing reference-counted window words with one
optional pointer to an original EDITWND backing allocation. The original
EDITWND declaration, not a parallel reduced structure, defines its contents.
The recovered control provider allocates that host carrier and transfers it
once to an enrolled window; failed adoption leaves allocation ownership with
the caller. A retained window borrow also retains the carrier, even after
native destruction detaches the property. No new HWND map, property, class
registry, guest pointer or release callback is introduced.

This backing is distinct from PED and its original text/undo/line resources.
Those remain owned by the original allocation and terminal functions. Final
window-reference release may free the host EDITWND carrier only; it must not
call guest cleanup, free PED itself or guess which guest allocations survived.
The pending finalization integration must prove original resource cleanup
before that release. Attachment rejects a detached/terminal window and a
second carrier. Access requires the caller's existing window reference; no
raw pointer survives its final release. The existing publication lock protects
attachment/access without being held through original EDIT callbacks.

The existing wow_window_class_lifetime_fixture.c now includes supplemental
ownership assertions: null/unpublished/detached adoption rejection, unchanged
caller ownership on failure, successful adoption preserving last-error,
duplicate rejection, replacement identity isolation, and retained payload
access after nested native destruction and class retirement. Its heap DWORDs
are expressly opaque ownership tokens, not substitute EDITWND layouts or PED
allocations. It does not inspect freed memory or claim that these assertions
prove original editor resource cleanup. This fixture source has not yet been
built or run; it joins the grouped verification after production wiring.

Source review of dispatch_bound and wow_window_native_proc identifies all
three terminal retirement paths in wow_window_words_binding.c: native
dispatch finally, guest dispatch finally, and the outer denied-entry safety
path. Original editec.c routes both WM_NCDESTROY and WM_FINALDESTROY to
ECNcDestroyHandler. Therefore recovered EDIT finalization must precede
terminal property withdrawal, including when a guest subclass swallowed
WM_NCDESTROY. It must use the original cleaned-up state to avoid duplicate
resource destruction; the outer safety path is not permission to reenter a
guest after callback/resource scope acquisition failed. The host carrier's
final HeapFree is deliberately insufficient to satisfy this obligation.
This remains a production integration gap, not a completed cleanup path.

### Original control validation and lookaside recovery (OPENNT-HOST-074)

The original EditWndProcWorker begins with VALIDATECLASSANDSIZE and
INITCONTROLLOOKASIDE from usercli.h. The first rejects original cleaned-up
and deleted FNIDs before touching PED. The second initializes the original
pool, publishes PED in the original carrier, and marks cleanup before
destroying the HWND on allocation failure. Recover these exact macros rather
than adding a parallel adapter cleanup flag or a replacement allocation path.
The declaration subset now also retains original LOOKASIDE and its prototypes;
a declaration-only include branch lets the finite allocator binding avoid
pulling the complete EDIT/WND state into unrelated translation units.

Original rtlinit.c supplies AllocateFromZone/FreeToZone and all three pool
functions. Its full SHA-256 is
D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C
at the pinned OpenNT revision. Their bodies are copied unchanged to a separate
WOW_ORIGINAL_EDIT_LOOKASIDE branch of the existing mirror. The selected
RtlCaptureAnsiString branch remains untouched. A private binding supplies
only the original process-wide lock initialization, EditLookaside storage and
UserLocalAlloc/Free heap leaves. Original zone declarations come unchanged
from public/halkit/inc/nthal.h (hash in the mirror register); no kernel zone
service or USER initialization shell is imported. The pool lasts until
process exit, as original USER does; it is not the guest text heap.

The pending scoped EDIT entry now establishes that lock before original
editor entry. The supplemental wow_edit_lookaside_fixture.c exercises four
pool slots, a fifth overflow allocation, zeroing, live-pool reinitialization
and freed-slot reuse against these original functions. It remains unbuilt
and unexecuted. Allocation-failure and real control teardown coverage are
still required in the complete production group. Production selection remains
off: this import does not establish canonical class selection, state refresh,
original terminal fallback, or runtime acceptance.

### Original EDIT memory and cross-unit interfaces (OPENNT-HOST-075)

Recovered globals.h retains the six original LOCAL* macro bodies and their
instance/DS comment. Original source hash is
FD78200232574074F7EC68D2F5B851C27EE9D5024427310D18EBBCB048336E97
at the same pinned revision. wow_edit_memory_bindings.h now includes this
subset after binding the original callback names to the existing registered
table. There is no second table or native Local* fallback. In particular,
LOCALREALLOC preserves all five arguments and the optional updated-pointer
output consumed by ECLock/ECInsertText; neither the composite memory handle
nor instance value is truncated by this seam.

The usercli.h subset additionally recovers BLOCK/STRIPINFO and the complete
EC/ML/SL cross-unit declaration group, excluding the unrelated HelpMenu
declaration. Its native UserLocal* heap and retail UserGlobal* operation
macros are also retained unchanged. The two initially authored heap macros
in wow_edit_lookaside_bindings.h are removed in favor of these original
definitions; only pUserHeap is bound to GetProcessHeap. Text allocation is
still distinct from host PED/line allocations and global undo/clipboard
memory. No guest buffer mirroring or new ownership registry is introduced.

The supplemental wow_edit_memory_callbacks_fixture.c exercises all six
original macros against a test registration table: argument order, full
instance/handle preservation, realloc pointer update, null optional pointer,
success/failure return preservation and exactly twelve callback invocations.
It is unbuilt/unrun and cannot replace the real guest-memory/word-break group.
The original seven C units remain unchanged; production selection still waits
for the complete state/class/lifecycle facade.

### Original FNID transition and scoped state access (OPENNT-HOST-076)

Source: windows/core/ntuser/kernel/ntstubs.c at the pinned revision, SHA-256
A5997517B8BBB0B9E69AFC973F8BF26D8859B6FB9F4C8C1513553C52E3EE2918.
NtUserSetWindowFNID is a finite independently composable body: after HWND
validation, assign by OR only when GETFNID is zero or the requested value is
exactly FNID_CLEANEDUP_BIT. Even setting the same existing control FNID again
returns false. Retain this entire body unchanged; a new adapter policy would
unnecessarily duplicate original code. Its receive/trace shell is unavailable
without the excluded USER server, so the named shim binds that shell to
existing scoped host state. No other server entry is imported. Original
FNID_CLEANEDUP_BIT/DELETED_BIT/STATUS_BITS and GETFNID are restored in user.h.

ADAPTER-WOW-059 reuses wow_user_borrow_scope's existing retain/deduplicate
operation through wow_user_borrow_enrolled_window. It allocates no identity
table and never promotes a sampled native object into an EDIT owner. The
caller must already have established the recovered editor's task/borrow
entry and attached the initialized original carrier. wow_edit_validate_hwnd
returns that carrier until the scope leaves; missing scope/carrier fails.
The finite NtUserSetWindowLong storage leaf accepts only offset zero with
sufficient original cbwndExtra, returning the previous PED value. It does
not write native USER extra bytes, guest WND pages or unrelated window data.
This is the private storage operation required by INITCONTROLLOOKASIDE, not a
general SetWindowLong implementation. Type/cleanup policy remains original.

The supplemental wow_edit_state_fixture.c exercises the original transition,
including first assignment, rejected reclassification, same-FNID rejection,
cleanup marking/repetition, scoped lookup, PED exchange, index/size rejection
and retained backing after destruction. Its artificial pointer values are
opaque storage tokens and are never dereferenced or treated as real PEDs.
The fixture is unbuilt/unrun. Creation population, compatibility flags,
relationship refresh, canonical class selection and original final-destroy
dispatch remain pending before production graph selection. No capability
completion or new runtime image is claimed by this finite source recovery.

### Native field refresh and creation-state separation

ADAPTER-WOW-027's existing refresh operation now optionally returns its
WINDOWINFO sample after validating the same enrolled binding. Existing
callers retain their original wrapper. ADAPTER-WOW-059 uses this one sample
and the refreshed WW module/style fields to update only original rcWindow,
rcClient, style, ExStyle and hModule. It deliberately does not copy the whole
WW union: doing so would overwrite original private state/state2. FNID, PED,
expected version and creation compatibility bits remain untouched. The
supplemental state fixture now checks geometry/style/module refresh and those
preserved fields, plus refresh rejection after native destruction; it is
still unbuilt and unexecuted.

Source trace rules out several shortcut initializers. Original createw.c
transfers WS_EX_ANSICREATOR before CBT and later sets WFANSICREATOR; it is not
a synonym for arbitrary class A/W encoding. Original client/ntstubs.c obtains
dwExpWinVerAndFlags from GETEXPWINVER(hModule), not merely the current task's
version. Dialog creation passes its SCDLG_ANSI provenance explicitly for both
the root and controls. WFOLDUI comes from GACF_RANDOM3XUI, whereas WIN31/40
compatibility comes from the module version. These sources must be retained
when the carrier is initialized; current refresh supplies none by guessing.

Frame-presence flags also cannot be initialized from style alone. Original
calcclrc.c suppresses all frame flags for minimized windows, accounts for
empty client regions, and has distinct horizontal (strictly more space) and
vertical (at least enough space) scrollbar thresholds. WFFRAMEON is changed
by NC activation and the active-window paint path (dwp.c/paint.c), not by
WS_CAPTION. Preserve these as outstanding native-observation/original-logic
bindings; the new geometry refresh is not evidence that these flags are
correctly supplied. No blanket zero/default or style-only approximation is
installed, and the production editor remains unselected pending this group.

### Module-version callback binding and retained-source review

Original wow32.c registers W32GetExpWinVer in pfnGetExpWinVer. Its wcall32.c
body distinguishes native module instances by the low word, otherwise reads
the original NE mapping when available or uses RET_GETEXPWINVER. Reuse that
registered callback; do not substitute GetVersion, a hardcoded 3.1 value, the
current task's version, or a new NE parser. OPENNT-HOST-075 now also retains
the exact usercli.h GETEXPWINVER macro, including its null-module substitution
with GetModuleHandle(NULL). The private callback-name binding points to the
same registration table, and original EDIT entry readiness requires this
slot along with its memory/word-break callbacks.

The supplemental macro fixture additionally checks full supplied-module
identity, the null-module substitution and unchanged DWORD result; its call
count is now fourteen. These are mock boundary assertions, not a claim that
the real NE/guest callback has been executed by the recovered editor.

Read-only source review compared LF-normalized literal ranges against the
pinned original: usercli.h lines190--258 (control macros),301--321 (lookaside
declarations),1110 (module-version macro), rtlinit.c149--EOF (pool bodies),
and kernel/ntstubs.c6170--6188 (FNID body). All five unchanged-range containment
checks returned true. This verifies the retained source fragments only;
there is still no S3 compile/runtime result or release.

### Carrier creation from explicit original inputs

ADAPTER-WOW-059 now allocates and adopts the original EDITWND carrier using
the existing enrolled window identity. Its inputs explicitly include the
actual creation A/W choice, module-specific expected version, original task
compatibility flags and selected class's logical extra-byte count. It does
not infer these from the control name or current task version. Creation copies
the current native rectangle/style/module sample and original WOW words,
initializes original self/handle fields and leaves PED allocation to the
unchanged INITCONTROLLOOKASIDE path. Negative sizes, duplicate adoption,
missing scope/identity and allocation/publication failure do not replace an
existing carrier; an unpublished allocation is freed on adoption failure.

The ANSI/version/compatibility field projection is source-derived from
kernel/createw.c, pinned SHA-256
DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7.
Full xxxCreateWindowEx requires excluded USER server state and cannot be
imported merely to set these fields. The finite projection retains its ANSI
creator flag, GACF_RANDOM3XUI flag, LOWORD version, nested WIN31/WIN40 checks
and pre-3.1 ALWAYSSENDNCPAINT branch. Original flag definitions and byte-access
macros are retained in user.h, with byte-access macros opt-in to real original
WND layout consumers so the existing cleanup facade is not redefined.

The fixture now creates its carrier through this production-source boundary
rather than manually allocating it; source assertions cover initial ANSI/3.1
state, version truncation at the original boundary, empty PED, duplicate
refusal and error preservation. It is still unrun. Actual creation callers
must still supply their proven inputs before selection. Frame-presence and
relationship population, original OLDUI native style policy and terminal
dispatch are not supplied by this constructor and remain required work.

The original usercli.h CALLWORDBREAKPROC macro (lines609--612) is also restored
under OPENNT-HOST-075: tagged WOW procedures use the registered
pfnWowEditNextWord, native procedures retain direct invocation. No adapter
word-boundary algorithm or blanket guest callback replacement is introduced.
Both branches and retained text pointer lifetime remain grouped runtime
acceptance requirements, not proved by importing the macro.

### Actual creation encoding publication

ADAPTER-WOW-027/031/052 now retain known/unknown creation encoding with the
existing window binding. Explicit create_bound records its actual unicode
argument in its existing scoped creation record and publishes the inverse
ANSI value before returning from the matching CBT claim. Dialog root claim
likewise publishes the actual SCDLG_ANSI flag used by CreateDialogIndirectParam.
Neither path reads the current class/window procedure encoding to infer the
creator. Publication rejects retired identities and conflicting rewrites;
same-value repetition is harmless. An unknown query does not change its output.
The getter remains usable through an already retained reference after detach.

The EDIT constructor no longer accepts an independent ANSI argument: it
consumes this recorded fact and rejects unknown provenance before allocating.
The existing native creation fixture now checks A and W creation of the same
MDI class, value/error preservation and conflicting overwrite rejection. The
state fixture checks unknown-provenance rejection before successful creation.
Both are unrun; the actual production creation source is changed but not built
or published. This adds no new registry/property or guest callback during CBT.

Internal child enrollment is deliberately not labelled known yet. Original
comboini.c explicitly forwards its WFANSICREATOR into CW_FLAGS_ANSI for the
internal EDIT, while dlgbegin.c forwards SCDLG_ANSI to each template control.
Those source-defined paths still need binding; generic parent inheritance
would misclassify unrelated native creation and is not installed. This finite
remaining obligation continues to block original EDIT production selection.

### Grouped fixture assertions extended, not executed

The macro fixture now exercises both original CALLWORDBREAKPROC branches:
a real native function pointer is called directly, while a tagged opaque
Win16 procedure reaches only the registered WOW callback. Pointer, position,
length, action and return values are checked without converting text or
dereferencing the tagged procedure. The complete supplemental callback count
is sixteen; this still does not prove guest word-break reentry/lifetime.

The state fixture now adds six separate A/W-created windows across versions
0300,0309,030a,03ff,0400 and a high-word-tagged0400 value. It asserts the
original nested WIN31/WIN40 thresholds, OLDUI flag, pre-3.1-only forced NC
paint behavior, original LOWORD storage and initially empty PED/FNID. Each
case destroys its window and releases its retained scope. These are staged
source assertions, not runtime results. Current Status is consolidated to
state the remaining dependency group once rather than accumulating progress
paragraphs that might suggest completed production selection.

### Conversion transport review and bounded assertions

ADAPTER-WOW-060 is staged, not selected or executed. Source inspection of
editec.c EditWndProcWorker confirms that the first entry initializes PED
encoding from WFANSICREATOR and the opposite-encoding entry retrieves that
same PED; it must not create another editor or reinterpret the input buffer.
wow_edit_conversion_bindings.c uses native CPD conversion with the scoped
opposite-encoding entry as destination. The existing CallWindowProc bridge
checks opaque CPD identity before the WOW-procedure tag and preserves the
native-call ownership boundary. No editor message conversion algorithm is
copied into the adapter.

The staged wow_edit_conversion_fixture.c links the actual adapter against
explicit boundary doubles. It asserts both direction flags, scoped target
identity (not the unscoped original entry), unchanged message arguments,
return/error propagation, rejection before native dispatch for an unknown
function or wrong target, and CPD failure without dispatch. It is not yet
built or run. This supplemental test cannot establish native translation or
guest behavior; the previously specified real CPD message-family and nested
destruction acceptance remain required.

Lifetime review also confirms that wow_window_words_terminal removes window
identity before final reference release. Consequently original EDIT terminal
fallback must execute before that removal, within the live task/borrow scope;
freeing the host EDITWND allocation alone does not release original PED or
guest LOCAL memory. This remains an integration prerequisite, not a verified
cleanup capability or permission to move guest callbacks into HeapFree.

### Terminal-owner source trace: normal, failed-create and task exit

Read-only trace compared original kernel/createw.c lines1872--1915 with
retained client/editec.c EditWndProcWorker/EditWndProc and edecrare.c
ECNcCreate/ECNcDestroyHandler, then followed the current project's
destroy_terminal_windows, wow_window_words_prepare_exit and
wow_window_words_terminal. No runtime experiment or guest change was made.

| Path | Original contract | Required integration boundary |
| --- | --- | --- |
| Normal WM_NCDESTROY forwarded | ECNcDestroyHandler frees text/undo/tabs/lines/widths/PED, sets CLEANEDUP, then notifies the combo parent. | Keep task, HWND identity and relationship borrows live through the complete handler, including parent notification. |
| Application subclass swallows destruction | xxxFreeWindow sends WM_NCDESTROY first, then dispatches WM_FINALDESTROY to the canonical FNID control target if not cleaned. | Finalize after application dispatch, before removing the window property; never send the fallback through the application subclass again. |
| LOCALALLOC fails during creation | ECNcCreate frees PED and sets CLEANEDUP but need not clear the PED slot. | Trust original FNID validation; never infer liveness from a nonzero PED or unconditionally free it from the binding destructor. |
| Task exit | Original late cleanup suppresses client fallback under TIF_INCLEANUP. Current SG_EXIT preparation deliberately precedes guest GlobalFreeAll and exposes resource_cleanup_live. | Recovered EDIT cleanup must use this existing early-resource phase; late task retirement cannot call guest LocalFree or supply the missing cleanup. |

The concrete task-exit selection gap is now located: current
wow_window_words_prepare_exit installs cleanup.native_cleanup (or native
DefWindowProcA), clears the application callback and marks native dispatch.
That is appropriate for the currently native-owned controls but insufficient
for a recovered original EDIT: native USER has no knowledge of the added PED.
Canonical recovered-target selection must therefore cover exit preparation,
not only creation and CallWindowProc. Keep the existing prepare-all-then-
destroy ordering so that destruction of a parent cannot enter an unprepared
child application's procedure. Do not replace this with a separate unordered
loop freeing each PED.

The entry wrapper's finally block only releases its borrowed reference; its
comment now states this explicitly. It does not implement final destruction.
No finalizer was added to the last-reference HeapFree path, no second cleanup
registry was introduced, and no partial runtime selection is claimed. The
normal/subclass/failure/early-exit paths above are one required integration
group, with actual original cleanup and guest-memory acceptance still open.

The next implementation adds wow_edit_finalize_window to the existing
ADAPTER-WOW-059 state boundary, without a new registry or mirror change. The
caller must retain task/borrow scope and HWND identity. Untouched zero-FNID
state is a no-op; cleaned EDIT state is a no-op even with a nonzero stale PED;
wrong/deleted-but-not-cleaned state fails. Otherwise the canonical scoped W
entry receives original WM_FINALDESTROY. Success requires original CLEANEDUP,
since the control returns zero for both successful cleanup and rejection.
No application subclass is invoked, no PED is freed by the adapter, and the
scoped entry still rejects guest access outside early-resource permission.

The supplemental state fixture now stages boundary-double assertions for
untouched, wrong-class, deleted, poisoned-but-cleaned, rejected canonical
dispatch, successful canonical dispatch and duplicate finalization. The
double sets only the original FNID marker: it does not prove resource release,
combo notification or real terminal wiring. These cases have not run. Actual
terminal owners must still call the helper before property removal and select
the recovered canonical cleanup target during prepare-all task exit; the
production graph remains unchanged until the full dependency group is ready.

### Terminal owner wiring, still unbuilt

wow_window_words_terminal now invokes the scoped EDIT finalizer for an
enrolled carrier before taking the exclusive publication lock and withdrawing
identity. It rechecks enrollment afterward because original combo notification
can nest destruction. Failure uses the existing fail-fast boundary rather than
retiring an unclean editor and returning to guest execution. No callback runs
under the publication lock. Already-retired collection does not reacquire an
EDIT through a removed HWND property.

wow_window_words_prepare_exit now chooses the original scoped ANSI EDIT
entry for an attached EDIT carrier; other controls retain their established
native cleanup target. This preserves the lifecycle owner's existing
prepare-all/destroy sequence and ANSI forwarding entry. The common scoped
entry supports finalization as an internal operation: it establishes the
existing normal or early-resource task permission and borrow scope once,
then calls the canonical original W body without recursive wrapper entry.
The state fixture's explicit boundary double follows that original body ABI.

This is source wiring, not a running feature. The selected window-binding
translation unit now requires the pending EDIT entry/state/original closure
at link time; generator selection and standalone fixture link inputs must be
updated with the complete group before building. Do not attempt a partial
provider build or claim the current source is already link-complete. Native
detach-only storage fixtures still do not exercise terminal dispatch; actual
normal destruction, swallowed WM_NCDESTROY, nested combo notification,
failed creation and SG_EXIT/GlobalFreeAll ordering remain runtime gates.

### One logical EDIT class target, not a parallel selection table

Source trace confirms wow_class_client_query_native already replaces the
native query's procedure with wow_class_words_target. Original walias.c
GetStdClassWndProc populates its existing stdClasses cache from that query.
Therefore no new GetClassInfo hook or second standard-class cache is needed.

The existing native-class import now selects wow_edit_window_procA for the
original FNID_EDIT record while preserving the borrowed native registration
and original native procedure as metadata. Enroll-created-window no longer
overwrites either recovered A/W entry with the modern per-window procedure.
The native-procedure matcher recognizes the same recovered A/W entries and
returns the canonical recovered ANSI cleanup target. Selection uses existing
FNID/procedure identity, never an executable or class-name special case.

This wiring does not prove creation completeness. Internal creator encoding,
original module-version capture timing, class extra-byte metadata and parent
relationships must be established before the first original entry. In
particular, querying the version lazily inside EditWndProc would introduce
a guest callback at a different point than original client/ntstubs.c's
pre-create GETEXPWINVER; no such lazy callback was added. Native registration
must not be allowed to initialize a second native EDIT before the recovered
entry runs. The current unbuilt source requires the full EDIT link closure;
the published package remains unchanged and no acceptance result is claimed.

### Explicit recovered-EDIT creation state wired

For an explicit create whose resolved logical target is a recovered EDIT A/W
entry, create_bound now captures module version via the existing class
context's original registered callback before installing its CBT scope or
calling native CreateWindowEx. It preserves GETEXPWINVER's null-module
substitution and does not substitute current-task version. Class extra bytes
come from the selected native registration; compatibility flags come from
the current original task carrier. The class reference is released on a
callback exception as well as ordinary failures.

claim_explicit records the actual requested A/W encoding, establishes a
bounded borrow scope and constructs EDITWND before forwarding CBT or allowing
WM_NCCREATE. Failure rolls back the existing enrollment; no partially built
state is accepted. No PED/text allocation occurs in this constructor: original
EditWndProcWorker still owns that work.

This covers resolved direct EDIT targets only. It does not yet cover an
application subclass's first call to the original EDIT, template/internal
children, or the complete original client validation order: current class
resolution precedes the newly captured version, whereas original
_CreateWindowEx queries version before server class resolution. That ordering
must be reconciled with the common creation contract before acceptance, not
hidden by claiming all creation paths are connected. Original relationship
and frame initialization remain outstanding. No build or runtime test ran.

### Product creation version ordering corrected

The product PFNCSCREATEWINDOWEX entry now obtains the registered module
version before class-string capture and atom/class resolution, for every
product creation request. It passes that captured value synchronously to
create_bound; the recovered EDIT branch does not query twice. No TLS cache
or new callback table is introduced. The low-level native A/W fixture gateway
retains a direct-EDIT fallback query because it accepts an already resolved
atom rather than the product's original input. Thus the preceding paragraph's
product-ordering defect is corrected in source, while internal/template and
subclass coverage remains open. Runtime order/exception acceptance is pending.

Full-function recovery was checked, not assumed unavailable. Pinned original
client/ntstubs.c SHA256
7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01
contains _CreateWindowEx at lines1362--1526. Its MDI branch invokes
CreateMDIChild and MDICompleteChildCreation, not just native create. Original
client/mdiwin.c SHA256
77471975010484FC6935B91A56CF7450225D44402F6D9E3709FBBDFF1C8A706E
shows CreateMDIChild reading PMDIWND.pmdi, active/maximized child state,
cascade geometry, child-ID counts and system-menu state. Those are not the
current native MDI client's public storage. Importing that whole branch
while still passing WS_EX_MDICHILD to native CreateWindowEx would duplicate
MDI policy; returning fabricated private MDI state is not a valid facade.
The original large-string capture/init helpers are also absent from the
currently selected rtlinit slice. Full _CreateWindowEx is therefore not
declared composable by this audit and is not imported opportunistically.
Its exact pre-create version callback boundary is retained through the
existing bounded adapter; no second MDI implementation was introduced.

### Explicit subclass entry reuses captured creation facts

The existing window-words record now retains a once-published creation tuple
(module version, compatibility flags, class extra bytes). Actual A/W origin
remains in its existing field. All explicit product creates capture the tuple,
not only those initially selecting EDIT. Publication rejects conflicting
replacement; a read of unknown facts fails without modifying the output.
There is no extra HWND map, guest allocation or cross-task cache.

After establishing the task/borrow scope, the recovered EDIT entry now ensures
its carrier from this captured tuple if an application subclass is the first
caller. Already attached state is reused. It performs no guest version query
and does not substitute current task compatibility or current procedure
encoding. Original VALIDATECLASSANDSIZE/INITCONTROLLOOKASIDE remain responsible
for class validation and PED allocation. Missing internal/template provenance
is still an explicit failure, not inferred from a parent.

The unrun state fixture now includes unknown-output preservation, duplicate
publication, conflicting replacement, initialization from captured facts and
reuse of the same carrier. This is supplemental storage/selection coverage,
not real subclass or guest acceptance. Frame/relationship state and internal
creation sources remain required before selecting and verifying the group.

### Dialog producer/retirement audit: shared EDIT segment is dialog-owned

Read-only source inputs from the pinned OpenNT revision:
client/dlgbegin.c SHA256
C3DF77A2987E97DF8872C3DAB8F6E7C10A08850C5EBCB439C0EC1DC88B0CDC44;
client/dlgmgr.c SHA256
8E2383F52E4E3F65F656BD492143D7A8ED5D0A2526AD8E6AF10D242465FF8D7A.
The inspected owners are InternalCreateDialog (lines176--951) and the
dialog procedure's WM_NCDESTROY/WM_FINALDESTROY cleanup (lines668--675).

InternalCreateDialog sets CD_GLOBALEDIT only when DS_LOCALEDIT is absent.
For a WOW template EDIT, it lazily calls GetEditDS once into PDLG.hData,
uses that composite handle as hmodCreate and obtains that module's version.
Each subsequent template EDIT reuses the same hData. DS_LOCALEDIT and non-EDIT
controls keep the normal module. The template loop also passes the actual
SCDLG_ANSI creation flag, original converted create-parameter value, and
precomputed version to NtUserCreateWindowEx. Allocation or child creation
failure follows the original DFNOFAILCREATE policy, not unconditional dialog
success or unconditional failure.

Dialog cleanup releases hData once and clears it. Original child destruction
precedes parent NC destruction, so EDIT LOCALFREE must complete before this
segment release. In the current adapter release_creation only frees a host
record and can run after the last borrow; putting ReleaseEditDS there would
repeat the wrong late-resource pattern already excluded for PED. The proper
boundary is terminal dialog processing while normal/early-resource permission
and guest memory are still live, before its window/property ownership ends.

Current wow_dialog_creation stores root ANSI provenance but no hData owner or
template-child creation transaction. Native CreateDialogIndirectParam alone
does not connect the retained original GetEditDS/ReleaseEditDS functions.
Using current_creation or an enrolled parent as proof that any CBT child is
the next template child is insufficient under reentrant application creation.
No such heuristic is installed. Producer selection must distinguish original
template creation from nested explicit creation and preserve the original
shared-segment/module/version tuple as one transaction.

The complete InternalCreateDialog is not merely a template iterator: it also
owns PDLG initialization, WOWDlgInit timing, font ownership, default/focus
selection, failed-control policy and revalidation. Importing that body while
leaving a second native template creator active would duplicate control
creation and initialization. Its full dependency/ownership adaptation remains
part of the admitted dialog group; this audit does not admit a second parser,
fabricated PDLG overlay, or successful EDIT fallback to native memory. No
guest changes, runtime test or publication resulted from this source audit.

### OPENNT-HOST-077: retain original EDIT rectangle/frame helpers

Original editsl.c calls CopyInflateRect and DrawFrame; editml.c also calls
DrawFrame. Recovered true subsets at their original rtl/rect.c and rtl/draw.c
paths retain those complete function bodies. Full-file SHA256 values and
admission are in src/opennt-host/README.md. Original inc/user.h DrawFrame
command declarations are retained too (source SHA256
ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1).
The only function-file changes are the subset/provenance header and private
precomp inclusion. Read-only normalized source-body comparison reports both
bodies present and identical; this is static provenance evidence, not a test
of execution or pixels.

Outgoing closure is finite: border metrics, original system/gray brushes,
POLYPATBLT declarations and UserPolyPatBlt. Original userrtl.h maps the latter
to PolyPatBlt in the client build; ADAPTER-WOW-057 already supplies that leaf.
No replacement FrameRect/border algorithm or unrelated RTL drawing function
was added. Production metric/brush/precomp selection remains pending with
the complete EDIT group.

The unrun wow_edit_drawing_fixture.c and its isolated edit-drawing/precomp.h
stage assertions against these actual original bodies: in-place rectangle
inflation, unequal X/Y border sizes, all four ordered frame rectangles,
system versus gray brush, PATCOPY/PATINVERT and preservation of original
DrawFrame's unconditional TRUE after a failed PolyPatBlt. The boundary double
does not render pixels and cannot establish native GDI or guest acceptance.
Neither source unit is yet selected by the production generator.

### Dependency sweep and OPENNT-HOST-078 client-view semantics

A read-only lexical sweep of the seven EDIT units removed comments/strings
before enumerating call-like identifiers, then traced private names to their
definitions. The inventory is a source-audit aid, not a compiler-resolved
call graph: conditional LPK names, macros and same-unit definitions still
require explicit classification. In particular commented UpdateWindow31 and
LocalReallocSafe are not live missing dependencies.

The sweep identified independently composable original _IsWindowVisible,
_GetClientRect, _FChildVisible and FChildVisible. OPENNT-HOST-078 retains
their complete bodies in original rtl/wow.c, rtl/winmgr.c and client/winmgrc.c;
source hashes and finite outgoing closure are registered in the mirror README.
Original visibility/type flags and TestwndChild are confined to the original
client-view declaration branch, not the separate cleanup facade.

These are not interchangeable aliases for modern public APIs: _IsWindowVisible
returns TRUE for null and walks to the desktop; _FChildVisible checks parents,
not self, and tolerates a missing parent; _GetClientRect returns minimized
metrics for pre-Win4 windows but actual client extents for Win4-compatible
windows. The host consumer needs the native RECT branch, not the source's
separate WOW WORD-output branch. The unrun wow_edit_view_fixture.c exercises
these distinctions, validation failure and extents above65535 against actual
original bodies using isolated test-owned WNDs. It does not prove production
relationship borrowing or native visibility sampling.

FillWindow remains a separately identified native leaf candidate: original
client/ntstubs.c delegates to NtUserFillWindow(hwndBrush, hwndPaint, hdc, hbr).
Installed x86 win32u SHA256
31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C
exports NtUserFillWindow at RVA19A0; dumpbin disassembly ends its syscall stub
with ret10h, consistent with four x86 stack arguments. This alone is not a
complete argument/behavior proof; native caller and CTLCOLOR reentry review
remain required before adding it to the eight-leaf binding. No guessed FillRect
substitution, compiler trial or runtime test was used to fill that gap.

### ADAPTER-WOW-061: FillWindow native call and current-task version

The previous native-leaf candidate audit is superseded by source-wired
ADAPTER-WOW-061, still excluded from the production graph. This is an EDIT
dependency binding, not a new renderer, message owner or USER server import.

Source inputs at the pinned OpenNT revision:

- client/ntstubs.c SHA256
  7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01:
  FillWindow forwards the four arguments after native-DC fixup/null rejection.
  inc/ntsend.h MSGERROR/ERRORTRAP explains its zero result without adding a
  new last-error code on this null path.
- kernel/paint.c SHA256
  15C5B8F566E6AE40D9E1F69424134508A5CA380615D22C7D2743D16D89A4D50E:
  xxxFillWindow chooses paint as the absent brush-window fallback, obtains
  the clip, then xxxPaintRect temporarily sets the relative brush origin,
  resolves small CTLCOLOR values through a callback, fills and restores origin.
- kernel/random.c SHA256
  B3ED1082041C1C600C92487B45FF0C4750A50456E5BA7EA14A3EC8EF83A1F9CD:
  xxxGetControlColor uses native default handling for another process and for
  an invalid returned brush. Its actual comparison is PpiCurrent, despite
  the surrounding comment mentioning threads. Do not replace original
  editec.c ECGetControlBrush with this service: that client optimization has
  its own actual thread comparison and remains in the original editor.

Read-only PE inspection reconfirms installed x86 USER SHA256
56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E and
win32u SHA256 31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C.
USER IAT slot100EEBB0 is NtUserFillWindow. MSVC14.43.34808 Hostx64/x86
dumpbin /disasm shows four pushed arguments at calls1002A61C,100925BF and
1009BEC3. The middle caller chooses small color values1/6 before passing DC
and two window values; these are compatible with the original EDIT caller.
The syscall's ret10h corroborates the four-slot ABI, not semantic equivalence
or universal Windows-version availability. Native behavior remains an explicit
group acceptance gate; absent exports fail readiness rather than loading an
alternate DLL or substituting a local paint algorithm.

wow_edit_call_bindings.c wraps the installed service in the existing
wow_user_native_call_begin/end boundary, targeting the potential color
receiver. Finally restores ownership on return and exception; bookkeeping
does not replace the operation's last error. No native or guest identity is
retained beyond the call. The existing restoration owner remains responsible
for its terminal failure contract. No mirror painting body was changed.

Supplemental tests are now staged, not executed:

- wow_edit_native_fixture.c adds an invisible child and memory DIB; checks
  actual CTLCOLOR receiver/paint/DC, parent-relative origin and restoration,
  clip-preserved pixels, direct brush without callback, null brush-window
  fallback, empty-clip success without callback and resource release.
- wow_edit_call_fixture.c links the real adapter with transport doubles;
  checks normal, nested, rejected-entry, failed-native, rejected-restoration
  and exception-unwind paths, paired scopes and last-error preservation.
  It does not prove the runtime scheduler, guest callbacks or native pixels.

The dependency review also separates two original version contracts.
client/usercli.h GetAppVer reads GetClientInfo()->dwExpWinVer, whereas
GETEXPWINVER calls the registered module-specific callback. The former is
used by ECGetBrush, multiline editing and rare font handling; replacing it
with window/module version would change original compatibility decisions.
Original kernel/queue.c xxxInitTask already initializes the selected task's
pClientInfo->dwExpWinVer (upstream SHA256
0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8).
The existing lifecycle owns that record. OPENNT-HOST-075 now retains the
unchanged GetAppVer macro from its already registered usercli.h source;
ADAPTER-WOW-061 supplies only a scoped const pointer to the existing record.
No new record, delayed guest callback, version truncation or fallback value
is introduced. Canonical EDIT entry rejects missing client-info before entry.
The call fixture checks record identity, changed values and missing-state
rejection; the memory fixture checks that original GetAppVer does not invoke
GETEXPWINVER's callback. These are likewise unrun.

Review status: code/source review only; no compiler or runtime test was run,
no image/ZIP was changed and no S3 publication is claimed. Full private
precomp selection, native creation provenance for internal/template controls,
shared dialog DS ownership, WND relationship/frame state and combo cleanup
remain required before the grouped build and real guest acceptance.

### Original dialog creation and modal lifetime group (079--080)

Question: can the current native dialog carrier supply the registered WOW
callbacks and original shared EDIT data segment, and can only its modal
wrapper be replaced? Source review answers both in the negative. This is a
source-proven integration gap, not attribution of a new observed app failure.

Pinned OpenNT inputs (revision5c5b979ec08c17d3ca2eb70e8aad62d26515d01c):

| Original input | SHA256 | Retained responsibility |
| --- | --- | --- |
| client/clres.c | C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5 | Template extent and modal entry |
| client/dlgbegin.c | C3DF77A2987E97DF8872C3DAB8F6E7C10A08850C5EBCB439C0EC1DC88B0CDC44 | Creator, W32InitDlg timing, children and shared DS; not imported by080 |
| client/dlgmgr.c | 8E2383F52E4E3F65F656BD492143D7A8ED5D0A2526AD8E6AF10D242465FF8D7A | DialogBox2 and InternalDialogBox retained by080; default handling/validation remain group dependencies |
| client/dlgend.c | 191C9CE7DCE323873F5EF40F74141950940F6723E14D48B5FA09CA5517DDE6CD | EndDialog, complete body retained |
| inc/user.h | ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1 | Original DLG/DIALOG/PDLG/DLGF_ANSI declarations |

Installed x86 USER and win32u identities are the same as in061 above.
Read-only MSVC dumpbin inspection found UserRegisterWowHandlers at USER
RVA C1CE0 returning C0000022 then ret8. This installed export refuses the
registration; exporting its name does not provide the original callbacks.
The native CreateDialogIndirectParamAorW entry at RVA30EC0 masks flags down
to its client/ANSI path; it is not proof of the original16-bit creator.
NtUserSetDialogPointer's existence likewise cannot license installing an
NT4 PDLG in a modern private structure. No such call is introduced.

The actual selected caller is WU32DialogBoxParam in mvdm/wow32/wudlg.c.
After its original ConvertDialog16, the modal branch calls
DialogBoxIndirectParamAorW; modeless calls pfnOut.pfnServerCreateDialog.
ADAPTER-WIN32-051 currently takes the former to wow_title_modal, which adds
style scope and calls native DialogBoxIndirectParamA/W. The latter reaches
wow_user_server_create_dialog and its CBT/return hooks, which publish WW
and call original W32InitDlg after WM_CREATE. Modal creation does not take
that carrier. Original W32DialogFunc requires the WW and vpfnDlgProc set by
the original initialization. Neither the callback's existence nor a native
dialog window proves this prerequisite.

Recovery decision: select the original template/creator/default/modal/end
group together. Native USER remains the window and input service, not a
second authority for the recovered dialog's PDLG. One existing retained
window identity must own original dialog state and its lifetime. Native
private extra bytes cannot store it; no second HWND registry, independent
dialog scheduler or replacement template parser is admitted. The staged
original extent walker is not a bounds checker for untrusted arbitrary
memory: its caller supplies the already converted/owned resource.

Finite boundary and grouped implementation obligations:

| Boundary | Required owner/binding | Why the tempting substitution is rejected |
| --- | --- | --- |
| Create/modeless/modal | Original InternalCreateDialog for both entrances, W32InitDlg before children, original creation flags and module/parameter provenance | Adding a second modal hook does not recover shared DS or original child creation |
| Dialog state/validation | Original DLG/DIALOG, ValidateDialogPwnd, shared lookaside and existing retained window identity; superclass transition included | Native PDLG layout is unavailable; separate modal flags would disagree with default processing and EndDialog |
| Child EDIT storage | Original GetEditDS/ReleaseEditDS through registered guest callbacks; one hData per non-DS_LOCALEDIT dialog, freed after child cleanup | Neither parent module inheritance nor an independent host text heap has the original contract |
| Modal operation | Original InternalDialogBox and DialogBox2; existing scoped native message/callback boundaries | Native DialogBox bypasses the recovered creator/state; do not write another loop |
| End/default/return | Original EndDialog and default procedure must read the same PDLG/resultWP; original MapDialogRect and dialog navigation consume it too | Native EndDialog cannot end a loop watching a different PDLG |
| Object references | Existing retained window/borrow ownership through original ThreadLock/Unlock shape, refresh after native mutation and callback | HWND existence alone neither retains private state nor protects nested destruction |
| Foreground restoration | Original queue predicates NtUserQueryWindow(WindowIsForegroundThread) and NtUserGetThreadState(UserThreadStateIsForeground), requiring verified host bindings | Comparing foreground TID to current TID loses attached-input queue semantics; old enum values are not assumed modern ABI |
| Active/focus reset | Original EndDialog's ZapActiveAndFocus leaf, requiring verified host ABI | Original source explicitly says SetActiveWindow(NULL) plus SetFocus(NULL) is not equivalent |
| Native effects | Existing scoped enable/focus/show/position/destroy/message leaves; preserve result/error and callback ownership | Calling raw native operations outside the execution boundary loses WOW execution ownership |

OPENNT-HOST-080 stages the complete176-line DialogBox2,107-line
InternalDialogBox and complete EndDialog bodies, plus original foreground
predicate macros and dialog declarations. Only crop/precomp/selection
annotations are new. No original control flow is replaced and no USER
server is imported. The unit selections and private dependency environment
are still pending; no native function ordinal/enum is guessed to make them
link. ADAPTER-WIN32-051 and the displaced hook-based creation policy must
be removed when the whole original group becomes selectable, not retained
as a fallback. Current group is not yet usable as a replacement.

Review of079 compares all three imported bodies against upstream after
newline normalization: SkipIDorString10 lines, GetSizeDialogTemplate115,
DialogBoxIndirectParamAorW31, all equal. The supplemental authored template
fixture covers standard/extended headers, font and child alignment, version
dependent invalid styles and modal flag/result forwarding; it is unrun and
uses a manager double, so cannot prove modal or guest behavior.

Acceptance for the whole dialog group must include modal/modeless and
DS_LOCALEDIT/shared DS creation, null/invalid/disabled/child owners,
initialization-triggered EndDialog and DestroyWindow, failed child creation,
nested guest callbacks, message-loop WM_QUIT and idle behavior, superclassed
dialogs, child-before-DS cleanup and normal task retirement. Verify the
selected original callers and actual guest return/object results; a source
comparison or mock loop alone does not close these rows. No compile, test,
guest change, ZIP change or publication was performed for this staging.

080 review compares the three imported bodies against their pinned source
after newline normalization: DialogBox2(176), InternalDialogBox(107) and
EndDialog(153) all match. The mirror crop adds no alternate modal algorithm.
The supplemental wow_dialog_lifetime_fixture.c links those original bodies
with explicit state/native/creator doubles. It covers previously enabled and
disabled owners, child-owner normalization with paired locks, invalid-owner
zero versus creation-failure minus-one, idle-triggered ending, WM_QUIT repost,
initialization-triggered ending, focus/owner restoration, ownerless zap and
rejected non-dialog handles. Sequence assertions distinguish hide, wake and
actual destruction. Its minimal WND is expressly test-only, not a proposed
production layout; queue selector constants are test-local rather than guessed
modern ABI. This fixture is staged and unrun. Real modal dispatch/navigation,
guest callbacks, native active/focus and resource lifetime remain group gates.
git diff --check passed for tracked modifications (line-ending warnings only);
this is a whitespace check, not build or runtime acceptance.

### Creator/default-procedure closure (081)

The full dlgbegin.c read completes the source trace through resource/font
selection, root creation, WOWDlgInit, child creation, initialization and
failure. OPENNT-HOST-081 stages that complete original client unit, not an
adapter parser. Its required dlgmgr.c default group is retained under
WOW_ORIGINAL_DIALOG_MANAGER: GetParentDialog, xxxSaveDlgFocus,
xxxRestoreDlgFocus, DlgSetFocus, ValidateDialogPwnd, DefDlgProcWorker,
DefDlgProcA/W, RepositionRect and MapDialogRect. The modal-only fixture
does not select this group. Other numeric/check-button convenience entry
points are excluded from this dependency slice.

Original ownership/order to preserve:

- The creator initializes PDLG, locks the window and calls WOWDlgInit before
  children. W32InitDlg publishes the guest dialog procedure into WW and returns
  the application's WM_INITDIALOG parameter instead of the internal DLGDATA.
- A WOW dialog without DS_LOCALEDIT allocates one shared hData at its first
  qualifying EDIT and reuses it. Child instance/version and converted guest
  creation-data values must not be inferred from parent identity. Retain
  DFNOFAILCREATE and original rollback.
- Initialization may end or destroy the dialog. Revalidation, app-version and
  SCDLG_NOREVALIDATE branches cannot become one blanket success test.
- DefDlgProcWorker clears resultWP, enters the callback and revalidates.
  Documented direct-return messages differ from other handled messages, which
  return resultWP. WM_FINALDESTROY skips the application, releases shared DS,
  deletes the user font, frees the original lookaside DLG and clears its pointer.
  Its final return checks WFDIALOGWINDOW before accessing freed state.
- Focus, default-button, DS_CONTROL parent traversal, font metrics and
  MapDialogRect must consume the same state, not native USER's separate PDLG.

Two concrete remaining binding obligations were identified by source rather
than trial runs. NtUserCreateWindowEx receives counted Unicode class/text and
a separate WS_EX_ANSICREATOR flag. The existing wow_user_create_window_ex
carrier is ANSI-only and takes NUL-terminated strings. Passing LARGE_STRING
pointers to it or silently round-tripping through ANSI is rejected. Native
marshalling remains a group gate; no private ABI is guessed. In addition,
wow_user_get/set_window_longA currently forwards ordinary nonnegative indices
to native USER. Recovered dialog resultWP/procedure/user words must instead
reach the state used by DefDlgProc, while unrecovered native dialogs retain
native behavior. This is owner-scoped routing, not global interception.

The finite outgoing closure remains existing menu ownership, registered WOW
memory/superclass callbacks, original navigation/default-button source,
scoped native USER calls, GDI font/DC services, the already audited
GdiGetCharDimensions leaf, and current window/task observation/lifetime.
No USER server, global dialog registry, new scheduler or guest build enters.
The complete native creation/default replacement is not selectable until
these consumers agree on state and teardown.

An actual binding change accompanies staging: wow_dialog_creation_binding
calls original WOWDlgInit instead of naming W32InitDlg directly. The original
dispatcher is retained in client.c, upstream SHA256
787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472.
ADAPTER-WOW-056 maps pfnInitDlgCallback and pfnWowSetFakeDialogClass to the
existing registered input table. Original wow32.c assigns W32InitDlg and
SetFakeDialogClass there; the existing bridge copies that input. There is no
second callback table or selection policy. This adds a pending production
link dependency; the checkout is not link-complete.

The unrun dialog-memory fixture now checks original WOWDlgInit's registered
target, full-width argument/result forwarding, zero parameter/result and no
implicit allocation. It does not prove actual WW publication or guest entry.
Sixteen bodies were compared after newline normalization against pinned source:
five in dlgbegin.c, ten in dlgmgr.c and WOWDlgInit; all equal. No build/runtime
test, release, commit or push was performed. This is not S3 acceptance.

#### Native creation ABI review, not yet an admitted call

Rehashed the installed x86 USER/win32u files; both match the identities above.
Read-only dumpbin14.43.34809 (MSVC14.43.34808 tool directory) /exports and
/disasm /range inspection establishes a material ABI difference:

- win32u NtUserCreateWindowEx is RVA1880 and ends at188C with ret44h:
  seventeen x86 argument slots, not the fourteen in original dlgbegin.c.
- USER CreateWindowExW at RVA2E930 calls its helper at2E5B0. That helper
  separately handles class/text encodings and creator flags; the ANSI branch
  ORs80000000 into ex-style at2E791. It calls the counted-string helper2DF70.
- USER's IAT slot100EE9E0 resolves NtUserCreateWindowEx. At2E182--2E1D9 the
  helper pushes seventeen arguments. Original/selected class values, text,
  ordinary creation fields and additional context/flag inputs are distinct.
  The surrounding helper also has class-redirection/context handling.

This proves that a fourteen-slot cast is invalid, not that all seventeen
modern arguments have been semantically verified. The extra slots, counted
string layout, class resolution and activation-context ownership require
complete mapping before a native binding. Do not call internal code addresses,
copy the helper, use old enum values speculatively or substitute a lossy ANSI
round trip. Existing callback/window publication must surround the eventual
native operation. No runtime call or test was made during this PE inspection.

#### Resolved-atom counted creation and shared enrollment

Further inspection of the same pinned native images narrows the preceding
unresolved ABI issue to the selected resolved-atom form. USER helper2DF70
skips class redirection when version flags lack40000000: the selected class
remains the input class and the context local remains zero. Its native call
at2E1D9 uses ex-style, input class, selected class, counted title, style,
x/y/width/height, parent, menu, module, parameter, optional slot14, version,
optional slot16 and context slot17. The native dialog creator itself calls
this helper at2FD70 and30206; both pass zero for slots14/16. Creator ANSI
flags are separate from title encoding. This admits neither a general SxS
resolver nor private-code-address calls. Other Windows ABI versions remain
unproved; export availability alone is not acceptance.

OPENNT-HOST-082 retains the original12-byte LARGE_STRING family in its own
declaration guard. ADAPTER-WOW-062 stages the seventeen-slot x86 leaf using
the existing resolved class atom twice, original counted title, zero optional
slots, and LOWORD(version) as original createw stores in WND. Full version
and compatibility facts remain in the existing HWND binding. No native WOW
pointer, syscall number, imported kernel implementation or guest edit is used.

wow_user_create_dialog_window_ex retains the original fourteen-argument
caller shape. It resolves captured Unicode class text or an atom through the
existing class owner, copies only the three original WOW DWORDs when present,
and invokes the existing create_bound lifetime. CBT registration, callback
publication, native-call begin/end and failure retirement are not duplicated.
Review removed an unnecessary requirement for a get_version callback when
the original dialog caller already supplied its captured version; the normal
uncaptured EDIT path still requires that callback. This is an adapter change,
not a new mirror algorithm diff.

Unrun acceptance sources:

- wow_dialog_native_creation_fixture.c exercises the real native leaf with
  independent ANSI-creator/title-encoding combinations, non-NUL counted text,
  WM_NOTIFYFORMAT, parameter delivery, veto and terminal destruction.
- wow_native_creation_binding_fixture.c adds four real enrollment cases:
  class text versus atom, each with ANSI versus Unicode creator. They assert
  exact title length (the backing buffer has extra characters), preserved
  full version/compatibility/WOW words, creation encoding, terminal ownership
  removal and an empty object table after the group. No version-query callback
  is installed for this captured-version path.

These fixtures do not yet prove behavior. Production selection, original
dialog private-precomp/state/default-result integration, grouped compilation
and actual guest acceptance remain outstanding. Every existing fixture that
links the creation binding must also link its new native leaf. No S3 build,
runtime test, release, commit or push is claimed here.

#### Dialog pointer and window-long ownership review

Source tracing identifies three distinct original owners, not one generic
native extra-byte operation:

- client/ntstubs.c::_SetWindowLong owns DWL_DLGPROC in PDLG.lpfnDlg,
  process access validation, CPD conversion and DLGF_ANSI updates. Its
  non-dialog continuation also preserves initialized listbox owner-draw style
  bits; importing only the procedure assignment would discard this contract.
- rtl/getset.c::_GetWindowLong returns PDLG.lpfnDlg with matching CPD rules,
  DIALOG.resultWP for DWL_MSGRESULT and DIALOG.unused for DWL_USER. Other
  private dialog offsets fail, rather than exposing the internal PDLG pointer
  occupying offset4 in original DIALOG storage.
- kernel/getset.c::xxxSetWindowLong owns the resultWP/unused writes and old
  value return. DefDlgProcWorker clears resultWP before callback, revalidates
  after callback, and uses its documented direct-return message cases.

The current wow_user_private_access.c forwards nonnegative indices to native
USER. That is valid only for still-native dialogs; it cannot remain the owner
for recovered dialogs. The complete replacement must dispatch according to
the existing retained window's recovered state, not the numeric class name,
and route original client versus server responsibilities separately. A
three-offset native forwarding shortcut or a second independent PDLG table
would break this contract. This review changes the next implementation
boundary: shared retained WND/DIALOG storage precedes window-long routing;
do not attach original PDLG to modern USER's private state.

OPENNT-HOST-083 retains the complete unchanged SetDialogPointer body from
kernel/miscutil.c (provenance/hash in src/opennt-host/README.md). Its only
outgoing operations are local PDLG assignment, FNID and WFDIALOGWINDOW state
updates. Original ValidateDialogPwnd and final default-dialog cleanup are its
direct callers. Queue/desktop/shell routines in the same upstream file are
rejected as unrelated. No guest or modern private-memory layout is involved.

The existing unrun dialog-lifetime fixture now includes publication, clearing,
repeated clearing, existing-FNID preservation and unrelated result/user-field
preservation cases for this actual original function. Its WND is explicitly
a state double, not proof of production ABI or native dialog behavior. Add
the retained miscutil slice to that fixture's eventual grouped link. The
production state carrier, terminal cleanup and window-long integration remain
open; no runtime acceptance or delivery is claimed.

#### Class-extra storage extent correction

Original kernel/createw.c allocates sizeof(WND) + pcls->cbwndExtra before
assigning that exact extent to the new window. Review found that the staged
ADAPTER-WOW-059 carrier instead allocated sizeof(EDITWND) while recording an
arbitrary nonnegative captured class_extra_bytes. This would leave a
superclass's declared extra storage physically absent. It is a staged
adapter defect, not an observed published-product or original-code failure.

wow_edit_create_state now allocates the complete declared extent in the same
existing retained allocation. Its minimum EDITWND capacity keeps current
pre-validation PED reads safe without enlarging logical cbwndExtra; original
class validation remains authoritative. On the x86 target the nonnegative
int extent plus176-byte WND cannot overflow SIZE_T. No second allocation,
HWND identity, guest pointer or mirror algorithm change is introduced.

The unrun wow_edit_state_fixture now registers64 extra bytes, captures that
extent, verifies physical capacity before dereferencing it, checks the
extension is initially zero and preserves a final-byte marker through native
geometry refresh. All existing version/encoding/compatibility and cleanup
checks remain. This does not itself connect DIALOG: common-view access,
original callback/result routing and canonical dialog finalization remain
required before production selection and whole-group acceptance.

#### One retained client-state carrier, not parallel EDIT/dialog objects

ADAPTER-WOW-063 moves creation/provenance projection, ensure, validate and
native refresh into wow_window_client_state_bindings.c. The previous four
EDIT-specific function names and implementations are removed; creation and
EDIT entry consumers now call the shared functions. The original control
entry still owns PED and cleanup. No original algorithm is replaced by this
move and no guest layout changes.

wow_window_words_binding holds one original WND/class-extra pointer instead
of an EDIT-specific pointer. Attach still transfers one allocation only on
success, rejects duplicate/retired enrollment, and last existing HWND borrow
still frees it. No second map, property, retained identity or PDLG allocation
owner was added. Source selection must link the new shared implementation;
the production graph is still pending the entire control/dialog group.

Both terminal fallback and task-exit preparation now select recovered EDIT
by GETFNID(state)==FNID_EDIT, not by storage-pointer presence. A common WND
with zero FNID is not an initialized editor. Recovered-dialog canonical
dispatch must be connected before any dialog owner is production-selected.
The window/class lifetime fixture now allocates actual WND capacity with
zero FNID rather than a four-byte opaque payload, retaining all prior
duplicate-attachment, failed-transfer, nested-destroy and last-borrow checks.
The state fixture follows the new API and retains its complete extra-extent
checks. These source changes are unrun; no build/runtime/release result is
claimed and no native dialog fallback is declared replaced yet.

#### Original dialog publication on the shared carrier

wow_dialog_state_bindings.c now supplies the finite HWND-to-retained-WND
boundary for original SetDialogPointer. It uses the existing synchronous
borrow, rejects insufficient declared class-extra capacity, and invokes the
unchanged original body. The original caller retains PDLG allocation and
resource ownership. The binding does not allocate PDLG, create a property,
change native USER dialog state, infer a class or reimplement FNID transitions.
Successful publication preserves the incoming last-error value.

OPENNT-HOST-084 retains WFDIALOGWINDOW=0201 from the pinned user.h as an
original state-byte selector. It is not a public ex-style or a native service
number. Compile-time layout assertions pin the three DIALOG prefix offsets.
The independently authored native state fixture uses its existing64-byte
class-extra carrier to check failed-capacity non-mutation, same-WND identity,
PDLG publication, FNID/dialog-flag establishment, clearing/cleanup status and
preservation of resultWP/user/trailing extra bytes. It links the real original
SetDialogPointer; the modal control-flow fixture keeps its explicitly separate
test-local flag representation. Neither fixture has run.

This connects storage publication only. Original ValidateDialogPwnd/default
entry still needs its full precomp/registration closure, and window-long A/W
CPD/result ownership is still required. Do not select or publish the partial
dialog group, or claim the native creator fallback replaced, on this evidence.

#### Dialog getter reuse and native CallProc boundary

OPENNT-HOST-085 retains complete _GetWindowLong, unchanged, from original
windows/core/ntuser/rtl/getset.c, SHA256
4A2CB2067F5DD776676D7F808F40F6468082DB018E16BCF9A77DAF00BDDB34C9,
at the previously pinned OpenNT revision. CPD_DIALOG=0040 is retained from
the same original user.h declaration owner. Other getset.c functions are
not imported. This keeps process privacy, nullable procedure behavior, A/W
CPD selection, original allocation-failure return, result/user slots and
private-offset rejection together. The full function's non-dialog branches
also require class private-size metadata and negative-index forwarding;
they are not silently declared wired by the dialog-only fixture.

The new unrun wow_dialog_getset_fixture links this actual function with
transport doubles and actual original WND/DIALOG declarations. It checks
same/opposite encoding, both conversion directions, absent procedure, CPD
allocation failure, denied foreign-process procedure read, permitted result
read, private/overrun indices, negative-index delegation and no mutation.
It does not prove native CPD validation or actual Win16 execution.

Read-only current USER32 evidence (SHA256 rechecked as
56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E):
dumpbin14.43.34809 /exports plus /disasm /range of RVA7E340,7D767,260A0
and23D70 show CallWindowProcA entering helper7D767. FFFF-tagged inputs call
validator260A0 with type7, not original TYPE_CALLPROC8. The validator checks
entry type/status/uniqueness and conditionally maps through native USER.
The unconverted-message branch reads a64-bit previous-procedure value and
passes it through helper23D70; conversion-message dispatch instead retains
the opaque token. These addresses are evidence only, never callable inputs.
There is no exported general CPD decoder in the inspected USER export set.

Consequences: do not cast an opaque native token to original CALLPROCDATA,
copy old type numbers into native validation, or convert Unicode through an
ANSI forwarding thunk merely to avoid resolving original procedure encoding.
The existing native CallWindowProc forwarding used for WND subclass storage
does not by itself implement original PDLG get/set identity semantics.
Native CPD projection/ownership remains an explicit S3 dependency before
selecting original dialog get/set. No new runtime resolver, source policy
waiver, test pass or publication is claimed by this static investigation.

#### Getter ownership selected; setter/conversion still required

The actual wow_user_get_window_longA caller now asks the dialog-state leaf
whether an already recovered dialog owns a nonnegative index. That leaf
calls original _GetWindowLong; it does not duplicate result/user/procedure
cases. A handled zero/error is terminal for this read and never falls through
to native USER. A missing recovered state, non-dialog state or negative index
preserves last-error and leaves the existing native/WW/WNDPROC owner intact.
No class-name heuristic, allocation, promotion or second registry is used.

The getter fixture now links that actual selector as well as original getset
and miscutil bodies. Its existing WND-lookup transport double checks zero/error
versus unhandled, absent native ownership, negative-index exclusion and
last-error preservation. It is still unrun and does not prove production
borrow/guest/CPD behavior. The edit-state fixture also gains this original
getter link dependency through the dialog-state translation unit.

Further native read-only mapping identifies USER IAT100EF1EC as exported
NtUserMapDesktopObject and100EF1F0 as NtUserValidateHandleSecure. The native
SetWindowLongA helper27590 uses validator260A0 with type7 for a CPD, maps
the64-bit previous procedure through23D70, then reads its transition flags
at offset18. This confirms why an NT4 pointer cast or generic CallWindowProcA
forwarder is not a decoded PDLG procedure/encoding pair. It does not yet prove
that the mapping export alone validates type, process, uniqueness and lifetime;
no raw native layout reader has been implemented on that assumption.

The getter change adds unresolved production links until the private-precomp
and native CPD boundary are wired. Setter and getter must be selected together;
do not build/publish this staged checkout as a coherent product or claim the
dialog replacement complete. Existing release images remain unchanged.

#### Original paired setter chain retained and connected in source

OPENNT-HOST-086 adds complete unchanged _SetWindowLong to client/ntstubs.c
under WOW_ORIGINAL_DIALOG_GETSET; the existing GetClassWOWWords selector
remains its other branch. It also retains complete xxxSetWindowLong in
kernel/getset.c. Exact pinned hashes are in the mirror registry. The complete
CALLPROCDATA/tag and SCROLLPOS/LBIV/LBWND declarations are retained from their
original owners: no fabricated prefix is used to compile the client's
initialized-listbox style protection. WFDESTROYED retains its original0380
state-byte selector. This admits declarations and these finite functions,
not the remainder of USER server, a listbox engine or native CPD overlays.

wow_user_set_window_longA now selects the same already-recovered dialog owner
as its getter. The dialog leaf calls original _SetWindowLong; its private
NtUserSetWindowLong continuation resolves the retained HWND to WND and calls
original xxxSetWindowLong. Result/user storage, previous-value return,
private-offset errors, caller checks and CPD normalization are not copied
into adapter switches. Native or negative-index cases keep their prior owner.
Handled zero/error cannot fall through to native USER.

The unrun dialog-getset fixture now links both original setter bodies and
the actual shared-state continuation, reusing original ntsend call macros.
It checks result/user round trips, denied caller non-mutation, same/opposite
procedure encoding, old-procedure CPD return, incoming CPD normalization,
foreign-process procedure denial and native-unhandled last-error preservation.
Its CPD validator remains explicitly a transport double; it does not prove
modern handle decoding. The preserved listbox guard is not covered by these
dialog cases and must not be advertised as listbox acceptance.

The actual production private-precomp, caller/type/CPD validation bindings
and complete original dialog lifecycle are still required. Added links remain
unselected/unverified; no compiler, test, package publication or guest change
was performed for this source group. P16 remains the delivered baseline.

#### CPD mapping export is not a complete resolver

Additional read-only disassembly of the same pinned USER image identifies
the handle-entry helper4100D: the exported gSharedInfo at RVA E49D0 supplies
the information pointer at+0, entry-array pointer at+8 and stride at+10.
The helper bounds the index using the shared count and rejects free type0.
Its caller260A0 separately checks type7, destroyed bit and handle uniqueness.
Thus NtUserMapDesktopObject alone cannot replace the complete validation.

The preceding-procedure conversion helper23D70 also compares the64-bit value
against two24-entry native server-procedure arrays before selecting a client
stub from private USER arrays (branches750E1/750FE). Plain truncation of the
native previous-procedure field is not a valid general resolver. Resolving
these private code/data addresses by fixed RVA is rejected; evidence addresses
are not runtime dependencies. A type-safe mapping/projection and server/client
normalization remain required before using native CPDs in original PDLG.
No new reader, pointer cast, temporary window or identity table was added.

The original paired get/set fixture now also covers the previously preserved
but uncovered client listbox style guard: initialized local, uninitialized
local, foreign-process and absent-private-state cases reach the actual client
and server functions, while a negative-style native transport double records
the final value. This is source-contract coverage, not listbox functionality
or guest acceptance, and remains unrun with the rest of the group.

#### Actual WOW callers and CPD lifetime audit

Question: can the dialog resolver be omitted because WOW always installs
W32DialogFunc, or replaced with a window-local procedure cache?

Read-only source inspection of wuwind.c WU32GetWindowLong (DWL_DLGPROC,
lines1665--1695), WU32SetWindowLong (lines2535--2584), wsubcls.c
GetThunkWindowProc/IsThunkWindowProc and wumsg.c WU32CallWindowProc rejects
both the blanket fixed-entry assumption and treating every returned value as
a callable pointer. The getter returns the guest procedure when present;
otherwise a known/faked dialog's host procedure is wrapped by the original
guest thunk. That thunk stores lpfn32 unchanged. The setter recognizes it
and passes the stored value back to SetWindowLong, instead of installing
W32DialogFunc. Calls through that thunk reach original message conversion
and CallWindowProc. This is a source-supported save/restore route, not proof
that a particular application currently exercises it.

The existing class resolver is not an available provider: production
wow_user_task_lifecycle.c explicitly assigns classes.resolve_callproc=NULL,
and wow_class_client_publish rejects a tagged procedure in that state.
Fixture-supplied resolvers cannot establish production support. Native class
imports have a separate opaque-token forwarding path in
wow_class_words_import_native; that path is not CPD decoding for PDLG or
RegisterClass. Preserve this distinction when completing the shared seam.

The pinned OpenNT kernel/getset.c FindPCPD/GetCPD shows that CPD_WND and
CPD_DIALOG both select the window's CLASS as list owner. Reuse requires both
the previous procedure and the full WORD option value to match. Allocation
uses HMAllocObject with the current thread/desktop; the object is linked by
USER locks and returned through MAKE_CPDHANDLE. kernel/class.c DestroyClass
releases the class CPD chain with UnlockAndFreeCPDs, respecting already
marked destruction. Therefore a cache freed at each window's NCDESTROY
would have the wrong lifetime even if a one-window fixture passed.

Exact inspected upstream hashes (same pinned revision as this ledger):

- kernel/getset.c: F0D22E4285E8B33B873854913A11587C67D78E50E1F5DB28EC89EF8B6FF06D3D.
- kernel/class.c: 82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A.
- client/clmsg.c: 2F87967A05E730E2C95B8CE0B92443D323F76BAEC97446D3C4F8C9DC163710FD.

Procedure: rg for GetCPD, TYPE_CALLPROC and UnlockAndFreeCPDs in the pinned
ntuser tree; read complete GetCPD and CallWindowProcAorW bodies, the class
destruction call site, and the actual WOW caller branches above; verify file
hashes with Get-FileHash. Confidence is high for these source contracts;
no native lifetime or guest runtime result is inferred.

Disposition: do not import a second USER handle manager merely to reuse
GetCPD's list search. Original CallWindowProcAorW itself sends conversion
messages through CsSendMessage with FNID_CALLWINDOWPROC, so a private cloned
token is not automatically usable by modern USER's converter. Native token
creation/calling remains the existing finite candidate; validated procedure
and encoding recovery remains unresolved. The next resolver review must
cover saved-token restoration, class lifetime across two windows, opposite
encoding and invalid/deleted tokens together. No new runtime table, mirror
import, test pass, build or publication is introduced by this audit.

#### Canonical dialog final destruction before identity retirement

ADAPTER-WOW-063 now stages wow_dialog_finalization_binding.c and selects it
from wow_window_words_terminal for retained FNID_DIALOG state. Original
createw.c xxxFreeWindow sends WM_NCDESTROY then uses the canonical FNID owner
for unfinished control cleanup; dlgmgr.c DefDlgProcWorker's WM_FINALDESTROY
branches directly to DoCleanup without entering lpfnDlg. The adapter calls
that unchanged worker, not native USER's unrelated dialog storage. Original
ReleaseEditDS, DeleteObject, FreeLookasideEntry and SetDialogPointer retain
their resource ordering. No mirror body or guest changes are needed.

The boundary retains the existing HWND reference and task/borrow scopes.
During task cleanup it uses the existing early-resource permission and checks
the owning task, as the staged EDIT boundary does; no guest application
procedure is dispatched. This supplies the unavailable client-side lifetime
edge, not a replacement cleanup scheduler. No publication lock spans the
original worker. Completed cleanup is idempotent; absent/uninitialized PDLG,
wrong class extent or deleted state cannot trigger allocation. Success checks
the original cleared PDLG/dialog flag and CLEANEDUP bit, not worker return0.

tests/adapter-mvdm-host-out/wow/wow_dialog_finalization_fixture.c links the
actual boundary with explicit worker/scope doubles. Cases cover success,
repeated cleanup, unsuccessful worker, borrow failure, uninitialized state,
short extra storage and task-cleanup owner isolation. It is staged and unrun;
it does not prove original DS/font release, native destruction, reentry or
guest acceptance. Original default dispatch, early-exit canonical selection,
private-precomp and group build selection remain required. Existing P16
release files are unchanged; this is source wiring, not a delivered repair.

The following staged caller wiring extends that same entry, without copying
DefDlgProcWorker policy: wow_dialog_window_procA/W acquire the existing task
and HWND borrow, ensure only the retained host carrier, then call the original
worker with the real A/W input. Original ValidateDialogPwnd still owns PDLG
promotion/allocation and fake-dialog notification. FNID_DIALOG native-class
imports now publish this canonical callable target beside the existing EDIT
selection. The explicit WOW DefDlgProcA boundary chooses it for retained
dialog state; untouched native dialogs keep the previous scoped modern USER
call. A handled zero/error cannot retry against native PDLG. Cleanup-time
normal entry is rejected; only the bounded finalizer has resource permission.

The staged fixture adds both encodings, handled-zero selection, non-dialog
native-unhandled result/error preservation, and rejection of ordinary calls
during cleanup. The same worker double is explicit; these are unrun boundary
cases, not proof of original default-message behavior. Early-exit canonical
selection and removal of the transitional native creator remain prerequisites
for full group selection. No new mirror diff or guest mutation is introduced.

#### Early-exit dialog dispatch and resource ordering

Read-only review of pinned kernel/createw.c PatchThreadWindows
(lines2533--2598; SHA256
DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7)
confirms that FNID_DIALOG=02A2 lies outside FNID_WNDPROCEND=029E. Original
thread cleanup therefore installs DefWindowProc, not DefDlgProc, for it.
Clearing the application dialog procedure was not found in that function;
inventing such a write would not be the original mechanism.

The existing prepare-all pass now selects DefWindowProcA for a recovered
FNID_DIALOG rather than its previously observed native cleanup procedure.
It still completes for every task-owned window before any destruction.
Native USER retains child destruction order. At terminal withdrawal the
already staged resource-scoped finalizer invokes original WM_FINALDESTROY,
which skips lpfnDlg and releases shared EDIT DS only after the children have
been destroyed. Do not run this finalizer in the prepare-all pass: doing so
would release a dialog data segment while child editors still use it.
This extends ADAPTER-WOW-063, with zero additional original-body changes.

The existing wow_user_task_dialog_lifecycle_fixture gains a live-dialog task
exit case. It requires actual retained PDLG before exit, retains the existing
window binding across the exit, checks no application terminal callback,
native window/table retirement, and original cleared PDLG/dialog flag plus
CLEANEDUP status afterward. Native page-domain doubles remain explicit.
This case is unrun and intentionally cannot pass through the transitional
native creator alone. It does not claim guest shared-DS proof; that and the
complete creator/private-precomp integration remain required group gates.

#### Original creator entry replaces the native approximation

The owner-required replacement is now made in source: ADAPTER-WOW-063's thin
wow_user_server_create_dialog delegates the exact seven arguments to retained
InternalCreateDialog. Task and borrow scope acquisition/restoration surround
the original client algorithm; it is not wrapped in one native-call handoff
that would release ownership while original code accesses its WND/PDLG.
Each native creation still uses the separately audited counted-string gateway
and existing explicit creation enrollment. The original body owns template,
font/menu, PDLG-before-children, WOWDlgInit, initialization/revalidation and
rollback. Its native/private dependencies remain to be selected and verified.

Removed from the product source: ADAPTER-WOW-052's per-dialog creation heap
record/TLS stack/property, separate CBT and CALLWNDPROCRET hooks, initialization
compensation, dialog-procedure forwarding and separate rollback/refcount owner.
Also removed the claim/cancel/prepare-exit APIs and dialog_root discriminator
from common enrollment. No empty compatibility stubs or native creator fallback
remain. The standard task CBT observer and explicit HWND enrollment are not
removed: they serve original root/child creation and other controls.

Source review specifically confirms original WOWDlgInit is conditional on
LOWORD(hmod), unlike the removed unconditional compensation. Existing native
fixtures must supply the intended original module identity/callback setup;
do not change original creation to satisfy the old approximation's assertions.
The actual guest producer remains the authority for the WOW module contract.

The new wow_dialog_creation_entry_fixture.c links the thin entry with explicit
creator/task/borrow doubles and asserts seven-argument identity for A/W,
success/failure last-error, exception unwind, scope-entry failure, cleanup and
unknown-flag rejection. It is unrun and proves no dialog algorithm or guest
capability. Full creator/default/modal/control link and real original-lifecycle
acceptance remain mandatory. This checkout is not a publishable partial build;
P16 assets remain untouched. No mirror body or guest was modified.

#### Modal and EndDialog caller replacement

Actual WU32DialogBoxParam in wudlg.c has two branches: modeless uses the
registered seven-argument creator; modal uses DialogBoxIndirectParamAorW.
WU32EndDialog separately calls EndDialog. Wiring only the registered creator
therefore does not restore the modal path. The previous public facade and
wow_title_modal substituted modern DialogBoxIndirectParamA/W and reduced flags
to an ANSI boolean. Those native modal calls are now removed from the product
modal helper. The provider redirect names the style facade separately from
the unchanged original clres.c symbol; complete flags reach the original
template-size/flag-selection entry and InternalDialogBox. Style policy stays
outside original code, with existing task/borrow scopes around the client
algorithm rather than a native-call handoff around original state access.

EndDialog now has a provider redirect and scoped recovered-state selection.
Recovered dialogs call retained dlgend.c (private-precomp name
wow_original_EndDialog); a false result is terminal, not a reason to retry
against native USER's PDLG. Untouched native dialogs retain native EndDialog
under the existing native-call boundary. The original fEnd/result/hide/post
algorithm is not copied into the adapter. The private-precomp alias and full
modal group link remain unresolved build work, not a selected capability.

The finalization/entry fixture adds original-end selection, negative result
forwarding, handled failure without native retry and native boundary failure.
Its scope/original-end providers are explicitly doubles. The existing native
task/style integration fixture now passes real SCDLG flags and ends dialogs
through the selected entry. Both remain unrun; original modal/end control-flow
fixtures and real guest modal acceptance are still mandatory. No guest or
original function body changed, and no package was built or published.

#### Remove the autonomous EDIT private-word setter

Dependency review found wow_edit_set_window_long directly overwrote PED after
checking only offset/capacity. Retained kernel/getset.c xxxSetWindowLong already
owns the actual rule: in a control's private extent, index0 is writable only
when the old word is zero or WFDESTROYED is set, subject to FCallerOk. Therefore
the adapter's unconditional nonzero replacement and its old positive test
were not faithful to the original owner.

That autonomous setter is deleted, not retained as fallback. The dialog-only
receive helper is renamed/moved to wow_window_client_access_bindings.c as
wow_window_client_set_window_long; it merely validates the retained HWND and
calls original xxxSetWindowLong. Original EDIT INITCONTROLLOOKASIDE and dialog
client _SetWindowLong share this receive edge through private bindings. No
new field-write switch, original-body change or guest change is introduced.

The edit-state fixture now expects a live nonzero overwrite to fail and sets
the original destruction flag explicitly for its synthetic cleanup case.
The dialog-getset fixture uses actual retained EDITWND and original server
body to cover first publication, denied overwrite/private offset, caller
denial and allowed destruction clear. It links the actual common receive edge;
FCallerOk/CBFNID remain explicit fixture transports, not production providers.
These fixtures are unrun. Production caller validation/control-size bindings
remain necessary before whole-group build/acceptance; no passing result or
publication is claimed from this correction.

#### Local ownership and canonical private extents

Read-only source tracing distinguishes two different original checks:
kernel/getset.c FCallerOk rejects ordinary callers modifying system/CSRSS or
winlogon windows; client/winmgrc.c TestWindowProcess permits the same thread
or compares the window process with the current process. They are not general
synonyms. The recovered state domain is narrower: wow_window_words_acquire
calls native GetWindowLongA(GWL_WNDPROC) before reading its property, and
wow_window_client_validate_hwnd requires that existing binding's allocation.
The new wow_window_client_owns_state leaf additionally compares the exact
allocation with the supplied WND. It rejects copied/sampled state and retired
windows, preserves last-error, and introduces neither privileges nor a new
registry. Mapping the original checks is legal only inside this local domain;
external public operations must retain their native access path. Production
private-precomp mapping remains pending, not silently replaced by TRUE.

The existing state fixture now covers live identity, a copied WND retaining
the same HWND, NULL, post-destruction rejection and last-error preservation.
It is unrun and does not replace cross-process runtime acceptance.

Canonical private extent provenance is client/clinit.c's standard-class table
and kernel/class.c's assignment of CBFNID from that registration. EDIT uses
CBEDITEXTRA; dialog uses DLGWINDOWEXTRA; listbox and combolist use
sizeof(LBWND)-sizeof(WND). A superclass's captured cbwndExtra remains its
allocation capacity, not the canonical CBFNID private boundary. Using that
capacity as CBFNID would incorrectly deny access to application extra words.
The complete canonical table/provider and build wiring remain group work;
no guest changes, compilation, runtime pass or publication occurred here.

Follow-up source resolution: original usercli.h line571 defines CBEDITEXTRA
as6, while the x86 EDITWND stores a four-byte PED. The previous fixture's
sizeof(EDITWND) CBFNID return was incorrect; this supersedes the earlier
claim that the structure alone represented the exact private extent.
OPENNT-HOST-074 now retains the original constant byte-for-byte as a
declaration, with no original algorithm changes. The fixture reserves the
actual class-extra capacity, returns sizeof(WND)+CBEDITEXTRA for canonical
EDIT, verifies read/write denial at offsets4/5 and permits an appended DWORD
at offset6. That permits application superclass storage without exposing the
two reserved bytes. The supplemental case remains unrun. Canonical table and
production private-precomp composition remain incomplete; this correction is
source/fixture progress, not restored runtime capability.

#### Canonical dialog identity through subclass and module retirement

Production caller tracing found a staged integration mismatch independent of
the extent table: wow_class_words_import_native selected the recovered ANSI
dialog entry, but wow_class_words_match_native recognized recovered EDIT only.
wow_window_note_native_procedure (called by the existing CallWindowProc bridge)
could therefore fail to record the called dialog owner. The matcher now uses
the existing FNID_DIALOG record to recognize its A/W recovered entries and
return the canonical ANSI target. No name heuristic or new identity list is
introduced; other native classes retain their existing lookup.

Original kernel/cleanup.c restores a dialog class through
gpsi->apfnClientW.pfnDialogWndProc on guest-module unload. The staged shared
table still supplied the generic native-class forwarder there, which would
restore the modern dialog procedure after creating recovered PDLG state.
That one slot now names wow_dialog_window_procW, the scoped original worker.
Original cleanup.c is unchanged; EDIT's distinct DefWindowProc restoration
and untouched native-control entries remain unchanged.

The existing standard-class fixture is updated to assert recovered initial
EDIT/dialog targets and original cleanup's recovered dialog target. It adds
A/W canonical matching, wrong-encoding rejection, unchanged output on rejection
and last-error preservation. No original control body or guest changed.
These checks are staged and unrun with the same complete S3 build group;
this does not claim restored execution or a new publication.

#### EndDialog queue reset: named modern export found

Original kernel/miscutil.c ZapActiveAndFocus reads the current input queue
and Unlocks spwndActive/spwndFocus. Original client/dlgend.c explicitly rejects
the SetActiveWindow(NULL)/SetFocus(NULL) pair as an equivalent operation.
Read-only PE inspection now resolves the previously open native ABI leaf:
SysWOW64/win32u.dll exports NtUserZapActiveAndFocus. Its x86 stub at RVA6EE0
ends in a plain RET, with no argument stack adjustment; current USER32 imports
the named function at IAT100EEBB8 and calls it at10089569 with no pushed
arguments, immediately before posting the dialog wake-up message. Addresses
are inspection evidence only, never runtime bindings. The export is resolved
with GetProcAddress on already loaded win32u, with no private-address call or
copied syscall/NT4 selector number.

Inspected hashes (SHA-256): win32u
31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C;
user32 56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E.
Commands used MSVC14.43.34808 Hostx64/x86/dumpbin.exe /exports, /headers,
/disasm /range:0x10006EE0,0x10006EF0 for win32u, and the retained read-only
build/M0-T447/S3/read-native-imports.ps1 -NamePattern ZapActiveAndFocus plus
USER32 /disasm:nobytes /range:0x10089500,0x10089588. These inspect files;
they do not invoke the service or establish runtime behavior.

ADAPTER-WOW-062 now resolves creation and queue-reset leaves atomically.
wow_dialog_native_zap_active_and_focus invokes the exact no-argument export;
its BOOL distinguishes missing dependency from invocation, not an invented
native result. No fallback changes notification semantics. The existing native
creation fixture adds active/focus setup on its own hidden window, checks both
identities cleared without activation/focus notifications, and repeats the
clear. This is staged, unrun coverage. The original EndDialog private-precomp
mapping and existing task/native-call boundary remain required with the full
group. No original function body or guest was modified, and nothing was built
or published in this step.

The matching call boundary is now staged in wow_dialog_call_bindings.c.
Original EndDialog has already marked fEnd/result before its queue-reset
operation and ignores the original VOID result. Consequently a dependency or
scope failure must not silently return as if reset succeeded. The binding uses
the existing wow_user_native_call_begin/end with a NULL target (current queue),
restores it in __finally on both normal and exception paths, and uses the
existing worker fail-fast owner for failed entry, unavailable service or failed
restoration. No new scheduler, queue state or exception recovery policy is
introduced. Native exceptions continue unwinding after successful restoration.

tests/adapter-mvdm-host-out/wow/wow_dialog_call_fixture.c links the actual
boundary and explicit scope/service doubles: success/error preservation,
rejected entry without native call, missing service with balanced scope,
native exception unwind, failed restoration and a subsequent successful call.
It is unrun, and it does not prove real queue behavior; the separate native
fixture owns that assertion. EndDialog's final private-precomp mapping and the
complete group build remain pending. No guest or original algorithm changed.

#### Terminal recovered-state destruction bit

Read-only createw.c tracing shows failure cleanup at871 and ordinary destroy
at1590 set WFDESTROYED before xxxFreeWindow; child destruction has the same
precondition at1763. The latter owner sends NCDESTROY and then the canonical
WM_FINALDESTROY. Recovered client state was initialized with zero flags and
its refresh deliberately preserved private bits, but no terminal path set
WFDESTROYED. Original xxxSetWindowLong therefore could reject cleanup writes
that require destruction permission.

The existing wow_window_words_terminal owner now sets that bit on enrolled
retained client state before selecting/invoking the canonical finalizer. Its
short publication section is exclusive for the state mutation and still ends
before any original/guest callback. No additional allocation, flag table,
message hook or mirror-body change is introduced. This fixes the final-release
precondition only: it is not evidence that parent/child ordering or the earlier
application NCDESTROY observation is fully restored. Those remain admitted
lifecycle obligations, including real rollback/ordinary exit/task-exit checks.
The group is still unbuilt/unrun; no publication or runtime success is claimed.

#### Shared callback refresh updates recovered client WND

Production caller search found wow_window_client_refresh was used only by
supplemental fixtures; callback entry and borrowed WW reads instead invoked
wow_window_words_refresh. The latter updated WW style/module but left the
new recovered client WND at its creation geometry/style. The client helper's
separate field copies were therefore not effective callback integration.

The existing wow_window_words_refresh_info transaction now applies its one
WINDOWINFO/module sample to both WW and an already attached client WND while
holding the existing publication lock. Only rcWindow/rcClient/style/ExStyle/
hModule change; private state/state2/FNID/extra words remain original-owned.
The duplicate field-copy implementation is removed from the client helper,
which delegates to this existing owner and preserves entry last-error.
The state fixture now invokes the actual callback refresh API for its moved
window and retained-private-data checks. It remains unrun. This establishes
source wiring for callback entry, not proof of the complete native-return
graph refresh or relationship lifecycle; those remain S3 group obligations.
No extra native sampling loop, guest write or original-body diff is added.

Native-return source tracing follows publish_call_windows into
wow_user_window_refresh_payload/project_graph, not the WW callback refresh.
After that existing graph transaction succeeds, its same-process rows now
update already enrolled local storage through wow_window_words_refresh_observed.
The common apply_window_info leaf is shared with direct callback refresh:
rectangles/styles come from the validated row, module from native USER,
and property identity is checked under the publication lock. No geometry
resampling or new enumeration is added. External rows are skipped; a valid
local window without the property is an explicit no-op, not enrollment.

If an enrolled update fails, project_graph returns failure to its existing
native-return/callback owner, which must not resume guest execution on a
reported stale-state failure. The guest projection may already have committed;
this is not advertised as rollback of that earlier transaction. The existing
fail-fast return contract remains in force. Client WND relationship pointers
are not supplied by this change and remain pending group integration.

The state fixture now exercises the observed-row entry, moved geometry,
private-state preservation, invalid input, last-error and no enrollment of
an untouched native window. It remains unrun, and does not substitute for
real native-return/modal/guest acceptance. This step changes no original body
or immutable guest; publication and compilation remain pending.

### Shared control allocator initialization before dialog entry

Source review of pinned OpenNT client/clinit.c:88 establishes that original
USER initializes gcsLookaside before control dispatch. Original rtlinit.c
InitLookaside/AllocLookasideEntry/FreeLookasideEntry all use that shared lock;
dlgmgr.c:312 owns DialogLookaside, with allocation at428/434 and release at698
in the staged unchanged bodies. The staged adapter previously called
wow_edit_lookaside_ready only from EDIT entry. Dialog-first execution therefore
had no established lock-initialization prerequisite. This is a source-proven
staged integration gap, not a diagnosis of a published P16 runtime failure.

The existing once-only initializer is now required by modeless creator,
original modal entry and dialog default/finalization entry. Failure returns
before original execution and before task/borrow scope acquisition; the
default entry also avoids taking a window reference on this failure.
DialogLookaside remains defined only by original dlgmgr.c. No allocator,
pool, thread or mirror-body change is added. The initialization keeps its
existing successful last-error preservation and process lifetime.

Supplemental creation-entry and finalization fixtures inject initialization
failure and assert no original dispatch, allocation/state promotion or leaked
scope/reference. The creation fixture also checks initialization precedes
original dispatch. These sources remain unbuilt and unrun, and do not prove
the real modal allocator or guest path. The entire selected group must link
the existing lookaside provider and original rtlinit bodies before unified
verification; production selection is still incomplete. Read-only source
search and diff review are the evidence for this step, not runtime acceptance.

### Production binding-group build selection

New-T404S5Wow32ProviderNinja.ps1 previously selected existing call sites but
none of the twelve staged dialog/EDIT/shared-state implementation units.
Those units now join providerSupportSources, using the existing per-source
existence check, x86 /Gz /MT compilation rule and provider support-object list.
No fixture implementation or comparison-tree path enters this selection.

rtlinit.c has mutually exclusive capture and lookaside subsets. The generator
retains the existing capture object and adds obj/opennt-user/control-lookaside.obj
with WOW_ORIGINAL_EDIT_LOOKASIDE; it does not replace capture flags or alias
two outputs to one object path. This links the existing lock owner with the
original allocation bodies once the whole group is complete.

This is intermediate build wiring only. Original EDIT/dialog/getset source
selection, private headers and remaining consumer bindings are not complete;
the generated DLL is not yet linkable or accepted. No generator/compiler/test
was run and no package was published. Continue the dependency closure before
one grouped background verification rather than using linker failures as the
source-discovery method.

### Canonical control-state producer selection

The next source-to-link review traces wow_edit_set_window_fnid to the retained
NtUserSetWindowFNID body in kernel/ntstubs.c, through its existing registered
name/receive macros. No adapter definition is missing or should be invented:
the missing edge was build selection. The generator now selects that exact
subset as control-fnid.obj, independently of existing client/ntstubs.c.

Likewise wow_dialog_set_pointer calls original SetDialogPointer in
kernel/miscutil.c. Its body establishes FNID_DIALOG/WFDIALOGWINDOW and clears
PDLG with FNID_CLEANEDUP_BIT during cleanup. The generator now selects it as
dialog-pointer.obj. A source-local include/control-state/precomp.h supplies
the existing real WND/DIALOG declarations, without changing the mirror body
or routing to modern USER storage. Its /I must precede global include roots:
otherwise the unrelated MVDM precomp.h wins. The private include is not added
globally and cannot alter original WOW32 translation-unit environments.

Existing wow_edit_state_fixture source asserts initial EDIT promotion,
rejected conflicting/repeated promotion, idempotent cleanup and the original
dialog-pointer transition. These assertions are still unrun. This step closes
two build-selection edges, not the complete source group or runtime contract.
No guest, original algorithm or published image changes. Continue private
EDIT/dialog/getset dependency wiring before unified background verification.

### Rectangle selection and drawing-service boundary review

The retained rtl/rect.c CopyInflateRect body has only RECT and scalar inputs;
its production object is now control-rect.obj using the same source-local
control-state environment. No arithmetic rewrite, original-body change or
native state is necessary. This is source/build selection, not executed proof.

The adjacent retained DrawFrame body still requires SYSMET, ahbrSystem and
ghbrGray plus the already staged native PolyPatBlt leaf. Pinned original
client/clinit.c:930 calls NtUserInitBrushes to populate the client brushes;
client/random.c owns their globals and kernel/init.c LW_BrushInit creates
the gray pattern. Read-only dumpbin /exports of the installed SysWOW64
win32u.dll shows no NtUserInitBrushes export (the InitBrush/SysColor search
returns NtUserSetSysColors only). Therefore this old service cannot simply
be added to the native resolver as though present. No native call was made.

Source search of the staged seven EDIT units finds DrawFrame at
editsl.c:1894 and editml.c:1641, both using DF_WINDOWFRAME. This bounds those
current consumers, but does not authorize replacing the original algorithm,
silently fabricating the gray brush or assuming the full drawing contract is
connected. Original drawing remains unselected pending its finite system
metric/brush binding and failure/resource-lifetime review. No test, compiler,
guest modification or release operation was performed for this review.

### ADAPTER-WOW-064 drawing environment integration

The missing service is bound without replacing DrawFrame. Its process-life
system-brush array borrows native GetSysColorBrush results. The sole owned
gray brush uses original LW_BrushInit's monochrome8x8 WORD pattern through
CreateBitmap/CreatePatternBrush, deleting the temporary bitmap as upstream
does. Source: pinned OpenNT windows/core/ntuser/kernel/init.c, SHA256
7489347051F1A4F5096D5FC162CE0EAB6851510B9F1DCA0B5B799FEBC3943BD6.
The server display owner/public-brush promotion is not imported; there are no
cross-process brush handles. Pattern provenance remains Microsoft.

The existing EDIT entry requires successful once-only resource preparation
before original dispatch. Failure publishes no partial array/gray handle;
retry is confined to initialization, never replay of an application operation.
Borrowed brushes are never deleted. The owned brush remains until process exit,
like the original-control lookaside lock, so task exit cannot invalidate another
task's active drawing. Native resource failures return a deterministic error.
The original DrawFrame still owns its return semantics, including its original
TRUE result after PolyPatBlt failure; the adapter does not conceal that contract.

control-drawing/precomp.h maps only metrics/resources/PolyPatBlt, while the
production generator selects unchanged rtl/draw.c as control-draw.obj. Metrics
use native GetSystemMetrics and the old palette extent remains COLOR_INFOBK+1;
modern palette additions must not change DF_GRAY. No extra renderer, guest
change, system color mutation or original-body diff is introduced.

wow_edit_drawing_resources_fixture includes the actual adapter with GDI test
doubles: failed color lookup, bitmap allocation and pattern brush creation,
temporary cleanup, zero partial publication, retry, idempotence and last-error.
It remains unrun, alongside the original geometry fixture. Real raster/guest
acceptance and complete EDIT/dialog/getset integration are still pending.

### Remove duplicate original EDIT pool ownership

Whole-unit source review finds LOOKASIDE EditLookaside at original editec.c:16
and DialogLookaside at dlgmgr.c:312. The staged lock adapter also defined
EditLookaside, which would conflict with the complete original editor when
selected. That adapter definition and its unnecessary exported declaration
are removed immediately; no renaming of the original symbol or mirror diff
is used to accommodate a duplicate owner.

The allocator-only fixture does not link editec.c, so it now uses a private
fixture_pool for its original rtlinit allocation/overflow/reuse assertions.
The adapter continues to own only the existing process-life gcsLookaside
initialization. Both real control pools remain in their original translation
units. A source search across the staged EDIT/dialog units and adapter/test
files confirms the duplicate's removal; tests and full group link remain unrun.

### Original EDIT context-menu resource closure

Review confirms usercli.h already retains the original EC/SL/ML shared
prototypes; no second declaration block is added. The actual missing dependency
is ECMenu's LoadMenu(hmodUser, ID_EC_PROPERTY_MENU) at editec.c:271. Original
client/res.rc supplies this menu, and usercli.h:1292 defines ID1. Borrowing
modern user32's private resource numbering would not prove that contract.

OPENNT-HOST-087 registers and retains the original menu body only at its
original res.rc path, with Windows RC constants replacing the unavailable
full USER header environment. It preserves Undo/Cut/Copy/Paste/Delete/Select
All IDs and ordering. The generator links a separate edit-menu.res into the
same WOW32 image; original wow32.rc has strings/version, not a conflicting menu.
No guest resource, whole USER module or bitmap package is imported.

The actual editor private-precomp must bind this resource owner to original
WOW32 hmodWOW32, initialized by W32DllInitialize. The existing class adapter's
hmodUser=installed USER32 must remain unchanged: native class ownership and
recovered EDIT resources are different contracts. This consumer mapping and
runtime menu acceptance remain pending; merely linking the resource is not
proof of a functioning context menu. No original command-policy rewrite or
new menu construction routine is introduced. Source hashes/provenance are
registered in opennt-host/README; the complete group is still unbuilt.

### Unicode native message return boundary

The NtUserGetControlBrush name found by raw search in EDIT is commentary,
not an actual imported call. Original ECGetControlBrush at editec.c:2885
selects its parent/owner and calls SendMessageWorker or DefWindowProcWorker;
retain that algorithm instead of adding an unnecessary private service.

The current scoped native-message owner in wow_user_task_lifecycle.c exposed
only SendMessageA/DefWindowProcA. Original EDIT/dialog A/W branches require
the corresponding Unicode leaves. Both W wrappers now use the same existing
WOW_NATIVE_MESSAGE macro, including native result classification/publication,
execution restoration, last-error and exception cleanup. No conversion, new
thread, scheduler, dispatcher or mirror diff is added. The public header
declares both; the original private worker mapping remains pending because
PWND and private message payloads must not be blindly passed as HWND values.

The existing lifecycle fixture now exercises the W wrapper's numeric result
and last-error plus non-ANSI WM_SETTEXT/WM_GETTEXT through the actual native
entries, inside its retained task/window scope. Its existing postconditions
still assert scheduled owner, events and message identity. This supplemental
case remains unrun; whole-group guest and native-call acceptance is required.

### Original USER Unicode compilation contract

Pinned ntuser/client/sources.inc:47, rtl/client/sources.inc:30 and
kernel/sources.inc:31 select UNICODE. The provider's existing WOW32 flags
instead define ANSI. In particular usercli.h's LPCREATESTRUCT prototypes and
original controls' generic LoadMenu/SendMessage cannot inherit an arbitrary
WOW32 build environment. The generator now declares control_user_flags as
/UANSI /DUNICODE for the newly selected original USER control objects.
This is per-object, leaving original WOW32 and existing adapter builds intact;
the selected x86 /Gz /MT ABI is unchanged. Future original EDIT/dialog bodies
must use the same source-proven island. No forced source-header macro after
windows.h, string conversion workaround or original-body edit is used.

Review also confirms original clmsg.c DefWindowProcWorker includes client
compatibility/color and private-state logic. ECGetControlBrush's cross-thread
fallback passes a PWND in lParam; original color cases do not consume it as
a public HWND. Consequently no blanket DefWindowProcWorker-to-native macro
is installed by the W-leaf addition. Its private caller/provider contract,
including retained Win3.x compatibility, remains an explicit unresolved group
dependency. This source review is not build/runtime verification.

### Full default-handler dependency review and integration order

Read pinned client/clmsg.c DefWindowProcWorker from its entry through the
closing brace, including retained FE_IME conditional source, and the original
kernel/server.c gawDefWindowMsgs/gawDefWindowSpecMsgs arrays. This changes
the immediate integration order: do not build a HWND-only replacement for
the private worker or import its body against a zero-filled WND facade.

| Required family | Original consumer | Current evidence / required closure |
| --- | --- | --- |
| Message classification | FDEFWINDOWMSG and the two server.c arrays | Preserve the original server/client split and unknown-message zero behavior; no arbitrary modern default-handler substitution. |
| Relationships and geometry | HELP, mouse wheel, WINDOWPOSCHANGED, MOUSEACTIVATE | Current retained control WND refresh updates rectangles/styles/module only. Parent/owner/child pointers and referent lifetime must use existing window ownership/borrow machinery, not raw unpinned pointers or another registry. |
| Window name | GETTEXT and GETTEXTLENGTH | Current control-state creation leaves strName zero. Supply original Unicode length/buffer semantics and synchronous mutation/retirement before selecting these readers; zero initialization is not an accepted empty-title result. |
| Compatibility and colors | CTLCOLOR cases | Preserve WFWIN40COMPAT-dependent foreground/background behavior and original brush selection. Native DefWindowProc is not evidence of old-task compatibility. |
| Native leaves and return | CsSendMessage FNID_DEFWINDOWPROC, SendMessageWorker, hit test | Existing A/W scoped native APIs can carry appropriate HWND calls, but private PWND payloads require source-specific mapping. Keep original private dispatch decisions. |
| Shell/conditional profile | WM_KLUDGEMINRECT and FE_IME blocks | Source conditionals must follow the selected original profile. No fake shell message ID, unimplemented-success stub or accidental IME enablement is admitted. |

The source audit of wow_window_client_state_bindings.c and
wow_window_words_binding.c finds no producers for strName or spwndParent/
spwndOwner/spwndChild in the retained control state. Existing project_graph
has relationship observations for the guest view, but its new local refresh
copies only WINDOWINFO fields. That separate guest publication cannot be
claimed to have populated private host pointers. Existing borrow scopes retain
window references and release them before collecting retired storage; use that
lifetime owner when closing these fields, not a second identity map.

These are concrete prerequisite defects in the staged S3 group, not a new
S or a waiver. Finish retained-state producer/update/retirement contracts
before default-handler composition, then remaining private dispatch/getset
bindings and group build. No clmsg/server import or runtime replacement is
made by this review: its outgoing contract is not yet completely composable.
Earlier original draw/allocator/resource selections remain staged. No current
published failure is attributed to these unbuilt changes, and no new test
pass or release is claimed.

### Stored window-name boundary: source and installed export evidence

Question: can the missing retained WND name be captured without sending an
application WM_GETTEXT, and does the original reader retain its buffer across
an application callback?

Read pinned client/ntstubs.c InternalGetWindowText (1081), kernel/ntstubs.c
NtUserInternalGetWindowText (5455), kernel/winmgr.c _InternalGetWindowText
(176), client/clmsg.c WM_GETTEXT/WM_GETTEXTLENGTH (761--823), and rtl/chartran.c
WCSToMBEx. The last file SHA256 is
0F9F0B7FDE3AAA475492A50F333745508698228C38FDBF9A21CBD4BFBEAD383B.
The original server getter copies strName directly; it does not send a window
message. The original client wrapper returns the native count unchanged and
writes a terminating zero on a zero result, despite its BOOL return spelling.
Do not normalize that return to TRUE/FALSE in a binding.

Read-only dumpbin /exports, /imports and /disasm of installed x86 user32.dll
(SHA256 56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E)
find InternalGetWindowText at RVA AC400. Its body forwards three arguments
through the import table, preserves EAX, zeroes the first WCHAR on zero, and
returns with ret 0Ch. The imports include NtUserInternalGetWindowText.
This is a candidate named-export boundary, not permission to call an RVA or
infer all modern kernel behavior from the user-mode wrapper. Dynamic empty,
failure, truncation and no-application-callback checks remain unrun.

The original default reader's temporary name pointer is used only within
GETTEXT: Unicode memcpy or WCSToMB with allocation disabled, followed by
termination and return. GETTEXTLENGTH similarly consumes the buffer locally.
WCSToMBEx takes the Rtl conversion or NLS conversion path, not application
message dispatch. Searching the seven selected EDIT bodies finds their own
GETTEXT/SETTEXT handlers but no direct strName reads; dlg*.c likewise has no
direct strName read. This narrows the lifetime requirement: do not introduce
a multi-version title registry merely on the assumption that these readers
hold a pointer across application callbacks. Concurrent refresh still needs
serialization; the existing binding reference alone pins the WND, not a
separately replaced name buffer.

Implementation constraints established by this review: capture stored text,
not GetWindowText/WM_GETTEXT; retain count semantics and distinguish empty
from error where the native contract permits; update at the existing owner;
protect reader/replacement overlap without holding publication locks across
application dispatch; free owned text with that owner's final release. The
original server casts capacity to WORD, so its legacy limit must not silently
be inferred as the installed export's capacity contract. No new getter,
snapshot registry, mirror diff or production selection was added by this
review. The S3 dependency group remains unbuilt and unpublished.

### Canonical control entry refresh correction

Source tracing of wow_window_words_binding.c dispatch establishes two
different paths: WNDPROC_WOW refreshes WW before DispatchClientMessage;
the native-procedure branch does not. Canonical recovered EDIT/dialog entries
enter their task/borrow scopes and call wow_window_client_ensure_state, whose
existing-state branch previously returned without updating that WND. Original
editsl.c (WM_SIZE at2320; client-rectangle consumers1886/1893) and editml.c
(WM_SIZE at3275; rectangle consumers1635/2728) require live geometry rather
than a creation-time copy. A native-return graph refresh cannot substitute
for the state needed inside the earlier native callback.

ADAPTER-WOW-063 now makes the existing-state ensure branch call the existing
wow_window_words_refresh owner. This adds no mirror change, new sampling
registry, field-copy policy or fallback. Failure prevents entering the
ordinary original handler with stale state. The original final-destruction
entry intentionally bypasses ensure_state and continues to use retained
private data; it must not depend on another native geometry query.

The supplemental wow_edit_state_fixture now changes size, style and extended
style, invokes the actual repeated ensure entry, and checks rectangle/style
agreement, last-error preservation, allocation identity, FNID/private flags,
module-version and PED preservation. It also checks rejection after native
destruction. These cases are authored but unrun. This corrects the staged S3
entry graph; it is not attributed as a defect in published P16. No build or
runtime acceptance is claimed; git diff --check passes.

### Original control-state guards selected with native-result refresh

OPENNT-HOST-088 retains the complete SetWindowState/ClearWindowState bodies
from client/client.c (pinned SHA256
787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472).
Read-only normalized function comparisons report exact equality for both
bodies. Their original TestWF guards remain the only set/clear decision
owner. A separate control-state.obj selection leaves existing client.c
dispatch/registration/dialog selections intact.

The compile seam substitutes function names for the two symbolic SFI tags;
it sends neither generated NT4 SFI numbers nor hard-coded modern syscall IDs.
The existing wow_edit_call_bindings transport calls the already audited native
set/clear exports, then wow_window_client_refresh. This closes the immediate
native/local style consistency edge without duplicating SetWF mutations or
introducing SetWindowLong's style messages. Readiness/refresh failure uses
the established fatal invariant boundary because the original operation is
void and continuing with stale state would falsely report success. There is
no retry after native mutation and no claimed rollback of that mutation.

The existing call fixture now requires the additional original client.c
selection with WOW_ORIGINAL_CONTROL_STATE. Its transport doubles cover all
four audited flags, original repeated-set/repeated-clear no-op guards,
native-before-refresh ordering, error preservation, pre-mutation readiness
failure and post-mutation refresh failure. The separate native fixture already
contains style-result/no-extra-notification assertions. Both remain unrun;
the complete original EDIT consumer/private-precomp and grouped test build
are still pending. This is staged wiring, not native or guest acceptance.

The preceding CPD review confirms the existing unresolved boundary rather
than solving it: original _SetWindowLong normalizes a CPD to a real procedure
and encoding, and DefDlgProcWorker calls that procedure directly. Keeping an
opaque modern CPD in lpfnDlg would violate both contracts. No unsafe decoder,
temporary native dialog, second token registry or silent fallback was added.

### EDIT menu resource consumer and Ex tracking seam

Trace: unchanged editec.c ECMenu loads ID_EC_PROPERTY_MENU through hmodUser,
updates enablement, calls NtUserTrackPopupMenuEx with NONOTIFY/RETURNCMD,
destroys the menu, then sends the selected command (Select All uses -1).
The sole hmodUser reference in the seven selected EDIT files is this load.
Original WOW32 wow32.c declares hmodWOW32 at86 and initializes it from the DLL
handle at222. Original usercli.h1292 defines ID_EC_PROPERTY_MENU as1.

The EDIT-private call header now includes a menu binding that maps only that
translation-unit contract: hmodUser becomes the already loaded WOW32 module,
LoadMenu uses the retained Unicode resource/parser owner, mutation/destruction
use the existing native menu publication owner, and focus/command dispatch
use the existing native-call scopes. The original numeric resource ID is
retained under OPENNT-HOST-087 alongside its already-selected res.rc. No path
lookup, second resource image or host USER32 resource-ID assumption is added.

The existing menu tracking owner now shares its resolution/publication and
native-call finally across TrackPopupMenu and TrackPopupMenuEx. Each retains
its own native argument list; Ex is not emulated through the older API.
The numeric return is not reduced to a boolean, because original ECMenu needs
the selected command. Cancellation/error and owner/menu destruction still
use the existing common native-return publication, not a new cleanup owner.

wow_edit_menu_binding_fixture.c checks the real header mapping with transport
doubles: module/resource identity, load failure, signed coordinates, popup
flags, numeric command and -1 preservation, destruction selection and Unicode
command dispatch. This does not exercise ECMenu or the native tracking loop;
its scope is explicit. Whole-group verification must additionally exercise
actual menu resources, no-selection/read-only/password enablement, cancel,
selected commands and callback destruction. No test was run. Production EDIT
private-precomp/body selection is still incomplete, so no runtime capability
or release is claimed. Static diff whitespace validation passes.

### Numeric child ID in the retained control WND

Review of the remaining ECNcDestroyHandler input establishes another missing
producer: edecrare.c396--397 forms WM_PARENTNOTIFY using pwnd->spmenu as the
child ID. Original createw.c661--670 distinguishes a child's numeric value
from a top-level menu pointer; rtl/getset.c GWL_ID uses that same distinction.
The staged retained-state constructor previously left spmenu zero, and its
refresh never supplied an ID. This is not repaired by the separate guest-view
menu graph or by loading the EDIT context menu resource.

ADAPTER-WOW-063 now captures GWL_ID at child-state construction and updates it
in the existing apply_window_info publication owner. Zero and high-bit values
retain DWORD semantics. Both sites distinguish zero success from native query
failure via last error. Child classification matches original TestwndChild
(WS_CHILD with WS_POPUP absent), not a loose bit test. When a refresh changes
that classification to non-child, it clears the previous numeric value so it
cannot be dereferenced as a MENU pointer. No native HMENU is cast to PMENU.
Top-level private menu-pointer projection and parent-pointer lifetime remain
explicit dependencies, not claimed completed by this change.

The unrun state fixture adds a real native parent/child pair, verifies initial
ID0xfe01, refresh to valid zero and then0xfffffe02, preserves WND identity,
and destroys/releases both windows. This supplements source tracing; the
original combo notification/retirement interaction still needs the complete
group acceptance. No new mirror diff or guest modification is introduced.

### Relationship-owner review and observation consistency correction

The existing project_graph already computes a closed observation set with
full next/parent/child/owner identities. capture_window includes control_id;
the committed graph then updates local retained WND geometry/styles. The
recent ID addition initially queried native GWL_ID again on that path. This
could combine committed geometry with a later ID. The observed refresh now
accepts the transaction's numeric ID and consumes it directly. Ordinary
callback refresh continues querying current native state. All callers and
fixture signatures are updated; the child fixture explicitly supplies an
observation ID different from the native ID and checks the two paths.
No new sampling pass, guest write or handle namespace is introduced.

The lifetime review confirms why parent pointers cannot yet be assigned:
wow_user_borrow_scope retains enrolled HWND bindings and separately retains
sampled query backing, whereas project_graph publishes relationships only
to the guest view. Those are not live host PWND pointers. Existing references
can retain an enrolled parent through nested destruction, but a persistent
parent/child pair of owning references would form cycles; raw unborrowed
links would instead dangle. Query-only external/desktop nodes also lack the
local creation facts required by the control-state constructor. Do not
silently create a mutable control owner for such nodes or alias guest memory
as that owner's native allocation.

The next relationship integration must reuse the committed identity graph
and existing borrow/retirement owner, cover enrolled and observation-only
referents, and define nested-scope refresh/release before publishing host
links. This review has not implemented those links. The ID transaction
correction is implemented but unrun, and no parent-lifetime, group-build or
runtime pass is claimed. git diff --check passes.

### Separate retirement domains: concrete host-link prerequisite

Read release_window_backing, collect_retired_windows,
page_domain_window_referenced, page_domain_withdraw_window and retire_handle
in mvdm_softpc_wow_page_domain.c, and wow_window_words_release/terminal in
wow_window_words_binding.c. The guest domain can withdraw a handle identity
while incoming graph edges still retain its allocation. Physical reclamation
waits for explicit borrows, retained-owner release AND those incoming edges.
Withdrawal clears next/child/owner and retains the desktop parent for the old
borrow. Identity reuse and backing lifetime are intentionally distinct.

The native binding instead frees client_state when its own reference count
reaches zero. Calling release_retained_window first may leave the guest
allocation alive because of incoming edges; it does not retain client_state.
Therefore a guest graph reference is not proof that a new raw host PWND
edge remains valid. The proposed direct reuse must account for BOTH lifetimes,
not merely copy pointer values from the guest graph. No raw host relationships
have been published, so this is an unresolved staged integration prerequisite,
not evidence of an existing dangling pointer in P16.

Necessary host-edge contract is now precise: logical HWND withdrawal must
remain immediate; old referents stay alive while existing source/reader
borrows can use them; rewiring must precede final referent release; HWND slot
reuse must not resolve an old edge to its new generation. Permanent mutual
references without destruction-time unlinking, or delaying logical withdrawal
until physical free, would violate those requirements. Cycles with explicit
unlinking are not inherently invalid; the review below corrects that earlier
overbroad rejection. Extend the existing native binding/borrow owner
if needed, rather than making guest backing own arbitrary native callbacks or
creating another HWND identity registry. Observation-only and desktop nodes
must retain their query-only role.

The source audit also found missing declarations for the just-selected
original state helpers. Their exact original usercli.h prototypes are now
retained under OPENNT-HOST-088, avoiding implicit C declarations at EDIT
call sites. This is declaration/build wiring only, not a test or release.

### Original explicit unlink permits bounded relationship references

Read pinned kernel/createw.c2310--2353 and2384 (SHA256
DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7).
The original owner explicitly keeps parent/owner relationships through
WM_DESTROY and WM_NCDESTROY; the source cites an application that calls
GetParent after every message. Afterwards it replaces the parent with the
desktop where applicable, unlocks child/owner/last-active, dereferences the
class and finally drops the destruction borrow. Its later free path unlocks
the retained desktop parent. Therefore mutual references are a viable
source-shaped mechanism when their explicit unlink/retirement is paired;
they must not be rejected merely for forming a live graph cycle.

This narrows the proposed host integration to existing binding references
per published relationship, replacement that retains new endpoints before
releasing old ones, and an explicit post-notification unlink in the terminal
owner. Reader borrows remain separate: an edge removed by a nested callback
does not end a reader's already-acquired reference. No runtime-global lock
may remain held through application dispatch; wow_user_runtime.h explicitly
distinguishes data locking from callout borrows. The original ordering, not
a cycle collector or an invented early-null policy, is the reference.

Read dlgmgr.c GetParentDialog in addition to the EDIT visibility loop. It
uses WFDIALOGWINDOW/WEFCONTROLPARENT/DFCONTROL and returns an ancestor PWND.
Consequently a one-slot scratch parent view based only on WINDOWINFO cannot
serve the complete group: it loses dialog classification and returned-object
lifetime. Query-only nodes cannot silently masquerade as initialized local
dialogs. Their mapping still requires explicit treatment before production
host edges are enabled. This is a design correction from source evidence,
not an implemented relationship owner or a test pass.

### EDIT thread identity is comparison-only, not a THREADINFO dependency

Procedure: searched both the staged and pinned ntuser/client ed*.c and
dlg*.c files for GETPTI, PtiCurrent and GETPPI, then read the complete
ECGetControlBrush body and its clmsg.c color branches. Both searches have
one match: editec.c2905. Original user.h1155 defines GETPTI as head.pti;
this consumer compares identities only, with no THREADINFO dereference.
The admitted EDIT/dialog group therefore does not require importing or
constructing a private NT4 THREADINFO merely for this expression.

Preserve the original branch: same-thread parent/owner receives
SendMessageWorker; a different-thread recipient uses DefWindowProcWorker
instead, avoiding an added cross-thread application callback. The latter
passes an EDIT PWND as lParam, not HWND. Original clmsg.c696--759 color
branches do not dereference that payload and retain compatibility-dependent
color selection. Neither unconditional native SendMessage nor a blanket
native DefWindowProc substitution preserves this caller contract.

The existing capture_window transaction already records and rechecks native
process_id/thread_id with full HWND identity; its window_observation schema
already carries both. Integrate that captured identity with the retained
host relationship binding, rather than building a second thread registry,
casting a DWORD into a fictitious THREADINFO pointer, or querying a possibly
reused HWND after callback return. Missing/retired observation is not thread
zero and must not be classified as an ordinary foreign-thread success.
Scope this comparison mapping to the EDIT compilation environment: original
scheduler and task cleanup consumers still need their real task bindings.

This resolves the thread-object question in the relationship design, not
the remaining relationship lifetime/private-precomp implementation. No
runtime code, guest bytes, package or test result changes in this review.

### Stored-name producer and bounded reader lifetime

The next source inspection found that callback_enter in
wow_user_task_lifecycle.c explicitly leaves the runtime data lock before
returning to recovered client code. Thus that lock cannot implicitly protect
clmsg.c's synchronous strName readers. The SDK 10.0.22621.0 WinUser.h13505
declares InternalGetWindowText as an int-returning three-argument WINAPI;
use that named USER32 import, not a private RVA or a WM_GETTEXT callback.
Earlier original-source/installed-export evidence above remains applicable.

ADAPTER-WOW-063 now stages stored Unicode name capture at host WND creation
and refresh. A full buffer grows and retries; allocation/query failure leaves
the old published name unchanged. Length is bytes, capacity includes the NUL,
and the LARGE_UNICODE_STRING 31-bit capacity bound is checked. Empty strings
remain valid. Creation rollback and final binding release free the buffer;
logical HWND withdrawal does not invalidate an already-retained name.

Replacement waits for a per-binding shared name-reader lock, swaps the three
name fields together, then frees the displaced buffer. Lock order is
publication then name; a name reader must not call publication APIs, USER,
application code, refresh or binding release while holding it. This is not a
lock across original default-handler callbacks. Only its synchronous
GETTEXT/GETTEXTLENGTH cases may use this read guard; that original-worker
consumer is not yet selected/wired. No second name registry, permanent old
buffer list, guessed native WND offset or guest mutation is introduced.

wow_edit_state_fixture.c adds long-name growth, empty-name replacement,
invalid-HWND output preservation and a WM_GETTEXT/GETTEXTLENGTH trap proving
the intended no-application-callback boundary when run. These are staged
assertions, not passing evidence. Group build and reader-concurrency,
destroy/reuse, allocation-failure and actual original default-handler tests
remain required. git diff --check passes; no build, runtime test or release
was performed for this partial group.

Follow-up reader audit: pinned clmsg.c760--829 keeps both Unicode and ANSI
GETTEXT/LENGTH reads inside synchronous branches. ANSI uses WCSToMB or
RtlUnicodeToMultiByteSize; replacing these with a newly authored copier
would discard original conversion/length semantics. kernel/winmgr.c176
_InternalGetWindowText is a different stored-name copy API, not a substitute
for these branches. Retain the full original default worker rather than
introducing a second text-message implementation just to consume this field.

The existing state fixture now also starts one joined native reader while
the main thread performs100 actual native title changes and retained-state
refreshes. It checks that each locked observation is entirely the300-character
X name or600-character Y name, including byte length, capacity and NUL; no
test-side name provider replaces production capture/publication. Reader
completion precedes stack/binding release; a stuck reader fails the fixture
process rather than returning dangling storage. After native destruction,
the held binding must still expose its last name through the same read lock.
These cases are unrun and do not prove exhaustive concurrency, allocation
failure or original default-worker integration. They close missing fixture
coverage without compiling/testing an incomplete dependency group.

### Control send boundary uses the approved sole native transport

Read pinned clmsg.c258--479 SendMessageWorker, not just its signature. It
implements private Hook checks, inter-thread/server-procedure routing, A/W
conversion, canonical control fast dispatch and CALLPROC_WOWCHECKPWW. Its
private gSharedInfo.awmControl/FNID procedure tables and native Hook state
cannot be recovered from a copied WND. Recreating them would also compete
with the expressly approved modern-USER delivery/conversion boundary.
Therefore this operation uses the existing scoped native transport rather
than importing a second client message dispatcher or forcing IsHooked true.
This is the already-approved architecture boundary, not a new waiver of
original EDIT/default-window algorithms.

The seven staged EDIT units have exactly two SendMessageWorker consumers:
editec.c2914 sends the selected parent's CTLCOLOR message with native HDC
and child HWND; editml.c2085 sends WM_CHAR/VK_BACK to its own window. Both
carry public native message payloads. The EDIT-only header now maps their
unchanged call shape to wow_window_client_send_message, which extracts
head.h once and selects wow_native_SendMessageA/W. Existing scope restoration,
native result/last-error and required return-state publication remain in that
transport owner. No new callback invocation, payload translator, dispatcher,
registry or mirror diff is introduced. DefWindowProcWorker's private PWND
color payload is not routed through this binding.

The supplemental wow_window_client_message_fixture.c links that production
binding with transport doubles, asserting HWND identity (not the PWND address),
A/W selection, payload and numeric brush result, last-error, null rejection
and no post-call state reread. It remains unrun and is not guest/native
delivery evidence. Full original EDIT/private-precomp selection and the
source-scoped default-handler/relationship closure still precede group build.

### CPD restoration cannot dispatch through the current procedure

Re-read the complete retained client/ntstubs.c _SetWindowLong and pinned
kernel/getset.c GetCPD742 onward. For DWL_DLGPROC the original setter first
obtains the old logical procedure/encoding, creates the return conversion
identity if required, then resolves an incoming saved CPD to its previous
procedure and actual encoding before updating PDLG. dlgmgr.c490 subsequently
calls that stored procedure directly. These are distinct contracts; a
successful CallWindowProc on a token does not expose its underlying address.

GetCPD chooses pcls for CPD_WND and CPD_DIALOG, searches spcpdFirst by
procedure plus WORD options, and links a new object into that class list.
Corrected wow_edit_conversion_bindings.c's inaccurate per-window-ownership
comment accordingly. Its runtime remains a borrowed native token passed to
the existing CallWindowProc bridge; it adds no token teardown.

Rejected simplification: storing one generic trampoline in PDLG which looks
up the current window procedure on invocation. A caller can save procedure A,
replace it with B, then call A. Looking up B loses the saved identity and may
recurse. Nor may GetCPD be called with another CPD as its target: the original
owner explicitly asserts that dwProc32 is not CPD-tagged. Any eventual mapping
must preserve each saved procedure/encoding identity across replacements and
class lifetime, and must account for tokens originating outside our creator.
No arbitrary modern CPD decoder, private address access, temporary window or
procedure replacement is introduced by this review. This constrains the
pending binding rather than falsely marking the dependency resolved.

### Retained-state identity and failed child-ID observations

Reviewed apply_window_info's final HWND-property/binding check against
detach_matching and the existing class/window lifetime fixture. Name capture
happens before publication; the final check prevents a retired binding from
accepting fields from its replacement, and frees the unpublished candidate
on rejection. Extended that existing fixture with distinct old/replacement
stored names on the same HWND, rejected old-binding refresh, independent
locked reads and retained name after actual DestroyWindow. No new identity
map or native lifetime claim is made. This added case remains unrun.

The sweep found a production inconsistency in capture_window: unlike the
new retained WND constructor/refresh, its earlier child-ID sample used only
WS_CHILD and accepted GetWindowLong's zero even on error. The graph could
therefore publish a failed query as a valid numeric ID and later feed it to
WND.spmenu. It now uses original TestwndChild's CHILD-without-POPUP predicate
and zero/last-error distinction, matching the other two producers. Negative
sampling is rejected before graph publication; valid zero and all DWORD ID
bits remain data. This is a project-boundary correction, no original mirror
or guest change. Group query-failure/identity tests are still required; no
runtime result or new publication is claimed by source review.

### CPD input encoding is distinct from retained target encoding

Pinned client/clmsg.c CallWindowProcAorW explicitly defines bAnsi as input
encoding. For a tagged CPD and a translated message it passes that flag to
CsSendMessage; for WM_USER and non-translated messages it calls the saved
pfnClientPrevious directly. Pinned kernel/server.c xxxWrapCallWindowProc
uses that saved destination and CPD_UNICODE_TO_ANSI to select the target's
SCMS_FLAGS_ANSI. Thus the token does not require its caller always to use
the entry opposite the target's encoding. Both A and W entry contracts must
work with the same saved identity. This closes the source-level direction
question, not the modern token decoding/storage dependency.

Extended the existing native ABI fixture, without adding a separate runner:
both input encodings call both saved destinations after current-procedure
replacement; replacement must not receive those calls. After source-window
destruction, a live same-class peer exercises the saved tokens, plus opaque
WM_APP payloads. The class remains registered throughout. These assertions
are unrun requirements for the grouped suite, not claims about installed
USER behavior or recovered dialog execution.

The original dialog setter's normalization requirement remains real: native
CallWindowProc supplies invocation but not pfnClientPrevious for a subsequent
direct PDLG call or GetCPD. No private decoder, executable trampoline, nested
CPD or current-procedure lookup has been introduced. Do not select the
original dialog get/set body until this storage boundary is connected.

### Complete default-window owner and scoped name reads

OPENNT-HOST-089 now retains all 663 lines of DefWindowProcWorker from pinned
client/clmsg.c (source SHA256
2F87967A05E730E2C95B8CE0B92443D323F76BAEC97446D3C4F8C9DC163710FD).
An LF-normalized comparison of the entire retained function against that
source passes. No case, compatibility decision or conditional IME branch
was removed. Source selection and symbol binding are outside the body.

The ADAPTER-WOW-063 wrapper now pairs retained HWND binding and name read
lock only for WM_GETTEXT/WM_GETTEXTLENGTH, verifies exact host WND identity,
and releases both in finally even if the original copy raises. All other
messages enter the original worker without this lock. Original server.c
gawDefWindowMsgs excludes both text reads; gawDefWindowSpecMsgs includes
both. Their client branches perform only conversion/copy, not callbacks.
The original DEBUG gfTurboDWP server fallback cannot run under that lock:
this cohort explicitly requires the existing product Release selection.
No native getter replaces the original name-reading algorithm.

The EDIT call seam selects the wrapper; its adapter object is staged in the
generator. The original default body still awaits complete private-precomp,
message-table, relationship and native-server-operation bindings before
production selection. This graph remains intentionally unbuilt/incomplete.
The added boundary fixture covers exact identity, no lock for callbacks,
normal last-error/result forwarding and exception unlock. It is unrun and
does not establish original default behavior or guest acceptance.

### Original default-message classification is now connected

OPENNT-HOST-090 retains both original default-message arrays (95 lines
including their intervening original comment) and complete 22-line
InitWindowMsgTable from kernel/server.c. LF-normalized source comparison
passes for the complete retained group. WNDMSG and FDEFWINDOWMSG also retain
their original declarations/expression. Nine private messages missing from
the installed SDK use the existing pinned winuserp.h values; no new IDs or
heuristic switch were invented.

The process owner initializes a private candidate pair using that original
algorithm, then publishes once. SharedAlloc is a raising process-heap leaf;
finally frees either partial allocation on failure, and the once operation
can retry. There are no USER calls or application callbacks in initialization.
The default wrapper establishes readiness before taking any name lock.
Original code still owns classification and the client/server branch order.
In particular WM_CONTEXTMENU is in the server table, not the special table,
despite a client switch case being present. WM_GETTEXT/WM_GETTEXTLENGTH are
special-only; the stored-name reader guard is therefore local to the copy.

The generator selects the table object and adapter; it still does not select
the incomplete default worker/private-precomp group. Supplemental table
acceptance covers independent message-membership cases, UINT_MAX bounds,
zero/terminator behavior, error preservation and stable once-published
addresses. It remains unrun. Allocation-failure retry and concurrent startup
acceptance remain required in the grouped suite. Window relationships,
native server-call boundary, shell state and complete build wiring remain
open; the message tables are no longer an unspecified producer.

### Default-window server-call seam reuses existing native return ownership

Reviewed every CsSendMessage call in the complete retained clmsg.c worker:
all use zero xParam and FNID_DEFWINDOWPROC, with the caller's A/W flag.
The new finite receive binding accepts exactly that operation and rejects
nonzero extra data or other FNIDs. It calls existing
wow_native_DefWindowProcA/W, whose WOW_NATIVE_MESSAGE definition already
owns callback execution release/restoration, message-result observation and
required return-state publication. No new message engine, WOW scheduling
owner, private USER pointer conversion or syscall ordinal is introduced.

The default-worker symbol seam also routes its SendMessageW and
SendMessageWorker calls to the existing scoped A/W native transport. This
does not replace the original default worker wholesale: client compatibility
color/text/window-position logic remains in the retained source. The symbolic
FNID_DEFWINDOWPROC declaration is retained from pinned user.h649 under the
existing OPENNT-HOST-089 registration. Boundary acceptance checks A/W,
numeric return/last-error, untouched message payload and rejected operation
forms; it remains unrun. The adapter object is staged in the group graph.

Further source tracing makes two remaining defaults dependencies explicit:
FindNCHit depends on private scrollbar/menu-present flags, class/icon state
and original border metrics; no zero-filled surrogate is acceptable.
The WM_KLUDGEMINRECT path obtains uiShellMsg via SetTaskmanWindow(NULL), while
original kernel/server.c1883 initializes it with UserAddAtom(L"SHELLHOOK",
TRUE). Calling modern SetTaskmanWindow merely to initialize that field would
have desktop-global effects and is not adopted. Neither dependency is
silently redirected or considered complete by the new server-call seam.

### Shell message identity comes from USER's message atom namespace

Pinned kernel/server.c1883 uses UserAddAtom(L"SHELLHOOK", TRUE).
kernel/userk.h3804 defines _RegisterWindowMessage through AddAtomW; its
AddAtomW macro calls UserAddAtom with bPin FALSE. atom.c shows bPin controls
pinning after insertion into the same UserAtomTableHandle, not a distinct
message namespace. The product only needs that registered message identity;
it must not claim ownership of the native desktop's task-manager window.
Pinned miscutil.c _SetTaskmanWindow(NULL) really clears that desktop pointer,
so invoking it on modern USER would not be an innocuous initializer.

The existing default-message once owner now registers SHELLHOOK using native
RegisterWindowMessageW after both bitmap allocations succeed, and publishes
uiShellMsg together with them. Registration failure frees both candidates and
retains its error; allocation failure occurs before registration. Successful
publication retains the registered identity for process lifetime, without
GlobalDeleteAtom, a private atom table or native taskman-window mutation.
The original worker still needs its finite server-state view connected to
this member before selection; the missing field must not be filled with zero.
The supplemental fixture checks registered-message range and name identity;
it remains unrun. This is an ADAPTER-WOW-063 producer binding, not a guest or
original algorithm modification.

### Default-worker color and shell state is consumed at the entry boundary

Audited every gpsi/oemInfo/SYSRGB/ahbrSystem/ghbrGray read in the complete
retained default worker. Its direct requirements are system colors/brushes,
screen bit count for the scrollbar branch, and uiShellMsg. The new finite
per-call context supplies those inputs without casting any native structure.
Color messages sample current GetSysColor values and reuse the existing
original-pattern gray/system-brush owner. Only WM_CTLCOLORSCROLLBAR reads
desktop PLANES/BITSPIXEL, acquired/released before original execution. Other
messages do not acquire a DC or initialize drawing resources.

The real default wrapper now captures this context before any name lock,
publishes a TLS pointer for unchanged original field reads, and restores the
previous pointer in finally. Nested calls cannot overwrite the outer call's
snapshot, and exceptions cannot leave TLS pointing to an unwound frame.
This is a synchronous input facade, not another window/state registry.
The original NULL SetTaskmanWindow fallback has a finite initialization
binding that only fills the current context's SHELLHOOK identity; non-NULL
requests and out-of-scope calls fail without touching the desktop.

Updated the wrapper fixture for nesting, exception restoration and capture
failure. A native-input fixture covers colors/brush identities, screen-depth
capture and the bounded shell initializer. Both are unrun. The generator
selects the input owner with the other staged adapters, while the original
worker remains excluded until relationship, hit-test and private-precomp
closure. No runtime claim or new publication follows from this source wiring.

### Nonclient layout flags are outcomes; border width has an exact public input

Pinned kernel/calcclrc.c clears WFHPRESENT/WFVPRESENT/WFCPRESENT/WFMPRESENT
before calculating the client rectangle. A minimized window returns before
these flags are installed. Horizontal presence requires requested scrolling
AND strictly more available height than CYHSCROLL; vertical presence requires
at least CXVSCROLL width. Menu presence is set only for a non-child with a
menu. Therefore mapping raw WS_HSCROLL/WS_VSCROLL directly into the private
present flags is incorrect. No such approximate producer has been added.
Native/style-owned frame geometry cannot be reconstructed just by filling
those bits from requested styles; the complete remaining binding must respect
actual layout. NTW16S already handles its styled WM_NCHITTEST before forwarding.

One input does have an exact existing service: original rare.c's
SPI_GETBORDER writes gpsi->gclBorder, also populated from
NONCLIENTMETRICS.iBorderWidth in inctlpan.c. Default-state capture now queries
that service for WM_NCHITTEST, without inferring logical units from padded
frame pixels. OPENNT-HOST-091 retains the complete 37-line GetWindowBorders
body byte-equivalent after LF normalization; its true-subset build selector
separates the existing _FChildVisible body without altering either algorithm.
The generator selects the original border object with a finite precomp.
The logical-unit fixture covers style precedence, client/window switches and
different border settings; the input fixture checks the native SPI producer.
Both remain unrun. Actual present/icon/class/parent inputs and full FindNCHit
selection are not established by this step; no new runtime release is claimed.

### EDIT thread comparison uses retained creation identity, not a fake THREADINFO

Rechecked all seven original ed*.c units: their sole PtiCurrent/GETPTI use
is editec.c2905, the equality test in ECGetControlBrush. Same-thread parents
receive CTLCOLOR through SendMessageWorker; other threads use the original
default worker without invoking that parent's application procedure. No
THREADINFO field is dereferenced by this selected use.

The existing HWND binding now captures its native owning thread at creation
enrollment, rejects a foreign-process attachment, and retains that immutable
ID after detach. The sole production attach caller is enroll_created_window,
which receives the full native creation HWND. Existing acquire/private-word
paths still use their short-handle-capable validation; no new thread query
was added to those access paths. This is not a new task/thread registry.

An exact host-WND borrow lookup reuses the current nested borrow list before
acquiring a live HWND property. Thus a saved retired allocation is not rebound
to the replacement property's state or thread identity. The EDIT-only macro
seam compares native thread IDs, leaving the original branch unchanged; it
does not manufacture a THREADINFO pointer or redefine the original scheduler's
thread accessors. Unknown state is an adaptation failure, not a guessed
same-thread or cross-thread result. The lifetime fixture now checks identity
across detach/reattach and retained-scope lookup; it remains unrun.

The binding covers recovered local WNDs. The still-missing relationship/view
producer must supply retained identities for observed native/external parent
nodes before the complete control group is selected; it may not treat an
unowned parent as local or synthesize zero thread identity. Actual two-thread
CTLCOLOR/default-branch acceptance remains part of grouped verification.

### Default-name reader uses the same retained allocation after destruction

The earlier name-reader wrapper still acquired its binding through the live
HWND property. That contradicted its caller's retained WND contract: nested
destruction may remove the property while the original caller legitimately
continues using its borrowed state. It now uses the same exact-state scope
lookup as the thread identity consumer, and no longer acquires/releases an
extra property-owned reference. The existing outer borrow owns storage;
the synchronous name lock alone protects replacement/free during copy.

The wrapper fixture now supplies only the scope-lookup boundary, not a live
property acquire/release double; unexpected reintroduction of those calls
cannot silently pass that fixture. The shared class/window lifetime fixture
also resolves a retained state from a nested scope after actual native window
destruction, preserving last-error and creation thread identity. These cases
remain unrun. This repairs the staged project boundary, with no original
body/guest modification or claim that relationship publication is complete.

### Observed-window thread identity comes from the existing retained page owner

Traced page_domain_prepare_window, project-window commit, window_borrow_link,
release_window_backing and collect_retired_windows. Existing backing records
already retain process_id/thread_id with generation, readers and retirement.
Reuse that record rather than making native-thread lookups after destruction
or adding a second HWND/thread registry.

Added a finite read_window_identity export which requires an outstanding
backing borrow and known nonzero metadata, leaves outputs unchanged on
failure, and permits reads after logical retirement while that borrow lives.
The NTVDM export list is updated with the declaration/implementation. This is
a paired host ABI addition for the eventual NTVDM/WOW32 grouped build, not a
guest/wire change or an independently publishable DLL change.

The existing sampled-word scope now copies this retained identity into its
unpublished window binding before exposure. Observation still creates no
property or task ownership; later enrollment checks that the creation thread
agrees instead of silently replacing the observed identity. Exact-state lookup
also recognizes sampled borrows. The existing page-domain promotion fixture
now checks identity before/after retirement and rejection after final release;
the tests remain unrun. Host relationship pointers and read-only host WND
carriers still require implementation; this closes their identity source,
not their complete producer/lifetime graph.

### Observed WW fields use one committed backing, not a second native sample

Reviewed wow_user_borrow_window and the page-domain prepare, refresh, graph
commit, copy-view and retirement paths. The former mixed retained WOW words
with newly queried WINDOWINFO/GWL_HINSTANCE. The staged reader now obtains
words/style/exstyle/module from the same retained backing. Module travels as
host metadata with the existing observation row and borrow, never in guest
WND pointer fields. Both paired host declaration and NTVDM export are present.

The follow-up source review found ordinary refresh/promotion did not update
the newly added module metadata. Sampling now belongs to page_domain_window_info:
all fallible native reads finish before copied fields and module are committed.
Preparation uses that same path rather than a duplicate module query. Graph
commit still uses its captured row without native revalidation; retained dead
external observations retain the approved S2 observation boundary.

Extended existing CCPU fixture cases verify committed style before native
mutation, unchanged reads until refresh, refreshed style/words/module,
module in exported graph observations, reads after native destruction and
logical retirement, and unchanged output on reads after final release.
These are source additions only, not test results. Complete grouped build
and execution are still pending. No original algorithm or guest was changed;
no host relationship pointer/state production is claimed by this change.

### Saved dialog procedure: native reference must cover restoration, not only call

Rechecked selected wuwind.c WU32GetWindowLong DWL_DLGPROC and
WU32SetWindowLong's IsThunkWindowProc branch. The latter passes a recovered
32-bit procedure identity to SetWindowLong before clearing vpfnDlgProc;
therefore treating every dialog procedure as W32DialogFunc would remove an
original reachable restore path. Recovered dialog get/set must handle that
path, not only creation with the usual WOW dispatcher. Native-owned dialogs
remain a separate route; their existence does not prove recovered PDLG works.

The existing native fixture tested saved CPD invocation but not the contract
that blocks original _SetWindowLong selection: normalization on restoration,
followed by WM_NOTIFYFORMAT's actual target encoding. Added an independently
authored empty native dialog case in check_dialog_cpd_restore. It saves the
Unicode procedure through the ANSI getter, replaces it, restores the saved
identity, and checks the raw Unicode getter and NFR_UNICODE. It then performs
the reverse direction and checks the ANSI getter and NFR_ANSI. The dialog is
invisible and destroyed by the fixture. It is a native reference witness,
not a proposed scratch-window decoder or a production implementation.

This case remains unrun until grouped verification. The original-getset mock
fixture alone cannot close this dependency: it supplies CALLPROCDATA directly,
which modern public USER does not expose. No CPD cast, encoding assumption,
test-pass claim, runtime source selection or semantic waiver was added.

### Creator dispatch and custom-dialog WND must precede PDLG promotion

Traced complete explicit enrollment against retained dlgbegin.c's root
NtUserCreateWindowEx, immediate ValidateHwnd/ValidateDialogPwnd, WOWDlgInit,
and child-creation sequence. Original EDIT does not discover shared DS by
walking parents: dlgbegin allocates PDLG.hData with GetEditDS and passes it
as each qualifying child's module. Thus preserving the creator and its
state is a prerequisite; a parent-derived substitute would be wrong.

Two project-source gaps were found and corrected together:

- wow_class_words_import_native already selected wow_dialog_window_procA,
  but enroll_created_window exempted only recovered EDIT from replacing the
  target with native GWL_WNDPROC. Both A/W recovered DIALOG targets now retain
  the same protection. Explicit ordinary DIALOG creation also captures the
  same facts as EDIT before entering native creation.
- claim_explicit allocated shared WND only for EDIT. The original dialog
  creator also needs it for custom dialog classes before its immediate
  ValidateHwnd, even when no default dialog procedure ran during creation.
  All explicit original paths with captured facts now publish WND before
  their first procedure callback. This is the existing binding-owned
  allocation; original ValidateDialogPwnd remains the sole PDLG promoter.

The counted-creation fixture now checks WND during actual WM_NCCREATE and
checks retained instance, version and creator encoding after return for
atom/string and A/W creator combinations. No build or run has occurred.
Generic implicit creation still lacks proven creator provenance; relation
state, CPD normalization and complete private-precomp remain open. This
change adds no mirror or guest diff and does not claim original-dialog
runtime acceptance merely from the dispatch correction.

The supplemental native-creation fixture now also exercises the previously
missed DIALOG target directly: import the real native #32770 descriptor into
the existing class owner, enroll its native child through the actual internal
enrollment function, and assert the logical target remains
wow_dialog_window_procA. Rejecting that enrollment must remove its property
and restore native dispatch before fixture destruction. It does not invoke
the unwired original manager, substitute a dialog implementation, or claim
guest acceptance. This is an unrun negative-control/rollback witness for the
specific target-selection correction, separate from the full dialog suite.

Rechecked GetParentDialog and its outgoing operations against pinned
client/dlgmgr.c: the complete original body is already retained in the staged
unit, so no duplicate parent-walking helper is needed. Its WFDIALOGWINDOW,
WEFCONTROLPARENT, DFCONTROL and WFCHILD decisions require correctly retained
host relationship nodes; the remaining work is their producer/lifetime, not
another search algorithm. Native parent HWND lookup alone cannot stand in
for those original fields.

Also reconfirmed the separate EDIT-to-COMBO terminal edge in edecrare.c and
combo.c: WM_PARENTNOTIFY/WM_DESTROY repairs the canonical PCBOX edit/list
references. Ordinary SendMessage could dispatch an application subclass and
miss that repair. A future binding must use the retained canonical native
combo owner (where combo remains native), with existing call-scope restoration
and once-only retirement evidence, or admit a composable original combo
slice. No PCBOX overlay or generic notification substitution was introduced.

### Retained host-state provenance is not current HWND validity

Source review found wow_window_client_owns_state still called live
ValidateHwnd although the name/thread readers already used exact retained
state. That would deny a valid caller-owned WND after nested destruction or
bind its access decision to a replacement HWND property. Pinned
client/winmgrc.c TestWindowProcess first accepts identical owner-thread
identity; kernel/winmgr.c compares the WND owner's process. Neither makes
retained object provenance depend on a replacement property's allocation.
Kernel/getset.c FCallerOk has additional system/logon rules; this project
boundary remains explicitly limited to its local recovered-state domain.

The predicate now uses existing wow_user_borrow_client_state. Safety follows
from attach_client_state requiring an enrolled local binding, attachment's
same-process validation, and exact allocation matching by the nested borrow
list. An unpublished observation cannot attach writable state. A copied WND
does not acquire ownership merely by naming a valid HWND. This change does
not revive native handles or bypass the original caller's ValidateHwnd.
Original caller predicates and error policy remain untouched.

Extended the shared-lifetime fixture to check old allocation ownership after
detach/re-attachment, reject a copied WND carrying the current HWND, and retain
ownership/error state after nested native destruction. Existing negative tests
already reject unpublished-state attachment. These additions remain unrun.
No new registry, lifetime counter, original-source diff or guest change was
introduced. Host relationship publication is still incomplete; this fixes
one inconsistent consumer of the lifetime that publication must retain.

### Canonical native combo retirement call boundary

Inputs: staged byte-identical client/edecrare.c ECNcDestroyHandler and pinned
OpenNT client/combo.c ComboBoxWndProcWorker (WM_PARENTNOTIFY case at line1045),
plus existing standard-class import, logical exchange and native-call owners.
Source tracing confirms that EDIT first releases PED and marks its FNID
cleaned, then directly calls the combo worker with WM_DESTROY, child ID and
child HWND. The combo worker clears its editable/list references. Sending
the message to the application's current subclass would not preserve that
canonical-owner call, even if an ordinary example happened to forward it.

The original combo body cannot consume our recovered WND: its PCBOX and
child references belong to native USER for the currently native combo.
Rather than import a second combo engine or cast that private state, the
finite binding calls the immutable native standard procedure already captured
by wow_class_words_import_native. The new accessor searches that existing
public-class owner under the existing USER lock; it ignores application
classes and subsequent logical procedure exchanges. It returns a native USER
procedure identity, not a class pointer or another registration.

wow_edit_combo_destroy_notify preserves the original five-argument call
shape through the EDIT build header. It admits only the source-proven
numeric WM_PARENTNOTIFY/WM_DESTROY edge, retains the exact parent WND in the
existing borrow scope and uses the existing native-call begin/end for
execution restoration, including exceptions. CallWindowProcA handles the
captured native target (including an opaque native CPD); no CPD decoding is
needed for this call-only edge. Its ANSI flag carries no text conversion for
this numeric message. No retries, synthetic destroy messages, guest edits or
original EDIT body edits are added. This does not solve dialog CPD storage.

Supplemental unrun assertions cover the immutable standard target after
logical class exchange, exact notification arguments/result/last-error,
exception restoration and rejection before dispatch of other messages.
These are adapter/lookup cases, not proof of modern native combo cleanup.
Grouped acceptance must additionally exercise the real combo's child
destruction with an application subclass that does not forward this message,
normal combo destruction, custom/superclass cases and once-only cleanup.
Host parent-relationship production and complete EDIT graph selection remain
open; no runtime restoration or publication is claimed here.

The adjacent CPD audit did not justify removing the registration guard:
wow_class_words_register_target also rejects unresolved CPDs. Removing only
the front guard would leave the operation unsupported. No partial guard
change, fake resolver or claim that the mock resolver is a production owner
has been made.

### Relationship lifetime: durable self versus borrowed related windows

Read the complete retained ED and DLG declarations and searched all seven
EDIT units plus dlgbegin/dlgmgr/dlgend for relationship reads and stores.
ED.pwnd is a durable pointer to its own window allocation; ED.hwndParent and
listboxHwnd are handles. DLG.hwndFocusSave and hwndSysModalSave are handles,
not owning PWND edges. The old spwndFocusSave store visible in dlgmgr.c is
commented out and is not a production lifetime requirement. These facts
reject replacing the existing stable self allocation with per-call copies,
but do not establish a need for permanent parent/child reference cycles.

Reached relationship consumers have distinct required inputs:

| Original consumer | Required relation/state | Lifetime requirement |
| --- | --- | --- |
| ECGetControlBrush | Parent/owner, actual owner thread, compatibility bits | Chosen target survives the synchronous default/message call |
| ECNcDestroyHandler | Parent and initialized COMBO FNID | Parent identity survives canonical child-destruction notification |
| GetParentDialog | Parent chain, WFDIALOGWINDOW, WEFCONTROLPARENT, DFCONTROL, WFCHILD | Traversal/result remain valid in the enclosing original call |
| dlgmgr default-button path | Child's parent and COMBO FNID | Stable target across dialog callbacks |
| DefWindowProcWorker HELP/wheel/context/move | Parent/owner, desktop identity and client rectangle | Stable nodes across sends; numeric movement uses the correct parent |
| _FChildVisible | Parent chain and visibility/child style | Complete acyclic observed traversal, not just immediate parent |

The existing borrow scope can retain the allocations, but it does not yet
publish these links. A nested call must not restore stale relationship links
over a completed mutation; a native return must refresh before original code
resumes. Independent host WND storage is not retained by guest graph refs.
No pointer is allowed to survive a scope that was its only reference owner.
These are implementation constraints, not a claim that a scoped graph is
already wired or that external private flags are known.

An additional source check found that wow_window_note_native_procedure writes
cleanup.fnid before invoking the actual standard procedure. That records
which cleanup target was reached, not successful private control initialization.
Blindly copying it into host WND.fnid would bypass the original control
validation/initialization contract. Likewise a native class name cannot prove
WFDIALOGWINDOW/PDLG. The staged canonical combo call remains dependent on a
correct parent-state producer; its existence alone cannot close retirement.

No new persistent graph, reference cycle, fabricated initialized flag or
original-source edit was made by this audit. The immediate implementation
boundary is now explicit: preserve the existing durable self state, produce
and retain related observations at the execution boundary, and obtain private
state only from its actual recovered owner. Native/private-state consumers
without such an owner remain blockers, not zero-initialized observations.

### Internal CBT enrollment lock boundary

Following the actual combo-internal creation path exposed a synchronization
gap independent of missing creator facts. create_bound drops the USER domain
through wow_user_native_call_begin before native CreateWindow. Its CBT hook
and the task's ordinary creation observer then called enroll_internal without
reacquiring that domain, although class-list acquire/target publication require
it. The original guest Hook path enters a task callback first, but finishes
its enrollment after leaving that scope. Therefore neither the entry nor the
completion helper could assume all callers still held the domain lock.

ADAPTER-WOW-031 now separates the locked enrollment body from its public
boundary. Entry verifies the current runtime thread matches the supplied
owner, acquires the existing domain only when absent and restores the entry
state in finally. Completion likewise protects rollback/reference retirement
and releases its borrow even when rollback raises. No extra lock, scheduler,
task registration or guest-execution acquisition is introduced.

The native Hook forwarder explicitly drops a caller-held domain around
CallNextHookEx and restores it before completing the enrollment. Enrollment
itself is inside that try/finally, so an exception does not skip completion.
Result, last-error and downstream veto remain owned by the existing chain;
an ownership restoration failure uses the established fail-fast boundary.
Original guest Hook/thunk code and guest images are unchanged.

Supplemental native-creation assertions check downstream Hooks see no held
domain and repeat the actual dialog enrollment/rollback with held and unheld
entry states, verifying both are restored. Existing nested creation and veto
cases remain in that fixture. These changes are unbuilt/unrun and do not
prove original EDIT or internal creator provenance is restored. The complete
dependency group and normal publication gates remain required.

### Creation rollback identity and exception-unwind completion

Reviewed the new enrollment lock boundary against claim_explicit and the
outer create_bound owner, rather than treating a successful native-create
fixture as proof of all unwind paths. Two additional structural defects were
visible without running a test:

- claim_explicit stored scope.window only after creation-state allocation.
  Generic enrollment likewise stored enrollment.window only after obtaining
  its borrow. An exception after acquiring storage could leave rollback with
  HWND zero. Each transaction now records the already-matched window before
  the first step that can acquire its storage. Existing-window/no-claim paths
  still acquire no rollback ownership.
- create_bound's single finally called terminal cleanup before popping TLS,
  removing the CBT hook, releasing retained objects and restoring native-call
  ownership. Original control finalization can call out; an exception there
  skipped every later operation. Nested finally blocks now guarantee those
  later obligations are attempted in order, without catching/swallowing the
  exception or retrying the original finalizer. Entry last-error is restored
  after bookkeeping; normal result/error selection remains unchanged.

The lock/ownership changes are project adapter corrections under
ADAPTER-WOW-031/063, with no guest or original function-body change. Static
diff checks pass. The existing creation fixture's normal/nested/veto cases
remain required, and grouped acceptance still needs a deterministic terminal
exception-injection case that proves TLS pop, hook removal, references and
execution restoration followed by another creation. That exception case is
not yet wired or run at this audit point; no runtime acceptance is claimed by
source inspection. The following continuation records its subsequent wiring.

### Creation unwind acceptance wiring and local reference review

The existing `tests/observation/verify-wow-task-lifecycle.ps1` entry now accepts
`-Case native-creation -CreationUnwind`, with ProviderBuildRoot and
WorkerBuildRoot pointing to the completed production dependency graphs and
BuildRoot below build/M0-T447/S3. It must run under the owned background
supervisor, alongside the ordinary native-creation row without that switch.
Neither row has run against the staged S3 group.

The fault row replaces only the selected creation object with
`wow_creation_unwind_injection.c`, which includes the actual production
translation unit using its generated compile flags. It does not add a runtime
fault flag or alter the normal production-object selection. Native creation
and destruction finish first, then the retained creation-scope terminal leaf
raises a deterministic exception outside USER's callback exception filter.
Assertions require propagation, preserved last-error, restored execution
ownership, empty creation TLS, removal of the installed hook, exactly one
release of the retained scope borrow, and a subsequent successful creation.
The normal fixture additionally requires final object-table emptiness.
The runner rejects a missing unwind marker even if the process exits zero.
This is a host-boundary fault test, not real-guest control acceptance or proof
that every original control-finalizer exception has been exercised.

Review of the same generic-enrollment path also found a local class-reference
leak: before attachment consumes its reference, target selection/enrollment
could unwind past the sole ordinary-failure release. A finally now releases
the untransferred reference on both ordinary and exceptional exits; successful
attachment still consumes and nulls it. The temporary parent admission borrow
is likewise released in finally around class lookup/admission. No retained
parent relation, new identity registry, guest change or original-source diff
is introduced. These remain ADAPTER-WOW-031 ownership corrections. Static
review is not runtime evidence; deterministic pre-transfer failure coverage
and the complete group acceptance remain required.

### EDIT shared system inputs before private-precomp selection

The seven staged original EDIT files read exactly seven gpsi fields:
wMaxLeftOverlapChars/wMaxRightOverlapChars, cxSysFontChar/cySysFontChar,
tmSysFont, dtScroll and ucWheelScrollLines. No oemInfo or ghFontSys read was
found in those files. Selecting their source with merely WND/ED declarations
would still leave these runtime inputs without an owner.

Source trace on pinned OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c:

- kernel/init.c LW_LoadProfileInitData defaults both overlap values to3.
  public/internal/nls/inc/ntuserk.h identifies the actual value names as
  LeftOverlapChars and RightOverlapChars; they are not the C field names.
  kernel/profile.c maps PMAP_DESKTOP to the user's Control Panel/Desktop.
- kernel/init.c LW_DCInit selects SYSTEM_FONT and gets dimensions through
  GetCharDimensions, preserving its rounding rather than tmAveCharWidth.
- kernel/sysmet.c _SetDoubleClickTime sets dtLBSearch=dtTime*4, then
  dtScroll=dtLBSearch/5. Menu-show delay is not this input.
- kernel/rare.c SPI_GETWHEELSCROLLLINES returns the wheel-lines field.

ADAPTER-WOW-058 now captures these inputs before an ordinary EDIT entry,
publishes a stack-scoped TLS context for the original-client build binding,
and restores the previous context in finally. Native SYSTEM_FONT measurement
uses the already audited GdiGetCharDimensions export on an owned compatible
DC, restoring selection and deleting the DC. Capture publishes no partial
output, does not mutate system settings and preserves last-error on success.
WM_NCDESTROY/WM_FINALDESTROY and explicit finalization add no context-capture
dependency; their original cleanup body does not read these fields.

OPENNT-HOST-055 now also retains the complete FastGetProfileIntW body from
the same already registered profile.c source hash. An exact comparison after
line-ending normalization gives858 characters and identical=True. The original
Rtl integer conversion/default logic is not replaced. ADAPTER-WOW-042 admits
read-only PMAP_DESKTOP=5 in its opening leaf; task compatibility's public
entry still accepts only PMAP_COMPAT=14. There is no profile-cache or
impersonation implementation. The x86 generated import list and decorated
alias add RtlIntegerToUnicodeString alongside existing conversion imports.

Additional source hashes: init.c
7489347051F1A4F5096D5FC162CE0EAB6851510B9F1DCA0B5B799FEBC3943BD6;
sysmet.c 249E9164909EFAB7D07647ABE38BD4719A989058947623FA665F3C180957D4A2.
These are initialization evidence, not imported kernel runtime. The full
server initialization depends on display/server ownership and cannot be
composed here; this bounded input binding leaves all EDIT algorithms intact.

The existing edit-native fixture now includes context/null/error/font and
wheel/period assertions without registry writes. It remains unrun and cannot
prove guest rendering or editing. Before group acceptance, verify context
nesting and system-setting changes across native callouts, profile malformed
and missing-value behavior, and actual private-precomp selection. The original
integer reader does not check RtlUnicodeStringToInteger's status; retaining its
body is not proof of well-defined malformed-value handling. No wider
system-input timing waiver or malformed-value success is claimed. The six
existing S3 integration obligations remain open, not replaced by this input
binding or its supplemental fixture.

### EDIT native callout family wiring

Source audit of all NtUser calls in the seven original EDIT units finds the
paint pair, caret create/destroy/move/show/hide, invalidation, GetDC, ScrollDC,
cursor, capture/focus, scroll-info, system timer, FNID and menu/clipboard/beep
families. Existing menu, focus, capture, timer and control-state providers
are reused. Missing paint/scroll/caret/cursor/invalidation entries now use
ADAPTER-WOW-051's same WOW_NATIVE_CALL macros in wow_user_task_lifecycle.c;
there is no second scheduler, callback manager or returned-handle registry.
HDC/HCURSOR results are not mistakenly published as HWND roots. Begin/end
retain native arguments, result, last-error and finally restoration.

This is required by original behavior, not a speculative blanket wrapper:
kernel/paint.c xxxBeginPaint enters xxxInternalBeginPaint, which can send
WM_NCPAINT and WM_ERASEBKGND; kernel/sbctl.c xxxSetScrollBar's SB_CTL branch
calls xxxSendMessage(SBM_SETSCROLLINFO), and its nonclient branch may redraw
the frame. Direct native calls beneath a held USER domain would bypass the
existing callback/return contract. The original kernel implementations cannot
be imported without native window/display state; modern public operations
remain their admitted service owners. At pinned source revision, paint.c hash
is15C5B8F566E6AE40D9E1F69424134508A5CA380615D22C7D2743D16D89A4D50E and
sbctl.c hash isE60DD4AD4F71631CC312107BD4A3AC13FDD2CE577608E7F555AD68E2F78F44DE.

ADAPTER-WOW-061's WOW_ORIGINAL_EDIT_CLIENT build binding now maps the whole
reached NtUser family to those existing/scoped leaves. Pure DC/clipboard/beep
services bind to their public implementation; private FNID stays in the
recovered state owner, and timer ABI adaptation stays in ADAPTER-WOW-057.
CsSendMessage selects the existing exact EDIT conversion binding rather than
an unqualified native call. No original EDIT source/body or guest changes
are made in this wiring batch. Full private-precomp/body selection, related
WND state and the previously recorded CPD obligations remain open.

Acceptance is added to the existing real-CCPU mutation group in
tests/mvdm-host/wow_page_domain_fixture.c, not another stand-alone fake owner.
verify-wow-task-lifecycle.ps1's window-borrow/window-reclaim/graph-snapshot/
menu-resource rows now require WOW_CONTROL_NATIVE_RETURN_CCPU errors=0.
The row invokes actual BeginPaint/EndPaint, native and custom SB_CTL targets,
caret lifecycle, invalid HWND failure and unchanged cursor. It checks
synchronous callback lock release, return/error, scroll state, update-region
validation, caller lock restoration and the existing committed geometry view.
This exercises native services in a CCPU-backed controlled task, not original
guest EDIT instructions. The new row is unrun; complete S3 production/guest
acceptance is still required and no new release is asserted.

### Original EDIT compile closure: A/W table and expanded macro calls

Following original EditWndProcWorker's expressions, rather than just external
function names, identifies its gabThunkMessage array as an absent definition.
The existing conversion adapter supplies execution but cannot substitute for
this original classification. OPENNT-HOST-092 now retains the complete
constant definition from pinned client/clglobal.c (source hash
D8FE5A80D5848F2951843D7BA94752CB3223945A48630350835422389CD1DA6F) at the same
relative mirror path, with the original copyright header. Only scalar Windows
types replace precomp; no other USER globals, registration or process startup
is imported. Original globals.h supplies the unchanged extern declaration.
The WOW32 generator selects this data-only object with its other USER owners.

Static extraction/comparison found the retained definition identical after
CRLF normalization:26839 characters,1159 lines. Selecting current Release
branches (no FE_IME or MEMPHIS_MENUS) yields1024 boolean entries,53 true.
The source retains both original conditional families and a compile-time
WM_USER extent assertion. Original EditWndProcWorker still guards the array
with message<WM_USER and the PED encoding comparison. No custom switch or
new fast-path classification replaces that algorithm. The supplemental
conversion fixture now links this same table and asserts text/character/edit
messages versus numeric/paint/destruction and final-bound entries; unrun.

Transitive macro audit also corrects the preceding leaf-scan boundary:
the22 names counted in the seven C files do not include all expanded calls.
Original usercli.h INITCONTROLLOOKASIDE calls NtUserDestroyWindow on allocation
failure. The EDIT-only binding now maps it to the existing scoped native
DestroyWindow, so failure cleanup does not bypass the return owner. This is
not a new destruction implementation or an original macro-body edit.

The remaining private-precomp obligations are not solved by adding includes.
In particular, CBFNID comes from original class.c's registered cbwndExtra plus
sizeof(WND), not blindly sizeof(EDITWND). clinit.c registers EDIT with
CBEDITEXTRA=6, distinct from its four-byte PED slot. Its class-derived extent
must be preserved when binding the validation macro; a guessed structure size
would silently relax the original check. REBASE/Lock aliases likewise require
the existing retained host-state ownership, not native USER heap pointers.
No guessed CBFNID value or pretend relationship producer is introduced here.
Original bodies, private compile environment and complete consumer/state
closure remain unbuilt/unaccepted, with the six fixed S3 obligations intact.

#### Creation child-ID sample reuse

Source review of wow_window_client_create_state and apply_window_info found
that creation read GWL_ID twice: once during shared refresh and again before
initial WND publication. The observed-graph refresh already reused its supplied
ID; that did not cover initial creation. The existing refresh_info interface
now optionally returns the numeric ID it accepted alongside WINDOWINFO, only
on success. Creation consumes that result and removes its second native read.
No new owner, cache, guest field or original-source diff is added. This does
not claim atomic sampling of all modern USER fields or solve relationship
publication; it removes one demonstrable mixing of samples.

The existing child_id_case supplements initial/zero/high-bit ID assertions
with successful output/state agreement, last-error preservation and unchanged
outputs on invalid-HWND failure. The fixture remains unrun until the complete
S3 dependency group is linked. Static caller search finds only the existing
refresh wrapper, creation consumer and these supplemental assertions; all
use the revised interface. Review rejects native cbWndExtra as a replacement
for canonical CBFNID: the superclass/private-boundary distinction documented
above remains binding and that production table is still open.

#### Canonical client-control extent query

Follow-up implements the finite registration-data query, not a native class
registration service. Original RW_RegisterControls in client/clinit.c supplies
eight Release client-control entries; kernel/class.c adds sizeof(WND).
OPENNT-HOST-093 retains the missing exact BUTN/BUTNWND, COMBOWND,
STAT/STATWND and MDI/MDIWND declarations. Existing LBWND, CBEDITEXTRA and
DLGWINDOWEXTRA complete the expressions. The full original registration routine
cannot be called without replacing native system classes, unrelated control
procedures and USER initialization; that is the finite stopping boundary.

wow_control_registered_extent now queries these canonical expressions and
masks original lifecycle status bits. It preserves last-error on success,
rejects unknown/server FNIDs and leaves failure outputs unchanged. No native
cbWndExtra, superclass extent, fabricated SERVERINFO or second registry is
used. Original size validation and private-word policy are still their
original bodies. This supplies their data dependency, not a replacement
validation algorithm; private-precomp CBFNID mapping is still pending.

The production generator selects this data-query owner. The supplemental
dialog/getset fixture no longer invents the EDIT extent: its CBFNID wrapper
calls that owner, with all eight x86 extents, lifecycle bits, invalid/NULL
inputs and last-error checks. Its runner/link integration and execution are
still pending with the complete S3 group. No runtime pass or publication is
claimed. All imported declarations were compared against the pinned source;
there are no edits to original function bodies or guest components here.

#### Original EDIT entry message filter

Whole-entry source tracing found another compile/dispatch dependency:
EditWndProcA/W in editec.c first use FWINDOWMSG(message,FNID_EDIT) before
entering EditWndProcWorker. gabThunkMessage answers A/W conversion, not this
dispatch-membership question; substituting it would select the wrong owner.
Original kernel/server.c gawEditWndProc and client/usercli.h FWINDOWMSG now
remain unchanged under OPENNT-HOST-090. Static normalized-line comparison
reports exact equality for all1464 characters of the array, including its
duplicate entry, zero sentinel and disabled WM_NCPAINT entry. Private
WM_FINALDESTROY=0x70 and WM_SYSTIMER=0x118 are verified against original
public/internal/windows/inc/winuserp.h lines118/128.

The existing message once-owner builds the EDIT slot using original
InitWindowMsgTable alongside the two default-window bitmaps. Failure releases
all allocations before publishing anything; successful publication remains
immutable. Original FNID indexing is retained for FWINDOWMSG, with only the
EDIT slot initialized/consumed. This does not implement other controls'
dispatch or expose a modern SHAREDINFO overlay. The EDIT entry requires this
initialization before all original calls, including destruction-only entries:
otherwise a first WM_NCDESTROY after earlier entry failure could dereference
an unpublished table. Normal cleanup after creation uses the already completed
once-state, not a new per-window bitmap.

The existing message fixture adds positive EDIT/creation/destruction/timer
membership, WM_NCPAINT/WM_USER/zero/out-of-range exclusions and stable repeated
initialization checks. These assertions and allocation-failure acceptance
remain unrun; no runtime claim follows from the static table comparison.
The complete private-precomp, relationship and body-selection work still
precedes unified build and acceptance.

#### Client Lock is not a server ownership provider

Read-only tracing of pinned client/usercli.h lines326--344 resolves the
expanded INITCONTROLLOOKASIDE Lock dependency: original client CheckLock is
empty, Lock assigns and Unlock clears. Those exact definitions now remain
under the original EDIT compilation opt-in, registered with OPENNT-HOST-074.
They must not be replaced with invented server handle/reference operations.
The original macro stores PED.pwnd in the same host WND owner; current
wow_window_words_terminal selects canonical EDIT finalization before
withdrawing identity and dropping property/table references. Existing nested
borrow scopes retain that allocation through callback return. There is no
new self-reference cycle. Related parent/owner/child pointers still require
their separate publication/lifetime work and are not proved by this macro.

The dialog CPD audit remains unresolved rather than silently weakened:
rtl/getset.c returns a CPD on cross-encoding DWL_DLGPROC reads, client/winmgrc.c
normalizes a CPD back to its target and encoding on writes, and dlgmgr.c
directly invokes PDLG.lpfnDlg (and uses DLGF_ANSI for WM_NOTIFYFORMAT).
Public CallWindowProc can invoke an opaque native CPD but does not provide
the two normalization outputs. Merely replacing the direct invocation does
not prove correct get/set restoration or notify-format behavior. No such
replacement, heap decoder, temporary window or new semantic waiver was added.

#### EDIT extent consumer binding

The EDIT-only call header now maps CBFNID to wow_edit_control_extent, which
requires the canonical registration query added above. Original
EditWndProcWorker supplies literal FNID_EDIT to VALIDATECLASSANDSIZE; the
original macro still decides rejection and FNID initialization. The adapter
does not compare window capacity or change its error/return branches. An
unsupported internal FNID reaches the existing invariant-failure boundary
instead of returning zero and silently disabling original size validation.
This binds the EDIT consumer only; original getset's complete private compile
environment remains pending. The actual call-adapter fixture adds exact
six-byte extent, status-bit, last-error and invariant-rejection assertions,
using the real query owner rather than a size double. They are unrun with
the rest of S3; original EDIT body selection is still incomplete.

#### Native EndDialog uses the shared call boundary

Review of the staged dialog-finalization owner found a duplicate native
begin/EndDialog/end wrapper for dialogs not owned by recovered PDLG.
It is removed in favor of wow_native_EndDialog generated by the existing
WOW_NATIVE_CALL mechanism. Recovered dialogs still invoke original
wow_original_EndDialog with no native retry on a handled FALSE. The outer
borrow release remains in finally for both paths, including exceptions.

Important causal limit: native_call_end already fail-fasts on restoration or
publication failure in production. The old local `ended=FALSE` branch does
not prove that a production restoration failure was swallowed. This is
deduplication and contract consolidation, not an observed-fault repair claim.
No original source or guest changed. The supplemental finalization fixture
now tests native delegation failure/success/exception and retained-reference
release; the shared boundary's real native/guest acceptance remains part of
the complete S3 group. Tests are staged and unrun; diff whitespace check passes.

### Integration decision boundary: do not substitute peripheral edits

The current source review reconfirms two unresolved inputs that control
selection of the whole recovered control/dialog group:

1. Generic internal enrollment in enroll_internal_locked installs the
   recovered target but does not supply creation encoding/version/extra-byte
   facts. The retained-state constructor intentionally rejects unknown facts.
   Parent/class identity alone is not evidence of the original creator flag.
2. DWL_DLGPROC restore requires a saved CPD's actual target and encoding.
   Native invocation is available, but the production resolver is absent.
   The prior inspected mapping export alone omits validation and native
   server-to-client procedure normalization; fixed private RVAs are rejected.

These are not resolved by the added registration/message tables, client
macros or duplicate-wrapper removal. None of those changes establishes a
link-complete or publishable S3. Repeating their audits or adding peripheral
helpers must not be presented as closing these two dependencies.

Under the current stopping boundary, there is no verified provider ready
for either input. This is not a mathematical claim that no solution exists.
A broader modern-USER private-boundary runtime binding requires explicit
admission beyond the current rejection of private heap/RVA decoding; a
semantic reduction also requires separate approval. Do not silently choose
either, select the incomplete bodies or publish them over P16. Preserve the
existing worktree while requesting that architecture decision. All other
fixed S3 obligations and the full T goal remain open, not removed or passed.

### Owner admission: bounded modern-USER adaptation research

The owner's subsequent "批准啊" approves the requested bounded design
investigation. It supersedes the pending-approval stop above, not its
technical findings. Preserve the current worktree and P16 release. This is
not permission to select incomplete recovered bodies, decode guessed native
layouts, call fixed private RVAs, change guest media or relax semantics.

The investigation has two outputs, not an open-ended USER recovery project:

1. Internal creation: identify the actual producer of encoding, expected
   version, compatibility flags and class extent before the first recovered
   callback. Trace native-container/recovered-child combinations separately
   from original creators already routed through explicit creation scopes.
   An enrolled parent is not a substitute for missing creator provenance.
2. Saved procedure restoration: provide the saved target and its encoding,
   with handle type/generation/lifetime validation and native server/client
   normalization, or demonstrate an equivalent representation at the existing
   procedure owner. Invocation alone is not decoding. Do not introduce a
   second HWND registry or silently duplicate native dialog procedure state.

Compare a named native service, a source-shaped retained owner, and a minimal
registered representation adaptation before choosing one. Record rejected
routes and any observable difference. Fixed-address decoding remains excluded;
any newly discovered semantic difference is not approved by this admission.
Acceptance must include A/W saved-target restoration after replacement,
class-peer lifetime, nested callbacks, malformed/stale identities, internal
creation failure and teardown. Fixtures must exercise the chosen real provider,
not a resolver double. Implement the resulting dependency group together and
then run grouped compilation and acceptance before coherent publication.

This admission adds no source or runtime change and makes no test-pass claim.
The six finite remaining S3 groups listed in Current remain the exit boundary.

#### Creator-path discriminator for the admitted investigation

Read-only source check of pinned ntuser/client/comboini.c,
xxxCBCreateHandler, lines164--218, distinguishes two children of the same
combo: the list creation passes creation flags0, whereas the EDIT creation
explicitly derives CW_FLAGS_ANSI from the combo's WFANSICREATOR. Both pass
the combo module, and list creation precedes EDIT creation. Consequently a
generic parent-fact copy in enroll_internal_locked is not a valid substitute
for the actual producer. The native class's current procedure encoding also
does not establish that producer input.

The existing explicit claim path already captures its own encoding and facts
before constructing shared state; the generic internal path does not. The
design must distinguish an original creator entering that explicit boundary
from a native container creating children internally. Any proposal that
recovers the original container must account for its whole finite dependency
closure, not import comboini.c merely to eliminate the missing flag. This is
source evidence and a design discriminator, not a new import or runtime pass.

#### Producer audit narrows the server-procedure question

Question: does the presence of server-to-client conversion in native
CallWindowProc prove that every CPD resolver must reproduce its private
procedure arrays? No; the producer path must also be established.

Inputs: pinned OpenNT rtl/getset.c GetWindowData and kernel/getset.c
xxxSetWindowLong/GetCPD; installed SysWOW64/user32.dll SHA256
56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E.
Read-only inspection used MSVC14.43.34808 Hostx64/x86 dumpbin /disasm /range
and the existing build/M0-T447/S3/read-native-imports.ps1. The PATH dumpbin
could not find LINK.EXE; the fully qualified MSVC tool succeeded. No image was
loaded by this inspection, no private routine was called and no test ran.

Original GetWindowData and xxxSetWindowLong have mutually exclusive
server-procedure and CPD-generation branches. The server branch maps the
procedure to its client entry; CPD generation occurs in the other branch.
Original GetCPD stores its supplied dwProc32 unchanged in pfnClientPrevious.
Thus the original getter/setter sources do not support treating a kernel
procedure address as the normal CPD payload.

The installed x86 image provides a useful, narrower corroboration:

- GetWindowLongA at RVA261D0 calls helper28F80. Its dialog procedure branch
  at28FEA reads a32-bit procedure, checks the dialog encoding bit, then passes
  that procedure in EDX and options41/42 to helper7E365.
- Its GWL_WNDPROC continuation29350 has a server-procedure branch2946B--2948D
  that returns the mapped client procedure directly. The other branch maps
  EDIT at294C9 or the class-neutral procedure at2950F before CPD generation
  at29530--2953D, passing the resulting32-bit client value in EDX.
- Helper7E365 passes EDX, options and HWND to IAT100EEC28, independently
  identified as NtUserGetCPD by the import reader. The previously inspected
  general conversion helper23D70 is therefore not evidence that these getter
  paths pass a raw server address to NtUserGetCPD.

All RVAs above are inspection coordinates for this exact binary, never
runtime bindings. This changes the next design question: first establish
the accepted CPD producer domains (including class getters and setters),
then determine whether their stored payload already is the client target.
Do not require a cloned private procedure table merely because the general
consumer can map one. Conversely, these two getter paths alone do not prove
all producers or the modern kernel's stored representation. Handle type,
generation, destruction/access checks, target encoding and class lifetime
remain mandatory. The bounded native-provider design is still unproven;
no resolver implementation, production selection or release is authorized
by this evidence alone.

#### Original CPD producer sweep and cross-family acceptance

The follow-up rg sweep of the pinned ntuser C sources found all GetCPD
call sites, not just window getters. Read complete relevant branches in
client/classc.c, kernel/class.c, rtl/getset.c, kernel/getset.c and
client/ntstubs.c, plus the kernel NtUserGetCPD entry and client forwarding
wrapper. The additional original class producers have the same distinction:

| Producer | CPD key | Mapping before possible creation |
| --- | --- | --- |
| client/classc.c GCL_WNDPROC | CPD_WNDTOCLS | Server mapping or client-neutral mapping; conversion flag only in the latter path |
| kernel/class.c _GetClassInfoEx | CPD_CLASS | Server mapping or client-neutral mapping; conversion flag only in the latter path |
| kernel/class.c xxxSetClassData | CPD_CLASS | Previous server procedure is mapped directly; CPD generation belongs to the non-server branch |
| rtl/getset.c DWL_DLGPROC / client/ntstubs.c _SetWindowLong | CPD_DIALOG | Original PDLG retains the callable client procedure and its encoding |
| rtl/getset.c GetWindowData / kernel/getset.c xxxSetWindowLong | CPD_WND | Server and CPD-generation branches remain separate |

The original kernel NtUserGetCPD entry accepts window/dialog/window-to-class
options, validates its HWND through BEGINRECV_HWND, and delegates to GetCPD;
it does not itself map the procedure. Original GetCPD stores dwProc32 directly.
This completes the original-source producer enumeration. It does not establish
the modern kernel implementation or prove the modern class producers; those
must not be reported as verified from the NT4 source.

One important caller distinction is now explicit: original _SetWindowLong's
dialog branch restores bAnsi from CPD wType, but xxxSetClassData only restores
the saved procedure and subsequently updates CSF_ANSIPROC from its input
bAnsi (or server mapping). A shared resolver may expose target/encoding facts;
it must not impose one universal setter encoding policy or silently change
the retained class algorithm to look like the dialog algorithm.

The existing native ABI fixture gains check_class_cpd_dialog_restore: create
an independently authored Unicode class, obtain native tokens through both
GetClassInfoExA and GetClassLongA, restore each into a real dialog's DLGPROC,
and require the original raw Unicode target from GetWindowLongW. It also
checks actual saved-token conversion dispatch, restores the real dialog
callback before destruction, and destroys both windows before class removal.
It deliberately does not require tokens from different CPD option families
to be equal. This supplements, rather than replaces, the existing opposite-
encoding dialog restore and class-peer lifetime cases. It uses native USER,
not a decoder double or production scratch object. The fixture is staged and
unrun; no source group has been compiled or published. Diff whitespace review
passes. These cases are acceptance obligations for the eventual provider,
not evidence that the missing production resolver is already implemented.

#### Matching modern kernel symbols establish the CPD storage operation

Modern GetClassInfoExA/W forwards to NtUserGetClassInfoEx (USER32 IAT
100EE880); its procedure-generation decision is not entirely in USER32.
Read-only source inspection therefore followed that owner instead of guessing
from the public return value. No kernel module was loaded or called by this
inspection and no driver, guest or production source was changed.

The old build/M0-T445/S3/win32kfull.pdb has GUID
54af189c-3116-81b6-abc0-f98f58cd324b and does not match the installed image.
It was rejected. The matching public PDB was downloaded from Microsoft's
symbol server to build/M0-T447/S3/win32kfull.pdb using the installed image's
RSDS identity E44AE6E5F6284389F7A58878592D07B81. Restricted DNS initially
failed; the approved network retry succeeded. Input identities:

- win32kfull.sys SHA256:
  94F00AABE76A4AF3794E2134B806BF89E556B66814859C285DE62173350A5173.
- Matching PDB SHA256:
  4D7EA254ADDA641A8074F913B3989435C592986A070A8E45348040EBDAFE7799.
- GUID e44ae6e5-f628-4389-f7a5-8878592d07b8; image age1, DBI age1,
  PDB information-stream age3. The initial equality-only age check rejected
  this too strictly. Microsoft's [PDB1::OpenValidate4](https://github.com/microsoft/microsoft-pdb/blob/master/PDB/dbi/pdb.cpp)
  requires matching GUID, PDB age at least image age, and matching nonzero
  DBI age. Applying that rule accepts this PDB, not the old GUID.

The disposable read-pdb-publics.ps1 parses MSF streams and public symbol
records without executing the inspected image. Matching symbols locate
NtUserGetCPD at RVA E3830 and GetCPD at E38B8. MSVC14.43.34808 Hostx64/x64
dumpbin /disasm /range:0x1400E3830,0x1400E3B90 shows:

1. NtUserGetCPD retains its incoming target in RSI, checks the HWND and
   allowed option families, and passes that target in R8 to GetCPD.
2. GetCPD retains R8 in R14 at E38D3. Its search compares the retained
   payload's qword at+10 to R14 and word at+18 to the requested options
   (E3994--E39A2).
3. New allocation uses native type7. After class-list insertion, E3A2C--E3A3C
   stores exactly R14 at payload+10 and the options at+18. There is no
   intervening target normalization. The existing-entry and allocation-failure
   branches likewise do not rewrite the requested target.
4. _GetClassInfoEx at26CC4 retains the original split: its server branch at
   27102--27115 maps and bypasses CPD creation; the other branch maps the
   client-neutral procedure at27067, checks encoding, and passes the selected
   target at27134--27147 into GetCPD.

These offsets and symbol addresses are evidence for these pinned files only,
not runtime constants. The public symbols are research inputs, not product
dependencies. The result establishes the modern allocation/search storage
operation and this class-info producer, closing the earlier uncertainty about
an additional remapping inside GetCPD. It does not establish that every input
token belongs to those producer paths or make an unchecked desktop-heap reader
valid. Class/window setter producers, type/access/generation validation,
cross-call lifetime, malformed inputs and supported-version detection still
belong to the finite native-provider proof. No acceptance test has run and
P16 remains the published package.

#### Modern mapping and secure validation are not interchangeable

Using the same identity-matched win32kfull symbols and image, read-only
dumpbin inspection locates NtUserMapDesktopObject at26050, _MapDesktopObject
at26274, HMValidateHandleWithDescriptor at27740, and
NtUserValidateHandleSecure at1AECD0. These are inspection RVAs only.

_MapDesktopObject calls HMValidateHandleWithDescriptor with typeFF (any
nonfree type), not CPD type7. The validator checks index bounds, uniqueness
with native wildcard handling, destroyed/type flags and the native access
path. Mapping then rejects the relevant entry flag40 and searches the
process's desktop mapping before returning the mapped address. The exported
mapping call does not provide an expected-type argument or a retained object
lease to its caller. Thus calling it alone cannot justify interpreting the
result as CPD, and its pointer must never be retained in recovered PDLG.

NtUserValidateHandleSecure has a narrower conditional contract. It initializes
its result to zero, tests thread-state bit29 at1AED0D, and only calls the
underlying ValidateHandleSecure when that bit is set. This corroborates the
previous USER32 validator's conditional call rather than licensing an
unconditional Boolean precheck. A proposed resolver that always calls it and
rejects FALSE would reject ordinary valid handles when that condition is off.
No such call was added to production.

The finite provider contract now separates four operations: native mapping/
access validation; CPD type and full-entry identity checks; a call-local copy
of target and conversion facts; and consistency/retirement checks before the
copy is used. The copied original CALLPROCDATA view must not contain or retain
a native desktop-heap pointer. A current HWND procedure query cannot replace
the saved target. Neither SEH nor mapping success alone proves identity or
lifetime, and no unverified double-read scheme is claimed to establish a
linearizable snapshot. Native class lifetime and concurrent retirement still
need a proved ownership argument before selecting the resolver.

This source-level finding rules out the unconditional secure-validator route
and narrows the remaining design; it is not a runtime pass or permission to
guess modern shared-table layout on another OS build. No guest, production
source or release image changed in this inspection.

#### Immediate consumer lifetime, not a new persistent CPD lease

The next read-only comparison checks the consumer rather than assuming a
new ownership requirement. In the same pinned USER32 image, CallWindowProcA
at7E340 directly enters7D767. The CPD path calls the type7 validator at7D799.
For non-converted messages it reads the target at7D7F4--7D7FA and then invokes
the selected callable; for converted messages it passes the opaque token to
the native conversion service. This path has no separate CPD retain/release
operation spanning the application callback.

The native dialog setter is even closer to the retained original consumer:
helper27590 validates the incoming CPD at27707, reads and maps its target at
27716--27721, reads conversion flags at27723--2772B, then stores the copied
procedure and ANSI flag in dialog state at2772E--27755. There is no application
callout between validation and these copies, and no CPD pointer is stored in
the dialog. Original client/ntstubs.c _SetWindowLong has the corresponding
immediate target/flag reads and raw-procedure storage.

Design correction: the adapter does not need to invent a persistent CPD
lease spanning later dialog callbacks. That would exceed both observed
consumer shapes. The proposed resolver returns a call-local original-shaped
copy; original callers immediately consume the fields, and only the actual
procedure/encoding survive. No asynchronous use, native-pointer retention,
background refresh or per-HWND CPD cache is permitted. Calls which require
conversion continue to use native CallWindowProc with the opaque token.

This is not proof of arbitrary concurrent native class destruction safety.
The short read interval still requires validated native layout/type/access
and consistency, and the resolver must not introduce a guest callback or
release/reacquire execution ownership between validation and copying. The
earlier requirement for a long-lived native CPD ownership argument is narrowed
to this actual synchronous read interval. Do not claim a stronger atomicity
guarantee than native USER, or quietly discard a required guarantee of the
recovered original caller. No production resolver or release is selected yet.

#### Setter producers and distinct encoding policies

Read-only MSVC dumpbin disassembly of the same identity-matched win32kfull
image closes the remaining ordinary class/window setter producer trace:

- xxxSetClassData at BE8F8 calls the client-neutral mapping helper at25230
  for the non-server old procedure. Only an unchanged target with mismatched
  encoding enters BEEB9--BEED7, calling GetCPD with CPD_CLASS and the selected
  old target. The server path at BEE39 maps directly and skips CPD creation.
  Incoming CPD validation at BEEDC--BEEF7 reads the target, not wType; the
  subsequent class encoding still follows this setter's bAnsi argument.
- xxxSetWindowLong delegates negative indices to26491C. Its GWL_WNDPROC
  path at264A3D--264AB5 separates server mapping from client-neutral mapping
  and CPD_WND creation. Incoming CPD validation at264AC3--264ADF reads both
  target and wType, replacing bAnsi with the token's conversion direction.
  Thus ordinary window restoration differs from class restoration, matching
  the original owners. These addresses are inspection evidence, never runtime
  bindings; the complete pointer-setter path is not implied by this x86 case.

The supplemental native fixture adds check_setter_cpd_encoding: real public
SetWindowLongA and SetClassLongA generate the saved tokens, then restoration
checks the distinct encoding outcomes through IsWindowUnicode and A/W getter
views. It restores the actual wide class/window procedure before destruction;
it deliberately never sends text to a wide callback temporarily labelled ANSI.
The test is staged, unrun, and scheduled with the complete dependency group.
It prevents a future common resolver from incorrectly merging caller policies;
it does not implement or validate that missing resolver.

The named gSharedInfo export was also checked against the entry accessor at
4100D: shared-info pointer at+0, entry table at+8, stride at+10 (hex), native
entry type at+18 and generation at+1A. This confirms how the previously
inspected validator gets its entries on this exact x86 USER32 image, not a
portable declaration or authority to read unchecked layouts on other builds.

#### Internal combo creation: matched native producer, not blanket inheritance

The original-container alternative is not an isolated comboini import.
Pinned client/combo.c WM_KILLFOCUS (lines557--566) directly resets LBIV's
iTypeSearch and frees pszTypeSearch; combodir.c xxxCBDir reads pLBIV and calls
xxxLbDir directly. A recovered combo coupled to a native list cannot retain
those original operations by casting native list storage. Importing only the
creator would split the same PCBOX owner between incompatible implementations.
This rejects that partial composition, not eventual recovery of the full
combo/list dependency group.

For the admitted native-producer investigation, Microsoft public wuser32.pdb
was obtained from the symbol server under the exact image key
DE1417065F1A81A0A689324C2870A1C01. The existing read-pdb-publics.ps1 validates
GUID de141706-5f1a-81a0-a689-324c2870a1c0, PDB age3, DBI/image age1 against
the previously pinned SysWOW64 USER32 hash. The PDB SHA256 is
3D35E07A016D2F0DCCBACC5D2C6B21FEB360ABC5FFE54722C5792B131910AC64.
It is disposable research evidence at build/M0-T447/S3/wuser32.pdb, not a
runtime, build, source or release dependency.

Read-only dumpbin inspection of xxxCBCreateHandler at846D3 confirms:

- The list is created first at848CF with zero creator flags and the combo
  module (loaded at8486D--84872), matching the original producer distinction.
- The EDIT branch reads the combo WND byte13 bit5 at84965--8496C and passes
  the resulting Boolean in the creator-flags slot at8498F. It passes the
  combo module at84992 and edit ID1001 at84998, then calls the same creator
  at849C2. No inference from the combo's current procedure encoding is used.
- The called helper at2E5B0 tests that flags argument's bit0 at2E78B and sets
  the native ANSI-create bit at2E791. Its module-version input is resolved
  before the kernel call (2E5FC--2E61A), not after the first EDIT callback.

These are inspection offsets only. This confirms the modern producer still
distinguishes creator encoding from mutable subclass encoding. It does not
license a generic parent copy or a class-name/child-ID heuristic: other
creation callbacks can create similar children. Original createw.c also
distinguishes WFANSICREATOR from WFANSIPROC (lines319--343), so an arbitrary
IsWindowUnicode sample is not sufficient provenance. Production still needs
an exact admitted creation input or a validated native view before the first
recovered callback. No native-state reader, partial combo import, test run or
release is added by this research.

#### CPD stale-token contract: generation is not encoded

The provider proof must not require a stronger identity contract than its
input can represent. Pinned inc/user.h lines1240--1242 define CPDHANDLE_HI
as FFFF and MAKE_CPDHANDLE as MAKELONG((WORD)h, CPDHANDLE_HI). Thus the public
CPD token replaces the original handle's generation with a tag; it retains
the slot index only. rtl/wow.c ValidateHandleMacro accepts either the current
generation, zero or HMUNIQBITS (FFFF). GetCPD returns that tagged form on
both reuse and allocation paths. Modern USER's previously inspected type7
validator masks the high word to7FFF and accepts that wildcard too.

Consequences for the admitted resolver and acceptance:

- While the owning native class remains alive, a saved CPD must retain its
  original target even after source-window procedure replacement or source
  window destruction. Existing saved-CPD and class-peer cases cover this.
- Free, wrong-type or inaccessible current slots must fail native validation.
  A failed decode must not install a guessed target or reinterpret the tag
  as a function address.
- A token retained after its owning class dies has no encoded generation
  with which to distinguish a later CPD reusing the same slot. Arbitrary
  stale-after-reuse rejection is therefore not an original guarantee and
  cannot be asserted as one. Adding a second token registry would change
  representation without recovering that missing history for foreign tokens.
- Current-entry identity checks can detect changes during a bounded read;
  they do not recover historical token generation or prove ABA immunity.
  Native type/access/current-entry validation remains required. This narrows
  an overstrong earlier proof requirement, not the original lifetime contract.

No stale token is invoked as a test. A future negative case must identify the
actual free/wrong-type state it checks rather than assume a numeric token can
never be reused. This finding removes the proposed need for a separate CPD
generation registry; native-layout/access and creation-fact bindings remain
unimplemented, and no acceptance or publication result is inferred.

### Approved collaboration group: dispatch and retirement closure

Owner approves the bounded non-MVDM USER recovery direction; the private
native-layout reader is rejected. This source audit supersedes that proposed
provider, not the retained observations of native behavior. Inputs are the
read-only OpenNT client sources and current staged WOW32 bindings. Inspection
used Get-Content and rg for original definitions, calls and terminal messages;
no build, runtime test, native mutation or source import occurred in this step.
The reference tree's files were readable; a fresh git rev-parse was denied,
so this step does not independently re-certify its recorded revision.

The initial candidate control-body set is ten client units: combo.c,
comboini.c, combodir.c, lb1.c, lboxctl1.c, lboxctl2.c, lboxctl3.c,
lboxmult.c, lboxrare.c and lboxvar.c. This is a candidate set, not a claim
of complete outgoing link closure. Existing seven EDIT units remain necessary.
In particular lboxctl3.c owns xxxLbDir, xxxDlgDirListHelper and
xxxDlgDirSelectHelper; these are not reasons to import a separate directory
service or replace original directory enumeration with a new implementation.

Confirmed entry/exit requirements change the implementation order:

| Reached source | Required composition | Current gap / disposition |
| --- | --- | --- |
| lb1.c ListBoxWndProcA/W and ComboListBoxWndProcA/W | All four entries reach the same ListBoxWndProcWorker and retained LBIV owner. | Bind both standard-class identities; do not recover LISTBOX but leave COMBOLBOX native. |
| lboxctl2.c xxxLBoxCtlKeyInput, SHIFT+F8 branch | Original SetTimer installs xxxLBoxCaretBlinker; the callback directly validates HWND and reads pLBIV. | Native callback must enter the existing task/borrow/control context before the original body, including late callback/retirement checks. W32TimerFunc is a guest timer-slot callback, not this native control callback. Do not reuse its slot ABI or invent another timer manager. |
| lb1.c WM_NCDESTROY / WM_FINALDESTROY -> xxxDestroyLBox | Release original strings/items/lookaside and preserve owner-draw DELETEITEM callbacks. | Hook normal destruction and task unload to the same owner before withdrawing the retained WND. |
| lboxrare.c xxxDestroyLBox | After FNID_CLEANEDUP_BIT, directly call original ComboBoxWndProcWorker for parent WM_PARENTNOTIFY/WM_DESTROY. | Both EDIT and ListBox retirement require the recovered combo owner; the staged native-only EDIT notification seam must be superseded when selecting this group. |
| combo.c WM_NCDESTROY / WM_FINALDESTROY -> xxxCBNcDestroyHandler | Same recovered PCBOX through both terminal routes. | No fallback to modern USER interpreting the original state. |
| wow_standard_class_bindings.c cleanup table | Unloaded guest subclasses restore the recovered canonical ComboBox, ListBox and ComboListBox entries. | Current three entries still restore wow_class_call_original_native; switching creation alone would leave this mismatch. Replace them atomically with group selection. |
| lboxctl2.c xxxTrackMouse, WM_LBTRACKPOINT return 2 | Original SFI__RESETDBLCLK cancels double-click tracking. | Unavailable native operation needs a separately traced bounded provider; neither silently omit it nor use the historical numeric service selector against modern USER. |

The callback is not an EDIT system timer: original ListBox also uses ordinary
message timers for scrolling and search, plus the explicit caret callback.
The EDIT helper that rejects non-null system-timer callbacks cannot satisfy
this entire family. Retain their separate original entry contracts.

Acceptance must include SHIFT+F8 caret activity and destruction, ComboLBox as
well as standalone ListBox, owner-draw deletion reentry, internal child
creation failure and canonical restoration on task unload. These assertions
are requirements discovered from source, not passed tests. Creation/provider,
private-precomp and complete outgoing dependency audit remain open. Next
composition work must cover these entry/retirement bindings together rather
than importing only comboini.c or enabling a partially recovered class group.

#### ListBox double-click reset provider (ADAPTER-WOW-065)

Source trace resolves the tracking return-2 branch to kernel/rare.c
_ResetDblClk: its entire state operation is PtiCurrent()->pq->timeDblClk=0.
Do not import that kernel queue or reproduce double-click recognition in WOW.
Current SysWOW64/win32u.dll exports NtUserResetDblClk by name; SHA256 is
31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C.
dumpbin /exports and /disasm /range:0x100064E0,0x100064F8 show the x86
zero-argument syscall stub ending with plain ret. The matched win32kfull PDB
reader resolves NtUserResetDblClk to RVA002AC8D0 in the previously hashed
94F00AAB...5173 image. Disassembly at1402AC8D0--1402AC90C obtains current
thread information, clears a queue field and returns1; it consumes no input
arguments. These offsets and the observed service number are research only,
never runtime constants. Exact field naming follows the original semantic
owner; no native-layout reader is implemented.

The new ListBox leaf resolves only the named export from already-loaded
win32u, returns missing-module/export/native failure, and installs no hooks,
new state registry or loader search. It does not alter EDIT readiness.
Original-call wiring must enforce failure rather than discard it. The leaf
is staged, not selected or tested, with zero original-mirror diff. This closes
the provider-selection question for this operation on the inspected host,
not the runtime behavioral gate or general future export compatibility.
The complete control acceptance must exercise the tracking return-2 path;
a successful call/export lookup alone cannot prove double-click cancellation.

#### Control message membership and fixed WOW message ABI

Source comparison found a declaration hazard independent of private layouts:
original wmtbl32.c has fixed CBEC_SETCOMBOFOCUS/KILLCOMBOFOCUS entries at
0x163/0x164; wmsgcb.c and wcntl32.c compute conversions using the corresponding
symbols. The prior wow32_provider_private.h derives both from host CB_MSGMAX.
SDK10.0.22621.0 selects0x165 when _WIN32_WINNT>=0x0501, whereas original NT4
winuser.h selects0x162 for WINVER>=0x0400. Thus the expression is not a stable
original ABI. This is a source-proved conditional mismatch, not a claim that
an observed application failure has been attributed to it or that every
previous build selected the newer macro branch.

ADAPTER-WOW-066 replaces those two SDK-relative declarations with the original
evaluated values in one shared wow_control_message_abi.h carrier. The same
carrier supplies the finite original list/drag messages for retained USER
tables. Original guest code and original WOW conversion/table bodies remain
unchanged. Modern control APIs must not be assumed to share these private
numbers; the recovered-group dispatch remains an implementation prerequisite.

OPENNT-HOST-090 now also retains the complete gawListboxWndProc (86 lines)
and gawComboWndProc (84 lines). A read-only regex slice comparison with the
reference server.c reports both present and identical after CRLF/LF
normalization. No entries, duplicate values or ordering are changed. This
adds170 original source lines, zero changed original table lines. Original
server initialization maps LISTBOX and COMBOLISTBOX separately to the list
table and COMBOBOX to the combo table; the existing once-owner now follows
that mapping alongside EDIT. Allocation failure frees every allocated slot
before publication; it does not publish a partially initialized group.

Supplemental wow_default_messages_fixture assertions now cover the recovered
four slots, private tracking and focus messages, terminal messages, excluded
cross-control messages and repeated-initialization identity. They are unrun
until the complete group is ready. git diff --check passed. Neither static
table equality nor declarations prove control entry, callback, rollback or
guest acceptance; those remain open with the full S3 group.

#### Remove duplicate default-window bitmap implementation

The collaboration-context audit encountered a now-composable duplicate:
wow_private_user_compat.c retained two autonomous message arrays and a manual
merge despite the original server.c inputs now being selected. Original
client.c WowGetDefWindowProcBits has exactly those two shared bitmap inputs,
a caller-provided buffer and its length; it requires no private desktop heap,
window state or native syscall. The registered recovery ladder therefore
selects the original owner immediately rather than retain parallel logic.

OPENNT-HOST-094 retains the complete51-line function unchanged under a
separate client.c build selector. Reference client.c SHA256 is
787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472.
Its compile environment binds gSharedInfo to the existing original-table
publication, and UserAssert to the host assertion facility. The selected
Ninja object uses the existing /Gz original-x86 ABI. No guest file is changed.

The registered output callback continues to call the same bounded wrapper,
now only initializing tables, rejecting null/short spans before buffer writes,
establishing original USER16's zeroed-input precondition and invoking the
original merge. No duplicate message list, classifier or merge loop remains
in the wrapper. Buffer extent comes from the original tables, not a separate
hard-coded maximum. The retained boundary fixture adds repeated publication
from nonzero storage and trailing-byte preservation assertions. It is unrun,
as are the other staged S3 fixtures; no native/guest pass is inferred.
Read-only function-slice comparison reports the reference body present and
identical after newline normalization (51 lines); git diff --check passed.

#### Remove duplicate registered FillWindow policy

Original client/ntstubs.c FillWindow performs handle/null-DC setup and forwards
to NtUserFillWindow; the unavailable server paint/queue ownership cannot be
imported as a host runtime. ADAPTER-WOW-057/061 already binds the installed
operation and brackets CTLCOLOR reentry with the existing task/native-call
owner. The registration bridge nevertheless called ADAPTER-WOW-022's separate
public-API reconstruction of clipping, brush origins and color
fallback. This duplicate is now deleted, along with its obsolete declaration.
registered_fill_window uses wow_edit_FillWindow; its VOID callback ABI and
argument conversion remain unchanged. No guest or additional mirror change.

The existing source/ABI evidence for NtUserFillWindow applies to both callers;
this change does not introduce another system call or private-layout read.
The composition observer now names the actual shared binding and also records
the original bitmap merge core introduced above. Symbol presence is still
explicitly not runtime acceptance.

Standalone wow_fill_window_fixture and wow_window_ownership_fixture now call
the production native leaf directly: they do not initialize a WOW runtime.
All prior pixel, brush origin, short-handle, cross-process and callback-side
destruction assertions are retained. Their scope is expressly native-only;
moving them to the leaf does not prove the registered guest callback.
wow_edit_call_fixture retains scoped begin/end rejection, native failure,
null-DC, nested call and exception-unwind assertions. Real guest FillWindow
through the registration slot remains mandatory in the grouped verification.
No test has run for this replacement and the published P16 is unchanged.

#### Original collaboration state declaration

OPENNT-HOST-095 adds the unchanged original CBOX declaration and adjacent
control IDs/style/owner-draw/caret constants from usercli.h (85 lines).
The existing LBIV/COMBOWND declarations remain in place; no second state
definition or native-memory overlay is added. The selected extent-binding
translation unit enables the declaration and has compile-time x86 checks
for CBOX size76 and COMBOWND/LBWND data-pointer placement after WND. These
assertions are staged, not executed by a compiler yet. The source hash matches
the previously registered usercli.h identity.

Read-only inspection of all ten candidate bodies also bounds their gpsi
inputs to atomSysClass, cxSysFontChar, cySysFontChar, dtCaretBlink, dtLBSearch,
dtScroll and ucWheelScrollLines. Existing EDIT context already supplies font,
wheel and scroll inputs; the remaining timers must preserve original
sysmet.c's dtLBSearch=4*doubleClickTime and native caret blink preference.
comboini.c consumes only the EDIT and COMBOLISTBOX registration atoms, not
arbitrary class-name inference. These require the existing registration
owner's values before creation; zero-filled missing inputs are not allowed.

Retirement audit confirms xxxCBNcDestroyHandler destroys its list child before
freeing PCBOX so owner-draw WM_DELETEITEM callbacks run while the combo is
still alive. It then clears parent/edit references and sets CLEANEDUP. Shared
entry code cannot skip all context merely because the outer message is
terminal: destruction can synchronously enter another original control and
its owner callback. Conversely failure to acquire fresh drawing metrics must
not strand already allocated control state. This ordering must be preserved
when generalizing existing EDIT scopes; declaration availability alone does
not justify selecting the native-to-original dispatch switch.

#### OPENNT-HOST-096: selected original control-body source staging

Owner-approved recovery selects the ten client bodies listed below for source
staging, not production compilation or dispatch. The direct original MVDM
consumers are the standard-control identities in walias.c, control-message
conversion in wcntl32.c/wmsgcb.c and saved-control procedure routing through
wuwind.c/wsubcls.c. Existing original EDIT also reaches the canonical combo
retirement owner directly. Retaining only the creator would leave PCBOX/LBIV
producers and consumers split, as established above; a new editor or list
algorithm is rejected. Native private-state casting and recursive USER server
import remain rejected alternatives.

All paths below are relative to the read-only reference
windows/core/ntuser/client and retain that path under src/opennt-host. Reference
revision remains the recorded5c5b979ec08c17d3ca2eb70e8aad62d26515d01c;
the current content identity is independently pinned by each hash. Existing
notices stay unchanged. lboxctl2.c/lboxctl3.c have historical module/history
headers but no explicit copyright line; none is fabricated. Source admission is not a new grant of
redistribution rights. No guest source or binary is changed.

| File | Reference bytes | Reference SHA256 |
| --- | ---: | --- |
| combo.c | 70439 | 360E3C653CC70079F8F9D0DB42D37F5580A82881F1FFB9B1FEF954D8F1E7463D |
| comboini.c | 19722 | B54A3787073F51E0578ACF2AA52AFEC144ABDE1C367F30DDBA8855810D5C4EC8 |
| combodir.c | 5078 | 0F3AA1F93E7E6FBA011504E1090231AEF5ED2488AF98DC9AC757A90CB10925B9 |
| lb1.c | 23743 | 73DD9DFFBF8C7184CC51771E34A4ACE046EC476DE4A5B9DB3EE50FCD08C7C1A7 |
| lboxctl1.c | 33265 | 47681BF76054CC55DE0533D29A012D344D96EFCE7AAA60199857A166597B4B73 |
| lboxctl2.c | 103467 | A99C9EF3B6A7A07B219DDDA18FF2FB6A8A11C42F5378E5F77E10E336AD7D1744 |
| lboxctl3.c | 35922 | C20AEDAD6B3EC2A596F7423EBCEB0D130A56B2F978A3988C4DE0E15B49BB8989 |
| lboxmult.c | 3250 | FB3569B4DDCD34E17A3426B97F3E23C6C9371FF8F2E874A61FBD7D16B013F363 |
| lboxrare.c | 23128 | E8914F4761ADE70A1A7357B5DC8C17744CE1BC49379D03A99470F815EF85F64D |
| lboxvar.c | 6427 | B4A452B32BA7CC19CCF45BDB79EF273641E200033C72D74A27EE0F1B4A05641C |

Outgoing-boundary disposition for this source-staging cohort:

- PCBOX/LBIV algorithms, item storage, selection, layout, directory filtering,
  internal messages and mutual retirement stay in these original bodies.
- HWND identity, retained WND/relations, creation facts and FNID transitions
  use the existing owned bindings; missing relation/creation providers stay
  explicit integration gaps, never fabricated fields or native overlays.
- Native window mutations/message callouts use the existing scoped USER
  boundary. Named reset-double-click service uses ADAPTER-WOW-065; ordinary
  timers and the caret callback still require their complete scoped binding.
- Heap/lookaside, text conversion and GDI primitives reuse the admitted owners.
  TabTextOut, control-color/brush queries and help/nonclient helpers must be
  resolved to their original finite owners or exact native leaves before
  compilation; these are not permission to import a USER/GDI server.
- _CreateWindowEx must forward the original module/ANSI flags through the
  existing counted-creation boundary for both child classes. Class cleanup
  restoration must switch with creation and normal dispatch, not later.

No source-body modification is planned in these ten files. The group adds
324441 reference bytes of original source, not an equivalent quantity of
new autonomous code. The production graph stays unchanged for these bodies
until their boundary providers and private compile environment are connected.
Remaining unresolved providers prohibit a link-complete or runtime claim.

Source staging completed for all ten named files through apply_patch, without
overwriting existing destinations. A read-only whole-file comparison reports
all ten equal after CRLF/LF normalization, including their final newline.
The first import batch stopped at the missing explicit notice in lboxctl2.c;
inspection confirmed the reference header rather than silently bypassing the
check. The remaining five imports used the pinned whole-file SHA256 identity.
The final comparison covers both batches. There are10985 source lines before
the final-newline split elements; no body edits or control algorithms were
added. These new source files are deliberately not added to the link graph yet.

#### ADAPTER-WOW-067: internal control creation inputs

Read-only comparison of comboini.c's two _CreateWindowEx calls confirms that
the list child passes creator flags zero, while the EDIT child uses the
parent's immutable WFANSICREATOR. Both pass the original module, atom class,
null title and null WOW words. The existing current-procedure encoding is not
a substitute for this creation provenance. The finite binding in
wow_window_creation_binding.c preserves these inputs and the original
ntstubs.c WORD truncation of GETEXPWINVER, then delegates to the existing
counted creator. It does not add a window registry or modify the general
WOW creation entry. The source owner and alternatives are registered in
src/wow32-dll/README.md; no additional mirror-body diff is required.

verify_control_creation_inputs in wow_native_creation_binding_fixture.c now
covers both encodings, one module-version query per creation, retained version
and zero WOW words, destruction, and rejection of MDI style, title text,
non-atom class, unsupported flags, supplied WOW words and missing version
provider. Runtime class context is restored with finally. These are authored
assertions, not executed evidence. Production combo/list dispatch, private
compile environment and group lifecycle acceptance remain open; this helper
must be selected only for the audited original internal-control callers.

#### Superseded native combo destruction bridge

The earlier canonical-native-combo proposal above is superseded by the
owner-approved OPENNT-HOST-096 recovery. Source inspection of edecrare.c:396,
lboxrare.c:424 and combo.c:1045 proves both original child owners notify the
same ComboBoxWndProcWorker directly after cleanup. Its WM_PARENTNOTIFY branch
updates PCBOX's edit/list references, not native USER's private combo state.
Keeping the previously staged native bridge would split that state authority.

Removed wow_edit_combo_destroy_notify, its build-header macro redirect and
the otherwise-unused wow_class_words_original_standard accessor. No original
body is edited: ordinary linkage must resolve the existing direct calls to
combo.c. This removal is not a dispatch switch or a completed link closure.
Do not retain native fallback for the original combo state owner.

Removed the unrun mocks of the superseded bridge, including their accessor
assertions; the class exchange/cleanup scenario itself remains. FillWindow's
native-call positive, failure, exception and nested-call cases are unchanged.
This does not discharge real control lifecycle acceptance: the complete group
must exercise EDIT and list destruction, parent/subclass bypass, partial
creation rollback, owner-draw WM_DELETEITEM reentry while PCBOX remains live,
normal parent destruction and task-exit cleanup. Class selection, shared WND
relationships and canonical restoration must be completed together before
the original control group enters the production graph. No test was run and
P16 is unchanged.

#### OPENNT-HOST-097: original list tabbed-text dependency

Original lboxctl2.c:291 calls TabTextOut with Unicode item text, original tab
positions, origin zero and drawing enabled. The retained MVDM control/message
consumers reach this through the admitted list collaboration group. Audited
client/drawtext.c TabTextOut is independently composable: it retains tab
selection (including negative/right-aligned stops), text measurement, mapping
orientation, opaque rectangles and packed extent return. Its only non-public
dependencies are IsSysFontAndDefaultMode, GdiGetCharDimensions and gpsi's two
font dimensions. The latter two reuse the existing measured context/native
leaf. rtl/drawtext.c IsSysFontAndDefaultMode is an independent four-line
function; retain it too, rather than copying its decision into an adapter.

Reference is the previously pinned OpenNT tree/revision. Original Microsoft
1985-91 notices remain. Whole-reference SHA256 identities:

- client/drawtext.c: 014E568B4F4AE57D54B8AFC4502C890CD19EF1A698273561D9627B1B9C9EC10A.
- rtl/drawtext.c: D94876C3A091278D92030AFF49F37151C8F728F86F0B7D3F08C3F1F7199EA746.

The subsets keep their original relative paths and unchanged function bodies.
The compile facade maps UserGetHFONT to GetCurrentObject(OBJ_FONT),
UserGetMapMode to GetMapMode and ghFontSys to the borrowed SYSTEM_FONT;
original userrtl.h's client branch likewise selects GDI font/mapping queries.
No native font is deleted. Remaining text/DC primitives remain public GDI
calls. The source's failure warning is diagnostic-only. Entire drawtext units
are rejected because unrelated DrawText/metafile/conversion owners are not
required by this caller; a new tab-layout algorithm is rejected because this
original function is independently composable. Private USER structures and
guest changes are not needed. Grouped drawing acceptance remains unrun.

The common captured control context also now supplies dtLBSearch and
dtCaretBlink. Original sysmet.c computes dtLBSearch=4*dtDblClk and
dtScroll=dtLBSearch/5; original NtUserGetCaretBlinkTime returns dtCaretBlink.
Both times derive from current native preferences, never fixed guessed
periods or global setting changes. The native getter's zero failure rejects
capture; its INFINITE preference is retained. Existing context assertions are
extended for these values but have not run. The list timer callback still
requires its scoped entry; captured parameters alone do not connect timers.

After import, read-only function-body comparisons (CRLF/LF normalized) report
both TabTextOut and IsSysFontAndDefaultMode unchanged. The provider generator
selects distinct client/rtl text objects below obj/opennt-user, with original
UNICODE and existing /Gz control flags; no root-level object is created.
The existing native-boundary fixture now calls the actual original function
for plain, tabbed, repeated-tab and trailing-tab strings, default/custom tab
positions, drawing-versus-measurement return consistency, and invalid inputs.
Public GetTabbedTextExtentW is a supplemental comparison, not proof of guest
list drawing or pixels. TLS context is restored in finally. These assertions
remain unrun until the complete group is connected. Static git diff --check
passes; this is neither compiler nor runtime acceptance.

#### ADAPTER-WOW-068: control-color native service boundary

Source audit: combo.c:1260/1868 and lboxctl2.c:74/78 consume original
GetControlBrush/GetControlColor. client/ntstubs.c:623/641 supplies thin client
wrappers; kernel/ntstubs.c:3752/3775 validates/locks the windows and invokes
random.c's xxxGetControlBrush/xxxGetControlColor. The latter selects owner or
parent, avoids foreign-process CTLCOLOR dispatch and falls back when a brush
is invalid. A replacement SendMessage plus guessed brush is rejected. Full
USER server import is also rejected: the native named services are present.

Read-only dumpbin /exports and /disasm on SysWOW64/win32u.dll SHA256
31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C
shows NtUserGetControlBrush at RVA18C0 returning with ret0C, and
NtUserGetControlColor at RVA1E90 returning with ret10. These match the original
three/four x86 arguments. RVAs/service numbers are evidence only; runtime
uses GetProcAddress on the already loaded system module. This is a native
service binding, not a version-bound private-object reader. Export absence
fails initialization rather than silently falling back to a new algorithm.

The existing native export owner now supplies both leaves. The color scoped
edge hands off to the explicit parent, not the control's own task, and restores
native-call state on ordinary return, failure and exception. Unrun mocks cover
that recipient, result/last-error, rejected entry/return, native failure and
nested callout. These supplement, not replace, real owner-draw/guest tests.
No original mirror body is changed by this binding.

The brush leaf is not yet a production caller: its implicit receiver needs
the original parent/owner relation before handoff. Do not select it by simply
using the child's task as receiver. Original client wrapper/fixup semantics,
relation ownership and grouped native/guest acceptance remain required. The
source wrapper candidates are inspected but not imported in this step.

Supplemental native acceptance source now checks both actual services, not
only transport doubles: valid explicit-parent result/identities, child-to-
parent routing, popup-to-owner routing, unowned-popup self routing, null and
wrong-GDI-type brush fallback against DefWindowProc, and rejected destroyed
control identities without another callback. A bitmap supplies a real handle
of the wrong type; no arbitrary address or undocumented private object is
dereferenced. All HWND/HDC/brush/bitmap resources are fixture-owned and
released. The windows stay hidden. These cases are authored in
wow_edit_native_fixture.c:check_control_color and are not run yet. They do
not cover cross-process filtering or prove real guest callback scheduling;
those remain grouped acceptance obligations. No source wrapper or runtime
class selector is enabled by adding these assertions.

#### Host relationship lifetime: rejected premature wiring

Current-code inspection narrows the remaining relation-owner work. The
guest graph's copy_window_view reports full historical endpoint identities
and keeps backing through incoming edges. In contrast,
wow_window_words_release frees client_state when the binding's local reference
count reaches zero, even if release_retained_window leaves guest backing
alive. Therefore a published guest relation is not evidence that its host
PWND target remains allocated. Directly filling spwndParent/spwndOwner from
temporary borrows would leave a dangling host pointer after scope exit.

Two tempting shortcuts are rejected by concrete consumers:

- Persisting sampled WND through wow_user_borrow_scope alone is insufficient:
  leave releases each sampled binding. Original PCBOX/ED/LBIV retains PWND
  beyond that one invocation, not only during a synchronous query.
- Adding observed entries with pOwner=NULL to the task cleanup table is
  invalid. Original kernel/cleanup.c:247 dereferences its TYPE_WINDOW owner
  as THREADINFO. The native lifecycle's narrower owner-equality loops do not
  make the original cleanup consumer safe. Observations must not acquire
  guest task ownership or become cleanup targets for foreign HWNDs.

The required implementation boundary is the existing full-generation window
lifetime, not native HWND validity or a low-word-only lookup. Host retained
state must survive all incoming relations and active original aliases, while
task ownership remains separate. Retirement must withdraw identity before
final physical release, without a parent/child reference-count cycle. The
same graph refresh transaction must expose either a complete old or complete
new set of host relations; partially assigned links are not admissible.
This is the existing S3 retained-relationship obligation, not a new feature.
No relation fields, cleanup-table classification or native brush recipient
were changed on the strength of temporary borrows. The original control group
must remain unselected until this ownership closure is implemented.

#### ADAPTER-WOW-069: one generation-owned host storage slot

Implementation extends the existing mvdm_softpc_wow_window_borrow record,
not a new handle registry or original mirror. Its backing already identifies
the exact generation and survives published incoming links, active borrows
and deferred decommit. bind_window_host_storage requires a live borrowed
generation, valid pointer/disposer and an empty slot; success transfers one
storage owner. window_host_storage requires an active borrow, including a
retired-but-retained generation. No host pointer is written into guest memory.
The two host-only exports are included in the existing NTVDM build export list.

All four borrow-record disposal sites use one destructor: direct retired
release, collector reclamation, preparation rollback and quiesced domain
teardown. It clears the slot before the nonthrowing host-only disposer and
preserves the error from before that disposer. No guest/USER callout or domain
reentry is allowed. The original guest and mirrored source remain unchanged.
This carrier restores the missing allocation lifetime, not window policy.

The observed-to-managed promotion fixture now asserts invalid registration,
single assignment, no release while live with zero readers, reacquisition,
promotion/retirement retention, rejection of post-retirement registration,
one physical-release disposal, and rejection after final release. Its witness
uses static storage, not a stack address that might outlive a failed assertion.
It does not claim full incoming-edge, failed-decommit or shutdown acceptance;
those existing lifecycle cases must be extended before grouped delivery.
No fixture has run.

Consumer integration must remove reciprocal ownership, not merely increment
both references: a generation-owned WOW binding cannot also keep its own
generation alive via retired_guest_backing. Local property/table references
remain separate from observation ownership. Atomic host-relation publication,
observed host WND construction, host class lifetime and provider-unload ordering
remain required before attaching production host storage. Until then the new
slot stays unused by WOW and changes no published runtime capability.

Additional unrun assertions extend the existing failed-decommit cases for
both ordinary borrows and retained-owner release: the disposer must not run
on failed physical free or failed collector retry, must run once after the
successful retry, and must not repeat on another collection. The dead-graph
transaction case also retains the companion through rejected link changes
and successful logical withdrawal until the outstanding reader releases.
The full page-domain fixture attaches one companion to the desktop at its
last protected entry, releases its reader while keeping the generation live,
and checks exactly one disposer call after session disposal. These extend
the actual carrier cases rather than replacing them with a mock allocator.

Loader/teardown source audit: nt_bop.c loads hWOWDll through SafeLoadLibrary,
checks exports and only frees it in those pre-initialization failure branches.
The project path resolver supplies system32/WOW32.DLL. No normal hWOWDll
unload path was found. mvdm_standalone_worker_cleanup first detaches USER,
ends guest-memory service and unbinds the session, then disposes the session
(which owns page_domain_dispose). Consequently a generation disposer must
not use USER runtime/guest services even though the module code remains
loaded. wow_class_words_release only decrements its storage reference and
HeapFree's the final carrier; association removal and guest class retirement
belong to earlier owners. Consumer attachment must preserve that separation.
No additional DLL pin or changed shutdown order is justified by this audit.
Source findings are not a passing runtime teardown test.

Observed-WW consumer integration now uses acquire_generation instead of
allocating a new binding per scope. The existing backing borrow protects the
generation owner while an additional host reference is acquired. Scope exit
releases both, but does not dispose live-generation storage. Promotion looks
up that committed generation even when an earlier observing scope has ended;
it does not register a previously unknown HWND. Replacing a retained class
reference at enrollment releases the old storage reference after publication.

The two managed withdrawal owners (detach and terminal) now drop reciprocal
retained_guest_backing before dropping their property/table references when
the generation owns the binding. This happens outside publication_lock, so
generation reclamation can release its host reference without deadlock.
Ordinary non-generation-owned bindings keep their existing release behavior.
The final generation callback clears weak/backing identities before the
host-only release, including quiesced domain teardown. No task callback,
guest allocator or USER query is invoked from that final release.

The original-classifier/native-domain fixture now observes a window, ends
that scope, verifies the retained generation still owns the same WW and
reacquires it in a new scope before the existing nested/retirement checks.
These assertions remain unrun. Initially managed windows, observed host WND
construction and complete relation publication are still pending; retained
WW is not itself proof of the full original PWND collaboration contract.

Initially managed client-state attachment now extends ADAPTER-WOW-069: after
the existing owner publication, attach_client_state borrows that exact HWND
generation and transfers a separate host reference before exposing client_state.
Existing matching storage is reused; conflicting storage or a stale property
rejects attachment. Failure never retires or replaces the already committed
guest identity. Property-only native bindings have no graph owner and retain
their existing local lifetime. The temporary generation borrow is released
outside publication_lock; normal retirement uses the previously staged
reciprocal-owner release. No original-source or guest edit is needed.

The native creation fixture now asserts storage identity at creation and while
holding a backing borrow across DestroyWindow, then releases both owners.
These assertions are unrun; its grouped link must provide the actual retained
domain contract, not a publish-success-only stub. This is not evidence of
complete relationship publication, observed WND construction or runtime success.

### Client locking and fixture-owner closure

Source tracing of comboini.c xxxCBCreateHandler, combo.c notification paths
and lboxrare.c xxxLBCreate/xxxDestroyLBox reaches the client usercli.h locking
environment, not kernel HM locking. Pinned OpenNT client/usercli.h lines
330--350 defines ThreadLock variants as DBG_UNREFERENCED_LOCAL_VARIABLE,
ThreadUnlock as its argument, Lock/Unlock as pointer assignment and TL as a
dummy int structure. OPENNT-HOST-074 now retains this complete bounded block
under its existing original-client selector. A normalized complete-block
comparison with the pinned read-only source is equal. No control algorithm,
guest file, server locking implementation or replacement reference manager
is added. This closes a private-precomp declaration dependency only; these
macros do not prove that cached parent PWND remains alive across callouts.
That obligation remains with the retained-host relationship group.

The native-creation runner was checked at tests/observation/
verify-wow-task-lifecycle.ps1: it links the real CCPU session/page-domain
fixture, explicitly excluding the parent stub for native-creation. Therefore
its new backing/storage assertions can exercise the actual domain once the
whole group links. The separate wow_user_task_dialog_lifecycle_fixture has
an explicit numeric publication mock; it now pairs host-storage transfer and
disposal too, rejects duplicate or unborrowed storage installation, and clears
storage before invoking its disposer. Its retirement still forbids outstanding
borrows and has no incoming graph edges. It must not be cited as retained-graph
or guest-memory evidence. Neither fixture has been run for S3.

### Canonical collaboration-control finalization

The pinned kernel/createw.c xxxFreeWindow (lines 1877--1910) performs the
canonical FNID fallback after application WM_NCDESTROY and before identity
withdrawal, bypassing a subclass that did not clean up the built-in control.
The existing staged terminal binding covered EDIT/Dialog only. Extend
ADAPTER-WOW-059 to initialized FNID_COMBOBOX and FNID_LISTBOX through the same
task/borrow entry, renamed wow_control_finalize_bound/window. Original
combo.c and lb1.c both handle WM_FINALDESTROY; comboini.c
xxxCBNcDestroyHandler destroys the list first to deliver WM_DELETEITEM before
the combo becomes a zombie, frees its lookaside state and marks CLEANEDUP.
lboxrare.c xxxDestroyLBox releases item state and sends the original direct
combo parent notification. ComboListBox uses ListBoxWndProcWorker and thus
initializes FNID_LISTBOX, not a fabricated separate cleanup identity.

The original handlers remain unchanged and authoritative; the adapter only
selects their canonical entry and verifies CLEANEDUP. Existing rejection of
DELETED-only and unknown FNIDs, already-cleaned idempotence, original-resource
cleanup task scope and enrollment recheck after nested destruction remain.
No guest/mirror body change, extra cleanup registry or native-control fallback
is introduced. Supplemental doubles verify combo/list selector and repeated
cleanup suppression but do not prove nested original notification or actual
resource release. Those require complete original body/relationship wiring
and the grouped runtime gate, still pending. This also does not claim the
retained-generation storage alone protects every cached PCBOX/LBIV pointer.

### Collaboration entry and restore selection

Pinned client/clinit.c RW_RegisterControls selects ComboBoxWndProcW with
ComboBoxWndProcWorker, ComboListBoxWndProcW with ListBoxWndProcWorker, and
the separate ordinary ListBox entry. Preserve all A/W entry distinctions;
do not collapse ComboListBox's message bitmap into ordinary ListBox dispatch.
ADAPTER-WOW-058 now uses the existing scoped entry for these six functions,
sharing task/borrow/context/finally restoration with EDIT. The finite
wow_control_client_procedure selector is consumed by class target publication,
native-target recognition, explicit creation preservation and task cleanup.
It returns NULL outside the admitted four control class FNIDs; Dialog keeps
its existing separate entry and PDLG owner.

The sweep also found module-unload restore slots in
wow_standard_classes_bind_cleanup still pointing at native combo/list bodies.
Those three slots now select the recovered W entries. Otherwise an unloaded
guest subclass could leave a recovered control's private state paired with
the wrong native procedure. Original EDIT's intentional DefWindowProc restore
slot is retained, not generalized away. No source algorithm or original
cleanup.c body changes are needed; the class fixture now checks all A/W
targets, opposite-encoding rejection and restored original control targets.
These assertions are unrun. Production group linkage, actual subclass/module
unload and nested notification acceptance remain mandatory, not established
by the selector check or static diff validation.

### Original internal-creator inputs

comboini.c lines 164--170 and 211--217 call _CreateWindowEx with atomSysClass
ICLS_COMBOLISTBOX and ICLS_EDIT, not class strings. The previously staged
ADAPTER-WOW-067 callee had no original-client macro binding, and the shared
call context had no atomSysClass input. It now maps that call name to the
bounded existing creator and copies the already initialized lifecycle
cleanup_shared.atomSysClass into the same call snapshot. The snapshot extent
is compile-checked against that owner; runtime checks require the initialized
same-process lifecycle and both nonzero required slots before normal entry.
Original inc/user.h indices 1 and 12 are retained under OPENNT-HOST-074.
No lookup/register operation or mutable second class authority is added.

The creator continues to capture module version once, preserve the original
list flags=0 and EDIT's explicit ANSI-creator flag, and publish WND before
the first original callback. Standard-class fixture assertions now compare
both populated cleanup indices with the canonical class identity. These
assertions are unrun. A separate audit of enroll_internal_locked confirms
generic native implicit creation still has no explicit creator-encoding or
module-version provenance; it must not be treated as covered by comboini's
explicit original producer. Its closure remains in the fixed creation group.

### Shared control wheel accumulator

The control-state sweep reaches editml.c's focus reset and wheel accumulation
(2872, 3084--3111) and lb1.c's corresponding reset/accumulation (245,
277--283). Both reference gcWheelDelta. Pinned client/clglobal.c lines 26--34
owns the original process-wide int and explains the partial-wheel/focus
contract; the imported clglobal subset previously contained only message
bits, leaving this symbol unprovided. OPENNT-HOST-092 now retains the original
definition/comment in its already selected object. The project entry facade
supplies an extern declaration, avoiding another tentative definition in
each control translation unit. No new wheel algorithm, TLS owner, per-window
replacement or original reset change is introduced. Source closure does not
prove runtime focus switching or fractional-wheel behavior; those remain
part of the original-control group acceptance.

### Ordinary ListBox timer callout boundary (ADAPTER-WOW-070)

Pinned ntuser/inc/ntuser.h declares the ordinary four-argument NtUserSetTimer
contract. Reached lb1.c/lboxctl2.c consumers use ordinary window timers for
scroll, search and add-selection caret; they are not EDIT's NtUserSetSystemTimer
calls. All reached callbacks are NULL except lboxctl2.c:2184, which passes
xxxLBoxCaretBlinker. The original body at 1963--1988 validates HWND, reads
LBIV and calls xxxLBSetCaret. It does not establish the provider's modern
task/borrow scope by itself. Registering that raw address would bypass the
boundary now supplied to original WNDPROCs.

The facade now maps ordinary creation to wow_control_set_timer and deletion
to native KillTimer. The finite registration adapter preserves window/ID/
interval and NULL WM_TIMER delivery, admitting only the original non-NULL
caret callback. Its trampoline uses the existing scoped entry, refreshes an
existing WND without creating one, rejects non-list or cleaned/deleted FNIDs,
then calls the original caret routine unchanged. Native USER still owns timer
storage, replacement and delivery; no second timer registry, blink state or
guest callback conversion is added. This is an adapter boundary, not a mirror
algorithm diff. Original system timers remain on their existing named native
binding. Acceptance still requires replacement/deletion, queued-after-destroy,
callback exception/nesting and real add-selection caret behavior in the whole
group. None is claimed passed by this source change.

### Original explicit control-color wrapper (OPENNT-HOST-098)

Required original caller: lboxctl2.c's parent-color query. Source owner is
pinned client/ntstubs.c GetControlColor (641--658), retained as a whole
unchanged function under its own source selector. Normalized comparison is
equal. The existing imported ntsend.h owns BEGINCALL/ERRORTRAP/ENDCALL;
ADAPTER-WOW-068 supplies only the NtUserGetControlColor service and its
explicit-parent task handoff. Native USER retains brush validation and fallback.
Original usercli.h retail HANDLE_FIXUP=0/DBG=0 expands FIXUP_HANDLE to no
repair/diagnostic action; the compile facade retains that behavior for already
native HDC inputs, not an NT4 shared GDI-handle-table reader.

The provider generator selects control-color.obj separately, so this selector
does not remove existing GetClassWOWWords or dialog get/set bodies. The color
call fixture now invokes the original wrapper for its outer calls while
retaining native service/receiver failure and nesting doubles; the grouped
fixture link must include this original object. No fixture has run. The
implicit-recipient GetControlBrush wrapper still requires its own correctly
scoped recipient boundary and is not silently treated as this explicit path.

### Implicit brush recipient: lifetime prerequisite, not another color algorithm

The next source audit examined kernel/random.c xxxGetControlBrush (208--237).
Its selector is finite (popup owner versus non-popup parent, then self), but
its ThreadLock/ThreadUnlock are kernel operations. Pinned kernel/userk.h
896--908 links TL into PtiCurrent()->ptl and calls HMLockObject; kernel/ex.c
ThreadLockObject documents the same retained-object/thread-stack contract.
The client/usercli.h no-op macros imported for client control bodies are not
a valid substitute for this kernel subset. Do not import random.c under that
client precomp, or claim the selector alone closes GetControlBrush.

The existing generation owner also is not a complete proof: current
page_domain_window_referenced scans committed live NEXT/PARENT/CHILD/OWNER
edges, not arbitrary retained PCBOX/LBIV pointers. A later relationship commit
can remove such an edge while original code retains a local recipient across
a native callback. The host relationship group must acquire a scope borrow of
the exact selected generation before that callout, preserve it through nested
destruction/return, and release it afterward, without introducing permanent
parent/child reference cycles. Do not replace the original selection algorithm
with a new GetParent/GetWindow heuristic simply to avoid this dependency.
No random.c import, private native-object reader or unsafe lock macro mapping
is made in this audit. This identifies the concrete prerequisite for the
already-open relationship group; it does not add a new S3 workstream or imply
that the explicit GetControlColor path is unimplemented.

### Existing control message edge: exact-state retention

Review of wow_window_client_send_message showed a real unguarded edge: it
only checked PWND non-NULL, read head.h and entered native transport. Original
combo/list callers pass cached parent/control pointers, so pointer presence
does not itself add a storage borrow. ADAPTER-WOW-063 now asks the existing
wow_user_borrow_client_state owner to retain that exact allocation in the
caller's scope before sending, captures HWND once and does not dereference
the state after dispatch. Failure propagates without dispatch. No separate
scope registry, HWND policy or source-control algorithm is introduced.

The existing borrow implementation searches current/ancestor retained aliases
first and otherwise requires the live binding's client_state pointer to match.
The existing class-lifetime fixture contains detach/replacement and copied-WND
rejection cases. The message-boundary fixture now explicitly checks retention
precedes A/W transport and rejects failed retention without sending, while
preserving successful return/error behavior through simulated mutation.
These fixtures are unrun; their separate roles do not constitute a completed
real reentrant destruction test. Observed external host-WND representation and
atomic relationship publication remain open and are not bypassed here.

### Whole collaboration declarations (OPENNT-HOST-099)

The ten imported combo/list source units depend on client/usercli.h's shared
declarations, previously absent despite the CBOX/LBIV layouts being present.
Retain the pinned source's complete contiguous lines 1941--2141, from
xxxAlterHilite constants through LBCalcAllocNeeded, under
WOW_ORIGINAL_CONTROL_PROTOTYPES. This includes exact cross-file return and
parameter types, avoiding implicit-int/wrong-prototype calls or manually
reconstructed declarations. No function implementation is supplied by this
header and no unrelated dialog/menu declarations are imported with it.

Outgoing declaration types are the retained original PWND/CBOX/LBIV plus
public Win32 scalar, rectangle, point, string and DROPSTRUCT pointer types.
Original public/sdk/inc/winuser.h:4833--4841 defines DROPSTRUCT as six native
data fields (two HWNDs, format/data DWORDs, POINT and control-data DWORD).
The eventual private precomp must provide that ABI and define the original
layout selectors before enabling this block; the opt-in block is not proof
that the private precomp is complete. No compile, runtime test or new source
algorithm is claimed by declaration recovery.

### Host-control handle access (OPENNT-HOST-100)

Pinned inc/user.h:1215--1224 supplies PtoH, the retail PtoHq, HW and HWq.
These expressions require only original HEAD.h, already retained in the host
WND layout, and therefore are directly composable. Import those four exact
macro lines under WOW_ORIGINAL_CLIENT_HANDLES; the control-call facade enables
them only after the original client layout. Remove its former hand-written
HWq expression immediately rather than keeping two providers. PtoH/HW preserve
the original NULL result, unlike unconditional dereference.

This is not a general HWND conversion or guest-pointer adapter. The separate
WOW_ORIGINAL_CLIENT_DISPATCH HW macro still serves its different synchronous
dispatch-view structure and is not replaced by these WND-only definitions.
REBASE macros, actual host relationship pointers and their retention remain
separate obligations; no kernel desktop delta or guest address is introduced
to satisfy them through these handle macros. No runtime acceptance is claimed.

### Original control state selectors (OPENNT-HOST-101)

Static token inspection of the retained EDIT/combo/list files found 28
missing original WF/CBF/EF selectors. This is lexical coverage, not proof
that every reference is in an enabled branch. Preserve their definitions from
pinned inc/user.h:1695--1870, plus WFTILED/WFICONICPOPUP and the original
four tiled/popup predicates at 1928--1931. ClrWF already exists locally;
no duplicate or invented ToggleWF is added. The constants and predicates
are available only with the original host client WND layout.

These are two different input contracts: offsets 0x0C--0x0F select existing
style bytes, whereas WFMPRESENT/WFVPRESENT/WFHPRESENT/WFCPRESENT and WFFRAMEON
select private state. Native style copying alone cannot produce the latter.
Their original nonclient producer/refresh and hit-test dependencies remain
in the existing fixed nonclient group, not silently replaced by synthesized
flags. This source recovery adds 34 unchanged definition lines and no
runtime behavior; compilation and grouped acceptance remain pending.

Follow-up producer tracing identifies kernel/calcclrc.c:xxxCalcClientRect
as the owner of the four presence flags. It clears them first, handles an
iconic/empty client, computes menu geometry under a kernel menu lock, and
requires strictly greater height for a horizontal scrollbar but greater-or-
equal width for a vertical scrollbar. These are not aliases for WS_HSCROLL
and WS_VSCROLL. WFFRAMEON is separate activation/flash state (kernel/dwp.c,
focusact.c and paint.c), not merely whether this is the foreground window.

The editsl.c presence-flag optimization is inside #if 0 and is not an active
runtime obligation. Active consumers remain rtl/winmgr.c:FindNCHit (also
called by lboxctl2.c and client/clmsg.c), and editec.c:ECIsAncestorActive
for caption/frame activation. This prevents treating dead source as another
runtime gap. No kernel menu layout/locking import is authorized by this
inspection; the already-bounded nonclient group must resolve these live
consumers coherently with the actual native geometry owner.

### Public control paint/text callout closure (ADAPTER-WOW-061 extension)

Source sweep finds synchronous UpdateWindow in combo.c, lboxctl2.c,
editml.c, editec.c, dlgbegin.c and dlgmgr.c; RedrawWindow in editml.c and
lboxctl2.c; SetWindowText in combo.c and SetDlgItemText in lboxctl3.c.
The original client build is Unicode. The existing private call family
previously covered BeginPaint/invalidation and ANSI text but omitted these
public paths. Calling native USER directly could enter application code
while the enclosing recovered control still held exclusive execution.

Extend the existing WOW_NATIVE_CALL family with UpdateWindow, RedrawWindow,
SetWindowTextW and SetDlgItemTextW. Preserve public API arguments/results
and native last-error, including finally restoration; no drawing algorithm,
text conversion or paint queue is authored. The item setter's actual child
is the target of the existing target-aware handoff. Add SendMessageW's
already implemented redirect alongside SendMessageA. Original-control
selection explicitly enables this shared facade, rather than assuming the
unrelated MVDM precomp supplied WOW_USER_PRIVATE_REDIRECT. The dialog precomp
still needs its planned explicit mapping, without renaming its original
EndDialog implementation into the outward bridge.

Supplemental entrypoint: verify-wow-task-lifecycle.ps1 -Case task-lifecycle,
wow_user_task_lifecycle_fixture.c:verify_control_callouts. The test uses real
native parent/child HWNDs and production wrappers, checking UpdateWindow
and RDW_UPDATENOW actually deliver paint with exclusive ownership released,
a nested Unicode text setter during paint, ownership restored on return,
Unicode child text and a nonexistent item with no callback. Normal teardown
removes the test bindings and destroys both HWNDs. These assertions are
unrun and do not replace original-control/guest acceptance. This extension
has zero original-source body edits; private dialog CPD normalization remains
unresolved and no opaque native token is inspected as an NT4 record.

### Unicode modal-loop queue boundary (ADAPTER-WOW-051 extension)

Pinned client/dlgmgr.c:DialogBox2 calls generic PeekMessage and DispatchMessage;
the source package selects UNICODE. Existing lifecycle_message always used
PeekMessageA, and the public redirect only provided ANSI queue/dispatch
entries. The recovered modal body needs the W boundary to avoid bypassing
taskman/publication or wrongly transcoding native character input.

One lifecycle_message body now takes native queue encoding. Existing ANSI
ABI forwards FALSE; messageW forwards TRUE. Both scans use that encoding;
yield/wait, PM_NOYIELD, returned roots, error and quit semantics stay unchanged.
The runtime bridge supplies GetMessageW/PeekMessageW from the same lifecycle;
DispatchMessageW uses WOW_NATIVE_CALL_TARGET just like A. No original-source
diff, second queue or authored character converter. Original dialog precomp
must still select this facade and its original EndDialog/DefDlgProc names
coherently; added entrances alone are not modal-loop completion.

The task-lifecycle fixture posts WM_CHAR U+4E2D to a real Unicode window,
peeks without removal, retrieves and dispatches through W bindings, checks
callback permission and return restoration, then checks the empty filter.
W WM_QUIT and invalid LPMSG cases mirror existing A assertions. Tests are
unrun pending coherent group build. CPD normalization remains open: original
setters consume raw target/encoding, but opaque native tokens cannot be read
as PCALLPROCDATA. No private reader or second dialog authority was added.

### Direct ListBox geometry context (ADAPTER-WOW-058 extension)

Source trace of lboxrare.c:xxxLBSize (line 524 in the pinned source) finds
a live direct GetWindowBorders call. Its retained original implementation
reads gclBorder from wow_default_current. Previously that context was only
installed by wow_default_window_proc, so a normal control resize could reach
the border owner without a valid context. This is a missing caller binding,
not a defect in original border arithmetic.

Normal edit_entry now captures the existing WM_NCHITTEST geometry context
before publishing either TLS snapshot. It installs control and geometry
contexts together inside the same task/borrow scope and restores both in
finally. Reentrant entries each retain their own stack snapshots; teardown
still skips new metric acquisition. Existing SPI_GETBORDER remains the source
of logical border units, not host padded-frame pixels or a hardcoded constant.
The default-window binding fixture adds normal and exceptional nested calls
under an outer control geometry context. It is supplemental, unrun evidence;
original ListBox integral-height resize/reentry remains a grouped acceptance
requirement. Zero original-body diff.

The wider FindNCHit dependency trace remains bounded: rtl/winmgr.c contains
both SizeBoxHwnd and _HasCaptionIcon. SizeBoxHwnd requires retained parent
geometry and presence flags; _HasCaptionIcon additionally consumes class
identity, private small/large icon properties and generic-icon identity.
Neither is solved by GetWindowBorders or by fabricating caption/presence
flags. No kernel icon/property subsystem was imported in this change.

### Nonclient boundary decision proposed, not selected

Follow-up traces the complete outgoing boundary of FindNCHit in pinned
rtl/winmgr.c:22--255, SizeBoxHwnd:555--592 and _HasCaptionIcon:649 onward.
The first consumes actual menu/scrollbar presence, border metrics and two
helpers. SizeBoxHwnd traverses retained parents and examines caption presence;
_HasCaptionIcon reads original class identity, internal small/large icon
properties and generic-system-icon identity. A copied WINDOWINFO plus style
bits does not supply this full state. Recursive kernel menu/icon/property
recovery and host-version-bound private readers remain excluded.

A bounded alternative has been submitted to the owner: delegate only the
default nonclient hit test to native DefWindowProc, with the existing scoped
native-call boundary and HWND/point arguments. Keep original control editing,
layout, notification, dialog and default text/color algorithms. This would
align hit testing with native-owned frames but adopt native nonclient policy;
it is NOT proven equivalent to every NT4 compatibility branch. For example,
FindNCHit explicitly returns HTNOWHERE for a legacy border case; modern USER
cannot be presumed to preserve that branch. No substitute or exception is
implemented pending the owner's choice. An approval must precede runtime
selection and must be recorded as that narrow semantic boundary, not as an
original-source equivalence claim.

Window relationship audit independently confirms project_graph currently
commits copied guest relationships before updating local scalar host fields;
it does not populate host spwndParent/spwndChild/spwndOwner/spwndNext. Existing
generation-owned storage and borrow scopes are the retained storage owners,
not a complete host-link transaction. Do not insert naked HWND-to-PWND writes
after each row or create cyclic parent/child reference ownership. A complete
host-link preparation/commit and callback retention remains required in the
existing fixed relationship group regardless of the nonclient decision.

### Direct nonclient consumer and existing style ownership audit

Source inspection of client/lboxctl2.c around line 1266 identifies a second
live FindNCHit consumer besides clmsg.c's WM_NCHITTEST case. A captured
ComboBox list click outside rcClient but inside rcWindow releases capture,
calculates the hit, sends WM_NCLBUTTONDOWN and restores capture. Therefore a
decision about the default-handler branch alone is insufficient: the binding
must cover this direct call without replacing the original capture ordering.
This is source evidence, not a witnessed dropdown interaction.

The existing src/ntw16s-dll/win16_style.c WM_NCHITTEST case uses its own layout
and native scrollbar rectangles; its custom menu deliberately returns
HTBORDER for separate menu handling. Calling native DefWindowProc directly
does not traverse that subclass. Consequently the proposed native delegation
must not be described as automatically preserving styled hit testing. The
normal subclass entry and the direct original control call require distinct
call-chain review before selection. No recursive SendMessage substitute or
new style callback API is admitted by this audit. Owner approval of the
nonclient semantic boundary remains pending.

### Copied WND child/menu role consistency

The staged page-domain change applies the original user.h TestwndChild
classification to copied style values: both WS_CHILD and WS_POPUP participate
in the comparison. wow_user_object_bindings.c already captures numeric IDs
with this comparison. Six page-domain consumers previously tested WS_CHILD
alone: field publication, native ID sampling, menu incoming-reference scan,
window-menu assignment, window retirement and observation export. Their
shared predicate now agrees with the producer; WS_CHILD|WS_POPUP is not
treated as a numeric child ID. No original source body is changed.

Review of verify_native_slot_reuse in wow_page_domain_fixture.c confirms its
new synthetic observation case checks a mixed-style row cannot overwrite the
menu slot with 0xfedcba98, permits clearing the menu and exports no control ID.
It restores the original copied style before native handle-reuse checks. This
does not mutate native window styles and does not prove a real mixed-style
window lifecycle. In particular the zero-menu assertion alone does not cover
nonzero menu-reference retirement; that coverage remains required before
claiming this six-consumer sweep verified. All added assertions are unrun.

The existing synthetic guest-menu graph case now also applies the mixed
style to its carrier WND with a nonempty menu: assignment must succeed,
retirement must refuse with ERROR_BUSY and preserve the edge, explicit
detach must clear it, and reattachment must succeed. It restores the original
style before the existing ordinary-menu cases. This supplements the zero-menu
observation test with incoming-reference checks; it is still unrun and does
not establish native mixed-style creation or window-retirement coverage.

### Observation module identity uses its sample (ADAPTER-WOW-063)

capture_window already records GWL_HINSTANCE in each window_observation, but
wow_window_words_refresh_observed discarded it and apply_window_info queried
GWL_HINSTANCE again. Host WW/WND could expose a different module sample than
the copied graph while retaining earlier rectangles/styles. The existing
internal refresh interface now carries the captured DWORD module beside the
numeric child ID. The observed path no longer re-queries it; ordinary
refresh_info still queries native state. No original source or guest changes.
Stored-name acquisition and host relationship transactions remain unfinished.

wow_edit_state_fixture supplies a distinct synthetic module and zero, checks
both WW.hInstance and WND.hModule, then verifies ordinary ensure_state restores
the actual native module. All fixture callers and the sole production caller
use the updated internal signature. Assertions are unrun pending group build.

### Control identity and durable relationship retention audit

Original client/usercli.h VALIDATECLASSANDSIZE sets the actual WND.fnid on
first valid control entry, before INITCONTROLLOOKASIDE allocates private
state. Allocation failure sets FNID_CLEANEDUP_BIT before DestroyWindow.
The retained kernel/ntstubs.c NtUserSetWindowFNID owns this transition;
wow_edit_state_bindings.h maps that original body to the local state receiver.
No independent initialization flag or assignment from cleanup.fnid is needed.
The current terminal selector uses GETFNID(client_state), not dispatch class
identity, and the finalizer rejects unsupported identities and skips cleaned
state. These are source-wired facts, not proof of original-body selection.

The remaining relationship dependency includes durable control-private edges,
not only WND.spwndParent/Child/Owner/Next. comboini.c records CBOX.spwndParent
at line 89, spwndList at 171 and spwndEdit at 218; its destruction clears them
at 393/397/404. Its self-EDIT case deliberately avoids a lock (the original
comment calls out a catch-22). lboxrare.c records LBIV.spwndParent as desktop
or actual parent at lines 83--85. These pointers survive the creation call.
Original client Lock/Unlock are assignment/clear and ThreadLock is a no-op;
restoring those macros is not a host-allocation lifetime implementation.

Current borrow_client_state first searches exact retained scope allocations,
then acquires a live property and checks pointer equality. This prevents
selecting a replacement HWND generation, but cannot rescue an already freed
cached pointer: its callers read state->head.h before acquiring that borrow.
The page-domain collector checks references, retained owner and live copied
WND graph edges, not CBOX/LBIV private pointers. Therefore publishing naked
host relationship pointers and relying on later SendMessage borrowing is
explicitly rejected. This is a prerequisite within the existing relationship
group, not new functionality or a reason to import the kernel lock manager.

Required lifetime witnesses for that group are now explicit: a notification
destroys/recreates its recipient before the original caller resumes; a list
or EDIT is destroyed before combo finalization; failed internal child creation
unwinds prior children; self-EDIT dropdown-list cleanup releases no self-cycle;
and no old pointer resolves to a reused HWND. The implementation must account
for durable private edges as well as temporary callback borrows, without
permanent parent/child reference cycles. No such implementation or runtime
pass is claimed by this audit.

Follow-up on the pinned original createw.c:xxxFreeWindow (1817 onward)
establishes destruction ordering: destroy children, deliver WM_NCDESTROY,
invoke the canonical WM_FINALDESTROY fallback if not cleaned, then set the
deleted state. Client lboxrare.c:xxxDestroyLBox first releases LBIV, marks
FNID_CLEANEDUP_BIT, and then directly notifies ComboBoxWndProcWorker using
the still-retained parent WND. combo.c:WM_PARENTNOTIFY clears spwndList by
plain assignment or changes spwndEdit to the combo itself, also by plain
assignment. Those writes do not call Unlock/Lock.

Consequently redefining only the original client Lock/Unlock macros to
increment/decrement host references is NOT a complete solution: the ordinary
parent-notification path would bypass reference release, while the self-EDIT
transition would change ownership without acquisition. It would require
additional original-body changes and must not be selected as a supposedly
zero-diff lifetime repair. The recovered original child notification must run
before host relationship withdrawal; retained WND storage must cover that
notification even though the child's private LBIV has already been freed.
The existing exact-generation owner remains the required storage boundary.
These observations narrow the design choices; no lock-manager replacement,
permanent retention or guest modification has been installed.

### Complete menu-role style sweep

The source-wide WS_CHILD sweep of wow32-dll/source finds four additional
menu-role consumers using the bare bit: wow_window_creation_menu,
wow_menu_native_refresh_window and both local/observed loops of
wow_menu_client_refresh_all. They now use the same full type comparison as
capture_window and page-domain. Pinned createw.c first establishes fChild
with TestwndChild, then uses !fChild for class-menu loading (around line 635)
and TestwndChild for menu-versus-numeric-ID storage (around line 659).
This repairs the existing adaptation of that policy; it is not new routing,
a different menu owner or an imported-source diff.

wow_creation_menu_fixture adds native WS_CHILD|WS_POPUP class-menu creation
and WM_NCCREATE rejection cases. Existing assertions require exactly one
resource load, a real associated menu, no premature sizing callbacks and
menu destruction on normal/failure cleanup. The test's role checks are also
updated to the full type comparison. These cases remain unrun; modern USER's
actual mixed-style normalization and cleanup must be checked by grouped
acceptance, not inferred from original source. The source sweep now leaves
no bare WS_CHILD menu-role test in wow32-dll/source. git diff --check passes.

### Public control extra words and initialized ListBox styles

The dialog-only public get/set selector bypassed the recovered original
rtl/getset.c and client/ntstubs.c for other recovered controls. As a result,
control class-extra accesses still selected modern USER storage, and the
original initialized-ListBox LBS_OWNERDRAW*/LBS_HASSTRINGS protection was
unreachable through public SetWindowLongA. Original bodies already contain
both policies; a second adapter implementation is unnecessary.

ADAPTER-WOW-063 now uses common try_get/try_set selectors for any existing
recovered WND. Nonnegative indices enter the original owners; GWL_STYLE
writes also enter the original client setter. Other negative indices retain
their existing logical WNDPROC/WOW-word/native owners. Missing recovered
state still falls through without allocation or last-error changes. Original
handled zero/errors never trigger a native second attempt. There is no new
original-body diff. The existing original getset fixture now exercises the
common selector for initialized, uninitialized, foreign-process and absent
ListBox state and ordinary class-extra reads. This fixture uses mocked CPD
validation and cannot prove native token normalization. All changes are unrun.

### Nonclient boundary approval

Owner reply approves the previously proposed native ordinary-frame/custom
NTW16S-frame split and the explicit NT4 border compatibility limitation.
Earlier pending-decision paragraphs above are historical. Implementation is
not yet selected or tested. Both original FindNCHit call sites, custom style
ordering and native-call execution restoration must be covered together.

### Approved nonclient binding implementation (ADAPTER-WOW-063)

The default/control compile facade maps FindNCHit to one scoped native
boundary. It retains the exact host WND allocation, snapshots HWND before
leaving WOW execution, queries NTW16S's existing geometry if attached, and
otherwise calls native DefWindowProcW for WM_NCHITTEST. The input is the
original packed signed screen point. Finally restores execution even after an
exception; restoration failure is terminal under the existing return contract.
No SendMessage recursion, native private-layout reader or guest edit is added.

The original FindNCHit translation unit cannot compose from the admitted
public observation alone because actual-presence/icon state is missing;
the smallest original-body facade cannot supply those facts without the
excluded private reader/kernel producers. No mirror-body patch solves that
missing state. The owner's explicit semantic exception therefore selects
native ordinary-frame policy, not an equivalence claim. Other clmsg/control
algorithms remain original. Future restoration requires an admitted complete
state producer, not guessed flags. This accounts for the recovery ladder.

NTW16S exposes win16_style_hit_test as an additive ABI-2 export. Its subclass
now calls that same function; the existing scrollbar rectangles, custom menu
HTBORDER handling and frame geometry are preserved, not copied into WOW32.
It rejects a different GUI thread and leaves output untouched when unhandled.
WOW32's existing module loader requires the export when styling is enabled;
an older DLL lacking it fails loading explicitly. Style-off adds no DLL load.
Both NTW16S architectures and their consumers require coherent rebuilt release
verification; this source change is not permission for mixed publication.

wow_default_native_fixture adds a real unstyled HWND comparison, mocked styled
selection, invalid/denied entry cases and exception restoration. ntw16s_dll
adds a real DLL query/subclass grid comparison, wrong-thread rejection and
post-removal unhandled behavior. These are unrun supplemental checks; real
original ComboBox capture/scroll/close behavior remains required. Source macro
wiring is staged, not proof of final private-precomp selection or link closure.

### Shared getset placement and dialog invocation constraint

The common recovered-window try_get/try_set bodies now live beside the
private storage receiver in wow_window_client_access_bindings.c, with their
declarations in wow_window_client_state_bindings.h. The dialog state binding
retains only its original SetDialogPointer publication. This avoids making
non-dialog control storage depend on a dialog-specific selector. Source scan
of the admitted original control/dialog files finds their live internal
extra-word write uses NtUserSetWindowLong; the existing facade selects the
same original server receiver. No speculative Unicode public API family is
added merely because the original USER island is compiled Unicode.

The CPD closure must also cover actual invocation, not only get/set. Original
client/dlgmgr.c calls PDLG(pwnd)->lpfnDlg directly at line 491, while
client/clmsg.c:CallWindowProcAorW validates a tagged CPD, extracts its raw
target for unconverted messages, or delegates converted messages to the
server. Thus placing a modern opaque token in lpfnDlg and assuming native
CallWindowProc will eventually see it is incorrect: this original dialog
call does not traverse CallWindowProc. Existing mocked getset tests cannot
prove that path. The original WM_NOTIFYFORMAT branch also consumes DLGF_ANSI.
Normalization/invocation/encoding must be resolved together before selecting
the original dialog body; no opaque token cast or raw function call has been
introduced to bypass this requirement.

### Guest dialog subclass round-trip reaches the CPD boundary

Read-only source tracing of the current mirror confirms this is not merely
a native-extension edge case. In wow32/wuwind.c, the DWL_DLGPROC getter
returns a stored guest callback when present, but otherwise obtains the
host value and passes it to GetThunkWindowProc. The setter calls
IsThunkWindowProc first and restores that recovered host value through
SetWindowLong(DWL_DLGPROC). wsubcls.c retains the supplied lpfn32 in its
TWPLIST; constructing a guest-callable thunk does not decode a host CPD.
Thus a Win16 caller can save and restore an opaque host conversion token.
Restricting the binding to W32DialogFunc or assuming all incoming values
are raw code addresses would narrow existing original WOW behavior.

Pinned original kernel/getset.c:GetCPD (lines742--826) derives pcls for both
CPD_WND and CPD_DIALOG, searches its spcpdFirst and allocates/links a native
TYPE_CALLPROC object containing the original procedure and conversion type.
Its outgoing closure includes the native handle allocator, desktop/thread
ownership and class-held object locks; importing the function alone cannot
produce a token accepted by modern CallWindowProc. This rejects importing
that kernel allocator as a purported small dialog adapter. The existing
native GetCPD call can produce an opaque token but does not provide its
inverse normalization interface.

Required group acceptance now explicitly includes the original WOW
get/thunk/set round-trip, not only direct native fixture writes: saved
same-encoding callback, opposite-encoding callback, source-window destruction
with a live class peer, and subsequent invocation/WM_NOTIFYFORMAT. Invalid
current token validation must fail without publishing a guessed procedure.
These are unexecuted acceptance requirements, not passing evidence. The
previously recorded class-lifetime and wildcard-generation limitations still
apply. No private native-layout reader, second dialog owner or guest change
is introduced by this audit.

### Creation version input: restore original null-module rule

The retrospective creation-chain review found that
wow_control_create_window_ex called get_version(instance) directly, unlike
the explicit WOW and recovered-control creation entries. Original
client/ntstubs.c:1401 truncates GETEXPWINVER(hModule) to WORD, and retained
client/usercli.h:67 defines GETEXPWINVER to substitute GetModuleHandle(NULL)
when the supplied module is null. The control binding now preserves both
rules, still querying once before native creation and retaining the original
module argument for actual creation. No parent-version inference or duplicate
version state is added. ADAPTER-WOW-067 registers the corrected boundary;
additional original mirror/guest diff is zero.

wow_native_creation_binding_fixture retains its existing explicit-module A/W
creation/retirement checks and adds a null-module version-query fault case:
the callback must receive the executable module, run exactly once, and raise
before enrollment without replacing the active runtime/class context. Its
synthetic class module means this case is not evidence of successful native
null-module class creation. All added assertions remain unrun until grouped
selection/build acceptance; source inspection and diff checks are not runtime
proof. Owner confirms original S3 scope and the retrospective audit/remediation
are cumulative requirements of this same active S.

### Retrospective projection audit: commit ordering is not yet a group commit

Current source path is wow_user_object_bindings.c:project_graph ->
mvdm_softpc_wow_page_domain_project_window_graph ->
wow_window_words_refresh_observed -> apply_window_info. The page-domain
prepares backings/classes and validates all links before replacing the guest
view. However, the subsequent local-WND loop captures stored names with
HeapAlloc/InternalGetWindowText and checks the HWND property separately for
each row. A name allocation or identity failure in row N therefore occurs
after the guest view and earlier local rows have already changed. This is a
source-proven staged S3 integration defect, not proof of a reproduced P16
failure. Its callers' terminal post-native failure policy prevents unsafe
continuation there, but does not make this a rollback-safe publication for
all project_graph callers. Do not classify it as an external freshness waiver.

The correction belongs to the fixed relationship/lifetime group: prepare all
fallible local name buffers and exact binding references before the existing
page-domain commit; validate the corresponding generations under the existing
execution owner; publish local fields without another native query/allocation
or independently failing per-row refresh. Failure before commit must release
only scratch/borrows and leave both views unchanged. Commit must transfer
prepared buffers once, preserving original FNID, flags and behavior-private
control state. Scratch is not another retained registry. The same group must
include relationship targets and their destruction/reentry lifetime, rather
than declaring scalar-only preparation to be complete atomic publication.

The existing name-reader boundary in wow_default_window_bindings.c protects
only original WM_GETTEXT/WM_GETTEXTLENGTH conversion/copy; those branches do
not call out. Keep its reader lifetime when swapping/freeing names. Do not
hold the name/publication locks across native callbacks or use a name lock
as a substitute for the existing execution owner. Full lock-order and cached
PWND retirement review remains required before implementation selection.

Acceptance must inject failure after at least one prepared local row but
before commit and compare both previous projections, then cover successful
multi-row update, nested destruction and same-slot replacement. Existing
page-domain-only rollback tests cannot prove the local-WND half. No new test
run or runtime repair is claimed by this source finding; it remains a finite
S3 blocker under inventory row HWND discovery/mutation/publication.

The scalar/name portion now stages one bulk call through the existing
window-words owner. It captures names and retains exact bindings before
acquiring publication_lock, validates every local property/state identity,
then calls the page-domain transaction and swaps only prepared fields. The
individual and bulk paths share commit_window_info; no second field policy
is retained. Finally releases the publication lock before freeing buffers
or releasing retained bindings, including exceptional preparation exit.
Source review of page-domain commit/reclamation and wow_class_words_release
found no callback/publication-lock reacquisition in that called closure;
this is source evidence, not a measured deadlock test.

wow_edit_state_fixture adds two unrun negative checks: a second invalid local
identity after the first name is prepared, and an invalid graph descriptor.
Both require the existing local rectangle, stored-name pointer, module and
WW style to remain unchanged. A successful multi-row CCPU commit and complete
relationship/cached-PWND retirement are still open. This correction must not
be reported as completion of the entire relationship/lifetime group. No
mirror or guest source changed; ADAPTER-WOW-063 records the implementation.

The page-domain fixture now also stages verify_local_observation_commit:
two real native HWNDs with enrolled WW storage, a captured complete graph,
an invalid second descriptor requiring both WW/guest styles to stay unchanged,
then a successful synthetic two-row style/module commit and last-error
preservation. It releases explicit borrows, detaches/destroys the test windows
and republishes the desktop. This uses the actual CCPU page-domain and bulk
provider, not a mocked graph result. It remains unrun; its synthetic sample
does not assert native style mutation or complete host-PWND relationships.
The separate local-state fixture covers name-buffer preparation failure.

### Original control references include shared private payload

The lifetime sweep must cover more than WND relation fields. Original
lboxrare.c:xxxLBCreate (lines181--191) stores the combo's PCBOX directly in
LBIV.pcbox for LBS_COMBOBOX. lboxctl2.c:xxxLBoxCtlKeyInput and other list paths
subsequently consume this shared private payload. It is neither an HWND nor
a second independently owned combo state. Do not clone PCBOX into a list
adapter or assume retaining only the parent WND keeps a freed PCBOX valid.

Original comboini.c:xxxCBNcDestroyHandler destroys its list before freeing
PCBOX, so list deletion callbacks still run with that shared owner present.
lboxrare.c:xxxDestroyLBox frees LBIV and marks its control cleaned before
notifying the parent combo directly through ComboBoxWndProcWorker. These
orders must remain intact through native destruction and the finalization
fallback. Early child destruction is a distinct case: combo.c WM_PARENTNOTIFY
rewrites spwndEdit to self or spwndList to null without calling Unlock.

Consequently a blanket ref-counting replacement of client usercli.h Lock /
Unlock is not a proved fix: the original client macros are assignments,
self references are intentionally not locked, and direct notification writes
would bypass such accounting. Preserve the original algorithms; any retained
reference adapter needs an audited complete producer/clear/finalization path.
The group witness must include owner-draw deletion reentry, early list/edit
destruction, parent destruction and failed combo child creation, with no
post-free PCBOX/LBIV access. This refines the existing lifecycle blocker;
it adds no new feature, source import, runtime patch or passing test claim.

### Dialog partial-creation retirement correction

Source trace: retained usercli.h VALIDATECLASSANDSIZE sets FNID_DIALOG before
dlgmgr.c:ValidateDialogPwnd initializes/allocates DialogLookaside. Either
allocation can fail with FNID present but WFDIALOGWINDOW clear and PDLG null.
Pinned kernel/createw.c:xxxFreeWindow lines1883--1914 performs the canonical
final callback and then marks FNID_CLEANEDUP_BIT regardless of its return.
The staged dialog adapter instead rejected the resource-empty state and its
terminal caller would fail the worker. The former fixture asserted that
rejection; it was not evidence of the original failure contract.

The existing terminal adapter now accepts only that consistent empty state
(valid dialog extent/type, not deleted), applies the original final cleanup
mark and makes repeated cleanup idempotent. It does not invoke a creator or
allocate PDLG. Finalization also skips new lookaside initialization: allocated
PDLG already required it, and an empty dialog needs none. Nonempty dialogs
still run the original final handler. Flag/pointer mismatch and unclean
deleted state still fail. This is original kernel-owner policy bound in the
existing adapter, not an import of the complete server destruction graph.

ADAPTER-WOW-063 registers the correction. The supplemental finalization
fixture replaces its erroneous empty-state rejection with no-allocation,
idempotence and last-error assertions, and adds both mismatch directions and
deleted-state negatives. It remains mocked/unrun; real original allocation
failure and guest teardown belong to grouped acceptance. Additional mirror
and guest diff is zero. Scope is the existing dialog rollback/finalization
row, not a new feature or semantic waiver.

The adjacent EDIT/ComboBox/ListBox check has a different original outcome:
usercli.h INITCONTROLLOOKASIDE marks FNID_CLEANEDUP_BIT before destroying the
window on both pool-initialization and payload-allocation failure. Their
existing canonical finalizer already accepts that bit without dereferencing
the private pointer, including the retained poison-pointer fixture. Do not
copy the new empty-dialog branch into these controls; original initialization
already owns the failure marker. This is a source-review disposition, not a
new passing failure-injection run.

### Shared style setter outgoing service still needs selection

The common extra-word/style entry now reaches original _SetWindowLong and
its initialized-ListBox guard, but kernel/getset.c:xxxSetWindowLong routes
negative indices to xxxSetWindowData. That production provider is not yet
selected; the dialog-getset fixture supplies a double. Thus that fixture
cannot establish actual native style mutation or return publication.
Pinned kernel/getset.c:xxxSetWindowData dispatches GWL_STYLE/EXSTYLE to
classchg.c:xxxSetWindowStyle. Its closure contains private DC-cache/desktop
visibility ownership and WFCPRESENT, as well as observable version-gated
WM_STYLECHANGING/CHANGED behavior. A no-op setter or writing only the copied
WND would create a competing authority; blindly importing the kernel closure
or treating a native setter as proved NT4 equivalence is also invalid.

This is part of the existing private-precomp/service closure and shared
getset audit row. Preserve the original client guard; select the native
resource mutation boundary only after accounting for its message/version
behavior and post-call snapshot. The approved nonclient hit-test exception
does not silently waive style-change notification differences. No new setter,
source import, semantic waiver or publication is introduced by this trace.

### Style-service consumer and notification audit

Read-only follow-up inspected the retained client/kernel getset bodies,
wow_user_private_access.c, wow_window_client_access_bindings.c and the pinned
OpenNT kernel/classchg.c. The reached negative-index closure is bounded:
wow_window_client_try_set_long admits GWL_STYLE only; GWL_EXSTYLE, ID,
HINSTANCE, USERDATA and HWNDPARENT retain their existing native-resource
routes, while WNDPROC and WOW words retain their existing identity owners.
Original dlgmgr.c also reaches NtUserSetWindowLong for DWL_MSGRESULT, which
the retained xxxSetWindowLong handles before its negative-index branch.
The control initialization macro's index zero is another positive private
word, not permission to import all xxxSetWindowData server branches.

The notification premise is now resolved by source, not by a host test:
wmtbl32.c maps WM_STYLECHANGING/WM_STYLECHANGED to WM32UNDOCUMENTED;
wmtbl32.h maps that to WM32NoThunking in retail. In W32Win16WndProc
(wmdisp32.c), this skips parameter conversion, not delivery: the message and
raw lParam have already been copied into Parm16, and execution still reaches
CallBack16. The debug WM32Undocumented also copies the parameters and returns
TRUE. Consequently neither build supplies a WOW-side notification filter.
Original classchg.c instead suppresses both sends unless WFWIN40COMPAT is
set, explicitly documenting the Quicken 2.0 compatibility reason. Merely
binding xxxSetWindowData to native SetWindowLong is not an equivalence proof.

The resource service must not re-enter wow_user_set_window_longA: that would
repeat the common selector and original client guard recursively. The existing
wow_native_SetWindowLongA is the nonrecursive execution/return boundary, but
its native notification behavior still needs resolution. Do not add a second
snapshot publisher or commit the proposed style into copied WND before native
acceptance. Host mutation and its reentrant callbacks can change or destroy
the target; the existing native-call return owner remains responsible for
publication and terminal adaptation failures.

Finite acceptance for this existing getset row now explicitly includes:
initialized ListBox owner-draw/string-bit preservation; old/new compatibility
notification eligibility and order; handler modification of proposed styles;
nested style writes and destruction during notification; native old-value and
last-error propagation; and post-return agreement of actual HWND, WW and copied
WND. Tests must distinguish a mock service assertion from real native mutation.
The private WFCPRESENT/minimize compatibility dependency remains separately
unresolved; ordinary nonclient-hit-test approval does not settle it. This audit
adds no source import, mirror diff, guest edit, semantic concession or runtime
pass, and does not add a seventh remediation group.

### Behavior-private dialog color state versus native resource flags

The full control-state consumer sweep finds an additional caller beyond the
four EDIT style families audited above: original dlgmgr.c's WM_CTLCOLOR
family temporarily sets WFWIN40COMPAT for DF3DLOOK, calls the retained
DefWindowProcWorker, then clears the bit. Original clmsg.c reads that same
flag to select COLOR_3DFACE versus COLOR_WINDOW for button/static/dialog
backgrounds. This is an original behavior-private input, not an instruction
to change modern USER's internal compatibility state. The current blanket
change_window_state forwarding wrongly sent the NT4 packed flag to the
native service, whose earlier ABI evidence covered only four EDIT flags.

ADAPTER-WOW-061 now handles exactly this flag through the existing borrowed
host WND, using original SetWF/ClrWF macros. It does not initialize missing
state, change the underlying HWND, acquire new native resources or refresh
the observation. Original client TestWF guards and dialog set/call/clear
ordering remain unchanged. Original private fields are preserved by the
existing scalar observation writer; no second compatibility cache is added.
The original client and dialog/default bodies remain unmodified. Direct
kernel random.c reuse is unnecessary: it mutates the historical server WND,
whereas the same-shaped service here must select the retained behavior WND.
Modern native compatibility flags are neither read nor written by this case.

The existing call fixture now stages the real original client guards plus
the corrected adapter, with native readiness/refresh deliberately disabled:
set, repeated set, clear, repeated clear, last-error preservation and missing
state rejection must make zero native state/refresh calls. This is unrun
mock boundary coverage, not proof of dialog rendering or full group delivery.
Grouped acceptance still needs real original DF3DLOOK color behavior.

The adjacent sweep also finds comboini.c's CBFHASSTRINGS, combined
scroll/border flags and WEFEDGEMASK, plus lb1.c's NtUserAlterWindowStyle for
multicolumn scroll normalization. Those are resource-style consumers, not
additional behavior-private exceptions; the four-flag EDIT ABI witness does
not establish their whole native boundary. They remain in the existing
control-state/private-precomp closure. This correction has zero new mirror
diff and zero guest diff and introduces no new semantic concession.

### Combo extended-edge declarations and list style operation

The source sweep confirms comboini.c:CBNcCreateHandler reads WEFCLIENTEDGE
to select f3DCombo, then clears WEFEDGEMASK after retaining its desired
scroll style. The retained user.h lacked both names and WEFWINDOWEDGE.
OPENNT-HOST-101 now includes the three unchanged definitions at pinned
OpenNT windows/core/ntuser/inc/user.h:1755--1757. These are ExStyle offsets
in the retained original layout; importing three declarations changes no
control algorithm and proves nothing about native private flag encoding.

lb1.c WM_NCCREATE separately calls NtUserAlterWindowStyle to remove vertical
scrolling for a multicolumn list and, for pre-4.0 compatibility, add horizontal
scrolling. Original kernel/ntstubs.c:6190 uses the exact mask/value expression
without style notifications. This is not xxxSetWindowStyle and must not be
replaced by public SetWindowLong merely to resolve its name. The existing
read-native-imports.ps1 inspection confirms a current x86 USER import at
IAT100EEC88; it does not prove the native implementation's argument contract.
The available O:/WINNT/System32/dumpbin.exe could not disassemble because its
LINK.EXE was unavailable. No binary observation or native ABI pass follows.

Remaining finite state-service evidence is therefore the combo packed flags
and list mask/value call, including no additional notifications and correct
post-call retained state. The already recorded dialog behavior-private bit
correction is independent of these resource services. No guest changes,
new behavior body, runtime run or publication occurred in this declaration
step; grouped compilation and lifecycle acceptance remain pending.

### List mask/value native leaf admission

GNU objdump supplied the missing read-only disassembly. Inspected x86
user32.dll SHA25656F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E
and win32u.dll SHA25631B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C.
USER's call at1009BFB2 follows a test for LBS_MULTICOLUMN|WS_VSCROLL and pushes
zero, WS_VSCROLL, HWND into NtUserAlterWindowStyle's decoded IAT100EEC88.
The named win32u export RVA1D90 ends in ret0C. This establishes the current
x86 three-argument HWND/mask/value call shape; no syscall number or RVA is
used by production. It is static ABI evidence, not a native behavioral pass.

Extend ADAPTER-WOW-057/061 with that named no-result native leaf and the same
required post-mutation WND refresh as the existing internal state services.
The unchanged original lb1.c keeps version selection and mask construction;
the adapter owns neither the normalization algorithm nor a second style copy.
Original ntstubs.c's direct WND update cannot operate on modern USER memory;
the current native service is the same-shaped resource boundary. Do not
replace this operation by public SetWindowLong or import the kernel server.
Missing readiness or required refresh is a terminal adaptation failure.
Supplemental native and mocked-boundary assertions must verify both original
masks, unchanged unrelated bits, no style notifications, refresh and failure.
These stay unrun until the complete S3 group is ready. No mirror body or guest
change is required; the facade maps the existing original symbol.

Implementation now stages the export in the existing native readiness table,
the same-shaped control facade and required refresh. The call fixture covers
both original masks, retained unrelated bits, readiness rejection before any
mutation and terminal refresh failure after mutation without replay. The native
fixture covers actual style results, restoration and unchanged notification
count. Both remain unrun; original ListBox creation and guest lifecycle still
require complete-group selection. Diff whitespace review passes at this step.

### Combo packed-flag service evidence

On the same hashed x86 USER image, objdump disassembly at100844DE--10084510
shows the native combo caller testing owner-draw bits, then passing0D02 to
the set helper100BEE28,0EB0 to clear helper100BCF3E, and0903 to that clear
helper. The previously inspected helpers forward their packed argument to
the decoded NtUserSetWindowState/NtUserClearWindowState imports. These match
the unchanged original comboini.c CBFHASSTRINGS, combined scroll/border and
WEFEDGEMASK operands, respectively. This extends the four-flag EDIT ABI
evidence to the actual combo consumers; no private native WND pointer is
read by production and no fixed address enters its code.

The existing state binding already forwards these resource operations and
refreshes both style fields through the retained owner; a second combo-specific
setter is unnecessary. Its mocked original-client fixture now checks combined
style guards and ExStyle clearing while preserving unrelated bits. The native
fixture includes these three operand sets, exact restoration even when only
part of a mask was originally set, and the existing no-extra-notification
assertion. These assertions remain unrun. Thus argument-shape/source coverage
is established, not native behavior, original combo creation, drawing or guest
lifecycle acceptance. No additional runtime body, mirror or guest diff is
introduced for this consumer sweep.

### Shared control/dialog native-resource facade sweep

A lexical census of NtUser calls in the recovered ed*/combo*/lb*/dlg* units
was followed by caller inspection (comment-only SetScrollPos matches were
excluded). The control compile facade omitted several resource operations
already implemented by the common native-call owner. It now maps EnableWindow,
MoveWindow, SetWindowPos, SetParent, SetActiveWindow and ScrollWindowEx to
their existing wow_native counterparts, WaitMessage to the existing WOW-aware
queue wait, and ReleaseDC to the public DC release. Original combo geometry,
list scroll policy and dialog enable/activation order remain at their callers.
Real geometry and parent relationships remain native-owned, with publication
and execution restoration supplied by the existing native-call boundary.
No second wait, resource implementation or mirror body is added.

This closes facade omissions, not all internal NtUser calls. ShowWindow needs
the original dialog SHOW_OPENWINDOW operand audited before a shared mapping;
DragDetect, foreground activation, thread-state/query and dialog system-menu
services remain distinct outgoing bindings. A spelling match does not prove
these contracts. Complete private-precomp selection and group runtime tests
remain required; the macro additions alone are not behavior acceptance.

Follow-up resolves the ShowWindow operand: pinned public/sdk/inc/winuser.h:261
defines SHOW_OPENWINDOW as1; kernel/showwin.c explicitly documents it as the
old Win1.03 spelling of SW_SHOWNORMAL, including activation and restoration.
The facade now maps NtUserShowWindow to existing wow_native_ShowWindow and
retains the old spelling as an SDK alias if absent, with a compile-time equality
assertion. Original dialog calls and combo SW_HIDE/SW_SHOWNA calls share that
existing native-call boundary, not a new display policy. This supersedes only
the ShowWindow uncertainty above; foreground/query/thread-state and drag
services still require their separately accounted bindings. No runtime pass
or complete original-dialog acceptance is claimed.

### Dialog native queue flag binding

Original dlgmgr.c sets/clears QF_DIALOGACTIVE during activation and clears it
at final destruction. Original inc/user.h:2358 defines0x4000; kernel/ntstubs.c
NtUserSetThreadState applies flags/mask to the current queue, and input.c:4534
uses that queue bit to produce KF_DLGMODE (0x08000000) in message lParam.
It is native input-queue state, not the original private WND color bit and not
a new WOW scheduler flag.

On the previously hashed native x86 images, USER call1002A74D passes a mask
of0x4000 and flags of0 or0x4000;1002AEDB clears it with0/0x4000. The decoded
IAT100EEBA8 is NtUserSetThreadState; its named win32u export RVA1DF0 ends in
ret8. These are actual dialog call operands and stack evidence, not inferred
enum compatibility. ADAPTER-WOW-062 may bind this same-shaped no-result native
operation through its existing resolver; missing service remains a terminal
adaptation failure, never a no-op. Keep original activation/cleanup ordering
and current GUI thread. No callback, HWND publication or second queue-state
cache belongs to this direct operation. Do not import the kernel queue/input
implementation or manufacture the bit on arbitrary posted WM_CHAR messages.

Supplemental argument/failure assertions accompany the binding; actual native
input KF_DLGMODE and real dialog cleanup remain whole-group acceptance duties.
No runtime behavior pass is established by this static ABI evidence. The only
new mirror content is the exact original QF_DIALOGACTIVE declaration under
OPENNT-HOST-101; no guest or original algorithm changes are admitted here.

### Dialog foreground and system-menu dependency audit

Pinned kernel/ntstubs.c:6274 and:7846 answer the current-thread and target-window
foreground queries by comparing their input queue with gpqForeground. They do
not compare thread IDs. The retained dlgmgr.c modal-return edge destroys the
dialog, then reactivates its owner only when the current queue is foreground
and the owner's queue is not. Preserve this original conditional; replacing it
with GetWindowThreadProcessId(GetForegroundWindow()) equality is not equivalent
for attached input queues. Native Windows is the queue authority; neither the
WOW task scheduler nor the copied WND graph should cache an independent answer.

Original windows/core/w32inc/winuserk.h assigns WindowIsForegroundThread=6 and
UserThreadStateIsForeground=12. Current x86 USER disassembly contains nearby
NtUserQueryWindow calls with operand7 (1002A697/1002AE58) and many
NtUserGetThreadState calls with operand14. These observations do NOT establish
that those operations mean foreground queries. Do not forward the old enums,
infer new values from their numeric proximity, or read private native queue
pointers. The next required evidence is the native modal-return conditional's
complete call chain, followed by attached/detached input-queue acceptance.
This remains service-closure group4, not a new S or a new compatibility waiver.

For system menus, original dlgbegin.c:585 selects the special dialog menu only
when WFSYSMENU is set and WFSIZEBOX/WFMINBOX/WFMAXBOX are all absent. Otherwise
it requests the window's own normal system menu. Original kernel/mnapi.c:95
loads ID_DIALOGSYSMENU once into desktop-owned storage and binds it to the
window; client/res.rc:154 defines exactly SC_MOVE and SC_CLOSE (Alt+F4).
The algorithm's decision belongs to the original creator, while the real menu
resource and its ownership belong to native USER. Importing desktop locks,
private handle-table ownership and kernel menu structures is not required to
preserve the creator's decision.

On the previously hashed native images, USER at1002FE06 tests the three style
bits, calls named NtUserSetDialogSystemMenu through IAT100EEBC4 at1002FE10
for the simple case, and calls the normal system-menu getter with FALSE for
the other case. The named win32u export is present. This proves a candidate
matching service edge, not full resource equivalence or successful integration.
No native pointer/layout or old SFI operation number may cross this boundary.
Before selection, establish its result ABI, wrap the operation in the existing
execution/publication owner, and use the existing menu identity owner for any
exposed HMENU. Do not create a second desktop-menu cache.

Fixed acceptance additions to the existing group: (1) attached and detached
owner/current threads, with owner destruction during modal teardown;
(2) simple dialog Move/Close commands versus sizeable/min/max normal menus,
including lazy copy/mutation and destruction; (3) native service unavailable,
native failure and callback unwind with original return/error ownership.
These are unrun obligations, not tests passed by disassembly. This audit adds
no runtime, mirror or guest changes and does not close the other five groups.

Implementation follow-up: the named NtUserSetDialogSystemMenu export's x86
stub at RVA66C0 ends in ret4, agreeing with USER's single-HWND call described
above. ADAPTER-WOW-062 now stages this leaf and the existing native-call scope
around it. The original SFI spelling maps symbolically to that wrapper in the
control/dialog facade; no dispatch number or original algorithm changes.
Readiness failure is terminal; a native FALSE remains FALSE, and last error
survives return-state publication. The original creator continues to own its
existing treatment of the result. Real native return/resource behavior still
needs the group run; stack evidence alone is not that proof.

wow_dialog_call_fixture adds target identity, native TRUE/FALSE, unavailable
service, entry failure, native exception, return-publication failure and
subsequent-success assertions using explicit doubles. They are staged, unrun,
and do not prove real menu contents or lifecycle. No standalone micro-build
was run. Foreground queries and the other fixed closure groups remain open.

### Remaining public dialog resource leaves

Pinned usercli.h:1772--1783 maps context-help ID, cursor position/count and
foreground activation to their original services. In particular original
client.c:2960 SetForegroundWindow and the internal NtUser spelling enter the
same SFI_XXXSETFOREGROUNDWINDOW operation. The facade now delegates these
resource operations to public host USER through the existing native-call
owner; original dialog timing, tests, cursor positioning calculation and
balanced hide/show remain unchanged. No native syscall IDs or second cursor,
help-ID or foreground cache is added. Host activation restrictions still
apply; do not turn a failed native request into success or remove the original
queue predicates to make it appear successful.

NtUserGetSystemMenu now selects existing wow_menu_native_GetSystemMenu, which
already scopes the native call and publishes the returned menu through the
menu identity owner. Do not use a bare GetSystemMenu alias and bypass this
publication. The service sweep covers both special and ordinary menu branches.

The existing real-HWND lifecycle fixture adds help-ID set/clear and balanced
cursor-count assertions while retaining task execution ownership. These are
unrun. Actual cursor placement and modal foreground/owner restoration remain
group acceptance, on the isolated test desktop; no foreground test or cursor
movement was performed in this source pass. Full private-precomp selection
remains open, so the facade additions are not proof of production reachability.
All additions are adapter/fixture code; original bodies and guest are unchanged.

Build review distinguishes the two halves: New-T404S5Wow32ProviderNinja.ps1
already lists wow_dialog_call_bindings.c and wow_dialog_native_bindings.c in
providerSupportSources. The adapter's former "not selected" source comment was
stale and is corrected. This does not select dlgbegin/dlgmgr/dlgend or establish
their private-precomp closure. Keep that original-body selection gap explicit
rather than counting adapter source-list presence as recovered behavior.

### Whole original-body style-selector declaration census

Source-led scan covered edecrare/editec/editml/editsl/edmlonce/edmlrare/edslrare,
combo/combodir/comboini, lb1/lboxctl1/lboxctl2/lboxctl3/lboxmult/lboxrare/lboxvar,
and dlgbegin/dlgend/dlgmgr. Compare each WF*/WEF* token with selected retained
declarations, then check the other ABI/adapter headers before declaring it
missing. Seven names had no definition: WEFDLGMODALFRAME, WEFDRAGOBJECT,
WEFRIGHT, WEFCONTROLPARENT, WFMAXBOX, WFMINBOX, WFSYSMENU. OPENNT-HOST-101
now retains their exact pinned user.h definitions. This is seven added mirror
declaration lines, no changed original function or guest file.

In particular lboxctl2.c:1607 tests WEFDRAGOBJECT before original DragDetect,
synthetic button-up processing and WM_BEGINDRAG notification to the parent.
Pinned public/internal/windows/inc/winuserp.h:207 and MVDM WOW16 windows.h
identify the input as old WS_EX_DRAGOBJECT=2. Do not infer it from current
WS_EX_ACCEPTFILES, substitute a modern drop notification, or silently remove
the original branch. Verify the creation/style projection retains the approved
original input and its updates; a modern host style sample alone is not proof
that this old bit survives. DragDetect's execution/unwind binding and parent
identity lifetime remain in the existing service/lifetime groups.

The census closes this finite declaration gap only. It does not prove private
frame-state producers, enum compatibility, header ordering, full link or any
runtime behavior. No micro-build/test was performed.

### List drag resource boundary and state-source trace

Pinned client/user32.def:732 exports DragDetect = NtUserDragDetect; the native
receive body (kernel/ntstubs.c:6755) translates/locks HWND and invokes original
xxxDragDetect, whose kernel input loop owns capture, synchronous button state,
queue waiting and cancellation. ADAPTER-WOW-061 now maps the original ListBox
call to public DragDetect via the existing target-aware native-call boundary.
Keep POINT and BOOL unchanged; no speculative coordinate adjustment, replay,
private queue or new WM_BEGINDRAG producer. Original lboxctl2 keeps all selection
and parent-notification policy. The current USER export RVA3A570 is a direct
IAT jump, not evidence of an extra client-side coordinate conversion.

State trace: creation captures request.dwExStyle; retained WND creation first
refreshes WINDOWINFO, and commit_window_info subsequently assigns that same
observed ExStyle to both binding words and retained WND. Thus current code has
one native resource authority, not an autonomous cached drag bit. The original
createw.c similarly stores dwExStyle, with a pre-4.0 mask0x3f retaining bit2.
This identifies exactly where to inspect native preservation; it does NOT
prove modern USER drops bit2. Do not introduce a shadow override without that
evidence. The internal ComboBox child admission mask intentionally allows only
its traced TOOLWINDOW/CLIENTEDGE call shape; arbitrary application ListBox
creation has a separate entry and is not rejected by that internal mask.

Acceptance remains real legacy-style ListBox creation/update, drag threshold,
button release/Escape/capture loss, parent notification exactly once and
destruction/reentry with task ownership restored. No native drag was run in
this source pass. This adapter addition changes no original or guest body.

### No-argument operation caller and build closure

Review found ADAPTER-WOW-065 had a named native reset leaf but no provider
source-list entry or original-call binding. Both are now staged: the provider
support list includes wow_list_native_bindings.c; SFI__RESETDBLCLK selects
wow_list_reset_double_click through a symbolic NtUserCallNoParam facade.
The wrapper enforces the leaf's missing-service/native failure rather than
letting original lboxctl2.c discard it. The original _ResetDblClk changes only
the current native queue's double-click time, so no extra native-call yield,
copied WND refresh or second history cache is introduced.

The other reached zero-argument operation, SFI_ZAPACTIVEANDFOCUS in dlgend.c,
selects the existing separately scoped wow_dialog_zap_active_and_focus. Do
not merge these operations simply because both take no parameters. Original
dispatch bodies are unchanged; no NT4 SFI number reaches modern USER.

The actual wow_edit_call binding fixture now has reset success/error checks,
including no callout or window-publication scope for this queue-only leaf.
Source review corrected placement to run those checks before unrelated color
call counters are incremented. Assertions remain unrun. Real WM_LBTRACKPOINT
return-2 double-click cancellation remains the existing group acceptance;
build-list presence and the new mock cannot establish that behavior.
No guest or mirror diff is added by this wiring step.

### Original dialog pointer caller closure

The service census found SFI_SETDIALOGPOINTER in original dlgmgr.c installation
and final clearing had no facade mapping, although OPENNT-HOST-083's unchanged
SetDialogPointer and wow_dialog_set_pointer storage owner were already staged.
The new symbolic binding reaches that owner for both UINT pointer and zero.
Original SetDialogPointer remains responsible for pdlg, FNID_CLEANEDUP_BIT and
WFDIALOGWINDOW; no duplicated transition is added. UINT transport is the
original x86 caller shape, not a host-native dialog pointer representation.

The original service is VOID and its callers cannot consume a FALSE result.
wow_dialog_set_pointer_word therefore makes an invalid retained state/extent
terminal via the existing adaptation failure owner. It does not enter a
native-call scope because this operation mutates only the retained behavior
state under the existing caller borrow. The common HwndParam symbolic facade
is now enabled for original control/dialog clients as well as its standalone
state subset; no NT4 numeric service IDs are emitted.

Supplemental dialog-call fixture checks install, clear, preserved last error,
failure without mutation and absence of native-call scope. The existing state
fixture still supplies the actual original FNID/flag-transition assertions;
new doubles do not replace those tests. Both remain part of the unrun complete
group. No original-body, mirror or guest diff is added.

### Common get/set chain review: production versus fixture closure

Reinspection follows the actual public path: wow_user_private_access calls
wow_window_client_try_set_long; GWL_STYLE reaches original _SetWindowLong's
initialized ListBox guard, then wow_window_client_set_window_long and original
xxxSetWindowLong. Its negative-index continuation xxxSetWindowData is absent
from production. wow_dialog_getset_fixture defines a replacement accepting
GWL_STYLE and returning0x13572468, so that fixture proves guard/dispatch only,
not native mutation, notifications or publication. Retain the previously
recorded style-notification compatibility blocker; do not substitute a bare
native setter or call this path integrated on the strength of that fixture.

Procedure identity is a separate branch of the same source owner. Original
client ntstubs.c _SetWindowLong asserts old lpfnDlg is not a CPD, normalizes
an incoming CPD via HMValidateHandleNoRip, stores the actual previous address,
and changes bAnsi to the target procedure's encoding before writing DLGF_ANSI.
rtl/getset.c uses that flag when generating a token for a mixed-encoding read;
dlgmgr.c directly invokes lpfnDlg and returns the same flag for WM_NOTIFYFORMAT.
The current original-body fixture supplies synthetic CALLPROCDATA, not a
validated modern token resolver. That cannot establish the production contract.

Thus a proposed opaque-token bridge must cover save/get/set/restore, invocation
encoding, WM_NOTIFYFORMAT and lifetime together. Merely replacing the direct
call with CallWindowProc while leaving old normalization/flag policy unchanged
is not a demonstrated solution. Conversely, missing state during a try-get/set
lookup is not by itself proof of a regression: some enrolled native windows
intentionally lack recovered control state. Do not make all such lookups fatal
without first distinguishing the actual state owner. These findings remain in
the existing CPD and service-closure groups, not new S tasks or semantic waivers.

### Native style continuation: version argument is not equivalence evidence

Read-only revalidation matched the installed win32kfull.sys to its retained
PDB (GUID e44ae6e5-f628-4389-f7a5-8878592d07b8, PDB age3, DBI/image age1),
SHA256 94F00AABE76A4AF3794E2134B806BF89E556B66814859C285DE62173350A5173.
Public symbols locate xxxSetWindowStyle at RVA6654C, xxxSetWindowData at
RVA26491C and NtUserSetWindowLong at RVA199020. Addresses are research evidence
only; no private-layout runtime access or fixed-address call is authorized.

Within native xxxSetWindowStyle, the GWL_STYLE path prepares styleOld/New,
sets EDX to0x7c (WM_STYLECHANGING), and calls RVA34B40 at RVA66606. There is no
original WFWIN40COMPAT-style gate on that inspected preparation path. The
GWL_EXSTYLE branch at RVA669E7 also rejoins this setup. This does not prove
final callback delivery: the shared dispatch path may have additional rules,
and those must be traced before claiming a behavioral difference. It does
rule out treating the native creation version argument alone as a completed
proof of original notification equivalence.

Pinned classchg.c explicitly gates both notifications for Quicken2, permits
the changing handler to modify STYLESTRUCT, applies private minimized-frame
compatibility and native resource transitions, then returns the pre-call
style. A correct continuation must account for those ordering/return rules,
native mutation and post-call publication together. Importing the whole kernel
desktop/DC/visibility implementation or blindly calling native SetWindowLong
would each bypass the responsibility audit. No runtime substitution was made
on this evidence; the already fixed service-closure blocker remains finite.

Dispatch follow-up resolves RVA34B40 to the exact matched public symbol
xxxSendTransformableMessageTimeout. A broad symbol enumeration encountered
an absolute/non-section symbol and the research reader rejected it; the
targeted exact-name query completed successfully and independently confirmed
the RVA against the same matched image/PDB. Do not report the broad query as
a successful full symbol audit. No reader error was used to infer a runtime
failure.

The consumer side remains explicit: retained wmtbl32.c:193--194 labels both
style notifications WM32UNDOCUMENTED; wmdisp32.c skips conversion for the
WM32NoThunking case rather than defining a version-gated notification filter.
Consequently neither the symbol name nor the absence of a dedicated thunk
proves old tasks cannot receive the notification. The native transformation
dispatcher still needs its relevant route classified, not an unbounded
import of message-dispatch implementation. Do not add a message suppression
hook or extra state cache before that source/ABI boundary is established.

Further matched-symbol trace narrows the remaining dispatch question:
RVA BA0F0 is IsMiPPointerMessage, not a version-compatibility predicate. Its
caller passes message and wParam only. In the transformable dispatcher, the
ordinary same-thread client path passes the original message/parameters to
RVA534C0 at RVA34D36. The matched symbol for that target is
xxxSendMessageToClient. The alternate direct-server path is selected from
native procedure state; it is not evidence that recovered client procedures
are suppressed. The inspected client-send prefix handles pointer-message
translation, procedure kind and native dispatch selection. It does not supply
the missing original classchg version gate as an identified adapter contract.

This excludes the earlier unresolved pointer-transformation predicate as a
candidate explanation for legacy style-notification suppression. Remaining
proof must concern the actual client callback edge/compatibility handling,
not repeated inspection of IsMiPPointerMessage or the source ListBox guard.
No native private flag is read by production and no runtime conclusion is
claimed from symbol names alone.

### Style-notification marshaling edge resolved

Read the installed kernel image as a PE file (not live kernel memory).
At the inspected client-send dispatch table, message0x7c and0x7d each select
entry0x613; its low-byte index0x13 selects image VA1400E8630. The matched PDB
names that target SfnINOUTSTYLECHANGE. The body copies the two style DWORDs
into its callback payload and issues the client callback; it does not contain
the original classchg WFWIN40COMPAT notification gate on this path.

The corresponding SysWOW64 USER PDB was independently identity-checked:
GUID de141706-5f1a-81a0-a689-324c2870a1c0, PDB age3, DBI/image age1,
image SHA256 56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E.
___fnINOUTSTYLECHANGE@4 is at RVA36B90. Its complete body through RVA36BE0
passes the captured style structure into the supplied client dispatch callback
and returns result/modified data. There is no legacy-version condition in this
marshaler. This resolves the previously open marshaling edge, not a claim that
every arbitrary application subclass must process every received message.

Decision: reject an unqualified xxxSetWindowData-to-public-SetWindowLong alias
as proof of original style behavior. Keep original compatibility/notification
policy distinct from native resource mutation. Do not introduce a broad
message filter: original callers may explicitly send those same messages,
including during reentry. Any scoped mutation adapter must distinguish its
own service notifications without suppressing independent application sends,
and preserve handler-modified style, nested mutation/destruction, old-value
return and publication. A silent extra notification is a semantic change,
not covered by the approved nonclient hit-test exception. The behavior gate
is still unrun; no host-private table/address appears in production code.

Owner decision requested after this trace: either retain the original legacy
notification contract and continue designing an exact resource boundary, or
explicitly admit modern host style-write/notification semantics while keeping
original control validation, WOW conversion and lifetime ownership. No answer
has yet been applied. The difference is observable (original comment names
Quicken2), not a mere internal refactor. If admitted, it still does not waive
handler modification, destruction/reentry, safe message representation,
old-value/error propagation or coherent native/copied-state publication.
If not admitted, do not use callback suppression based solely on message ID,
thread or target HWND as a substitute for service-origin identity.

### Provider-support census while style policy awaits decision

A fresh source-list census covers all21 current wow_control_extent,
wow_default_*, wow_dialog_*, wow_edit_*, wow_list_* and wow_window_client_*
implementation files. Each is explicitly named in the staged
New-T404S5Wow32ProviderNinja.ps1 support list, including the recently added
ListBox reset leaf. No further omitted adapter source was found in this finite
cohort. This is source-list evidence only: original consumer selection, emitted
Ninja flags, compilation and final symbol resolution remain unproved.

Tracked tools/tests scripts do not yet name the new dialog-call and edit-call
fixture source/link groups. Their staged C assertions therefore must not be
reported as registered or runnable acceptance merely because the files exist.
Group5 build closure must select original bodies and register the supplemental
fixture link inputs alongside them; group6 must run them together with real
guest/lifetime cases. Do not add a separate delivery gate for each leaf or
substitute these transport doubles for original-body/runtime acceptance.
The unanswered style semantic choice remains unchanged; this independent
source audit does not imply consent to modern notifications.

### Supplemental call-boundary build registration

New-T447S3CallBoundaryNinja.ps1 now records the two previously unregistered
dialog-call and edit-call link groups. The dialog group links its actual
adapter with explicit native/execution doubles. The edit group additionally
selects the unchanged original client.c state guards and ntstubs.c color
client in separate objects, using the same subset selectors as production.
Native leaves and the real runtime are deliberately not linked into these
double-based tests. This is boundary coverage, not native-service or guest
acceptance, and must run in the completed group's background verification.

The generator requires an explicit toolchain batch file, forces x86, uses
optimized MSVC compilation and rejects output directories outside repository
build/. It does not compile, run or publish. Other supplemental fixtures and
the original consumer/private-precomp selection remain group5 obligations.
The new graph has not yet been generated, compiled or exercised; no new pass
or delivery is claimed. The style-notification decision remains pending.

### CPD consumer closure: class registration shares the dialog blocker

Read-only tracing against the pinned original and the current worktree
establishes three distinct contracts, which a single opaque invocation test
cannot prove:

| Consumer | Original ownership | Current evidence / remaining requirement |
| --- | --- | --- |
| CallWindowProcAorW (original client/clmsg.c:1230) | Validates CPD; either extracts target or sends a conversion request, then applies WOW callback selection | Existing native invocation bridge delegates opaque native tokens to USER; this does not expose their target or actual encoding to other consumers. |
| DWL_DLGPROC get/set and DefDlgProcWorker | client/ntstubs.c stores a normalized target plus DLGF_ANSI; rtl/getset.c exports conversions; dlgmgr.c directly calls stored target and uses DLGF_ANSI for WM_NOTIFYFORMAT | Dialog fixture supplies CALLPROCDATA. Product normalization remains absent. Merely replacing the direct call with CallWindowProc would leave setter, getter and encoding contracts unresolved. |
| _RegisterClassEx (retained kernel/class.c) | Resolves CPD before registering the actual procedure | wow_user_task_lifecycle.c sets classes.resolve_callproc=NULL; wow_class_client_publish rejects tagged inputs without a resolver. This same NULL assignment exists at f47d951b0. Class fixtures install their own resolver, not a product provider. |

Original kernel/getset.c:742 GetCPD chooses the window's class for both
CPD_WND and CPD_DIALOG, searches spcpdFirst by target plus full conversion
options and links new objects into that class list. Therefore a per-dialog
record discarded at HWND destruction is not a faithful replacement. The
function body's ownership is more specific than its introductory historical
comment about window/thread lifetime. Modern USER remains the authority for
its own token storage; this source trace does not establish a modern layout
or permit reading it as the old structure.

The finite group3 acceptance must include a real token saved and restored
through dialog A/W access, invocation with converted text, WM_NOTIFYFORMAT,
and re-registration through the existing class consumer, plus invalid token,
class retirement and reuse cases. Fixture doubles cover original branches
only. No native token normalization, class completion or new runtime pass is
claimed. The remedy must share one identity/encoding owner; do not add
independent dialog/class registries or bypass the original registration body.
This audit adds no mirrored code or guest change. It identifies an inherited
product gap, not an S3 regression and not a reason to reopen S2's bounded
closure. It remains a blocker for the affected S3 chain.

### Original dialog initialization/shared EDIT DS production selection

The production generator had no WOW_ORIGINAL_DIALOG_EDIT object despite
retaining the three original client.c bodies. It now selects them together as
obj/opennt-user/dialog-memory.obj with the original x86 /Gz convention and
the control Unicode profile. This is distinct from the state-guard, bitmap
and class-client selections of that file. The existing registration bridge
supplies the same PFNWOWHANDLERSIN table; no new heap or callback registry.

The memory-callback fixture previously exercised LOCAL* and word-break macros
but did not invoke these three bodies. It now calls WOWDlgInit, GetEditDS and
ReleaseEditDS against explicit table inputs: allocation flags/256-byte size,
WORD result widening, LOWORD release conversion, signed initialization
parameter, full DWORD result, zero failures and last-error propagation.
The supplemental call-boundary generator links this fixture to the actual
client.c subset, not a replacement implementation. Six new callback calls
are asserted independently of the existing16 macro calls.

Implementer/reviewer source check: callback signatures match wowuserp.h's
PFNINITDLGCB/PFN16GALLOC/PFN16GFREE; all original bodies remain untouched.
Existing OPENNT-HOST-071/081 and ADAPTER-WOW-056 cover this selection; no new
mirror or guest diff. The graph and assertions remain uncompiled/unrun,
and do not establish real guest DS allocation/destruction or dialog closure.
Original full-body/private-precomp, CPD and relationship work remain required
before whole-group build and runtime acceptance.

### Implicit control-brush recipient: original reusable owner identified

The complete20-unit source census reaches GetControlBrush from combo.c:1260,
1868 and lboxctl2.c:78. EDIT instead retains its distinct ECGetControlBrush
client optimization in editec.c:2885; do not replace that optimization with
the generic brush service. NtUserSetScrollPos appears only in two commented
lines in lboxctl1.c and is not a missing live service edge.

The original generic chain is client/ntstubs.c GetControlBrush (null-DC check)
to kernel/random.c xxxGetControlBrush, then xxxGetControlColor. The middle
function selects popup owner versus parent, falling back to self, and retains
that recipient around the color call. It is a bounded reusable algorithm;
there is no need to reproduce its selection in project code or delegate this
policy wholesale to an implicit-recipient modern syscall. The already staged
explicit-recipient color boundary can remain the native resource/service leaf.

The integration consequence is precise: recover the two small original
wrappers, bind their object retention to the existing exact-state borrow
owner, and translate the explicit source/recipient PWND handles only at the
native color edge. The selected recipient, not merely the child, must enter
wow_user_native_call_begin, because its existing cross-task path uses that
identity to establish sender/receiver execution ownership. Copying the
original ThreadLock as a no-op without supplying the lifetime would not close
this dependency. Relationship publication remains the existing group2
prerequisite, not permission to sample a different recipient after reentry.

The staged implicit native brush leaf and its resolver requirement should be
removed when this original chain is wired; native-reference tests may keep
an explicitly test-only system call for comparison, not a product bypass.
No new source is imported or selected by this audit, and no runtime behavior
is claimed. This resolves the source-owner choice for the existing group4
brush obligation, not the remaining relationship implementation.

### OPENNT-HOST-102 implementation and source-only review

The identified chain is now staged and selected: unchanged client
GetControlBrush in its own ntstubs.c subset, unchanged kernel
xxxGetControlBrush in random.c, wow_control_brush_bindings for exact storage
retention/handle conversion, then the existing explicit native color boundary.
The outer adapter owns a nested borrow scope with finally cleanup. The
original ThreadLock site retains the exact selected recipient into that scope;
ThreadUnlock defers release until the immediately enclosing service returns.
There is no application call between that site and scope exit. Exceptions also
leave the scope. No second relation table or recipient-selection algorithm.

Removed the unused product NtUserGetControlBrush declaration, leaf, resolver
slot and readiness requirement. Native-fixture calls are now explicitly
test-only reference calls; they cannot satisfy the new original-chain gate.
The new fixture links both actual original bodies and adapter with lifecycle
and native-color doubles, covering parent/owner/self, mixed popup/child styles,
null DC, entry/validation failure, source/recipient retention failure, native
exception cleanup and subsequent successful invocation. It is registered in
the supplemental graph and remains unrun; no real-HWND lifetime proof claimed.

Background review PID11916 exited0. Both imported function bodies compare
exactly with the read-only OpenNT source after CRLF normalization. Both build
generators pass PowerShell parsing. Logs: build/M0-T447/S3/
call-registration-review/brush.stdout.log and brush.stderr.log. A subsequent
Git revision query on the external source directory was permission-denied;
this turn therefore does not claim a fresh verification of its Git revision.
The existing pinned-source authority is unchanged. Targeted diff whitespace
check found no errors; removed product symbol search returned no matches.

Mirror change: one bounded random.c subset plus a separate client wrapper
selection, registered OPENNT-HOST-102; two original bodies unchanged, only
include/subset facade added. No MVDM or guest modification. ADAPTER-WOW-068
records native dependency removal and new lifetime boundary. This resolves
the brush binding's source ownership, not group2 relationship publication or
group6 execution/lifecycle acceptance. No build, fixture execution, release,
commit or push occurred in this step.

### Host borrow / page-generation lifetime pairing

Source audit of wow_user_borrow_scope found that sampled records already own
both a host binding reference and backing borrow, but ordinary records only
own the host binding. A generation owning host storage is not the reverse
guarantee: reciprocal permanent ownership was deliberately removed to avoid
a cycle. Before host relationship/class pointers are exposed, their caller
scope must retain the corresponding generation independently of HWND lifetime.

ADAPTER-WOW-069 now stages that pair in the existing borrow record. A new
private parent service retains an exact backing/host-storage pair without
resolving a numeric HWND; it rejects absent/mismatched storage and reference
overflow. A withdrawn but still allocated matching generation may be pinned,
without restoring its public identity. Property-only records have no pin;
on later exposure after attachment they upgrade in place. Sampled records
already own their backing and are unchanged. Scope leave releases backing
while its host reference is still held, then releases the host reference.
No permanent owner cycle, new HWND table or guest-memory pointer is added.

The parent export list, header and consumer are updated together. The real
page-domain fixture adds matching/mismatched/null storage and live/withdrawn/
reclaimed generation assertions. The numeric dialog fixture now models
deferred disposal until its last transient borrow rather than requiring zero
readers at logical retirement. These are staged, not executed, assertions;
they do not substitute for real generation reuse/CCPU/guest acceptance.

Reviewer boundaries: original graph withdrawal already clears child/owner/
next and restores the desktop parent in guest backing. Do not diagnose that
as missing guest cleanup or patch guest data. Host relationship pointers are
still not produced by project_observed, and this lifetime pairing alone does
not close group2. No original mirror/body change in this step; whole-group
build and runtime verification remain outstanding.

### Observed WND inputs: reject synthetic compatibility provenance

Reached control/default source inspection finds direct parent/owner reads
in clmsg.c (including WM_HELP) and REBASEPWND parent traversal in dialog
logic. The20 control/dialog bodies' reached pcls access is dlgmgr.c:447's
atomClassName check; recovered combo-private spwndList/spwndEdit pointers
remain a separate original control-data lifetime, not host tree edges.
Do not infer that filling four tree links closes these distinct contracts.

The default handler consumes WFWIN40COMPAT for CTLCOLORSTATIC/CTLCOLORDLG/
CTLCOLORBTN. EDIT's read-only/disabled branch can reach that default handler
for a different-thread recipient. Original creation facts establish this bit
for recovered owned windows, but arbitrary observed HWNDs have no such facts.
Neither zero-filling the bit nor assuming all observed windows are modern
constitutes original-source equivalence.

A candidate public input was inspected and rejected. Microsoft documents
WINDOWINFO.wCreatorVersion as the creating application's Windows version:
https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-windowinfo
However this installed x86 USER32 image's matched PDB identifies
GetWindowInfo at RVA26280; successful completion at RVA264DB loads immediate
0500 and RVA264E0 stores it into output+3A, the WORD wCreatorVersion field.
This path does not read the target's creation version. Image SHA256 remains
56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E;
PDB GUID de141706-5f1a-81a0-a689-324c2870a1c0, image/dbi age1, PDB age3.
These are read-only research addresses, never runtime private-field offsets.
No observed-state version producer or new compatibility waiver is added.

Separately strengthened the staged page-domain pin test: after retirement,
release the earlier backing borrow while the independent host pin remains,
assert host storage/words are still readable and disposal has not happened,
then release the pin and require one disposal. The previous immediate
pin/unpin assertion alone did not prove independent lifetime. Still unrun;
no build, runtime or delivery evidence is claimed by this review.

### Bound the observed-recipient decision to its actual original entry

The ECGetBrush callsite at editec.c:1044 adds a crucial gate: it selects
WM_CTLCOLORSTATIC only for (read-only OR disabled) AND current task version
at least VER40. Otherwise it selects WM_CTLCOLOREDIT, whose retained default
color branch does not inspect WFWIN40COMPAT. ECGetControlBrush then chooses
the default path only for a different-thread recipient. Ordinary same-thread
CTLCOLOR callback delivery is not implicated and must not be replaced.

Owner has been asked whether only WM_CTLCOLORSTATIC for such a recipient
without recovered creation facts may use native DefWindowProc. Managed
original recipients and all other messages remain original. The potential
waiver is the old native recipient's NT4 background-color compatibility;
it is neither permission to execute an application callback that the original
skips nor to replace all default processing. No response or implementation
is inferred from goal continuation. Existing style-notification approval
remains independently pending. No source mask, invented version bit or
general native fallback has been installed.

### Owner disposition of the observed default-color boundary

Owner replies: "同意，类似问题也相同原则处理". This approves the
previously described minimal host-default substitution and equivalent cases,
not a broad replacement of original control behavior. The architecture rule
now records that standing boundary. The unknown creation facts must remain
explicitly unknown; failure to acquire a retained object is not evidence of
unknown facts and must not silently trigger native fallback. Preserve the
original different-thread default selection rather than introducing an
application SendMessage callback. Recovered owned recipients remain original.

This is an approval/design update, not implementation or runtime evidence.
Observed WND construction/relationship publication and the color-path tests
remain in the existing whole-group closure. The separate style-notification
ordering decision is not classified as an equivalent default-color case.

### Implement the approved EDIT-only default-color boundary

Source review found ECGetControlBrush passes `(LONG)pwndEdit` to its
different-thread default worker, whereas public WM_CTLCOLORSTATIC expects
the source HWND. A generic native DefWindowProc substitution would therefore
cross the wrong identity boundary. OPENNT-HOST-103 registers one call-target
change at this exact original branch; parent/owner/self and thread selection
stay unchanged. All other original default-worker calls stay untouched.

wow_edit_default_control_color in the existing default adapter obtains an
exact scoped recipient, queries its existing creation facts, and preserves
original handling when facts are known or the message is not STATIC. Only
ERROR_NOT_FOUND from the facts getter permits the admitted native default.
It then retains the exact source, captures both HWNDs and calls the existing
CsSendMessage default-service adapter with zero extra/FNID_DEFWINDOWPROC.
That existing adapter owns native execution restoration; no SendMessage to
the application or new policy/state map is introduced. Lookup errors fail
without fallback. Entry last error is restored before either implementation.

The existing default binding fixture now covers known facts, another color
message, unknown facts with converted source identity, recipient/source
retention rejection, non-absence fact failure and native exception propagation.
Its production-wrapper/double target joins New-T447S3CallBoundaryNinja.ps1.
These assertions are unrun; they do not prove real HWND lifetime, HDC colors,
or the full EDIT dependency group. Original body selection and observed WND
publication remain required. Mirror budget increases by one call identifier
and its local comment; no guest inputs change. Earlier byte-identical EDIT
statements describe the pre-103 import, not this staged revision.

### Sampled-borrow identity across nested retirement and reuse

Source trace of wow_user_borrow_window and wow_user_borrow_observed_binding
found both searched outstanding sampled records by HWND alone. Outer scopes
can retain a retired record after the domain publishes a replacement with the
same native numeric identity. The old search would return the retired WW and
could promote its binding into the replacement. This is a source-proven
identity ambiguity, not a reported reproduced application crash.

The existing page-domain backing token already distinguishes these retained
allocations. Lookup now borrows the currently published backing before cache
search and requires HWND plus backing equality. A hit releases the temporary
duplicate pin; a miss transfers it to the new sampled record. Allocation
failure releases the pin. Promotion resolves through that checked lookup and
matches its returned WW, never an earlier HWND-only record. Existing retired
aliases keep their original scoped lifetime. No registry, guest change,
original-body change or new native API is introduced.

verify_sampled_generation_reuse in wow_native_creation_binding_fixture.c
stages a deterministic real-page-domain retirement/republication using one
real native STATIC HWND, while an outer borrow remains. It asserts distinct
WW identities/markers, current promotion in the nested and resumed outer
scope, and preservation of old aliases. This deliberately models generation
replacement, not actual native handle allocator reuse; actual destruction
and reused-HWND acceptance remain required. It has not run. Existing numeric
dialog lifecycle mocks cannot prove this case because their tokens equal the
row index rather than independently retained generations.

### Registered class identity for the unchanged original dialog consumer

The retained control/dialog source census has one direct CLS data consumer:
dlgmgr.c's WOW superclass notification compares pcls->atomClassName with
WC_DIALOG. Current WND construction had no pcls producer. The existing
wow_class_lookup_entry is not layout-compatible with original CLS beyond
incidental fields; casting it would violate the responsibility rule.

ADAPTER-WOW-063 now embeds an original-layout identity projection in the
existing class binding. Its sole admitted field, atomClassName, is assigned
by the same publish function as the authoritative registration atom. Both
native-class adoption and owned-class registration use that function. The
value is immutable afterward; there is no independently updated class fact,
additional lookup or allocation. Other CLS fields are not supported inputs.
WND attachment obtains the projection from its already retained class binding.
Final release of the WND already releases that class reference, so detach and
unregister do not invalidate an outstanding WND's class atom.

The unchanged original dlgmgr expression can now consume this input for
registered classes without another mirror diff. The original cleanup code
continues using its separate cleanup view and authoritative lookup entry;
do not conflate that view with the recovered original-layout WND. The staged
wow_window_class_lifetime_fixture asserts shared class identity across old/new
WND bindings and reads the atom after native window destruction and class
unregistration while the outer borrow remains. These tests are unrun.
Class-less observed native windows still need graph-owned class identity;
this bounded wiring does not claim complete observed-WND or dialog closure.

### Original same-address pointer macros for the control client cohort

Source: pinned OpenNT windows/core/ntuser/rtl/userrtl.h lines71--74;
contrast client/usercli.h lines179--184. Original RTL already has a
same-address representation branch: REBASEALWAYS/REBASE/REBASEPWND return
the field and REBASEPTR returns the input. OPENNT-HOST-104 retains those
four definitions unchanged, with original notice and bounded subset guard.
No _USERK_ profile is defined by the build. The adapter's original-layout
WND/CLS pointers are native process-local addresses, not server-form guest
projections, so no kernel/client desktop delta applies to this cohort.

The consumer census includes Combo/List parent traversal, dialog parent/
owner traversal, dlgmgr's class atom, EDIT brush recipient and single/multiline
EDIT's captured creation text. REBASEPTR is therefore not exclusively a WND
operation: replacing it with a window-handle lookup would corrupt text input.
The common original default/edit/dialog facade selects the retained macros.
The existing class lifetime fixture now asserts unchanged class identity,
null parent/owner preservation and ordinary pointer identity after retirement.
This is staged source wiring, not run evidence; it supplies no missing graph
edges and is not a substitute for the separate retention/publication closure.

### Grouped call-boundary verification, 2026-10-16

Configured tracked New-T447S3CallBoundaryNinja.ps1 with the installed VS2022
BuildTools Common7/Tools/VsDevCmd.bat, x86 /MT /O2 /Gz /DSTD_CALL, under
build/M0-T447/S3/call-boundaries-x86. Background Ninja uses -j4, a300-second
deadline and30-second owned fixture deadlines. All intermediate files remain
in build. No product or guest images are replaced.

The initial detached runner PID25160 disappeared without logs/exit record;
it is not a successful build. The owned supervisor retry PID35868 produced
actual compiler failures in r2: unguarded wowuserp.h was included twice and
original-layout PTHREADINFO conflicted with the input-group adapter typedef.
The new guarded wow_shared_user_abi.h owns the one declaration inclusion;
the layout uses a locally scoped PWOW_CLIENT_THREADINFO alias. Original
structure declarations and guest inputs are unchanged. r3 runner31616,
Ninja15820 failed link1120 because the fixture omitted the existing
wow_control_registered_extent provider. The graph now links its actual source,
not a replacement test implementation. No runtime tests ran on that failure.

r4 runner19812/Ninja26196 built successfully in19.07seconds. Dialog passed;
EDIT failed only the newly added fixture assertion equating CBEDITEXTRA to
sizeof(PVOID). Source review of original client/usercli.h:571 proves the
constant is6. The assertion is corrected to sizeof(WND)+6; product behavior
is unchanged. The runner now collects all fixture failures after a successful
build rather than skipping independent cases after one assertion failure.

r5 runner12320/Ninja25028 built incrementally from20:12:27.428 to
20:12:42.855 UTC, exit0. Five fixtures completed by20:12:43.972, all exit0:

| Fixture | PID | Observed output |
| --- | --- | --- |
| wow_dialog_call_fixture | 37072 | dialog-call failures=0 |
| wow_edit_call_fixture | 6708 | edit_call_failures=0 |
| wow_edit_memory_callbacks_fixture | 35264 | WOW_EDIT_ORIGINAL_MEMORY_MACROS failures=0 calls=16 |
| wow_control_brush_fixture | 22660 | control_brush_failures=0 calls=8 |
| wow_default_window_binding_fixture | 30416 | default_window_binding_failures=0 |

Logs: build/M0-T447/S3/call-boundaries-r2 through r5; r5 exit.txt is0.
The test programs are tracked under tests/adapter-mvdm-host-out/wow and the
tracked generator reproduces their source/link selection. Toolchain launcher
and Ninja graph are retained beside the binaries. SHA256 of the tested EXEs:

- dialog: 3CCB1E6F9F7886A2BD8D6CDDCFFED3D2CF9384400FF51FFA02564C6C90E718E8
- edit: 53BF02F97818E00090924D42AAE4E8429D69CC79940861DF686DD6DEEAB9EFC2
- memory: C288514288922BCF15C15C19501F1EF75903F001FF23B09BFAB98CCD8BEC1D4E
- brush: 8E173FAEEAA2E214AB4147417E61E5DBDF06FEDB64BA4B1A8CBFE4E9F6D733AA
- default: C8E8B7C7852ADE3B514D90335B92CA79DD7378E2ADC17D1068DBA73BCE527061

Scope: actual adapters and selected original call/color/memory bodies with
explicit native/lifecycle doubles. This proves their asserted call pairing,
error/exception handling, original brush recipient selection and default-color
dispatch boundary. It does not execute the full EDIT/dialog bodies, native
HDC color behavior, the new generation-reuse or class-lifetime assertions,
or guest programs. Those remain open. No S3 runtime product delivery,
publication, commit or push occurred. Full dependency-group acceptance and
all13 coherent publication remain mandatory.

### Dialog system-input context and nested entry review

Source census of retained dlgbegin.c finds message-font cloning at78,
DPI conversion at88, system font metrics at134/442/443, work-area placement
at476/477 and process class atoms at688/715/746. Retained dlgmgr.c consumes
snap-to preference at601/919 and screen/work-area clipping at1001/1003.
dlgend.c has no direct gpsi/oemInfo/rcScreen consumer. These are inputs to
original algorithms, not grounds to replace parsing, layout or cleanup.

ADAPTER-WOW-063 now stages wow_dialog_context_bindings: host preferences and
screen metrics come from public USER/GDI; atom identity comes from the
initialized process cleanup table; existing EDIT font measurement is reused.
The view is call-scoped, not a second preferences database. Creation owns a
temporary HFONT supplying original CreateDlgFont's clone operation. Ordinary
dialog dispatch captures no font; final destruction avoids preference queries.
Capture commits its output only on success; release preserves last error.
Creation and dispatch restore the preceding TLS view in their finally blocks.
The private dialog header selection now exposes this view to original bodies.

Review added explicit outer-context identity/font preservation assertions to
wow_dialog_creation_entry_fixture for success, failed creation, SEH unwind and
failed capture. The finalization fixture only requires a context for messages
that actually capture one, excluding WM_NCDESTROY/WM_FINALDESTROY. Native
context acquisition has its own fixture with explicit runtime/metric doubles.
The supplemental generator has a separate dialog-context-boundaries target.
All these new assertions and source changes remain unbuilt/unrun; r5's five
passing fixtures predate them and are not evidence for these changes.

No new original-body diff or guest modification is introduced by this group.
Full private-precomp composition, host relationship production, CPD/class
consumer closure and real dialog/guest lifecycle acceptance remain required.

### Grouped dialog context boundary verification

Background runner29724 / Ninja21828 (dialog-context-r1) failed compilation
in9.52s because the supplemental graph omitted the existing wingdip.h include
directory. No tests executed. Added the same original ABI windows/inc search
root already selected by the production generator; no header copy or source
workaround. Retained logs below build/M0-T447/S3/dialog-context-r1.

Background runner4360 / Ninja28312 (dialog-context-r2) compiled the entire
three-fixture group in27.87s, exit0, from20:23:58.628 to20:24:26.499 UTC on
2026-10-16. Persistent call-boundaries-x86 cache, MSVC x86 /MT /O2 /Gz,
Ninja -j4, build deadline300s; every executable deadline30s. Actual tests:

- context PID8244: dialog_context_failures=0, exit0,0.15s;
- creation PID17820: dialog-creation-entry failures=0, exit0,0.14s;
- finalization PID38796: dialog-finalization failures=0, exit0,0.12s.

Recorded supervisor log is build/M0-T447/S3/dialog-context-r2-supervisor.log;
per-command stdout/stderr and exit.txt are in dialog-context-r2. The tracked
New-T447S3CallBoundaryNinja.ps1 target dialog-context-boundaries reproduces
the source selection. This verifies native preference/font acquisition with
runtime/metric doubles and actual entry wrappers with original-worker doubles:
argument forwarding, capture failure, outer-context preservation, unwind,
and cleanup independent of preference capture. It does not execute original
dialog parsing/layout/resource bodies or a guest. No product publication or
S3 closure follows from these results; the full group obligations above remain.

### Modal entry bypass found by production call-chain review

The passing creation fixture covers wow_user_server_create_dialog, not all
original creator callers. wow_title_modal calls retained clres.c's
DialogBoxIndirectParamAorW, which reaches InternalDialogBox (dlgmgr.c200)
and its direct InternalCreateDialog call. That route bypasses the scoped
creation wrapper. Selecting the original bodies with only the previously
tested wrapper would leave gpsi/oemInfo context absent on this modal path.

wow_title_modal now captures the same dialog context after task/borrow entry,
retains it through original creation and DialogBox2, and restores the previous
TLS context/releases its temporary font before scope teardown in finally.
Capture failure records the error and never enters the original manager.
No original modal behavior is replaced and no mirror diff is added. This
production entry change is staged, unbuilt/unrun: dialog-context-r2 did not
link wow_title_policy.c and does not prove this path. Modal creation, nested
dialogs, callback destruction and exceptional unwind remain full-group tests.

Modal-entry teardown review also found that SEH bypasses the normal-path
SetLastError after finally. The task/borrow/style cleanup therefore could
replace the throwing call's error. The finally now saves/restores that error
around cleanup; normal return still reports its explicit result/style failure.
Added wow_dialog_modal_entry_fixture with the actual title-policy source and
explicit manager/execution doubles: nested creation preserves the outer font
and TLS view, failed capture does not enter the manager, and throwing manager
cleanup deliberately clobbers last-error to exercise preservation. The tracked
supplemental graph exposes dialog-modal-boundary (shell32 link dependency for
the remaining real title-policy functions). This new fixture is unbuilt/unrun,
not evidence for the original modal loop or enabled NTW16S attachment.

### Original-client compilation composition

Production generator review confirms the distinction between selected support
sources (wow_dialog_*/wow_edit_* entries) and the still-unselected original
EDIT7/combo-list10/dialog3 bodies. Existing control-state, control-drawing and
control-geometry precomp headers only cover smaller source subsets; none is
a valid general client-group precomp. Selecting the twenty bodies against
those subset environments would not complete their original dependencies.

Added src/wow32-dll/include/control-client/precomp.h as the single composition
point for existing original WND/control layouts, registered memory callbacks,
control/native calls and drawing leaves. The shared client profile remains
explicit; dialog units add their dialog profile and select the dialog context
last, so nested EDIT input state does not become dialog system-input authority.
No new algorithm, native private-structure cast or mirror-file change is added.
The production generator does not select it yet. Header composition and the
twenty original bodies remain uncompiled; required symbols, original state
and host relationship/CPD producers still require source audit and completion
before a grouped production build. Do not treat helper selection or this
header's existence as original-control execution evidence.

### Control input and retail diagnostic consumer sweep

Read all gpsi/oemInfo/SYSRGB consumers across admitted ed*.c, combo*.c,
lb*.c and dlg*.c against their entry producers. EDIT/combo/list metric,
wheel/timer and class-atom inputs are populated by edit_entry; dialog-only
font/work-area/snap-to inputs use the new dialog context. A relevant mismatch
is that wow_default_capture_context(WM_NCHITTEST) populates border units but
not argbSystem. It cannot be used as a palette source for combo.c1889-1893
or lboxctl2.c339-399. The shared client precomp now binds SYSRGB directly to
GetSysColor, matching the existing bitmap binding and single host palette
authority, without adding a second cached table. Original control painting
and color choice remain unchanged.

Also connected edecrare.c648 GetCharWidthInfo to its already implemented
native leaf and declared the existing original DrawFrame/CopyInflateRect
owners with their /Gz ABI. Reused the existing fixed NT4 palette extent.
For retail diagnostics, inspected pinned OpenNT windows/core/ntuser/inc/
winerrp.h retail branch: RIPERR sets UserSetLastError; RIPMSG ignores format
arguments (source SHA256
A04D4685C2A5FEF4C68614161A616EAC40B4FD97EFBCB5C0C9B52F2169E3CEFF).
The facade retains these semantics with SetLastError and no-op
diagnostics, including RIPMSG3 expanded by original class-size validation.
This is an ADAPTER-WOW-058 compile binding, no new mirror import or runtime
diagnostic engine. All new bindings are staged, uncompiled; no runtime result
is inferred. Original control identity/relationship and CPD closure remain
required before selecting the twenty bodies.

### Public CPD normalization candidate and modal-entry verification

Background modal-cpd-r1 runner18724 / Ninja3456 built both targets in18.89s
(2026-10-16 20:34:04.957--20:34:23.845 UTC), exit0, x86 /MT /O2 /Gz,
Ninja -j4,300s deadline. The actual title-policy entry fixture PID3988 passed
dialog-modal-entry failures=0 in0.12s. It verifies nested context restoration,
failed capture and error-preserving SEH cleanup using explicit manager/runtime
doubles, not original modal scheduling or enabled NTW16S. EXE SHA256:
D8D93B7DEDEEA7E84B2A8F19E1C18D906F66A8E1A30533EE5E340ECA963736EC.

The research-only wow_cpd_class_normalization_probe PID20904 finished in0.15s,
exit1 with four encoding assertions failed. It creates only test-owned native
classes/windows, obtains a real opposite-encoding CPD, registers a scratch
class using that token, queries both A/W class information, then destroys and
unregisters its resources (cleanup checks passed). Both directions recover
the original raw procedure but expose it through the scratch registration's
encoding, not the source procedure's encoding. Example source ANSI address
001B1400/tokenFFFF0ADB: scratch registered W returns queried_W=001B1400 and
queried_A=FFFF0D6F; the Unicode source shows the symmetric result.
EXE SHA256 B0B6C0BA6E93F82EA023987053B96E89375521237C1356B28F12F360BD67FC9F.
Logs are build/M0-T447/S3/modal-cpd-r1 and its sibling supervisor log.

Conclusion: public registration/query is not a complete decoder for original
dialog _SetWindowLong, which reads both pfnClientPrevious and wType to set
DLGF_ANSI (retained client/ntstubs.c127--131). Do not select this as a universal
normalizer or infer encoding from registration. Source review distinguishes
the class consumer: retained kernel/class.c40--44 reads only the target, not
wType. Target-only reuse remains a candidate for that narrower contract, but
invalid-token validation, lifetime and scratch-class side effects have not
been verified; no product resolver or extra registry is installed. The probe
is outside the default test target and its failure is retained as rejected
full-decoder evidence, not a passing acceptance test. Full CPD closure remains.

### Retained-generation input for host WND relationships

Source review identifies the existing complete observation reader as the
proper data owner, not fresh GetWindowInfo/GetParent calls from host WND
construction. Previously copy_window_view enumerated only currently published
slots; a caller retaining old backing across retirement/reuse could not read
its complete view through that API. read_window_words_view exposed only WW,
style and module, insufficient for relationship/class/geometry projection.

Factored the existing row-copy body into copy_window_observation and reused
it for both bulk enumeration and new read_window_view(backing,row). The new
entry requires an outstanding generation borrow, reads no native state, and
commits output only on success. It retains exact historical full identities;
the terminal relationship clearing remains with page_domain_withdraw_window,
not duplicated in the reader. Numeric class/rectangle fields come from the
same retained backing. Its returned links do not authorize reacquiring an old
object through a reused numeric HWND; subsequent host projection must retain
the exact target generation before exposing a pointer.

The existing WOW_WINDOW_LINKS_CCPU fixture gains assertions for live parent/
sibling values, retired generation after same-slot reuse, terminal cleared
edges/desktop parent and failure leaving output untouched after last release.
The parent export list includes the new read entry. This is ADAPTER-WOW-069
host adaptation, no mirror/guest change, no new registry and no new sampling
policy. Source/tests are staged, unbuilt/unrun. Host WND construction and
atomic pointer publication remain incomplete; this reader alone does not
establish relationship closure or permit product publication.

### Exact relationship-target retention

Lifecycle review confirms class_is_referenced also inspects retained window
backings, and terminal withdrawal clears next/child/owner while retaining
the desktop parent. The host projection still must not turn a returned old
full HWND into a new allocation through slot lookup. Added the adjacent
borrow_window_relation operation: an already borrowed source pins the exact
target backing named by its committed relationship. Existing retired target
records remain eligible; only a cold live target uses full identity plus
allocation validation before creating its first ordinary borrow record.
No native query, ownership transfer, persistent edge reference or second
registry is introduced. Callers release the target with existing release_window.

This is a synchronous execution-owner operation, not a general concurrent
lookup. Null edges succeed without a pin; invalid relation/source and count
overflow fail without publishing output. CCPU fixture assertions now cover
next/parent target identity, paired release, null child and rejected relation
with untouched output. Export selected beside read_window_view. Both new
read/retention operations and assertions remain unbuilt/unrun; their host
pointer-projection consumer is not implemented yet. ADAPTER-WOW-069 covers
this existing generation-owner extension; no mirror or guest modification.

### Scope integration of exact relation pins

wow_user_borrow_scope now shares one retain_window_generation helper between
ordinary sampled WW lookup and relation-target borrowing. The helper consumes
the exact generation pin on every path and reuses existing nested-scope
records; it performs no native HWND lookup or resampling. Existing enrolled
or promoted bindings preserve their captured native thread identity instead
of being rejected by the observation-only setter. A mismatch remains an error.

wow_user_borrow_window_relation pins its source generation, obtains the exact
edge target from the page-domain owner, releases the temporary source pin,
then transfers target retention to the current borrow scope. No property is
attached, no task ownership inferred, and no target WW refresh overwrites
managed private state. Empty edges return TRUE/NULL; failure does not replace
the caller's output. Native-creation fixture adds empty-edge/invalid-edge and
retained-old-source assertions around its deterministic generation-reuse case.
All new scope/relationship tests remain unrun. Host WND construction and
pointer publication still need this retained binding's consumer, so the
relationship feature is not declared complete or delivered.

### Lifecycle verifier stale-object closure corrected

Before starting the accumulated relationship regression group, reviewed its
actual producer tests/observation/verify-wow-task-lifecycle.ps1. It extracts
all provider object paths from the selected DLL link command and archives
them into provider.lib, but previously requested only task-lifecycle plus a
few case-specific objects from Ninja. Other changed selected sources could
therefore remain stale in that archive. This is a verification defect; it
does not establish that any particular past result used stale inputs.

The script now asks Ninja to update every selected production object before
archive construction, in bounded32-target command batches to avoid cmd.exe
length limits, then checks file presence. It keeps incremental dependency
caching and does not force the full DLL link or add unselected sources.
Fault-specific fixture substitutions still happen afterward. This closes the
known stale-object omission but does not by itself validate graph provenance
or all compiler flags. The updated verifier and pending relationship group
have not run; no result from old objects is reported as current acceptance.

### Whole original control-body compile: original-bodies-r1

New explicit original-control-bodies target in the supplemental generator
compiles all seven EDIT, ten combo/list and three dialog translation units
through the staged shared precomp. It does not select production bodies,
link fixtures in their place, or modify guest inputs. All objects stay in
build/M0-T447/S3/call-boundaries-x86/obj. Background runner PID39500,
Ninja PID19636, 2026-10-16T20:46:56.8117690Z through
20:47:47.9973408Z, exit2. Ninja -k0 attempted the complete group; eighteen
units report compiler errors. This is failed integration evidence, not
runtime acceptance. Logs: build/M0-T447/S3/original-bodies-r1/build.stdout.log
and original-bodies-r1.supervisor.log. No product publication occurred.

The finite next composition group is now grounded in actual compiler inputs:

- Original EDIT scalar declarations (undo types, modifier constants,
  allocation/text limits) are absent from the imported usercli subset.
  Read-only source trace identifies original client/usercli.h lines577-619
  as their owner; recover declarations, not replacement algorithms.
- Combo/list item declarations, selection constants and directory globals
  are similarly absent. Trace their original declaration/storage owners;
  do not invent independent copies of mutable state.
- Task client information is only forward-declared at original-body use;
  the owned initialization view has dwTIFlags/dwExpWinVer/dwCompatFlags but
  no CI_flags. Merely including its header cannot resolve that semantic
  input. Establish the original CI_16BIT producer and appropriate owner.
- Dialog private templates, control/window casts, screen inputs and SEH
  vocabulary are incomplete; modern public API name collisions also emit
  warnings. Resolve signatures and exact scope before linking bodies.

These are declaration/service closure work inside existing S3, not new
features. Compiler diagnostics include cascades and do not count independent
runtime bugs. No blanket macro stubs or guessed private state were added.

### Recover original scalar/item declarations; preserve clipboard gap

Source-traced declaration batch restores EDIT allocation/text-limit,
modifier and undo constants plus list allocation/search/selection constants
and LBItem/LBODItem records in the existing usercli subset. Registered under
OPENNT-HOST-074/086 in src/opennt-host/README.md with source hash and ranges.
No original C-body changes or guest inputs; no mutable storage or autonomous
behavior introduced. This batch is not rebuilt while the shared declaration
and service-input group remains incomplete.

The CI_flags compile failure is not merely a missing typedef. Original
editec.c:2125-2137 documents that after EmptyClipboard the 16-bit task must
invoke pfnWowEmptyClipBoard to invalidate WOW's cached 16-bit clipboard
handles. The current task initialization view lacks CI_flags, so adding a
zero-filled field would silently suppress required behavior. Existing
GetClientInfo supplies the active task's version but its clipboard identity
input still requires a source-backed producer/lifecycle binding. Do not
infer that identity from executable bitness or weaken the unchanged guard.

### Shared extent and task-info composition follow-up

The whole-body precomp now selects the already registered control-extent
declarations (COMBOWND, STAT/STATWND included), reusing the same source used
by wow_control_registered_extent. A duplicate combo declaration considered
during review was removed before delivery. LBUP_* release flags and original
timer/key constants were recovered with source ranges/hashes registered in
the mirror README. Including the existing task-init header exposes its real
version/flags members without inventing CI_flags or a second task record.

Source review confirms lboxctl3's directory-label fitting reads STAT.hFont
through FNID_STATIC. Declaration availability does not prove a host-observed
static has that original behavior state: the window-state ownership audit
must retain that distinction before enabling the complete control group.
The canonical edit entry already requires an active TDB and registered
pfnEmptyCB; this identifies the available task/callback owners but is not
proof of the original CI_16BIT producer. That binding remains unfinished.
No new build/test or product publication is claimed for this batch.

### Original resource ABI closure (OPENNT-HOST-105)

The repository already contains original winuserp.h in its admitted ABI
mirror. Its extended dialog resource structures were not exposed through
the narrowed control-client precomp. New bounded controlabi.h retains the
unchanged DLGTEMPLATE2/DLGITEMTEMPLATE2 field layouts and two-byte packing,
plus EDIT/directory scalar constants. Whole private SDK inclusion would
bring unrelated declarations into the modern SDK environment. The retained
original dlgbegin parser remains the consumer; no new parsing algorithm.
Source hashes/ranges and the declaration-only exception are in mirror README.
The precomp asserts original sizes26/24; assertions are staged, not executed.

The existing usercli subset now also exposes original word/dword resource
alignment macros and RepositionRect's void signature. This resolves the
identified declaration omission without changing its original body.
Original UTCHAR Unicode selection is retained in the inc/user.h subset.
No rebuild was started: remaining shared private operands/service inputs,
CI_16BIT identity and host WND graph/state closure remain in the finite S3
group. This is not evidence of runtime acceptance or a released product.

### Directory globals, brush operands and build language contract

Recovered four directory-string definitions from original clglobal.c in
that existing sole global owner. Their externs, CCHFILEMAX, list/private
message constants and dialog style operands are now exposed to the retained
bodies; mirror README registers source ranges and hashes. Source review
shows HIGHLIGHT/WINDOWTEXT errors originate in the missing SYSHBR macro,
not missing color enums. Its original token-pasting expression now uses the
existing system-brush table; edit entry already requires drawing_ready.
No numeric replacement HIGHLIGHT constant or new brush cache was added.

Original list exception guards now retain actual __try/__except/__finally
compiler syntax via the shared precomp; no exception handler was removed.
Producer audit found clglobal still inherited ANSI WOW compiler flags, which
would miscompile its recovered WCHAR TEXT literals. Its build rule now
selects the existing Unicode control flags only for that original USER unit.
No full build or runtime pass is claimed; all changes await grouped closure.

### Task identity projection for the original clipboard guard

Original retained kernel/queue.c:228 sets TIF_16BIT during xxxInitTask before
queue attachment; line259 publishes task flags to the client view. This
existing scheduler/task owner is the authority for whether the bound caller
is a16-bit task. The native TEB CI_flags producer was not found in the
inspected source; no equivalence of NT4 and modern TEB layouts is claimed.
ADAPTER-WOW-061 now projects the bound task's TIF_16BIT into the original
CI_16BIT value0x4 at wow_edit_client_info access. The added owned view field
has no independent flag writer and is not native CLIENTINFO storage. The
original editec clipboard condition and registered pfnEmptyCB are unchanged.
Version and compatibility fields keep their original owner.

The shared precomp exposes those task constants and original VER40. Staged
fixture assertions toggle task membership and check projected-bit refresh,
unchanged version and last-error preservation. They have not run. Actual
EDIT clipboard cache invalidation still requires grouped guest acceptance.

Separate source trace: dlgmgr foreground tests compare input queue identity,
not merely window PID or thread ID (original ntstubs.c:6274/7846). No thread-ID
approximation was implemented. That native service binding remains open;
it must handle attached queues without creating a second queue authority.

### Native modal-return foreground chain recovered

Read-only symbol/byte inspection now replaces the earlier nearby-call guess.
Current x86 user32.dll SHA256
56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E,
matched PDB identifies DialogBox2 at RVA8965F. Its modal-return tail:
10089886 calls NtUserDestroyWindow (IAT100EF180); 10089890 pushes10 and
10089892 calls NtUserGetThreadState (IAT100EEC30). A zero skips restoration.
1008989C pushes7 then owner HWND;1008989F calls NtUserQueryWindow
(IAT100EEBAC). A nonzero skips restoration. Otherwise100898AA calls
NtUserSetForegroundWindow (IAT100EEBBC) with that owner. This is the same
conditional operation ordering as retained dlgmgr, not the earlier unrelated
operand14 thread-state calls. No private WND offsets from the inspection
are used in the adapter.

Current x86 win32u SHA256
31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C:
named GetThreadState export RVA1190 ends ret4 at119C; QueryWindow RVA1270
ends ret8 at127C. Direct import decoding and dumpbin ranges establish named
service arity and actual modal query operands, not universal version ABI.

ADAPTER-WOW-062 now resolves both named exports with its existing all-or-none
native dialog dependency group. Finite predicates issue native operations10/7;
missing dependencies raise the existing adaptation failure. Private precomp
maps the two original symbolic queries to those predicates, not old numeric
enums12/6. No original dlgmgr body diff, native queue mirror, thread-ID
comparison, guest modification, syscall ordinal or runtime code patch.

Not yet built/run. Attached-versus-detached queues, owner destruction/reuse,
native errors and final foreground ownership remain grouped acceptance
requirements. Static control-flow evidence is not runtime equivalence proof
and does not establish support on uninspected hosts. Existing native-service
compatibility governance remains unchanged; no new refusal policy was added.

### Shared screen input: remove divergent dialog-only sampling

Original client/clinit.c:937-944 constructs DISPLAY IC and initializes
rcScreen to0,0,DESKTOPHORZRES,DESKTOPVERTRES. Source SHA256
83C7E304B38501C9AFD1251C2CF671F1BC0E799560AF10B225CEDEE891E3E8A2.
combo.c:1715 uses its bottom to choose downward/upward dropdown placement;
original dialog clipping also consumes this input. The staged adapter had
only a dialog-specific SM_CXSCREEN/SM_CYSCREEN sample, leaving combo without
the input and changing its source. That autonomous sampling is removed.

ADAPTER-WOW-058 now captures the same DISPLAY device-cap inputs in the shared
control invocation context. ADAPTER-WOW-063 dialog capture reuses that record
without a second sample/storage; scoped precomp selects the proper existing
invocation for each original body. No layout algorithm, monitor selection or
guest code is changed. Native Windows owns the actual resource; the view is
invocation-local, never a competing desktop state authority. Failure leaves
caller output unchanged; successful capture preserves last error; IC is
released before any subsequent acquisition/failure.

Staged native-context test compares the dimensions with a separate DISPLAY
IC; dialog-context double uses distinctive1234x987 dimensions and checks
that dialog capture preserves them rather than resampling. These assertions
have not run. No compile/runtime/visual acceptance or publication claimed.

### Whole-group compile original-bodies-r2

After the accumulated declaration, resource, task-info, foreground-service
and screen-input batch, rebuilt all20 original bodies plus nine supplemental
fixture executables in one hidden owned job. Runner30036, Ninja15280,
2026-10-16T21:02:36.3400665Z through21:05:58.7923787Z (202.45s), exit2.
Logs: build/M0-T447/S3/original-bodies-r2/build.stdout.log and sibling
stderr/exit.txt; supervisor original-bodies-r2.supervisor.log. All52 Ninja
steps attempted. Nineteen original body compilations complete; dlgbegin
has13 compiler errors. The runner deliberately skips execution on any group
build failure, so no newly passing fixture or product acceptance is claimed.

Remaining hard errors are now in the original creator: DS_VALID40,
DS_COMMONDIALOG, cursor-position input ptCursor, DIALOGCLASS, SCDLG_*, CODEBIT,
ICLS_COMBOBOX and DFNOFAILCREATE. Cursor state is a host service input, not
permission to add a zero point or substitute a stale cached location.
The log also contains95 C4013 implicit-function warnings, including
ValidateHwnd, _GetClientRect, _IsWindowVisible and original collaborator
entries. They require source-backed declarations/bindings even where x86
currently emits an object; integer/pointer assumptions are not ABI proof.
Full service/identity/graph closure and original bodies in production remain
unfinished. Do not promote the nineteen compiled objects to product support.

Before this group, ADAPTER-WOW-057 added a native GetSystemInfo leaf for the
original EDIT creation address discriminator, preserving last error and
avoiding a fixed NT4 user-address ceiling on a modern host. It is not a
pointer-validation service. Existing same-address REBASEPTR remains the
pointer representation contract. That native leaf and the new foreground
queries are not exercised by these supplemental call doubles; actual native
and guest acceptance still belong to the full group. No guest mutation,
publication, commit or push occurred in this run.

### Original creator and collaboration declaration batch after r2

Counted r2 C4013 diagnostics by exact name:95 occurrences,47 unique names
(not49). Recovered original DS/SCDLG/class/style/resource operands and
original EDIT/Combo collaborator signatures. Original user.h shared geometry
and text declarations are now visible through the control-client declaration
selection. README expands OPENNT-HOST-105 with exact source ranges. This
removes implicit-int assumptions at their source; it does not imply every
declared original implementation has already been selected/linked.

The ptCursor omission is a service-input gap rather than a missing constant.
Original dlgbegin.c DS_CENTERMOUSE reads shared current cursor state when it
decides placement (after earlier resource/creation activity). ADAPTER-WOW-062
now supplies GetCursorPos exactly there via a POINT-valued private-precomp
binding, with last-error preservation on success and the existing adaptation
failure path on failure. No entry-time stale cursor snapshot, fixed origin,
new placement algorithm, global cursor mutation or original creator-body
change. Actual native and guest tests remain pending; no rebuild/publication
was started during this declaration/service-input batch.

### Original string dependency closure after r2

Source tracing found four missing bodies, not missing host services:
dlgbegin requires RtlInitLargeUnicodeString and wcsncpycch; lboxctl3 requires
the initializer, FindCharPosition and TextCopy. Existing selected chartran.c
had omitted its final initializer; rtl/random.c had no selected string slice.
OPENNT-HOST-105 now retains those four complete original bodies with source
identity and finite closure recorded in the mirror README. Original ACP macros
now route control-client MBToWCS/WCSToMB to already selected original Ex bodies.
No new conversion/copy algorithm, host-private pointer interpretation or guest
change. Provider support selection adds rtl/random.c; chartran was selected.

Read-only source review extracted balanced function bodies from the pinned
comparison and local files and removed whitespace for comparison. All four
reported ORIGINAL_BODY_MATCH; git diff --check exits0 (line-ending warnings
only). This is source-equivalence evidence, not compile or runtime evidence.
wow_control_string_fixture.c and original-string-boundaries Ninja target use
actual retained bodies and the existing allocation/NLS binding. Assertions
cover found/not-found, empty capacity, single-character capacity, truncation,
null input, odd byte limit, copied length and the original non-padding/no-NUL
on truncation wcsncpycch contract. The test is unrun, queued for the complete
control closure build; no isolated compile/test or publication was performed.
Remaining original-control graph, creation provenance, CPD normalization and
native boundary/lifecycle requirements are unchanged. This does not claim S3
completion or functional original-control selection in the product.

### Original dialog navigation closure and creator wiring

Post-string source tracing found dlgbegin/dlgmgr call _GetNextDlgTabItem but
the client/dlgmgrc owner was absent. OPENNT-HOST-106 recovers its five-function
required closure plus original rtl/winmgr.c _IsDescendant, with pinned hashes
and rejected unrelated public/group wrappers in the mirror README. Original
depth-first container traversal, filtering, reverse lookup and cycle stopping
remain unchanged. This is not a delegation to modern GetNextDlgTabItem.
Read-only balanced-body comparison against the pinned source reports
ORIGINAL_BODY_MATCH for all six functions. No server allocation, desktop
creation or native private-layout dependency is imported with them.

The borrowed graph is still an explicit prerequisite: validation currently
only reaches enrolled owned WND, while external/desktop relation construction
is unfinished. Mapping ValidateHwnd to that narrow validator would conceal a
real missing dependency, so no such mapping was made. The new navigation
fixture deliberately supplies consistent original WNDs and asserts only the
algorithm, not production object publication. It joins the grouped Ninja
graph as dialog-navigation-boundaries, with actual original bodies; no test
has been run for this addition. The original-body group now contains21 units.

A second source-verified wiring omission was the original dialog creator's
14-slot NtUserCreateWindowEx call: its existing enrolled adapter was defined
but the client precomp had not selected it. That binding is now explicit.
Native17-slot service remains behind the existing adapter; no original WOW
word array is sent into modern USER. Source/body checks are not proof of a
working dialog: creation provenance, full WND graph, CPD and native-return
acceptance remain open. No product build/publication/commit/push this batch.

### Host relationship consumer closure, without fabricated validation

Reviewed current attach_client_state, project_observed, retain_window_generation
and borrow_window_relation together. Existing page-domain generation ownership
and scope pins are available; project_observed updates scalar/name fields only.
No host spwndNext/Parent/Child/Owner assignment exists in the two current WND
owners. Therefore the required graph is not published, and enrolled-only
validate_hwnd cannot be substituted for general original validation.
Observed binding promotion also reuses the same WW allocation; a proposed
observed host WND must preserve that alias and must not acquire original
creation flags merely by being sampled. This remains the same finite graph
obligation, not a new registry or task expansion.

The adjacent original consumer audit found five declarations without their
retained implementation: _IsChild, _ClientToScreen, _ScreenToClient,
GetTopLevelWindow and IsVisible. Recovered the complete original bodies in
their existing rtl/wow.c and rtl/winmgr.c owners under OPENNT-HOST-106. All
five pass read-only balanced-body/whitespace comparison with the pinned
source; README records hashes/ranges/callers. No algorithm edits, no native
private data and no guest modifications. Actual-body edit-view assertions
and grouped Ninja selection now cover these leaves. Tests remain unrun;
production graph, creation and return-state acceptance remain required.

### Behavior-state provenance before resource WND publication

Reviewing observed-to-enrolled promotion showed that `enrolled` is current
property membership, not durable original-state provenance: detach clears it,
while an outer callback may still hold the old WND. Conversely the presence
of a WND pointer cannot prove ownership once resource-only projections exist.
ADAPTER-WOW-063 now records behavior-state provenance only at successful
attach_client_state publication and tests it together with exact allocation
identity. wow_window_client_owns_state uses that owner query, not merely a
successful scope lookup. The field is a capability discriminator, not a copy
of host geometry, style, native validity or class state; it is never inferred
from a class name, FNID or native thread ID. It is not cleared by retirement.

The existing window-class-lifetime fixture adds rejection of unpublished
storage/wrong pointers and acceptance of the exact retired original allocation
while rejecting the replacement HWND binding. Assertions are staged/unrun.
This prepares the existing owner for read-only resource projections; no such
projection or full relationship publication is claimed yet. No mirror diff,
new registry, additional lock, guest change or runtime publication this batch.

### Resource WND construction and same-allocation promotion

ADAPTER-WOW-063 now has prepare_resource_state at the existing window binding
owner. It pins that binding's exact generation and reads the committed view;
it never resolves the numeric HWND anew or samples native USER. Only handle,
rectangles, styles, module, numeric child ID and class atom are projected.
Desktop FNID requires the caller's explicit desktop identity. No creator
version/encoding, dialog initialization, procedure, title or control-private
state is fabricated. This is a constructor, not the missing graph/scalar
refresh transaction. All fallible work precedes publication, failure frees
the candidate and preserves existing state; the generation pin is released
through the existing owner even on unwind.

Observed WND allocation includes capacity for the observed class extra extent,
without exposing that capacity as original private cbwndExtra. Later genuine
creation builds its ordinary original-state candidate, and attach_client_state
copies it into the existing retained address, preserving current relation
pointers. The temporary candidate/name transfers only on success; insufficient
capacity rejects without destroying either allocation. No realloc of an
already borrowed PWND. Class identity then switches from controlled atom-only
projection to the existing retained original class owner. The create/ensure
paths distinguish resource presence from owned state, and the private HWND
validator rejects resource-only state. Thus observing a native window cannot
silently enable private control words or suppress original initialization.

The lifetime fixture adds rejection of resource construction without a retained
generation. Positive resource construction, promotion/capacity failure, nested
retirement and full relationship publication are still unrun/unimplemented
acceptance portions respectively; no green claim is made. The constructor is
not yet called by production graph traversal. No mirror or guest change, no
product build/publication/commit/push. The original execution algorithms remain
unchanged; this is generation-owned data/lifetime adaptation only.

### Prepared host relationship graph and non-fallible publication

ADAPTER-WOW-063/069 now supplies borrow_client_graph at the existing scoped
borrow owner and publish_client_graph at the existing WND owner. Traversal
starts by retaining the root's exact backing, follows the committed four
relationship edges through borrow_window_relation, and deduplicates bindings
in call-local growing scratch. No HWND lookup/native sampling, second registry
or independent tree policy is introduced. Every row captures rectangles,
styles, module, child ID and class atom from the same retained generation.
Resource construction is completed before any relationship pointer changes.

The publication validates every expected allocation, source handle, row size
and target index before its first write. It then installs scalar projections
and all four pointer fields with no allocation, native call or fallible work,
under publication_lock and the caller's required USER execution ownership.
Original FNID/compatibility/control state is preserved. Failed preparation or
row validation leaves prior relationships/scalars intact; prepared resource
allocations may remain in their existing generation owner, but do not become
original writable control state.

Nested refresh requires more than retaining until the inner scope leaves:
shared WND links remain updated when suspended outer original code resumes.
Before publication, graph generation pins are transferred to the outermost
active borrow scope. This retains newly introduced targets across inner return
without restoring stale links, creating permanent edge refs or reference
cycles. All scratch is freed in finally; normal scope exit owns every pin.
The source graph is finite/deduplicated even with parent-child cycles; original
consumers still depend on the existing validated snapshot's tree semantics.

window-class-lifetime adds actual publisher assertions for a two-allocation
graph, preserved last-error, invalid later row rejecting earlier scalar/link
updates, and explicit clearing. These assertions are unrun and do not prove
generation traversal or nested pin transfer. Required remaining integration:
invoke after original-entry preparation and after native-return observation,
including finalization, with the established execution/borrow owner; validate
those paths, promotion and allocation failure using the complete group. No
production entry is enabled merely by adding these APIs. No mirror/guest
change or product build/publication/commit/push occurred in this batch.

### Original-state ownership review after relationship wiring

The entry/return wiring is now staged: EDIT/ComboBox/ListBox ordinary entries
and their original finalizers prepare the retained graph; ordinary dialog
entries do likewise. Canonical dialog final destruction retains its existing
non-allocating early cleanup path. Native-call return refreshes the combined
owned roots in active borrow scopes after observation publication, before
resuming original code. This is source wiring, not execution evidence.

The single-person review found a definite unpublished constructor regression:
wow_window_client_create_state tested owns_client_state immediately after
HeapAlloc, before attach_client_state could transfer ownership. A successful
new allocation therefore failed as ERROR_NOT_ENOUGH_MEMORY and leaked. Restore
the allocation's NULL check; ownership is established only by successful
attachment. This is a project-adapter error, not an original algorithm defect.

Conversely, the private HWND validator and original-state refresh still tested
pointer presence rather than original-state capability. They now reject a
resource-only projection using owns_client_state. The public extra-word
get/set adapters, dialog pointer publisher and control finalizer all consume
that validator, so observation alone cannot admit original private storage.
Generic retained graph reads deliberately still accept resource projections;
their pointer-presence checks must not be mechanically replaced with ownership
checks. Construction, borrowed resource identity and original private-state
ownership are three different conditions.

The existing real-HWND creation fixture now explicitly asserts an initially
empty state, preserved success error, acquired ownership, successful private
validation, and rejection of duplicate construction without replacing the
published allocation. Assertions are staged and unrun. The full fixture's
new finalizer graph prerequisites also need their intended runtime closure;
no native-only double is claimed to prove graph lifetime.

Default-worker call tracing confirms clmsg.c consumes parent/owner pointers
for HELP, MOUSEWHEEL and CONTEXTMENU. Do not solve its remaining entry coverage
by reacquiring a PWND through its numeric HWND: a retained state may outlive
nested destruction and handle reuse. That boundary, creation provenance,
positive generation/promotion tests, CPD and original-body composition remain
in the existing finite S3 closure list. No new mirror diff, guest change,
build, test pass, publication, commit or push is claimed by this review.

### Default-worker entry closure without another graph scan

The selected client call-site sweep resolves DefWindowProcWorker users to
combo.c, edecrare.c, editec.c, editml.c, editsl.c, lb1.c and dlgmgr.c.
The control procedures and caret entry use edit_entry; ordinary dialog default
and EndDialog use dialog_entry. Their graph preparation precedes original
execution. Canonical dialog WM_FINALDESTROY directly enters DoCleanup, not
the ordinary default-worker call sites. ECGetControlBrush's separate native
recipient case retains its explicit approved color boundary. These are source
coverage findings, not proof of original-body production selection.

Therefore do not add another HWND refresh or full graph traversal inside
every default-worker call. The wrapper now retains the exact incoming WND
for every message through wow_user_borrow_client_state, rather than doing
so only for GETTEXT/GETTEXTLENGTH. That existing owner searches retained
allocations first and rejects a replacement allocation on fallback lookup.
The name-reader uses the same retained binding; no second lookup, new mapping,
new snapshot owner or behavior engine is added. An invalid borrow stops before
context publication or original dispatch. Native call returns continue to use
the already staged combined graph refresh for suspended original roots.

The boundary fixture now checks one exact-state lookup per default entry,
including failed context capture, and rejection/unwind of a non-text HELP
entry when its state cannot be retained. Existing nested TLS restoration,
name-lock exception and narrow native-color assertions remain. These assertions
are unrun; the real retained-generation/retirement/promotion group is still
required. Original creator validation/provenance and CPD remain separate
existing composition gaps. No mirror or guest changes, product build,
publication, commit or push in this continuation. git diff --check passes.

### Control composition: public font macros and existing RTL transform

The r2 implicit-function inventory contained SelectFont/DeleteFont and both
RtlRunEncodeUnicodeString/RtlRunDecodeUnicodeString. Original client/precomp.h
includes windowsx.h; public/sdk/inc/windowsx.h lines57--58 define the font
macros as typed SelectObject/DeleteObject calls. Installed SDK10.0.22621.0
definitions match. The shared control precomp now includes that SDK header,
preserving the original dialog creator's font lifetime/order without a
mirror change or replacement functions.

ADAPTER-WOW-057's native declaration boundary now declares the RTL functions
with original public/sdk/inc/ntrtl.h signatures (VOID NTAPI, PUCHAR seed for
encoding, UCHAR seed for decoding, PUNICODE_STRING buffer). The existing
NTDLL import-definition and x86 stdcall alias owner selects both @8 imports.
Original editec.c ECLock/ECUnlock still owns first/final-lock decisions,
encoded flag, seed, LocalLock lifetime and counted byte extent. No new loader
or transform implementation is introduced.

Source: pinned OpenNT base/ntos/rtl/sertl.c encode/decode bodies and
base/ntdll/ntdlldef.src exports266--267. Original Windowsx SHA256:
0662A1073D652A424355914051398246580074703DEDC2339F68B2DF808F2FCD.
Original sertl.c SHA256:
D0AD63A50590933326E2F2F9AA56651B9C8A69EF9DAA25B176CAD047ECC6B0D2.
MSVC14.43.34808 Hostx64/x86 dumpbin confirms current SysWOW64 NTDLL exports
both names (RVAs DDB30 decode, DDB90 encode); DLL SHA256:
23D7772221E7A763CCF3F3FE39592086EB3091603554C4981F0AB430FA2CA686.
This proves export availability, not behavior. PATH dumpbin failed to find
LINK.EXE; the explicit MSVC tool produced the evidence. No test was run.

The existing grouped string fixture (linked with ntdll.lib) now asserts
the original fixed-seed vector, zero/odd/even byte extents, untouched tail
and metadata, roundtrip and nonzero generated seed. Assertions remain unrun;
they do not prove original EDIT password-control execution. Body selection
and remaining finite dependencies still block product composition. No new
mirror diff, guest change, build, publication, commit or push.

### Window validation contract: two entries, not three algorithms

Read-only source trace resolves client/usercli.h:165 RevalidateHwnd directly
to ValidateHwndNoRip. client/wow.c:154--188 defines both window validators:
first return the current non-NULL CallbackWnd when its HWND matches, otherwise
call HMValidateHandle or HMValidateHandleNoRip with TYPE_WINDOW. In
rtl/wow.c:256--339 both consume the same ValidateHandleMacro. The ordinary
failure sets ERROR_INVALID_WINDOW_HANDLE; NoRip returns NULL without that
error-reporting tail. Revalidate is not a stronger fresh-host-validation mode.

The shared macro checks the handle-table index, uniqueness (including the
original zero/all-ones short-handle allowances), object type and desktop
mapping/access. Its NT4 shared table, desktop pointer delta and native private
mapping service are unavailable host mechanisms, not portable pointers.
Do not import that table implementation or cast modern USER storage into it.
The existing page-domain/current-generation owner supplies the corresponding
identity input under the approved observation boundary.

The current wow_window_client_validate_hwnd instead means enrolled original
private-state access. Keep that restrictive contract for get/set, PDLG and
finalizers; do not alias the general original ValidateHwnd names to it. General
validation must permit resource-only desktop/native relationships without
giving those objects control-private ownership. Exact-PWND retention after
retirement is also distinct from current-HWND lookup.

Finite implementation constraints for the existing validation gap:

- One general identity resolver, two reporting wrappers; Revalidate remains
  the original alias. Do not add a third resolver or a new permanent map.
- The callback fast path must identify the innermost actual window callback,
  not any old object retained by an outer borrow. Current task_callback_scope
  carries execution ownership only (binding/held/resumed); it does not yet
  establish CallbackWnd. An arbitrary retained-window lookup is insufficient.
- Preserve nested callback restoration and exact backing lifetime in the
  existing scope owner. Outside that fast path use the committed current
  generation, not a stale same-HWND borrow; apply reporting only after shared
  resolution so NoRip does not acquire the ordinary invalid-window error.
- Acceptance includes current callback, nested other-window callback, native
  resource-only window, wrong/stale handle, post-return destruction/reuse,
  short-handle forms and last-error preservation. Existing private-validator
  tests alone cannot prove this general contract.

Pinned client/wow.c SHA256:
1ECE9DB0E35238434843BB73ECE92DD66000A44BC6B1148A08BCABF1329F8D9A.
Pinned rtl/wow.c SHA256:
71E5B523BD44077AB4E86FFC6093544922C815C759BB753C32B4155E3206691D.
This trace narrows the pending implementation to the original two-entry
contract and prevents an incorrect blanket alias. It does not claim that
the resolver/callback binding is implemented or tested. No product delivery.

### Reuse the existing callback identity owner

Further cross-component tracing finds the existing page-domain callback pair:
callback_window publishes guest TEB CachedHWND/CachedWND; restore_callback
restores the outer pair. dispatch_bound already uses it for guest window
callbacks. Thus the absence of CallbackWnd in task_callback_scope does NOT
justify adding a second TLS identity owner. Reuse this pair for host original
validation too, and extend missing publication sites within the same owner.

The staged borrow_callback_window API now returns a retained exact backing
for an exact nonzero HWND match, or success/zero when no callback matches.
It validates the active execution domain and the existing backing record
before dereferencing a cached address. A retired cached generation may differ
from the current handle-table slot; it is retained directly. A cold live
generation is accepted only when the existing current lookup identifies that
same allocation. Reference overflow and inconsistent state fail without
changing the caller's output. Successful lookup preserves last-error and
transfers one ordinary borrow to the existing release_window owner.

The existing NTVDM export list includes this narrow API. The page-domain
lifecycle fixture now checks retired callback borrow, same-HWND replacement
inside a nested callback, restoration of the old backing and no-match after
outer restoration. The test releases every acquired reference and checks
success error preservation. These assertions are unrun; they extend actual
page-domain lifecycle coverage, not a new mocked callback registry.

General ValidateHwnd consumption and remaining control/dialog callback
publication sites are still pending. This API alone does not connect them or
prove full entry restoration. No new mirror diff or guest modification; no
build, publication, commit or push. Static diff checking passed.

### Recovered control/dialog callback publication and automatic unwind

The existing borrow scope now optionally saves the previous page-domain
callback pair; it does not hold a competing current-window record. A new
set_callback operation admits one actual recovered window entry per scope,
retains its enrolled binding/generation, then publishes through the existing
page-domain callback_window owner. Failure leaves publication inactive;
duplicate publication rejects rather than overwriting the saved outer pair.
Ordinary temporary borrow scopes do not publish a callback.

Scope leave restores the outer pair before changing current_scope or
releasing any backing. It uses the existing invariant-failure outcome if
restoration fails, preserving the caller's error on successful leave. The
existing finally blocks therefore cover normal return and exceptions without
another per-entry restoration implementation.

Recovered EDIT/ComboBox/ListBox/caret entries publish after graph preparation
and before invoking the original procedure. Ordinary dialog-message entry
does the same. EndDialog is an API operation, not a new callback, and canonical
final cleanup is not an application callback; neither replaces the current
pair. Existing guest dispatch publication remains with its current owner.

The actual page-domain observed/enrolled fixture now exercises nested scoped
publication, duplicate rejection, unchanged identity across an ordinary inner
borrow, inner restoration and final clearing, releasing its extra pin before
the existing retirement/free-accounting assertions. The dialog entry-order
double additionally asserts publication before ordinary original dispatch.
These tests are staged, not run. The general original ValidateHwnd consumer
and the complete coherent build remain open; this is not acceptance or a
delivery. No original mirror or guest modification was made.

### General WND storage resolution from the existing identity owners

The borrow owner now exposes window_state as the storage/lifetime input for
general original validation. It first borrows the existing exact callback
pair; otherwise it resolves the committed current window generation and
checks full-handle uniqueness. Zero/all-ones high words follow original
rtl/wow.c ValidateHandleMacro, rather than treating sign extension of only
negative SHORTs as the sole all-ones form. A stale non-short full handle is
rejected even when its index is live. No native HWND resampling, property
enrollment, new registry or invented creator flags are involved.

Both paths consume one generation pin through retain_window_generation and
prepare the reachable host WND graph through the existing publisher. This
also retains newly required targets through the outermost active scope.
Resource-only state remains resource-only; general identity resolution does
not promote private control authorization. Success restores entry last-error;
invalid identity and preparation failure remain distinct. The caller must
already own USER execution; the original reporting wrappers and their entry
lock boundary are still pending and are not silently aliased to the private
enrolled-state validator.

The actual observed/enrollment fixture now checks resource-only WND reading,
same pointer for both original short-handle forms, rejection of stale full
identity and NULL, last-error preservation, and no original-state capability
granted by observation. These assertions are unrun. The remaining original
validator wrappers, creation entry/provenance and grouped lifecycle build
remain required; no product behavior or delivery is claimed. No mirror or
guest change. Static diff checking passed.

### General original validation callers connected

ADAPTER-WOW-063 adds the general reporting entry and its NoRip companion in
the existing window-client-state owner. Both use window_state, acquire the
existing runtime metadata lock only when not already held, and restore entry
lock ownership in finally without holding it through original application
callbacks. Success preserves last-error. NoRip suppresses only the ordinary
invalid-window error; allocation, domain and invariant failures remain
diagnosable rather than masquerading as a missing window.

The shared original control/dialog precomp now binds ValidateHwnd and
ValidateHwndNoRip to those entries and keeps RevalidateHwnd as the original
NoRip alias. It deliberately does not map them to validate_hwnd, which guards
original private control ownership. Resource-only desktop/native owners can
therefore supply original navigation/relationship reads without acquiring
PDLG or extra-word write authority.

Recovery-ladder disposition: original client/wow.c's small validators depend
on GetClientInfo()->CallbackWnd and HMValidateHandle; rtl/wow.c's latter
implementation depends on NT4 shared handle storage and desktop mapping.
Those unavailable mechanisms are replaced at the existing identity owner,
not reproduced as another CLIENTINFO/handle table. Original callback-first,
current-generation/short-handle and invalid-handle reporting decisions are
retained in the named boundary. No original control algorithm is replaced,
and no private modern USER pointer is reinterpreted. The previously registered
source hashes and call-site inventory identify the original contract.

The real page-domain fixture now exercises general success and NoRip success,
ordinary invalid-handle reporting and NoRip last-error preservation alongside
resource-only/no-private-ownership and short-handle cases. This is staged,
unrun coverage. Current callback lifecycle assertions remain separately
required; a source binding does not prove real callback execution or complete
composition. No mirror diff or guest change, no build/publication/commit/push.

### Native USER connection and desktop-WND dependency

The remaining original ConnectIfNecessary call in dlgbegin.c is a native
GUI-thread connection prerequisite, not WOW scheduling. Original usercli.h
reads TEB Win32ThreadInfo and invokes NtUserGetThreadState(-1). The precomp
now retains its return-zero-on-failure control flow through the existing
ADAPTER-WOW-057 native owner using public IsGUIThread. No private TEB offset,
private numeric service or second connected-thread flag is introduced.

The adapter queries existing GUI-thread state, requests conversion only if
needed, and verifies the resulting state. It does not treat any nonzero
conversion result as success: Microsoft's IsGUIThread documentation explicitly
lists ERROR_NOT_ENOUGH_MEMORY as a possible conversion failure return.
Success preserves entry error; conversion failure returns a meaningful error.
Reference: [IsGUIThread contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-isguithread).

Original client/wow.c _GetDesktopWindow performs that same connection then
rebases the desktop WND from CLIENTINFO. Its named binding now connects through
the same native leaf and resolves GetDesktopWindow's identity through the
existing general retained-WND owner. Original list/default callers therefore
receive the same projected desktop object instead of a native USER pointer.
The control/default compile selections bind that operation explicitly.

The native ABI fixture adds a joined fresh-thread case, verifies real GUI
state, repeated connection and success error preservation. The actual-domain
fixture asserts a non-NULL desktop WND equal to general desktop resolution.
These are staged, unrun assertions, not proof of complete original dialog
creation. GetContextHelpId, CPD/provenance and remaining whole-group closure
remain open; no small build/test loop was started. No mirror/guest changes or
product build, publication, commit or push.

### Context-help inheritance and retained native property input

Continued the finite control dependency closure, not a new feature family.
Source: pinned OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c,
windows/core/ntuser/rtl/help.c, SHA-256
7ffe7d97c36f1aa6ab119eb7f0c9ca70fd8eb57cbefe322e40d0c5df041be16f.
Direct caller: retained client/combo.c WM_HELP branch, line1071. Original
GetContextHelpId reads the local private property, then follows TestwndChild
parent versus owner until a nonzero value or the desktop/end. The native
[GetWindowContextHelpId contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowcontexthelpid)
returns the specified window's ID, or zero when absent; it is only a leaf,
not an alternative inheritance algorithm.

Recovery ladder: retain the original complete traversal in its original
rtl/help.c path. The original private _GetProp/internal atom table cannot
read modern USER and importing that server table would violate the stopping
boundary. OPENNT-HOST-107 registers the one changed property-read expression
and two unrelated cropped functions, plus standalone includes. No changed
branch/order, no guest or original MVDM diff and no duplicate traversal.
The original algorithm remains selected as provider support, while the full
control-body production selection is still pending.

The page-domain's existing generation record is the sole retained observation
owner. Native capture and refresh supply context_help_id; graph preparation
and its non-failing commit copy it with the existing fields. Full view reads
return that stored scalar, including after retirement. No native pointer,
internal property atom or additional guest layout field is introduced.
The ADAPTER-WOW-063 leaf resolves the exact scoped PWND allocation, pins its
generation and reads under the existing USER execution owner, restoring the
entry lock/error state. It does not resample the HWND, so native destruction
or handle reuse cannot select a replacement object's help ID. Missing scope,
owner, pin or view is an invariant failure, not a fabricated zero. The existing
SetWindowContextHelpId native-return publication remains the mutation owner.

Staged assertions: actual original traversal in wow_dialog_navigation_fixture
with an explicit property-input double covers own ID, nearest ancestor,
owner, mixed CHILD/POPUP styles, null owner, desktop stop and direct desktop
input. The real CCPU40 page-domain observation fixture sets32 native help IDs,
checks committed values, changes the native value without publication and
checks the old view, then destroys/withdraws the native window while retaining
and reading its last committed ID. This separates algorithm evidence from
generation-lifetime evidence; neither fixture has run after these changes.
The production leaf's complete callback/lock integration and real guest help
request remain group acceptance obligations. No test pass, build, publication,
commit or push is claimed. Existing CPD/provenance and other finite S3 gaps
remain open; no small compile/test discovery loop was started.

Review evidence: a source comparison extracts both GetContextHelpId bodies,
substitutes only the registered property expression and removes comments/
whitespace; the remaining bodies match. git diff --check passes. CURRENT is
31516 bytes, below its32768-byte ceiling. These are static checks only.

The same source sweep of r2's existing47 implicit-function names (without a
new compiler run) distinguishes declarations from implementations. The current
tree has declarations but still lacks _GetDlgItem (client/wow.c:251),
_FindDlgItem (client/dlgmgr2.c:645), _AdjustWindowRectEx
(client/winmgrc.c:550), xxxRemoveDefaultButton and xxxCheckDefPushButton
(client/dlgmgr2.c:24/67). Original DlgSetFocus already exists in dlgmgr.c.
These are remaining entries in the existing dialog/default closure, not new
S scope or reasons to introduce native replacement algorithms. _GetDlgItem
is a direct-child ID scan; _FindDlgItem adds original WFWIN40COMPAT-gated
descendant traversal through the already recovered _NextControl. The two
default-button routines additionally need the existing scoped SendMessage,
validation and retained graph across callbacks. _AdjustWindowRectEx requires
original NeedsWindowEdge, the actual current task compatibility version and
the existing border/metric context. Recover and bind these as a dependency
group before claiming the implicit-declaration/build closure complete.

### Restore the dialog lookup, default-button and geometry cohort

OPENNT-HOST-108 now retains six complete original bodies, with exact source
hashes and crop/selection boundaries in src/opennt-host/README.md:
client/wow.c _GetDlgItem; client/dlgmgr2.c _FindDlgItem,
xxxRemoveDefaultButton and xxxCheckDefPushButton; client/winmgrc.c
_AdjustWindowRectEx; rtl/winmgr.c NeedsWindowEdge. Source extraction/comparison
against the pinned tree confirms all six bodies match after newline
normalization, without expression substitutions. Only crops, includes and
compile selectors change; no original MVDM/guest change or custom replacement
algorithm is added. Existing autonomous production providers for these exact
private workers were not found; declarations alone were not implementations.

Recovery ladder: these functions directly compose through the already admitted
control-client precomp, so no native GetDlgItem/AdjustWindowRectEx replacement
is selected. Direct lookup retains sibling order and numeric IDs. Descendant
lookup retains its original WFWIN40COMPAT gate and _NextControl traversal.
Default-button selection retains original WM_GETDLGCODE, DM_GETDEFID and
BM_SETSTYLE ordering, including same-focus Win4 repair and disabled-default
handling. Their original client ThreadLock macros are not claimed to pin
native HWND lifetime: existing outer graph borrows retain actual WND storage
through nested native calls, and native-return publication refreshes its
relationships before resumed traversal. Validation retains exact generations.
Real destruction/reentry validation remains a required group test.

Rectangle arithmetic retains NeedsWindowEdge's explicit/modal/static/sizebox
and version decisions, caption/menu adjustment and GetWindowBorders. The
current bound task supplies GetAppVer via existing CLIENTINFO; native metrics
and the established SPI_GETBORDER binding supply system inputs. Inspection
found creation/modal entry had dialog context but not GetWindowBorders'
default geometry context. The existing dialog context now owns that finite
geometry input when creating; both creator and modal wrappers install it and
restore their respective prior pointer in finally. Ordinary dialog teardown
still does not capture geometry or fail because preferences are unavailable.
No second persistent metric authority or window-layout engine is introduced.

Supplemental graph selection now compiles the original dlgmgr2/client-wow/
winmgrc units alongside the previous whole body cohort. The navigation fixture
links those actual bodies and original GetWindowBorders/NeedsWindowEdge. Its
explicit transport and property doubles cover lookup scope/version, missing
ID, default-button focus transfer/restoration, disabled default, same-focus
repair, old/new caption geometry, tool caption and client/sizing borders.
Creation/modal boundary fixtures add geometry identity/restoration checks,
including modal nesting and exception unwind; context capture adds geometry
failure/output-unchanged and selected-input assertions. All are staged and
unrun; mocks do not prove guest callbacks or real native retirement.

Static diff checks pass. Full original body selection, link closure,
CPD/provenance, real graph lifecycle and product regression/publication remain
open. No build, test pass, product publication, commit or push is claimed.

### Original body cohort r4: duplicate import correction

The owned background verifier is tests/observation/verify-wow-control-boundaries.ps1.
It selects the full 24-unit admitted control/dialog compilation cohort and
twelve supplemental fixture executables in one run, reusing
build/M0-T447/S3/call-boundaries-x86. Failed compilation suppresses all fixture
execution, so older binaries cannot be counted as new acceptance.

original-bodies-r3 stopped before compilation because Windows PowerShell could
not load Get-FileHash. Retained runner stderr records that failure. Restarting
under the available PowerShell 7 produced original-bodies-r4: runner PID24208,
Ninja PID13464, build start2026-10-16T22:30:47.8505272Z, elapsed210.225 seconds,
exit2. Evidence and input hashes remain under build/M0-T447/S3/original-bodies-r4;
the adjacent runner stdout/stderr retain process/result records. No fixture
executed and nothing was published.

Source inspection identifies a project import error, not an original algorithm
defect: OPENNT-HOST-105's RtlInitLargeUnicodeString body was inserted twice,
along with its prototype and the chartranapi.h large-string include selector.
Remove only the duplicate copies. The remaining body matches the pinned
original after newline normalization, with one definition in each tree.
The eight new/extended RTL/dialog helper source files were inspected with a
definition-name duplicate screen; no further duplicate names were found.
That screen is supplemental lexical evidence, not a C parser or link proof.

The sole implicit-declaration warning in r4 is _FChildVisible, called by the
unchanged FChildVisible wrapper. Original ntuser/inc/user.h:2474 declares
BOOL _FChildVisible(PWND pwnd); the target-private control precomp now carries
that exact declaration. Its original rtl/winmgr.c body already exists in the
selected navigation support. No replacement implementation or guest change
is added. These changes reduce duplicate mirror content and do not add a new
behavioral divergence beyond the existing registered source slices.

Grouped verification r5 starts only after r4's owned session reports terminal
exit2. It retains the same build cache and uses a new evidence directory;
the result is pending, not a product or guest acceptance claim.

### Original body cohort r5: grouped supplemental verification passed

Owned runner PID18244 started2026-10-16T22:35:56.8950894Z under PowerShell7,
invoking verify-wow-control-boundaries.ps1 with EvidenceRoot
build/M0-T447/S3/original-bodies-r5 and the existing call-boundaries-x86 cache.
Ninja PID22124 started22:35:59.1173166Z and exited0 after141.163431 seconds.
All24 original compilation targets were requested together. This is incremental
dependency-selected compilation, not a claim that all objects were rebuilt.
No C4013 implicit-declaration diagnostic, compiler error or failed target is
present in this run. Other historical conversion/linkage/uninitialized warnings
remain visible and are not silently classified as proof of safety.

All twelve executables then exited0, completing22:38:26Z. Captured stdout:

| Fixture | Result marker / coverage |
| --- | --- |
| wow_dialog_call_fixture | dialog-call failures=0; explicit call-boundary doubles |
| wow_edit_call_fixture | edit_call_failures=0; scoped EDIT services |
| wow_edit_memory_callbacks_fixture | WOW_EDIT_ORIGINAL_MEMORY_MACROS failures=0 calls=16 |
| wow_control_brush_fixture | control_brush_failures=0 calls=8 |
| wow_default_window_binding_fixture | default_window_binding_failures=0 |
| wow_dialog_context_fixture | dialog_context_failures=0; capture and rejection inputs |
| wow_dialog_creation_entry_fixture | dialog-creation-entry failures=0; context restoration |
| wow_dialog_finalization_fixture | dialog-finalization failures=0 |
| wow_dialog_modal_entry_fixture | dialog-modal-entry failures=0; nested/exception scope |
| wow_control_string_fixture | WOW_CONTROL_STRING_OK; actual original RTL bodies |
| wow_dialog_navigation_fixture | WOW_DIALOG_NAVIGATION_OK; actual navigation/help/default/geometry bodies over synthetic graph |
| wow_edit_view_fixture | original_edit_view_failures=0 |

The evidence directory contains each test stdout/stderr, build stdout/stderr,
source-sha256.csv, build-sha256.csv and exit.txt=0. Adjacent runner logs retain
every PID, start, elapsed and exit. Source hashes include untracked imported
files, but this is not a sealed full-product dependency/package manifest.

Reviewer disposition: the duplicate-import correction reduces mirror lines;
the missing prototype is original ABI in the target-private facade. No original
algorithm rewrite, new guest diff or extra behavioral waiver was introduced.
The pass establishes this supplemental cohort only. Real page-domain help-ID
retention tests, full original production selection/link, creation provenance,
opaque procedure normalization, native graph reentry/destruction and real
Win16 application/control acceptance remain in the existing finite S3 scope.
No S3 product publication, commit/push or closure follows from these fixtures.

Post-update documentation governance ran as owned hidden PID3648, starting
2026-10-16T22:39:31.4836958Z, and exited0 with
`Documentation governance verification passed.` Its logs are retained in the
r5 evidence directory. Static git diff whitespace validation also passed.

### Modal keyboard chain: actual import bypass and bounded original closure

Read-only COFF inspection of all24 original cohort objects used MSVC
14.43.34808 Hostx64/x86 dumpbin /symbols, hidden PID24168, exit0; output is
original-bodies-r5/cohort-symbols.log. Counting external common symbols with
nonzero UNDEF size as definitions yields269 defined symbols,379 referenced
symbols and241 references outside the cohort. These counts include CRT,
native services and selected support; they are NOT241 missing implementations.
The important concrete finding is __imp__IsDialogMessageW@8: original
dlgmgr.c DialogBox2:139 still resolves its keyboard filtering to modern USER.
The retained dlgmgr2.c subset explicitly omitted IsDialogMessageA/W. Therefore
compiling the24 bodies cannot establish that the complete original modal
dialog algorithm has been restored. No product with this new group has been
published. The omission is assigned to existing S3 dialog/body closure, not S4
or a new feature; this corrects the prior incomplete dependency inventory.

Pinned source read in full for the required chain:

| Original owner | Required retained behavior | Remaining boundary |
| --- | --- | --- |
| dlgmgr2.c IsDialogMessageA/W:213/257 | Message-filter call, ownership check, WM_GETDLGCODE selection, Tab/arrows, Enter/Escape, default-button updates, translation/dispatch | Select original entry through existing borrow/task/native-call owners, not the SDK dllimport; A entry restores original character after conversion |
| dlgmgrc.c group helpers:22/50/427 | WS_GROUP wrap and disabled/invisible filtering over original descendant navigation | Directly composable through existing retained graph; recovered unchanged below |
| dmmnem.c FindMnemChar/xxxGNM_FindNextMnem/xxxGotoNextMnem | Escaped ampersand matching, group/global search, static-label forwarding, focus versus click, duplicate-mnemonic behavior | Native text/message/focus returns must refresh/preserve existing scope; no replacement mnemonic engine |
| dlgmgr2.c:543/547 and dmmnem.c:315/323 | BST_DONTCLICK around focus prevents duplicate BN_CLICKED | Requires actual button behavior owner; never reinterpret native button storage as BUTN or fabricate a mirrored bit that native USER does not consume |
| btnctl.c WM_SETFOCUS:1379 | Original consumer of BST_DONTCLICK for radio buttons | Source proves why merely omitting that bit is not equivalent. Current control-extent metadata alone does not supply a live original PBUTN |

Source hashes: dlgmgr2.c
A3E9286C40E4E11EC872FD9B2D7B23CBA640F09FB4840C4E6130E301D5277B9E;
dlgmgrc.c
678D2CD4B79A15A79E6C0257B4BD93AD69FA7BE95A513B627565490256CB1AFC;
dmmnem.c
638EFC31FFD622BA31E16672D1418291C3E1291B6D0C57F31D08C32D45FA1720.
No new source universe or guest input is involved. The private button
cooperation must be resolved before selecting the complete original message
chain; this is not covered by the default-color waiver. No private flag is
sent to modern USER and no broad host behavior substitution is installed.

OPENNT-HOST-109 restores the three directly composable group-navigation bodies
in their existing original dlgmgrc.c owner plus the original declaration in
usercli.h. All three match pinned source after newline normalization. Their
outgoing dependencies are the already retained _NextControl/_PrevControl/
_GetChildControl and original TestWF flags over the same retained WND graph.
No helper algorithm belongs in an adapter. Added actual-body synthetic-graph
assertions cover forward/reverse group wrap, one-member group, disabled and
hidden members and a group without explicit boundary flags. They are staged,
unrun after r5: do not cite r5 as evidence for these changes. No isolated
rebuild is started; the rest of this named dependency chain is audited before
the next cohesive build/acceptance. Whole native button/control substitution
or recursive USER server import is not admitted by this recovery.

Acceptance for the complete chain remains actual guest dialog Tab/Shift-Tab,
group arrows, Enter/Escape, duplicate and escaped mnemonics, exactly-once
button notification, filter interception, reentrant destruction and retirement,
plus A/W character restoration and existing modal return/quit behavior.
Synthetic navigation assertions cannot replace those observations.

### Native button suppression service: documented candidate not yet usable

The next source audit follows original btnctl.c, not an invented notification
filter. ButtonWndProcWorker owns BUTN allocation through ButtonLookaside and
INITCONTROLLOOKASIDE, consumes BST_DONTCLICK in WM_SETFOCUS, and frees the same
allocation on NC/final destruction. Its complete source also reaches the
private OEM bitmap atlas, DrawPushButton, PSMGetTextExtent and icon services.
Importing the entire button renderer solely to write this one state bit is
not yet justified. The original dialog/mnemonic algorithm and native button
state owner must cooperate without two copies of the suppression state.

Modern SDK winuser.h:11387 defines BM_SETDONTCLICK=0x00F8. Microsoft's
[message contract](https://learn.microsoft.com/en-us/windows/win32/controls/bm-setdontclick)
states that TRUE/FALSE controls focus-generated BN_CLICKED and provides no
return value. It is a relevant narrow native service candidate, not proof of
equivalence or permission to ignore subclass/lifecycle effects.

Added research-only wow_button_focus_service_probe.c and explicit
button-focus-research Ninja target, outside default and product targets. It
creates hidden, test-owned radio/auto-radio windows in A/W forms, observes
parent notifications, and tests ordinary focus, swallowed subclass messages,
direct public class-procedure invocation, set/clear repetition, another button
and destruction. No input injection, desktop switch, private layout access,
guest change or product adapter was introduced. The class procedure is obtained
through GetClassInfoExW and remains opaque to CallWindowProcW.

Owned background runs under the established optimized x86 graph:

| Run | Runner/start UTC | Observation |
| --- | --- | --- |
| button-focus-research-r1 | PID42852,2026-10-16T22:49:14.4928334Z | Build0; direct WM_SETFOCUS/WM_KILLFOCUS probe exits1 with20 failed suppression assertions across4 cases. Cleanup/negative controls pass. This did not prove actual focus behavior. |
| button-focus-research-r2 | PID12396,22:50:47.0305679Z | Build0; --actual-focus uses SetFocus and verifies GetFocus after each change. Those focus checks pass, but the same20 assertions fail; counts7 instead of6 per case. |
| button-focus-research-r3 | PID6264,22:53:07.7795210Z | Build0; adds an unsubclassed button receiving ordinary SendMessageW. Suppressed focus still emits1 click;22 total assertions fail. This rules out the direct-class-call bypass as the sole cause. |

Each run's stdout/stderr is retained directly under build/M0-T447/S3 with the
run prefix. All owned handles reported terminal exit1; no retry remains live.
The300-second diagnostic deadline applies to the whole owned build/probe tree,
not a30-second observation slice. The latest source SHA256 is
D133CADD9724E29D099D66F8E7D75F950A64C7E5DE5F843E06A5F56A4EFD620D;
EXE SHA2569D53E6CFD2FE182D19A565DACF718A7AE1AF8BD9B8182A36987F0582D09A4F9E.
These failures stay failures; absence of a native return value cannot mask them.

Read-only matching-PDB/objdump audit identifies installed user32.dll
SHA25656F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E:
ButtonWndProcW at10078F80 first tests its message-membership table, then calls
ButtonWndProcWorker10079000. The worker's F4-based switch maps F8 through
index4/slot3 to10079948, which sets/clears0x80; its WM_SETFOCUS branch at
10079327 tests0x83 before notifying. This confirms a source-shaped internal
consumer, but does NOT yet explain why the public probe did not reach the
expected effect. No inspected addresses, offsets or private bits enter product
code. Public symbols output is button-focus-user-symbols.log (PID36704,exit0).

Disposition: do not install the candidate or claim that native buttons lack
the feature universally. Entry filtering/compatibility and the exact public
service path remain to be established from code. Existing S3 native-button
cooperation gap stays open; full modal keyboard integration remains blocked
by that gap, while other finite S3 groups remain available. No new mirror diff,
production test pass, publication, commit or push is claimed by this research.

### Button service follow-up: distinguish public entry from private worker

The retained r4/r5/r6 research logs add state/notification diagnostics and
procedure-module identity, not a successful service binding. All three exits
remain1 (22 assertions failed). In r6 the public class procedure is77B15680,
owned by NTDLL at77A60000 (RVA B5680), whereas loaded USER32 is769B0000.
Thus matching the USER32 worker's machine code alone does not establish that
the public call reached its BM_SETDONTCLICK branch. The pre-comment-edit probe
source hash is1B3E2DFB730EB590CE95504A6F783FA37A9A4D4C133C1356CE03182CB3F0AC03.

Read-only disassembly of the installed x86 NTDLL shows that RVA B5680 is an
indirect jump through RVA13C170. Its on-disk target is initialization/failure
code, not proof of the live slot's destination. No live destination was read;
do not claim either a USER32 dispatch or a system defect from these bytes.
The expected Windows Kits x86 cdb.exe is absent. No private address, table or
state bit is admitted as a runtime binding by this investigation.

Re-reading both original consumers also bounds the required behavior:

| Original caller | Exact suppression scope | Behavior after focus |
| --- | --- | --- |
| dlgmgr2.c IsDialogMessageW arrow branch | Only DLGC_UNDEFPUSHBUTTON/DEFPUSHBUTTON with original FNID_BUTTON; the radio branch is separate and does not set this bit | Update default push button; auto-radio branch separately uses BM_CLICK when unchecked |
| dmmnem.c xxxGotoNextMnem | Non-push-button mnemonic branch with original FNID_BUTTON | Clear bit after DlgSetFocus, then BM_CLICK only for a unique mnemonic |

Consequently the radio-only research probe exposes a service effect needed by
the mnemonic path; it is not an acceptance test for both original branches.
Restored integration must also cover push/default buttons, auto-radio,
duplicate mnemonics, application subclassing, callback destruction and focus
reentry. The original operations are set/clear, not saved-state restoration
or reference counting. No parent-notification filter or shadow suppression
state is selected to hide the failed public-service assertions. Existing
group-navigation assertions remain staged and unrun; this audit does not
promote them or the original modal keyboard chain to verified functionality.

### Classic button message-table omission: bounded static explanation

Read-only follow-up used the already identity-validated win32kfull.pdb and
installed win32kfull.sys, whose SHA256 remains
94F00AABE76A4AF3794E2134B806BF89E556B66814859C285DE62173350A5173.
InitMessageTables is section10 offset3040 (preferred VA140409BE0).
Its button initializer at140409F22 starts with message84 and walks the WORD
array at RVA375F40, storing the maximum and allocated membership bitmap at
server-info offsets52E0/52E8. The array ends after128 at RVA375F82 and contains
F0--F7 but no F8. The separate population loop must be distinguished from
the maximum scan; no live table is modified or presumed inspected here.

This supplies a concrete static explanation consistent with the failed
classic-button probe: the USER32 public wrapper checks membership before
entering the worker, so the existence of an F8 case inside the worker alone
does not make F8 publicly reachable. The Microsoft BM_SETDONTCLICK contract
does not document a common-controls-version prerequisite. Do not invent one
or silently change the application's button class/activation context.
This is evidence about these identified installed images, not a universal
Windows support claim or proof of the unobserved NTDLL live jump target.

Recovery now follows the original btnctl.c service closure instead of another
unchanged-probe retry. Its local state/notification/capture behavior and
lookaside lifetime are inseparable from the two dialog writers. Initial
provider search confirms BitBltSysBmp, DrawPushButton, PSMGetTextExtent and
NtUserGetIconSize are not yet providers in the selected USER/control slice.
Existing EDIT/ComboBox/ListBox lookaside adaptation is relevant reuse, not
proof that the button renderer and OEM resources are already available.
No button import, private worker call, notification filter or product change
is selected by this preliminary dependency inventory.

The subsequent body-level review separates small recoverable helpers from
the actual resource boundary (no compilation was used to discover these):

| Required provider | Original dependency and current disposition |
| --- | --- |
| Button lookaside | Existing original rtlinit.c allocator plus wow_edit_lookaside_ready is reusable; a button-owned pool and exact BUTNWND lifetime still need wiring. No second allocator is justified. |
| DrawPushButton | rtl/draw.c original body uses DrawEdge, FillRect, system colors, gray brush and display BitCount. Existing DrawFrame subset/gray-brush service are reusable, but do not already supply this body or its scoped BitCount input. |
| PSMGetTextExtent | rtl/text.c original body removes mnemonic prefixes through GetPrefixCount then calls UserGetTextExtentPointW. Neither helper is proved selected by merely having DrawTextExW. Recover the original prefix algorithm rather than independently stripping ampersands. |
| xxxButtonDrawCheck | btnctl.c selects OBI_CHECK/RADIO/3STATE and state offsets. Non-flat radio drawing performs mask SRCAND then SRCINVERT with an OEM bitmap atlas; dimensions are read before drawing. This cannot be replaced by a successful no-op BitBlt service. |
| BitBltSysBmp | kernel/drawfrm.c is only a blit from gpDispInfo->hdcBits using oemInfo coordinates. Do not import the whole kernel frame renderer or reinterpret modern OEM state. A resource-provider decision must cover atlas lifetime, dimensions, colors and mask semantics together. |
| Icon dimensions | btnctl.c calls NtUserGetIconSize before image/text layout. A public icon-resource adapter must include ownership and cleanup of any GetIconInfo bitmap copies, not merely return guessed system metrics. |

In particular, native drawing or NTW16S decoration does not by itself provide
the original BUTTONSTATE owner needed by dialog navigation. Conversely,
introducing that owner while leaving native button behavior active would
create competing states. The integration decision must select one behavior
owner across creation, messages, focus/capture, notification and destruction.

### OPENNT-HOST-110: original mnemonic-prefix helper recovery

Admit unchanged GetPrefixCount from pinned OpenNT rtl/drawtext.c, SHA256
D94876C3A091278D92030AFF49F37151C8F728F86F0B7D3F08C3F1F7199EA746.
It is the finite string algorithm used by original PSMGetTextExtent and menu
drawing. Append it to the existing same-path subset; no separate autonomous
ampersand parser is introduced. Its entire outgoing closure is CH_PREFIX,
MAKELONG and caller-owned readable/writable buffers. The target-private
precomp supplies the original CH_PREFIX spelling and function declaration.
FE_SB remains the existing unselected build variant; retain its source guard.

The caller must reserve a trailing WCHAR in addition to charcopycount.
The original rtl/text.c PSMGetTextExtent passes all255 elements of szTemp as
charcopycount, while GetPrefixCount always appends a terminator. Thus a long
input can exhaust that capacity even without prefixes. Do not import/select
that caller unchanged on the assumption that arbitrary captions are short.
Its exact admissible input range and original-host correction remain to be
resolved before the full measurement group is selected. No new guest change
or semantic waiver is made. This helper is staged, not built/tested; grouping
and production-selection gates remain. Mirror budget: one unchanged100-line
body plus registration comment; zero behavioral edits to the body.

Resource follow-up: original inctlpan.c sysBmpStyles defines sixteen button
mask/check/radio/three-state entries; GetFrameControlMetrics uses13x13 for
materialized button images and aliases unchecked three-state images.
The atlas is generated by DrawFrameControl, not an immutable guest bitmap.
Original DrawBox/DrawFrameControl use Marlett glyphs and system colors;
resource reconstruction must preserve mask and alias semantics and avoid
importing the unrelated control-panel/profile machinery.

Follow-up confirms kernel/text.c has the same255-WCHAR scratch array and
full-capacity copy call; selecting it instead of rtl/text.c does not remove
the capacity defect. btnctl.c BNCalcRect(CBR_GROUPTEXT) passes strName.Length
without a255-character clamp, so the comment is not an enforced caller
precondition. Its one-character spacing calls are bounded; those do not prove
the caption calls safe. Both prefix-free long strings (terminator overflow)
and prefixed long strings (subsequent extent length exceeding the populated
scratch text) need coverage in any eventual correction.

The existing string fixture now calls the real GetPrefixCount body for
ordinary text, a single mnemonic, escaped ampersands, no-copy counting,
bounded copying including terminator/canary, zero capacity and trailing
ampersand. The whole-group generator links that original translation unit
using the existing control-drawing precomp; no fixture parser double is used.
These assertions are staged, not run. No isolated build was started.

### OPENNT-HOST-111: original push-button drawing leaf

Admit unchanged DrawPushButton from original rtl/draw.c, SHA256
7491E5B3331DE8CB7A2189B8EFBE91F56076F80A08338BF816D793E80A4FAF76.
Direct caller is btnctl.c xxxDrawButton. Its closed outgoing services are
public DrawEdge/FillRect/DC color APIs, borrowed system brushes and the
existing owned gray pattern brush. Original state/flag decisions, dither
selection, DC color restoration and BF_ADJUST geometry remain unchanged.
Guard the additional subset with WOW_ORIGINAL_BUTTON_DRAWING until the
complete button owner is connected; existing DrawFrame fixtures remain valid.

The existing default context is not a usable bit-depth source for this call:
control entry captures it with WM_NCHITTEST, while BitCount is populated only
for WM_CTLCOLORSCROLLBAR. Reusing its zero would falsely select low-color
dithering. Instead capture display planes/bits alongside the existing DISPLAY
IC geometry in the scoped control context, with positive/overflow checks
before publishing the context. No extra DC, process cache or private OEM
overlay. The drawing precomp maps only this admitted body to that input.
Existing brush readiness and nested control-context restoration remain the
entry's responsibility. Stage the native-context assertion; do not claim
pixel or guest acceptance. Mirror budget: unchanged body plus selection guard.

### OPENNT-HOST-112: bounded original text-measurement recovery

Original rtl/text.c SHA256
D2DAE26E4766C2567225CB6BFC8147B4E80955B4CA49DCAF97B6193E7067A8E2
supplies PSMGetTextExtent. Recover its prefix parsing and two extent branches,
with a registered original-host capacity correction under standing authority.
Reuse the UserRtlAllocMem/UserRtlFreeMem pattern already present in original
rtl/drawtext.c PSMTextOut and the existing LocalAlloc-backed project binding.
Retain255 WCHAR stack storage for counts below255; at255 or above allocate
cch+1 WCHARs, checking count/allocation-size validity first. Unlike original
PSMTextOut's strict-greater comparison, equality must allocate because the
parser appends a terminator. Do not add a second prefix parser or heap owner.

On allocation failure return FALSE with zero extent and an allocation error;
invalid negative/overflowing counts return FALSE/ERROR_INVALID_PARAMETER.
Finally cleanup frees only the temporary allocation and retains the leaf's
last error. Original packed prefix-count representation, GDI measurement
selection and TRUE return after the GDI call remain; this is not a new claim
of arbitrary-length/full32-bit prefix-count support. No guest source changes.
Validation must include254/255/256/long captions, prefixes and escaped '&',
native extent comparison and failed allocation/lifecycle cleanup. Staged;
none of these new measurement assertions has run yet.

The whole-group string fixture now compares the recovered measurement with
native GetTextExtentPointW on independently specified printable strings at
lengths0/1/254/255/256/1024, with plain text, mnemonic prefix and escaped
ampersand variants. Negative count checks zero extent and explicit error.
Its graph includes the real text.c body and existing allocator provider.
Allocation-failure and exception cleanup instrumentation remain uncovered;
these staged assertions are not a substitute for button/guest acceptance.

Whole-cohort original-bodies-r6 completed with build exit2 (86.7150835s,
Ninja PID26276, start2026-10-16T23:27:01.2716610Z; owned runner5544).
The complete error list is four uses of missing WFGROUP in original dlgmgrc.c;
no tests were executed against stale binaries. OPENNT-HOST-109 now restores
the original inc/user.h WFGROUP=0x0E02 declaration. This is an NT4 TestWF
encoding over our original state, not a native style constant or private
host offset. No algorithm change. Retain the failed run and rerun the group.

### original-bodies-r7 grouped verification

Owned hidden runner26892 started2026-10-16T23:28:43.2208290Z; Ninja20240
started23:28:44.9125441Z and completed exit0 in149.0896211s. All14 fixtures
completed exit0, final evidence exit.txt=0. Inputs, generated graph hashes,
build logs and per-fixture output are under build/M0-T447/S3/original-bodies-r7;
runner stdout/stderr are the adjacent original-bodies-r7.runner logs.
No compiler errors, link errors or C4013 implicit-function warnings remain
in this run; existing original-source conversion/linkage/uninitialized-value
warnings are not claimed eliminated. No process from this owned job remains
awaited; the execution handle reported terminal exit0.

The passing set is dialog_call, edit_call, edit_memory_callbacks,
control_brush, default_window_binding, dialog_context, dialog_creation_entry,
dialog_finalization, dialog_modal_entry, control_string,
text_measure_failure, button_drawing, dialog_navigation and edit_view.

New assertions prove the actual original group-navigation helper branches,
prefix output/count behavior, native GDI extent comparisons at the selected
lengths, allocation failure returning zero extent, guarded allocation cleanup
on raised leaf exception, error preservation across free, and drawing
edge/brush/dither/rectangle/DC-color decisions against explicit doubles.
The original DrawPushButton body is compiled under its explicit selector in
the fixture; this does not select a native button behavior owner in product.
Native control-context display-depth assertions are outside this runner and
remain unrun. Complete button/atlas/modal integration, real graph lifecycle,
guest execution and coherent all13 publication/commit/push remain pending.
The failed native BM_SETDONTCLICK research is not rerun or reclassified.
Post-record documentation governance (owned hidden PID30288) passed exit0;
stdout/stderr are retained in the same r7 evidence directory. Diff whitespace
check passed; CURRENT remains below its32768-byte limit.

### Button owner/resource integration: concrete remaining edges after r7

Source inspection, not another runtime probe, fixes the integration boundary:

| Edge | Current implementation | Required same-owner integration |
| --- | --- | --- |
| Registered extent | wow_control_registered_extent already includes original BUTNWND | Reuse it; do not add another window registry or native extra-byte overlay. |
| Canonical procedure | wow_control_client_procedure selects EDIT/Combo/List only | Add A/W button entries through the same task/borrow/context/finally owner only when original btnctl and resources are linked. |
| Creation enrollment | recovered_control_procedure's finite list omits Button | Include the canonical button entry before its first original callback, not by class-name heuristics after native creation. |
| Task cleanup patching | shared.apfnClientW.pfnButtonWndProc still points to wow_class_call_original_native | Select the original canonical owner; existing wow_window_words_patch_cleanup already queries the shared selector and must not restore native button state after original initialization. |
| Final destruction | wow_control_finalize_window accepts EDIT/Combo/List only | Dispatch original ButtonWndProcW WM_FINALDESTROY with the same CLEANEDUP/DELETED protection and retained allocation, without an application subclass callback. |

Original btnctl.c allocates BUTN through INITCONTROLLOOKASIDE, links spwnd,
and releases it on NC/final destruction before setting FNID_CLEANEDUP_BIT.
Allocation failure marks cleanup and destroys through the existing original
macro. Recovery must cover failure before initialization, nested destruction,
NC followed by final cleanup, module/task exit and handle reuse. Do not add
an independently managed suppression flag to compensate for native buttons.

Resource provenance was re-read in full for the relevant table and factory:
inctlpan.c SHA2560E3999DF8610405C990760EE1A417BF312E3B76744F8F9387BBD94583F9D800D;
inc/user.h SHA256ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1.
The sixteen button entries occupy OBI52--67. GetFrameControlMetrics and
CreateBitmapStrip generate13x13 materialized images and resolve zero-width
aliases before rendering. The entire kernel factory also replaces global
caption caches and system metrics: it must NOT be imported merely to supply
these button resources. Native GDI resource creation and a scoped derived
button-only resource view remain the bounded adapter boundary; never read
modern USER's private atlas or retain guest/NT4 pointers to one.

There is also a source-proven mapping inconsistency to resolve before copying
the alias rule: DOBI_3STATE is8, but OBI_3STATE63 minus OBI_CHECK53 is10.
Consequently the source rule maps unchecked63 to55 (CHECK_D), and pushed65
to57 (CHECK_CDI), rather than53/55. This arithmetic was independently computed
from the pinned header definitions; it is not a pixel-test observation.
No alias correction or resource factory is implemented yet. A retained
correction must be locally registered and derive the offset from the named
base indices, with state-by-state resource assertions. This is an original
host-resource issue, not permission to modify guest or substitute NTW16S
Win3.x resource IDs for NT4 OEM indices. The r7 pass does not cover it.

### OPENNT-HOST-113: button-only native resource carrier

Admit the complete original sysBmpStyles declaration from the pinned inctlpan.c
identified above, retaining every row and original OBI numbering. Only remove
static to expose this read-only data to the resource adapter. No kernel factory,
caption cache, profile operation or system-metric mutation is imported. Its
five private drawing constants already exist in the canonical winuserp.h;
recover those exact constants into the bounded control ABI. Retain original
OEMBITMAPINFO and button OBI constants in inc/user.h under an opt-in guard.

The project resource carrier may materialize only OBI_RADIOMASK through
OBI_COUNT-1. Native GDI owns its scoped DC/bitmap; no global cache or HWND
registry. Private atlas placement is linear13-pixel rows, invisible to the
algorithm because all lookups use the matching metadata. Creation publishes
handles only after every required image is drawn; failure deletes partial
objects. Release restores/destroys the memory DC before deleting its bitmap,
and retains last error. Initialization refuses to overwrite live handles.

Register the original-host alias correction here: use
OBI_3STATE-OBI_CHECK rather than stale DOBI_3STATE=8 for the unchecked three-state
resource alias. All original style rows and button logic remain unchanged.
Native DrawFrameControl is only the resource-rendering service, not the button
WndProc or a replacement for original click/focus/navigation behavior.
No existing autonomous resource provider is selected for this group, so none
is retained alongside it. Stage only until resource state/failure/lifetime
assertions and the complete button integration group pass. No guest changes.

Staged implementation now retains the table byte-for-byte after newline
normalization except removal of static, checked against the pinned source.
wow_button_resources owns only a caller-scoped memory DC/bitmap and original
image metadata. Its factory uses a local pending object, with finally cleanup
for native exceptions as well as ordinary failures; success transfers handles
only after rendering completes. Metadata uses the corrected named-index
alias. Three original raster operations are admitted and source rectangles
are bounds checked. No native HWND procedure or global style is changed.
No build selection or runtime pass is claimed yet; initialization, aliases,
mask composition, nested scope independence, failed creation/rendering and
normal/exception resource release remain the next group assertions.

### Button callback/drawing audit after resource staging

Read the complete pinned client/btnctl.c (SHA256
579205CFBD7DA6CF97E05235C74B3A221415DD07AE6F73C30649C9BF9E06EF2E),
not just its BM_SETDONTCLICK branch. Its remaining closure is bounded by the
following owners; no original button procedure is selected by this audit.

| Family | Source contract and implementation disposition | Required evidence |
| --- | --- | --- |
| Capture/focus/click | xxxBNSetCapture, xxxBNReleaseCapture and BM_CLICK own BST_INCLICK/INBMCLICK, check cycling and parent notification ordering. Reuse existing scoped native capture/focus and SendMessageWorker edges, never a second suppression flag. | Nested notifications, capture loss, focus-driven radio click suppression, callback destruction. |
| Borrowed state | Parent/group traversal and every original BUTN access after callout require retained original allocations, not only a retained HWND number. WM_NCDESTROY and WM_FINALDESTROY free BUTN before marking CLEANEDUP. | Retention through nested destruction, no freed BUTN access, finalization once, reused HWND not rebound to old state. Current WND retention alone is not proof of BUTN retention. |
| Brush alignment | client/draw.c PaintRect validates brush and paint windows, translates brush origin using their client origins, handles CTLCOLOR pseudo-brushes, fills, then restores origin. Recover this original body through existing validation/color boundaries; plain FillRect is insufficient. | Parent-origin pattern alignment, same-window case, invalid identity, reentrant color callback and restoration. |
| Visibility | Existing rtl/winmgr.c IsVisible already owns ancestor visibility traversal. Reuse it and the same retained relationship graph. | Hidden ancestor and destroyed/reused relationship cases; no native-private flag sampling. |
| Bitmap/icon/text | BNDrawText borrows application font/image handles, computes alignment, measures prefixes and invokes DrawState with its original multiline callback. No application-owned image deletion. | Font/image replacement, disabled/default drawing, multiline callback and DC restoration. |
| Atlas | client/mngrayc.c BitBltSysBmp is a direct metadata-to-blit wrapper; the admitted scoped atlas supplies only its resource service. | All sixteen metadata rows, fourteen materialized images, aliases, radio mask composition, resource rollback. |
| Style/name | BM_SETSTYLE and NCCREATE use shared style mutation; WM_SETTEXT repaints old group caption before _DefSetText and redraw. | Native-return state refresh and retained name lifetime; the existing pre4.0 style-notification choice remains unresolved, not waived here. |

PaintRect source SHA256
872A056F63921A5B3D1C924E35F19EEA1660FE7481E25A174E875C0F12959B98;
client/mngrayc.c SHA256
171EF0B7CD668CF51DB86D9AE2FEA4269CC002F0ABA17033BF968D84C10F0F3D.
The latter contains ClientDrawState's original effects, global scratch DCs,
critical section, callback and PSMTextOut dependency. Do not silently bind the
whole algorithm to native DrawState merely because its API matches. Its
scratch-resource/reentry closure must be resolved before button selection;
this does not admit the entire USER initialization subsystem. Icon-size
source uses private PCURSOR/animated-frame state and doubled mask height;
no host icon handle may be cast to that layout.

Resource fixture now uses actual GDI through instrumented leaves and the
selected resource implementation plus original table. Thirty-six injected
failure/exception cases cover acquisition, selection and all fourteen render
steps, each followed by successful retry and paired release. Additional
assertions cover exact metadata aliases, nested independent scopes, idempotent
release, last-error preservation, invalid blit ranges/operations and native
pixel comparison. These are supplemental resource assertions, not original
ButtonWndProc, NTW16S pixel identity or real-guest acceptance. Background
original-bodies-r8 is the first execution of this expanded group; retain its
actual result rather than assuming the assertions pass.

### Supplemental original-bodies-r8 result

Owned hidden runner31556 started2026-10-16T23:44:05.9750663Z, completed
exit0. All fifteen supplemental executables passed, including the fourteen
r7 fixtures and new wow_button_resources_fixture. Resource fixture PID3028
ran2026-10-16T23:46:11.3197133Z, exit0 in0.3643451s with
button_resource_failures=0. The runner joined all children and terminated;
no background job remains awaiting observation. Exact Ninja PID/timing,
source/graph hashes and individual outputs are retained under
build/M0-T447/S3/original-bodies-r8 and its adjacent runner logs.

This proves the staged carrier's thirty-six injected failure/exception
cleanup/retry paths and real-GDI resource/pixel assertions described above.
It does not prove original ButtonWndProc lifetime, creation selection,
full dialog mnemonic processing or real guest execution. No product binary
was replaced, committed or pushed by this supplemental run. Original-control
cohort compiler warnings remain separately visible in the build log; no
warning-free product claim follows from exit0.

### OPENNT-HOST-114: original button painting dependency group admission

Recover client/draw.c PaintRect and client/mngrayc.c BitBltSysBmp/BltColor
as unchanged function subsets, with original notices and upstream paths.
Source hashes are recorded in the preceding button audit. Original btnctl.c
directly requires PaintRect and the atlas wrapper; original ClientDrawState
requires BltColor. The source bodies preserve brush-origin alignment,
CTLCOLOR translation, raster operations and DC brush/color restoration.
Modern GDI remains the resource owner; existing exact-state validation and
color-call boundaries retain original window identities across callbacks.
No full USER factory or kernel graphics initialization is imported.

The required ladder selects composable original bodies, not autonomous
FillRect/BitBlt implementations. Keep ClientDrawState selection pending until
its scoped scratch-DC/bitmap and callback unwind are connected. Do not bind
native DrawState as an unreviewed replacement for the original effects.
No existing replacement PaintRect/BitBltSysBmp/BltColor was found in the
selected project sources; no duplicate implementation is retained.

Budget: three original function bodies unchanged; file-subset boundaries
and opt-in compile guards only. Acceptance requires patterned-brush origin,
pseudo-brush callback, invalid window, atlas metadata and raster/DC state
tests together with the complete button consumer. These imports alone are
not runtime closure and do not modify guest.

### OPENNT-HOST-115: original mnemonic text output capacity/lifetime repair

Recover rtl/drawtext.c PSMTextOut (same pinned source/hash as110).
The original uses a static255-WCHAR buffer, allocates only for cch>255,
then calls GetPrefixCount with cch capacity even though it appends a
terminator. Exactly255 characters therefore overrun the static buffer;
concurrent callers also share that mutable scratch. The existing prefix
parser remains unchanged. Keep original text output, mnemonic selection,
overhang, baseline and underline drawing calculations.

Registered minimal correction: automatic per-call buffer, allocate at
cch>=255 using existing USER RTL allocation, validate count/pointer and
size arithmetic, and finally release heap scratch including exceptional
GDI return. Preserve last error through cleanup. The no-prefix early return
uses the same finally owner. Do not import the unrelated full DrawText
engine or write a new mnemonic parser. This is an original host correction
under standing authority, not a guest patch or semantic compatibility waiver.
The packed16-bit prefix contract remains unchanged. Boundary255,
allocation failure, nested/concurrent scratch independence, GDI exception,
underline output and DC-color restoration require grouped verification.

The114 bodies are now staged without body changes. The115 output body is
staged with automatic scratch, >=255 allocation and finally cleanup; the
finally also restores the temporary underline background color if its GDI
output raises. It uses no new parser or allocator. The production generator
now includes PaintRect, BltColor, the existing PSMGetTextExtent body and the
opt-in PSMTextOut body. BitBltSysBmp remains guarded until the scoped atlas
consumer is wired. This is source/build-graph work, not a completed link.

New wow_mnemonic_drawing_fixture exercises the actual original prefix/output
bodies with explicit GDI/allocation doubles: count254/255/1024, allocation
failure, reentry preserving outer scratch, prefix/escape output, exact
underline geometry and background restoration on raised text/underline
exceptions. It is selected in the next whole-cohort runner, not run yet.
Do not relabel r8 as covering these later edits. Complete ClientDrawState
scratch resources, ButtonWndProc selection and callback-private-state lifetime
remain the same finite consumer closure; no guest code or delivered binary
changes accompany this staging.

Source review confirms all three114 function definitions equal the pinned
original after newline normalization. The first comparison pattern for
BltColor accidentally matched its earlier forward declaration; requiring a
newline after the opening parenthesis selects the definition and passes.
No source correction was needed for that comparison artifact. Whitespace
check passes. Owned hidden documentation governance PID28432 started
2026-10-16T23:51:59.9370142Z and completed exit0; logs are
build/M0-T447/S3/drawing-staging-governance.stdout.log and matching stderr.
These are source/document checks only; new implementation fixtures remain
unrun until the consumer group is connected.

### OPENNT-HOST-116: original state drawing with scoped native resources

Direct reached caller is lboxctl2.c xxxLBDrawLBItem (DST_COMPLEX, optional
DSS_UNION), in addition to btnctl.c text/bitmap/icon/multiline drawing.
Its current unqualified DrawState resolves to the SDK implementation; remove
that substitution in the shared original-control compile environment now.
Recover ClientDrawState from the pinned client/mngrayc.c (hash above), keeping
type dispatch, effects, font setup, callback, return and raster ordering.
Original clinit.c InitClientDrawing input source SHA256
83C7E304B38501C9AFD1251C2CF671F1BC0E799560AF10B225CEDEE891E3E8A2.

Reject importing global USER initialization. A scoped native resource binding
provides the original8x8 monochrome scratch, font/brush/colors, bitmap DC,
dimensions and critical section; nested calls receive distinct resources and
restore the previous TLS context. No second control state engine, HWND owner
or process-wide drawing cache is introduced. The original body still owns
scratch growth. Final cleanup finds its current selected owned bitmap after
growth, restores the DC's initial object and deletes only owned resources.
Application bitmap/font/icon handles remain borrowed. On exceptional exit
restore the destination viewport modified by the original body, not an
unconditional SaveDC/RestoreDC that would erase normal callback side effects.

Register two minimal original-host corrections: add finally around the
original critical section, and fix DSS_UNION's duplicated PolyData.cx=cy
assignment to PolyData.cy=cy. The original otherwise passes uninitialized
height to PolyPatBlt; this is visible from source and directly reached by
disabled ListBox drawing. No other effects branch is replaced or suppressed.
Native icon sizing uses owned GetIconInfo copies and bitmap dimensions,
returning the original doubled-mask height contract; never cast HICON to
PCURSOR. Only the original index-zero consumer is admitted.

The private drawing entry serves recovered control callers, not a new public
DrawState export. Recovered callers use TEXT/PREFIXTEXT/COMPLEX/BITMAP/ICON;
private system-menu glyph indices are outside this button-only resource scope.
Required grouped proof: ListBox UNION extent, default/disabled raster output,
nested callback resource independence, callback exception/viewport recovery,
bitmap/font non-ownership, allocation rollback, handle balance and original
button/list consumer link. No guest modification or runtime pass is claimed.

Implementation review tightened the ownership design before testing: cleanup
must NOT infer ownership from GetCurrentObject, because a callback can leave
an application bitmap selected. The scoped carrier records only the original
algorithm's successful scratch-bitmap selections; callback selections do not
transfer ownership. Restore the DC's initial object, destroy the DC and delete
only the recorded owned scratch. Add a bounded original failed-selection
guard to delete the newly allocated, unselected scratch and return FALSE.
This third116 diff replaces undefined failure behavior, not a normal effect.

The shared control precomp now redirects DrawState to the scoped entry; the
production graph selects original ClientDrawState plus its resource carrier,
original atlas table and existing prefix/extent bodies. The ListBox call no
longer intentionally resolves to native DrawState. This is staged source
selection, not a claim that the complete product graph has linked or run.

The staged wow_control_draw_state_fixture links actual original drawing,
prefix and extent bodies with real GDI resources. Only metric context and
the already audited PolyPatBlt leaf are explicit fixture inputs; the latter
asserts the original UNION rectangle width/height before forwarding to GDI.
Cases retain normal-versus-MONO callback return semantics, nested distinct
resource contexts/TLS restoration, exceptional viewport restoration, borrowed
bitmap survival after a callback leaves it selected, scratch growth and
twenty-call GDI handle balance. These assertions are added to the grouped
runner but have not run. No opportunistic per-edit compile/test is launched.

Source-based lifecycle tracing also confirms the existing terminal owner in
wow_window_words_terminal selects EDIT/Combo/List only, in addition to the
canonical-dispatch/creation/finalization list already recorded above. Button
enrollment must update that owner as part of one group. Retaining WND storage
does not by itself retain its separately freed BUTN across parent callbacks;
the original NC/final-destroy contract remains a required open proof, not an
assumed consequence of the resource tests.

### OPENNT-HOST-117: original Button owner and bounded callback lifetime

Admit complete original client/btnctl.c, source SHA256
579205CFBD7DA6CF97E05235C74B3A221415DD07AE6F73C30649C9BF9E06EF2E,
without body edits. Retain original user.h button flags and usercli.h private
button-state constants as opt-in declaration subsets. Earlier116 supplies its
original state drawing,114 its brush/raster helpers,110--115 mnemonic output;
existing canonical control storage/dispatch provides the rest of the group.
Do not implement another click/check/focus/navigation engine.

Source tracing distinguishes two lifetimes: createw.c xxxFreeWindow sends
WM_NCDESTROY and canonical WM_FINALDESTROY before its final WND thread unlock.
ButtonWndProcWorker frees BUTN during NC/final destruction, whereas outer
BM_CLICK and capture/focus paths can still reference BUTN after a parent
callout. rtlinit.c FreeLookasideEntry immediately returns its storage to the
zone or heap. WND retention alone cannot prevent reuse of that private block.

Use a bounded adapter at the original Button FreeLookasideEntry call only:
same-thread invocation frames retain the exact original WND allocation under
the existing borrow scope. A release is recorded on the oldest active frame
whose original BUTNWND slot is that allocation; physical release calls the
unchanged original allocator when that frame unwinds. NC notification,
Unlock, CLEANEDUP marking and normal return order remain in original code.
One pending release per exact control lifetime, no heap queue, HWND-keyed
registry, copied BUTN, replacement pool or altered guest. A newly reused HWND
with a different retained WND cannot match the old frame. Nested finalization
without an outer button invocation releases at its own return. Out-of-order
or conflicting release is an invariant failure, not a silently leaked block.
The existing task/borrow/native execution owner remains responsible for thread
serialization; this is not a new cross-thread reclamation mechanism.

The source recovery ladder retains the entire original state machine and
allocator; only physical allocation lifetime crosses the adapter boundary.
This corrects a demonstrated source-level unsafe reuse window under standing
original-host correction authority. Runtime reproduction is not claimed.
Acceptance: nested destruction/recreation, NC plus final cleanup, exceptional
unwind, independent windows and generations, no early pool reuse/double free,
ordinary click/capture/focus behavior, class selection and task exit. Register
all canonical creation/dispatch/terminal consumers together before product
selection; no complete Button or guest pass follows from importing the file.

The complete btnctl.c is now imported unchanged after newline normalization.
wow_button_lifetime implements the bounded stack-frame deferral and calls the
unchanged FreeLookasideEntry at final unwind. Its scope requires the existing
retained private WND and task/borrow boundary; it neither creates state nor
changes the original pbutn->spwnd clearing or CLEANEDUP flag. This prevents
physical reuse only: it does not prove all outer original behavior safely
handles cleared back-references. Source review and actual original-consumer
destruction cases remain mandatory before product selection.

The unrun wow_button_lifetime_fixture models nested NC cleanup, exact-state
generation identity despite equal HWND, immediate unrelated-generation release,
normal/exception outer unwind and last-error preservation with explicit
allocator/procedure doubles. It does not prove the original Button algorithm
or pool behavior. The next grouped runner includes it; no per-leaf build is
started. The complete Button body still needs its flag/declaration facade,
stored-text native boundary, canonical message table and coordinated entry/
creation/finalization/terminal selection. Do not publish the partial graph.

The unchanged gawButtonWndProc table is now retained in the already selected
kernel/server.c subset and initialized in the same unpublished candidate as
other control tables. Original source SHA256
3C6131EFB37173F5A280BD77EC1D887529AFB35EEF03B6F0248A3C75E7887047;
definition comparison passes after newline normalization. Its FNID slot is
inside the existing awmControl range, so no array/layout expansion or new
message registry is needed. Existing failed-initialization cleanup iterates
all slots and includes Button without a special rollback path. Membership
and immutable-reuse assertions are added to wow_default_messages_fixture,
unrun. This does not select the original Button procedure by itself.

Original button style/state constants are retained under the117 opt-in guards
in user.h/usercli.h; szOneChar is declared against its existing original
clglobal.c definition. No native USER flag values are sampled or fabricated.
The button invocation wrapper now saves entry last-error before validation
and rejects a missing procedure explicitly. Remaining text service traces to
client/ntstubs.c _DefSetText: original LARGE_STRING construction then
NtUserDefSetText, not SendMessage(WM_SETTEXT) or a recursive SetWindowText call.
Reuse that original construction through the counted native boundary and
publish the existing retained name view on return; do not add a second name
cache. Its native ABI/resource closure is not yet selected or verified.

### Grouped retry r9: drawing composition failures

Owned hidden runner37296, start2026-10-17T00:09:46.7316201Z, ran the
supplemental cohort in original-bodies-r9 against the existing x86 build root.
Compilation failed; the runner correctly skipped all fixture execution.
ClientDrawState lacked original DSS_DEFAULT, and its registered try/finally
adaptation left CDS_Leave immediately before a closing brace. Restore the
original winuserp.h constants in the existing control ABI declaration owner;
source SHA256 B2838AE608353A14659B85C2BE290034A6BB02DEE9AC078E82C298F2F9B9B513.
The dedicated drawing facade consumes that declaration instead of maintaining
three private constants itself. OPENNT-HOST-116 also registers a null statement
at CDS_Leave, required by C syntax after adding finally cleanup; no algorithm
or return-order change. Rebuild the group before claiming these checks pass.

### Grouped retry r10: supplemental cohort passes

After the two composition repairs above, hidden runner13820 started at
2026-10-17T00:12:46.2391107Z and exited0. Evidence is under
build/M0-T447/S3/original-bodies-r10 plus its adjacent runner stdout/stderr.
The runner captures source/graph/toolchain hashes, child identities, elapsed
times and exit codes. All18 selected x86 fixtures exit0. New explicit markers:
mnemonic_drawing_failures=0, control_draw_state_failures=0 and
button_lifetime_failures=0. The earlier r9 build exits2 after147.2099438s;
it executed no tests and remains negative evidence, not a pass.

These checks exercise original prefix/output and DrawState algorithms,
real GDI scoped-resource cleanup including callback exceptions, and modeled
Button state deferral/nesting/generation/error boundaries. They do not prove
the full original Button procedure, native class selection, real HWND
destruction or Win16 integration. Button message-table assertions in the
separate default-messages fixture are still unrun. Existing original-source
warnings remain; no warning-free build is claimed. No guest or release image
changed and no product publication/commit is claimed.

The text-service review additionally traces original kernel/ntstubs.c
NtUserDefSetText to kernel/getset.c DefSetText, without a title-redraw step.
By contrast kernel/dwp.c WM_SETTEXT calls DefSetText followed by
xxxRedrawTitle. Button's WM_SETTEXT calls _DefSetText and then its own paint
path. Therefore a generic DefWindowProc(WM_SETTEXT) replacement is not proven
equivalent either; preserve the counted storage operation and existing return
publication boundary. The native text-service binding and full Button
destruction/reentry group remain finite open dependencies, not implemented
or waived by this supplemental pass.

### OPENNT-HOST-118: original stored-text client construction

Recover unchanged client/ntstubs.c _DefSetText and rtl/chartran.c
RtlInitLargeAnsiString, alongside the already retained Unicode constructor.
Pinned source hashes respectively:
7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01 and
0F9F0B7FDE3AAA475492A50F333745508698228C38FDBF9A21CBD4BFBEAD383B.
Original caller is ButtonWndProcWorker WM_SETTEXT. Preserve null-pointer vs
empty-string input, ANSI flag, byte counts and original return/error macros.
No replacement parser, conversion buffer, text cache or recursive message.
The full ntstubs unit requires excluded USER services; retain this complete
original wrapper in its existing opt-in subset. Its finite native service
belongs behind the existing call/return and retained-window owner, not another
text authority. Original host bodies have zero algorithm edits; no guest edits.

Read-only current x86 ABI inspection finds win32u NtUserDefSetText export at
RVA1900 with ret8; image SHA256
31B7FE5EA35A8C0A79051C58C8E0AE1ADDCDA323F661846B60DCFEB1152E5A1C.
Matched USER PDB de141706-5f1a-81a0-a689-324c2870a1c0 identifies _DefSetText at
RVAab485 and its imported _NtUserDefSetText@8. USER image SHA256
56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E.
These are inspection locations only, never runtime addresses or ordinals.
Two stack arguments alone do not prove the counted-string layout; native
caller inspection and grouped null/ANSI/Unicode/failure/lifecycle acceptance
are still required before selecting the service. Original wrapper staging
does not establish native service or full Button integration.

Caller inspection now rejects direct original-structure pass-through: current
USER _DefSetText calls constructors at RVA30500/ccf3c, both writing Buffer at
+8 and an additional zero DWORD at+12. Length is+0 and the ANSI bit is bit31
of+4. Original x86 LARGE_STRING is only12 bytes. The native boundary must
marshal a16-byte descriptor, with the extra field zero, and keep its pointed
text borrowed synchronously. This is copied input representation, never a
native private-object cast. Preserve all original byte counts and ANSI state.
Use only the named NtUserDefSetText export, not those inspection addresses.
Its entry and return use existing exact-window borrow/native-call owners.
The existing dialog-creation native leaf also passes original LARGE_STRING
directly: treat that as an open ABI defect requiring its own caller tracing
and the same representation audit, not an already verified creation path.

Implementation staging now retains both original function bodies unchanged
(newline-normalized comparison passes). The existing EDIT/control native
export owner resolves NtUserDefSetText by name and marshals the16-byte input;
the existing call adapter retains exact WND identity, restores execution and
publishes through wow_user_native_call_end, preserving native result/error.
Required publication failure is terminal, never a false result after a
successful irreversible store. There is no independent stored-text cache.
The production generator selects the original client wrapper as its own
opt-in object with the shared control facade; full Button selection remains
open. ANSI constructor and call-boundary assertions are staged in existing
fixtures and are unrun. Earlier r10 does not validate these changed inputs.
Native-layout/real-window failure and destruction tests remain required;
no per-leaf rebuild, guest change, product publication or commit occurred.

The descriptor conversion is consolidated as a private WOW native-input
header, wow_native_text_descriptor.h, rather than duplicated per service.
It owns no storage, discovery, callback or policy; it copies original counts,
encoding and pointer into the inspected16-byte call-local representation.
Native strings and original strings remain distinct types. Original class
resolution still turns class names into atoms before the creation service.

Creation caller tracing confirms the same representation: current USER helper
2E5B0 constructs its title at stack+18, writes pointer at+20 and zero at+24
(descriptor+8/+12), and calls the same ANSI constructor CCF3C at2E7E3.
The Unicode title path writes the same pair at2E844/2E84D. The previously
audited2DF70 path forwards this counted title to the17-slot service.
Therefore replace the staged creation leaf's original12-byte pass-through
with the shared16-byte conversion; keep both resolved-atom arguments as atoms,
not converted/dereferenced strings. Native callbacks borrow original title
storage synchronously; the stack descriptor remains live across the call.
No mirror/body change is needed. Register this as correction of the earlier
ADAPTER-WOW-062 ABI staging, not a new creation policy or compatibility waiver.

Both native leaves now consume wow_native_text_descriptor and the one
wow_native_text_capture conversion; the earlier private native_control_text
duplicate is removed. Sweep of WOW32 LARGE_STRING consumers found the two
native exits plus original class-resolution/enrollment callers. Those callers
retain original input; only service exits change representation. No raw
original title cast remains at native creation. All extra argument slots and
class atom identities stay unchanged.

Staged acceptance extends wow_dialog_native_creation_fixture: its four
creator/title encoding combinations now place the12-byte original descriptor
at a readable page boundary, with the following page PAGE_NOACCESS. Assert
conversion fields, unchanged input, counted title rather than trailing text,
and existing creation/rollback/destroy cases. wow_edit_native_fixture adds
direct Unicode/ANSI counted storage, null clearing, invalid HWND rejection
and zero WM_SETTEXT redispatch assertions. These are source additions only;
they have not run. No claim of cross-version native ABI or completed guest
control integration follows. The grouped build remains deferred until the
remaining Button dependency/lifecycle group is connected, as owner requested.

Button lifecycle tracing confirms original client/usercli.h Unlock really
clears the pointer (it is not a no-op server lock). In btnctl.c, WM_KILLFOCUS
calls xxxBNReleaseCapture, which can notify a parent, then calls xxxBNGetDC;
that helper reads pbutn->spwnd through IsVisible. WM_CTLCOLOR callouts in
xxxBNInitDC also precede subsequent pbutn-based drawing helper calls.
Physical block deferral alone therefore remains insufficient proof; no
original-state resurrection or general callback-abort policy is introduced.
Keep these concrete post-destruction paths in the existing117 lifecycle
acceptance group before selecting the full original Button procedure.

Button drawing has an independent scope requirement: BNCalcRect reads atlas
metadata before DrawState, and xxxButtonDrawCheck blits outside DrawState.
Do not borrow the nested DrawState TLS context or add a process-global atlas.
Extend the existing117 button invocation frame with its own113 resource scope;
initialize metadata lazily from the active control display depth, and release
GDI resources on the same finally path as private-state deferral. Nested
button invocations own independent resources and recover the outer frame.
Final destruction alone must not acquire a DC or depend on display capture.
This is resource wiring, not a second button renderer. Original check/radio
state selection and BitBltSysBmp remain the intended algorithms. Full original
body/build selection stays pending on the recorded lifecycle closure.

The117 invocation frame now implements that scoped resource ownership and
provides the native atlas blit leaf. Its existing exact-WND/physical-state
retention is unchanged. Extended wow_button_lifetime_fixture links the actual
resource carrier and original table, checking lazy metadata, nested independent
atlases, recovery of the outer resource scope, and GDI release on normal and
exceptional unwind. Destruction-only tests retain a missing drawing context,
so cleanup cannot accidentally start requiring display acquisition. These
new assertions are staged, not run. Remaining drawing binding is the original
BitBltSysBmp caller facade; do not redirect it to an unrelated outer DrawState
scope during nested button callbacks. Canonical creation/class/finalization
selection and post-destruction control behavior remain the same finite group.

The117 drawing facade now composes the unchanged original BitBltSysBmp in a
separate object, with only its resource inputs bound to the current Button
invocation. WOW_ORIGINAL_BUTTON_BLIT_ONLY excludes the already supplied
BltColor definition from that object; no helper algorithm changes. The
supplemental lifetime fixture links this real helper and compares every
check/radio/three-state image pixel against the current invocation's atlas,
both inside a nested invocation and after recovering the outer scope. This
does not depend on a DrawState context. These assertions remain unrun until
the recorded whole Button dependency group is ready; product selection and
publication remain pending. No guest input or release artifact changed.

Static verification after this facade change: the selected BitBltSysBmp
body is identical to the pinned original after newline normalization;
git -c core.safecrlf=false diff --check passes. Background documentation
governance PID34700, start2026-10-17T00:33:00.2759906Z, exits0; captured logs
are build/M0-T447/S3/button-blit-governance.stdout.log and .stderr.log.
These checks are not compilation, runtime acceptance or delivery evidence.

Button consumer wiring now stages all five identified selection sites:
the shared control selector supplies distinct scoped A/W entries; explicit
creation recognizes them as recovered procedures; class/module cleanup uses
the same canonical W entry rather than native Button fallback; terminal
window retirement includes Button; finalization invokes the original body
inside its private-state/resource scope before identity retirement. The
outer existing control entry still owns task, retained WND graph and native
return scopes. No second class or task registry is introduced. Standard-class
target assertions now include Button. The former native-button behavioral
case in that fixture still needs a properly enrolled original-control setup;
it is not accepted evidence for this changed target.

The production graph stages btnctl.c, its lifecycle binding, the original
button-only blit object and DrawPushButton selection. The supplemental cohort
adds btnctl.c as well, not a standalone per-leaf build. The shared facade maps
only the proven atlas, icon-size and deferred-free dependencies. The original
BS_PUSHBOX declaration is retained from pinned winuserp.h line884. No Button
algorithm body is changed by this wiring. This is an intermediate, unbuilt
graph, not runnable-product acceptance; published P16 remains untouched.

Further source inspection corrects the destruction hazard precisely:
rtl/winmgr.c IsVisible(NULL) completes its loop and returns TRUE. Therefore
xxxBNGetDC can reach HWq(NULL) after a nested destruction clears spwnd; it is
not safe merely because IsVisible is a helper. BNReleaseDC also reads the
cleared backlink. The remaining117 closure must handle post-callout liveness
and already-acquired DC release together, without resurrecting spwnd, removing
the original logical cleanup or treating a reused HWND as the same object.
No broad exception-catching or blanket callback-abort behavior is installed.

117 minimal host correction admission: preserve original control-color
dispatch and immediate logical destruction. After GetControlBrush returns,
do not select fonts or continue painting if the callback cleared spwnd.
xxxBNGetDC rejects an already cleared backlink before IsVisible (whose NULL
result is TRUE); when destruction occurs during color initialization, release
the acquired DC using the HWND captured before that callout. BNReleaseDC
receives the worker's already captured HWND, not a lookup or restored spwnd.
This changes one private helper signature and its six same-file callers;
no public ABI, resource registry or guest change. These are bounded cleanup
corrections, not permission to replace the original button state machine.
Focus/capture/notification and radio traversal callouts remain separate rows
of the same117 closure. Full original-body lifecycle assertions are required
before acceptance; current boundary doubles alone cannot prove this repair.

The pending grouped runner now includes wow_button_dc_fixture. It links the
actual btnctl.c DC bodies using function-level linking, with observed real
GetDC/ReleaseDC leaves and an explicit color-callback double that performs
the logical backlink clear. It checks normal acquisition, teardown before
release, rejection before acquisition and teardown inside color acquisition;
each acquired DC has one release with the captured HWND. This is not native
destruction enrollment, whole Button callback behavior or guest evidence.
No build/test has run for these new inputs yet.

The117 callout correction also covers focus/capture and click dispatch:
after SetCapture/SetFocus, nested BM_SETSTATE/WM_LBUTTONDOWN/UP or parent
notification, stop further control behavior when logical cleanup has cleared
the same BUTN backlink. NotifyParent rejects a cleaned/deleted retained WND.
No HWND-only IsWindow test is sufficient because the numeric handle can be
reused. Radio iteration checks the owning button after each message and
balances its existing ThreadUnlock before early return; a missing next item
ends traversal rather than dereferencing NULL. Drawing text also rejects a
cleared backlink, covering BS_USERBUTTON's notify-before-text path. Private
block deferral continues to make these checks safe; it does not delay logical
cleanup. These guards require grouped destructive-callback tests and are not
yet verified runtime behavior.

Whole-cohort original-bodies-r11: owned runner38144 started
2026-10-17T00:39:47.9367779Z and exited2. Ninja29452 started
00:39:49.8375609Z, exited2 after84.1404203s; no fixture ran. The two complete
btnctl.c compilation variants expose three absent declarations, not new
runtime results: BS_HORZMASK/BS_VERTMASK from pinned winuserp.h887--888 and
WFTABSTOP from pinned user.h1777. These exact values are now retained in the
existing declaration carriers under117; no replacement algorithm is added.

Whole-cohort original-bodies-r12: runner37756 started
2026-10-17T00:41:40.4134222Z, exited1120. Ninja17756 started
00:41:42.1593391Z, exited1120 after161.55655s. Both complete btnctl.c
variants compile; there are no C4013 or C compilation errors. The new DC
fixture fails linkage with35 unresolved dependencies from the rest of the
same original object. /Gy plus /OPT:REF does not provide the assumed partial
link closure. All runtime fixtures were correctly skipped after build failure.
Reject that standalone-link assumption; retain the DC assertions and complete
their real dependency composition before rerunning. Do not insert untyped
aliases or no-op providers to manufacture a pass. No product build, publication
or guest acceptance follows from the successful object compilation.

The DC fixture now declares its non-exercised boundaries explicitly in
wow_button_dc_unreached.c, compiled against the production facade's exact
prototypes. Every function terminates with diagnostic exit99 if reached;
none returns a fabricated result or implements control behavior. Required
data symbols have their declared types. This permits the linker to resolve
the complete object while testing only the named original DC bodies. It
does not establish the other35 dependencies, full Button entry, or runtime
cleanup. The color/visibility doubles and their limitations remain explicit.
No untyped linker alias, mirror crop or production substitute is introduced.

original-bodies-r13 runner32108 (2026-10-17T00:47:09.9712865Z) exits1.
Ninja13948 starts00:47:12.1485341Z, exits0 after13.6434614s. All19 fixtures
execute;18 pass, including button_dc_failures=0. The Button lifetime fixture
fails its two combined post-unwind error/DC-handle assertions (lines150/154),
with button_lifetime_failures=2. The blit pixel and nested-scope assertions
do not fail. This is not a whole-group pass. Split the two combined assertions
to retain actual last-error and object-type values; no assertion is weakened.
Resource release versus witness behavior remains unclassified pending those
values and source tracing. No new runtime release or guest change occurred.

original-bodies-r14 runner3796 (2026-10-17T00:48:16.7956425Z) exits1.
Both failing resource assertions preserve error291095 (0x47117), but report
GetObjectType=3. original-bodies-r15 runner32248 starts
2026-10-17T00:49:21.3475462Z and exits0; all19 fixtures pass. Its independent
native control creates a compatible DC, asserts successful DeleteDC, then
also observes GetObjectType=3. Therefore the removed zero-object-type assertion
was an invalid release witness on this host, not a demonstrated product leak.
The replacement witness observes actual native DeleteDC/DeleteObject calls,
requires every return to succeed, exact two-DC/two-bitmap nested release counts
per normal/exception case, and the outer DC identity. Object-type diagnostics
remain printed. Last-error, nested atlas isolation, original pixel blits and
deferred exact-generation private-state release assertions remain enabled.

The resource observer changes only the fixture compilation of the existing
carrier; no production release behavior is changed to satisfy these tests.
The DC-only fixture also passes with no unexpected dependency invocation.
These19 supplemental results cover their declared boundaries, not complete
Button focus/radio destructive callbacks, enrolled native destruction, the
full WOW32 product link, actual guest controls or all13 delivery. Those remain
S3 obligations. P16 and original guest inputs remain unchanged.

Production-consumer review after r15 finds a concrete remaining selection
gap: the provider generator selected control adapters but not the24 original
client collaboration bodies already compiled by the supplemental cohort.
The product graph now explicitly selects all24 with the same client/dialog
facade flags, plus the original RTL winmgr/wow retained-view bodies. Distinct
control-body/control-view object names avoid collisions with MVDM thunk and
previous original subset objects. The original source bodies are unchanged.
This is product graph wiring, not a full provider-link pass. Default-handler
and get/set selection, original dialog keyboard/mnemonic closure, opaque CPD
normalization and the recorded lifecycle acceptance remain open. No native
fallback is introduced to hide those missing providers.

Dialog keyboard/mnemonic source tracing identifies a concrete additional
private-state borrower: original dlgmgr2.c IsDialogMessageW and dmmnem.c
xxxGotoNextMnem save PBUTN, set BST_DONTCLICK, call DlgSetFocus and clear the
bit afterward. No outer Button WNDPROC necessarily exists, so Button-only
invocation deferral cannot protect that saved pointer during focus destruction.
The existing117 frame is now exposed as a caller-owned wow_button_scope;
Button invocation itself uses its enter/leave, preserving the same stack,
resource and oldest-frame release owner. No new registry, reference table or
state copy is added. A staged fixture models the direct original-dialog
borrow around nested finalization. Those assertions and refactoring are not
covered by r15; no repeated per-leaf build is performed. Original dialog
keyboard/mnemonic import and its two scope consumers remain pending, including
post-focus HWND lifetime and full callback acceptance.

The mnemonic/filter outbound sweep finds two direct native-call gaps in the
shared facade: Unicode GetWindowText and CallMsgFilterW can run application
code, but only ANSI GetWindowText had the existing native-return wrapper.
Both now use the same wow_user_task_lifecycle native-call macro family;
GetWindowTextW is mapped beside its ANSI counterpart, and original dialog
CallMsgFilter selects the Unicode boundary. The latter captures the input
message HWND before the Hook may mutate the message. Modern USER still owns
message transport and Hook execution; original dialog code owns the decision
to filter and its processing order. No new Hook engine, state cache, guest
change or semantic waiver is introduced. These staged leaves remain untested
until the complete keyboard/mnemonic cohort is wired.

OPENNT-HOST-119 source admission: pinned OpenNT client/dmmnem.c SHA256
638EFC31FFD622BA31E16672D1418291C3E1291B6D0C57F31D08C32D45FA1720
contains FindMnemChar, xxxGNM_FindNextMnem and xxxGotoNextMnem. The selected
original dlgmgr.c modal loop requires IsDialogMessage, whose original
dlgmgr2.c body directly calls this cohort. Retain the original source owner,
not an adapter reimplementation or the current native IsDialogMessage bypass.
The required outward closure is finite: original child/group/tab traversal,
original DlgSetFocus/default-button collaboration, existing retained WND
views, public character case conversion, and scoped text/message/focus/Hook
transport. No USER server/kernel state is needed for the mnemonic algorithms.

Import disposition: complete original dmmnem.c, not yet build-selected.
Before selection, register and implement the two known lifecycle borrowers
and resolve the original function's missing return at the exhausted512-step
search tail (currently undefined result); do not treat original source identity
as proof of safety. Original dialog IsDialogMessage A/W requires the retained
RTL message-character converter, scoped entry and A/W ABI naming, plus graph
revalidation after filters. These are the same finite keyboard/mnemonic group,
not deferred optional behavior. Guest USER remains immutable.

119 correction registration before selection: keep the original DONTCLICK
set/focus/clear sequence inside the existing117 Button scope, leaving it in
finally so focus exceptions cannot leak a pending private-state release.
After focus or flash callbacks destroy the exact retained window, skip any
subsequent click/parent notification. Never use IsWindow on a possibly reused
HWND as the identity test. On the original512-iteration exhaustion path,
return NULL after the existing ThreadUnlock; this is the same not-found
result used by the other exits, replacing an indeterminate return value.
Both changes require original-body lifecycle/negative assertions before
acceptance. There is no new mnemonic parser, focus policy or guest change.

119 additionally admits unchanged IsDialogMessageA/W from pinned
client/dlgmgr2.c SHA256
A3E9286C40E4E11EC872FD9B2D7B23CBA640F09FB4840C4E6130E301D5277B9E.
Retain both bodies under WOW_ORIGINAL_DIALOG_KEYBOARD in that original owner,
beside the already recovered default-button workers. Do not select the bodies
until the existing source-shaped entry, name/ABI mapping, task code-page
input, retained converter and callback lifetime are complete. This recovers
algorithms, not another copy in an adapter; direct modal/modeless callers
must subsequently share one scoped entry. The Button declaration gate is
also available as a state-only selector, so dialog code can read the original
flags without redirecting its unrelated FreeLookasideEntry or drawing atlas.

119 retry audit: the source-only keyboard group still clears BST_DONTCLICK
and restores IsDialogMessageA's caller-owned ANSI wParam only on normal return.
A focus/filter callback exception can leave a live button's suppression bit
or the caller's MSG altered. Register moving these original restoration
statements into finally, with the existing Button scope released afterward.
No catch/retry, replacement dialog algorithm or guest change is admitted.
Original direct reuse lacks exceptional restoration; a facade cannot restore
these function-local saved values without replacing the original owner.
The bounded mirror correction retains the normal sequence and exception
propagation. Whole original keyboard-body lifecycle tests remain required.

Code-page source tracing identifies the original producer rather than assuming
ACP: kernel/kbdlyout.c obtains CHARSETINFO from xxxClientGetCharsetInfo and
updates pClientInfo->CodePage on layout activation; inc/ntcb.h's client side
uses TranslateCharsetInfo with TCI_SRCLOCALE. rtl/chartran.c consumes that
thread value. Existing production task-client state has no CodePage producer.
The original client/client.c ToAsciiEx also demonstrates a public locale query
derived from HKL. This is reference evidence only, not proof that every modern
HKL maps identically; no fabricated private flag, cached second code-page
authority, blanket ACP binding or converter selection has been installed.

Retry grouped verification: original-bodies-r16 started runner38204 at
2026-10-17T01:00:19.1134194Z in the restricted execution environment. It left
input manifests and empty build logs, but no completion/exit record; the
runner was no longer present. Its outcome is incomplete, never a pass. No
second run began while that runner was observed live. Retried outside the
sandbox as original-bodies-r17: runner25652 at01:01:18.3365559Z, Ninja35736
at01:01:19.6815242Z, build exit0 after100.1159714 seconds, runner exit0.
All19 supplemental fixtures pass. The Button lifetime fixture additionally
proves caller-owned dialog scope retention through nested finalization,
exception cleanup, preserved last error and a subsequent independent call
without a stale scope. Native resource-release observations also remain green.
Source/build hashes and full logs remain under build/M0-T447/S3/original-bodies-r17.

This verifies the scoped lifetime adapter, not real original dialog keyboard
execution. The119 DONTCLICK/ANSI restoration edits were applied after r17 and
remain untested; the keyboard selector is still off. The production graph,
full retained window lifecycle, code-page/CPD boundaries and actual guest
acceptance remain open. No publication, commit or push occurred, and P16
remains the deployed all13 baseline. No guest image/source was changed.

119 converter implementation admission: recover the complete original
RtlMBMessageWParamCharToWCS body from pinned rtl/chartran.c. Its unchanged
conversion consumes two ANSI bytes and preserves non-character messages.
The inaccessible NT4 CLIENTINFO code-page slot is the only replaced input:
ntuser_rtl_compat queries GetKeyboardLayout(0)'s language and the locale's
default ANSI page using the public query shape already present in original
client/client.c ToAsciiEx. Failure retains the original keyboard-layout
producer's CP_ACP fallback; range is checked before narrowing to WORD.
This binds the current native input-layout resource, not a cached task flag
or a second keyboard-layout manager. Preserve last error around this input
query. Original algorithm remains in chartran.c; no handwritten character
converter or native IsDialogMessage substitution. Tests must include ordinary
characters, packed pairs, unchanged unrelated messages and layout/query
failure before the complete keyboard group can be accepted.

119 scoped entry composition: modeless WU32IsDialogMessage and original
dlgmgr's modal loop must share wow_dialog_messageA/W. The entry owns only
the existing task callback scope, borrow scope and dialog context; the
original IsDialogMessage bodies own filtering, focus, mnemonic and dispatch.
Fail entry before dispatch on missing context; unwind in reverse order and
preserve the original result/error. No native IsDialogMessage fallback is
permitted after an original handled/failed call. Private symbol renaming
separates the recovered body from the SDK dllimport; this is not another
implementation. These bindings are staged with the same unaccepted group.

The recovered converter's exact upstream SHA256 is
0F9F0B7FDE3AAA475492A50F333745508698228C38FDBF9A21CBD4BFBEAD383B.
Current group now stages both production caller redirects, private original
symbols, dmmnem/product selection and supplemental complete keyboard-body
compilation. The old navigation-only fixture deliberately retains its
non-keyboard object; it does not prove the newly selected functions.
The separate scoped-entry fixture tests ANSI/Unicode forwarding, nested
context restoration, result/error preservation, entry failures and exception
unwind with original-body doubles. String assertions cover all original
character-message selectors and packed two-character output. Native layout
failure and original-body destructive callback tests remain outstanding.
119 also rejects a NULL message-target validation after CallMsgFilter before
traversing its original WND graph; the filter can invalidate that HWND.
No keyboard conversion or scoped-entry claim is covered by r17.

Whole keyboard/cohort run original-bodies-r18: owned hidden runner35808
started2026-10-17T01:08:32.3622431Z, Ninja21296 at01:08:33.7957364Z.
Build exit0 after70.8841551 seconds; all20 boundary fixtures exit0 and runner
exit0. Exact input/build hashes and stdout/stderr are sealed under that run.
The complete recovered IsDialogMessage and dmmnem units compile without
implicit-function diagnostics; the keyboard-entry fixture reports failures0
and string fixture WOW_CONTROL_STRING_OK. The original converter body equals
the pinned upstream function after newline normalization. MVDM precomp's
existing private-access inclusion supplies WU32IsDialogMessage's redirect;
no additional MVDM mirror edit was needed.

Evidence limits: the keyboard-entry algorithm is a declared test double;
the full original keyboard bodies are compiled, not linked into that fixture.
Neither result proves actual dialog keyboard/child destruction, handle reuse,
native locale-query failure or the complete production link. Remaining119
acceptance must cover those actual original-body transitions before release;
the general S3 retained-graph/CPD/default-owner dependencies remain separate
fixed obligations. No guest changed, no new release/commit/push is claimed.

119 destructive-callout source audit: dmmnem retains PWND storage but can
continue to a text query, focus or click after WM_GETDLGCODE/GetWindowText
destroyed that exact window. A retained allocation does not keep its HWND
live, and HWND-only revalidation could select a replacement generation.
Register exact retained WFDESTROYED/FNID_STATUS_BITS guards after both query
forms, after ordinary focus and before final return. Existing push-button
flash guards and scoped BUTN lifetime remain. Each stop balances its existing
ThreadUnlock and returns the existing no-match NULL; original navigation,
mnemonic matching and normal notification order remain unchanged. Direct
original reuse lacks those post-callout checks; a native service leaf cannot
choose the caller's traversal return. No new renderer/behavior engine or
guest change is needed. Verify the actual original dmmnem body with explicit
native-call and graph doubles; label HWND reuse there as simulated, not real
Windows handle allocation or full guest proof.

The added wow_dialog_mnemonic_lifetime_fixture links the actual dmmnem
object and production Button scope, not a mnemonic substitute. Its two-node
navigation/native-call doubles exercise normal push/other-button matching,
both dialog-code query stages, text destruction, both flash transitions,
ordinary/button focus destruction, click destruction and live/destroyed
focus exceptions. The allocator asserts DONTCLICK was cleared before free;
each native double rejects calls after simulated HWND reuse. A final normal
call verifies no stale Button scope after exception. This proves the named
original-body continuation guards; real USER graph/handle reuse remains open.

Grouped original-bodies-r19: hidden runner24560 started
2026-10-17T01:12:57.9029226Z; Ninja8168 at01:12:59.1393199Z, build exit0
after8.9087562 seconds. All21 fixtures exit0; runner exit0. The new original
mnemonic witness reports failures0. Retained input hashes and logs are under
build/M0-T447/S3/original-bodies-r19. No implicit-function or compile/link
errors are present. This does not certify untested keyboard entry branches.

Finite remaining keyboard lifetime sweep: IsDialogMessage's WM_CHAR and
WM_KEYDOWN dialog-code replies, Tab focus-to-default-button update, arrow
radio/nonbutton focus-to-default update and radio query-to-click, plus dialog
destruction during any of those callbacks. DlgSetFocus's internal query,
selection and focus sequence is part of that same closure. Check retained
exact identity before a later operation; do not use a replacement HWND as
evidence of survival. These are keyboard-group obligations, not S4 expansion.
Full product publication/guest gates remain unfulfilled; no package or guest
was changed and no S3 commit/push occurred.

119 next keyboard group registration: DlgSetFocus retains the exact validated
PWND before its first WM_GETDLGCODE and rejects destroyed/cleaned state after
that query and optional EM_SETSEL, before setting focus. IsDialogMessage
checks both its retained dialog and control after character/key dialog-code
replies; after Tab/arrow focus it skips default-button processing for a dead
target/dialog, and a radio query cannot be followed by BM_CLICK after either
has died. Reuse existing processed-message unlock exits, not a new dispatcher.
Default-button callbacks remain part of this finite group; their source
requires the same check before subsequent queries/style writes.

Attribution clarification: original kernel/handtabl.c HMMarkObjectDestroy
marks HANDLEF_DESTROY and defers destruction while cLockObj is nonzero;
HMFreeObject requires zero locks. The standalone borrow retains copied WND
storage, not modern USER's HWND allocation. These guards repair the modern
integration lifetime boundary; original source's lack of identical guards
alone is not proof that original NT4 reproduced modern HWND reuse failures.

The staged119 continuation now includes default-button removal/query/style
sequences and Enter's focus-query identity. Enter retains its target before
WM_GETDLGCODE rather than resolving the same numeric HWND after that callback.
Its default-ID query also checks dialog retirement. No additional guest or
MVDM thunk diff is introduced. Tests/adapter-mvdm-host-out/wow/
wow_dialog_keyboard_lifetime_fixture.c links actual original dlgmgr2 plus
production BUTN scope, with explicit graph/native/focus doubles. The cases
cover source query, Tab/arrow focus, arrow target query and radio-check
destruction of target or dialog, plus normal push/radio behavior. The default
button algorithms are linked, not replaced by test implementations.

These post-r19 source changes and fixture are unbuilt/unrun. The next unified
run must not reuse r19 as proof. Remaining finite test preparation includes
actual DlgSetFocus query/selection/focus body (currently doubled by this
fixture), destructive default-ID/style cases, Enter focus identity and ANSI
message restoration. No new compile was launched for each branch. Full
production/guest acceptance and the broader fixed S3 obligations remain open;
no publication, commit or push occurred.

Retry evidence, 2026-10-17 UTC: original-bodies-r20 runner3688 started
01:23:10.4017684Z; Ninja29568 started01:23:11.7510118Z and built successfully
in13.9012736 seconds. Of22 fixtures,21 passed; keyboard lifetime failed once
at its native-send assertion (fault DEFAULT_ID, target destruction). A copied
lookup returned the retained retired target after DM_GETDEFID. The original
continuation then queried its HWND. Registered119 now rejects destroyed or
cleaned copied targets before that query; no replacement HWND validation,
new behavior engine or guest change is added.

original-bodies-r21 runner27576 started01:24:00.0170538Z; Ninja21048 started
01:24:01.1402524Z, build exit0 in9.5494092 seconds. All22 fixtures and runner
exit0; original-keyboard-lifetime reports failures0. Each run retains source
hashes and individual logs below build/M0-T447/S3. This supersedes the staged
unbuilt statement above for the tested group. Actual original DlgSetFocus is
now linked, with fail-fast test-only providers for its unreached dependencies.
Query/selection/focus destruction, default-ID destruction of target or dialog,
and ANSI restoration on filter exception are exercised with explicit doubles.
These are not real HWND reuse, full keyboard branch coverage or guest proof.
Production composition, remaining fixed S3 lifecycle/behavior gates and
publication remain open. Published P16 is unchanged; no commit/push occurred.

Post-r21 source composition audit: production selects the default binding,
message tables and geometry but omitted client/clmsg.c, the actual provider
of wow_original_default_window_proc. Select that existing complete original
body in both graphs, under WOW_ORIGINAL_DEFAULT_WINDOW and its finite
control-client/default-precomp.h environment. The facade has no EDIT TLS
dependency: retained WND/relationships, existing native-call leaves, original
WCSToMBEx and rtl/wow.c::_GetClientRect remain the owners. Original user.h
lines1934/2629 provide GetChildParent/DO_DROPFILE; pinned winuserp.h supplies
SHELLHOOKINFO and SWP_STATECHANGE/NOCLIENTSIZE/NOCLIENTMOVE. This extends
089/090 composition, not the default-worker algorithm or approved semantics.
No new mirror body diff or guest modification is introduced.

This selection is unbuilt/unrun. Finish the finite default/getset service
closure before the next grouped compile; r21 is evidence only for its sealed
inputs. CPD normalization and style-write notification compatibility remain
explicit unresolved dependencies, not fake successful providers. Source audit
also confirms the existing original chartran.c defines WCSToMBEx and the
production control-view-wow object selects _GetClientRect. Full product and
real guest evidence remain required; no new package is published.

Get/set closure source audit after default selection (no build run):

| Reached original owner/edge | Current binding and finite remaining obligation |
| --- | --- |
| rtl/getset.c::_GetWindowLong, nonnegative private/control words | wow_window_client_try_get_long retains owned state and treats zero/error as handled. Original body still needs production selection. Preserve EDIT index-zero dereference and original bounds/denial policy; never delegate its failure to native extra storage. |
| client/ntstubs.c::_SetWindowLong | try_set_long selects nonnegative words and GWL_STYLE; original client preserves initialized ListBox owner-draw/string bits. Needs its own opt-in object and complete facade, not reuse the unrelated ntstubs class-query object. |
| kernel/getset.c::xxxSetWindowLong | Existing wow_window_client_set_window_long reaches this unselected original. FCallerOk can bind only to existing exact local-state ownership, not a recreated system/logon privilege model. CBFNID must reuse the existing original extent owner. |
| DWL_DLGPROC GetCPD/HMValidateHandleNoRip | Native GetCPD token generation exists. The client also needs previous address AND encoding when accepting a token; existing class registration resolver is NULL, and class-probe evidence recovers no reliable encoding. Do not reinterpret native tokens as PCALLPROCDATA, silently skip normalization, or borrow the class facade that lacks wType. |
| Negative-index GetWindowData/xxxSetWindowData | Neither blanket native dispatch nor a successful placeholder is admitted. Existing procedure/WW identity handling remains authoritative; style mutation's pre-4.0 notification difference is still awaiting its explicit compatibility disposition. |

This separates a missing object selection from genuinely unresolved service
semantics. The already-selected logical WND procedure forwarder in
wow_window_words_binding.c preserves an opaque token for CallWindowProc;
it is not a decoder usable by original dialog get/set, which must store the
actual procedure and encoding. Do not delete it as an algorithm duplicate
before the original replacement can preserve that boundary.

Default-group verification source now includes wow_default_original_fixture,
linking actual clmsg.c, rtl/wow.c and chartran.c. Its explicit graph/dispatch
doubles exercise A/W stored text/truncation, length, HELP parent forwarding,
move/size forwarding and original pre-4.0 minimized geometry, color branch,
NC service routing, F10 native delegation and neutral default returns.
Message-table inputs are doubles, not proof of full production classification.
The existing wrapper fixture remains separate. Both graphs and this new case
are staged, unbuilt/unrun; no per-branch compile was started. Complete the
remaining get/set dependency group before unified verification.

Source continuation audit refines that scheduling: get/set cannot be selected
until CPD/default-style dependencies are resolved; it need not block verifying
the now complete default-window cohort. Review of every clmsg default branch
finds WM_WINDOWPOSCHANGED sends WM_MOVE and then WM_SIZE. A move callback can
retire native HWND while the borrowed copied WND survives. Register a minimal
OPENNT-HOST-089 destroyed/cleaned-state check before size dispatch; retain
all successful original ordering and algorithms. Native resource lifetime
cannot be made equivalent to original kernel object locks by storage pinning.
The original-body fixture now destroys on move and asserts no subsequent size
dispatch, alongside the complete prepared positive group. This is not an
inherited NT4 defect claim. No new private native layout or guest mutation.
Run the unified default/control boundary group once after this source audit;
get/set remains explicitly unselected, not represented by fake providers.

Default cohort verification, 2026-10-17 UTC: r22 runner37668 started
01:32:28.6166838Z; Ninja28436 at01:32:30.7628597Z, exit2 after61.0555s.
Compile exposed duplicate SHELLHOOKINFO (already supplied by the modern SDK),
missing opt-in original HW/HWq and WFMAXIMIZED. Removed duplicate declaration,
selected existing original handle macros and recovered exact user.h1791 flag
0x0F01 in the facade. No algorithm replacement. r23 runner37036 at
01:33:52.7863258Z; Ninja5116 at01:33:53.9927245Z, exit1120 after9.0049652s:
test linked view_wow from an older test-only metrics facade and lacked its
two injected globals. Corrected the fixture link to a distinct rtl/wow object
compiled with the production control-client facade, not invented globals.

r24 runner27176 at01:34:24.7552752Z; Ninja5840 at01:34:26.2928436Z,
build exit0 after9.0233373s. All23 boundary fixtures and runner exit0;
original-default reports failures0. Raw input hashes/build/test logs retained
in build/M0-T447/S3/original-bodies-r22 through r24. Prior failed runs did not
execute the fixtures and are not passes. These results cover the selected
original default body, real original rectangle/conversion helpers and named
dispatch doubles, not actual native HWND lifecycle or guest behavior.
Full WOW32 link, CPD/getset service closure, other fixed S3 gates and coherent
publication remain open. No guest change, release, commit or push occurred.

### CPD window substitution research (distinct from rejected class registration)

Pinned kernel/getset.c::xxxSetWindowData, GWL_WNDPROC branch, explicitly
normalizes both pfnClientPrevious and wType before updating WFANSIPROC.
Unlike class registration, it therefore has the source contract required by
the dialog client setter. This source-led candidate uses only public APIs on
owned message-only windows: create a known scratch window, set its procedure
to a native CPD, read A/W procedures and IsWindowUnicode, then restore its
known W procedure before destruction. It is not a native layout decoder or
production selection; the old rejected class probe remains unchanged.

Research fixture wow_cpd_window_normalization_probe tests both ANSI/Unicode
source procedures with both A/W setters (4 combinations), destroys the source
window while retaining its class before normalization, and verifies target
address, opposite-view token, actual encoding, zero source-procedure callbacks
during normalization, and destruction/unregistration cleanup. It runs only
under explicit -CpdWindowResearch, outside normal supplemental acceptance.
This selector does not claim omitted full-group gates passed.

Background cpd-window-r1: runner23464 started2026-10-17T01:37:12.4622864Z;
Ninja9764 at01:37:13.6957987Z, build0 in8.6723219s; probe31768 at
01:37:22.3891512Z, exit0 in0.3009332s; runner0. All4 cases report the correct
raw address/encoding, callbacks0 and failures0. Raw logs and source hashes
remain in build/M0-T447/S3/cpd-window-r1. This overturns only the inference
that no public address-plus-encoding route has been demonstrated; it does not
overturn the failed class-registration probe or certify a production resolver.

Before selection, audit invalid/stale tokens, native failure and restoration,
class lifetime, same-thread nested calls, and system/application hook effects
of scratch-window creation and temporary subclassing. No arbitrary procedure
may execute merely to decode it. A production binding must retain existing
task/native-call/borrow ownership and not introduce a second persistent CPD
registry. Per-call scratch windows are research mechanics, not an approved
runtime design or a claim of unchanged observable hook traffic. No product,
guest, publication or release was changed by this research.

CPD safety refinement: source inspection of dlgmgr.c::DefDlgProcWorker proves
lpfnDlg is invoked directly, so merely preserving an opaque token there would
not be equivalent. DLGF_ANSI also feeds WM_NOTIFYFORMAT; guessing the flag
from the setter encoding is incorrect. Conversely, the first scratch-window
candidate temporarily installs arbitrary target code as GWLP_WNDPROC. Its
zero-callback observation cannot prove safety against incoming messages.
Do not adopt that candidate directly as production.

The client/ntstubs.c::_SetWindowLong DWL_DLGPROC branch has the required
address/type normalization without replacing the window's message receiver.
Extended research creates an owned #32770 message-only window, replaces its
GWLP_WNDPROC with a known safe native procedure while its separate dialog
slot is occupied, and uses A/W DWLP_DLGPROC queries for address/encoding.
Ordinary WM_NULL is explicitly sent while the slot contains the target;
the target must not execute. Clear the slot before restoring the original
window procedure and destroying the window, so normal dialog cleanup occurs.
IsWindowUnicode describes the safe W window procedure, not the slot encoding.

cpd-window-r2 hidden runner13948 began2026-10-17T01:39:47.8296856Z;
Ninja36584 at01:39:48.9595657Z, exit0 in8.6135022s; probe33492 at
01:39:57.5919287Z, exit0 in0.3788199s; runner0. Four old window-procedure
cases and four dialog-slot cases pass. In all four slot cases A/W queries
recover the source address and encoding, the receiver remains Unicode,
explicit message dispatch causes zero target callbacks, and cleanup passes.
Source windows are already destroyed, but their classes remain retained.
Logs/input hashes: build/M0-T447/S3/cpd-window-r2. This supports the safer
slot mechanism on the tested host; it does not prove hostile hooks, invalid
or stale CPDs, allocation failures, nested resolver calls or product isolation.

Finite candidate design for continued audit: keep native token authority in
USER, return only a call-local address/encoding result, and use no CPD cache
or persistent second identity registry. A scoped native-call boundary must
own any helper HWND, restore its safe procedure/empty slot before releasing
it, and preserve result/error through cleanup. Reject ambiguous A/W views
instead of accepting an opaque value as executable code. Existing task/borrow
owners must surround host calls; registration/creation hook effects and
reentrant helper loss require explicit handling before integration. This is
a design candidate, not a selected resolver or a relaxed semantic contract.
Original class/dialog consumers remain incomplete; no product or guest change.

cpd-window-r3 completes the prepared token-failure group in the research
fixture. Its call-local decode_slot first requires the CPD tag, installs the
token only in the guarded dialog slot, requires exactly one nonzero raw A/W
view and one opaque view, and clears the slot in finally before committing
address/encoding output. Ambiguous or invalid views return failure, not an
executable opaque address. This function is test research, not product code.

For each of the4 dialog combinations, tests now reject FFFF0000 with output
unchanged and slot empty, then successfully decode a valid token while
preserving LastError=0447. After unregistering the source class (source window
already destroyed), the same old token must fail, leave output unchanged and
clear the slot. Explicit WM_NULL while the slot is occupied still executes no
target callback. Source/class/scratch cleanup assertions remain mandatory.

Hidden runner35884 started2026-10-17T01:41:57.9739761Z; Ninja14220 at
01:41:59.2210155Z, exit0 after9.1489561s; probe31008 at01:42:08.3872446Z,
exit0 after0.3692397s; runner0. Eight positive combinations and all added
negative/recovery assertions pass. Logs and input hashes remain under
build/M0-T447/S3/cpd-window-r3. No invalid token is dispatched as code.

This establishes the token/slot transaction on the tested host, not the
remaining helper-window lifecycle contract. Creation veto, destruction during
hooks, nested normalization, helper identity after destruction/reuse, cleanup
failure and existing WOW native-call ownership remain finite integration
requirements. No host-version restriction, persistent CPD cache, application
procedure invocation for detection or new runtime helper is approved by this
result. Full product/guest checks and existing style-notification disposition
remain open. No source/runtime publication or commit/push occurred.

CPD helper observability gate: research r4 installs only a test-process
thread-local WH_CBT observer, keeps all prior positive/invalid/stale/recovery
assertions and counts the owned scratch helper's HCBT_CREATEWND and
HCBT_DESTROYWND. Hidden runner33212 began2026-10-17T01:46:16.6928840Z;
Ninja38860 at01:46:18.2775958Z exited0 in10.3656667s; probe9520 at
01:46:28.6645509Z exited0 in0.4016875s; runner0. Exact output records
helper_cbt_creates=8 helper_cbt_destroys=8 and failures0. Logs/source hashes:
build/M0-T447/S3/cpd-window-r4. The observer was removed successfully.

Conclusion: neither hidden nor HWND_MESSAGE means invisible to existing
application CBT hooks. The candidate adds observable create/destroy events
that original private CPD lookup did not require. The owner has not approved
that difference. Do not select the candidate as transparent, suppress hooks,
or treat prior permission for narrow host color/default behavior as this
waiver. A product helper would additionally need create-veto, hook-reentry,
early-destruction and rollback handling; positive decoding does not prove
those obligations. Request the bounded compatibility disposition before
production implementation. Without acceptance, continue seeking a no-helper
binding; the tested helper remains research only. No product/guest changes,
publication, commit or push result from this gate. The broader S3 scope is
unchanged and is not declared blocked or complete.

### Original EndDialog body linkage audit

EndDialog lifetime repair admission within OPENNT-HOST-080: the selected
ValidateDialogPwnd callback, owner EnableWindow, focus transfer, hide and
owner activation are the complete enabled callout sequence for this body.
wow_window_words_terminal marks the retained allocation destroyed, and
original DefDlgProcWorker/SetDialogPointer release and clear PDLG. Continuing
after those operations can dereference NULL or send to a still-valid/reused
numeric HWND. This is a rehost lifetime boundary, not evidence of a reproduced
NT4 defect. The original client ThreadLock macros do not solve it.

Five two-line exact-WND checks are registered before implementation: validation
and owner-enable retirement return FALSE before fEnd/result is written;
focus/hide/activation retirement returns TRUE after the end request committed.
Normal order and native failure handling stay unchanged. No revalidation of a
replacement HWND, delayed PDLG cache, reordered owner-enable or new exception
handler is added to production. SYSMODALWINDOWS is not selected. The fixture
now exercises all five callouts with destroyed-only, cleaned-only and combined
terminal state while keeping the numeric HWND valid, plus its ten prior
normal/invalid cases. The caught-AV assertion exists only in the test.
This models exact-state retirement, not real Windows handle reuse or the
full modal loop. The larger creation/default/final-cleanup audit remains open;
these checks cannot be cited as closure of the whole dialog lifetime group.

Grouped verification r28: runner8824 started2026-10-17T02:04:36.8658737Z;
Ninja29308 at02:04:38.5296586Z exited0 after11.217463s; all24 supplemental
fixtures and runner exited0. Actual EndDialog body reports cases=25 failures=0.
Logs/input hashes: build/M0-T447/S3/original-bodies-r28. Mirror executable
delta is10 added lines (five conditions and returns), with local080 markers.
No guest change or new source family. Independent review retains unproved
owner-window retirement, nested EndDialog, full native/guest lifetime and
the creation/modal/default/cleanup chains as explicit S3 obligations; the
test's live HWND is only a deterministic replacement-identity double. No
publication/commit/push and no whole-S3 acceptance follows from this pass.

Source-led continuation found a concrete composition defect before product
link: control-client/precomp.h inherited EndDialog=wow_user_end_dialog from
wow_user_private_access.h. The r24 original_dlgend.obj symbol table defines
_wow_user_end_dialog@8, while wow_dialog_finalization_binding.c calls
wow_original_EndDialog. Thus the staged product would have a duplicate adapter
definition and a missing original-body definition. The older finalization
fixture supplies an explicit body double and cannot detect this mismatch.

Correction stays at the existing adapter/build boundary: only dlgend.c is
compiled with WOW_ORIGINAL_DIALOG_END, whose precomp override names its body
wow_original_EndDialog. Other original call sites retain the scoped adapter.
Both production and supplemental generators select the same flag. No original
body, guest image or wire ABI changes; incremental mirror-diff budget is zero.
This completes the already registered ADAPTER-WOW-063/OPENNT-HOST-080 name
binding rather than introducing a second dialog termination implementation.

The new wow_dialog_end_original_fixture links that actual object, using
explicit validation/mutation doubles and real hidden host HWNDs for IsChild
and the posted wake message. Eight owner/active/disabled combinations verify
result publication, operation order and exactly one wake; invalid-window and
non-dialog cases verify no mutation or wake. It does not prove a real modal
loop, WOW execution ownership or callback-driven destruction. The latter
remains a source-audit obligation: enabling the owner/focus/hide/activation
can reenter, and retained WND storage alone does not pin native HWND or PDLG.
Do not claim an original NT4 defect or implement speculative guards merely
from this observation.

Background r25 runner24540 started2026-10-17T01:52:31.3249602Z;
Ninja35100 at01:52:33.4868226Z failed1120 after64.9017771s. The new
fixture accidentally inherited adapter macros for its own PeekMessageW and
DestroyWindow setup/observation; no fixtures ran. Undefining those only in
the test restores native harness ownership, without changing product calls.
The retry r26 runner38688 started2026-10-17T01:53:53.7369759Z and exited0.
All24 supplemental fixtures pass. Actual original-EndDialog test36004 at
01:54:07.5754446Z exited0 after0.2807528s, reporting cases=10 failures=0.
Logs and input hashes are in build/M0-T447/S3/original-bodies-r25 and r26.
Post-build symbol inspection confirms _wow_original_EndDialog@8 is defined
instead of _wow_user_end_dialog@8. Full product link, callback-destruction
acceptance and guest regression remain open; no publication/commit/push.

### Dialog entry-name and reentrant-state group audit

Read-only COFF inspection of every original_*.obj from the supplemental
cohort extends the EndDialog check. The second concrete collision is
dlgmgr.c::DefDlgProcA: inherited public-call macros made its definition
wow_native_DefDlgProcA, already implemented by wow_user_task_lifecycle.c.
The native adapter first selects recovered state and otherwise invokes
native USER; it is not the original body. DefDlgProcW also retained an SDK
name rather than an explicit private original name. Source search finds no
other references to these A/W wrappers in the selected original dialog group.

Only dlgmgr.c now selects WOW_ORIGINAL_DIALOG_DEFAULT, mapping both bodies
to wow_original_DefDlgProcA/W. Scoped recovered dispatch still calls original
DefDlgProcWorker, while the native adapter retains untouched-dialog ownership.
No original algorithm is cropped or changed. The keyboard-lifetime fixture
adds a rejecting native-adapter symbol beside the real dlgmgr object: a
recurrence of the alias collision fails linking rather than silently passing.
The two dlgmgr2 objects in the supplemental graph intentionally share helper
definitions but are linked into different fixtures; production selects one
keyboard-enabled object. Compiler-folded string constants are not duplicate
behavior providers. Complete product symbol/link closure remains required.

The lifecycle review covers the whole original dialog group, not merely
EndDialog. Current terminal ownership marks WFDESTROYED before finalization;
original DefDlgProcWorker cleanup frees PDLG, then original SetDialogPointer
clears its pointer and sets FNID_CLEANEDUP_BIT. Borrow retention preserves
WND storage, not the freed PDLG or actual HWND. Original client/usercli.h's
ThreadLock macros are no-ops; do not attribute their behavior to a kernel
lock or propose implementing a second kernel lock system. Source observations
below identify unproved continuations, not an established runtime crash or
proof that original NT4 exhibited the same failure.

| Group | Actual callout and continuation | Required finite proof/remediation boundary |
| --- | --- | --- |
| Validation | ValidateDialogPwnd can call pfnWowSetFakeDialogClass after publishing PDLG; callers then use PDLG. | Destroy/reenter during that callback; distinguish failed validation from successful allocation subsequently destroyed; no duplicate allocation or resurrection. |
| Creation | InternalCreateDialog calls WOWDlgInit, WM_SETFONT, child creators, GetEditDS, WM_GETDLGCODE and WM_INITDIALOG before subsequent PDLG/state accesses. | Track root and child exact identities, resource transfer and rollback as one group; preserve SCDLG_NOREVALIDATE and the explicit pre4.0 destroyed-on-init return exceptions. Never substitute a reused HWND. |
| Modal loop | DialogBox2 sends WM_CANCELMODE before setting fDisabled and can show/update/dispatch before revalidating. | Cancel/show/dispatch destruction and same-value handle reuse; no dereference after cleanup, no dispatch to replacement, preserve result/WM_QUIT/owner-restoration ordering. |
| Default procedure | Direct dialog callback already checks FNID_STATUS_BITS; DM_SETDEFID writes PDLG after xxxCheckDefPushButton callbacks and WM_SETFOCUS has nested focus callbacks. | Preserve existing valid guards; cover nested destruction at later callouts and stop stale continuation without bypassing the original default-button algorithm. |
| End | EnableWindow precedes result/fEnd writes; focus, hide and owner activation precede later HWND operations/wake. | Destruction at each callback, nested EndDialog and failed native operations; specify return and wake/owner behavior from original contracts before inserting guards. |
| Final cleanup | ReleaseEditDS crosses the registered WOW memory-service boundary before hData is cleared; application reentry from that service is not yet established. Font/lookaside release and pointer clearing follow. | Trace the concrete registered target before requiring new reentry guards. Verify exactly-once release, no fresh allocation during teardown and execution/error unwind. No delayed-state cache is admitted by this audit. |

This is one finite S3 dialog-lifetime verification/remediation group. It does
not expand to full S4 message semantics or move a basic lifecycle defect to
a later task. Existing entry/finalization doubles do not prove these body
continuations. Actual body tests must include callbacks and handle-reuse
negatives, then the production/guest gate; no per-leaf build schedule is
introduced. Name-binding corrections add zero mirror-body diff. Lifecycle
changes, if needed, must first register exact source-local minimal diffs and
retain original normal behavior and compatibility returns.

Pinned original client/wow.c lines154--189 confirm BOTH ValidateHwnd and
ValidateHwndNoRip return CallbackWnd.pwnd on the callback fast path. Original
usercli.h maps RevalidateHwnd to the latter. Therefore changing NoRip to
unconditionally reject retired callback state would not simply restore an
omitted original distinction. Current wow_user_borrow_window_state deliberately
retains this same callback identity; liveness checks belong at the proven
continuation boundary, not a global change to callback identity semantics.

Name-binding group verification r27: hidden runner39404 started
2026-10-17T01:57:08.8843190Z; Ninja23556 at01:57:10.6330810Z exited0
after64.7039284s; all24 fixtures and runner exited0. Inputs/logs remain under
build/M0-T447/S3/original-bodies-r27. COFF inspection now finds distinct
wow_original_DefDlgProcA/W and DefDlgProcWorker definitions; the rejecting
native-adapter link witness coexists. This proves the corrected body binding,
not complete native/guest dialog lifetime. No guest/mirror body change,
publication, commit or push occurred.

### Production get/set dependency closure checkpoint

Following r27, inspect New-T404S5Wow32ProviderNinja.ps1's complete original
control selection and providerSupportSources against the original-body COFF
references and source owners, without another compile/test run. COFF section
indices are hexadecimal (SECTA etc.), and nonzero COMMON records are data
definitions; ignoring either creates false missing-provider reports. The
supplemental keyboard and non-keyboard dlgmgr2 objects are distinct test
link closures, not two production owners.

| Reached family | Existing selected production provider | Disposition |
| --- | --- | --- |
| EDIT/combo/list cross-unit algorithms | Seven EDIT and ten combo/list client bodies in the control-body loop | Existing original definitions account for ECDeleteText, MLEditWndProc, xxxCBPosition, xxxLBCreate and related references; do not invent providers from an incomplete object scan. |
| Character search/copy/conversion | rtl/random.c, rtl/chartran.c and the existing ntuser_rtl_compat service | Selected support inputs; no missing TextCopy/FindCharPosition replacement is needed. |
| Relationships/coordinates/visibility | control-view-winmgr/wow and control-body-winmgrc | Selected original providers; lifetime/publication still requires independent verification. |
| Drawing/tab/mnemonic/extent | control-draw, control-text-client/rtl, control-text-extent and existing drawing/resource bindings | Selected original algorithms; do not infer runtime correctness solely from selection. |
| Pools/state/EDIT DS | control-lookaside, control-state, control-fnid, dialog-pointer, dialog-memory and original COMMON pool definitions | Existing owners; no new allocator or separate control data registry needed. |
| Window extra-data getter | rtl/getset.c::_GetWindowLong | Source retained but absent from production graph; called by wow_window_client_try_get_long. |
| Window extra-data client setter | client/ntstubs.c::_SetWindowLong with WOW_ORIGINAL_DIALOG_GETSET | Source retained but absent from production graph; called by wow_window_client_try_set_long. |
| Private control/dialog setter | kernel/getset.c::xxxSetWindowLong | Source retained but absent from production graph; called by wow_window_client_set_window_long, including initial private-state publication. |

These last three form a single still-open production integration group,
not three independently deliverable helper changes. Their finite boundary
is TestWindowProcess/FCallerOk exact local provenance, CBFNID extents,
GetCPD generation, incoming CPD address-and-encoding normalization,
GetWindowData/xxxSetWindowData negative-index routing, and matching private
precomp. The existing dialog-getset test header supplies explicit doubles
for these operations; it is not a production provider or acceptance proof.

Source trace: wow_user_get/set_window_longA enters try_get/set_long first;
nonnegative recovered indices remain original-owned even on zero/error.
The client setter additionally retains the initialized ListBox ownerdraw/
string-style guard. The server setter owns initial control-pointer admission,
destruction-time replacement, result/user data and bounds checks. Native
fallback must not be used to hide an absent original provider. Negative
procedure/WOW-word routing already has distinct existing owners; forwarding
GetWindowData back to the public adapter would need explicit cycle analysis,
not an unreviewed generic alias.

No production get/set selection is claimed until both unresolved semantic
edges have disposition: incoming CPD normalization (the tested helper emits
CBT events) and pre4.0 style-notification timing. Neither broad narrow-host-
default approval nor an automatic goal continuation approves these different
effects. A self-contained CPD disposition question has been presented again;
no answer has yet been received. Other fixed S3 lifecycle and full production
verification obligations remain active. This audit changes no guest, mirror
body or published artifact and does not repeat the r27 test run.

### Registered dialog callbacks: distinguish service calls from application reentry

Retry source audit follows the actual registration in mvdm/wow32/wow32.c,
not callback names alone. The existing wow_user_registered_callbacks table
remains the sole registration owner; no additional callback registry is needed.

| Entry | Registered implementation and observed operations | Conclusion |
| --- | --- | --- |
| WOWDlgInit | W32InitDlg in wcall32.c reads GWL_WOWWORDS, writes class/dialog-procedure words and returns the original init parameter. | No direct application callback in this body. Keep the existing initializer; its word-access boundary still requires production validation. |
| ValidateDialogPwnd fake-class notification | SetFakeDialogClass in wumsg.c updates existing WOW words and reads/replaces DWL_DLGPROC. | No direct application callback; the unresolved procedure-token normalization boundary can introduce reentry if the helper design is selected. Do not declare that proposed helper transparent. |
| GetEditDS | W32GlobalAlloc16 enters WOWGlobalAllocLock16, then the immutable WOW16GlobalAllocLock handler calls GLOBALALLOC and GLOBALLOCK. | This is a real guest transition, not by itself proof of an application window callback. Allocation/compaction notification dependencies remain distinct from release. |
| ReleaseEditDS | W32GlobalFree16 enters WOWGlobalUnlockFree16. WOW16GlobalUnlockFree calls GLOBALHANDLE, GLOBALUNLOCK and, only at zero lock count, GLOBALFREE. | Trace the memory-service path rather than inject a hypothetical window-destruction callback and call it an original regression. |

Read-only continuation into kernel31/3ginterf.asm and 3gmem.asm finds the
ordinary GlobalFree path uses gfree/free_object; the inspected free_object
body does not call gnotify. The observed gnotify call in grealloc is its
discard branch, not this free path. GlobalFree also has cursor/icon and DDE
special branches, so this observation is not a universal nonreentrancy proof
for every global-memory handle or every helper below free_object. The dialog
EDIT segment originates from GetEditDS's GHND | GMEM_SHARE allocation; its
complete lifetime and the selected runtime profile still need group evidence.

The earlier lifecycle table's categorical statement that ReleaseEditDS can
reenter has therefore been corrected to an unproved service boundary. No
additional mirror guard, early handle clearing, second cleanup-state owner or
guest change is justified by this trace alone. Real message callouts during
creation, modal processing, focus/default-button handling and EndDialog remain
in the finite lifecycle group. Existing r28 supplemental results are unchanged;
this source audit adds no new runtime pass, publication or delivery claim.

### Default-dialog focus/default-button continuation group

Source trace establishes a concrete caller gap, distinct from the speculative
memory-service reentry above: xxxCheckDefPushButton synchronously sends
WM_GETDLGCODE, DM_GETDEFID and BM_SETSTYLE. Its existing retirement guards
return void; they cannot prevent DefDlgProcWorker's DM_SETDEFID caller from
subsequently writing PDLG(parent)->result after cleanup freed that allocation.
xxxRestoreDlgFocus likewise invokes the helper, then SetFocus, then clears
PDLG->hwndFocusSave. Both operations can send application messages. The
WM_SETFOCUS caller can navigate the old graph when restoration returns FALSE;
WM_NEXTDLGCTL can run default-button processing after DlgSetFocus destroys
the retained root or focus child.

OPENNT-HOST-119 now registers this contiguous caller group in dlgmgr.c.
Use existing exact borrowed WND terminal flags, not a fresh numeric HWND
lookup after callbacks. Restore retains its saved target before the helper;
when only that target retires, skip SetFocus but still clear the surviving
dialog's saved focus. When the root retires, do not touch freed PDLG.
DM_SETDEFID retains its original TRUE return and unlock sequence while
skipping a destroyed parent's result write. WM_NEXTDLGCTL retains its unlock
sequence and TRUE return while skipping stale default-button work.
Normal message order and original focus/button algorithms are unchanged.

The existing actual-body keyboard lifetime fixture now uses a complete
DIALOG/DLG backing allocation and clears PDLG on injected root retirement.
Added cases exercise normal DM_SETDEFID, destruction at its query/style
callouts, and root/target destruction during WM_NEXTDLGCTL focus. These
assertions are staged and unrun. Restore-focus success/failure, real HWND
reuse, modal/creation lifecycle and full guest acceptance remain part of
the same finite group; this is not a passing group or a product delivery.
No guest input is modified. No per-leaf build/test is launched for this edit.

The caller group is now verified together, not as separate per-guard runs.
The same fixture adds RestoreDlgFocus normal completion, saved-target-only
retirement, root retirement in WM_GETDLGCODE, root retirement in SetFocus,
and WM_SETFOCUS's caller continuation after failed restoration. A fixture-owned
hidden native STATIC HWND satisfies the original IsWindow check; all focus
and message side effects remain declared doubles. It is destroyed explicitly
at fixture exit, not offered as product-retirement evidence.

Grouped run r29: runner26432 started2026-10-17T02:14:55.3881154Z;
Ninja4284 started02:14:57.2308949Z, exited0 after11.2553118s. The new
default-procedure cases reached the rejecting wow_edit_control_extent double:
fixture setup had allocated DIALOG/PDLG but omitted FNID_DIALOG and the
documented extra-byte count. The original VALIDATECLASSANDSIZE therefore
correctly attempted first initialization. Runner exited1; no pass is claimed.
Correct the fixture's initialized-state inputs, retaining the rejecting
initialization double instead of replacing it with fabricated success.

Grouped run r30: runner27964 started2026-10-17T02:15:52.3267417Z;
Ninja28724 started02:15:53.9433746Z, exited0 after11.3551713s. All24
supplemental fixtures and the runner exited0. Evidence/input hashes remain
under build/M0-T447/S3/original-bodies-r29 and original-bodies-r30;
the shared incremental root remains call-boundaries-x86. No guest rebuild,
product publication, commit or push occurred.

Modal source audit remains separate: DialogBox2 sends WM_CANCELMODE before
writing fDisabled; PeekMessage may deliver sent messages before its return;
ShowWindow precedes UpdateWindow; a dispatched timer can lead directly to
ShowIt. Its existing numeric revalidation alone does not prove that the exact
borrowed callback state is live. Before modifying these paths, retain the
original destroyed-dialog return and determine owner re-enable/activation
ownership; do not insert generic early returns that strand a disabled owner.
The focus/default tests above do not cover those modal continuations.

### Modal-loop continuation group and owner disposition

Read-only trace of the pinned original client/dlgmgr.c and dlgend.c places
normal owner enabling in EndDialog, and create-failure restoration in
InternalDialogBox / DialogBox2's failed-validation branch. DialogBox2's
post-dispatch destroyed-dialog return0 does not itself enable the owner.
The inspected kernel/createw.c does not implement the client DLG fDisabled
policy. Therefore the new terminal checks retain the original abnormal
return0 rather than introducing a second owner-restoration policy. This is
not a claim that destroying a modal dialog without EndDialog is equivalent
to normal dismissal.

OPENNT-HOST-080 adds four checks and strengthens the two existing
revalidation exits in DialogBox2: after cancellation before the fDisabled
write, at the idle/show label, after ShowWindow before UpdateWindow, and
before processing a retrieved non-quit message. The ShowIt label also guards
the timer-dispatch jump. WM_QUIT retains its original repost-before-exit
order. Existing retained terminal flags identify the old generation even
when numeric validation still returns its borrowed WND. No new cleanup or
message engine, guest changes, or guessed host-private state is introduced.

The retained wow_dialog_lifetime_fixture is now wired into the common
supplemental graph. It links the actual modal manager, EndDialog and
SetDialogPointer bodies against an explicit test-only WND/service facade;
the production-layout manager object is still compiled in the same run.
Seven retirement edges (cancel, empty peek, message peek, show, update,
timer dispatch, quit peek) each exercise destroyed-only, cleaned-only and
combined flags. Numeric validation intentionally continues to succeed while
PDLG is cleared. Assertions reject further update/dispatch/destroy, require
the original owner-enable behavior, and require quit repost37. Existing
normal end, disabled-owner, create-failure, invalid-owner, focus and pointer
transition assertions remain. These are not real handle-reuse tests.

Background r31 runner33768 started2026-10-17T02:19:06.3980007Z;
all25 supplemental fixtures and the runner exited0. Build/run input hashes
and PID/timing logs are in build/M0-T447/S3/original-bodies-r31. No production
publication, commit or push occurred. Diff whitespace check passes.

The compiler also identifies the original InternalDialogBox no-owner path's
uninitialized fDisabled local. This belongs to the already admitted creation/
failure group, not a new task or justification for suppressing the warning.
Creation success/failure/initialization callback lifetime, owner destruction/
reuse, complete product get/set closure and real guest acceptance remain open.

### Creator resource and compatibility-return audit

Source read: complete dlgbegin.c lines470--958, existing dialog creation
wrapper, and the original registered WOW memory service. Before root creation,
the creator owns hMenu and hNewFont and uses DeleteFontAndMenuAndFail. After
root validation, it initializes the original PDLG; the CD_USERFONT assignment
to hUserFont transfers that font's cleanup to the original dialog finalizer.
WM_SETFONT, child creation, child WM_SETFONT/WM_GETDLGCODE, WM_INITDIALOG,
focus/default-button work, foreground and show/update are the concrete message
boundaries. Adding a generic jump to the pre-transfer font/menu cleanup after
those boundaries would risk releasing resources a second time.

Two original return rules must remain separate: the post-initialization
IsWindow failure returns the old HWND for GetAppVer<VER40 (Omnis7); the final
CreateDialogReturn revalidation is bypassed by SCDLG_NOREVALIDATE (Gupta/WOW).
Therefore neither a universal NULL-on-retirement helper nor unconditional
strengthening of every RevalidateHwnd is an acceptable creator repair.
The existing pre/post-init return labels and transfer point must determine
each guard's destination. No creator guard has been added on speculation.

The compiler's no-owner defect is source-proven independently: in
InternalDialogBox, fDisabled is assigned only inside the non-null owner
branch, then read in the failure condition and passed to DialogBox2 on
success. OPENNT-HOST-080 registers one FALSE initializer. Existing owned
window behavior is unchanged; no-owner means no previously disabled owner.
The original-body lifecycle fixture adds no-owner creation failure and
end-during-creation success, checking no owner mutation and initialized
fDisabled. These new assertions and the one-line correction are staged,
not covered by r31; verification will accompany the creation group.

Remaining creator work is finite: resource ownership at pre/post-transfer
failure, exact root/child retirement at the listed message calls, both legacy
return rules, and original cleanup ordering. GetEditDS's guest transition
still requires tracing its allocation/compaction callbacks before treating
it as arbitrary application reentry; gnotify's name alone does not establish
that contract. Guest source is read-only evidence throughout.

### Allocation callback established; preserve the EDIT segment handoff

The follow-up trace now establishes the concrete application call, rather
than inferring it from a function name. kernel31/3gmem.asm galloc calls
gsearch; 3galloc.asm's do_compact path calls gcompact; 3gcompac.asm permits
discard when the heap/flags allow it and calls gdiscard/gnotify. In
3gmoreme.asm's GN_DISCARD/GAH_NOTIFY path, gnotify finds the owning TDB,
loads TDB_GNotifyProc, switches the task PDB context, and executes an indirect
far call to that procedure. 3ginterf.asm IGlobalNotify installs the
application-supplied procedure in that TDB field. GetEditDS's GHND |
GMEM_SHARE allocation does not itself request GA_NODISCARD/GA_NOCOMPACT.
This is conditional reachability under memory pressure, not a claim that
every allocation invokes application code or that a runtime crash was seen.

OPENNT-HOST-081 stages the necessary resource handoff correction: retain
GetEditDS's return in a local HANDLE before touching PDLG. If the exact
retained root has terminal flags, release the newly allocated segment (if
any) and use the existing CreateDialogReturn path. Otherwise publish hData
and retain the original allocation-failure/NoCreate handling. Original
cleanup cannot have owned the new segment before publication, so this
prevents both writing freed PDLG and leaking the successful late allocation.
No independent state registry, guest mutation or release-path reentry claim
is introduced. Complete creator return/lifetime cases remain unrun; r31
does not cover this edit or the no-owner initialization.

The two independent pending compatibility decisions (CPD helper CBT events
and pre4.0 style-setter notifications) were presented together in a
self-contained owner question. No approval is inferred from automatic goal
continuation or from the narrower default-color approval. Independent
source-proven lifecycle work continues within the existing S3 scope.

### Creator message-continuation group staged

OPENNT-HOST-081 now covers the remaining message continuation points in
dlgbegin.c as one group. After root WM_SETFONT and child creation, terminal
root state goes to CreateDialogReturn, not DeleteFontAndMenuAndFail. The
child is retained before notifications; child WM_SETFONT/WM_GETDLGCODE
retirement goes to the original NoCreate branch (including DFNOFAILCREATE),
while root retirement stops without reading freed PDLG. Query result is
held locally until the original result owner is confirmed live.

After WM_INITDIALOG, the existing failed-revalidation exit remains; a
still-borrowed terminal root reaches the original post-init compatibility
branch through CheckDialogAfterInit. That branch still returns the old
handle for pre4.0 callers and NULL otherwise. Focus callbacks use the same
branch for root retirement; a dead new focus child is not passed to another
default-button notification. Foreground retirement retains its existing
NULL-return policy. Show retirement skips UpdateWindow. Final validation
checks exact terminal state only when SCDLG_NOREVALIDATE is absent.

No new registry, cleanup routine, wrapper policy or application-version
heuristic is added. The existing PDLG/resource transfer and return branches
remain authoritative. Production source guards alone do not establish
runtime correctness: the actual creator-body fixture must cover the matrix
below before this group can be called verified.

| Input group | Required assertions |
| --- | --- |
| Normal, font and shared-EDIT variants | Original notification order, one resource transfer, returned root, final result/default control identity. |
| Root retirement at font, child-create, child-font, child-code, allocation | No subsequent child/message/state work; no pre-transfer cleanup after transfer; late EDIT allocation released exactly once. |
| Child-only retirement at font/code | Existing NoCreate failure versus DFNOFAILCREATE continuation; no notification to the retired child. |
| Init/focus retirement | Original pre4.0 branch and modern NULL behavior, with numeric IsWindow still true to distinguish exact identity. |
| Final validation | SCDLG_NOREVALIDATE preserves its explicit old-handle contract; ordinary callers reject the retired identity. |
| Foreground/show/update | Existing foreground failure return, no UpdateWindow after show retirement, no stale return without the explicit exemption. |
| No-owner modal wrapper | Initialized fDisabled, no owner mutations, original creation-failure/success values. |

All creator changes remain unbuilt/unrun; r31 proves only its recorded input
set. Diff whitespace check passes. Guest, release assets and the deployed
package are unchanged. Full production linking and real HWND/guest evidence
remain required beyond this supplemental matrix.

### Actual creator-body verification r32--r34

wow_dialog_creator_lifetime_fixture now links production-layout
original_dlgbegin.obj and the already retained original rtl/random.c string
provider. It does not copy the creator algorithm. Authored standard templates
exercise native GDI font creation with explicit HWND creation/message/cleanup
doubles; hidden real STATIC HWNDs remain valid during simulated retirement
to reject reliance on IsWindow alone. These HWNDs are fixture-owned and
explicitly destroyed after the cases, not evidence of product cleanup.

The fixture passes49 combinations: one normal shared-EDIT/font creation,
eleven root callback-retirement edges times two application versions times
two final-revalidation modes, and two child-only retirement edges times
DFNOFAILCREATE off/on. Assertions cover result handle, no subsequent native
work on terminal identities, one shared-segment release, no duplicate root
destruction, and preserved post-init/final return rules. The existing modal
fixture also passes the newly added no-owner creation success/failure cases.
No claim is made for real GlobalNotify scheduling, real handle reuse,
pre-transfer font/menu allocation failure, or complete product creation.

Background attempts, all terminal and retained:

- r32 runner28248 started2026-10-17T02:27:39.2254096Z, exited2. Fixture
  compilation failed because APFNWOWHANDLERSIN is a pointer typedef and
  pfnGetExpWinVer is a facade macro. Correct the fixture record type and
  undefine that macro only in the test; match declared LONG/BOOL signatures.
- r33 runner31288 started2026-10-17T02:28:32.4729338Z, exited1120. The fixture
  link omitted wcsncpycch. Source tracing located its existing original
  rtl/random.c provider; add obj/string_owner.obj, not an invented string copy.
- r34 runner31680 started2026-10-17T02:29:09.3727667Z, exited0. Ninja27540
  started02:29:11.5098643Z, exited0 after7.3181161s; all26 fixtures pass.
  Creator fixture37824 exited0 after0.3733957s with failures=0.

Each run retains source/build hashes and exact child PID/timing/output under
build/M0-T447/S3/original-bodies-r32 through r34, reusing call-boundaries-x86.
The creator/control bodies compiled with the production private layout;
supplemental doubles do not select or prove the complete product link.
Mirror exceptions080/081 and source-local markers cover the bounded changes.
Guest, release assets and deployment remain unchanged; no commit/push.

### Private-word failure routing before product get/set selection

Source trace from wow_user_private_access.c through try_get/set_long finds
an adapter-owned contract defect: failed validate_hwnd restored the entry
error and returned FALSE, which the outer public accessor interprets as
permission to read/write native extra bytes. An original-owned window whose
borrow failed must not silently switch storage authorities. This is distinct
from the two still-pending CPD/style compatibility choices.

ADAPTER-WOW-063 now distinguishes that failure using the existing published
window binding and owns_client_state capability. An owned view returns
handled/zero with the original validation error; a missing or resource-only
binding leaves the operation unhandled and restores entry error. No second
state map, native write, original-host/guest change or provider substitution
is added. The successful original getter/setter path remains authoritative,
including zero results and errors. The query does not treat creation facts
as ownership, and resource-only observations remain ineligible for private
control storage.

The existing window-client message fixture now covers owned validation
failure, missing binding, resource-only binding, original zero/error results
and error-preserving owner release, alongside its existing exact-message
argument/borrow tests. It is wired into the common group runner but not yet
built/run. Original _GetWindowLong/_SetWindowLong remain declared doubles in
this wrapper fixture, so it does not prove the still-open original get/set
provider closure. No per-leaf compilation is launched. Diff check passes.

### Retry verification r35

The existing supplemental cohort was rebuilt incrementally and executed by
the owned hidden background runner25296, started2026-10-17T02:36:00.3851403Z,
exit0. Ninja36928 started02:36:02.2541043Z and exited0 after11.4215118s.
All27 fixtures exited0. The newly selected window-client message/access
fixture2492 started02:36:13.6972210Z and exited0 after0.2282168s.
Its assertions cover owned validation failure without native-storage fallback,
absent/resource-only ownership, preserved errors across release, and original
handled zero/error results. The creator/default/focus/modal cohort also passes.

Command: tests/observation/verify-wow-control-boundaries.ps1 with EvidenceRoot
build/M0-T447/S3/original-bodies-r35; shared build root remains
build/M0-T447/S3/call-boundaries-x86. Exact source/build hashes and each child's
stdout/stderr are retained in that evidence directory; the outer runner logs
are adjacent original-bodies-r35-runner.stdout.log and .stderr.log.
These boundary doubles do not prove real HWND reuse, guest execution or the
complete product get/set dependency closure. No guest, release asset or deployed
image changed. No S3 publication, commit, push or closure is claimed.

### Original get/set chain added to the grouped verifier, r36

The existing dialog_getset fixture was outside the common cohort. Add its
three retained original translation units (rtl/getset, client/ntstubs with
WOW_ORIGINAL_DIALOG_GETSET, kernel/getset), the actual window access adapter
and the canonical control extent provider to one linked target. No duplicate
getter/setter algorithm or production service substitute is introduced.
Update the fixture's failure-owner dependencies with explicit rejecting
doubles: only invalid HWND lookup may reach acquire; unexpected message or
state retention calls fail the fixture. Extend the EDIT publication case to
use a real local first-word value and assert original same-process dereference
versus foreign-process slot-address behavior, followed by terminal clearing.

The linked chain also asserts dialog result/user/extra-word bounds, procedure
encoding and conversion requests, private-control initial/terminal writes,
denied callers, canonical registration extents, and initialized ListBox
ownerdraw/string-style protection. CPD decode/generation, process authority
and native style writes remain named test doubles. In particular, its four
style writes prove the original mask calculation, not modern notification
timing. The test does not settle either pending compatibility choice.

Hidden runner25236 started2026-10-17T02:38:42.9522827Z, exit0; Ninja28208
started02:38:44.9135809Z, exit0 after12.0629532s. All28 cohort fixtures exit0.
The new linked fixture12236 started02:38:56.9995403Z, exit0 after0.2226262s:
dialog_getset_failures=0 conversions=4 style_writes=4. Reproduce with
verify-wow-control-boundaries.ps1 and EvidenceRoot
build/M0-T447/S3/original-bodies-r36. Its source/build hashes and per-child
logs are retained; the shared incremental cache remains call-boundaries-x86.
Compilation retains historical-layout/unused-variable warnings and a fixture
UserAssert macro-redefinition warning; these are not claimed warning-free.
No original source body, guest or product image changed in this verification
addition. Full product get/set service composition and real guest acceptance
remain open; do not substitute this test-only precomp into the product graph.

### Relationship/lifetime verifier owner-selection audit

Source trace confirms the intended resource path: borrow_window_state retains
the page-domain generation (callback identity first); borrow_client_graph
walks exact retained relations, prepares resource-only WNDs without control
ownership, and transfers relation pins to the outermost active scope before
publishing links. publish_client_graph validates every row before scalar/link
writes. Native-call return enters the existing execution owner before graph
publication. These are source conclusions, not completed lifecycle evidence.
The staged class-lifetime fixture covers old/replacement bindings and rejected
graph transactions; the page-domain fixture contains retained-generation
reads after retirement/reuse. Neither is certified by supplemental r36.

The production-object lifecycle runner already rebuilds its selected objects,
but still passed /force:multiple to LINK. That permits duplicate provider
definitions and weakens proof of which owner executed. Remove that option;
retain the exact library set, explicit fault-object exclusions, map output
and failed-link refusal. Any duplicate revealed by the next complete-group
link must be resolved by explicit source/ownership selection, not suppressed
again. This does not retroactively prove a wrong provider was selected by a
historical run, nor does a strict link alone prove runtime identity/lifetime.

The stricter verifier is staged and unrun. No new isolated compile/test was
launched: the complete production get/set closure remains prerequisite to
this group. No mirror or guest code changed. Required remaining evidence is
real retained graph/promotion/nested-return/retirement behavior with the
current production objects, then ordinary guest and package acceptance.

### Reached get/set service boundary, source-only follow-up

Rechecked the pinned original rtl/getset.c GetWindowData and kernel/getset.c
xxxSetWindowData against every current source caller of _GetWindowLong,
_SetWindowLong, xxxSetWindowLong and wow_window_client_set_window_long.
The current public try_get adapter excludes all negative indices before the
original getter; existing logical/native negative-index owners remain outside
that call. The try_set adapter admits only nonnegative indices and GWL_STYLE.
Direct original dialog calls write DWL_MSGRESULT; original usercli.h control
publication/cleanup macros write index zero. Thus the reached server negative
branch is GWL_STYLE, not a request to import NT4 window parenting, server/client
procedure tables, system/logon privilege state or all of xxxSetWindowData.
This is a current selected-call-graph conclusion, not permission to silently
drop a newly reached original index or install a success/no-op stub.

CBFNID already has a production canonical-size adapter:
wow_edit_control_extent delegates to wow_control_registered_extent, including
FNID status-bit normalization. Reuse that owner rather than introducing a
second size table. Exact local private-state provenance also already exists in
wow_window_client_owns_state; resource-only projections must not qualify.
Outgoing CPD generation has an existing native leaf, but the incoming token's
address and encoding still require the pending normalization decision. The
original dialog setter stores both, and DefDlgProcWorker calls the stored
address directly; a generic CallWindowProc alias does not satisfy that contract.

Two outstanding owner decisions still prevent selecting the whole get/set
group as a product implementation: helper-window CBT observability and native
pre4.0 style-notification semantics. The owner was asked explicitly again;
automatic goal continuation is not a choice. No new build/test, source body
change, runtime substitute or release was made during this source-only audit.

### Owner rejects CPD helper and requires original style-rule feasibility

The owner explicitly rejects creating hidden helper windows for CPD
normalization. Do not install that research approach. The owner also requires
investigating whether the original style-notification rule can be preserved;
modern notification semantics remain unapproved. This supersedes treating
these two questions simply as pending yes/no approvals.

Rechecked pinned classchg.c xxxSetWindowStyle: both notifications are gated
by WFWIN40COMPAT, explicitly for Quicken2. The operation also owns clipping
cache invalidation, child/menu transitions, visibility accounting, minimized
compatibility and extended-edge state. Merely changing style bits without
these native resource consequences is not an equivalent general setter.

Installed win32kfull.sys again matches the retained PDB and image SHA256
94F00AABE76A4AF3794E2134B806BF89E556B66814859C285DE62173350A5173.
Read-only objdump confirms xxxSetWindowData's style branch at RVA264F79
calls xxxSetWindowStyle with WND/index/new-value, without a notification
control argument. Its RVA6654C entry dispatches WM_STYLECHANGING at RVA66606.
NtUserAlterWindowStyle at RVA219F70 masks the caller's mask with0030023F
at RVA219FE4 and writes only those style bits. It is not the full setter.
NtUserSetWindowState/ClearWindowState additionally validate their packed
flags through RVA1DE5C0 and the16-byte allowlist at RVA371900:
000000000000001000030200200AB902. These services are not unrestricted
private-state mutation; raw NT4 WFWIN40COMPAT0480 is not admitted by that
table. These RVAs/bytes are research evidence only, never runtime offsets.

Research fixture wow_style_notification_probe.c compares the native full
setter and masked leaf on its own ordinary test window, not a product helper.
Background runner22488 started2026-10-17T03:05:34.5501005Z, exit0;
Ninja34508 started03:05:36.3422712Z, exit0 after10.5473799s;
probe36208 started03:05:46.9087666Z, exit0 after0.2261449s.
Evidence root build/M0-T447/S3/style-notification-r1 retains hashes/logs.
Reproduce using verify-wow-control-boundaries.ps1 -StyleNotificationResearch
with a new EvidenceRoot; no production source or guest changes are involved.

Observed: full WS_BORDER mutation delivers one changing and one changed
notification to both WNDPROC and WH_CALLWNDPROC. Masked mutation leaves
WS_BORDER unchanged but sets WS_HSCROLL, delivering neither notification.
A call-window hook injects an independent same-message send during the full
mutation: two changing and one changed arrive, all with InSendMessageEx=0.
The printed synthetic wParam is65520 versus native -16 (the hook payload's
representation), so this is not proof of byte-identical messages. It proves
that HWND/thread/message plus an active setter flag or InSendMessageEx alone
does not identify the service-generated notification. No proposed suppression
implementation is validated by this fixture. It uses a thread-local hook,
not a claim of executed global-hook coverage.

Conclusion supported by these inputs: the inspected full setter cannot be
used unchanged to reproduce pre4.0 notification absence; the inspected silent
leaves cover only subsets and lack the full setter's resource effects. A
WNDPROC-only filter also cannot erase notifications already observed by a
native hook, and a broad scoped filter wrongly captures independent sends.
This rejects these concrete routes, not every possible future host mechanism
or a mathematically universal impossibility claim. Full exact equivalence is
not established and must not be reported as implemented or impossible merely
because these candidates fail. No new semantic waiver or product publication.

### Owner-approved style service continuation

The subsequent explicit approval accepts native style notifications even for
pre-4.0 windows. It supersedes the unanswered style-choice record above, not
the rejection of CPD helper windows. ADAPTER-WOW-063 now supplies the reached
xxxSetWindowData GWL_STYLE continuation through the existing
wow_native_SetWindowLongA execution/return boundary. Original client ListBox
validation and server private-word policy remain in their retained bodies.
Only this audited index is admitted; other indices fail ERROR_INVALID_INDEX.
The exact PWND is retained before callout, HWND is captured before reentry,
and the adapter never writes requested style over callback-modified native
state. Original classchg's private desktop/DC/visibility dependencies remain
native resource operations, not imported NT4 server state or copied writes.
No new mirror diff, notification filter, helper window or guest change.

The getset fixture now uses this production continuation instead of its
previous xxxSetWindowData double; the native resource/return service remains
an explicit double. The access fixture covers A/W-independent bit handling,
exact borrow rejection, invalid/null input, return/error preservation and
simulated retirement during the native call. Native reentry, actual resource
effects and guest behavior still require full integration acceptance; these
fixtures cannot establish those facts. Full private-precomp/CPD closure and
all13 publication remain open.

Background grouped run style-service-r37: runner PID17956 started
2026-10-17T03:37:02.1120669Z; Ninja PID35344 started03:37:03.9695871Z,
exit0 in13.0102807 seconds. All28 supplemental fixtures report exit0.
Actual three-body getset fixture PID36672 reports
dialog_getset_failures=0 conversions=4 style_writes=4; access fixture PID39768
reports WOW_CONTROL_MESSAGE_BINDING failures=0. Source/build hashes and raw
logs are under build/M0-T447/S3/style-service-r37. Existing original-layout,
fixture-macro and calling-convention warnings remain visible; this is not a
warning-free or full production build. No assets or installed files changed.

### CPD consumer contract separation

Source follow-up corrects the previously combined normalization requirement.
Pinned OpenNT kernel/class.c::_RegisterClassEx consumes pfnClientPrevious
only, not wType; registration encoding comes from its entry. Retained
client/ntstubs.c::_SetWindowLong(DWL_DLGPROC) consumes both fields and changes
DLGF_ANSI to the underlying procedure encoding. The class probe's recovered
address but wrong encoding rejects it as a dialog normalizer, not as an
address-only class operation. This does not select a helper class/window.

Dialog normalization affects a whole chain: rtl/getset.c and client/ntstubs.c
produce outgoing CPDs on encoding mismatch; dlgmgr.c directly invokes lpfnDlg
and uses DLGF_ANSI for WM_NOTIFYFORMAT/NF_QUERY. Original kernel/getset.c
GetCPD asserts that its input is not already CPD-tagged. Replacing only the
direct invocation with CallWindowProc would leave token-as-address wrapping
and incorrect encoding in the remaining chain; reject that partial repair.

Ordinary wudlg.c creation supplies W32DialogFunc, a real host callback, not a
guest far pointer or native CPD. However wuwind.c::WU32SetWindowLong's
DWL_DLGPROC branch uses IsThunkWindowProc to restore native procedures from
guest thunks; GetThunkWindowProc carries previous native results back to the
guest. Therefore normal creation success cannot exclude native-token restore
from acceptance. No second dialog callback engine is needed for the ordinary
guest case. Class context still sets resolve_callproc=NULL and rejects CPD
registration, so that separate boundary also remains open.

This source audit changes the required design split, not runtime capability.
No new compilation or publication: complete storage/get/set/A-W/invocation/
lifetime together before testing the next integration group. No guest edits,
helper-window implementation or new semantic concession.

### Opaque invocation is not complete dialog procedure ownership

Consumer sweep of the selected ntuser source finds the complete PDLG procedure
field surface in dlgbegin.c (initial address and ANSI flag), ntstubs.c
(replacement/old-value conversion), rtl/getset.c (retrieval/conversion), and
dlgmgr.c (direct invocation plus WM_NOTIFYFORMAT/NF_QUERY). No other selected
ntuser C source reads or writes lpfnDlg or DLGF_ANSI. This is a source-surface
inventory, not runtime coverage or proof that external consumers cannot exist.

The existing wow_user_registration_bridge.c::call_previous_window_proc already
handles native opaque tokens through CallWindowProcA/W and wraps native return
with execution restoration/publication. Its tagged WOW path uses registered
pfnWowWndProcEx. Reuse that owner for invocation if a complete opaque-storage
design becomes viable; do not add a second message conversion engine.
However it supplies neither underlying procedure identity nor encoding to
the other four dialog operations. The installed native GetCPD leaf is only
an outgoing-token service, not a decoder. Replacing the invocation alone is
therefore insufficient even though ordinary CallWindowProc is supported.

Microsoft's CallWindowProcA contract explicitly allows DWL_DLGPROC results
to be opaque internal values and delegates encoding conversion to that API:
https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-callwindowproca
Its window-encoding explanation says IsWindowUnicode describes the current
window procedure, not every procedure in its subclass chain:
https://devblogs.microsoft.com/oldnewthing/20180906-00/?p=99665
These support native invocation, not recovery of a saved token's historical
encoding. Do not infer DLGF_ANSI from current HWND encoding or a registration
entry when restoring a saved dialog token.

Producer audit also prevents an unjustified normal-creation-only shortcut:
wuclass.c obtains thunkable native results from class-info, class-long and
class-procedure replacement; wuwind.c does so for dialog/window getters and
procedure replacement. A future provenance-based design must cover all these
sources, native token ownership beyond one HWND, class retirement and handle
reuse. A map populated only by the project's NtUserGetCPD calls is incomplete.
No such partial map or cached encoding guess was added. This fixed producer/
consumer inventory is the required scope for the next design, not a request
to expand S3 beyond its already admitted CPD closure.

### Producer-cache feasibility: original lifetime is class-scoped

Pinned kernel/getset.c::GetCPD/FindPCPD keys the token by underlying procedure
and full CPD option word, links it into pcls->spcpdFirst, and reuses that
class-owned token. kernel/class.c::UnlockAndFreeCPDs is called during class
destruction; class clones initialize a separate CPD list. HWND retirement is
not this lifetime boundary. The misleading nearby historical comment about
window/thread death is not a substitute for these actual allocation/free
owners. Modern native token lifetime still requires native-owner evidence;
do not impose these NT4 private lists on current USER.

Retained wsubcls.c::GetThunkWindowProc deduplicates by lpfn32 and stores that
value in guest thunk metadata. IsThunkWindowProc reads the stored value back;
neither supplies CPD encoding, original source class, or a native retention
lease. Extending that guest record is prohibited and unnecessary for normal
opaque CallWindowProc invocation. Thus this existing thunk table cannot serve
as a complete address/encoding resolver or certify a token-keyed cache across
native class destruction/reuse. wow_window_note_native_procedure records
standard-control cleanup ownership, not CPD contents or lifetime; do not
repurpose it as a decoder.

Decision: do not implement the proposed producer-only CPD cache. It would
cover known outgoing calls but leave foreign/native producers and invalidation
unproved while creating a second source of truth. No runtime changes or
tests were performed for this rejected design. The finite remaining CPD
design must either obtain normalization from the native owner without the
rejected helper/private-layout approaches, or explicitly redesign the full
procedure storage/get/set/query/invocation boundary and account for its
observable A/W semantics before selection. The approved style exception does
not authorize weakening these semantics.

### Original outgoing CPD owner selected, incoming normalization still open

OPENNT-HOST-120 restores the complete unchanged client/clmsg.c::GetCPD body
from the pinned source (SHA256 recorded in opennt-host/README.md). A separate
production object selects it with the existing control-client facade, original
HW projection and existing named NtUserGetCPD service. No copied token table,
private native layout or new conversion algorithm. The selected callers are
retained window/dialog getset, not a PCLS-to-native-pointer bridge. Function
body comparison against the pinned source passes after newline normalization.

The shared getset fixture is staged to link that actual original body, removing
its fixture_get_cpd algorithm-level replacement; only the native transport
remains a double. Both production and supplemental object selections are
staged, not compiled or run. Do not reuse r37's pass as evidence for these
new inputs. Incoming normalization, A/W query/restore, full lifecycle and
product link/guest acceptance remain open. This is part of the complete
getset group; no per-leaf build/test was started.

The alternative of borrowing the actual dialog's native procedure slot is
not selected. Current canonical cleanup explicitly retains recovered PDLG
and does not assume modern USER owns corresponding dialog state. Using the
slot as temporary scratch would require proven initialization, visibility
during reentry and finalization ownership. The earlier helper-window probe
does not establish those facts for real application windows. No native
DefDlgProc initializer, temporary application-slot mutation or second PDLG
owner was introduced.

### ANSI-producer narrowing reviewed, not a generic decoder

The selected MVDM compilation is ANSI; original wuwind procedure getters and
restoration enter the A facade, and the class query uses GetClassInfoExA.
Recovered USER control bodies are separately Unicode. This supports ordinary
Win16 call-site encoding, not a universal assertion about the origin of every
saved native CPD. No reverse-encoding inference was installed.

Class registration needs a more precise reachability description than the
earlier broad producer list: WU32RegisterClass does not call IsThunkWindowProc.
It tags the guest address with WNDPROC_WOW, preserves the original in WC and
adjusts WOWCLASS_VIRTUAL_NOT_BIT31 for high guest selectors. A guest thunk
obtained from GetClassInfo therefore remains a tagged guest thunk on this
registration path, rather than automatically becoming the native CPD input.
Do not cite GetClassInfo alone as proof of a reached native registration CPD.
Conversely, do not delete the original CPD branch or claim it unreachable
without checking selector-tag collisions and the complete registration ABI.
DWL_DLGPROC restoration explicitly uses IsThunkWindowProc and remains the
proved incoming native-token path. This audit changes reachability evidence,
not the original guest encoding algorithm or its acceptance requirements.

Source-notice review also restores clmsg.c's original Microsoft module header
above its admitted subsets. No executable expression changed. The outgoing
GetCPD object and fixture remain staged/unbuilt; no additional tests or
publication were run during this audit.

### Outgoing CPD grouped verification — r38/r39

Question: does the admitted outgoing original GetCPD compose with all three
original get/set bodies without the former algorithm-level test replacement?
The background runner is tests/observation/verify-wow-control-boundaries.ps1,
using the retained build/M0-T447/S3/call-boundaries-x86 cache and distinct
callproc-group-r38/r39 evidence directories. Both runs record source and graph
SHA-256 inputs, stdout/stderr and exit.txt; adjacent runner logs retain PIDs.

The r38 outer runner PID31756 started2026-10-17T03:52:31.4985678Z and exited1120.
Ninja PID36464 exited1120 after77.4262356s: GetCPD referenced unresolved HW.
No fixtures executed after this failed build. The fixture facade had omitted
WOW_ORIGINAL_CLIENT_HANDLES; production already selects those original macros
through wow_edit_call_bindings.h under WOW_ORIGINAL_EDIT_CLIENT. The correction
selects the same existing user.h definitions in dialog-getset/precomp.h, not
a replacement HW function or an edit to the original algorithm. No additional
mirror/guest diff or semantic exception is introduced by this correction.

The r39 outer runner PID29620 started2026-10-17T03:54:28.7393646Z and exited0.
Ninja PID31528 started03:54:30.3691424Z, exited0 after16.5506676s; all28 fixture
executables exited0. Getset PID600 returned
`dialog_getset_failures=0 conversions=4 style_writes=4`.
Original-layout/macro warnings remain; this is not a warning-free build.

This now proves the outgoing original body's HWND/options/procedure forwarding
and the tested getter/setter results against explicit native transport doubles.
It does not prove incoming CPD normalization, actual native token lifetime,
full product composition or guest behavior. Those obligations remain unchanged.
The separate real-page-domain/native-creation lifecycle runner requires the
current complete provider closure; do not revive stale provider objects or
force duplicate definitions to manufacture its pass. Installed P16 and
assets/release are untouched; there is no S3 delivery or closure claim.

### Retained-window fixture identity correction and executable selection

Source review of wow_window_class_lifetime_fixture.c found its lifetime
markers occupied the first DWORD of WND, namely head.h. This fabricated the
identity passed to host-state consumers even though the test created actual
native windows. Move the markers to dwUserData and set head.h to that actual
window before attachment; verify both values after retirement. Do not change
production identity validation to accommodate the fixture. This is a test-only
correction, with no mirror or guest diff and no new runtime claim.

The lifecycle runner now explicitly selects this previously unlisted fixture
with -Case class-lifetime and requires its zero-error completion marker.
It links current production owner objects, while using the existing explicit
class-publication double; it cannot prove real CCPU publication. The separate
window-borrow/native-creation rows retain that requirement. No new permissive
stub or duplicate-definition override is added. This selection and corrected
fixture are staged, not built/run: execute with the complete lifecycle group
after product dependency closure, not with stale provider objects. The r39
28-fixture evidence does not include this fixture.

### Exact-state attachment and borrow preconditions

Following the corrected fixture, source tracing found the same missing
identity precondition at two production entries. attach_client_state checked
the HWND/property association only when binding->objects existed; a
property-only enrolled binding could consume a candidate whose head.h named
another window. borrow_client_state could return a cached matching allocation
even when its HWND argument disagreed with that allocation's retained head.h.
These are adapter validation defects, not evidence of an observed guest crash.

ADAPTER-WOW-069 now checks the existing HWND/property association before both
attachment routes and removes the duplicate managed-route query. A failed
check returns ERROR_INVALID_WINDOW_HANDLE without consuming the candidate.
Exact-state borrowing validates the retained head.h before any scope search;
it deliberately does not check the live property, because an already retained
allocation remains valid after native destruction or replacement. No second
identity map, native window policy, guest change or original mirror diff.

The only production attachment caller, wow_window_client_initialize, already
sets head.h from its actual creation HWND before transferring ownership.
The class-lifetime fixture adds a different-live-HWND attachment rejection,
caller-owned storage check and a mismatched borrow rejection followed by a
successful correct borrow. Existing retirement/replacement assertions remain.
Source review and diff check pass; these production changes and assertions
are unbuilt/unrun pending the complete lifecycle cohort. r39 remains prior
supplemental evidence, not verification of these new production inputs.

### Production composition compile check

The supplemental graph is not the production composition. To check that
boundary, generate New-T404S5Wow32ProviderNinja.ps1 with BuildRoot
build/M0-T447/S3/production-cohort-x86 and compile all217 selected obj/*.obj
and obj/*.res targets in24-target batches with ninja -k 0. Reuse that graph
for the correction run. The owned background command is
build/M0-T447/S3/compile-production-cohort.ps1; its stdout/stderr are
production-cohort-r1.*.log and production-cohort-r2.*.log in S3's build root.
The generator requires an existing parent import library and receives
build/M0-T445/S6/r003/ntvdm.lib solely to generate the graph. This diagnostic
does not link that library, build a DLL, validate parent ABI compatibility,
publish a binary, or exercise guest code. A current coherent parent remains
mandatory before product linking and delivery.

r1 owned PID2508 started2026-10-17T04:04:45.1389327Z and exited1. Continuing
all batches exposed two failing translation units: original lboxctl2.c lacked
INT_MIN/INT_MAX, and wow_edit_entry_bindings.c's caret callback lacked PWND's
layout. Pinned OpenNT client/precomp.h line9 includes limits.h; our selected
production facade omitted it. The adapter timer entry similarly consumed the
retained WND's fnid without including its existing layout owner. Supplementary
fixtures did not demonstrate these production compilation dependencies.

Registered under ADAPTER-WOW-058/063, add limits.h to control-client/precomp.h
and wow_user_client_view_layout.h to the owned timer-entry translation unit.
No algorithm, state owner, native private-layout cast, mirror or guest change.
r2 owned PID33644 started2026-10-17T04:06:19.4499074Z and exited0; all217
targets compiled using the unchanged production Release flags. Last compiler
log output was04:07:20Z, approximately61 seconds after supervisor start (not
an independently instrumented Ninja duration). Warnings remain, including
original MLCalcXOffset's return-path warning, MapDialogRect declaration linkage
and ANSI flag overrides; this is not a warning-free or linked product claim.
The exact-state guards now compile, but their lifecycle assertions remain
unrun. Full CPD/getset provider closure and real guest acceptance stay open.

The follow-up existing verify-wow-control-boundaries.ps1 used EvidenceRoot
build/M0-T447/S3/production-composition-r40 and the reused call-boundaries-x86
cache. Owned supervisor PID42908 started2026-10-17T04:07:08.2099524Z, exit0.
Ninja PID42168 started04:07:10.2895059Z, elapsed74.4686721 seconds, exit0;
all28 supplemental fixture processes exited0. Input hashes and each process's
start/elapsed/exit/logs are retained by that runner. No CPD-helper research
switch was used. These tests still do not cover real guest execution or the
pending class-lifetime fixture. Governance PID40792 started04:08:29.6590295Z
and exited0; diff --check also passed before this result-record update.

### Production getset closure and original private-word setter selection

Static inspection of the r2 production COFF objects, accounting for common
data definitions as well as section-defined symbols, confirms the adapter's
get/set callers lack three intra-component providers: _GetWindowLong,
_SetWindowLong and xxxSetWindowLong. This is not a DLL link check; parent
imports and external libraries were not included. An initial diagnostic
PowerShell parser incorrectly reused Matches during a nested match and
failed; only the corrected Regex.Match-based inspection is used here.

Trace: wow_window_client_set_window_long retains and validates the local
WND, then calls original xxxSetWindowLong. Its original FCallerOk guard,
CBFNID reserved-field rules, result/user fields, extra-byte bounds, previous
value and assignment must remain the original kernel/getset.c implementation.
That body never decodes CPD: negative-index forwarding reaches the existing
xxxSetWindowData style continuation. The public caller already limits that
continuation to approved GWL_STYLE; other private-word writes use the original
bounds/guards. No host fallback is added on an original zero/error result.

Select the already registered OPENNT-HOST-086 kernel body in the production
graph as private-word-setter.obj. Its finite precompile selector binds
FCallerOk to wow_window_client_owns_state and uses existing CBFNID extent
queries. This ownership check retains the exact scoped allocation and rejects
foreign/resource-only state; it does not emulate NT4 system/logon privileges
or inspect modern private structures. Add the missing RIPERR3 error-preserving
diagnostic macro and the two exact function declarations. Registered under
ADAPTER-WOW-063; no original-body or guest changes.

This selection is staged and deliberately not followed by another one-object
build/test. The remaining original client and RTL selection still need their
complete boundaries. _GetWindowLong's only current production caller excludes
negative indices, while the unchanged body still references GetWindowData;
do not satisfy that dependency with a fabricated success/no-op provider.
The client dialog setter still requires real incoming CPD address/encoding
normalization. Existing r40 fixture doubles prove neither of these missing
production services, and r2 predates this graph change. Full grouped compile,
link, lifecycle and guest verification remain mandatory before publication.

### Negative getter: retained-view completeness and cycle audit

Source inputs: pinned rtl/getset.c GetWindowData (lines173 onward), current
wow_window_client_create_state, wow_window_words_binding logical_procedure,
wow_user_get_window_longA and wow_native_GetWindowLongA. No runtime probe.

The original negative getter cannot simply be added to consume our current
WND projection. create_state populates identity, extent, name, geometry,
style/ExStyle, module, child ID, WOW words and creation compatibility. It does
not publish lpfnWndProc, WFANSIPROC or WFSERVERSIDEPROC. The real logical
procedure is owned by the window binding's cleanup target/native_callproc;
GetWindowData also expects real PMENU identity for a top-level GWL_ID, not
the child-ID-only value in this projection. Native dwUserData is likewise
not established by this constructor. Importing the original negative body
without those inputs would return invented zeroes or apply wrong encoding.

The current negative-index facade is not an interchangeable native query:
GWL_WNDPROC hides worker/NTW16S transport identities using the existing logical
target; private WOW indices return the existing retained WW; ordinary values
come from modern USER; GWL_HWNDPARENT additionally publishes the returned HWND
through the existing discovery owner. Replacing this with blanket native
GetWindowLong loses those identity/publication contracts. Conversely, blindly
forwarding every GetWindowData call into the public facade risks future
recursion: the public facade first invokes the recovered private-word getter.
Current production entry explicitly excludes negative indices, so the present
call graph is acyclic, but the unchanged getter still requires a linked
negative provider. A complete shared leaf must preserve the special owners,
A/W conversion and failure behavior, and stay below the public dispatch.

Current exported project facade is ANSI. That fact does not prove an arbitrary
saved CPD's original encoding, nor justify a fabricated Unicode implementation
when selecting the original BOOL bAnsi body. No negative-provider stub, new
WND procedure mirror, private native decoder or temporary application-dialog
slot mutation has been installed. This narrows the remaining design work to
the existing logical procedure/encoding boundary, rather than importing the
rest of USER to obtain fields absent from the current retained view.

### CPD public-contract boundary revalidation

Revalidated current Microsoft CallWindowProcA and GetWindowLongPtrW reference
documentation against the selected client setter/getter and dlgmgr paths.
The documented returned value may be an opaque internal procedure identity;
CallWindowProc owns its invocation and character conversion. That contract
does not promise a public inverse yielding both raw address and original
encoding. This is not a proof that every possible implementation is impossible.

The distinction is observable here: original client/ntstubs.c normalizes the
incoming token and sets DLGF_ANSI from its true target encoding, not from the
setter's A/W entry. Original dlgmgr.c WM_NOTIFYFORMAT/NF_QUERY returns ANSI
or Unicode using that flag; original RTL getter also uses it for outgoing
procedure conversion. Existing wow_user_registration_bridge can invoke an
opaque token correctly but does not supply those other contracts. Therefore
using it alone cannot be reported as complete original semantics. A setter
that merely preserves its caller's ANSI flag would be a new compatibility
boundary, not an implementation detail authorized by the unrelated style or
color approvals. No such change is selected.

Official references inspected:
- https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-callwindowproca
- https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowlongptrw

The alternate actual-dialog native-slot design still lacks initialization,
single-owner and finalization proof; the hidden-helper experiment is not
evidence for it. Do not silently substitute that design, create an extra
window, or decode a native token using NT4/private-host layouts. Published
baseline and guest remain unchanged.

### Guest procedure-thunk provenance is not a closed producer set

Rechecked every GetThunkWindowProc/IsThunkWindowProc reference in the selected
MVDM wow32 C/header tree, then read wsubcls.c's implementations and the
WU32Get/SetWindowLong and WU32SetClassLong consumers. This resolves a specific
remaining uncertainty in the ANSI-producer narrowing argument, without
another runtime probe or an encoding cache.

GetThunkWindowProc deduplicates entries by lpfn32 and writes that value into
the guest thunk record. However, IsThunkWindowProc does not require that its
input belong to the vptwpFirst list, or that GetThunkWindowProc created it.
It reads the three DWORDs immediately before the supplied guest address,
checks SUBCLASS_MAGIC, and accepts the supplied lpfn32; a zero value instead
selects GetStdClassWndProc. Its explicit comment permits looking up the
standard thunk directly from USER.EXE. WU32SetWindowLong's DWL_DLGPROC branch
passes the returned nonzero native value straight to SetWindowLong.

Consequently, enumerating our normal ANSI getter producers does not establish
the origin/encoding of every procedure value accepted by the original restore
contract. Adding membership validation, or assuming a saved CPD was issued by
an ANSI getter, would narrow that contract. This is a source-level acceptance
boundary, not a claim that a particular retail program uses a fabricated or
copied thunk. No unsupported guest-memory test or guest patch was performed.

This rules out using producer enumeration alone as the proof for a generic
incoming decoder. It does not rule out preserving independently recorded
provenance for known tokens, but such a cache still needs the already-open
native lifetime/reuse proof and a correct unknown-token path. No provenance
restriction, cache, helper window or decoder was added.

### Actual-dialog procedure ownership alternative: approval boundary

Complete selected-source consumer sweep finds procedure/encoding use in
dlgbegin initialization, client/RTL DWL_DLGPROC get/set, dlgmgr invocation and
WM_NOTIFYFORMAT. Message result storage is read/reset/returned by dlgmgr and
read/written by RTL/kernel getset. Therefore a real-window native-slot design
cannot be implemented as a one-call decoder substitution.

Bounded design now owner-approved; implementation/runtime verification pending:

- Modern USER owns the actual dialog's procedure slot and DWL_MSGRESULT;
  original extra-word getters/setters delegate those two fields to this one
  owner. No second authoritative copy or reinterpretation of native CPD.
- Original dialog creation, modal loop, focus/default-button/navigation,
  children and original font/EDIT-segment cleanup remain original-owned.
- Ordinary procedure invocation uses the matching public A/W getter and
  CallWindowProc. WM_NOTIFYFORMAT dispatch/default handling must use the
  native dialog owner so target encoding is not inferred from the setter;
  the application callback must execute exactly once and its message-result
  writes must reach that same native result slot.
- No helper HWND or guest change. The actual dialog must be initialized and
  finalized through its supported host dialog contract. Current task-exit
  patching routes dialogs to DefWindowProc before DestroyWindow; this needs
  an explicit no-guest-callback terminal path that retires native procedure
  storage while the HWND is still valid. Calling native DefDlgProc after
  HWND retirement, or silently adding a second uncontrolled PDLG, is invalid.

Microsoft DefDlgProcW documents default processing for custom dialog classes
with at least DLGWINDOWEXTRA and warns against calling it from a dialog
procedure (recursive dispatch). A candidate must enter it from the window
adapter, not from its own application-dialog callback. Reference:
https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defdlgprocw

This approved design changes the selected NT4 get/set/notification implementation
to modern native semantics for those narrowly named fields/messages. Exact
NT4 token representation and every old notification/default-return detail
cannot be promised from the public contract. The owner's subsequent explicit
approval selects this narrow compatibility boundary independently of the
earlier style/color approvals; it does not assert runtime success.

Acceptance must cover A/W query-save-restore in both directions,
unknown valid native tokens, exactly-once callbacks and DWL_MSGRESULT, nested
subclass replacement/destruction, creation failure rollback, normal and task
termination, and HWND reuse. Keep the existing whole-product and immutable
guest gates. This is not an assertion those cases pass or that all S3 work
reduces to this one boundary. Repeated documentation of the unresolved decoder
is no longer treated as implementation progress; implementation is authorized.

### Approved actual-dialog owner implementation, first grouped verification

OPENNT-HOST-121 is registered in src/opennt-host/README.md for five existing
mirror files: dlgbegin/dlgmgr, client ntstubs and RTL/kernel getset. The
production control facade selects public native dialog procedure/result
storage; original PDLG procedure/ANSI/result fields are not competing owners.
The adapter is wow_dialog_procedure_bindings.c/.h under ADAPTER-WOW-063.
Initialization temporarily clears/restores the actual HWND's procedure via
the W API, ensuring no extra application callback during native WM_NULL.
Regular invocation uses the existing A/W callback bridge. WM_NOTIFYFORMAT
enters native dispatch instead of invoking the application twice. Normal
NCDESTROY and the no-callback task-exit target retire native storage while
the HWND exists; original WM_FINALDESTROY must skip native result reads.
Creator promotion/binding failures retire the already-created HWND and avoid
destroying its attached menu twice. Original algorithms/resources stay owned
by their original bodies. No guest or published artifact changed.

Real-HWND probe: tests/observation/verify-wow-control-boundaries.ps1
-DialogNativeOwnerResearch -EvidenceRoot build/M0-T447/S3/dialog-native-owner-r4.
Outer PID29268, start2026-10-17T04:36:18.0488570Z, exit0; build exit0 in
11.5212583s; probe PID39760, start04:36:31.4653138Z, exit0 in0.8343534s.
Actual production procedure adapter plus explicit scheduler/publication
doubles:64 iterations,256 application callbacks,32 terminal callbacks,
zero assertions. Cases cover both A/W restore directions, native conversion,
one notification callback, shared message result, callback destruction and
callback-suppressed terminal cleanup. No production helper windows or native
token bit decoding. This does not prove real WOW scheduler/guest behavior,
unknown cross-window tokens, allocation-failure rollback or leak freedom.

Earlier r1/r2 failed a probe assumption: native SetWindowLongA can return an
opaque token for a previously empty Unicode slot even though the getter
returns zero. r3 verifies restoring that returned value empties the slot;
it does not require numeric token equality. r3 exit0 over64 iterations.
This is precisely why native return tokens are preserved, not normalized by
an inferred encoding flag. r4 repeats using the production adapter.

Production compile-only r3: PID41380 start04:35:24.7510670Z exit0,219 selected
objects/resources. This is not a DLL link and predates the last two cleanup
guards. Supplemental whole-group r41 failed linking two fixtures lacking the
new explicit transport doubles; no fixture ran. Its input snapshot also
predates those two guards, so it is not acceptance evidence. r42 contains the
guards and doubles: outer PID33608 start04:39:22.9227822Z exit0; Ninja31360,
start04:39:25.0797768Z exit0 in21.5746646s, all28 fixtures exit0. Finalization
fixture asserts native retirement occurs while task/borrow references exist
and is not repeated by the later original private finalizer. Keyboard and
creator fixtures retain explicitly labelled transport doubles; they do not
substitute for real-dialog integration acceptance.

Finite remaining obligations: production getter/client-setter composition
and its negative-data continuation; independent class-token boundary;
integrated creation/failure/nested-subclass/normal/task-exit/handle-reuse
tests; coherent full link and ordinary immutable-guest regressions; all13
publication/hash/commit/push gates. These remain open, not new scope and not
an invitation for owner acceptance of an unpublished build.

Production compile-only r4: outer PID39412 start2026-10-17T04:40:07.4518450Z,
exit0. All219 selected targets, now including the two final cleanup guards,
compile successfully. No DLL link or runtime claim follows from this result.

### Production private-word caller selection

Re-read the current provider graph, access facade, retained three getset
bodies and pinned original rtl/getset.c::GetWindowData (lines173 onward).
The production graph had xxxSetWindowLong only; the public facade directly
references _GetWindowLong/_SetWindowLong. Both client owners are now selected
as distinct objects, with original ntsend macros and the existing retained
allocation ownership predicate for TestWindowProcess. No additional mirror
body, replacement control logic, guessed encoding flag or dummy link symbol.
This selection is staged and unbuilt; r4's219-target result does not cover it.

Actual callers of _GetWindowLong in the selected source tree are confined to
wow_window_client_try_get_long, which rejects negative indices before entry.
Nevertheless the retained full body references GetWindowData, so full-link
closure still requires an explicit disposition. Original GetWindowData reads
USERDATA/style/menu/module/relationships/WOW words and maps server/client
procedures plus encoding from NT4 private flags. The retained view does not
currently establish all those private inputs. Do not manufacture them, assume
ANSI origin, return placeholder zero, or call the public facade recursively
without proving its A/W and gateway identity contract. Existing public
negative-index access remains in wow_user_private_access.c; its ANSI logical
procedure handling is not automatically a Unicode provider. This bounded
dependency is the next source-composition obligation, not a reason to run
another unchanged native probe or declare the product linked.

Disposition after complete source/header/DEF/build-script reference sweep:
_GetWindowLong has exactly one selected production call site, guarded by
index>=0 in wow_window_client_try_get_long, and is not a component export.
Select its nonnegative contract using WOW_ORIGINAL_PRIVATE_WORD_GETTER;
retain its existing negative-index branch as an explicit INVALID_INDEX
boundary rather than importing the unused GetWindowData implementation.
OPENNT-HOST-085 registers this bounded subset; public negative getters are
unchanged and never enter it. This removes the link dependency without a
fake provider, cached private flag or new native procedure mapper. Existing
full-body comparative fixtures retain their original negative branch.
Build/runtime proof of the new selection remains pending the integration group.

### Private-word integration group r43

The real-HWND native-owner probe now links all three retained original getset
bodies plus wow_dialog_procedure_bindings.c. Procedure/result reads and
writes pass through those original owners; task/publication and retained-view
identity are explicitly labelled doubles. It asserts the selected internal
negative-index guard, A/W opaque-value restoration, converted text, exactly
one notification callback, message-result visibility, no competing original
result/procedure copy, callback destruction and callback-suppressed teardown.
The probe is included in the ordinary supplemental whole group (29 cases),
not only an optional research switch. Original getset's comparative full-body
fixture remains separately selected for its original contract.

Production-cohort r5: outer PID4916 start2026-10-17T04:45:49.0491469Z exit0;
all221 objects/resources compile, including newly selected original reader
and client setter. No parent DLL link or production execution is claimed.
Whole-group dialog-getset-integration-r43: outer PID36540,
start04:45:59.4156824Z exit0; Ninja39288 start04:46:01.4596436Z,
exit0 in83.118348s. All29 fixture executables exit0. Native-owner probe
PID32804 start04:47:29.4005801Z exit0 in0.9005157s reports64 iterations,
256 callbacks,32 terminal callbacks,0 errors. Artifact/source identities and
per-case results are retained by the runner in that evidence directory.

The negative GetWindowData link dependency is now removed from this selected
internal contract, not postponed. Outstanding product obligations remain
class-token handling, full link and real lifecycle/guest acceptance. Source
review finds the sole RegisterClassWOWA producer in WU32RegisterClass, which
tags the guest procedure and preserves virtual selector bits before calling
the registered host boundary. Do not infer all accepted tagged values are
disjoint from FFFF solely from this fact; class publication still rejects
that tag without a resolver. No class-decoder implementation or acceptance
claim is made by the dialog/getset tests.

### Full production link r1 and finite composition corrections

Static COFF definition/reference comparison over the selected221 objects
confirms no missing component-owned wow_* implementation; seven runtime
symbols belong to the parent worker. This is not a link success claim.
Then build/M0-T447/S3/link-current-cohort.ps1 regenerates and builds the
existing worker cache and the S3 provider using that current parent import
library, without staging/running/publishing a package. Outer PID40324 start
2026-10-17T04:52:03.0117823Z exit1. Worker14844 start04:52:37.3521642Z exits0
in7.1899274s. Provider28264 start04:52:44.9315076Z exits1120 in6.5007876s.
Logs and process records are in build/M0-T447/S3/full-link-r1.

Source-traced missing edges and staged corrections:
- Existing worker leave_for_callout/reenter_after_callout implementations
  lacked export/import-name wiring. Add them to the parent DEF generation
  and provider COFF aliases; do not duplicate the execution-domain methods.
- Original list/default bodies reference RtlUnicodeToMultiByteSize and
  RtlIsNameLegalDOS8Dot3. Both exports were confirmed in the installed x86
  ntdll export table. Add them to the existing native import/alias table;
  no copied string or DOS-name algorithm.
- The modal style entry called DialogBoxIndirectParamAorW, but only clres's
  bitmap selection was built. Select the already admitted original template
  slice separately and give it a private body name, avoiding recursion into
  the public style redirect. Its existing scoped modal entry remains owner.

Still-open group: MapDialogRect and four DlgDir* functions collide with
USER32 imports. Selected original MVDM callers still import native names;
merely renaming original definitions would leave those callers bypassing
the recovered behavior. Complete the public entry/task/context routing and
private body naming together before the next build. Do not use force-multiple,
drop original bodies, or conceal the conflict through linker ordering.
Import/modal changes are staged, not yet verified by a second link. Published
P16 remains unchanged; no original guest input was built or modified.

### Full link r2, public entries and real application exit failure

ADAPTER-WOW-063 adds wow_dialog_api_bindings.h and extends the existing
dialog finalization entry, not a second execution context. The five public
MapDialogRect/ANSI directory calls select private original bodies for owned
state and native APIs for unowned host state. Both branches retain execution
and error boundaries. The control facade privately names nine A/W original
bodies; MVDM redirects the five reached ANSI entries. No additional original
mirror-body diff, opaque native cast or ignored duplicate symbol is needed.

full-link-r2 outer39396 start2026-10-17T04:58:32.7441075Z exits0. Worker8060
exits0 in7.6003044s; provider39916 exits0 in13.2593233s. Exact commands,
process times and SHA256 are retained under build/M0-T447/S3/full-link-r2.
dialog-public-entry-r44 outer21456 start04:58:42.9663298Z exits0;
build15144 starts04:58:44.7562572Z, exits0 in90.3720187s. All29 cases pass.
Native HWND probe32428 reports64 iterations/256 callbacks/32 terminal/0 errors;
the finalization fixture verifies five original API routes plus failure/unwind.
Its original API bodies are explicit doubles, not actual directory behavior.

current-apps-r1 outer38652 starts05:03:56.7500218Z and exits2. Package is
expanded from assets/winnt.zip, with hash-checked release13 and only the two
current linked images replaced. No live-installation wholesale copy or guest
change. WINMINE flag/counter/reset/open and SOL deal/stock/close assertions
pass; both launchers return0x42b instead of0. r2 repeats with existing traces:
both reach registered-task-cleanup failure5023 after WM_NCDESTROY. This is
failed integration evidence and a delivery blocker, not an allowed S2 risk.
The finite next diagnosis is task/module cleanup and retained WND/CLS release;
do not weaken the normal-completion assertions or publish this candidate.

ADAPTER-WOW-063/parent page-domain release correction: r3's existing trace
plus failure-only diagnostics proves native UnregisterClass succeeded for
both applications, cWndReferenceCount=0, storage references=2; only the
subsequent copied CLS release fails. Original class.c DestroyClass requires
zero live window associations, not zero adapter snapshot borrowers. S3's
retained graph deliberately keeps old WND aliases through nested callbacks.
Immediate physical CLS deallocation therefore cannot implement logical class
removal. Keep the original cleanup/list removal and native unregister; hand
the copied allocation to the existing page-domain collector until retired WND
readers drain. A release bit belongs in its existing allocation record, not a
new registry. Reject release while any published WND still names the class,
and reject new WND publication using a released class. Existing strict physical
retire API keeps its negative contract. The new ownership-transfer entry is
parent-private C ABI; no guest layout or mirror diff. Validate live rejection,
retained read safety, no resurrection, final reclamation and real app exit.

full-link-r4 outer41664 start2026-10-17T05:12:05.8982308Z exits0;
worker6136/provider33680 exit0 in7.1958775s/6.7077369s. current-apps-r4
outer30392 start05:13:30.6720183Z exits0. Both original gameplay probes pass
without relaxed assertions and both launchers complete0. Trace shows exactly
the target task's normal retirement complete/status0. r1--r3 failed runs remain
retained; r4 is positive product evidence for this bounded correction only.

current-dialog-r1 outer37012 start05:15:08.0888200Z exits1. Launcher exit0
does not make this pass: guest emits phase0 only. Initial attribution to the
BUTTON owner was incorrect: lifetime.txt lines212--222 prove HWND007B0AC8
NCCREATE=1/CREATE=0 and WindowCreate claimed=1/result007B0AC8. Subsequent
HWND00EC0ED2 fails NCCREATE=0/error1168 without an explicit WindowCreate
record. The probe did not checkpoint phase1 before FindText, so phase0 on
disk cannot identify which creation failed. This is not the owner-waived S2
anomaly; preserve the failed run and its corrected interpretation.

class-release-lifetime-r1 fails to compile the existing combined page fixture:
two unrelated helper bodies were both named verify_callback_borrow. Rename
the one-argument scope helper; add its missing c_sas_storedw declaration and
use a wide integer-resource cast for GetClassInfoExW. These are test repairs,
not product workarounds. r2 reruns the unchanged broad lifetime gate plus new
release assertions. r2 outer12624 start05:14:56.0956928Z exits1 at link:
the selected production closure now pulls RPC/BaseClient and original CRT
definitions also supplied by ccpu_host_fixture_seams/legacy_stdio_definitions;
VdmRedirLoaded is defined by both fixture nt_bop and provider wkfileio.
No force-multiple is permitted. New class-release assertions have not run;
repair this finite test-composition ownership conflict before claiming the
broader retained-reader gate. Actual WINMINE/SOL r4 remains separately valid.

### Implicit native creation is not recovered-control ownership

Source chain: original WCD32FindText -> WCD32FindReplaceText -> native
FindText(lpfr), with original WCD32FindReplaceDialogProc handling guest
conversion/callback/resources. Its native dialog creation reaches the generic
CBT enrollment, not the explicit original dlgbegin/control creator. The
generic path selected the imported canonical recovered dialog procedure but
never supplied original creation facts; its first ensure_state therefore
fails ERROR_NOT_FOUND. This agrees with r1's successful explicit owner and
subsequent unclaimed creation failure; runtime confirmation is pending.

ADAPTER-WOW-063 correction distinguishes creator ownership at the existing
enroll_created_window call, not by filename/class spelling. Explicit original
creators retain recovered selection and their captured state. Generic native
standard creation retains the actual native per-window procedure while still
enrolling identity, task ownership and rollback. Guest class procedures remain
guest procedures. Original combo children and dlgbegin children already use
the explicit version/encoding-bearing creator and remain recovered; native
COMMDLG is not a replacement for those algorithms. This refines the earlier
"internal child" statement: original internal creators are covered, arbitrary
native creators cannot inherit ownership merely from an enrolled parent.

The enrollment fixture now checks the native per-window target and rollback
restoration with and without held execution ownership. The guest probe writes
phase1 immediately after owner success; no existing assertion is relaxed.
No mirror/guest diff, hidden HWND or new persistent state. Group verification
must include real FindText hooks, repeated destruction, task exit and the
existing explicit guest app paths before claiming this correction verified.

Grouped result: full-link-r5 launcher34160 starts2026-10-17T05:26:27Z;
worker37396 starts05:26:54.9275730Z and exits0 in5.4243443s,
provider29308 starts05:27:00.6753539Z and exits0 in5.6538182s. Initial sandbox
launcher5260 disappeared without creating a build directory or output; its
empty outer logs remain and are not build evidence. The authorized second
launcher owns the actual build. Exact input/output hashes are retained there.

native-creator-group-r1 launcher42288 starts05:27:25Z; apps child41344
starts05:27:26.2483914Z and exits0, both unchanged gameplay/normal-exit
assertions pass. Dialog child35248 starts05:27:54.4862827Z and exits0;
current-dialog-r2 passes16 ordinary destruction rounds plus the seventeenth
dialog's task-exit cleanup,17 hooks, original unlock/free callbacks before
exact-once task retirement. Probe phase1 now independently confirms owner
creation. This is real unchanged-guest product evidence, not a mocked return.
The group aggregate exits1 solely because governance child24924 reports
CURRENT over32KiB; shorten duplicated status chronology and rerun that gate.
The updated native enrollment fixture remains unrun because the separately
recorded composite fixture link conflicts persist. No release/commit claim.

### Composite lifecycle fixture symbol ownership repair

Source audit of verify-wow-task-lifecycle identifies three different collision
classes, not a product linker defect. Its selected real RPC core/style and
BaseClient objects conflict with the CPU-only ccpu_host_fixture_seams. Keep
the real owners; replace that fixture object with an absent-environment leaf
and a fail-closed common_rpc_bind_local boundary. That function is the sole
definition of common/rpc/local_binding.c, so an explicit fixture definition
prevents extracting the archive member without duplicate-symbol suppression.
Unexpected service connection exits120 before contacting any real BaseSrv.

Original nt_bop.c and wkfileio.c each define VdmRedirLoaded in separate product
images. They are independent states, not competing owners of one state.
The combined fixture recompiles the selected provider wkfileio translation
unit with only a test-local symbol spelling; its algorithms remain unchanged
and both cells remain separate. This is test composition, not a mirror diff
or a new product owner. The runner verifies exactly one replaced object.

The production crt_compat supplies sprintf/sscanf/_swprintf, while selected
original fprt supplies printf/fprintf interposition. Remove the fixture's
additional legacy_stdio_definitions archive rather than override those owners;
retain product oldnames and UCRT libraries. No FORCE:MULTIPLE, successful stub,
guest change, assertion removal or production binary change. Run the full
window-borrow gate (including class-release) and native-creation after the
combined composition rebuild; until then these repairs are unverified.

Composite-lifetime-r3 outer40348 starts2026-10-17T05:33:11Z; window-borrow
child40192 starts05:33:12.3371606Z, exits1. The full selected executable links
without duplicate-symbol suppression. Remaining LNK4217 notices identify
the known DLL-data-to-static aliases, not multiple definitions. The real CCPU
class-release row now executes and reports errors=0: live rejection, retained
class access, no resurrection and final free-byte recovery all pass. Original
menu resource, mutation/return, rollback and observation rows also execute.
These partial positive rows do NOT make the aggregate gate pass.

The aggregate reports35 failed assertions. Source inspection classifies24
as the six recovered classes' A/W canonical-procedure assertions still
expecting modern native procedure addresses (lines2484/2486/2489/2490 at this
run). The production classifier deliberately selects recovered entries.
Creation/reuse/terminal rows also construct only bare task/process structs
and previously used native BUTTON/COMBO/dialog implementations; recovered
creators now require the actual runtime/version/registered callback context.
Observed-words rows call runtime validation without that context. Reconcile
these fixture prerequisites against the production entry contracts, preserving
original-control behavior assertions; do not replace BUTTON with STATIC,
restore native controls or drop failed rows just to make the fixture pass.
The exact35 failures remain in window-borrow/stderr.txt. Native-creation was
not run after the first-case failure. The verified production app/dialog rows
remain separate evidence, not substitutes for this gate.

### Reconcile fixture contracts before another build

Staged fixture-only corrections: canonical matching now supplies the selected
recovered A/W entry and checks the canonical ANSI result, while adding a
negative that an unrelated modern control address must not classify as that
original owner. Non-recovered classes still use the native registration. The
observed-words test now binds/unbinds the real runtime for public validation;
it needs no fabricated TDB because it exercises identity, not task callbacks.
These changes have not been rerun and are not passing assertions yet.

Remaining creation group has a finite setup closure: memory/word-break and
version callbacks through UserRegisterWowHandlers, one runtime/class context,
client-info and task execution owner, and cleanup class atoms belonging to
that same lifecycle process. Existing bare process/thread locals lack these
required inputs. Use original W32 callbacks and native-module branches where
appropriate; no success-only callback replacements. The terminal subcase must
reuse that context rather than attempt a nested runtime bind.

The Combo subcase also uses modern GetComboBoxInfo to inspect native private
state. Original comboini.c creates CBLISTBOXID1000 and CBEDITID1001, then may
reparent the list; original CBOX.spwndEdit/spwndList remains the authoritative
relationship. Fixture inspection must retain/read that original state and
verify actual child identity, item mutation, veto and teardown. Do not use a
modern private-state query as an oracle for an original-owned control, omit
the list because it was reparented, or replace the test with a native Combo.
Finish this whole setup/assertion group before the next compilation.

The creation fixture now binds one lifecycle process/runtime/task/client-info
and class-version context for managed reuse, original BUTTON/Combo creation
and terminal dialog cleanup. It registers the actual W32 memory, version,
dialog-init and word-break callbacks, restoring the previous table afterward.
Native-module branches are supplemental boundary coverage, not guest InitTask
proof. Combo child inspection reads retained original CBOX state; actual
child messages, veto, object retirement and terminal assertions remain.

Composite-lifetime-r4 (outer7920, child25756 at05:44:11.0169613Z on
2026-10-17) fails compilation: the test confused APFN pointer aliases with
PFN table values, redundantly included the unguarded original ABI header,
and lacked lookaside declarations. r5 (outer17708) corrected table values
but still fails the include/calling-convention issues. Neither executed
the fixture. r6 (outer40748 at05:46:02Z) corrects those remaining test-only
compile boundaries. Child39424 starts05:46:03.4165995Z; the linked fixture
executes but exits-1073740768 (C0000420). Eight canonical-class assertions
fail at lines2580/2582/2584/2585 (two class entries). Managed reuse now
reports errors=0 for both managed creation and null-class retained generations;
execution advances through original BUTTON/Combo to terminal dialog cleanup.
It stops after reporting the recovered native BUTTON cleanup target. Do not
claim complete Combo/terminal acceptance without the final aggregate markers.
The runner's missing-native-mutation-marker exception is downstream of this
early assertion termination, not evidence of a native geometry mutation bug.
Native-creation's separate case was not run after the first-case failure.
Remaining finite investigation is identify the two ambiguous/mismatched class
entries and the exact terminal restoration rejection using existing traces;
retain the strict assertions and the real original control owners.
Composite-governance-r3 outer16892 exits0. No publication or commit performed.
No product source, mirror exception or guest image changes in this group.

### Composite r6 source/identity and terminal-scope diagnosis

The linked map proves both original list bodies and their wrappers were folded:
ListBoxWndProcA/ComboListBoxWndProcA both00437fc0, W both00438030;
list_entryA/combo_list_entryA both004286f0, W both00428710 (preferred
addresses). Thus the eight assertions are the two list FNIDs, not an unknown
class failure. match_native intentionally rejects non-unique procedure
identity. Keep that rejection, testing unchanged outputs/error for a folded
pair; retain unique A/W assertions when the selected addresses differ. Do not
disable product ICF or guess a FNID from a non-unique address. The ordinary
class/window identity remains its retained class/FNID, not the folded address.

Production wow_user_task_prepare_exit keeps resource_cleanup_live TRUE across
destroy_terminal_windows and clears it in finally. The fixture instead cleared
that flag to test rejection, then destroyed the recovered BUTTON without
restoring the permitted resource scope. Existing-r6 trace (outer6588,
fixture9636 started2026-10-17T05:49:24.1764680Z) exitsC0000420 and records
stage=finalize-original-control status=5023 immediately after child WM_NCDESTROY.
The source rejects precisely that missing scope. Restore the scope only around
native destruction, with finally; application callback denial and suppression
assertions remain. No product relaxation, guest execution or hidden HWND added.

Composite-r7 outer38584 / child28928 starts05:50:14.7099248Z with both
test corrections, existing lifetime tracing and complete group assertions.
Its outcome is pending; the separate standard-class fixture has the same
folded-address assertion correction but is not included in this runner.

r7 completes window-borrow with fixture exit1, no fail-fast. Native-class
projection, BUTTON/Combo creation/veto, normal and terminal dialog destruction,
callback denial, allocation recovery, observed-window identity, returned-window
and mutation groups report errors=0. One assertion remains: line653, creating
the dialog-cache witness from verify_callback_scope. That generic callback
fixture deliberately binds a bare process/object table, not an initialized
original dialog lifecycle/client/class context; wow_dialog_capture_context
rejects it with5023. Move the unchanged cache/nested-destroy assertions to the
already initialized original-creation scope, passing the real TEB and its
object table. This also retains the existing --dialog-cache-failure injection
inside its callback. Do not fabricate success in dialog_capture_context or
replace original InternalCreateDialog with a host dialog creator.

r8 outer39952 starts05:52:52Z; the group now includes window-borrow,
native-creation and standard-class. Results pending. No product/runtime edits
or additional mirror diff in these fixture corrections.

r8 window-borrow now reports errors=0 for the complete callback group,
including WOW_DIALOG_CACHE_CCPU and WOW_WINDOW_BORROW_CCPU. Fixture exits1
at the later host-storage-domain-exit stage. That historical witness binds
its own disposal storage onto the desktop WND; original control graph traversal
now creates that WND's real host storage. bind_window_host_storage correctly
refuses replacing an existing owner. Stage the same exactly-once domain-exit
assertion on a separate native observed HWND, kept live until after session
disposal. This is a test witness, not a hidden product window or a replacement
for desktop ownership. This last test adjustment has not yet been executed.
The native-creation/standard-class cases remain unrun because the group stops
on the first failed case. Do not claim the whole gate passed.

r9 outer30100 starts2026-10-17T05:55:29Z with the independent exit-storage
witness; the runner owns the same three-case sequence and900-second child
deadlines. Composite-governance-r4 outer41540 runs alongside it. Both are
pending; resume these exact jobs/logs rather than restart on observation loss.

r9 window-borrow child32096 exits0: WOW_PAGE_DOMAIN_LIFECYCLE_OK including
creation, callback, retired-storage recovery and domain shutdown. Governance
r4 exits0. Native-creation child556 starts05:56:47.5396439Z and exits1;
its common window-borrow prefix still passes. The added creation fixture
reports11 failures: eight counted-title length/content assertions (four
creator/name forms), three implicit-dialog procedure/rollback identity
assertions. It reports97 callbacks, nested=1, rejected=1, vetoed=1,
destroyed=13, table_empty=1. Those positives do not certify title or rollback.
Standard-class was not run after that failure.

Source boundaries under investigation are the counted native title service
through an enrolled ANSI gateway/custom callback and the opaque native
procedure saved/restored by rollback_enrollment. Standalone native-title
coverage uses a Unicode native class and does not prove the enrolled ANSI
chain. Native Get/SetWindowLong tokens cannot be assumed interchangeable
with fixed raw callback addresses; do not weaken these assertions before
checking actual values, encoding and behavior. Added default-on fixture
diagnostics retain all11 assertions and print the returned UTF16 title and
before/selected/returned procedure identities/encoding.

r10 outer27196 starts05:59:46Z; child21356 starts05:59:46.7874720Z.
Runner now continues independent cases after an ordinary failed exit (but
stops a timed-out process tree), preserving a nonzero aggregate. This avoids
hiding standard-class coverage behind the earlier failure. No new product,
mirror or guest edit. Results pending, published P16 unchanged.

r10 is terminal: aggregate2, window-borrow0, native-creation1,
standard-class1 (link failure, no runtime). Native-creation child8972 starts
06:01:00.4682148Z. Each counted-title case returns length3 and UTF16
0041,0042,0058,0000 (ABX), not the requested two-character AB. All four
windows report IsWindowUnicode=0. Do not normalize away the extra character
in the assertion. Inspect counted descriptor capture, native ANSI class
delivery, original gateway dispatch and DefWindowProc behavior as one chain.

Implicit #32770 diagnostics: held0 initial/before/selected771ba1c0,
SetWindowLong's previous777c56d0, rollback getter777c56d0; held1 begins and
selects777c56d0. Both are ordinary addresses, not taggedFFFF CPDs. The earlier
opaque-token hypothesis is therefore NOT an explanation for this run. Both
iterations remain ANSI. Trace where the getter versus setter result diverges
before changing rollback or accepting procedure identity drift. All three
assertions stay failing and no product fix has been selected.

Standard-class child40600 starts06:02:20.3867039Z. Link rejects seven parent
page-domain symbols present in both parent-stub.obj and the real selected
softpc-fixture-bindings archive: publish_menu(s), publish_named_class,
release_class, retire_class/menu, set_expected_windows_version. The old
standard-class fixture also mixes a native checkbox HWND with the recovered
control target without the original creation context. Reconcile its whole
owner/dependency setup against the real CCPU fixture, including class cleanup,
not force-multiple, native-control substitution or deletion of assertions.

No background jobs from r9/r10 remain pending. The two boundary issues and
standard-class fixture composition are the finite remaining group work;
other existing S3 delivery and semantic gates remain unchanged. r9/r10
window-borrow positives prove their own scope, not whole S3 completion.

### Original create-message string contract and fixture ownership repair

Pinned OpenNT kernel/createw.c lines260--262 carries the original title
descriptor into CREATESTRUCTEX. inc/ntcb.h INLPCREATESTRUCT selects the
encoding conversion for the receiver; kernel/ssend.c CALC_SIZE_IN reserves
Length plus a WCHAR, and LARGECOPYSTRINGLPWSTRA (line387) captures
Length/sizeof(WCHAR)+1 characters. CaptureAnsiCallbackData converts precisely
that input including the expected terminator; it does not invent termination
at the descriptor's Length. Original dlgbegin.c calls RtlInitLargeUnicodeString
for ordinary titles; its distinct ordinal branch retains the four-byte ordinal.
Thus the fixture's {'A','B','X','Y',0}, Length4 is not a correctly terminated
ordinary producer input. Its AB-only creation-callback assertion was invalid;
the direct Unicode native-leaf storage probe does not establish that assertion
for ANSI callback capture. Change to {'A','B',0,'X','Y',0} with the same Length4
and spare trailing capacity. Preserve all title, encoding and captured-fact
assertions. Do not add a product truncation/copy policy to satisfy malformed
fixture input; original bodies and native descriptor adaptation are unchanged.

Standard-class now uses the same real CCPU session driver as native-creation,
without parent-stub.obj. Its native checkbox row previously attached a
recovered original BUTTON procedure to modern native private state. Keep that
row explicitly native: attach the immutable native class target and verify its
forwarding/check state across logical class changes. Original cleanup target
assertions remain; the real original BUTTON creation/lifetime group in the
driver remains separate and mandatory. This is not a runtime fallback to
native controls. No assertion or behavior coverage is removed.

Native dialog rollback still fails strict address checks. Add a native-only
get/set/restore control on an unenrolled child, then recreate the child before
the unchanged adapter assertions. It records API return/class/export identities
without making token/address assumptions or altering product restoration.

r11 outer32272 starts2026-10-17T06:09:40Z with these three fixture changes.
The full three-case group is pending. No product/mirror/guest changes or release.

### Composite r11 outcome and native-control comparison

r11 completed with aggregate exit2: window-borrow passed, native-creation
failed three raw-address assertions, standard-class failed49 assertions.
All four correctly terminated title forms produced AB. Native-only dialog
subclass/restore changed the first getter address771ba1c0 to777c56d0;
the latter equals the setter's previous value, DefDlgProcA and the native
class procedure. Project rollback exhibited precisely the same transition.
The old equality to the initial getter is not a native restoration contract.

The revised fixture compares the selected target to the immediate getter,
the saved/restored target to the direct native-only control, and checks
unchanged encoding, removed project property and successful ANSI-set/Unicode-get
text behavior after each rollback. r12 outer38140, child12584, start
2026-10-17T06:15:58Z, exit0:97 callbacks, nested/rejected/vetoed paths,
13 destructions, empty table, and complete page-domain lifecycle pass.
Evidence: build/M0-T447/S3/composite-lifetime-r12. No runtime policy changed.

Standard-class source review identifies a separate composite setup defect:
original cleanup.c caches spfnwp on its first invocation, even menu-only
cleanup. The driver supplied a zeroed cleanup_shared table before its later
original-control cases. Populate it with wow_standard_classes_bind_cleanup
before that first call in the full-control fixture, as the production owner
does. Do not reset original static state or relax restoration assertions.
r13 reruns all three cases with that initialization; result pending.
This correction adds no production, mirror or guest diff. Other fixed S3
CPD, real-guest and coherent-release gates remain open.

r13 outer6744 started2026-10-17T06:17:52Z and completed with exit1.
Window-borrow and native-creation both pass. Standard-class failures fall
from49 to one, at the private BUTTON restoration assertion. Original
cleanup.c traverses private and public lists with the same atom-indexed
restore table; the production table now selects the recovered wide BUTTON
procedure. The private row alone still expected modern USER's native BUTTON.
Align it with the unchanged original table rule, retaining exact procedure
identity, cleared guest words and native-forwarder behavior checks. No product
change is needed for either fixture defect. r14 outer42924 starts
2026-10-17T06:22:11Z to reverify standard-class; result pending.

r14 completed exit0 (child2436): WOW_STANDARD_CLEANUP errors=0 restored=9,
private cleanup, class resolution/storage/precedence/retirement all errors=0,
and WOW_PAGE_DOMAIN_LIFECYCLE_OK. Together with r13's two passing cases this
completes the three-case composite fixture group after the source-proven
fixture corrections. The last edit affects only the standard-class assertion;
r13's window/native cases have unchanged inputs. This is supplemental real
CCPU/native-object lifecycle evidence, not proof of every guest control or
the unresolved incoming class CPD contract. Published P16 is unchanged.

### Registration selector/tag collision: complete consumer inventory

Source-first continuation after composite verification; no new runtime run.
Selected wuclass.c::WU32RegisterClass preserves the guest procedure in WC,
sets WNDPROC_WOW, and clears WOWCLASS_VIRTUAL_NOT_BIT31 only when the original
selector already had bit31. Thus 7fff:0b98 maps to ffff0b98 while
ffff:0b98 maps to fffb0b98. This is an algebraic producer fact, not evidence
that the tested retail applications allocate either selector. Do not infer
allocator reachability or a demonstrated retail crash from this calculation.

Actual selected registration chain: WU32RegisterClass -> pfnOut's registered
register callback -> wow_class_client_register -> original RegisterClassWOWA /
RegisterClassExWOWA -> wow_class_client_publish -> original _RegisterClassEx
-> publish_native -> wow_class_words_register_target / publish_guest.
Search of selected production sources finds the registration bridge as the
direct project caller; the native query-to-guest thunk does not unthunk here.

Original kernel/class.c::_RegisterClassEx first validates ISCPDTAG using its
owner; successful validation replaces the value, failed validation leaves
the original value for InternalRegisterClassEx. Current project consumers:

| Boundary | Current ffff handling | Required group review |
| --- | --- | --- |
| wow_class_client_publish | Rejects if native resolver absent | Preserve original identity distinction, not shape-only rejection |
| wow_class_words_register_target | Rejects every ffff target | Cannot accept guest collision until downstream ownership is explicit |
| wow_class_words_publish_guest | Suppresses every ffff procedure | Guest projection must agree with authoritative target kind |
| wow_class_words_exchange_procedure | Stores every ffff as native_callproc | Setter producer/restore semantics must agree with registration |
| wow_window_words_attach_target | Wraps every ffff in native forwarding | Do not send encoded guest address to CallWindowProc |
| Original DispatchClientMessage | WOW high bit selects original callback | Supply the correctly classified target; retain original dispatch body |

Existing callback presence alone is insufficient evidence of guest ownership:
imported native standard classes also retain the WOW callback for later
subclassing. Therefore changing the test to ignore ffff, treating all such
values as guest, or enabling only the first registration check would be unsafe.
No runtime workaround, native-token decoder, helper window or guest change
was introduced. The class-client encoding fixture now includes both boundary
selectors with exact target/decode assertions; these are staged requirements,
not passing tests. They expose the known rejection by code inspection. Keep
the generic native CPD path and this collision in the same finite group3;
source-proven native ownership and invalid-token behavior remain to implement
before grouped compilation/acceptance.

### Class address owner: source conclusion and bounded native evidence

Read pinned kernel/class.c::InternalRegisterClassEx (lines211 onward) after
the wrapper: it copies COMMON_WNDCLASS including the retained procedure,
optionally maps known client stubs to server procedures, and does not reject
an unresolved ISCPDTAG merely by its bits. Thus the earlier wrapper's
validate-or-retain branch is not superseded by another shape-only check.
Selected original DispatchClientMessage sends a retained WOW high-bit
procedure through pfnWowWndProcEx. Registration and downstream identity must
be corrected together; unblocking only the wrapper remains unsafe.

New research-only wow_class_address_owner_probe.c separates this address-only
contract from the prior rejected full dialog decoder. A source window creates
a real A-to-W or W-to-A native token. The normalization operation itself only
registers/queries/unregisters a class and creates no HWND. It checks32 live
normalizations per source (16 through each registration encoding), duplicate
registration rejection, and absent registration after release. It then records
the same token after source window/class retirement, plus three unowned input
patterns. No unowned or expired procedure is invoked. Old failed dialog
encoding evidence and its unchanged probe remain intact.

Background class-address-r1: supervisor24036 at2026-10-17T06:30:25Z;
Ninja26364 at06:30:26.9447252Z, exit0; probe15144 at06:30:36.2692657Z,
exit0, CLASS_ADDRESS_OWNER failures=0. Incremental x86 /MT /O2 /Gz build
selects only class-address-research. Source/executable hashes, commands,
stdout/stderr and terminal codes are in build/M0-T447/S3/class-address-r1.

Observed: all64 live normalizations return the expected actual procedure.
For retired tokens, both A and W registration/query preserve the original
ffff token unchanged. Inputs ffff0b98, ffff0000 and ffffffff likewise register
and query unchanged on this host. Every duplicate rejects with
ERROR_CLASS_ALREADY_EXISTS and every owned class unregisters. These facts
support the same address-only validate-or-retain distinction as the original
class wrapper, without reading private USER memory. They do not guarantee
the behavior of every host version or prove a production lifetime boundary.

Implementation constraint: any selected binding must return the native
owner's resolved value only within the calling operation, distinguish resource
failure from an unrecognized token, preserve last-error and exception unwind,
and retire its registration before returning. No global token cache, native
encoding inference, helper HWND or guest patch. Retained class/WND targets
must distinguish resolved native calls from unresolved WOW-tagged inputs;
callback presence and ffff shape alone are both insufficient. This research
does not yet install that runtime binding or close group3. It resolves the
previously untested native validation/retirement question for the measured host.

### ADAPTER-WOW-071 integrated identity group staged

Runtime implementation follows the original validate-or-retain branch, without
changing a mirror. wow_native_class_resolve_procedure in the existing class
owner registers a call-scoped native class (no HWND), queries its target and
unregisters in finally. The private name uses current thread and live call
storage identity; a collision fails and never unregisters another owner's
class. Output changes only after successful query and release. Successful
calls preserve last-error; resource/query/cleanup failures block target mutation.
There is no global cache, private-layout decoder or temporary file.

Original _RegisterClassEx still selects validation and keeps unrecognized
input. The adapter supplies a call-local resolution record and prevents native
publication if the resolver encounters a resource failure. A NULL optional
fixture override now selects this native implementation in production. The
class storage projects WOW identity from its existing authoritative procedure
(native CPDs are already represented behind the existing native forwarder).
The transient target snapshot carries wow_encoded only to disambiguate ffff
at attachment; no persistent second target-kind owner is added. Guest class
projection no longer discards the legitimate encoded7fff selector.

Class/WND setters and previous-procedure dispatch validate possible native
tokens before classifying them. Existing recognized-token CallWindowProc
conversion remains unchanged; unresolved WOW values remain in original WOW
dispatch. Unowned native-window setter fallbacks reject unresolved WOW values
rather than installing them as raw host callbacks. Native implicit creators
explicitly clear the snapshot WOW designation when selecting a native target.
Direct raw-ffff dispatch requires that exact retained WOW binding; unrelated
opaque tokens cannot bypass native forwarding.

Tests are staged in the native-creation composite: real native token resolution,
original class-client registration of both recognized and unresolved inputs,
7fff guest-target creation/dispatch, class and WND native-token round trips,
and final class/window teardown. The original class-client encoding cases
retain low/high selectors and add7fff/ffff boundaries. Its old failed-validation
rejection assertions are replaced by original validate-or-retain expectations;
known-CPD resolution remains asserted. The class-procedure fixture retains
gateway rejection and now checks unresolved-WOW setter/restore behavior.

Source sweep additionally finds the same shape-only shortcut in the registered
CallWindowProc bridge and unowned private-access fallbacks; both are included
in this group, not deferred. No guest or original-mirror changes. All preexisting
uncommitted changes remain. Background supervisor36428 started
2026-10-17T06:37:24Z, sequential full-link-r6 then composite-lifetime-r15.
Results pending; existing research positives are not integration acceptance.

### Integrated run outcome and role-review correction

full-link-r6 succeeds: worker10436 exit0 elapsed5.0675685s; provider23544
exit0 elapsed7.2361385s. Full header dependency closure rebuilt88 provider
steps; output hashes are retained by the runner. Composite r15 window-borrow
and standard-class pass. Native-creation runner3776 starts
2026-10-17T06:39:20.0479494Z; the fixture reports
WOW_CLASS_TOKEN_IDENTITY errors=0 callbacks=10 and native-creation errors=0,
but fails the subsequent host-storage-domain-exit stage. Thus native-creation
and supervisor36428 return1, not a passing group. Standard runner35864 starts
06:40:58.2395129Z and passes9 restored classes. No process remains pending.

Source review during that run finds a retained preexisting class-setter
divergence: kernel/class.c::_SetClassData (1275 onward) stores pfnClientPrevious
and the setter's ANSI flag; unlike kernel/getset.c GWL_WNDPROC it does not
recover wType. Keeping a CPD forwarding token in class storage would retain
the wrong conversion rule and the source object's lifetime dependency. After
the group terminates, replace that class-only forwarding policy with the
resolved address plus CSF_ANSIPROC, retaining original setters' existing-WND
independence. Update exact address and ANSI-text assertions; keep the separate
WND conversion tests. This correction is staged but not yet compiled/tested.

The terminal fixture uses the test-only observe_window entry; production
has no calls to it. That entry requires a matching existing slot generation,
unlike full graph reconciliation. Added native source windows may expose a
stale-slot assumption in its final storage witness, but this is not yet proven.
Split observe/borrow/bind/release stage reporting without removing any assertion
or changing product retirement. Do not label this failure fixture-only yet.

Finite group3 follow-up remains: validate the corrected class setter; resolve
the terminal storage failure; and finish native WND-token retention/encoding
lifetime. The address-only helper cannot recover WND encoding. r1 proves
source-token expiry, so existing WND forwarding must not be described as
complete source-independent restoration. No helper HWND or new compatibility
waiver is selected. The staged candidate is neither published nor delivered.

### Class identity compilation and destination lifetime research

class-identity-group-r2 (supervisor27464, start2026-10-17T06:45:34Z)
terminates1: full-link-r7 worker passes but provider compilation finds
CSF_ANSIPROC unavailable in wow_class_words_binding.c. Composite r16 never
starts. The constant already exists under the original-class-client-only
selector; move that identical original user.h value0x00000002 to the shared
class metadata declaration, removing the client-local duplicate. No mirror
change or new private state. class-identity-group-r3 supervisor34332 starts
2026-10-17T06:50:14Z; full-link-r8 passes and composite-lifetime-r17 begins.
Results remain pending until its terminal record; do not reuse r15 as a pass.

Original kernel/getset.c::GetCPD stores WND/DIALOG conversion records on the
actual object's class. This motivates research on actual destination objects,
not another auxiliary HWND. class-address-r2 supervisor33332 starts
2026-10-17T06:51:23Z; probe12464 starts06:51:33.1213046Z and exits0.
The retained native-only probe covers four source/destination ANSI/Unicode
combinations, each with two distinct destination classes. It temporarily sets
each real destination WNDPROC from the incoming native token, obtains its A
view, then restores the prior W view and IsWindowUnicode value. No window
procedure callback occurs during those transactions. It destroys/unregisters
the source, invokes both retained destination procedures with WM_SETTEXT,
verifies exactly one callback and correct A/W text for each, and retires the
first destination class before invoking the second. All four cases pass;
the earlier64 address-resolution and invalid/retired observations also remain.
Raw inputs/outputs, build warnings, process identities and hashes remain under
build/M0-T447/S3/class-address-r2. This is research, not product acceptance.

Reviewer limitation: this demonstrates native lifetime and restoration on the
measured host, not atomic invisibility of the transient real WNDPROC to another
thread, allocation-failure restoration, or integration with the native Hook/
NTW16S subclass chain. Do not install this transaction in production merely
because the probe passes. No helper HWND, private-token decoding, guest edit,
new compatibility waiver or publication is introduced by this research.

### Exit-witness failure attribution and production-path correction

Composite r17 terminates1, with window-borrow and standard-class passing.
Native-creation again passes all identity/creation assertions, then records
new HWND00d9084c versus retained00d8084c and fails specifically at
host-storage-domain-exit-observe. This establishes a real low-slot reuse with
a different generation, not a disposal assertion failure. The fixture called
observe_window directly; that low-level entry deliberately accepts only an
empty or same-generation slot. No production source calls that test helper.
Production wow_user_window_project_graph already drops dead observations,
captures required full identities and commits generation replacement through
wow_window_words_project_observed. The exit witness now uses that path and
asserts the committed full identity before borrowing/binding its disposal
cookie. All existing no-premature-disposal and exactly-once-domain-disposal
assertions remain. No product lifetime patch or forced disposal is added.

composite-lifetime-r18 supervisor33840 starts2026-10-17T06:55:35Z using the
unchanged full-link-r8 worker/provider. Window-borrow passes. Native-creation
also passes, now witnessing00c505b2 replacing00c405b2, an empty host-storage
slot and WOW_PAGE_DOMAIN_LIFECYCLE_OK. It retains identity errors0/callbacks10
and creation errors0/callbacks97,13 destructions and table_empty1. Standard
class subsequently passes9 resolutions/restorations and domain exit. All
three case exits and aggregate exit are0; r18 completes the composite group.

Class-address research receives a durable -ClassAddressResearch selector in
tests/observation/verify-wow-control-boundaries.ps1. Explicit ANSI atom casts
and cdecl main remove the probe's declaration warnings. class-address-r3 and
the durable-entry r4 pass the same assertions. r4 supervisor40484 starts
06:57:30Z; build28740 exits0 in0.1753547s, probe31212 exits0 in0.210714s.
Reproduce by starting that verifier in the background with
-ClassAddressResearch -EvidenceRoot build/M0-T447/S3/<fresh-run>.
Its input hashes, logs and existing300s/30s deadlines remain under that run.
These findings close the identified fixture precondition defect, not native
WND token retention, real-guest coverage, the remaining finite S3 groups or
coherent release. Mirror/guest diff increase in this correction: zero.

### Dialog initialization unwind audit (ADAPTER-WOW-063)

Source review of wow_dialog_host_initialize finds that its temporary native
DWL_DLGPROC clear is restored only on ordinary return. native_default has
its own execution/publication finally, but an exception from that operation
skips the outer slot restoration. Original InternalCreateDialog's admitted
host-initialization edge requires its preexisting native procedure to survive
failure. This is an adapter defect, not an original algorithm defect.

Retain original dlgmgr and the already approved actual-dialog slot owner.
Place restoration in finally; preserve entry last-error on success and the
unwinding last-error on exception. Any restore failure must invoke the existing
terminal adaptation-failure boundary rather than return or propagate an
unrelated exception with silently corrupted native state. Do not retry a
consumed native operation. Group tests must cover A/W saved procedures,
success, native-call exception, initial mutation failure, restore failure and
nested initialization, with no extra application callbacks or leaked scopes.
Use real subject HWNDs plus explicit test-only setter fault injection; this
supplements and does not replace the existing real native-owner probe.
No hidden runtime HWND, new state owner, original-body diff or guest change.

WND-procedure lifetime research is separate: current procedure exchange has
no owner-thread-only restriction. A three-call temporary GWL_WNDPROC swap
cannot be claimed atomic to concurrent same-process USER consumers merely
because sequential destination-lifetime tests pass. It is not selected as a
runtime fix. Retain the finite lifetime/encoding obligation in S3 group3.

Second-role caller review confirms ValidateDialogPwnd also serves the original
default-dialog handler, EndDialog and MapDialogRect, not only creation.
Therefore ordinary restoration failure cannot assume creation rollback will
destroy the HWND; it must also use terminal failure after the consumed clear.
Initial clear failure is still an ordinary FALSE with the original slot intact.
dialog-initialize-group-r1 (3044,2026-10-17T07:04:59Z) stops at compilation:
the new test-only /FI argument was split by PowerShell array concatenation.
Other selected objects build, but no test or downstream product link is run.
Parenthesize the single argument and retain the failed build evidence;
this is not a runtime test failure or a passing candidate.

dialog-initialize-group-r2 (35056,2026-10-17T07:07:48Z) passes all30
supplemental programs. Boundary build34340 exits0 after8.9193247s; the actual
native-owner probe38544 retains64 iterations/256 callbacks/32 terminal
callbacks with errors0. New initialization fixture14672 starts07:08:06.6829676Z,
exits0 after0.2549132s:14 A/W success, callout-exception, first-set rejection,
restore-failure, exception-plus-restore-failure, nested success and nested
then-exception cases. All restore the required native procedure/encoding or
reach the asserted fatal boundary, without extra application callbacks;
scope depth ends0. Setter-failure and terminal signals are explicit doubles,
not evidence of real USER allocation failure or worker process termination.
The separate real native-owner probe remains unmodified and uses actual USER.

full-link-r10 worker21296 exits0 in4.9779355s; provider4728 exits0 in4.7203783s.
Composite r20 then starts under3128 at07:08:42.0574134Z and terminates0.
All3 case exits are0: borrowed-window lifetimes,97 creation callbacks with13
destructions/table_empty1 and identity callbacks10, and9 standard-class
resolutions/restorations. All reach WOW_PAGE_DOMAIN_LIFECYCLE_OK, including
actual reuse00a60f24 to00a70f24 in the creation row. Parent group35056 also
terminates0. This is complete supplemental/composite validation of the bounded
initialization repair, not whole-S3 or real-guest acceptance. No guest or
original mirror change, and no release update.

The WND-token source audit rejects the temporary three-call swap as a final
solution: original xxxSetWindowData resolves/stores under its USER authority,
whereas a project SRW lock cannot serialize unrelated native USER consumers
between separate Set/Get/Set calls. In particular, blindly restoring the old
gateway could overwrite another same-process subclass update. Do not add a
lock and claim native atomicity. The passing destination probe proves only
lifetime and encoding for the sequential case; it is not this concurrency
proof. No new compatibility boundary is inferred from that research pass.

### WND token reachability and class-retention review

Source-only review of pinned OpenNT and the staged adapter rules out a
standard-control-only disposition. src/mvdm/wow32/wsubcls.c contains obsolete
OLD_SUBCLASS_STUFF followed by the active GetThunkWindowProc implementation.
The active implementation explicitly accepts an unknown class when the HWND
belongs to this process (its comment names OLE windows), stores lpfn32 in the
original TWPLIST and returns a guest-callable thunk. IsThunkWindowProc reads
that stored value without restricting restoration to the original HWND or
class. wuwind.c::WU32SetWindowLong passes the recovered native value to
SetWindowLong(GWL_WNDPROC). The ordinary native-window route is therefore
part of the retained original contract, not an invented new feature.

Original windows/core/ntuser/kernel/getset.c::xxxSetWindowData validates a
CPD, extracts pfnClientPrevious and its actual A/W direction, then stores the
resolved procedure and WFANSIPROC. GetCPD attaches subsequent WND conversion
records to the destination WND's pcls. kernel/class.c::UnlockAndFreeCPDs marks
and releases those records during native class destruction. The source does
not rely on the source class remaining registered after a successful setter.
This describes native CPD lifetime, not the lifetime of application code:
unloading the DLL containing a still-used procedure is not made safe by it.

The staged wow_window_procedure_exchange instead saves the incoming token in
native_callproc; class-address normalization only distinguishes native CPD
from encoded guest values. Keeping that source-owned token is insufficient.
wow_class_words_destroy_native calls native UnregisterClassA before detaching
the project projection. A project reference can keep the copied CLS alive,
but cannot keep native CPDs alive. Deferring native unregister would change
observable class-name reuse and failure behavior and is not selected.

Actual-source commands: bounded reads of wsubcls.c:330 onward and :541 onward,
wuwind.c:2431 onward, wow_window_words_binding.c:1209 onward,
wow_class_words_binding.c:499 onward, and pinned kernel/getset.c:539 onward
and :742 onward plus kernel/class.c:978 onward. These are static reachability
and ownership findings, not a new runtime failure or proof that a particular
guest currently uses this cross-class restoration path. Existing sequential
class-address-r4 results remain the limited runtime evidence.

Disposition: retain group3 as a finite S3 blocker. Neither source-class
pinning by adapter references nor a standard-class-only exclusion solves it.
Any replacement must prove source retirement, A/W conversion, independent
destination retirement, rollback and native concurrent subclass safety while
retaining the existing callback and WM_NCDESTROY ownership. No helper HWND,
new runtime implementation, mirror/guest diff, build or release in this
source-review step. The working plan now distinguishes the staged tested
class resolver from the still-open WND lifetime problem and from P16.

### ANSI-origin token producer audit and class-only lifetime research

The next source audit checks information supplied by actual producers instead
of guessing direction from an arbitrary CPD. All GetThunkWindowProc call sites
in selected MVDM are in wuclass.c and wuwind.c. They consume GetClassInfo,
GetClassLong, GetWindowLong or the old result of SetClassLong/SetWindowLong.
The production-cohort-x86 graph compiles those MVDM units as ANSI, distinct
from its Unicode original-control units. wow_user_private_access redirects
the ANSI APIs; its native query fallbacks use explicit A calls. The class
query binding also calls GetClassInfoExA. The original wsubcls active thunk
writer saves lpfn32 unchanged; no export definition exposing that writer was
found in the source sweep. wumsg.c consumes IsThunkWindowProc for calls but
does not produce a different native token.

Pinned ntuser/rtl/getset.c::GetWindowData creates CPD_ANSI_TO_UNICODE for an
ANSI query only when the underlying procedure is Unicode; client/classc.c
uses the same direction rule for class getters. Thus an actual A-query result
provides more information than an arbitrary supplied opaque token. This is
producer-specific evidence, not permission to infer Unicode for every valid
FFFF value. A completed implementation still needs to audit returned logical
values, dialog getters, inherited native procedure targets and every retained
setter path rather than silently narrowing the original setter contract.

Extend only the existing native research probe with
ansi_origin_class_lifetime. A real Unicode source produces an ANSI-query CPD.
Two separate Unicode class registrations consume it and expose independent
ANSI-query CPDs, with no HWND created for either carrier class. The real test
subject keeps its original ANSI WNDPROC and encoding throughout. Destroy the
source HWND and unregister its class, call both retained procedures using
ANSI WM_SETTEXT, and unregister the first carrier before calling the second.
Assert correct Unicode text and exactly one application callback per call.
This supplies a candidate native lifetime resource without a temporary
destination WNDPROC swap. It does not yet select a product storage owner.

class-address-r5 supervisor31600 starts2026-10-17T07:23:07Z and terminates1
before compilation: the invoked powershell.exe lacks Get-FileHash. Retain that
failed input-capture attempt. Re-run the same durable verifier with the current
pwsh executable as class-address-r6, supervisor39656 starting07:23:39Z.
Build7716 starts07:23:40.8889958Z, exits0 after8.7096954s; probe32788 starts
07:23:49.6170930Z and exits0 after0.310361s. Aggregate exit0, all earlier
research assertions retained, ANSI_ORIGIN_CLASS_LIFETIME reports
carrier_windows=0, subject_unchanged=1, failures=0. Compiler stderr is empty.
The verifier preserves source/build hashes and the actual300s/30s deadlines.

Reviewer boundary: this proves the known-ANSI-origin case on the measured
host, not arbitrary-token direction discovery. Product selection additionally
requires a finite class/CPD ownership design: saved previous-procedure values
must remain valid for the original class lifetime, not merely the latest
window target; replacement, unregister, borrowed native classes, in-flight
callbacks and allocation/cleanup failure must have explicit owners. Do not
retain registrations indefinitely, fake native class retention with project
references, or create a second behavior authority. This is a research-only
test change; production/mirror/guest diff increase is zero and P16 remains
published.

### Review correction: ANSI producer provenance is not the full setter domain

Second-role review of the proposed class-only carrier finds a remaining
contract mismatch before product selection. Pinned kernel/getset.c explicitly
changes bAnsi to the validated CPD's actual direction, irrespective of which
SetWindowLong entry was called. In wuwind.c::WU32SetWindowLong, the ordinary
guest-procedure branch also ORs WNDPROC_WOW into the guest value before the
same setter. A selector7fff value becomes FFFF-tagged. Original validation
can recognize that bit pattern as an existing native CPD; the current identity
audit deliberately preserves that validate-first behavior. Such an input
does not establish that its matching native CPD originated from an ANSI
getter. IsThunkWindowProc also recognizes the original thunk representation
without maintaining a separate provenance registry.

Consequently, ANSI-only native producers do not prove that every valid CPD
accepted by the original setter describes a Unicode procedure. Treating all
recognized CPDs that way would narrow the original contract and can select
the wrong conversion. This is a source-derived counterexample to the proposed
inference, not a claim that a particular tested application allocates a
colliding selector or that class-address-r6 failed. r6 still proves the
explicit known-origin case; it is not a complete runtime fix.

Class lifetime review also confirms the original storage boundary:
wow_window_words_attach_target transfers its acquired class reference into
the window binding; wow_class_words_destroy_native unregisters the actual
class before detaching the projection; final wow_class_words_release frees
that projection only after borrowers leave. Any conversion carrier must
preserve returned previous-procedure values across subsequent setters and
window destruction until the associated class lifetime ends. Releasing a
carrier on each WND target replacement or retaining every carrier forever is
not equivalent. Borrowed standard registrations use forget_native rather
than native UnregisterClass and need their own existing shutdown boundary.

No production carrier is selected. The next source-led design candidate is
to let native USER perform the actual procedure assignment, rather than
copying a token behind a permanently installed gateway. Before selecting it,
audit immutable previous-guest-procedure identity, native/guest callback
execution scopes, original-control finalization, WM_NCDESTROY retirement,
and NTW16S's outer subclass. Current wow_window_native_proc owns dispatch and
terminal cleanup; merely bypassing it is not a solution. Existing application
Hook conversion is not an automatic lifecycle observer and must not be
represented as one. This remains group3; no additional feature, compatibility
waiver, guest change, helper HWND or production diff is admitted by this note.

### Native-slot replacement obligations and current-product app comparison

Source inspection of wow_window_words_binding.c::dispatch_bound shows two
distinct responsibilities behind the current native entrance: guest callback
execution/borrow scopes and original control finalization plus identity
retirement on WM_NCDESTROY. Original kernel/createw.c::xxxFreeWindow sends
WM_NCDESTROY and then independently invokes WM_FINALDESTROY for an uncleaned
built-in control, even when it has been sub/superclassed. Replacing a WND's
actual procedure therefore cannot depend on an application calling its old
procedure for final cleanup. wow_hook_callback.c only converts hooks that
applications installed; it is not an automatic post-destruction owner.
Native-slot selection remains unimplemented until these dependencies and
immutable previous-guest-procedure identity are resolved together. Merely
moving the setter to native USER does not close the contract.

Supplement the already completed full-link-r10/composite-r20 group with real
applications, using the existing isolated desktop observer and original
assets/winnt.zip. current-apps-r6 supervisor8168 starts2026-10-17T07:30:36Z.
WINMINE passes flag/counter change, reset, open-cell change, WM_CLOSE and
run16 completion. SOL fails probe exit4 before gameplay: its owned visible
Solitaire window exists, client431x261, but find_game rejected any width
below500. This failure is not evidence of an absent window or a game crash.
The observer's forced failure cleanup is not normal application exit.

The retained SOL probe now identifies by exact owned PID/class/visibility,
then explicitly establishes the prior585x384 gameplay geometry if the initial
window is too small. It uses ordinary SetWindowPos on its isolated test HWND,
checks the resulting client dimensions, and retains every original pixel,
deal, stock-draw, close and launcher-completion assertion. No display-mode
change, physical desktop operation, guest patch or product change is made.
Failure to establish that geometry remains exit4; it is never a skipped pass.

current-apps-r7 supervisor42000 starts07:32:18Z against unchanged full-link-r10.
WINMINE again passes all actions and normal completion. SOL is identified,
but the requested585x384 client becomes431x281, so geometry setup fails
before gameplay. Aggregate exit1. A same-environment published-P16 negative
control follows in current-apps-r8 (14892,07:33:14Z). The test runner's
explicit -PublishedBaseline branch copies only all13 manifest-verified release
images; it does not overlay the S3 worker/provider. Original guest ZIP and
the same updated gameplay probes are used. P16 WINMINE passes, while P16 SOL
has the identical431x261 initial and431x281 requested-resize result; aggregate1.
Both packages' full input hashes and child process/start/timeout/exit records
remain in the respective run roots. No live application test remains.

Conclusion: this SOL geometry prerequisite failure also occurs on P16 and
does not establish an S3-introduced geometry regression. It does not prove
SOL gameplay or normal exit on the current candidate. Earlier r5 gameplay
evidence remains historical; current required SOL verification stays open.
Do not weaken the minimum layout/pixel assertions or describe failed probe
cleanup as an accepted exit. Latest current WINMINE gameplay/exit is proved.
Only the test prerequisite handling and audit records change in this step;
no runtime/mirror/guest change or publication.

### Native procedure authority: concrete scope boundary

Follow-up source review confirms W32Win16WndProcEx's optional PWW is an
optimization, not a different guest callback: wmdisp32.c resolves NULL through
GWL_WOWWORDS before conversion and uses the explicitly supplied old guest
procedure. Original usercli.h CALLPROC_WOWCHECK uses NULL, while ordinary
dispatch can supply the window's words. Any native-callable representation
of a saved guest procedure must therefore retain that procedure's identity;
a single gateway which re-reads the newest WND target cannot represent every
previous procedure. Original wsubcls.c's guest thunk generator is the reverse
direction and emits16-bit code; it is not a reusable native Win32 trampoline.
No executable-thunk allocator exists in the current wow32-dll or NTVDM WOW
adapter source sweep. The historical src.old path is absent in this checkout;
it supplies no recovered implementation here.

The style boundary is a second concrete dependency. ntw16s-dll/attachment.c
and controls.c install SetWindowSubclass/DefSubclassProc chains. The existing
wow_user_private_access.c GWL_WNDPROC getter/setter deliberately exposes the
logical WOW target below these native carriers and keeps the outer carriers
installed. Simply calling native SetWindowLong instead would replace the
actual top-level procedure, not update that logical leaf. Returning its old
value would expose a style/comctl32 carrier where the guest expects its old
procedure. A native-slot migration thus requires an explicit style-chain
contract as well as independent finalization and stable guest-call targets.

Decision boundary: do not select native-slot migration as a local setter edit.
Request explicit admission for coordinated WOW32 procedure-entry/lifetime and
NTW16S integration redesign within the still-open S3 dependency group. This
is a scope request, not a claim that the full replacement design is already
proved, and not a request to waive old-procedure, A/W, style or cleanup
semantics. Existing source evidence rules out the proposed smaller fixes;
it does not prove no other public mechanism could ever exist. Keep P16 and
all staged fixes, do not add native executable thunks, new lifecycle hooks or
style-chain behavior before that admission. No hidden HWND or guest edit is
proposed; no runtime modification or repeated runtime test in this step.

### Renewed research: terminal notification is not destruction authority

Owner requests further research before presenting an implementation proposal.
This admits bounded source/API investigation and isolated native probes, not
the previously proposed WOW32/NTW16S runtime migration. Preserve all staged
changes, published P16, guest bytes and the existing single-person dual role.

Inputs: pinned OpenNT kernel/createw.c::xxxFreeWindow, kernel/class.c::_RegisterClassEx,
kernel/getset.c::xxxSetWindowData; current wow_window_words_binding.c terminal
and procedure-exchange owners; wsubcls.c::GetThunkWindowProc/IsThunkWindowProc;
wumsg.c::WU32CallWindowProc; NTW16S attachment/control/frame subclass sites.
Public reference: [Microsoft hook ordering](https://learn.microsoft.com/en-us/windows/win32/winmsg/about-hooks).
WH_CALLWNDPROCRET observes a sent message after its destination procedure
returns; it is not documented as a native-object lifetime ownership service.

Extend only wow_class_address_owner_probe.c with a thread-local native
destruction observation experiment. The real test subject does not forward
WM_NCDESTROY; it returns73. Install before/after-message and cancellation CBT
observers solely on the probe thread. No helper HWND is used to normalize a
procedure, no product code is selected, and there is no guest/desktop input.
The native subject windows are disposable test objects, never a proposed
runtime helper. Preserve all earlier class/address and conversion assertions.

Invoke verify-wow-control-boundaries.ps1 -ClassAddressResearch through owned
hidden current-pwsh supervisors. terminal-return-research-r1 supervisor42640
starts2026-10-17T14:44:14Z; build32228 starts14:44:22.6734158Z and exits0 in
9.9779548s; probe17968 starts14:44:32.6681101Z and exits0 in0.3521536s.
Its normal destroy, failed-WM_CREATE rollback and cancelled-then-successful
destroy each report exactly one before/body/after sequence. Cancellation
alone produces none. The return observer still sees a live HWND and return73.

Reviewer adds a necessary negative rather than selecting the mechanism.
terminal-return-research-r2 supervisor32672 starts14:45:22Z; build22716 starts
14:45:23.3605110Z and exits0 in8.7646221s; probe3932 starts14:45:32.1443722Z
and exits0 in0.3963725s. Aggregate0 with prior assertions retained. A plain
same-thread SendMessage(WM_NCDESTROY) gives the identical before/body/after
sequence, result73, IsWindow=1 and InSendMessageEx=0, while the window remains
alive. A subsequent actual DestroyWindow has the same observed fields and
only then retires the HWND. This disproves treating that notification alone,
even combined with IsWindow/InSendMessageEx, as proof of actual destruction.
The probe's synthetic flag is test knowledge, not an available product fact.
Logs, source/graph hashes and300s build/30s test deadlines remain under the
respective build/M0-T447/S3 run directories. Both runs are terminal.

This is not proof that every possible lifecycle binding is impossible. CBT
intent alone also cannot substitute for completion: destruction may be
cancelled, while failed creation has its own unwind path. Delayed invalid-HWND
observation does not by itself preserve original xxxFreeWindow's live-object
final-control cleanup ordering. A proposed native-slot migration must solve
these facts rather than install a return hook and claim original semantics.
The existing gateway is itself keyed to WM_NCDESTROY; this experiment does
not execute that production owner or establish a new application regression.

Source review also narrows the immutable-old-procedure problem: it is created
by moving guest dispatch into the actual native slot, not by every solution
to CPD ownership. Current GetThunkWindowProc stores each native old value;
the ordinary guest CallWindowProc path retains its explicit far procedure.
Keeping the existing gateway need not introduce native executable thunks or
rewrite NTW16S. The class-only known-ANSI-origin route remains a smaller
candidate, with the previously documented arbitrary-token direction boundary
unresolved. Do not silently narrow that contract or claim all inputs proved.
No production, mirror, guest or publication changes in this research group.

Second-role qualification: original btnctl.c:1770, editec.c:2510 and
combo.c:519 themselves accept WM_NCDESTROY and WM_FINALDESTROY at the same
cleanup branch. Thus the synthetic-message negative is not a claim that
original controls ignore an explicit terminal message, nor a new requirement
to make arbitrary application misuse safe. The distinction is specifically
the canonical fallback when a replacement procedure swallows the message:
original xxxFreeWindow runs that fallback during actual destruction, not
after any arbitrary sent message. Preserve this scope when comparing options.
It disproves the proposed sole-notification authority, not every possible
native-slot design or the usefulness of a return observer as one input.

Documentation governance terminal-return-governance-r1 completes0, supervisor
39296 starts2026-10-17T14:47:05Z. git diff --check passes. This research changes
one test source plus Current and this ledger; it neither selects a new
production finalizer nor turns the earlier native-slot scope request into an
approved implementation. Source proof leaves the smaller fixed-gateway route
eligible: distinguish known A-query tokens from arbitrary recognized native
tokens before claiming its class-only normalization covers all inputs.

### Decision-ready disposition, not an unproved full-compatibility proposal

Owner requests complete alternatives and tradeoffs rather than another list
of unanswered implementation questions. Recheck the actual producers and
consumers: wow_class_client_query_native uses explicit GetClassInfoExA but
may have no owned class entry; enroll_created_window captures GetWindowLongA;
standard imports retain native WNDCLASSEXA; original WU32GetWindowLong also
returns native dialog procedure values; WU32SetWindowLong and WU32SetClassLong
consume the original thunk recognizer. A numeric token-origin cache alone is
not a lifetime resource: native unregister/reuse can invalidate that inferred
provenance. Do not present such a cache as a completed solution. Class-only
normalization needs a controlled capture/owner boundary as well as encoding;
arbitrary external classes cannot acquire that boundary merely by observing
their atom/name or retaining a project CLS copy.

There is currently no verified implementation proposal that preserves the
full arbitrary-token contract under every existing constraint. This is not a
proof that no public solution can exist. Native-slot migration and early
normalization remain research candidates, not selectable delivered fixes.
Do not offer private-layout readers or helper HWNDs as ordinary options:
the owner has expressly rejected them. The concrete safe dispositions are:

1. Explicit fail-closed subset, requiring new owner compatibility approval.
   At the owned logical GWL_WNDPROC setter, after existing native-token
   validation and before publication, reject every recognized native CPD
   restore with ERROR_NOT_SUPPORTED. Preserve the current procedure, previous
   target ownership, WW fields and style chain on failure. Keep ordinary raw
   native addresses, guest procedures and invalid-token-versus-guest
   classification on their existing paths. Do not replace CPD values with raw
   addresses or guess A/W encoding. This prevents this setter from creating
   another source-class-dependent forwarding target, but deliberately removes
   a supported original restoration capability. It is not complete WOW
   compatibility and cannot close the existing S3 contract without an explicit
   revised boundary. Initial same-class native enrollment, direct CallWindowProc
   and dialog native slots are distinct consumers, not silently disabled by
   this narrowly defined decision; verify their actual owners separately.
   The expected runtime edit is WOW32-owned, not NTW16S, guest or original
   mirror. Exact line accounting follows actual implementation, not a guess.
   Required tests: A/W CPDs rejected without mutation; existing target still
   callable after rejection; native raw and ordinary/FFFF-shaped guest inputs
   retain behavior; failure error preserved; creation/normal/task cleanup and
   styled dispatch retained; complete product delivery gates still apply.

2. Retain the current baseline and explicitly defer this capability repair.
   No new rejection, no purported lifetime repair and no new runtime package
   for this issue. Preserve the source-proven risk and real application
   attribution as separate facts. S3's current group3 remains open unless the
   owner explicitly changes its closure boundary. Other admitted groups may
   proceed without claiming this one completed. This is a scheduling/risk
   choice, not a competing technical fix or silent approval to publish the
   staged incomplete dependency group.

Recommendation for a safety-first bounded implementation is option1 only
if the owner accepts the explicit compatibility reduction. If full original
restoration compatibility is non-negotiable, option1 is disallowed; do not
call option2 a solution or replace the missing solution with an unproved
large rewrite. No compatibility reduction or runtime change is enacted by
this research record. The owner decision concerns the stated tradeoff, not
permission to discover the design later.

### Superseding owner decision: preserve original restoration capability

The owner requires every practicable effort to retain original capabilities
and semantics, with evidence-backed explicit exceptions only when necessary.
The preceding conditional recommendation to reject all recognized native CPD
restores is withdrawn as an implementation direction. It is neither approved
nor implemented. Lack of a completed design does not prove impossibility.
Group3 remains the full original getter/thunk/setter/dispatch and retirement
contract, including same-process nonstandard native sources. Existing tests
rule out particular mechanisms, not this capability. Continue source-led
analysis of owned and unowned native paths and bounded lifetime adaptation;
do not treat an enrollment gap as permission to exclude supported inputs.
No production, mirror or guest change accompanies this decision. Existing
uncommitted work and all other S3 groups remain in scope and preserved.

### Source-led path split and full restoration regression contract

Reinspection after the owner's preservation decision distinguishes three
paths, rather than treating every native token as the same unresolved case:

1. wow_user_get/set_window_longA (wow_user_private_access.c) forwards actual
   GWL_WNDPROC operations to USER when no nonzero logical target is owned.
   wow_window_client_try_get/set_long excludes this negative index, so the
   original control-extra-word path does not intercept it first. Native-to-
   native restoration here already uses the host setter's native lifetime
   mechanism. This is source proof of routing, not a new runtime pass.
2. An enrolled nonzero logical target uses wow_window_procedure_exchange.
   The class resolver validates/recovers an address, but the binding retains
   the incoming source token. Destination dispatch can therefore depend on
   source-class retirement. The corrected source comment no longer claims
   absence of every public resolver or implies independent ownership.
3. enroll_internal_locked skips unknown native classes; FindPWW can still
   obtain a resource-view WW through wow_user_borrow_window. In that case
   the current fallback rejects a guest-tagged replacement rather than
   installing a native-callable guest target. The fixture's existing
   ERROR_INVALID_STATE check proves a defensive boundary only, not original
   nonstandard-window subclass compatibility. This belongs to the existing
   group3 get/thunk/set/dispatch contract, not a new feature or approved loss.

Original evidence: walias.c FindPWW; wuwind.c WU32SetWindowLong's two branches;
wsubcls.c GetThunkWindowProc's same-process nonstandard-window allowance;
kernel/getset.c xxxSetWindowData CPD address AND encoding normalization.
The unchanged original WU32SetWindowLong already carries the required guest
procedure and old-procedure semantics; do not replace its algorithm to hide
the missing host-side admission/lifetime binding.

Added transferred_callproc_contract(FALSE/TRUE) to the existing
wow_window_procedure_fixture.c. It creates a native ANSI or Unicode source,
obtains the opposite-encoding token, assigns it through the actual project
setter into two separately classed owned destinations, destroys/unregisters
the source, then requires both A/W text calls to succeed. Destination A is
destroyed/unregistered before B is exercised. This checks both CPD directions
and independent destination retirement, not merely invoking a still-live
source token. Assertions require successful replacement and the original old
guest target; refusing the operation cannot pass. This is newly staged test
source, NOT executed/pass evidence. It is scheduled with the repaired group,
not another one-off build. Callback scheduling remains an explicit fixture
double; eventual real guest/product acceptance is still mandatory.

Native-slot lifecycle alternative was also checked against original
createw.c: xxxDestroyWindow offers HCBT_DESTROYWND before destruction, but
xxxFW_DestroyAllChildren directly calls xxxFreeWindow for same-thread
children, and failed creation also reaches xxxFreeWindow. Therefore a CBT
acceptance bit plus terminal-return hook is not by itself a complete original
destruction authority. This narrows one candidate, not permission to drop
subclassing or an assertion that all native-slot designs are impossible.
No runtime repair or guest/mirror change is claimed by this audit batch.

### Existing-native subclass admission: ownership coupling located

The failed first-subclass route is not just a missing call to enrollment.
Tracing the complete proposed reuse chain exposes its exact dependencies:

- wow_user_private_access.c rejects the unbound guest tag; the original
  WU32SetWindowLong already selected the correct guest target and must remain
  the caller. The missing work is native callback/identity binding.
- wow_window_words_attach_target can reuse an observed generation's existing
  WW, preserving FindPWW's outstanding pointer. It accepts a NULL class, but
  that alone does not make the subsequent production publication valid.
- wow_window_words_publish_owner -> wow_user_window_publish ->
  mvdm_softpc_wow_page_domain_publish_handle permits observed-WND promotion
  only when its class changes from an observed CLS to a managed CLS. Existing
  native-window subclassing must NOT change its class. Passing NULL instead
  fails this promotion guard; importing/replacing the native class merely
  to satisfy the guard would confuse class ownership with window ownership.
- forget_observed_window and collect_observed_windows also infer WND cleanup
  ownership from CLS.observed. Any correction must audit these same consumers
  together, not merely relax publish_handle's guard.
- rollback_enrollment ends in wow_window_words_terminal. That is appropriate
  to rejected creation, not failed subclassing of a still-live native HWND.
  wow_user_window_retire retires the shared identity; it is not an unpublish
  operation preserving the prior observation. Do not reuse it as rollback.

Required correction remains in group3's existing identity/lifetime contract:
separate per-window callback ownership from class provenance while retaining
one authoritative owner and a controlled guest projection. Preserve the same
observed WND/CLS and outstanding WW aliases; prepare all fallible ownership
work before exposure; failure must leave native target, class, guest words,
relations and cleanup ownership unchanged. Reuse the original registered
callback in wow_class_client_context.callback and original native-thread
identity lookup; do not assign an arbitrary caller task as window owner or
invent another callback/task registry. Existing native-to-native fallback
must remain native and must not acquire WOW cleanup ownership.

This finding rules out the superficially small reuse patch and identifies
the shared ownership predicate that must change as a group. It does not
authorize broad native-thread scheduling, guest modification, helper HWNDs,
or a compatibility reduction. No new runtime change/test pass is claimed.

### Per-generation publication ownership implementation, not yet delivered

The first coherent ownership-layer correction is staged in the project-owned
mvdm_softpc_wow_page_domain.c and its header. Existing backing-lifetime
records now carry managed-publication provenance explicitly. The public
managed publish entry marks that generation; native observation creation and
graph sampling leave it observed. This is the copied publication's provenance,
not a second native HWND owner, task selector or class registration. Native
USER still owns HWND lifetime; the existing WOW object table owns callback
cleanup. No guest-layout field or original mirror body changes.

The six related decisions now consult this generation's provenance: replacement
of a stale observed slot, promotion of an existing observation, transactional
link retirement, graph-generation replacement/withdrawal, explicit observation
forgetting, and dead-observation collection. CLS.observed remains only class
provenance/collection state. Managed promotion can retain its existing native
class (zero class_server) or explicitly supply that class; duplicate ownership
and retired/invalid class identities still fail. All fallible sampling precedes
the ownership marker's commit. Class replacement for ordinary recovered
creation remains supported; this does not change native window classes.

verify_native_class_window_ownership in wow_page_domain_fixture.c adds two
cases (implicit existing class / explicit observed class). They require same
backing/class and unchanged native atom; correct private words; rejection of
duplicate adoption and observation-forget; observation collection must leave
the managed identity intact after native destruction until its actual owner
retires it; a retained borrow survives retirement and frees on final release.
The old observed-class rejection assertion was superseded by these positive
original-contract cases, while an invalid class-token rejection replaces it.
Existing managed-class promotion tests remain. Tests are staged, NOT run.

Second-role source review checked every remaining CLS.observed use: they are
class allocation/collection/descriptor operations, not WND ownership tests.
git diff --check passes for the changed source/header/fixture. Full group3
still needs reversible existing-HWND enrollment and actual setter wiring;
CPD lifetime/encoding remains open. This intermediate layer is neither a
published repair nor completion of first-subclass or original-call semantics.
No separate small build is launched; grouped implementation/acceptance remains
the owner-required schedule. Existing 258 dirty paths are preserved.

### First-subclass transaction implementation, verification pending

ADAPTER-WOW-033/048's project-owned path now stages
wow_window_procedure_adopt_native, reached from the unbound GWL_WNDPROC guest
branch only after native CPD validation. It reuses the current observed WW
generation and the registered original callback in class_client_context.
Native owner thread lookup uses the existing runtime/task relation, not the
subclassing caller as an invented owner. The original WU32SetWindowLong still
owns thunking the returned predecessor and updating its private WOW words.

Fallible property/object publication precedes one native WNDPROC exchange.
That exchange returns the actual previous native procedure. A new explicit
cancel-native-adoption operation validates the exact generation and unchanged
observed CLS, then removes only managed publication provenance. The companion
object-table rollback removes only that exact cleanup registration; the WW
property rollback does not run terminal cleanup or retire the shared handle.
Existing aliases, class and resource-only WND remain valid. Entry error is
restored on success; failure error survives rollback. Ordinary native target
replacement continues through native USER and does not acquire this owner.

Added composite verify_native_window_first_subclass: real page-domain and
native HWND, deliberately occupied ownership entry to fail publication,
unchanged original native target/property/CLS/WW on failure, retry using the
same WW, callback dispatch, original native target restoration and normal
destruction. The callback is explicitly a double, not guest execution proof.
The page-domain ownership cases additionally cover stale cancellation,
successful cancellation, duplicate cancellation failure and re-adoption with
private words unchanged. The separate native-dialog fixture explicitly fails
if its unsupported observed-adoption rollback is reached; it does not pretend
to provide a real page domain. All new cases remain UNRUN.

Second-role review does not close the group: pre-existing metadata-only
enrollment still needs its target-upgrade transaction; independent native
threads need original-contract attribution rather than invented WOW tasks;
cross-task/native-writer and pre-existing NTW16S subclass-chain cases need
actual acceptance. CPD transfer across classes remains unresolved. Raw native
WNDPROC installation can interact with an already installed external style
carrier; do not call this path fully styled-compatible without resolving that
edge. These are remaining group3 obligations, not exclusions or approved
fallback semantics. No guest or mirror changes and no publication occurred.

### Native adoption grouped acceptance: r23 failures and fixture correction

Inputs are the staged first-subclass transaction and per-WND ownership above.
The background native-adoption-boundaries-r1 cohort passed all 30 supplemental
cases. Full-link-r11 failed because the worker export list omitted
cancel_native_adoption; the export was added, and full-link-r12 passed both
worker (5.565 seconds) and provider (5.642 seconds). This is link evidence,
not runtime closure or publication.

composite-lifetime-r22 did not reach runtime: its new fixture used the
walias.h-only GWL_WOWvpfnWndProc spelling. It now uses the original alias's
GWL_WOWDWORD2 value. r23 supervisor PID4624, start 2026-10-17T15:38:19Z,
completed with exit3: all three cases reached the same four assertions in
native-class-projection and stopped before their later scenario checks.
The new per-WND ownership cases pass with errors=0, including cancellation,
retry, identity/class preservation and managed retirement. First-subclass
installation/rollback/restoration pass their individual checks, but dispatch
fails at lines1511/1512; later allocation balances fail at1686/1868. These
failures are retained, not accepted or treated as unavailable capability.

Source tracing identifies an exact fixture prerequisite omission:
verify_native_window_first_subclass supplied thread.ppi but no thread.ptdb.
dispatch_bound calls wow_menu_native_refresh_window before guest dispatch;
its menu_context explicitly rejects a missing ptdb with ERROR_INVALID_STATE.
The fixture now supplies the same task/execution/lifecycle context as the
existing real-CCPU callback suite. No production guard is removed. Its raw
DestroyWindow path now reconciles the copied graph before the following
independent allocation-balance tests; whether this resolves both balance
failures remains an assertion to verify, not a concluded product fix.

native-adoption-group-r2/window-procedure failed before runtime because it
linked both real lifecycle/page-domain objects and partial mock definitions.
The runner now selects the real page-domain owner (inactive for this native
procedure-only test), omits parent-stub, and compiles the real lifecycle with
only its two callback-scope exports renamed. The existing counting wrappers
call those real exports; entry/leave counters and all CPD assertions remain.
The normal lifecycle object is explicitly excluded from this fixture archive,
so no duplicate provider or link-order selection is permitted. This does not
claim guest scheduler acceptance from a native-only test.

Background native-adoption-group-r3 supervisor PID42668 started
2026-10-17T15:45:48Z using run-native-adoption-validation.ps1 with full-link-r13
and composite-lifetime-r24. It verifies unchanged supplemental input hashes,
then runs full link, all three composite cases and window-procedure serially.
Per-child PID/start/deadline/exit and stdout/stderr are retained below those
build/M0-T447/S3 run roots. Second-role review:
no capability rejection, guest change, mirror change or publication; CPD
source-class-independent lifetime and the remaining group3 edges stay open.

r3 exited1 before build: its input-identity guard correctly rejected changed
wow_window_procedure_fixture.c against the earlier boundary manifest. No
runtime case was executed or falsely reused. The runner now accepts an
explicit fresh boundary run, executes that entire cohort and checks its new
manifest before proceeding. Replacement group-r4 PID3200 started
2026-10-17T15:46:50Z, selecting native-adoption-boundaries-r2, full-link-r13,
composite-lifetime-r24 and its own window-procedure case. Those run names
were not created by r3's early refusal. This is a new run after a recorded
terminal result, not a restart based on observation timeout. Results pending.

group-r4 partial results: native-adoption-boundaries-r2 exits0 and
full-link-r13 passes worker/provider (5.397/4.773 seconds). r24/window-borrow
exits0 with WOW_NATIVE_FIRST_SUBCLASS_CCPU errors=0 and
WOW_NATIVE_CLASS_WND_OWNER_CCPU errors=0. Both previously failing balance
markers WOW_VIEW_RETIREMENT_CCPU and WOW_DEAD_GRAPH_CCPU now report errors=0;
the full fixture reaches WOW_PAGE_DOMAIN_LIFECYCLE_OK and WOW_FIXTURE_OK.
This supports the fixture-precondition/graph-cleanup diagnosis above without
changing product guards or assertions. It remains a callback double over
real HWND/page-domain storage, not proof of guest instruction execution.
The remaining serial cases are not yet reported as passing.

Further source review distinguishes a proposed edge from an observed caller:
the only src calls of wow_window_words_attach/attach_class are their wrapper
definitions. Product callers use attach_target from enroll_created_window
or the new adoption entry. enroll_created_window captures a native target
for native registrations; create_bound otherwise obtains the class target.
Thus metadata-only attachment is currently a fixture/API edge, not yet a
demonstrated product route. Do not add an upgrade engine solely because a
test can construct an empty binding. Keep tracing target-less registration
reachability before deciding that edge's actual required disposition; this
finding does not waive first-subclass support or CPD restoration.

r24 completes0: native-creation also passes (97 callbacks, nested/rejected/
vetoed creation, 13 destructions, table_empty=1); standard-class passes nine
class resolution/storage/precedence/retirement rows plus restored=9 original
cleanup policy. All three reach WOW_FIXTURE_OK. The following standalone
window-procedure runner PID39264 starts2026-10-17T15:51:47.7153947Z with its
900-second contract deadline; no CPD result is inferred from r24. Full guest
and publication gates remain open. Previous turn is progress (fixture changes
and changed runtime evidence), not a mere status restatement.

group-r4 terminal result is1. The standalone window-procedure fixture now
links without duplicate providers and executes, exits3. Existing native
CallProc and class normalization checks reach their success markers.
transferred_callproc_contract(FALSE/TRUE) each fails the four destination
WM_SETTEXT returns and final callback count: text_calls=0, cumulative
errors5/10 at lines286/288/294. The source destruction/unregister and both
destination setter return assertions did not fail. This isolates the
demonstrated gap to post-source-retirement invocation, not rejection of the
initial assignment. Logs and fixture-exit.txt are under
native-adoption-group-r4/window-procedure. Both destination classes are
independent, and both ANSI/Unicode source directions fail. The current
forwarding token is therefore not an adequate lifetime owner. No release.

Pinned original kernel/getset.c, GWL_WNDPROC branch, resolves the CPD's
pfnClientPrevious and wType before assigning lpfnWndProc/WFANSIPROC;
kernel/class.c DestroyClass frees its spcpdFirst list. Current
wow_window_procedure_exchange instead keeps the incoming source token in
native_callproc. Runtime evidence now directly confirms the previously
source-derived lifetime concern. Fix must preserve address AND encoding,
independent destination lifetime and existing style/subclass chains, rather
than refuse a valid transfer or weaken this regression.

Public API cross-check for the native-slot research alternative:
[SetWinEventHook](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook)
offers scoped in-context events, and
[event constants](https://learn.microsoft.com/en-us/windows/win32/winauto/event-constants)
describe window-object destruction notifications. Neither page establishes
the exact required original finalization order for failed creation, nested
child destruction and retained callbacks. This is an unverified possible
lifetime input, not a selected finalizer or evidence of equivalence. No
additional hook or hidden HWND was introduced.

### Public destruction event candidate: complete negative boundary evidence

Source requirement: pinned createw.c xxxFreeWindow destroys children, sends
WM_NCDESTROY, then invokes the canonical control WM_FINALDESTROY fallback
when FNID_CLEANEDUP_BIT is clear. The current CPD bug motivates evaluating
native-slot ownership, but that cannot discard this cleanup guarantee.
Public WinEvent documentation alone does not supply the required sequence.

Added research-only wow_window_destroy_event_probe.c. Its DLL build provides
the documented in-context callback module; the EXE owns every subject HWND.
Installation is restricted to GetCurrentProcessId/GetCurrentThreadId and
EVENT_OBJECT_DESTROY; callback asserts same-thread delivery and filters
OBJID_WINDOW/CHILDID_SELF. No auxiliary normalization HWND, guest input,
product linkage or installation is used. The same source builds twice with
WOW_DESTROY_EVENT_DLL selecting the DLL. New destroy-event-research Ninja
target is non-default and all output remains under build/.

Procedure: generate New-T447S3CallBoundaryNinja.ps1 for call-boundaries-x86,
build destroy-event-research, execute bin/wow_window_destroy_event_probe.exe.
Owned background supervisor26728 starts2026-10-17T15:56:54Z; build8916 starts
15:56:55.4169789Z, probe10192 starts15:57:04.9714825Z. destroy-event-r1 exits0,
with source/generator/graph/EXE/DLL hashes and child PID/deadline/exit logs.
Exit0 means the research executed, NOT that a replacement is equivalent.

Observed complete matrix (window procedure deliberately swallows NCDESTROY):

| Case | Object-destroy notification | Terminal message | Finding |
| --- | --- | --- | --- |
| Normal destruction | 1, before message | 1 | Event is not the final-cleanup point. |
| WM_CREATE returns -1 | 1, before message | 1 | Same early ordering. |
| WM_NCCREATE returns FALSE | 0 | 1 | Event misses failed initial construction. |
| Synthetic NCDESTROY, then actual destruction | 0 for synthetic, 1 for actual | 2 | Distinguishes this standalone synthetic case only. |
| Destroy parent with child | 1 for parent, 0 for child | 1 each, child first | Per-window event coverage is incomplete. |
| CBT veto, then actual destruction | 0 on veto, 1 on actual | 0 on veto, 1 on actual | Cancellation does not falsely trigger this event. |

Second-role conclusion: a sole WinEvent finalizer cannot preserve the original
contract. Reject that particular implementation, not the CPD capability or
every possible native-slot design. Combining it with other observations has
not proved the missing construction/child/order cases and is not selected.
No new original mirror diff, production change or guest modification. The
CPD source-class-independent lifetime defect remains an S3 group3 blocker;
the prior passing r24 evidence is unchanged because only research/test build
inputs changed. No new product build or publication was required or claimed.

### CPD acceptance separation and synthetic-terminal contract audit

Current-source review confirms that original btnctl.c and combo.c accept
WM_NCDESTROY and WM_FINALDESTROY at their cleanup branch. It does not follow
that xxxFreeWindow's canonical fallback runs after a synthetic message which
the replacement procedure swallows. Public
[WM_NCDESTROY documentation](https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-ncdestroy)
identifies DestroyWindow as producer and describes memory cleanup, but does
not establish permission to collapse every sent terminal message into actual
HWND destruction. Therefore this review finds no authoritative basis for
silently removing the earlier negative boundary. This is not a demand for
protection against every arbitrary application misuse; it preserves the
specific difference between the control's own processing and unconditional
adapter retirement. No runtime finalizer was changed.

The original GWL_WNDPROC assignment also consumes CPD.wType, whereas current
wow_window_call_native_previous always calls CallWindowProcA with the stored
token. A lifetime-only repair cannot be accepted as an encoding proof.
transferred_callproc_contract now adds source-live SendMessageA/W checks for
both destinations and both source encodings, preserving the source-retired
checks. Separate WOW_TRANSFERRED_CALLPROC_LIVE markers report the live phase's
callback count and error delta. This is a source-derived acceptance gap, not
an additional product feature. New assertions are staged and UNRUN; r4's
10 failed post-retirement assertions remain the last actual CPD result.
No small product rebuild is started for this test-only increment; include it
with the next coordinated group3 implementation/verification batch.

### Native-window adoption: original thread attribution trace

Question: does assigning a guest procedure to a same-process native HWND
implicitly create or select a WOW task for its native owner thread? Read-only
trace uses pinned OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c and the current
wow_window_procedure_adopt_native implementation; no runtime test is claimed.

- windows/core/ntuser/kernel/getset.c, xxxSetWindowData GWL_WNDPROC, checks
  process identity and zombie state, normalizes the predecessor and incoming
  CPD, and replaces the procedure. That branch does not allocate a WOW task.
- windows/core/ntuser/client/client.c, DispatchClientMessage, calls the
  registered pfnWowWndProcEx with the HWND, encoded procedure and adwWOW.
  usercli.h CALLPROC_WOWCHECK likewise selects the callback, not a new thread.
- base/mvdm/wow32/wmdisp32.c, W32Win16WndProcEx, immediately obtains CURRENTPTD
  and reads its flags, then eventually enters CallBack16. A PWW or a same-process
  HWND is not by itself sufficient execution context.
- wow32.h defines CURRENTPTD as the current TEB's WOW32Reserved. In this WOW32
  source tree its assignments are the initial temporary TD in wow32.c,
  W32Thread's actual task TD, and W32RemoteThread's debugger-specific TD in
  wkman.c. DLL_THREAD_ATTACH only calls IsDebuggerAttached; inspection of that
  function finds debugger notification, not task/TD construction.
- W32RemoteThread explicitly serves debugger Toolhelp callbacks and consumes
  prepared remote VDM state. It is not a generic window-subclass enrollment
  helper and must not be repurposed as one merely to satisfy a fixture.

Implementation consequence: do not synthesize a WOW task or borrow the setter's
task as the window's owner. Current adoption resolves the HWND's actual TID
through the existing runtime relation. The trace supports distinguishing a
native-class window on a WOW task thread from a window on a wholly unrelated
native thread; the earlier audit wording conflated these cases. This does not
prove that every same-process assignment may be rejected when that relation
is absent: original setter acceptance and eventual callback validity are
distinct contracts. No new rejection policy or compatibility waiver is selected.

Second-role review: the positive same-task native-class adoption already has
r24 fixture evidence. Cross-task ownership still needs the existing find_thread
provider and callback-entry chain checked together; arbitrary native-thread
execution cannot be declared restored from that test. CPD source-retirement,
encoding, and NTW16S predecessor identity remain separate unresolved group3
obligations. This trace removes an unjustified task-construction approach from
the candidate repairs, not any original capability from acceptance. No product
or guest change, mirror diff, build, or publication occurs in this increment.

### Cross-task attribution: existing scheduler chain and acceptance precision

Source continuation inspected wow_user_task_lifecycle.c init_style, find_thread,
callback_enter/leave, native_call_begin/end and retire_task, plus
wow_window_words_binding.c publish_owner and dispatch_bound. Enrollment records
the actual GetCurrentThreadId in the lifecycle list under the runtime lock;
adoption queries that list with the HWND's native owner TID. Native-send entry
records the receiver and calls original xxxSleepTask; callback entry on the
receiving thread uses original xxxDirectedYield when necessary. Return resolves
the receiver again under lock before DirectedScheduleTask/xxxSleepTask, or yields
if it has retired. Retirement unlinks/frees the task only after original cleanup.
Window cleanup ownership and the current callback execution task are different
concepts: explicit CallWindowProc must not be forcibly moved to the HWND owner.

The existing cross-callout fixture actually enrolls a second task and sends
native messages; it also covers receiver exit during a send. However its native
test window manually associates cleanup ownership and wraps callback entry. It
does not prove first-subclass adoption through the original WU32SetWindowLong
or guest execution. Do not promote those fixture results to that wider claim.

Staged assertions now verify the production find_thread result against native
HWND TID, process, actual enrolled task ID and window cleanup owner under the
runtime lock. The receiver checks its actual thread/task rather than only that
some scheduler owner is current. These assertions are UNRUN and are to join the
next grouped verification, not a separate small build. The source trace finds
no justification for a second scheduler or invented task registration. The
combined cross-task first-subclass/guest acceptance remains open; CPD and style
chain defects are not hidden by this narrower ownership check. No runtime code,
mirror or guest changed; no new publication is claimed.

### Style predecessor distinction: managed replacement versus first adoption

Read current wow_window_procedure_exchange, logical_procedure,
wow_window_call_native_previous and adopt_native alongside NTW16S attachment.c
and controls.c. Two contracts must not be conflated. An already-managed WOW
window has an existing logical application target below a comctl subclass
carrier; the private getter/setter deliberately preserves that outer carrier.
First adoption of a previously native window instead uses the real native
SetWindowLong return value. That predecessor may itself be a comctl carrier,
not the class's raw procedure. Substituting the leaf loses an existing chain;
exposing a carrier for the already-managed case loses the logical old target.
Neither a universal raw native setter nor a universal leaf substitution is a
complete repair. This source conclusion does not establish failure of every
native-slot design, or require an NTW16S rewrite by itself.

The CCPU first-adoption fixture now runs the existing rollback/retry/dispatch/
restore/retire sequence twice: without a subclass and with a real comctl
SetWindowSubclass carrier installed first. It verifies the exact pre-adoption
native predecessor survives publication failure and is returned on success;
after restoration the subclass registration and forwarding remain callable.
It reuses the existing native_chain callback, not a new behavior engine.
This is a native carrier acceptance extension, NOT actual NTW16S rendering or
guest-callback execution evidence. Assertions are staged and UNRUN for the
next grouped batch; r24 predates this additional case. No runtime or original
mirror changes. The source-class CPD lifetime and encoding defect remains
unfixed; this test must not replace its still-failing acceptance conditions.

### Consolidated adoption acceptance r5 started

After source-led ownership, predecessor-chain and CPD encoding review, run the
staged acceptance increments together against the existing first-adoption
implementation. This is verification of that implementation, not another
attempt to diagnose CPD by random runtime exploration or a claim of a new
runtime repair. The source-proven CPD blocker remains mandatory and is expected
to prevent delivery until repaired.

Hidden supervisor PID33732 started 2026-10-17T16:13:45Z using
build/M0-T447/S3/run-native-adoption-validation.ps1 with native-adoption-group-r5,
full-link-r14, composite-lifetime-r25, native-adoption-boundaries-r3 and
RefreshBoundaries. The boundary child PID2468 started16:13:46.7382965Z with
1800-second deadline; its exit file records0. Full-link child PID2360 started
16:13:55.6218137Z with2700-second deadline. Supervisor was authoritatively live
on inspection; no complete group outcome is claimed here.

The existing runner now also collects native-send, native-nested and
native-foreign cases, each with a900-second owned deadline, after the composite
and procedure cases even if an ordinary case fails. This includes the new
thread-identity assertions rather than leaving them outside the batch. Every
case retains its real exit and logs. No input assertion is relaxed to obtain a
green group. Product publication remains disabled; no guest changes.

First partial outcome: full-link-r14 exits0. composite-lifetime-r25/window-borrow
also exits0 and records WOW_PAGE_DOMAIN_LIFECYCLE_OK. Its two
WOW_NATIVE_FIRST_SUBCLASS_CCPU rows each report errors=0 for existing_subclass
0 and1, covering the new pre-existing comctl chain as well as the earlier
unsubclassed path. This rules out a blanket assertion that first adoption
necessarily destroys every existing subclass chain. It does not prove actual
NTW16S drawing, managed-target migration or CPD lifetime. native-creation child
PID9100 starts16:16:12.7746146Z; remaining cases have not yet completed at this
observation. Keep the complete r5 outcome open.

Subsequent r5 observation: composite-lifetime-r25 completes all3 cases with
exit0, including native creation's97 callbacks/13 destructions/empty table.
The runner records lifetime.exit0 and starts window-procedure PID33344 at
16:19:05.9392231Z with900-second deadline. CPD and the three native task cases
are not yet reported as complete. No restart, publication or closure.

Window-procedure r5 now terminates with fixture exit3 / runner exit1. Both
WOW_TRANSFERRED_CALLPROC_LIVE rows (ANSI source and Unicode source) report
text_calls=4 errors=0. After source destruction/unregistration, each direction
reports text_calls=0 with5 failed assertions,10 total, as in r4. Thus current
live-source forwarding passes this A/W matrix; do not describe encoding as an
additional demonstrated runtime failure. Any replacement must still prove its
own encoding preservation, but the reproduced defect here is specifically the
source-class-dependent lifetime. Existing tagged dispatch and subclass carrier
checks add no errors before the transfer failures. Raw cumulative errors=10
on later summary markers must not be misreported as10 new subclass failures.
native-send child PID41312 starts16:20:47.2051754Z; the rest of the group remains
running. Logs are under native-adoption-group-r5/window-procedure; no assertion
or product implementation changed in response to these expected failures.

r5 is terminal with exit1. The three native task cases all fail at link, before
runtime: parent-stub.obj and mvdm_softpc_wow_page_domain.obj provide the same
seven storage symbols (publish_menu/publish_menus/retire_menu/publish_named_class/
retire_class/release_class/set_expected_windows_version). These are not three
runtime ownership failures. The lifecycle fixture intentionally injects class
publication failure and has no active CCPU domain; simply deleting its stub
would remove that test contract. Source review finds the real page-domain
module is now also needed by the expanded provider closure.

Test-only repair selects that production module explicitly with exactly those
seven definitions renamed, leaving the existing boundary doubles as their
unambiguous exported owners. All other page-domain definitions remain real.
No /FORCE:MULTIPLE, permissive stub additions, product edits or mirror diffs.
The native fixtures still do not prove CCPU layout; r25 covers the real-domain
boundary separately. PowerShell syntax parsing and diff whitespace check pass;
link/runtime verification is pending, not inferred from those static checks.

After all r5 children terminated, hidden supervisor34452 starts
2026-10-17T16:26:17Z with build/M0-T447/S3/run-native-ownership-r1.ps1. It reruns
only the three previously unexecutable native-send/nested/foreign cases as one
batch, each with900-second owned deadline and fresh logs. The passed r25 cohort
and known failing CPD case are not redundantly restarted. No delivery or closure.

### Native ownership r1 terminal results and second-role review

native-ownership-r1 completes with exit0; native-send, native-nested and
native-foreign each have runner exit0 and fixture errors=0. The ordinary send
returns114 on the actual receiver task and passes the new native-TID/task-ID/
cleanup-owner assertions. Nested acceptance records callback owner restoration,
timeouts with both entry lock states, exceptions, destruction during send,
dead targets and receiver exit with both entry lock states. Foreign acceptance
delivers the external and project sends without assigning the pending project
send identity to the foreign callback. These are the selected existing native
scheduler fixtures, not proof of all S4 mixed-message contracts or guest code.

The linked native-send map explicitly attributes publish_named_class to
parent-stub.obj and cancel_native_adoption to page-native-fixture.obj; its
renamed real publication body has a distinct wow_fixture_real_page symbol.
This verifies the intended single provider selection for the colliding leaves,
with the class failure/retry assertion still reporting projected9/calls10.
No duplicate-definition bypass or additional success-only mock was added.

Second-role disposition: the native task identity/handoff assertions are now
verified, not pending. r25 proves its composite first-adoption/canonical-class
tests, including a pre-existing comctl chain; none executes real guest callback
instructions or actual NTW16S painting in that adoption case. These boundaries
remain explicit. CPD r5 still has exactly10 source-retirement failures while
both live-source A/W phases pass. It is not a new task-scheduling failure, and
the passed suites do not waive it. No product code changed during this batch;
no publication/commit/closure claim. Do not repeat these completed fixtures
without a relevant implementation/input change; return implementation effort
to the source-class-independent procedure lifetime contract.

### Bounded destination transaction: complete decision boundary, not admission

Recheck pinned kernel/getset.c GetCPD(742--826): CPD_WND/CPD_DIALOG select the
destination's pcls, CPD_WNDTOCLS selects that same class through a WND, and
CPD_CLASS takes a CLS. These flags choose ownership, not token decoding. The
function asserts non-CPD input, then stores dwProc32 and the caller's transition
flags unchanged. Thus the already bound native GetCPD service cannot by itself
clone an unknown token's address AND encoding. Do not run another speculative
probe passing a CPD as its raw address. This confirms the earlier source
finding rather than claiming a newly discovered API.

The smaller candidate with positive lifetime evidence remains class-address-r2's
transaction on the ACTUAL destination HWND. Unlike native-slot migration, this
restores the existing dispatch/style entry before returning, so it does not
require native executable guest thunks, a replacement finalizer or rewriting
NTW16S. Its explicit new compatibility cost is transient visibility to native
threads which bypass the project's execution/publication synchronization.
The owner has NOT approved that cost. No runtime implementation is selected.

Proposed complete flow for the owned logical WND setter:

1. Retain the exact WND binding and its class/generation; acquire the existing
   project execution/publication protection. Validate the incoming native CPD
   through the existing class resolver; preserve the old logical target and
   native forwarding token until commit. Do not allocate an auxiliary HWND.
2. Read the actual destination's IsWindowUnicode. Use the matching A/W native
   setter entry to install the incoming CPD and obtain the actual predecessor.
   Matching the old encoding avoids creating an opposite-encoding predecessor
   CPD just to undo this operation. The native setter, not project token bits,
   determines the incoming target's encoding. Original GWL_WNDPROC handling
   has no application style-change callout; do not generalize this to other
   SetWindowLong indices.
3. While installed, read the actual encoding and matching native procedure
   view. For an ANSI target the ANSI callable address is sufficient. For a
   Unicode target use the existing GetCPD service with that real Unicode
   address and CPD_WND|CPD_ANSI_TO_UNICODE to obtain an ANSI entry owned by the
   destination class. This avoids inferring direction from a token's numeric
   shape or treating an allocation fallback raw address as converted input.
4. In a finally path, restore the exact native predecessor through the entry
   matching its saved encoding, before publishing any logical change. Preserve
   the native error on ordinary failure. If restoration itself cannot establish
   the old entry/encoding, use the existing noncontinuable ownership-failure
   boundary, not a successful return with a partially replaced native window.
5. Only after successful restoration commit the destination-owned callable
   token/address to the logical binding and return the original logical target.
   A destination token belongs to its native class: replacing a window target
   must not invalidate saved old-procedure values. Do not independently free
   it on replacement or window destruction; normal native class retirement is
   its owner, as in the original implementation.

Failure/rollback acceptance must cover incoming rejection, native-set failure,
destination CPD allocation failure, restoration failure injection, preserved
logical target and error, actual native top/encoding restoration, saved old
values after further replacements, both source/destination A/W combinations,
source and independent destination class retirement, and existing style chains.
No failure path may guess encoding or continue after losing native ownership.
Real NTW16S and guest acceptance still follow the grouped fixture checks.

The concurrency limitation is concrete: an uninstrumented native thread can
GetWindowLong while step2 is installed, or SetWindowLong between steps2 and4;
the former can observe a transient target and the latter can be overwritten by
restoration. A project lock does not protect those calls. HWND destruction on
the owner native thread during an out-of-thread transaction is likewise outside
that protection if it bypasses the WOW execution gate. Do not call the sequence
atomic, install a fake global lock, suspend unrelated threads, or silently
serialize through another window. Concurrent unmanaged mutation/destruction
of this same destination must be excluded by explicit owner approval if this
candidate is chosen. Other windows, ordinary WOW cross-task calls and original
token restoration capability are not excluded by that proposed boundary.

Evidence and limits: r2 proves all four A/W combinations, two destination
classes, source retirement and native-entry restoration, with no callback in
the measured transaction. r25 proves the separate existing first-adoption
comctl-chain path, NOT this transaction with NTW16S. r5 proves the current
source-token implementation fails after source retirement. A complete product
implementation still needs the above deterministic rollback and integration
acceptance; research is not delivery. Exact private token readers, helper HWNDs,
class pinning and capability rejection are not selected alternatives. Continuing
full-atomic research remains possible but no complete verified implementation
has been found. This is a specific proposed semantic exception, not a claim of
mathematical impossibility or permission to weaken the current S3 contract.

### Resume audit: independent SOL sizing prerequisite and thunk candidates

Revalidated HEAD5a88aae77, active S3 and terminal native-ownership-r1 exit0.
The transient WNDPROC exception remains unapproved; no CPD runtime repair,
new callback architecture or native thunk allocator is selected. Repeated
explanation of that same blocker is not implementation progress. Preserve
the whole group3 gate and existing dirty work while examining the independently
open group6 SOL acceptance prerequisite; this is not S3 closure or deferral.

Read-only candidate audit of pinned OpenNT distinguishes actual ABIs:
base/mvdm/wow32/wsubcls.c::ConstructThunkWindowProc copies a USER16 procedure
into a16-bit code selector; it is not a native x86 WNDPROC entry. MFC
windows/mfc/src42/wincore.cpp::AfxWndProc looks up the current CWnd by HWND,
and SubclassWindow saves a super procedure; it does not give each historical
WOW target an immutable native callable identity. sdktools/apimon/apidll/i386/
machine.cpp::CreateMachApiThunk emits metadata pushes and jumps into ApiMonThunk;
its paired thunk.asm depends on API-monitor TLS, stack and trace state.
It is not an already composable WOW callback owner. The ntuser/server/x86/
thunk.asm file contains stack-switch/thread-termination code, not that missing
callback service. No source was imported or rejected solely for being outside
MVDM; these concrete candidates do not meet the required ABI/identity contract.

SOL evidence reuses current-apps-r7 and published-P16 current-apps-r8, not a
new run. Both stop before gameplay: initial client431x261; requested585x384
becomes431x281. The strict geometry gate is retained. The prior segment1
research manifest and r8 package both identify SOL SHA256
68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31.
Read its retained unrelocated segment1 disassembly at0462--04ef: the ordinary
message switch has no WM_GETMINMAXINFO(0024) or WM_WINDOWPOSCHANGING(0046)
case. The later registered-message comparison remains distinct; it is not
evidence of a dedicated0024 handler. Default branch0890--089f forwards all
five arguments; relocation08a0 resolves USER ordinal107, DEFWINDOWPROC in
the immutable mirrored user.def. No guest image was changed or rebuilt.

Original WM32GetMinMaxInfo and Thunk/UnThunkWMGetMinMaxInfo16 copy five point
pairs; WM32WindowPosChanging copies WINDOWPOS in both directions. These
bodies contain no scale-factor policy. Pinned kernel/dwp.c's default
WM_WINDOWPOSCHANGING calls xxxAdjustSize; kernel/createw.c clamps tiled/sizable
windows against the min/max tracking values after xxxInitSendValidateMinMaxInfo.
The retained r8 window trace contains the nested0024 inside0046, followed by
0047 and0005, consistent with that route. This does NOT establish the actual
host min/max values, DPI context or a root cause for431x281. It narrows the
next evidence requirement to native sizing inputs and observer coordinates,
rather than a speculative SOL-specific behavior patch or another CPD run.
Do not suppress the sizing notification, force game internals, weaken the
card/pixel assertions, or claim gameplay passed. No build, test, publication
or new production/mirror diff accompanies this source-only audit.

### SOL sizing observation and paired package acceptance r9/r10

Add read-only geometry diagnostics to wow_sol_gameplay_probe.c: client/outer
rectangles, styles, observer/target DPI awareness, target DPI, screen/maxtrack
metrics and monitor/work rectangles. Preserve LastError. These metrics are
explicitly caller-context observations, not the actual resize's MINMAXINFO.
No DPI-awareness change, extra target callback, display change, style mutation,
or min/max override is added. A failed exact resize additionally captures its
client pixels before the existing exit4. All deal/stock/pixel/normal-close and
launcher-completion requirements remain intact. Probe SHA256:
4EB67EB2457A98BF44D7DD0AACFE1236B3AB54C658F0AAA7A356E6F6C24EC2F2.

The existing build-owned app supervisor gains an optional mine/sol selection;
its default still runs both. Run r9 against only the complete manifest-checked
P16 release13, with immutable assets/winnt.zip input, to avoid rerunning the
unaffected mine baseline. Hidden supervisor35620 starts2026-10-17T17:12:37Z;
owned child38496 starts17:12:42.0905819Z with180-second deadline. Aggregate0,
compile clean, SOL result includes run16 completion. Current observed display
is1280x720, work1280x672, maxtrack1300x740; both processes DPI-unaware with
equal awareness contexts and target DPI96. SOL starts with585x384 client;
the conditional resize branch is not exercised. Ready/deal/stock hashes are
1e2d5c1e/96e82811/1c0e93ca, stock reference65174fed. All captures succeed.

Run r10 against the unchanged whole-group full-link-r14 worker/provider and
the other manifest-verified release images. Hidden supervisor41768 starts
17:13:28Z. Owned mine29532 starts17:13:31.8610462Z, sol5308 starts
17:13:43.0893760Z; each has180-second deadline and exits0; aggregate0.
Mine passes flag/counter, reset, open-cell pixels, WM_CLOSE and run16 exit.
SOL has the same measured display/DPI/client dimensions as r9, passes ready,
deal and stock changes, WM_CLOSE and run16 exit. Its ready/deal/stock hashes
are e82742d7/fe32688a/97df946a, stock reference65174fed. Differing deal pixels
are not cross-package equality assertions. Package/media hashes, child process
records, compiler/probe output and screenshots are retained in each run root.
Both runs are terminal, not pending waits; no product rebuild or publication.

Second-role conclusion: r10 supplies current ordinary off-style mine/SOL
gameplay/normal-exit evidence for full-link-r14. r9/r10 do not identify the
earlier431x281 failure's cause: old runs lack these environment observations,
and this run does not exercise forced resizing. Do not claim a product sizing
fix, successful small-desktop acceptance, styled-mode acceptance, WRITE
functionality or CPD repair. The strict previous failure remains evidence;
group3's10 source-retirement failures still block S3 delivery.

### Matching NTW16S dependency closure and three-application acceptance

Source review of verify-wow-write-frontier.ps1 confirms its limited contract:
four live samples of MSWRITE_MENU, MSWRITE_DOC and MSWRITE_PAGEINFO, not
editing/saving or normal exit. Use that existing strict gate rather than
calling a zero launcher exit successful application initialization.
Build-owned current-write-r1 supervisor5392 starts2026-10-17T17:17:38Z,
using sealed current-apps-r10 inputs. Off passes all four samples. Win16/All
return launcher exit0 without a retained worker/window snapshot, and both
fail the gate. Aggregate2; the observer deliberately reports observation,
not application acceptance. All child operations have90-second deadlines.

The source-derived cause is a candidate dependency mismatch, not yet a
WRITE behavior regression: wow_title_policy.c::load_titles requires the new
win16_style_hit_test export alongside ABI2 and creation/attachment exports.
The release ntw16s32.dll's30-export table lacks it, as verified with dumpbin
and retained in style-cohort-r1/release32-exports.txt. Off avoids loading the
style DLL on first use. The former app supervisor overlaid only the new
worker/provider onto release13, omitting the S3-modified style dependency.
Reject silently bypassing load failure or weakening the runtime export check.
Earlier Off app passes remain their stated scope; they never proved styled
compatibility of that mixed package.

Build existing S3 NTW16S sources as a complete x86/x64 DLL and thin-demo
cohort, without further product source edits. Hidden style-cohort-r1
supervisor35612 starts17:22:13Z. x86 build14228 starts17:22:15.5226255Z;
x64 build30740 starts17:22:24.0538684Z; each has600-second deadline and
exits0 with31 Ninja steps, MSVC /MT /O2 /W4 /WX. The boundary supervisor30280
starts17:22:33.4050749Z with300-second deadline; both actual DLL tests pass
their query-versus-subclass hit grid, foreign-thread rejection, independent
thread font state, restore/unload, opposite-width rejection and28-page demo
viewport/normal-exit checks. Aggregate0. Other compiled test executables are
not claimed executed. Source and selected output SHA256 manifests are sealed
in that cohort, and checked again before package assembly.

Correct the build-owned app assembler to require a verified matching style
cohort for current WOW32 and overlay ntw16s32.dll, ntw16s64.dll and the x64
W16Style.exe together. Published-baseline mode still uses the unchanged full
release manifest. New WRITE package assembly similarly validates the sealed
r10 inputs and style cohort before copying; it does not mutate r10 or release.
This is dependency closure for the approved S3 hit-test export, not new guest
or mirror behavior. The complete candidate remains unpublished.

current-write-r2 supervisor23776 starts17:23:27Z. All three modes now pass
the unchanged four-sample startup gate; aggregate0. They remain timeout-ended
observations, not normal exit or editing proof. current-apps-r11 supervisor
18348 starts17:25:34Z and uses the same full-link-r14 plus style-cohort-r1
composition. Only the previously missing Win16/All gameplay combinations run;
all four mine/sol cases and their launcher-completion assertions pass,
aggregate0, with180-second owned child deadlines. SOL measured585x384 client,
96 DPI and1280x720 screen in both modes. Package/media hashes, exact process
start records, result files and pixel captures remain in the named run roots.

Reviewer: this closes the demonstrated stale-style candidate assembly gap
and restores the three independent app frontiers for this selected build.
It does not repair CPD source-retirement, prove WRITE editing/normal exit,
prove every original control interaction, or authorize publication/closure.
The ordinary r10 path has no first-use style-DLL dependency; its Off results
are not rerun solely because the unused DLL was rebuilt. Future current-app
runs must not silently restore the old release DLL combination.

### S3 mirror-path responsibility audit and grouped declaration relocation

Revalidate the still-open CPD contract against the original/current setter,
forwarder and transferred-callproc fixture. No new full-domain normalization
implementation is selected, and the existing ten source-retirement failures
remain delivery blockers. A source-policy audit of the other fixed S3 groups
finds actionable ownership violations independently of that technical gap.

Enumerate every changed/untracked C/H/RC/ASM path under src/opennt-host and
src/mvdm, map to pinned OpenNT's original relative path, and test membership.
Of59 existing paths,57 have same-path originals. The two exceptions are
client/controlabi.h and rtl/chartranapi.h: both are project-made facade
filenames, not original source files. Registration of their declaration
provenance did not authorize inventing their mirror paths. Preserve all
other work; relocate both as one declaration-boundary group instead of
removing their required original declarations or conversion bindings.

The complete former controlabi.h moves to
opennt-abi/host-compat/include/wow_control_resource_abi.h; change its three
control-client/control-draw-state/button-resources precomp includes. The
complete former chartranapi.h moves to the same ABI include directory as
wow_char_conversion_abi.h; replace its relative original user.h include
with the same repository-qualified header, and update the already adapted
chartran.c include. Existing OPENNT-HOST-105 provenance, two-byte resource
packing, constants, string helpers and service signatures remain. Both owner
READMEs register the corrected location. No retained original function body,
guest, dispatch policy or CPD behavior changes. Mirror inventory decreases
by two invented facade files; this is not a claimed count of original
algorithm-diff lines. The chartran mirror still has its existing registered
one-include adaptation, now referencing the proper ABI owner.

mirror-layout-r1 hidden supervisor34400 starts2026-10-17T17:34:16Z. It captures
source inputs and the now57-path audit, then full-link-r15 succeeds: worker
34736 starts17:34:53.2788984Z; provider39796 starts17:34:59.3046976Z, each with
1200-second deadline. The existing VdmTib storage gate passes. The subsequent
boundary verifier exits1 BEFORE build/runtime while trying to hash deleted
tracked rtl/chartranapi.h. Retain that failed manifest attempt; it is not a
control execution failure. Git ls-files -co includes tracked removals.

Correct verify-wow-control-boundaries.ps1 to separately enumerate and record
git's deleted paths, exclude only that explicit set from file hashing, and
retain failure on any unexpectedly missing current input or Git failure.
New files remain included. source-deleted-paths.txt records the removed
tracked chartran facade; the previously untracked control facade is absent
by definition. Do not hide a missing input through a blanket Test-Path filter.

mirror-layout-r2 supervisor12836 starts17:35:59Z. It verifies unchanged r1
linked-source inputs and full-link-r15 outputs, reuses that completed link,
and launches boundary child31416 at17:36:01.0713618Z with1800-second deadline.
Build31496 starts17:36:03.1689555Z, runs53 dependency-selected Ninja steps in
75.475842s, exit0. All30 existing boundary executables exit0, including the
original dialog/control/drawing, native dialog-owner and initialization
fixtures. Existing original-source compiler warnings remain, not counted as
new test failures or erased through warning suppression. Aggregate r2 exit0;
both cohorts are terminal. The input/removed-path/build/child/output records
remain under mirror-layout-r1/r2, mirror-layout-boundaries-r1/r2 and
full-link-r15; r1 failed boundary evidence is not overwritten.

Read-only PE comparison with the preserved current-apps-r10 package finds
identical .text SHA256 for both selected images after relocation:
ntvdm0135D9CC968F6B2197026D0A2CA5AFE136E139AA14CDEAA4416A5816CD91A399;
WOW32 2C419B0ADE232A8990573D3357973C99168A084533E7D08B83DC3699271F7417.
Whole-file SHA256 values differ and the new build remains separately
identified; code-section equality is not whole-image/runtime acceptance.
Prior r10/r11 app evidence retains its original binary scope. No release,
commit, guest change, new semantic waiver or S3 closure is claimed here.

### Transient transaction review: native message delivery is another observable

Return to the blocking group3 implementation decision after the independent
source-layout group. Review the existing native event experiment and pinned
xxxFreeWindow ordering. The experiment filters OBJID_WINDOW/CHILDID_SELF
and records matched HWNDs; its missing child event and creation-failure
coverage cannot establish a sole canonical finalizer. Original final-control
cleanup is after the application's WM_NCDESTROY and before class dereference
and native identity withdrawal. Public event notification is not that owner.
Microsoft's [event reference](https://learn.microsoft.com/en-us/windows/win32/winauto/event-constants)
describes accessibility object notifications; its parent-child assumptions
do not promise the required native window final-control callback sequence.
[SetWinEventHook](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook)
also documents limitations on in-context delivery for console applications.
Do not infer a production NTVDM finalizer solely from the research hook flags.
No new event hook or native-slot migration is installed.

Source review reveals an omitted observable in the earlier proposed transient
destination transaction: assigning the real WNDPROC changes the native message
destination as well as saved-procedure queries. The project publication lock
cannot make an uninstrumented owner thread's native SendMessage wait for the
old gateway. The setter/query operations themselves having no callbacks does
not prove no callback can occur during the multi-operation interval. This
qualification supersedes the earlier proposed decision boundary's incomplete
list of concurrent reads/writes/destruction; none was approved.

Extend only wow_class_address_owner_probe.c with one barrier-controlled native
case. A helper performs the actual-destination CPD install/restore. While it
waits at an event barrier, the destination's own native thread reads the W
procedure and Unicode flag, sends a test message, replaces the window procedure with
a third native procedure and verifies that third procedure is callable.
The helper then restores its saved first procedure. The source window exists
only to produce a real Unicode CPD; all windows are disposable research
subjects. No helper HWND or product synchronization mechanism is proposed.
No random timing, sleep-based probability, guest or owner desktop input.

transient-observability-r1 supervisor28460 starts2026-10-17T17:44:36Z.
Build6612 starts17:44:38.6537398Z with300-second deadline, exits0 after
10.2968683s; native probe21964 starts17:44:48.9725259Z with30-second deadline,
exits0 after0.4553272s. Existing research assertions remain. New marker:
TRANSIENT_DESTINATION read=1 encoding=1 message_result=B2 update_lost=1
install_error=0 restore_error=0 failures=0. This means the hazard was
deterministically observed: the message entered the temporary Unicode target,
then the later restore overwrote the native owner's third-procedure update.
Exit0 is successful counterexample verification, not a repaired CPD gate.

Reviewer disposition: a safe fix must preserve application message entry and
original callback/control lifetime, not only independent token retention.
Excluding native reads/writes/destruction alone would not make this proposed
transaction correct. Do not ask for or infer approval of that insufficient
boundary, install a project lock and call it native atomicity, or mask this
failure with timing. Current runtime, mirror and release remain unchanged;
the ten existing source-retirement failures remain the last product CPD
result. Live ASCII A/W checks do not additionally prove arbitrary Unicode or
exact native encoding identity. Those remain explicit full-contract checks,
not reasons to weaken the setter's accepted inputs.

### Native ownership cohort: preserve original source type before host entry

The failed transient design motivates a structural repair, not another
normalization trick. Re-read pinned wuwind.c::WU32SetWindowLong: the original
native-thunk and real16:16 branches are explicit immediately after
IsThunkWindowProc. wuclass.c::WU32SetClassLong has the same split. The adapter
currently discards this distinction and stores a logical procedure below a
permanent native gateway. Native USER therefore never owns the restored
procedure/encoding, and retaining the source CPD cannot reproduce the original
kernel's assignment. Preserve the original branch type across this service
seam; do not infer it again from a DWORD whose namespaces collide.

The native input route, including unknown/native A- and W-query CPDs, uses
one real native setter. The guest route supplies a native entry for exactly
its original encoded procedure/callback pair. Reverse translation of only
our own entries gives the original caller its saved guest identity. A native
old target still takes original GetThunkWindowProc. This preserves both input
contracts rather than rejecting native CPDs or treating every tagged value
as a guest. Selector7fff's accidental foreign-CPD interpretation is an inherited
carrier-collision defect at this seam, not a required guest feature; the
source comments explicitly call that branch a real16:16 procedure. Register
the typed original-host correction under the owner's standing repair authority
and prove both contracts with colliding words, not by removing either test.

The original getter also distinguishes actual native subclasses from encoded
WOW procedures. NTW16S's comctl entry is an actual native subclass and must
participate in the original saved-previous chain. Current private-access
get/set instead bypass that outer entry to read/mutate a hidden leaf. Merely
exposing the outer entry without changing the setter could cause the saved
previous native chain to reenter the newly selected logical guest procedure.
Replace getters, setters and dispatch selection together. No NTW16S renderer
or private comctl-state reader is required by this ownership design.

ADAPTER-WOW-072 now stages an unselected native-entry module and declaration.
It inserts one immutable descriptor into the selected x86 stdcall ABI and
tail-jumps to the common scoped original-dispatch entrance. A saved entry
does not consult an HWND's newest target. Published descriptors live for the
worker/provider lifetime, with no class/window-based premature reclamation or
fixed entry quota. Construction uses RW memory, then RX protection and cache
flush before publication; duplicate concurrent construction discards only the
unpublished duplicate. This supplies no CPD decoder, helper HWND, new window
behavior engine or guest execution engine. The original source alternatives
and smallest missing ABI are registered in the WOW32 owner README.

MVDM-HOST-DIV-339 registers the prospective four typed service selections in
the two original host callers before editing them. The shared working plan
records the finite class/registration, immutable dispatch, existing first
adoption, thread-terminal observer, canonical cleanup, source selection and
joint acceptance closure. The original class setter must also stop its WOW
word update on a failed native procedure mutation; do not manufacture a
successful shadow state after the host rejected the change.

Second-role status: the module remains unselected. Its common immutable
dispatch entry now reuses the existing original task/borrow/graph/error scopes;
only explicitly typed saved guest targets bypass the current-target FFFF
comparison. Legacy dispatch callers retain their previous comparison and
terminal-owner behavior. No build or test is run for this partial group,
and no existing production entrance is switched to it. Complete
all the named dependencies before compiling and testing together. Canonical
terminal cleanup must be verified independently of successful normal callback
entry, including swallowed notifications, task cleanup and creation failures;
the earlier native return-observer evidence alone is not proof of that full
binding. Prior full-link-r15 and app evidence retain their exact input scope.
All original guest/media and the published P16 package remain unchanged.

### Native-slot source integration — typed original callers and terminal owner

Implementation continues the finite WND replacement cohort; no additional
semantic waiver or guest change is used. This entry supersedes the immediately
preceding statement that no original caller is switched in source. The new
entry module remains outside the selected build graph, so this is unfinished
source integration, not a candidate build or delivery.

Source-role implementation:

- DIV-339 now covers two getters as well as four setter branches in the two
  original MVDM host files. Input origin comes from original IsThunkWindowProc.
  Output origin is recovered only from an exact owned immutable native entry.
  The original four old/current return tests keep that origin, so a decoded
  guest selector7fff is not subsequently treated as a native FFFF CPD. Original
  far encoding, thunk creation and WW/WC updates otherwise remain unchanged.
  Class setter failure stops before SETWC, as WND setter failure already did.
- wow_user_get/set_procedure supplies the typed leaf. Native inputs go through
  one actual native setter; guest input prepares the immutable entry first.
  The saved old native value is decoded only when it is exactly our entry.
  Public untyped procedure setters no longer guess guest intent from bit31;
  they use the native contract. Existing private WW/WC/extra/style policies
  remain separate. Caller error is restored before the native call.
- WU32RegisterClass's existing output callback uses a typed guest registration
  entrance. The original client/server registration bodies remain selected.
  Scoped TLS origin is restored in finally, including nested registration;
  known guest FFFF input is not sent through the native CPD validator. Native
  registration receives a real procedure/immutable guest entry, not the fixed
  HWND-current gateway. Generic native registration still normalizes its CPD
  address through the preexisting public class-registration owner.
- Class target selection queries its actual native registration. Unchanged
  imported system classes still select the recovered original control bodies;
  this is not replacement by modern native controls. Mutated classes select
  their new real target, without changing existing WNDs.
- Creation selects the corresponding real entry/procedure. Rollback retains
  both previous and installed native identities, and restores only while the
  exact enrollment and installed value still match. First native-WND guest
  adoption prepares its immutable entry before publication and native exchange.
  Task cleanup patches the actual native WND slot before destroying windows.
- Task admission installs a thread-local WH_CALLWNDPROCRET observer before
  creation admission. Failure rollback and successful task retirement each
  release it through the existing task owner. The observer captures the exact
  binding/window before forwarding other hooks, then reuses original task,
  borrow and canonical control/dialog terminal cleanup. A downstream hook
  cannot redirect that retained generation by replacing the HWND/message.
  Synthetic NCDESTROY remains the old terminal-message contract, not proof
  that native USER has freed the actual window. Genuine destruction, creation
  failure, swallowed notification and handle reuse remain separate tests.

Second-role review caught the following before any build:

1. Returning only a decoded DWORD would recreate the guest/CPD ambiguity in
   the original old-value branch. Expanded the registered seam to six calls
   and four local origin tests, preserving original conversion algorithms.
2. Hook forwarding can reenter and replace a window: capture the HWND and
   retain its exact generation before forwarding; do not resolve it afterward.
3. A failed owner lookup changed LastError before first adoption: restore the
   service entry error before adoption/native assignment.
4. Mirror files are -text inputs. Restored their baseline LF byte convention
   after patching; the actual added mirror diff is 37+/4- in wuwind.c and
   39+/4- in wuclass.c, not a whole-file newline rewrite. Both are original
   host code, not guest. DIV-339 was updated before the source edits.
5. Explicit origin must control the outer tagged/native selection too, not
   only the inner FFFF test: a high native address is still native. The four
   return conversions now retain that distinction while preserving the
   original guest selector decoding and native GetThunkWindowProc bodies.
6. A downstream hook can retire the whole task. Recheck exact live enrollment
   before entering its task scope; an already retired retained binding is
   released without invoking another finalizer or resolving a replacement.

Finite remaining integration before the joint build:

- Retire the superseded logical-current APIs/forwarder selection and migrate
  their fixtures to actual native slots with explicit origin. Tests currently
  expecting an outer native subclass to remain hidden must instead verify
  real saved-previous chain forwarding/restoration and immutable old entries;
  retain all encoding, lifetime and failure assertions, not their old transport.
- Finish class/WND projection consumers and the non-enrolled native-class
  boundary audit. Do not invent a persistent native class snapshot as authority
  or claim private WC storage exists on an arbitrary native class. Preserve
  already supported native assignment; prove guest future-creation closure.
- Compose the new entry with product and fixture graphs together, and align
  manual-runtime fixtures with the independent terminal owner. Verify failed
  admission, CBT veto, WM_NCCREATE/CREATE failure, real/swallowed terminal edge,
  task exit, cross-task scopes and reuse before publishing any result.
- Run the already planned complete native CPD A/W/text/source-retirement,
  previous-entry/subclass and original-control dependency group, then product
  app acceptance with the coherent style package. No isolated entry-only test
  is being substituted for these gates.

No compilation/runtime test has run on these changed inputs. Earlier ten CPD
source-retirement failures remain the last failing runtime evidence; none is
declared fixed yet. P16, release assets and guest media are unchanged.

Static/document checks only: `git diff --check` passes. Hidden owned
native-slots-stage-governance-r2 exits0; its recorded verifier PID37500 starts
2026-10-17T18:28:27.2547603Z with a180-second deadline and reports documentation
governance passed. Supervisor5456 also exits0. The first launcher44116 produced
no retained verifier output/run directory and is not counted as a pass. These
are not compilation or runtime evidence; no new product package is selected.

### Native identity core selection — original CLS ownership and old WND removal

The next source-role pass traced original cleanup.c (module selector matching,
zero-window class restoration, deferred registered-class destruction and WND
neutralization), not just normal Get/Set callers. It disproves the prior
proposal to move *both* WND and CLS selection into public native setters.
Original CLS can be restored with zero HWNDs; SetClassLong requires an HWND.
Native class recreation would add lifetime/extra-data/atom problems, and an
auxiliary HWND is rejected. Keep original class behavior at its original owner.
This corrects the staged design before any runtime delivery, not an approved
loss of original class restoration or a new native-private mechanism.

The ownership split now selected in source is:

- Actual native WND slot: current window procedure and native CPD address/
  conversion ownership. Saved guest entries are immutable. No HWND-current
  callback/cache/forwarder decides what an old saved guest entry calls.
- Original CLS: class behavior, WOW words, original lookup/module restoration/
  deferred destruction; native class registration is its resource/creation
  carrier. CBT snapshots that class behavior into the real WND before its first
  procedure call. Explicit class mutation affects future, not existing WNDs.
- Native class input outside an owned original CLS: native public contract;
  no invented private WC record. Guest mutation remains rejected at that
  preexisting unsupported boundary, rather than installing a raw guest word.
- Original cleanup selects all procedures. OPENNT-HOST-122's five storage
  leaves apply three WND writes to the actual native slot and two CLS writes
  to original behavior with native origin. One selector guard excludes native
  CPDs/high addresses. Tree preparation projects current native WND selection
  before cleanup; it is not another continuously maintained current value.
  All six mirror lines are registered; no original loop/order is replaced.
- The task post-message observer retains the exact generation before other
  hooks, rechecks enrollment after their reentry, then uses the canonical
  terminal owner. Original control/dialog finalizers already own their needed
  task/resource/borrow scopes. Removed the extra outer guest-entry acquisition:
  native metadata-only destruction must not depend on guest execution entry.

Deleted from production source: the mutable WND target exchange, its
native_callproc retention, mutable native forwarding procedure, callback copy,
logical-current selection and fixed HWND-current native dispatcher. Retained
read helper only reads the real native slot and decodes owned immutable entry
identity. Original DispatchClientMessage/W32Win16WndProcEx/CallBack16 remain.
The default-off entry trace moved to immutable dispatch; it observes no new
behavior. Class CPD normalization is still the original address-only contract,
not the deleted WND source-token retention.

Original native GetClassInfo high-bit handling was swept too. Current worker
PE characteristics are0102 (LARGE_ADDRESS_AWARE false), and its generator adds
no large-address flag. Native class-info CPDs normalize to addresses; valid
native callback addresses in this worker do not occupy the WOW-tagged half.
No additional mirror edit is justified for that consumer by the current
product ABI. This is a read-only PE/source finding, not a native execution test.
Sparse guest class publication now uses explicit origin, never a native high
value as if it were a guest callback address.

Fixture migration retains the old assertions' semantics, not their transport:

- Real native subclass becomes the actual previous procedure; forwarding it
  still reaches the old immutable guest entry after a new guest selection.
  Restoring that native chain is tested separately from directly calling the
  saved guest entry. No assertion forces the old hidden native transport.
- Native CPD tests keep both live A/W text paths, source unregister, two
  independent destination classes and separate destination retirement. The
  verifier now requires their explicit successful markers as well as exit0.
- Class future/existing/nested-create isolation keeps original CLS mutation,
  with explicit guest/native origin. Added a real active-CCPU class-procedure
  fixture row instead of running real creation against an inactive page mock.
- A live native CPD word is also explicitly registered as a guest identity,
  then applied as native and guest to the same WND. Source intent must win
  even when the numeric word is identical. Original WU32 WND-return fixture
  also covers selector7fff without accidentally allocating a native thunk.
- Manual-runtime fixture rows install the same independent terminal observer;
  raw native callbacks no longer traverse a fake fixed guest gateway. The
  native dispatch denial fixture continues to require zero guest acquisition
  for native invocation; real CCPU scheduling is verified by its own rows.

Core build selection adds wow_window_procedure_entry.c to the provider graph.
No guest/system image is modified or rebuilt. WUWIND/WUCLASS remain37+/4- and
39+/4-; cleanup.c adds6/replaces6 lines, with its baseline CRLF retained.
The pinned-original verifier reverses exactly three WND writes, two CLS writes
and one typed guard, then compares the entire remaining owner against source.
No broader textual normalization or success stub is introduced.

Finite acceptance still outstanding: new core build and native/CCPU cohort,
original app/WRITE deep paths and styled lifecycle closure. Source audit found
the latter needs explicit attention: NTW16S attachment/frame/control/theme
state currently releases from comctl WM_NCDESTROY callbacks; creation observers
only clear pending/scoped records. Original forced neutralization now writes
the real WND slot and can bypass that subclass cleanup. Audit/repair that owned
retirement path before styled product delivery; this is an S3 lifecycle gap,
not an excuse to transfer a foundation defect or claim whole S3 completed.

The source group is now frozen for native-slot-core-r1: full-link-r16 followed
by native procedure/CPD, original class/creation/cleanup, task lifecycle and
cross-task native-boundary rows. The runner records source hashes (including
tracked deletions), actual hidden child PIDs, start times and deadlines. No
new result is asserted here; prior ten retirement failures stay historical
until this new cohort actually executes. Release/P16 and guest media unchanged.

### Native-slot core source gate and r3 build admission

Core r1 and r2 both terminated with exit1 before invoking the compiler.
R1 identified mixed line endings in the selected user.h subset; restored
its pinned owner's CRLF mechanically without changing declarations. R2 found
the unchanged TYPE_FREE--TYPE_GENERIC table interrupted by admitted FNID/WF
declarations. No type value differed. Moved the tail of the original type
table immediately after TYPE_CALLPROC, retaining the registered declarations
after the complete table. The verifier itself was not relaxed.

Native-slot-core-r3 supervisor PID30184 started2026-10-17T19:18:29.6978987Z.
Its source audit PID9632 exited0; worker/provider full-link-r16 PID39760
started2026-10-17T19:18:36.4470848Z with2700-second owned deadline. Inputs
remain frozen for this cohort. Runtime results and publication are pending;
the two failed source audits are not compile or runtime acceptance evidence.

Native-subclass-retirement-r1 independently compiled a disposable native x86
probe under build, without guest or selected-source changes during the freeze.
Its hidden supervisor PID26664 ran a90-second owned child deadline. Exit0,
marker errors=0 before=1 after_neutralize=1 remove=1 destroyed=1
terminal_calls=0 data=4660. Native USER retained the subclass query/removal
metadata after an actual SetWindowLongPtrA replacement, and removal did not
overwrite the new procedure. Actual DestroyWindow bypassed the removed old
subclass, proving why relying on that subclass's NCDESTROY leaks owned state.
This establishes the public resource-release primitive on this host only;
the planned shared NTW16S terminal leaf and WOW integration remain unimplemented.

Full-link-r16 completed exit0: worker incremental3 steps and provider198 steps.
Worker SHA2567193903C429E5BF28432FA675B212370B218293253B0B458F562351DF15A0D83;
WOW32 SHA2561EDC6AB5BAE2FE455E0E58E696AE2DD01419430014891F02B45795C7C6B0F3FA.
These are candidate build identities only, not published release artifacts.

Core r3 native-dispatch-gate terminated1 at fixture link: both the intentional
denying test callbacks and newly reached real lifecycle object define
wow_task_callback_enter/leave. The production product link had no duplicate.
The fixture needs explicit renamed real lifecycle symbols, as already used
by window-procedure instrumentation; never /FORCE:MULTIPLE or assertion removal.
No runtime failure or pass is established by this row.

Window-procedure then completed exit0. Strict markers report class restoration
and isolation errors0, tagged_calls7, native subclass forwarded2/preserved1,
missing_owner_rejected1. Both unicode0 and unicode1 live transferred CPD rows
report text_calls4/errors0; both source-retired rows report two independent
destinations/text_calls4/errors0. This replaces the previous source-retirement
ten-failure result for this exact native fixture/core candidate, not for real
Win16 applications or the entire S3. Remaining core cohort is still running.

Core r3 class-client completed exit0 with WOW_FIXTURE_OK case=class-client.
The same supervisor then admitted class-query PID14572 at
2026-10-17T19:26:18.5359907Z. Session74199 was re-polled live; no observation
timeout caused a restart, and frozen source/selected artifacts remain intact.
The terminal integration design now specifies one shared DLL endpoint and
per-owner existing disposal reuse, normal-remove separation, in-flight/owner
protection and both native DLL/real CCPU acceptance. No product source was
changed while this core cohort is live.

Core r3 class-query completed exit0 at19:28:36Z. Strict output:
WOW_CLASS_QUERY errors=0 queries=6 logical=1 native=1;
WOW_CLASS_COMMIT errors=0 publications=3 rollback=1 original_task=1;
WOW_FIXTURE_OK case=class-query. This verifies this fixture's original/native
query split and publication/rollback inputs, not all class or guest behavior.
The same supervisor admitted class-procedure PID36800 at
2026-10-17T19:28:36.9753299Z; it remains the active frozen-core acceptance.

Class-procedure then terminated1 before runtime at the real CCPU driver's
compile, not the class fixture body: tests/mvdm-host/wow_page_domain_fixture.c
still contains seven old wow_window_native_proc references (632,639,643,790,
954,1023,2928). Earlier removal sweep covered adapter fixtures but omitted
this driver. Do not restore the deleted production gateway to satisfy tests.

Source-led complete driver migration inventory for the next unfrozen group:

- Callback-borrow row: explicit guest target origin, allocate/select an
  immutable native entry and call that saved entry in both owner-failure and
  successful zero-result assertions. Retain error and callback-count checks.
- Native creation/veto/control-child rows: assert the recovered canonical
  native procedure selected by the actual class/FNID, rather than a universal
  removed gateway. Preserve creation-veto and metadata/cleanup assertions.
- Menu-creation rollback class: use the resource creation carrier and its
  explicitly native reject_menu_creation target; keep both failure cases.
- Class commit row has a known encoded guest procedure: select the typed
  guest registration entrance; retain real original CLS page/word assertions.
- Native adoption row's encoded guest assignments need the typed guest setter,
  including old-origin assertions. Keep allocation-failure/no-native-mutation
  and actual native predecessor restoration checks.
- Manual runtime rows require a bounded fixture-owned post-message terminal
  hook, as the other migrated manual-runtime fixtures do. Actual admitted task
  rows continue to prove their production hook lifecycle, not a test replacement.

This is one coherent test-driver migration, not new production behavior or
relaxed assertions. The core runner continues the already admitted remaining
cases against unchanged frozen inputs; class-lifetime PID33100 is the next
admitted case. The supervisor session74199 itself is verified live.

Class-lifetime completed its native fixture with exit0, but both output files
were empty and its mandatory marker was absent: the verifier correctly failed.
Do not reinterpret exit0 as sufficient lifecycle acceptance. A separate owned
immutable-artifact capture diagnostic (supervisor5092, child9200,
2026-10-17T19:33:44.6174548Z,60-second deadline) used ProcessStartInfo,
concurrent ReadToEndAsync and both WaitForExit overloads. Same executable
SHA25616267C3E4415A37CAF20B485EF3C9C6FF064BDCFE03F7B0E4C86977016942CA2
again exited0 with empty streams and failed its strict marker. No recompile
or source/package mutation occurred, and capture-drain alone did not fix it.

Source/link attribution establishes the missing-output cause:
class-lifetime/fixture.map selects _printf at00471970 from original-softpc-host-
fixture-roots:fprt.obj, not the CRT stdout implementation. Original
softpc.new/host/src/fprt.c::printf routes through OutputString when !PROD and
returns0 without output for PROD; OutputString uses the debug pipe/debugger,
not captured stdout. The fixture's CHECK logger and final marker use printf.
The same source sweep finds class-client and class-procedure use it too.
Fix those test output calls to the already working captured stderr route,
and require class-client's real marker as well as its process result. Do not
change recovered original logging, weaken required markers or claim class-
lifetime's runtime state proven yet. The changed link closure exposed a test
observability defect; its discovery is not a demonstrated guest regression.

The runner also lacks the parameterless WaitForExit after its timed native
fixture wait. Microsoft documents that the latter does not ensure asynchronous
redirected-event completion, whereas the former drains it:
[Process.WaitForExit](https://learn.microsoft.com/en-us/dotnet/api/system.diagnostics.process.waitforexit).
Retain that small harness correction in the next group, but the independent
fully drained capture rules it out as the observed empty-output cause here.
Native-creation and standard-class both failed at the same already inventoried
page-driver compile before runtime; no additional product failure follows.

Core r3 task-lifecycle completed exit0 with captured production-cohort markers:
WOW_STANDARD_CLASS_ADMISSION projected=9 calls=10 retry=1 errors=0;
WOW_CONTROL_CALLOUT paints=2 texts=3 errors=0;
WOW_USER_TASK_LIFECYCLE errors=0 native_direct=5;
WOW_FIXTURE_OK case=task-lifecycle. This row is independent of the failed
real-CCPU page-driver compilation and does not certify its skipped assertions.
Process-retirement PID37176 was admitted2026-10-17T19:36:12.9707566Z.

While the core source remains frozen, core-doc-audit-r1 completed exit0.
Hidden supervisor26808 owned documentation-governance child30472 starting
2026-10-17T19:36:29.2659053Z and relative-link child32176 starting
2026-10-17T19:37:06.0463548Z, each with180-second deadline. Both strict
verification scripts report passed/exit0. This is a structural evidence/index
check at this snapshot, not source/runtime closure or an authorized publication.

Process-retirement terminated1 at link, before its assertions: its explicitly
documented conversion-row mock defines mvdm_softpc_wow_page_domain_handle_metadata,
now also present in the real page-domain object. The verifier already renames
seven other deliberately replaced native-fixture leaves. Extend that exact
namespace treatment only for process-retirement's additional metadata leaf;
retain all other original page owners. No /FORCE:MULTIPLE, archive-order
selection or production-symbol renaming is admitted. This is another finite
test composition correction, not evidence of a process-retirement runtime bug.

Source review of the changed hook lifetime also identifies an acceptance
omission before the next group: InitRollback currently injects WH_CBT and
WH_FOREGROUNDIDLE admission failures only. Add WH_CALLWNDPROCRET to that
existing loop, preserve zero-live-hook/thread/lock/TD assertions and require
three observed failures in its marker. Run the separate existing InitRollback
row against the coherent revised group. This tests the newly added terminal
observer's actual admission rollback, not a test-owned hook substitute.

Dialog-lifecycle terminated1 at link before assertions. Its existing fixture
defines12 page-domain storage/conversion leaves and13 explicit unexpected-
guest-call guards; newly reached real page/wow32/wcall16/walloc16/wparam objects
now collide with those definitions. Audit this whole fixture boundary at once:
retain its denial assertions and namespace only its explicitly overridden
definitions in separately compiled test copies, selecting one original owner
for every other definition. Never delete guards merely because the real
functions now link. This fixture proves its documented native/model contract,
not real CCPU allocation; the separately selected real driver remains required.

Native-send completed exit0 with two actual early0 sends/receives returning114,
standard-class admission projected9/calls10/errors0 and normal task-lifecycle
errors0/native_direct5. Nested native invocation is the next admitted row.

During the frozen acceptance, prepared (not applied) build-owned
style-terminal-draft.patch. Pure in-memory context application validates22
hunks across11 existing project-owned files; no source/build input is changed.
The draft shares frame/control/theme/attachment terminal disposal with normal
NCDESTROY, retains in-flight owners, guards independently replaced combo-list
attachments and adds the matching WOW-loaded endpoint. Its canonical terminal
call revalidates the exact property/binding after original finalizers, before
touching style state, outside the publication lock. This avoids retiring a
replacement HWND after reentrant destruction. Existing explicit removal keeps
its appearance-restoration contract. There is no new original mirror diff,
guest change, fake message, helper window or compiled acceptance of this draft.
Source-owner registration, actual application, real-DLL/CCPU tests and package
delivery are still required after the current frozen cohort terminates.

### Core r3 conclusion and registered grouped remediation

Supervisor session74199 completed exit1; core exit.txt records10 failing
cases of16, with no frozen-input failure. Passing cases are window-procedure,
class-client, class-query, task-lifecycle, native-send and native-foreign.
Remaining rows are not certified by these six passes. Native-foreign retains
its real external-thread alias/send/owner assertions and ends errors0.
No release files, setup ZIP or guest binaries were written.

Native-nested executes nested send and both timeout rows, then exitsC000041D
when its WNDPROC raises an exception it expects an outer SendMessage caller
to catch. A minimal native x86 probe with no WOW linkage isolates the cause:
native-callback-exception-r1 supervisor12912,90-second compile/15-second
per-case deadlines. No hook and CBT-only both catch the exception and retain
LastError447a4002; WH_CALLWNDPROCRET instead exitsC000041D before the outer
handler. This proves a native callback transport/policy effect, not by itself
a failed original WOW unwind. No process exception policy was altered.

Microsoft's [WNDPROC contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc)
describes differing uncaught-callback exception handling on WOW64/64-bit
Windows; it does not promise this outer catch. The earlier provisional
"observer cannot be retained" conclusion is superseded by this public-contract
review. Preserve the existing modern-USER ownership boundary and original
guest exception handling. Do not waive a supported adapter restoration rule.

The revised native-nested source now tests both required cases independently:
real native callback-local handling returns117 with LastError447a4002, and a
throwing native-service leaf runs through exactly the current production
WOW_NATIVE_CALL_AFTER/WOW_NATIVE_MESSAGE macros. The verifier extracts those
two actual macros into a generated build header and records source/header
hashes, rather than inventing a recovery implementation. Both held/unheld rows
still require error, execution owner, lock state and psmsSent restoration;
new distinct strict markers require callback handling and production-macro
unwind. This source change is not yet compiled or accepted.

Alternative research: native-destroy-event-r1 supervisor35608 builds an
actual x86 callback DLL plus console and GUI probes. Both retain callback-local
native exception behavior and report DESTROY_EVENT nc=0 valid=1 metadata=1,
then NATIVE_NCDESTROY, with exactly one event before DestroyWindow returns.
This rejects WinEvent DESTROY as a post-NC cleanup replacement, despite public
event delivery being synchronous in these cases. Original createw.c::xxxFreeWindow
explicitly sends WM_NCDESTROY before its canonical final-control fallback.
The [WinEvent API](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook)
and [event definitions](https://learn.microsoft.com/en-us/windows/win32/winauto/event-constants)
were checked; no WinEvent code is selected into product components.

After r3 terminated, registered ADAPTER-WOW-073 before applying the shared
NTW16S terminal leaf and canonical WOW call. Its source owner/ABI, exact-
generation check, restoration-free disposal and normal-remove separation
are now implemented. No new mirror diff or guest change. A multi-file patch
reported a late context failure after earlier files had been written;
inspected actual source, removed duplicate header declarations and applied
only missing C hunks. No blind rebuild or acceptance claim follows.

Also applied, as one still-unverified remediation group: captured stderr
markers for the three affected class fixtures; mandatory actual class-client
marker; parameterless drain after timed process completion; three-hook
InitRollback coverage; exact process/dialog test-owner namespaces and renamed
real lifecycle provider for the native denial fixture. PowerShell parsing and
diff whitespace checks pass. No fresh compile/runtime group has begun yet.
That implementation inventory is now complete in source, not yet runtime
proof: the real CCPU driver no longer references the removed fixed gateway;
the new real-DLL terminal matrix covers seven control families plus frame,
normal/neutralized/reentrant cleanup, wrong-thread rejection, repetition and
unload-before-destruction; window-borrow additionally selects the matching
style DLL for original dialog/button terminal cleanup after neutralization.
Class-procedure adds actual linked WU32Get/SetWindowLong and Get/SetClassLong
callers through the production thunk scope, not extracted bodies. Guest
selectors7fff with offsets4/8 take the original non-thunk branch without
fabricated selector mapping and prove the encodedFFFF collision. Native CPD
direction and lifetime remain separately asserted by the native proc fixtures;
this helper does not claim executing guest code or native-thunk allocation.

Single-person review before core r4: original caller signatures, packed frame
layout compile-time checks, original IsThunkWindowProc low-offset rejection,
restoration of original WND/CLS behavior words, exact generation check before
style cleanup, in-flight subclass refcounts and explicit-remove separation
were checked. Corrected an entry-cache comment that incorrectly assigned
original CLS behavior to native USER. No additional mirror/guest change.

The lifecycle verifier now initializes the MSVC environment once per case;
compiler variables are scoped to build commands and the full original runtime
environment is compared after restoration. Original flags and assertions
remain. No speedup is claimed before measurement. Prior r3 case durations
were129.59--174.08seconds for completed native fixtures; source audit2.15s
and aggregate link62.03s. Failed early CCPU compilations are not comparable.

Next frozen run: build/M0-T447/S3/run-native-slot-core-r4.ps1, core-r4 and
style-cohort-r2 evidence roots; source inventory includes new test/helper and
build-generator files. Both-width style group retains DLL boundary/gallery,
attachment30lifetimes/default/dialog/title tests and adds terminal matrix.
Full-link-r17 precedes19 core rows including InitRollback, CreationUnwind,
styled real-CCPU borrow and style policy. Each child has an actual owned PID,
captured streams and independent deadline; final source hashes must match.
These are supplemental gates, not ordinary app/DOS or publication acceptance.
S3/T447 and the full goal remain open.

### Core r4 environment rejection and corrected r5 verification

The first launch34936 terminated before any evidence root or test output;
it establishes no execution or pass. Relaunched with an owned supervisor and
retained process observer:42812, tool session22866, eventual exit1. Core-r4
sealed unchanged source hashes and recorded30 unsuccessful rows: eleven
style stages failed CreateDesktop ERROR_ACCESS_DENIED before assertions;
nineteen core rows failed the test harness's full environment restoration
check before their fixture. These are not product-contract failures or passes.
Cleanup source reconstruction and full-link-r17 both passed. Output hashes:
ntvdm EBED1F1BB4A76D19BAD271E4A6F79DC0FC03DB5C973FAD986D64344F8B5AE28E;
WOW32 36807EBAD732942E67EF0BE6DDDFDBC8D4A50E8CFEF2D9091ABE14F722F4277E.
No release/guest/ZIP update followed.

Normal-user permission comparison style-terminal-permission-r1, PID40412,
same r2 x86 DLL/EXE, passed seven rounds and56 post-NC callbacks, seven control
families plus frame, normal/neutralized/reentrant cleanup, wrong-thread
rejection, repetition and unload-before-destroy; GDI1/1. It independently
proves the earlier CreateDesktop rejection was the restricted test environment.
Further tests use normal user permissions and owned isolated desktops; they
do not operate the owner's desktop or require a new semantic approval.

Style-normal-user-r1 supervisor36364 stopped on the existing default-button
GDI assertion after DLL boundary/gallery, title preservation and attachment
tests passed. Style-normal-user-r2 supervisor40836 completed all remaining
matrix cases instead of hiding later results: title, attachment30lifetimes,
dialog and terminal pass at both widths. Both terminal rows report56callbacks
and GDI1/1; dialog GDI46/46. Its4 failing rows are the same default-button
assertion against both r1 and r2 DLLs at both widths. The independent
default-gdi-sampling-r1 supervisor27048 retained identical assertions and
actual DLLs: raw and message-drained x86 sampling both report GDI16/19 and
fail for old and current DLLs. The diagnostic-only font-recreation option
reports2/4, not an acceptance pass. The cause of this older sampling/lifetime
failure remains unproved; do not silently waive it or blame terminal cleanup.
The permanent test now sends its count to stderr before ExitProcess so a
failed assertion cannot discard buffered diagnostics. No assertion changed.

After r4 ended, replaced build-scope environment mutation with per-child
ProcessStartInfo.Environment. One VsDevCmd initialization still supplies
unchanged compiler inputs, but the verifier's process environment is never
set/restored through text/null conversion. Parent environment equality remains
asserted, with PID/deadline/drained streams for every compiler child. R5
supervisor45012 starts a new frozen19-row cohort; cleanup audit/full-link-r18
pass. Early completed rows show16.50s native-dispatch-gate and16.26s
window-procedure, versus r3's138.02/129.59s; same assertions, new production
input identity, so this measures harness throughput rather than identical-
binary benchmarking. No runtime semantics or product pacing changed.

R5 window-borrow exposed a stale pre-migration test assumption at line1177:
it requires cleanup.native_cleanup for an actual original Button entry in the
fixture image even though that field represents native fallback, not original
control ownership. Original prepare_exit selects the retained client WND's
FNID first. The failed early assertion prevents later shared-CCPU marker rows
from executing; missing markers are consequential, not separate proved
mutation failures. Preserve r5 source until its whole cohort ends; then audit
the finite original-control/native-fallback oracle group before correction.
No product fix or restored typed-caller runtime proof was claimed at that point.

### Core r5/r6, real caller repair and current application evidence

Core-r5 supervisor45012 completed11/19; core-r6 supervisor29000 completed14/19.
Both source freezes finished unchanged. Core-r6's passing rows include the
actual CCPU window/control/menu graph group, native dispatch/procedure,
class client/query/lifetime, task/process retirement, native send/nested/
foreign, InitRollback and styled CCPU terminal cleanup. The styled row
reports actual attachment followed by neutralization and dialog/button
retirement, GDI6/6. This is stronger than independent DLL disposal alone,
not complete modal or real-guest acceptance.

Finite fixture repairs, without another production edit:
- Original control canonical ownership is asserted through retained client
  FNID; native fallback remains mandatory for native-owned paths. The old
  demand for both owners was removed, not the canonical-cleanup assertion.
- Native-creation includes the existing typed API declaration header; its
  missing prototype had selected cdecl names instead of actual stdcall names.
- A native-owned implicit dialog that enrollment does not subclass now
  strictly requires previous_native=NULL, installed_native=the existing
  target, unchanged procedure and A/W encoding after rollback. It must not
  undergo an unnecessary SetWindowLong normalization to satisfy the former
  fixed-gateway test. The separate native-only normalization control remains.
- Class-procedure receives the same controlled task identity as the other
  real-CCPU native fixtures. A duplicate unguarded original wowuserp.h include
  exposed by the new lifecycle header was removed, not patched in the mirror.

Native-slot-model-repair-r1 supervisor2364 passes native-creation and the
separate CreationUnwind row; its class row failed the duplicate header build.
Typed-original-callers-r1 PID42108 then passes class-procedure, callbacks40,
and four actual linked WU32 entrypoints: setters4/getters4, guest FFFF collision,
no fake selector mapping/execution or extracted original function bodies.
The combination covers17/19 core contracts with unchanged full-link-r19
production inputs. It is not one all-passing frozen cohort and must not be
reported as19/19 or S3 closure.

Two partial native-model rows remain open. Dialog-lifecycle's new original
creator first encountered absent registration input: diagnostic observer
dialog-exception-r1, supervisor39536, child17404, records indirect EIP0 in
InternalCreateDialog, return RVA2349e. Explicit native-only version input
removes that absence, not a production callback replacement. The verifier
selects one test-specific registered-input getter and retains every other
real bridge definition exactly once. No claim of real guest registration.
Further diagnostic r2 PID568 and trace r3 PID30916 recordC0000420 from
publish-native-return/status5023: this numeric mock lacks the complete graph
required by the selected original dialog. The style-policy model retains
task/peer Off propagation and dynamic captions, but its three original modal
calls fail with5023. Do not change product ownership/publication to accommodate
these incomplete models, or silently waive their unique assertions.

Current-apps-r12 supervisor13852 builds a private coherent13 candidate from
the immutable guest ZIP and hash-verified P16 artifacts, overlaying exactly
ntvdm/WOW32 from full-link-r19 and NTW16S32/64/W16Style from the actual r2
style artifacts. Matching DLL boundary, title, attachment, dialog and terminal
evidence is required; its limitations.json explicitly retains incomplete
native-model/default-GDI rows and does not claim a full style-cohort pass.
Both real WINMINE and SOL pass gameplay and normal exit Off. Candidate styled
modes, WRITE, DOS17 and publication/commit/push are still unexecuted gates.
No original guest was changed or rebuilt; release assets/ZIP remain unchanged.

Default-published-baseline-r1 supervisor30928 uses identical diagnostic
default-button EXEs against actual published P16 and current DLLs at both
widths: each reportsGDI16/19 and the same line73 failure. This establishes
no new lifetime regression for this assertion, not its root cause or repair.
Message-drained sampling also failed earlier. Keep the exact old failure
visible; do not count any such row as a pass or replace its assertion.

The next bounded verification replaces assumptions with formal task/CCPU
state. Original-modal-CCPU-r1 supervisor44292 used the controlled creation-only
task and both rows timed out, with no capability pass. Source current_task
requires InitTask's host_context->owner; the manual creation thread does not
provide it, so modal Peek/Wait cannot be proved there. R2 supervisor45924
now runs the same original A/W modal manager only after full lifecycle
initialize/register/InitTask, asserts return49, actual style attachment and
retained PDLG retirement, then performs normal task exit/disposal. No helper
HWND, guest change, production bypass or semantic waiver was added. This new
source/fixture remains unaccepted pending its result.

Mirror budget unchanged: wuwind37+/4-, wuclass39+/4-, cleanup6+/6- versus HEAD;
the prior DIV339/OPENNT-HOST122/ADAPTER-WOW072--073 registrations still own
those changes. Later repairs above affect test/input orchestration only.

### Real modal/registration ownership and ordinary application checkpoint

Original-modal-CCPU-r2 completed both rows with timeouts, not passes. The
stage-r3 probe PID11084 confirms runtime initialized, lifecycle registered,
task admitted, modal entered and WM_INITDIALOG called, but no WM_TIMER.
Source explanation is exact: wow_user_registration_bridge.c's A/W Peek and
Wait wrappers use its private wow_lifecycle. A separately initialized owner,
even with valid host_context, cannot satisfy current_task(&wow_lifecycle).
No production rewrite or special case was justified by that test mismatch.

R4 supervisor26204 instead obtains the existing registered owner through
real UserRegisterWowHandlers and pfnRegisterUserHungAppHandlers. It enters
that owner's original InitTask directly because launcher request metadata is
outside this native-only test; it does not invent a delivered launch or guest
TDB. Original memory/version/dialog callbacks remain the real W32 callbacks;
the optional guest-window leaf is an explicit rejecting/controlled native
fixture callback, not guest execution. Cleanup uses pfnWOWCleanup and the
same registered lifecycle disposal. Both A/W modal loops then deliver actual
WM_TIMER, return49 and retire retained original PDLG; Off plus Win16/All
attachment assertions pass. The whole R4 rows still fail: inserting a real
process lifetime inside another class-memory baseline perturbs that baseline,
and one HWND reuse setup fails. These are not passing aggregate runs.

The formal process-lifetime row is now a separate OriginalModalOnly scope in
the durable lifecycle verifier, with separate mandatory actual-modal,
registered-owner/normal-cleanup and page-domain completion markers plus
exit0. It does not run or claim the unrelated full class/window graph matrix.
Ordinary window-borrow retains its previous assertions, including allocation
and reuse. The first dedicated r5 rows complete every behavioral assertion
but exit4 because native USER/UxTheme auxiliary GUI threads outlive the test
thread and prevent CloseDesktop. The fixture now uses the same owned-child
process isolation already used by the real-DLL attachment test: parent never
joins the desktop, child performs full native/CCPU task lifetime, parent waits
for the entire child and closes the desktop successfully. No helper HWND or
process exception-policy change is involved; failures still propagate.

Modal-scope-regression-r6 supervisor21276 passes all3 rows with frozen source:
dedicated Off A/W modal, dedicated Off/Win16/All A/W modal, and the independent
whole window-borrow matrix. Both modal rows have result49, actual timer
delivery, retired PDLG and successful registered task/process cleanup. The
last row again proves original callback/control/menu/graph behavior without
inserting a second lifetime into its accounting interval. Its actual managed
slot reuse takes7 attempts for mode0 and1 for mode1. Earlier r5 full rows
failed to obtain a particular native slot within15seconds (661--758 attempts),
so graph-negative assertions after that prefix did not execute. A public-only
old-slot owner enumeration was added for observation, not a weakened reuse
assertion or a proven product root cause. Current-graph-negative-r1 now runs
the unchanged negative contracts serially, without concurrent application UI;
result remains pending. Preserve all prior non-pass outcomes.

Current-apps-r13 supervisor15152 passes real WINMINE/SOL gameplay and normal
exit Win16/All using the unchanged exact r12 candidate13 package. Combined
r12/r13 therefore retain all6 application/mode gameplay frontiers. Current-
write-r3 supervisor30048 verifies that same package's WRITE startup Off,
Win16 andAll:4 samples per mode, main/document/pageinfo all present. The
assertion explicitly says editing=unverified exit=unverified; intentional
observer cleanup is not a normal WRITE exit. O:/tmp physical stages and the
existing isolated-package owner provide runtime isolation; no O:/winnt tree
was cloned, no original guest written/rebuilt, and no assets published.

Post-core-doc-audit-r1 supervisor13080 passes documentation governance,
relative links and diff whitespace checks for its captured document state.
Later evidence/status updates require their final audit again. The active
packet remains S3 and the goal remains active. Full-link-r19 production
outputs still match the tested candidate; new modal/fixture repairs change
no runtime artifact, wire ABI, mirror budget or release manifest.

Finite remaining verification work: reconcile the two old partial native-model
contracts with the now-tested real registered-owner/CCPU producers, retaining
their unique publication-failure/veto/physical-allocation/callback recovery
assertions or mapping them to an actually executed stronger production test.
Do not delete a failed model solely to obtain a green count. Then run a final
frozen affected group, DOS17, S3 requirement/diff review and coherent13 delivery
before commit/push. Full message semantics stay S4 and individual termination
S5; genuine S3 lifecycle/callback gaps may not be transferred there. The
pre-existing raw default-button GDI16/19 failure remains explicit, not repaired.

### Final native-model retirement and durable grouped verification

Current-graph-negative-r1 PID4132 passes the complete existing CCPU graph/
discovery/failure group without changing assertions or budgets. It includes
external-observation churn, consumed-once post-native rejection, callback
owner/borrow/window-cache/dialog-cache restoration fail-fastC0000420 and
held/unheld discovery mutation/exception cleanup. This complements normal
callback recovery; it is not evidence of full S4 message identity semantics.

Retired the obsolete numeric dialog model only after this explicit mapping:

| Old unique assertion | Selected replacement and proof |
| --- | --- |
| Normal create/destroy, WW/PDLG available during callbacks | Actual original creator plus real CCPU callback HWND/backing, EDIT child and retained PDLG state in wow_dialog_real_lifecycle.h. |
| Nested destruction preserves outer callback cache | Same real producer; WM_APP110 enters111 then destroys itself, checking actual TEB callback identity/backing before and after native destruction. |
| Callback publication refusal before execution | One test-only callback-window rejection in the actual page-domain TU; exact production dialog entry returns0/error8 without application callback or stale cache/context/owner. |
| Exception restoration | Exact entire production wow_dialog_finalization_binding.c is compiled with only its capture leaf able to raise after successful capture. Direct production dialog entry must unwind task/borrow/context/error ownership. No uncaught exception is forced across native WNDPROC. Native callback-local handling is also retained. |
| Native Windows callback exception policy | Existing public WNDPROC boundary and real native-nested tests, not the obsolete expectation that every outer SendMessage catch must see an uncaught callback exception. |
| Post-native restoration failure | Current-graph-negative-r1's actual dialog/cache failure children must terminate once withC0000420; never continue guest execution with stale state. |
| Four creation rollback/physical release cases | Actual dialog and original registered native-window creator, each failing one publication or rejected by CBT. HeapWalk observes allocation liveness; retained veto references release before requiring physical absence. Two publication failures/two vetoes, no extra application callbacks. |
| Task exit finalization/no dying callback | Registered task prepare_exit neutralizes procedures, destroys actual dialog/EDIT, retires retained original PDLG flags/pointer, and denies dying application callbacks before normal registered cleanup/dispose. |
| Style policy peer isolation/dynamic captions | Existing task/peer policy model retains those exact assertions; its required sibling process now performs actual original modal A/W Off/Win16/All result/style/resource checks. |

Dialog-real-lifecycle-r1 supervisor25432 first passes the real four-rollback,
normal/nested and task-exit group. Subsequent source adds explicit pre-entry
callback rejection and production-finally capture exception to retain the
unique failure requirements rather than merely deleting the old test.
Core-final-models-r1 supervisor4264 passes both durable public cases with
source/artifact freeze unchanged. dialog-lifecycle emits all3 strict
WOW_REAL_DIALOG_SCOPE/ROLLBACK/LIFECYCLE markers with errors0. style-policy
requires its real original-modal child to exit0 first, then requires the
unchanged dynamic-caption and same-runtime Off-peer/parent-retention markers;
both pass with original-control callouts paints2/texts3/errors0.

Removed tests/adapter-mvdm-host-out/wow/wow_user_task_dialog_lifecycle_fixture.c
and the no-longer-needed wow_native_model_registration.h. Removed the twelve
numeric page-domain replacements, thirteen renamed original guest-call guards
and competing test registered-input getter from this verifier. Actual original
guest-call implementations are now linked once. Retained ordinary fixture
seams for unrelated cases; this is not a blanket claim that no test doubles
exist. Historical failures and exact old semantics remain recorded above.

The only separate test instrumentations in the new lifecycle case are
wow_dialog_page_faults.c (complete current page-domain TU, one publication
failure plus pre-callback rejection) and wow_dialog_scope_faults.c (complete
current production dialog scope, raising capture leaf). No production fault
parameter, guest edit, helper HWND, permissive duplicate linker flag or
changed runtime failure policy was added. Its dedicated child scope now
prints no modal-success marker when that case did not run a modal loop.

Durable grouped entrypoint tests/observation/verify-wow-native-slot-group.ps1
freezes current source/deleted-input inventory and actual ntvdm/WOW32/style
artifact hashes, executes19 core cases plus graph-negative in order, records
per-case PID/command/deadline/duration/exit and verifies source/artifact identity
again. It explicitly labels native/real-CCPU supplementary coverage, not guest
acceptance. Native-slot-final-r1 supervisor26988 is currently running that
20-case set against full-link-r19 and matching style-r2 artifacts; no partial
count may become an aggregate pass before its final hash checks.

Candidate-dos17-r1 supervisor23716 did not launch product tests: its prerequisite
PID identity check rejected a timezone-losing ConvertFrom-Json date conversion.
No core process was restarted or killed. The corrected build-only runner parses
the exact ISO start string from raw JSON; candidate-dos17-r2 supervisor35516
waits on the same verified-live core supervisor with a45-minute deadline, then
requires its recorded success before starting Console17/Window17. It reuses
the exact current-apps-r12 candidate, existing G7 and observer, the established
P16 controlled environment profile and isolated O:/tmp physical staging. It
does not claim arbitrary inherited-environment acceptance. Results pending.

No new production edit in this model-retirement group; mirror budget and
tested package identities remain unchanged. No publication/commit/push yet.

Native-slot-final-r1 supervisor26988 has now completed20/20 with exit0 and
both source/artifact freeze checks satisfied. This supersedes r3/r5/r6's
partial harness/model outcomes for the covered native/real-CCPU group, not
their historical records or the unexecuted S4/S5/full-guest obligations.
The final case order includes independent graph-negative after style-policy;
the entire group uses the same full-link-r19 ntvdm/WOW32 and style-r2 DLL.
Corrected environment setup has no parent-environment mutation. Completed
ordinary native cases are approximately13--16seconds, versus r3's130--174;
build cache/input differences remain documented rather than misrepresented
as an identical-input benchmark.

Candidate-dos17-r2 observed that same supervisor finish successfully before
starting any endpoint/input case. Console PID27376 passes all17 default
routes with exact guest/output/exit assertions, including native streams,
direct/nested COMMAND/MEM and EDIT return followed by MEM. Window PID27428
is now executing the independent17 Window-input routes; partial outputs
are not an aggregate pass. Source binaries remain the exact r12 candidate.
Post-native-original-group-r1 supervisor39984 is verified waiting on the DOS
supervisor's exact PID/start; after that process finishes it will run the
existing30 original control/dialog/EDIT boundary executables as a whole,
reusing the call-boundaries-x86 incremental cache. It records a preceding
DOS failure rather than allowing its own result to overwrite that gate.
No active build/test was interrupted or restarted for an observation slice.

### S3 P17 recovery publication checkpoint, not T447 closure

Candidate-dos17-r2 completes both matrices with exit0: Console PID27376 and
Window PID27428 each produce exactly17 expected/actual matches. The exact
same13-image package was used by real app groups r12/r13 and WRITE-r3.
Post-native-original-group-r1 supervisor39984 then starts original-controls
PID36620 only after DOS completion. Whole140-step source build completes;
all30 original-body/boundary executables exit0, including original creator,
modal resource lifetime, get/set, keyboard/mnemonics, Button/default borders,
EDIT memory, drawing, navigation, finalization and native dialog initialization.
This supplements actual production/CCPU/guest evidence; doubles in this suite
are not relabelled as real guest execution. No runtime timeout was relaxed.

Implementer/reviewer checkpoint: the old partial model's unique assertions
have explicit passing replacements above. Source scopes select original
behavior and only adapt native resources/identity/encoding/lifetime. No raw
modern CPD/USER token is read as an NT4 private object; actual WND owns its
native procedure and original CLS owns class behavior. Review retained exact
scope/error/refcount restoration, generation checks after reentrant finalizers,
normal-remove versus terminal-style disposal and all private test-fault
selection. The new fault leaves are test-only, with one real production owner;
no duplicate-provider suppression or product success stub was introduced.
The release below is a recoverable S3 progress baseline, not an assertion that
all S3 audit rows, full S4 messaging or S5 individual termination are complete.

publish-recovery-baseline-r1 PID25656 validates all passing prerequisites,
current source/artifact freezes, all30 markers, both17 matrices, previous
release identities and exact tested-package hashes. Before writing it saves
27 recoverable files (release manifest plus both sets of13) under
published-recovery-baseline-r1 with backup hashes. Only host names from the
explicit13-image allowlist may be copied. It does not clone O:/winnt or copy
arbitrary files from that tree, stage guest from the installation, or overwrite
guest/ZIP inputs. It compares PE machine type for every image.

Publication passes exit0: all13 candidate/asset/system32 hashes agree. Exactly
five host images differ from P16: ntvdm.exe, WOW32.DLL, ntw16s32.dll,
ntw16s64.dll and W16Style.exe; the other8 retain their exact identities.
Key full-link-r19 identities: ntvdm E60645E8ED97196B07E3C962D2D0F94900778329DC8913E61A4320F69BB602EB;
WOW32 88261F5BBEC982FD91EC939C56072920034F5692C0E8A4828AEF850B9656CF0F.
Complete13 identities and restoration paths are in verified-manifest.json and
backups.json. acceptance-boundary.json corrects the obsolete candidate-model
limitations while retaining WRITE and S4/S5 limits and the P16-identical
raw-default-button GDI assertion as unproved, never a pass. No existing
project process needed closure; an empty pipeline produced no stopped-process
file, so absence of that file is not claimed as a process-lifecycle witness.

Deployed-style-boundary-r1 PID33548 completes with exit-code.txt=0, loading
the actual published x86/x64 DLLs and running published W16Style.exe on an
owned isolated desktop. This tests deployed paths in addition to byte identity.
Final documentation governance/diff review and commit/push remain. No guest change,
new mirror diff or unapproved semantic reduction accompanies this checkpoint.

P17 source delivery is ead0aa64a0854ec5cd3f9469c161740443618777, retaining
the complete admitted S3 implementation, original slices, reproducible tests
and synchronized release images. Independent second-role review checked the
typed original callers and their registered mirror budget, native procedure
entry identity, exact-generation terminal observation, original finalizer
ordering and NTW16S inflight disposal. Remaining finite audit rows are not
closed by this progress delivery; their requirements remain in the plan.

Final documentation audit r1's sandbox-launched supervisor44296 produced no
terminal result and is not a pass. Normal-user owned background r2 supervisor
14900 completes governance, relative links and diff checks with exit0. Staging
then exposes one previously untracked source EOF-only blank line not seen by
unstaged git diff --check. Removed that blank from wow_edit_state_bindings.c;
the staged diff is exactly one removed empty EOF line, with all executable
tokens and line numbers unchanged. No behavior repair, build-graph change or
different runtime image is hidden in this formatting-only post-freeze change.
The source snapshot inventory from the frozen tests remains historical rather
than being rewritten to match the commit. Staged diff --check then passes.
The final documentation-only delivery records this source hash and the passing
deployed-boundary result; final documentation gates are rerun before push.

### Post-P17 selected control-chain audit and actual Win16 acceptance group

The previous goal turn made delivery progress: ead0aa64a plus c1c97ce80 are
pushed, both publication destinations match all13 images, and the worktree
was clean. Re-read the fixed S3 inventory against those actual sources before
selecting more work. Earlier staged/pending statements are chronology, not
proof that a selected implementation is still disconnected.

| Actual chain | Original behavior and selected provider | Authority/lifetime and remaining acceptance |
| --- | --- | --- |
| USER16 CreateWindow -> WU32CreateWindow/W32CreateWindow -> canonical creation | walias.c standard-class classification; wow_class_words_import_native replaces behavior target with wow_control_client_procedure for Button/EDIT/ListBox/ComboBox/ComboLBox | Native class is resource identity; retained original CLS selects behavior. Explicit/internal enrollment precedes dispatch. Real Win16 multi-control interaction needs a grouped witness; original bodies are already selected, so do not add a second editor/control. |
| Control message -> scoped edit_entry -> original client body | btnctl.c; editec/editsl/editml/edecrare/edmlonce/edmlrare/edslrare; combo/comboini/combodir; lb1/lboxctl1/2/3/lboxmult/lboxrare/lboxvar | Shared task/borrow/native-return scopes; original PED/PCBOX/LBIV own behavior-private data. W32Local* and original guest DS callbacks remain the only Win16 allocation path. Native/CCPU tests supplement rather than replace actual USER16 SendMessage assertions. |
| Ordinary DestroyWindow or task prepare_exit -> terminal observer -> canonical finalizer | Original control WM_FINALDESTROY/cleanup and registered WOW lifecycle | Original finalization precedes exact-generation withdrawal and NTW16S terminal disposal. Need real guest ordinary-destroy and live-composite-on-exit evidence with exactly one task retirement, not observer cleanup. |
| USER16 menu APIs -> original WOW conversion/parser -> menu native service/publication | Existing original MVDM menu thunks and clmenu resource parser; native owns actual HMENU | Existing native/CCPU graph tests cover identity/reuse/rollback. Add a real guest create/attach/check/detach/RemoveMenu-preserves-child/destroy sequence; do not replace parser or native resource owner. |

No new production omission or source replacement is established in these
four chains. This group addresses a concrete evidence gap in fixed C1/C2/C4,
not feature expansion, a new S or whole-S3 completion. Remaining dialog-template,
rollback, nonclient and whole-audit obligations retain their previous scope.

Add independently authored win16_controls_probe.asm and a controls case to the
existing owned-isolation runner. It calls genuine USER16/KERNEL ordinal APIs,
checks Button state, single-line selection/replacement, multiline CRLF/line
indices, ListBox strings/selection, all3 ComboBox forms and menu lifecycle.
It verifies eight ordinary destructions, then leaves a fresh owner/composite
for normal guest task exit. Nine binary phase records, actual LocalAlloc/Free
callback traces and exact-once target-task retirement are required. Failure
reports are decoded before launcher success checks. Run Off/Win16/All serially
on the same published P17 images; unchanged original guests remain immutable.
No product hook, failure policy, mirror diff or guest binary is modified by
this test-only group. No build/runtime pass is claimed before execution.

Actual-controls-group-r1 supervisor8052 fails before runtime: NASM reads the
unspaced modulo token as a macro name. Fix only that assembler spelling and
retain r1. Group-r2 supervisor30120 then assembles/runs the whole new witness.
Off passes all nine phases, ten creations, eight ordinary destructions,
all3 combo forms, original EDIT allocation/free callbacks and normal retirement.
Win16 mode fails at first CBS_SIMPLE creation: guest phase4, error014c,
created5, combos0. The launcher exits0 despite the guest assertion, so the
strict binary-report check correctly rejects the case. All mode did not run.

Source attribution: controls.c install calls GetComboBoxInfo and rejects
failure before it can attach the combo list. attachment.c and draw_combo use
the same API. The recovered combo.c has no CB_GETCOMBOBOXINFO branch, and
modern USER does not own its PCBOX. Read-only analysis of this actual host's
SysWOW64/user32.dll export RVA3a800 finds an indirect jump through import
RVAef0c8, identified as win32u!NtUserGetComboBoxInfo. Thus adding a message
handler or expecting this public query to discover original private data
would not establish a fix. No binary is modified or selected as source.

ADAPTER-WOW-074 design registration: supply only this missing renderer input.
Keep original CBOX, editrc/buttonrc, spwndEdit/spwndList and interaction flags
as the sole behavior authority. A same-thread scoped WOW query copies those
fields into public COMBOBOXINFO, with exact retained WND/child identity,
cleanup rejection, task/borrow/error restoration and no application callout.
Native controls without original ComboBox state retain GetComboBoxInfo.
Do not read modern PCBOX, synthesize a competing child registry, replace the
original combo or add modern messages to its algorithm.

Add optional same-width Ntw16sBindComboInfoApi to the existing NTW16S native
service boundary, independently of Hook's scroll/theme trampoline binding.
One immutable function binding per DLL lifetime; conflicting rebind rejects.
All four renderer/attachment queries use this single service. Native/Demo
consumers need no binding and retain their public native default. WOW binds
its query after validating all required exports and before publishing style
readiness. Its code module already outlives installed style callbacks. No
shared CRT objects, guest pointers, process policy or new window is introduced.
The additive export leaves existing ABI2 layouts/functions unchanged; the
new WOW consumer explicitly requires it from the coherent matching package.

Finite repair/acceptance: query and all renderer consumers, cleanup/invalid
size/wrong-thread/state rejection and native fallback; true original CCPU
ComboBox field/child projection; both-width DLL default/bind/rebind behavior;
whole actual Win16 controls Off/Win16/All plus template dialog lifetime;
affected source/body/native/style/app/DOS/coherent publication gates. Original
MVDM/OpenNT mirror budget is zero for this binding. This is the existing S3
control/style collaboration contract, not an application-name special case,
semantic reduction or new feature. No pass/publication is claimed yet.

Read-only native-boundary input identity: C:/Windows/SysWOW64/user32.dll,
version10.0.26100.8117, SHA256
56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E.
Its import/jump observation is diagnosis only: no RVA, syscall number, version
check or private host layout enters the implementation. The runtime uses the
public native default for native controls and original CBOX for recovered ones.

The entire repair is applied before building: optional immutable service,
all four consumers, exact-state WOW query, required-export/style-ready binding,
both-width native default/rebind tests, and real CCPU original-child/rect/error/
cleanup/foreign-thread assertions. No standalone leaf compile was started.
Control-info-build-r1 supervisor24932 passes both33-step style builds but
fails provider compilation: original extent declarations were selected before
WND existed, and importing oleacc.h's full COM/RPC declarations collided with
the retained NT4 ABI. Fix include order using the existing extent-owner
pattern; retain only three named public state-bit scalars rather than importing
an unrelated COM/RPC closure. No original header or algorithm changes.

Control-info-build-r2 supervisor14200 verifies exact unchanged style source
identities and reuses those two passed builds; provider14596 recompiles the
affected state unit and links successfully, worker32936 reports no work.
Its complete output manifest records WOW32
13B8CCBC857DA632F9B40C3C559203F1C483254061F7E8F178DB9F8216FB0C57,
ntw16s32 BDDCB9D1FCFD111D4C0B9885502FD30F48BC558A23A5D8EA7D3DF546722DBD54,
ntw16s64 1AA8D4D22FCD196A671AA8F5F99B5D6C90CF47CB43B6673FA41D0AC3D1C8235F,
W16Style577162D28BDD97D18E8005C2B7BAFB6F2898098DAF49E9127666954C1A25DEBD;
ntvdm remains the exact P17 E60645E8... image. Existing provider DEF warnings
remain recorded. No older DLL is mixed into the new candidate.

Control-info-acceptance-r1 supervisor13408 runs frozen native/CCPU20, real
both-width DLL/demo boundary (including new binding assertions), original30,
then independently staged real controls and16-round template dialogs in each
Off/Win16/All mode. Window-borrow already passes the added actual CBOX and
child-identity tests; no aggregate pass is inferred from partial results.
Control-info-product-r1 supervisor42636 verifies13408's actual PID/start time
and waits on that live handle before any app/DOS endpoint test. It will require
the entire acceptance result0 before proceeding to WINMINE/SOL gameplay/exit,
WRITE startup and Console17/Window17 on the exact same package. WRITE editing
and normal exit are still not claimed. Current published baseline remains P17.

Acceptance-r1 completes native20/20 and both-width DLL/demo binding tests.
Original30 compilation then exposes a single isolated OFF-only modal model's
unresolved wow_control_get_combo_info dependency in load_titles. Add an explicit
rejecting/unreached boundary double to that model, never a successful fake
query. Actual CBOX projection remains proved by real CCPU. Acceptance-r2
supervisor9576 verifies unchanged production/artifact and DLL-test identities,
reuses those passed groups and reruns original30; all30 pass. The sole changed
model source is not a selected TU in any native20 compile/link command.

Actual controls Off/Win16/All now all pass, including previously failing
CBS_SIMPLE and the other two forms. The16-round common-dialog Off witness
completes all19 guest records without error:17 created,16 ordinary closes,
17 hooks, one intentionally live dialog and one successful normal task exit.
But resource unlock/free callbacks each occur16 rather than17 times. The
strict verifier rejects this; do not lower that assertion or reclassify the
failure as the old unlocated common-dialog exception. Product-r1/r2 therefore
stop at their prerequisite; no product regressions have been launched yet.

Source root: ead0aa64a introduces original ComboBox/other control selection
in wow_class_words_import_native (confirmed blame lines257--264), explaining
the newly exposed native-info mismatch. Its task-exit dialog terminal procedure
also replaces all dialog WM_DESTROY delivery with DefWindowProc. This bypasses
existing original wcommdlg.c::WowOriginalFindReplaceDialogProc's resource-only
WM_DESTROY branch. The MVDM-HOST-DIV-335 wrapper already supplies exactly the
early resource-cleanup scope that this original owner needs; it is no longer
reached after native dialog delivery is neutralized. Reuse it, do not invent
resource-free logic or allow dying application callbacks.

ADAPTER-WOW-075 registration, same finite C3/exit group: associate a trusted
resource-only destructor with the existing exact HWND binding when its known
callback receives WM_INITDIALOG. Common-dialog modal stack records already
unwind through their original synchronous callers; persistent Find/Replace
records own the original deferred WM_DESTROY release. No class-name, filename,
TD-layout guessing or second resource registry is admitted. Ordinary WM_DESTROY
consumes this association before the original callback; task-terminal
WM_DESTROY consumes/invokes it once, outside publication locks and under the
existing early cleanup permission. Clear before reentry, retain the exact old
binding, reject conflicting owners/foreign threads/late registration, preserve
errors and propagate failure. Native destruction, original PDLG finalization
and NTW16S terminal disposal retain their existing ordering. No guest or new
MVDM/OpenNT mirror diff; existing DIV335 body and resource algorithms remain.

Style-life-r1 independently passes all8 title/attachment/dialog/terminal cases
on these exact new DLLs. P17/current raw-default comparisons both report
GDI16/16 and exit0 at both widths in this run. Earlier16/19 failures remain
unresolved historical risk, not a defect claimed fixed by this query repair.

### ADAPTER-WOW-075 real native-dialog selection correction

Combined build-r3 passed (supervisor33580); WOW32 SHA256
277788111B0B86318F16ED0437F2575228192548DA786F4C5A7AC6D0CF2B586A.
Acceptance-r3 passed19/20 native rows. Creation-unwind's unchanged15-second
native HWND lowword reacquisition prerequisite failed (688 attempts), not a
proved product assertion. Acceptance-r4 reran the complete strict row with
unchanged inputs/assertions/deadline and passed. Its native-coverage.json
records19 initial passes plus one full-row replay, not uninterrupted20/20.
Original30, resource-guard and both-width DLL boundaries pass in r4; the new
real-original-dialog resource-owner negative/reentry/cleanup assertions pass.
The first guest attempt in r4 was rejected before guest execution because
release NTSRV36932 occupied the global endpoint. Under permanent owner
authorization, exact path/PID/start verified closure stopped NTSRV36932,
NTCON23824, NTVWM29836 and NTVDM19420. Evidence:
build/M0-T447/S3/control-info-live-release-close-r1/identities.json.

Acceptance-r5 reused validated prerequisites and again passed actual controls
in all3 modes, but its common-dialog Off16-round witness still had only16
unlock/free callbacks each despite all19 guest records and task retirement.
No product regression or publication ran after this failed prerequisite.
Resource-exit-trace-r1 could not create O:/tmp's physical stage in the sandbox;
it is not runtime evidence. Escalated background resource-exit-trace-r2 ran
the one-round witness, retaining a temporary diagnostic-only TerminalOwner
trace. The final registered resource owner has original FNID0, while another
window has original FNID029F; the resource owner therefore bypassed the new
terminal procedure. The diagnostic source/build-r4 and trace remain sealed.
That temporary trace is removed from production source after attribution.

Correction within the registered075 boundary: an explicitly registered trusted
resource owner selects the resource-only terminal procedure regardless of
whether the host-native dialog has original client behavior state. Do not
fabricate FNID/PDLG, classify by title/class name, or substitute an authored
COMMDLG resource-free algorithm. The existing original resource-only callback
retains its guest template/context release and runs before guest resource
teardown. Other standard control/dialog finalizers remain unchanged. An added
real host dialog fixture has no recovered FNID and must release its registered
owner exactly once; the earlier original-dialog and negative assertions remain.
Build-r5 and complete acceptance on this corrected source are pending, not
claimed passing. No guest, assets/winnt.zip or MVDM/OpenNT mirror change.

Build-r5 passes; final candidate WOW32 SHA256
8C7561AB54159DC02E0ACC374949A9C34C8A50CA80293E0A58AA03C8649BF121.
Acceptance-r6 completed all20 native rows with19 passes. The added native
dialog fixture initially attached WW but omitted task-owner publication;
bind/prepare correctly rejected it with ERROR_INVALID_STATE. This is a test
setup defect, not permission to relax the production owner check. The fixture
now first asserts that rejection, then publishes the real existing owner and
performs the same prepare/destroy/reentry assertions. Only this guarded header
changed; wow_page_domain_fixture.c includes it exclusively under
WOW_REAL_DIALOG_LIFECYCLE_FIXTURE, set only for this row by the verifier.
All19 other row inputs and production images are unchanged.

Acceptance-r7 validates that exact one-header difference (prior SHA256
9CDE26584100AE13ADF9717475BE09011D46AB9EE65F9259A0AD2FDBFC91E1E0),
rebuilds/replays the complete dialog-lifecycle row, and passes. Its
WOW_NATIVE_DIALOG_RESOURCE_OWNER, WOW_REAL_DIALOG_RESOURCE_OWNER, scope,
rollback, physical-free, cache and original lifecycle markers all report0
errors. native-coverage.json explicitly distinguishes19 reused rows plus
one repaired full row from uninterrupted20/20. Original30 also passes on
this source. Actual guest and product gates remain pending here; prerequisite
failures in product-r3/r4 ran no product guest tests. Product-r5 is queued
behind acceptance-r7, never concurrently using its isolation endpoint.

### P18 bounded repair delivery and proof limits

Acceptance-r7 completes with exit0: native20 coverage as qualified above,
original30, common-dialog resource guard, DLL boundaries at both widths and
six actual USER16 witnesses. Each controls probe has all9 phases,10 created,
8 explicit destroys, original EDIT LocalAlloc/Free callbacks and one task exit.
Each common-dialog mode has19 records,17 creations,16 explicit closes,17
unlock callbacks,17 free callbacks and one target task retirement; the final
dialog is deliberately left live for original resource-only exit cleanup.

Product-r5 passes WINMINE flag/counter/reset/open-cell and SOL deal/stock-draw,
WM_CLOSE and normal run16 completion in Off/Win16/All independently. WRITE
passes4 startup samples in each mode (main/document/pageinfo), not editing,
save or normal full exit. Console and Window17-row DOS matrices have17/17
Expected==Actual each, including COMMAND/MEM/EDIT and nested execution. The
controlled environment profile is preserved; arbitrary inherited environments
are not claimed. No endpoint-sharing real guest tests ran concurrently.

Publication39192 (2026-10-17T22:51:23.7554661Z) verifies all gates and source/
artifact identities, retains27 prior assets/target/manifest backups under
build/M0-T447/S3/published-control-info-r1, and synchronizes13 named host images
to assets/release and O:/winnt/system32. All three sets' SHA256 match. Only
WOW32.DLL,ntw16s32.dll,ntw16s64.dll,W16Style.exe change from P17; ntvdm and the
other8 stay byte-identical. No release processes needed closure at publication.
The earlier exact four-process closure is retained separately above.

Post-deployment boundary29244 (2026-10-17T22:51:29.9797596Z) passes both actual
published DLL widths, opposite-width rejection, duplicate/foreign-thread
rejection, immutable query binding, font restoration/unload, plus published
W16Style's28 pages, viewport movement and normal process exit. Full evidence:
build/M0-T447/S3/control-info-delivery-r1. Original guest/ZIP unchanged;
src/mvdm and src/opennt-host diff is empty. Existing075 reuses DIV335 without
adding another mirror exception;074/075 owned seams are registered in README.

Second-role review: registration metadata is not another resource registry;
original CBOX and original COMMDLGTD remain authoritative for behavior/data.
Native USER remains HWND/lifetime owner. No modern private structure cast,
helper window, callback-enabling cleanup exception, alternate resource free,
test relaxation, original guest change or additional semantic waiver is used.
The fixture-only enrollment repair adds its previously missing prerequisite
and negative assertion; it does not alter production or omit the old checks.
Boundary tests supplement real guest proofs, not substitute for them.

This P is not whole S3/T447 closure. The finite S3 chain inventory and remaining
shared/private dialog DS/full creation/behavior proof obligations must still
be reconciled against actual guest coverage. Hidden control message probes do
not prove pixels, physical keyboard navigation, every owner-draw control, or
all dialog cases. Full mixed messaging/reply/reentry remains S4, individual
task termination S5. Earlier unexplained common-dialog/default-GDI anomalies
retain their recorded limits; these two source-attributed repairs do not
retroactively prove those unrelated old anomalies fixed.
