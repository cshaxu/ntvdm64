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
