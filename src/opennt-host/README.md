# opennt-host

OPENNT-HOST-124 adds only two guarded projection calls to the original
kernel/userk.h EnterWowCritSect/ExitWowCritSect handoffs. The selected original
SHA-256 is9260E0BA3CC89974E10B3804ED66D18B9864F4AE7D1175CCE6DDE8EE79D66E8C.
Original CLIENTINFO is per native thread; a serialized CCPU cannot instead
leave the last task's version/callback pair in its one guest TEB. The existing
host contract transfers that bounded view before execution-owner assignment
and before release, including InitTask's forced initial owner. Original
selection, counts, waits, returns and destruction remain unchanged. Pure
native source models without a host have no guest projection; a production
host supplies the operation. No guest change or native private TEB decoding.
See ADAPTER-WOW-087 and the T447 S4 source audit for ownership/terminal rules.

OPENNT-HOST-123 registers the native-stack eligibility input at five selection
conditions in kernel/taskman.c (normal event scan, idle-hook temporary owner,
WOWEXEC hardware selection, task-locked self wake and final wait completion).
Pinned original: OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, taskman.c
E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E.
Original event counts, priority/list algorithms, task-lock policy and native
wait transport stay unchanged. Modern USER does not supply NT4 SMS/wake-mask
eligibility; HEVENT_REMOVEME's one-scan exclusion cannot supply that persistent
input. The existing bound THREADINFO carrier supplies only whether its native
stack is at an actual guest callback/continuation. This is not a new ready
queue, guessed private object or clearing of original events. The
non-scan source entries are guarded rather than silently assuming eligibility.
InitTask's newly allocated task is ready by construction; retirement removes
the existing original TDB under the same lock. Adapter077 owns LIFO boundary
state and replaces target-based pseudo-SMS handoff. Planned added mirror body:
five predicate operands plus local provenance, no standalone algorithm.
Grouped scheduler/callback/native/guest/lifecycle acceptance is pending; no
runtime delivery is claimed by this registration. No guest change is allowed.

Retrospective OPENNT-HOST-027 register completion: ntrtl.h's existing
RtlNtStatusToDosError/NoTeb aliases select the private symbol domain of the
already admitted OPENNT-HOST-016 original error.c/error.h mapping owner.
The modern NTDLL names cannot select that exact NT4 table. Only symbol
binding changes; the original status table, fallback/order and source-shaped
TLS LastStatusValue carrier remain unchanged. No new mapping implementation
or TEB-layout cast is introduced. This records the existing local027 marker,
not a new source change or a second mapping policy.

OPENNT-HOST-122 registers five storage assignments in kernel/cleanup.c,
pinned to OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c: three WND procedure
writes and two CLS procedure writes use named adaptation leaves. One WND
selector-match guard also requires the explicit guest-origin projection,
so a modern native CPD/high address is not treated as a guest selector. Original
selector/module/task selection, loops, native/default target choice and
deferred class destruction remain unchanged. WND writes must reach the actual
native slot after ADAPTER-WOW-072; CLS writes retain original behavior and
mark explicit native origin, including zero-window module restoration.
Native class registration remains the resource owner, not another original
class-behavior engine. Failed native mutation marks the existing cleanup
failure channel; no guest edits, helper HWND or private native layouts.
This registration precedes the five mirror edits; joint verification pending.

T447 S3 mirror-path audit moves the project-only controlabi.h and chartranapi.h
facades to opennt-abi/host-compat/include/wow_control_resource_abi.h and
wow_char_conversion_abi.h. Neither filename exists at its former pinned
OpenNT path. Required original declarations, packing and conversion-service
bindings remain unchanged; selected original bodies include the ABI facade.
This removes two invented mirror files, with no behavior or guest change.

OPENNT-HOST-085 production selects _GetWindowLong's nonnegative private-word
contract with WOW_ORIGINAL_PRIVATE_WORD_GETTER. Source/export/caller audit
finds only wow_window_client_try_get_long calling this internal symbol; it
rejects negative indices first. Public negative access remains with the
existing facade. The selected body's negative guard reports INVALID_INDEX
instead of linking unused GetWindowData and its NT4 server-procedure/class
mapping closure. No dummy provider or approximation of private flags. This
is an internal subset boundary, not a changed public negative-index contract.

OPENNT-HOST-121 registers the approved actual-dialog procedure split, pinned
to OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Owners are client/dlgbegin.c,
client/dlgmgr.c, client/ntstubs.c, rtl/getset.c and kernel/getset.c. Bounded
selection changes procedure initialization/get/set/invocation, WM_NOTIFYFORMAT
and message-result storage only; original behavior/resource algorithms remain.
Modern USER owns those slots on the actual HWND. Public APIs cannot recover
original raw CPD encoding/layout; the owner approves modern narrow semantics
instead of private decoding, helper windows or a competing procedure cache.
Native cleanup is separate from original PDLG cleanup. Full production and
lifecycle/guest acceptance remain open; the native contract probe is not
integrated runtime proof. No guest change. See the S3 source-audit ledger.

OPENNT-HOST-120 retains client/clmsg.c::GetCPD as an unchanged complete body
under WOW_ORIGINAL_CLIENT_CPD. Source SHA256
2F87967A05E730E2C95B8CE0B92443D323F76BAEC97446D3C4F8C9DC163710FD,
pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. The original HW
projection and existing named native GetCPD service supply its finite closure;
only the selected window/dialog consumer is admitted, not a native class
pointer cast. No algorithm/body edit or private token decoder. Other clmsg
functions remain outside this selector. Production object selection is staged;
complete get/set composition and grouped verification remain open.

OPENNT-HOST-089/090 default-worker composition now selects client/clmsg.c
in the production and supplemental graphs. The finite default-precomp facade
reuses retained WND, bitmap predicates, original character conversion and
rtl/wow.c client rectangles, with original user.h GetChildParent/DO_DROPFILE
and winuserp.h SHELLHOOKINFO/SWP operands. No EDIT-context dependency is added.
Subsequent source audit registers one89 continuation
guard: after WM_MOVE, do not send WM_SIZE to a destroyed/cleaned retained WND.
Copied storage cannot pin native HWND lifetime. Supplemental r24 compiles this
selection and passes its original-body boundary tests; real guest acceptance
and full production linking remain separate and incomplete.

OPENNT-HOST-119 admits original client/dmmnem.c's complete mnemonic cohort
for the recovered dialog manager: source SHA256
638EFC31FFD622BA31E16672D1418291C3E1291B6D0C57F31D08C32D45FA1720,
pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Original search,
duplicate mnemonic, focus and click algorithms are retained. Its finite
outbound closure uses existing dialog navigation, retained WND and native
call leaves. Import alone is not product selection or runtime acceptance;
the source ledger records callback-lifetime and keyboard-entry prerequisites.
Registered minimal repairs hold BUTN across mnemonic focus callbacks and
return NULL at the exhausted search tail. The same119 group recovers the
original IsDialogMessageA/W bodies in dlgmgr2.c under an opt-in selector;
source SHA256 A3E9286C40E4E11EC872FD9B2D7B23CBA640F09FB4840C4E6130E301D5277B9E.
Their ANSI conversion, scoped entry and lifetime dependencies remain required.
119 also restores the caller's ANSI wParam and the temporary BST_DONTCLICK
flag on callback exceptions using finally at their original owners. The
button scope keeps BUTN live until the flag is cleared; this preserves normal
ordering and does not suppress the exception. Keyboard-body acceptance is
still pending, separately from the lifetime-boundary fixtures.
119 recovers unchanged RtlMBMessageWParamCharToWCS in rtl/chartran.c.
Its THREAD_CODEPAGE input binds to the current native keyboard layout's
locale ANSI page, without copying a modern CLIENTINFO layout. The source
ledger records original producer provenance, fallback and pending tests.
119's mnemonic callout guards reject exact retained destroyed/cleaned state
after dialog-code/text/focus callbacks, before later native operations. They
preserve original successful traversal and do not revalidate a reused HWND.
The same119 group retains DlgSetFocus's exact target across query/selection
callbacks and guards keyboard/default-button continuation on dialog/target
destruction. Copied WND retention cannot pin modern USER handle allocation;
this adaptation does not imply an identical original NT4 failure.
The default-ID continuation also rejects a destroyed/cleaned target returned
by copied lookup before querying it. Supplemental r20 fails that assertion;
r21 passes after this registered119 guard. Real HWND/guest coverage stays open.

OPENNT-HOST-118 recovers unchanged _DefSetText and RtlInitLargeAnsiString in
their original client/ntstubs.c and rtl/chartran.c owners. The source ledger
records exact hashes, counted-string semantics and pending native-service
binding/acceptance. No message-based text replacement or guest change.

OPENNT-HOST-117 admits original client/btnctl.c unchanged (SHA256
579205CFBD7DA6CF97E05235C74B3A221415DD07AE6F73C30649C9BF9E06EF2E).
The source ledger records its bounded callback-state lifetime adaptation,
required declarations and complete creation/dispatch/cleanup dependencies.
Import is not product selection; full consumer and lifecycle acceptance remain
pending. The original lookaside allocator and button state machine are kept.
Its drawing scope selects the unchanged mngrayc.c BitBltSysBmp alone through
WOW_ORIGINAL_BUTTON_BLIT_ONLY; BltColor is excluded from that second object
to avoid duplicate linkage. The facade binds only the current Button frame,
not an enclosing DrawState frame. Helper bodies remain unchanged.
The original winuserp.h BS_PUSHBOX/BS_HORZMASK/BS_VERTMASK values are retained in the existing bounded
control declaration carrier for btnctl.c; it is not a new control policy.
Original user.h WFTABSTOP accompanies its Button opt-in declarations.
OPENNT-HOST-117 additionally registers post-color-callback liveness checks
and explicit captured-HWND DC release in btnctl.c. Logical destruction still
clears spwnd; already acquired DCs are released without dereferencing that
cleared backlink. The source ledger records remaining callout coverage.
The same entry checks logical lifetime after focus/capture/click callouts and
within radio traversal, rejecting further notification/drawing after cleanup.

OPENNT-HOST-116 selects original client/mngrayc.c ClientDrawState for the
recovered list/button callers. Scoped GDI resources replace global USER
initialization; effect dispatch and raster logic remain original. Registered
diffs correct UNION height, failed scratch selection and lock unwind (including
a null statement at the original exit label inside the new try block). See
the S3 ledger for scope, provenance and pending consumer/lifetime verification.

OPENNT-HOST-114 recovers unchanged PaintRect, BitBltSysBmp and BltColor
subsets at client/draw.c and client/mngrayc.c. OPENNT-HOST-115 adds original
PSMTextOut under an opt-in selector in rtl/drawtext.c, with registered
terminator-capacity, per-call scratch and exception-cleanup correction.
See the T447 S3 source ledger for hashes, dependency boundaries and pending
grouped verification. Neither entry selects complete button behavior.

OPENNT-HOST-113 retains only kernel/inctlpan.c's complete read-only
sysBmpStyles table (static linkage removed), not its USER server factories.
Source SHA2560E3999DF8610405C990760EE1A417BF312E3B76744F8F9387BBD94583F9D800D.
Original OEMBITMAPINFO/button indices are opt-in inc/user.h declarations.
The scoped native resource carrier consumes button rows only; the source
ledger registers the stale three-state alias correction and pending tests.

OPENNT-HOST-112 recovers rtl/text.c PSMGetTextExtent with a registered
temporary-buffer capacity correction. Original source SHA256:
D2DAE26E4766C2567225CB6BFC8147B4E80955B4CA49DCAF97B6193E7067A8E2.
Original prefix/extent branches remain; long inputs use existing USER RTL
allocation bindings with finally cleanup. See S3 ledger for failure contract,
packed-count limitation and pending grouped acceptance. No guest changes.

OPENNT-HOST-111 retains unchanged DrawPushButton in rtl/draw.c behind
WOW_ORIGINAL_BUTTON_DRAWING. Pinned original source SHA256:
7491E5B3331DE8CB7A2189B8EFBE91F56076F80A08338BF816D793E80A4FAF76.
Public GDI/USER leaves and existing brush resources supply drawing; display
depth comes from scoped control input, not an NT4/modern OEM overlay.
Not selected as a complete button implementation; group verification pending.

OPENNT-HOST-110 appends unchanged GetPrefixCount to rtl/drawtext.c from
pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c; source SHA256
D94876C3A091278D92030AFF49F37151C8F728F86F0B7D3F08C3F1F7199EA746.
Original PSMGetTextExtent/menu drawing require this prefix algorithm.
Only CH_PREFIX/MAKELONG and caller buffers are required; no native private
state or replacement parser. Staged, untested. The fixed-buffer measurement
caller is not imported pending its capacity-contract audit; see S3 ledger.

OPENNT-HOST-109 extends the existing dlgmgrc.c subset with unchanged
UT_PrevGroupItem, UT_NextGroupItem and _GetNextDlgGroupItem. Direct callers
are original dlgmgr2.c IsDialogMessageW and dmmnem.c xxxGNM_FindNextMnem;
the existing _NextControl/_PrevControl/_GetChildControl and retained WND graph
are the complete outgoing dependency of these three bodies. Pinned OpenNT
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c source SHA256 is
678D2CD4B79A15A79E6C0257B4BD93AD69FA7BE95A513B627565490256CB1AFC.
All three bodies match after newline normalization. No autonomous group
navigation or native GetNextDlgGroupItem substitution is introduced.
Original dialog message filtering/mnemonics are still unconnected; these
helpers do not justify substituting modern IsDialogMessage for that owner.
Supplemental group-wrap/visibility/disabled assertions are staged, unrun.
See the S3 source ledger for the finite keyboard-chain dependencies and
native-button private-state boundary; no new guest or behavioral diff.

OPENNT-HOST-108 recovers the dialog lookup/default-button dependency cohort
from pinned OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c:
client/wow.c _GetDlgItem (SHA-256
1ece9db0e35238434843bb73ece92dd66000a44bc6b1148a08bcabf1329f8d9a),
client/dlgmgr2.c _FindDlgItem, xxxRemoveDefaultButton and
xxxCheckDefPushButton (SHA-256
a3e9286c40e4e11ec872fd9b2d7b23cba640f09fb4840c4e6130e301d5277b9e).
All four bodies are unchanged. Crops exclude other client/wow services and
the separate IsDialogMessage entry cohort; this does not waive its ownership
audit. Original dlgmgr/dlgbegin and combo/list callers consume these routines.
Existing retained WND graph, general validation and scoped native SendMessage
are the complete outgoing boundary; original version gates, IDs and button
selection remain here, not in another adapter. Standalone precomp supplies
the admitted client ABI. Staged only until grouped lifecycle acceptance.

The same cohort retains unchanged client/winmgrc.c _AdjustWindowRectEx
(SHA-256 26f0174e6a0db53d942506cbdc12efbfebc34a943d780170e7bf9d95775514a4)
and rtl/winmgr.c NeedsWindowEdge (SHA-256
d7e87476cc3e703c94b88d53b9a0761642b284a6776c0152738daF4c74290d2b).
The control selector excludes unrelated fixture compilation; original task
version and existing native metrics feed the unchanged arithmetic. Dialog
creation/modal entry retains the existing default geometry context across
the original creator and restores its predecessor on every unwind.

OPENNT-HOST-107 admits only GetContextHelpId from
windows/core/ntuser/rtl/help.c at pinned OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c (source SHA-256
7ffe7d97c36f1aa6ab119eb7f0c9ca70fd8eb57cbefe322e40d0c5df041be16f).
The retained ComboBox WM_HELP caller needs its original child-parent versus
popup-owner inheritance and desktop stop. Unrelated dialog enumeration is
cropped. The sole changed expression reads the exact retained generation's
help ID instead of NT4's internal property atom/table. Native USER supplies
that scalar during the existing observation transaction; no property table,
native private layout or second inheritance algorithm is introduced.
The adapter is ADAPTER-WOW-063; fixtures cover traversal and retained values,
pending grouped execution. No guest change or runtime acceptance is claimed.

OPENNT-HOST-104 admits the four unchanged same-address REBASE macros from
windows/core/ntuser/rtl/userrtl.h, pinned revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. This is the original _USERK_
branch's pointer representation, not a kernel execution profile: recovered
host WND/CLS and captured text pointers already share one native address
space. The bounded subset has its own guard and excludes the unrelated RTL
environment and cross-address desktop formulas. The selected client facade
includes it explicitly; no global _USERK_ definition, guest pointer conversion
or host private-heap interpretation is added. Original EDIT, Combo/List,
dialog and default callers retain their expressions. This supplies address
representation only, not graph publication or lifetime. Fixture assertions
are staged and unrun; whole-body private-precomp selection remains open.

OPENNT-HOST-103 registers one call-target change in editec.c's
ECGetControlBrush different-thread default branch. Owner approves native
WM_CTLCOLORSTATIC default processing only when the retained recipient lacks
creation facts. Original parent/owner selection and thread branching remain.
The adapter converts the internal source PWND to HWND only for native entry;
known recipients and other messages still call the original default worker.
Unknown private WFWIN40COMPAT cannot be recovered from GetWindowInfo's
constant version on the inspected host; no fabricated flag or additional
USER-server import is selected. Budget: one call identifier plus local
DIVERGENCE comment; no guest change. Boundary assertions and whole original
EDIT selection/runtime acceptance remain unrun. This supersedes the blanket
unchanged-body description for editec.c in OPENNT-HOST-073 only.

OPENNT-HOST-102 admits the unchanged GetControlBrush client wrapper from
windows/core/ntuser/client/ntstubs.c and xxxGetControlBrush from
windows/core/ntuser/kernel/random.c in the pinned OpenNT revision below.
Only subset selection/include environment changes: original null-DC check,
popup-owner/parent/self selection and color-call ordering remain. The private
facade retains exact WND allocations in the existing call borrow scope and
translates the explicit color recipient to the existing native boundary.
It neither imports USER server locking nor invents a second relation owner.
Whole relationship/publication and real guest acceptance remain required;
these staged imports do not claim completed control support. No guest change.

OPENNT-HOST-101 retains the reached original inc/user.h EDIT/combo/list
style and private-frame selectors plus tiled/popup classification macros.
Source is pinned OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c,
windows/core/ntuser/inc/user.h:1695--1870 and 1928--1931. Thirty exact
constant lines, three exact extended-edge selectors (1755--1757), the exact
QF_DIALOGACTIVE queue operand (2358), and four
exact predicate lines extend the original-layout
opt-in; no algorithm, native private layout or guest state is introduced.
The whole 20-unit EDIT/combo/list/dialog selector census additionally recovers
seven exact style-selector lines (1747--1748, 1761, 1765, 1776, 1778, 1781):
WEFDLGMODALFRAME, WEFDRAGOBJECT, WEFRIGHT, WEFCONTROLPARENT, WFMAXBOX,
WFMINBOX and WFSYSMENU. These are retained-WND selectors, not native private
structure offsets. WEFDRAGOBJECT's old extended-style producer still requires
identity/state audit; defining the constant does not prove that path works.
Style selectors read the already retained style field. Private presence/frame
selectors still require their nonclient-state producer before runtime
selection; declaring them does not close that obligation. Full user.h is
excluded because its kernel/server closure is outside this admitted slice.
The extended-edge selectors are required by unchanged comboini.c creation:
read WEFCLIENTEDGE for original 3D selection, then clear WEFEDGEMASK. Their
declaration does not establish native packed-flag compatibility; that outgoing
resource mutation remains separately audited in the S3 source ledger.

OPENNT-HOST-100 retains original inc/user.h retail PtoH/PtoHq/HW/HWq
expressions under a host-client-layout opt-in. These extract HEAD.h, with
PtoH/HW preserving NULL behavior. The original control-call facade selects
them after HEAD/WND and removes its local HWq replacement. Guest address
translation and the separate DispatchClientMessage view are not consumers.

OPENNT-HOST-099 retains client/usercli.h lines 1941--2141 as an unchanged
combo/list declaration subset: selection/hilite constants, combo creation,
destruction/notification, list item/selection/caret/scroll/directory and
variable-height/multicolumn helpers. It is opt-in after CBOX/LBIV and public
types, including PDROPSTRUCT. These are declarations only; the admitted ten
original control units own their implementations. No implicit C declaration,
project replacement function or server runtime is introduced.

OPENNT-HOST-098 retains client/ntstubs.c GetControlColor unchanged, selected
as a separate control-color object beside the class-query and dialog-getset
subsets. Original ntsend.h owns its return/error shell; the explicit parent
service maps to the existing ADAPTER-WOW-068 scoped native call. Retail
HANDLE_FIXUP=0/DBG=0 uses the original no-repair macro behavior for native GDI
handles. No brush routing, validation or fallback policy is reimplemented.
Implicit-recipient GetControlBrush is not covered by this admission.

OPENNT-HOST-092 wheel-state extension retains clglobal.c's original
gcWheelDelta definition and explanatory comment, consumed by editml.c and
lb1.c. The existing clglobal object supplies exactly one process-wide owner;
the adapter declares it extern rather than adding TLS or a per-control copy.
Original focus resets and scroll accumulation bodies remain unchanged.

OPENNT-HOST-074 declaration extension retains original inc/user.h ICLS_EDIT
(1) and ICLS_COMBOLISTBOX (12) for comboini.c's two internal creation calls.
These index the existing process-owned standard-class snapshot; no private
modern USER layout or new class-name table is imported.

OPENNT-HOST-097 retains client/drawtext.c TabTextOut and rtl/drawtext.c
IsSysFontAndDefaultMode as unchanged function subsets for original ListBox
tabbed item drawing. The complete outgoing closure is GDI primitives, existing
GdiGetCharDimensions, the captured system-font metrics, selected-font identity
and mapping-mode queries. No DrawText engine or USER server is imported.
Reference hashes and adapter leaves are recorded in the S3 source ledger.
Two separate objects use the existing control-drawing compile environment;
selection does not establish group runtime acceptance.

OPENNT-HOST-096 admits the owner-approved original client ComboBox/ListBox
collaboration bodies: combo.c, comboini.c, combodir.c, lb1.c, lboxctl1.c,
lboxctl2.c, lboxctl3.c, lboxmult.c, lboxrare.c and lboxvar.c. Their exact
source identities, caller relationships and remaining boundary disposition
are recorded in the S3 control ledger. Whole selected files retain Microsoft
notices and original algorithms; only line-ending representation may differ.
No directory wildcard, USER server or guest source is admitted. These files
are staged source inputs, not yet selected objects or runtime providers.
Required creation/state/callback/finalization bindings must be composed before
switching the class group. Presence alone is not a claim of finite link closure
or acceptance. This supersedes the candidate-only file list, not its open
provider work or the requirement to preserve original internal collaboration.

OPENNT-HOST-095 retains the complete original usercli.h CBOX declaration and
its adjacent ComboBox constants/macros (85 lines), unchanged, behind the
recovered-control layout selector. The existing LBIV and COMBOWND declarations
remain their sole owners. The declaration-only forward PCBOX is suppressed
when the full original typedef was selected. No native PCBOX is read or cast.
The canonical extent binding compiles this layout and checks the x86 CBOX
size and WND-extra pointer offsets. Reference usercli.h SHA256 is
11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA.
Original fields/constants are unchanged; no allocation, dispatch or lifecycle
capability follows from this declaration import alone. Grouped verification
remains pending.

OPENNT-HOST-094 retains client.c WowGetDefWindowProcBits unchanged (complete
51-line function). Its two shared-table inputs now use the existing original
server.c tables/initializer through the bounded WOW facade. A separate /Gz
object selects the function without changing the other client.c subsets.
The native registration boundary keeps null/size validation and USER16's
zeroed-buffer precondition; the autonomous duplicate arrays and merge loops
are removed from wow_private_user_compat.c. Original guest and callback ABI
remain unchanged. The retained boundary fixture is required in grouped S3
verification; source selection is not a runtime pass.

OPENNT-HOST-090 collaboration-group extension retains server.c's complete
gawListboxWndProc and gawComboWndProc arrays unchanged, including duplicate
entries and original order. Existing InitWindowMsgTable owns all four control
bitmaps (EDIT, LISTBOX, COMBOBOX and COMBOLISTBOX), matching original server.c
initialization. The facade grows only through FNID_LISTBOX and rolls back all
allocated slots on failure. No replacement dispatch algorithm is introduced.
Original private message values are supplied by the shared host ABI header,
not evaluated against modern SDK CB_MSGMAX. Controls remain unselected;
the supplemental membership assertions are staged and unrun with S3.

OPENNT-HOST-074 client-lock declaration extension: retain usercli.h's exact
CheckLock, Lock, Unlock and ThreadLock/ThreadUnlock family, including the
original dummy TL declaration, for the selected original control compile
environment (client/usercli.h lines 330--350). Original client Lock is assignment, not HM server reference
accounting. EDIT's INITCONTROLLOOKASIDE self-pointer is owned by the same
retained HWND binding as PED; finalization precedes that owner's release.
No new lock/reference manager or parent relationship lifetime is implied.

OPENNT-HOST-090 extension for T447 S3: retain server.c's complete
gawEditWndProc array and usercli.h's FWINDOWMSG predicate unchanged, using
the same pinned source identities as the existing tables and declarations.
Original FNID_START and private WM_FINALDESTROY/WM_SYSTIMER values bind the
finite EDIT consumer. The existing once-owner initializes and rolls back the
EDIT bitmap with both default-window tables; only the EDIT awmControl slot
is selected. Other control message dispatch is not admitted by this array.
No replacement message switch or altered original EDIT body is introduced.

T447 S3 registers OPENNT-HOST-093: declaration-only BUTN/BUTNWND from
inc/user.h and COMBOWND, STAT/STATWND, MDI/MDIWND from client/usercli.h.
Pinned file SHA256 identities are respectively
ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1 and
11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA.
These unchanged declarations supply the canonical extra-byte extents used
by client/clinit.c RW_RegisterControls (SHA256
83C7E304B38501C9AFD1251C2CF671F1BC0E799560AF10B225CEDEE891E3E8A2).
The finite ADAPTER-WOW-063 query retains those eight registration expressions
and kernel/class.c's sizeof(WND) addition. It does not register native classes,
copy modern cbWndExtra, import control implementations or recreate SERVERINFO.
Unknown/server FNIDs are rejected with unchanged output; they are not assigned
a guessed zero extent. Production private-precomp consumption and grouped
acceptance remain pending.

T447 S3 registers OPENNT-HOST-092: retain the complete constant
gabThunkMessage definition from pinned windows/core/ntuser/client/clglobal.c,
file SHA256 D8FE5A80D5848F2951843D7BA94752CB3223945A48630350835422389CD1DA6F.
Original EditWndProcWorker indexes it only below WM_USER when A/W differs.
The original boolean entries, order and FE_IME/MEMPHIS_MENUS conditionals are
unchanged; unrelated USER global state and the server precompiled environment
are excluded. The build binds Windows scalar types and verifies the original
WM_USER-sized extent. globals.h retains the original extern declaration.
This data-only object supplies classification to the original editor; native
CallWindowProc still owns conversion through ADAPTER-WOW-060. No replacement
message switch, new dispatcher, guest change or full USER import is admitted.
Build/runtime verification remains grouped with the incomplete S3 closure.

T447 S3 registers OPENNT-HOST-091: retain complete GetWindowBorders from
pinned windows/core/ntuser/rtl/winmgr.c without changing its style arithmetic.
Source file SHA256 is
D7E87476CC3E703C94B88D53B9A0761642B284A6776C0152738DAF4C74290D2B.
Its only server-state input, gclBorder, comes from SPI_GETBORDER: original
kernel/rare.c returns exactly that field. Original kernel/inctlpan.c assigns
it from NONCLIENTMETRICS.iBorderWidth. Do not infer it from frame-pixel widths
or apply a guessed constant. A finite native input capture supplies the value;
the complete hit-test/relationship group must still be connected before use.

T447 S3 registers OPENNT-HOST-090: retain kernel/server.c's complete
gawDefWindowMsgs, gawDefWindowSpecMsgs and InitWindowMsgTable unchanged,
from pinned 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c (file SHA256
3C6131EFB37173F5A280BD77EC1D887529AFB35EEF03B6F0248A3C75E7887047).
Retain WNDMSG from inc/user.h and FDEFWINDOWMSG from client/usercli.h;
nine private message constants come unchanged from the existing original
winuserp.h. The host owns only once-initialization, allocation and rollback.
SharedAlloc binds to a raising process-heap allocation: original initialization
assumes allocation succeeds, so allocation failure unwinds before publication.
No copied modern shared heap, replacement message classifier or server is
introduced. Initialization publishes both tables together; original message
IDs and bounds remain authoritative. Full default-worker selection is pending.
The same finite host publication now supplies the original SHELLHOOK message
identity through RegisterWindowMessageW, without clearing or assigning the
native desktop's taskman window. This is the original USER atom namespace,
not a hard-coded registered-message number or new message-routing policy.

T447 S3 registers OPENNT-HOST-089: retain the complete 663-line
DefWindowProcWorker body from pinned windows/core/ntuser/client/clmsg.c,
revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Only the source-selection
guard, private precomp and symbol-binding include surround the unchanged function.
Source file SHA256 is
2F87967A05E730E2C95B8CE0B92443D323F76BAEC97446D3C4F8C9DC163710FD. Original
message classification, parent forwarding, compatibility colors, stored-name
A/W conversion, sizing and conditional IME branches stay together. No server
dispatcher or native USER layout is imported. The existing native-call owner
must supply server-bound operations; retained state must supply names and
relationships. Name reads require the short reader guard outside this body,
never a lock spanning callbacks. This source is staged, not selected until
the complete S3 dependency group is linked and verified.

T447 S3 registers OPENNT-HOST-088: retain unchanged SetWindowState and
ClearWindowState from pinned client/client.c lines2826--2840 (SHA256
787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472).
A separate WOW_ORIGINAL_CONTROL_STATE object preserves their TestWF guards.
The two original prototypes are retained from client/usercli.h1465--1466
(SHA256 11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA).
Its compile seam maps the two symbolic SFI operations to the already audited
native leaves; no legacy syscall ordinal is sent to modern USER. The existing
retained-window owner refreshes the native result. No public SetWindowLong
replacement or new style-notification policy is introduced. Complete S3
selection and grouped acceptance remain pending.

T447 S3 registers OPENNT-HOST-086: retain complete client/ntstubs.c
_SetWindowLong and kernel/getset.c xxxSetWindowLong, unchanged bodies, under
the original pinned OpenNT paths. SHA256 sources are respectively
7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01 and
F0D22E4285E8B33B873854913A11587C67D78E50E1F5DB28EC89EF8B6FF06D3D.
The existing GetClassWOWWords selection is unchanged when the new dialog
get/set selector is absent. Preserve client CPD normalization/ANSI flags and
listbox style protection, then server result/user/private-offset/extra-byte
policy; do not replace those branches with a local switch. Exact original
CALLPROCDATA/CPD tag and LBIV/LBWND/SCROLLPOS declarations accompany the
finite slice, not native-layout overlays or a listbox implementation.
The usercli.h source hash is
11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA;
user.h retains its previously pinned identity. Full USER server and unrelated
stub functions are excluded. Native CPD validation, private-precomp and
shared-state caller bindings remain required before production selection.

Production composition now selects the unchanged kernel xxxSetWindowLong
body with its retained-local ownership and existing canonical extent inputs.
The client _SetWindowLong selection remains pending incoming CPD normalization;
do not infer that the whole get/set contract is linked from the kernel selection.
This selection change adds no original-body difference. Group verification is
pending after the remaining client/RTL boundaries are connected.

T447 S3 registers OPENNT-HOST-085: retain complete _GetWindowLong from
windows/core/ntuser/rtl/getset.c at pinned OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. The original function body and
dialog CPD/result/user/private-offset branches are unchanged. Its finite
outgoing boundary is original-layout WND/DIALOG, process validation, existing
native CPD ownership, class private-size metadata and negative-index access.
The rest of getset.c is not imported. Selected use must retain the existing
window borrow; native CPD structures are never cast to NT4 CALLPROCDATA.
Native conversion/validation, private-precomp and grouped acceptance remain
pending, so this is staged source, not an active replacement. Exact source
hash and native ABI observations are recorded in the S3 source ledger.

T447 S3 registers OPENNT-HOST-084: retain the original WFDIALOGWINDOW value
(0201) beside the existing WND state-byte macros in inc/user.h. Same pinned
source/hash as080. It is an original-layout host state selector, not a native
Windows window style or a win32u operation number. The dialog-state binding
uses the unchanged OPENNT-HOST-083 SetDialogPointer on the common retained
WND/class-extra allocation; no guest data or modern USER private storage is
modified. Group build/runtime verification remains pending.

T447 S3 registers OPENNT-HOST-087: original client/res.rc EDIT context-menu
resource only, from pinned OpenNT revision5c5b979ec08c17d3ca2eb70e8aad62d26515d01c,
source SHA256 C694D74B4EE57BE6F2979594E9BB98B59A31B1F48D13F1A1E0477F0DF54628A0.
Original ECMenu directly loads ID_EC_PROPERTY_MENU. Retain the menu body and
command IDs unchanged; exclude USER version, strings, bitmap and other resources.
The build adds a separate .res to WOW32, whose original resource file contains
no menu. A finite RC environment supplies Windows constants and the original
usercli.h ID1. The original menu logic remains in editec.c. Its eventual private
editor environment must bind hmodUser to existing hmodWOW32 for this lookup,
not change the class-registration adapter's native user32 module identity.
This resource/build selection is unverified until complete group acceptance.

T447 S3 registers OPENNT-HOST-083: retain only SetDialogPointer from original
windows/core/ntuser/kernel/miscutil.c, pinned OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, SHA256
3EE6D2EC087B1C6E32DE266861FF2111514DA909B40EC4914105B0F4480E8AE1.
The complete function body is unchanged. This finite state transition is
required by original ValidateDialogPwnd and DefDlgProcWorker cleanup: publish
PDLG, conditionally establish FNID_DIALOG, set WFDIALOGWINDOW, or mark cleaned
and clear that flag. It operates only on the retained original-layout window
owned by the existing binding, never installed native USER memory. The rest
of miscutil.c (queue, desktop, shell and server policy) is not imported. No
second dialog registry or rewritten lifecycle is admitted. Production caller
wiring uses the common dialog state. The production generator now selects
this subset with a source-local control-state/precomp.h environment from
wow32-dll, ahead of the unrelated MVDM precomp.h. The supplemental lifetime
fixture checks the transition but has not run; complete group linkage remains
pending. See the S3 source ledger.

T447 S3 registers OPENNT-HOST-082: expose original LARGE_STRING and
LARGE_ANSI_STRING beside the already retained LARGE_UNICODE_STRING in
inc/user.h (same pinned source/hash as080). A separate guarded declaration
selection lets the dialog native call carrier use the12-byte x86 descriptors
without importing an entire WND view. Bodies/layouts are unchanged; no string
conversion implementation is added. ADAPTER-WOW-062 binds only an already
resolved class atom and counted title to the verified17-slot native creator.
Class resolution, WOW words and callback/lifetime policy stay with existing
owners. The native leaf remains unselected pending full group verification.

T447 S3 registers OPENNT-HOST-081: recover the complete client/dlgbegin.c
creator/font/template unit, SHA256
C3DF77A2987E97DF8872C3DAB8F6E7C10A08850C5EBCB439C0EC1DC88B0CDC44,
and the complete default-dialog, focus, validation, rectangle and mapping
functions from client/dlgmgr.c (same pinned source as080). These are original
client algorithms, not a recursive USER-server import. Keep template/control
creation, WOWDlgInit, shared DS, initialization revalidation, resultWP and
canonical final cleanup together. The new manager group is selected only by
WOW_ORIGINAL_DIALOG_MANAGER; the modal-only fixture does not supply it.
No function body changes are introduced. Native window creation must marshal
the original counted Unicode strings and ANSI-creator flag independently;
blindly calling the existing ANSI-only WOW CreateWindow carrier is rejected.
Original navigation/default-button dependencies, common retained WND/PDLG,
registered callbacks, native metrics and scoped USER/GDI leaves remain the
finite outgoing group. Build selection is pending these bindings; current
native dialog creation/default wrappers are removed only with that complete
replacement, without a parallel fallback. See the S3 source ledger.

OPENNT-HOST-119 additionally covers the default-dialog callers of the
already guarded focus/default-button helpers. RestoreDlgFocus retains the
saved target before callback, stops after root/target retirement, and never
clears a freed PDLG after SetFocus. WM_SETFOCUS does not navigate a retired
root after restoration fails. DM_SETDEFID skips its result write after root
retirement but retains its original unlock/TRUE return. WM_NEXTDLGCTL skips
default-button work after focus destroys the root or either retained child.
These source-proven continuation guards reuse existing terminal flags, add
no state owner, and do not change the normal focus/default-button algorithm.
Supplemental r30 passes the actual-body focus/default caller assertions.
This is not real HWND-reuse or guest runtime acceptance.

OPENNT-HOST-080 also registers modal-loop exact-state checks: after
WM_CANCELMODE, before idle/show or ordinary message processing after queue
retrieval, after ShowWindow before UpdateWindow, and at the two existing
revalidation exits. Use the retained WND terminal flags and the original
destroyed-dialog return0; WM_QUIT is still reposted before exit. Owner
re-enabling remains EndDialog's responsibility, not a new generic teardown
policy. The ordinary DestroyWindow misuse path did not restore it in the
original manager either. Supplemental r31 passes the original modal bodies
with explicit state/service doubles, including21 retirement inputs. Real
HWND/guest lifecycle remains unverified.

OPENNT-HOST-080 additionally initializes InternalDialogBox's fDisabled to
FALSE for the no-owner path. The original assigns it only inside the owner
branch, then passes it unconditionally to DialogBox2 (and reads it before
the owner check on creation failure). One initializer removes the undefined
input without changing any owned-window state or adding an owner policy.
No-owner success/failure assertions are staged with the lifecycle fixture.

OPENNT-HOST-081 registers the creator's shared EDIT-segment allocation
handoff: capture GetEditDS's result locally, check the retained dialog's
terminal flags, release the new segment if its destination was destroyed,
otherwise publish it into the original PDLG. The actual guest allocation
path can compact/discard and call a task's registered GlobalNotify procedure;
this is distinct from the unproved release-path reentry claim. Use the
existing CreateDialogReturn/unlock path, without a second resource registry.
This change is staged for the complete creator lifecycle tests.

The same OPENNT-HOST-081 creator group retains exact root/child identities
across font, child-creation, dialog-code, initialization, focus, foreground
and show callouts. Post-transfer root retirement exits through the original
unlock/return path, never through pre-transfer font/menu deletion. A retired
child uses the existing NoCreate/DFNOFAILCREATE policy. Post-init retirement
keeps the original pre4.0 return branch; final validation still honors
SCDLG_NOREVALIDATE. No new lifecycle state or guest behavior is introduced.
The actual creator-body fixture passes49 supplemental input combinations in
r34. These resource/message doubles do not prove real guest/handle reuse.

OPENNT-HOST-080 additionally registers the EndDialog reentrant-lifetime
guard group. After validation/owner-enable, reject an exact retained WND
already destroyed or cleaned before writing fEnd/result. After result
publication, focus/hide/activation callbacks may destroy the dialog; stop
further HWND work and retain the successful TRUE result. Never resolve a
replacement from the numeric HWND. Five two-line checks retain normal call
ordering and use existing terminal flags; no state table or deferred cleanup
is added. Borrow retention cannot keep PDLG alive after original finalization.
Neither native ThreadLock emulation nor moving fEnd before owner-enable
preserves this contract. The original-body fixture covers all five edges;
supplemental r28 passes25 cases (including15 retirement inputs). Real guest
and other dialog lifetime chains remain open.

T447 S3 registers OPENNT-HOST-080: retain complete original DialogBox2 and
InternalDialogBox in client/dlgmgr.c, EndDialog in client/dlgend.c, and the
original DLG/DIALOG declarations in inc/user.h. Pinned source SHA256 values
are respectively 8E2383F52E4E3F65F656BD492143D7A8ED5D0A2526AD8E6AF10D242465FF8D7A,
191C9CE7DCE323873F5EF40F74141950940F6723E14D48B5FA09CA5517DDE6CD and
ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1.
These are unselected source slices for the admitted dialog dependency group,
not a new native message loop or an executable USER server. Keep original
owner disabling/restoration, WM_QUIT repost, idle notification, result and
wake-before-destruction ordering. State must belong to the existing window
identity, shared with the original creator/default procedure; it must never
be installed in modern USER's private dialog pointer. Private validation,
borrow/lock, creation and dialog-navigation calls remain explicit group
dependencies. Native foreground-queue queries and ZapActiveAndFocus require
verified host bindings; thread-ID comparison and SetFocus(NULL) are not
equivalent replacements. No function-body edits or production selection are
introduced by this registration. The S3 ledger records the finite boundary
and removal of the superseded native-modal carrier at group selection.

T447 S3 registers OPENNT-HOST-079: complete original SkipIDorString,
GetSizeDialogTemplate and DialogBoxIndirectParamAorW bodies in client/clres.c,
from pinned OpenNT SHA256
C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5.
The isolated WOW_ORIGINAL_DIALOG_TEMPLATE selection retains standard/extended
template walking, module-version style validation, original alignment and
modal flag filtering. Its finite outbound boundary is original template ABI,
GETEXPWINVER, diagnostics and InternalDialogBox. The latter must share the
recovered InternalCreateDialog/DefDlgProc/EndDialog state; a native modal API
is not a substitute. Only the source-selection guard and private precomp
include are added; the three original function bodies are unchanged.
No production generator selects this branch yet. ADAPTER-WIN32-051's native
modal replacement is scheduled for removal in that same complete dialog
selection, not a competing permanent implementation. Current native modality
does not supply original W32InitDlg or shared EDIT DS. See the S3 source ledger
for the verified missing native registration route and whole-owner obligations.

T447 S3 registers OPENNT-HOST-078: complete unchanged _IsWindowVisible and
_GetClientRect from rtl/wow.c, _FChildVisible from rtl/winmgr.c, and its
FChildVisible validation wrapper from client/winmgrc.c, all at the pinned
OpenNT revision. Source SHA-256 respectively
71E5B523BD44077AB4E86FFC6093544922C815C759BB753C32B4155E3206691D,
D7E87476CC3E703C94B88D53B9A0761642B284A6776C0152738DAF4C74290D2B,
26F0174E6A0DB53D942506CBDC12EFBFEBC34A943D780170E7BF9D95775514A4.
Original EDIT directly consumes these client-view semantics. Keep parent
traversal, null behavior and pre-Win4 minimized client geometry; do not map
them blindly to public IsWindowVisible/GetClientRect. Outgoing dependencies
are scoped original WND/parent views, validation, original flags, metrics and
OffsetRect. Host compilation selects the native RECT branch, not the WOW
WORD-rectangle branch. No other RTL/manager functions or server state enter.
Private precomp replaces the original build environment. These slices remain
unselected pending relationship/metric bindings and grouped verification.

T447 S3 registers OPENNT-HOST-077: unchanged CopyInflateRect and DrawFrame
bodies from original windows/core/ntuser/rtl/rect.c and rtl/draw.c at the
pinned revision below. SHA-256 respectively
7EB718CE4671BA8F1F50F891F61D4BB1EF2A1EBEF5F55C09DD145310ED496354 and
7491E5B3331DE8CB7A2189B8EFBE91F56076F80A08338BF816D793E80A4FAF76.
Original editsl/editml directly require these helpers. True subsets retain
original rectangle arithmetic and the ordered four-rectangle PolyPatBlt;
unrelated rectangle/drawing functions and resource globals are excluded.
The target-private precomp supplies declarations, border metrics, brushes and
the already admitted PolyPatBlt leaf. No replacement FrameRect algorithm,
kernel drawing server or new guest resource is admitted. These two units
remain excluded from the build until the full EDIT dependency group is wired;
native boundary and original geometry assertions remain unexecuted.

T447 S3 registers OPENNT-HOST-076: the complete original NtUserSetWindowFNID
body from windows/core/ntuser/kernel/ntstubs.c, pinned revision as below,
SHA-256 A5997517B8BBB0B9E69AFC973F8BF26D8859B6FB9F4C8C1513553C52E3EE2918.
Original EDIT validation/allocation/teardown directly requires this finite
state transition. Preserve its unset-FNID/cleanup-only rule and OR update;
do not reimplement that policy in the adapter. The receive macro is bound to
the already scoped original EDITWND host carrier, not the native USER object
or guest projection. The source symbol is private to the editor. No kernel
receive, handle table, scheduler or general USER server is imported. Original
FNID status constants and GETFNID are retained in inc/user.h. The pending
state facade supplies only carrier lookup and its PED extra-word access,
reusing existing window borrows. The production generator now selects this
body as control-fnid.obj, distinct from client/ntstubs.c. It remains unverified
until the complete S3 group; this is not general private native window access.
ADAPTER-WOW-059's creation-state projection uses original createw.c's ANSI
and compatibility branches, SHA-256
DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7,
at the same pinned revision. The full creator reaches excluded USER server
state; its finite field initialization is source-derived in the named adapter,
not an invented function in the mirror. The caller must supply actual A/W
provenance, module version, compatibility flags and logical class extra size.
Original flag encodings/access macros are retained in inc/user.h. No defaults
substitute for missing creation input, frame geometry policy or relationships.

T447 S3 registers OPENNT-HOST-075: original globals.h LOCALALLOC,
LOCALREALLOC, LOCALLOCK, LOCALUNLOCK, LOCALSIZE and LOCALFREE macros,
including their instance/DS contract comments, from the pinned revision.
Source SHA-256 FD78200232574074F7EC68D2F5B851C27EE9D5024427310D18EBBCB048336E97.
Only this true subset is imported; callback variable names bind to the existing
UserRegisterWowHandlers table in wow_edit_memory_bindings.h. No second table,
allocation implementation or USER global initialization is selected. The
usercli.h subset also retains the original BLOCK/STRIPINFO declarations,
EC/ML/SL cross-unit prototypes, native UserLocal* heap macros and retail
UserGlobal* macros from the OPENNT-HOST-072 source identity. The binding
supplies pUserHeap only, replacing the initially staged authored heap macros.
Debug global-memory instrumentation is outside the selected release profile.
The original GETEXPWINVER macro also uses the existing module-specific
callback, preserving its null-module substitution and complete return value.
The original GetAppVer macro is retained separately: it reads the current
task's existing client-info version, initialized by original xxxInitTask,
not the window's module version or the host executable's PE version.
ADAPTER-WOW-061 supplies this scoped GetClientInfo boundary without a second
client-info record or a new guest callback.
The unchanged seven-unit editor is the sole intended consumer. Allocation
arguments, instance identity and realloc's updated-pointer parameter remain
original. Build/runtime acceptance is pending the complete S3 group.

Declaration completion after original-bodies-r1 also retains unchanged
OPENNT-HOST-086 list-release flags LBUP_* from usercli.h:988-992.
OPENNT-HOST-074 includes original inc/user.h timer IDs (142,144,147) and
SYS_ALTERNATE (1893), SHA-256
ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1.
The shared precomp selects existing OPENNT-HOST-093 extent declarations for
COMBOWND/STATWND rather than duplicating those layouts.

OPENNT-HOST-105 additionally retains winuserp.h DS_VALID40/DS_COMMONDIALOG
(912/914) and SCDLG_* (902-905); inc/user.h class/resource/style operands
(128,158,179,1853), CHARSTOBYTES, MultDiv, MaskWF and dialog-unit conversions
(118-121,467,1924,2324-2325). Existing usercli subset retains original
visibility/handle macros (141,514,831), EDIT/Combo worker signatures
(1482-1489), and shared geometry/text signatures from inc/user.h:2568-2607.
These are declarations of original behavior, not autonomous replacements;
their full linked implementations and lifetime inputs still require closure.

The same bounded declaration group includes WM_LBTRACKPOINT, WM_BEGINDRAG
and SW_SCROLLWINDOW from winuserp.h:129,150,646; usercli.h's CCHFILEMAX and
MSGFLAG_SPECIAL_THUNK (1120,117); inc/user.h DF3DLOOK/DFCONTROL (1852,1856).
Shared precomp uses original SYSHBR's expression (inc/user.h:1089) against
the already initialized brush owner, plus MSVC SEH keyword spellings.
OPENNT-HOST-092 extends clglobal.c by the original four directory operands
awchSlashStar, achSlashStar, szOneChar and szSLASHSTARDOTSTAR (36-37,52-53),
source SHA-256 D8FE5A80D5848F2951843D7BA94752CB3223945A48630350835422389CD1DA6F.
Its producer now uses the existing Unicode control flags, matching original
WCHAR TEXT literals; no second directory-string owner is created.

T447 S3 registers OPENNT-HOST-105: the control resource ABI facade retains the extended
dialog template records and required EDIT/directory constants from original
public/internal/windows/inc/winuserp.h (lines364-426,879-900). Source is the
already admitted read-only ABI mirror, SHA-256
B2838AE608353A14659B85C2BE290034A6BB02DEE9AC078E82C298F2F9B9B513.
Unconsumed pointer aliases are omitted; original two-byte packing and fields
are unchanged. Existing usercli subset additionally retains original
NextWordBoundary/NextDWordBoundary (1066-1067) and RepositionRect (2167),
source SHA-256 11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA.
The inc/user.h subset retains UTCHAR's original Unicode selection (43-47),
source SHA-256 ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1.
This avoids importing the complete private SDK or writing a new parser;
there is no algorithm replacement, guest mutation or native object cast.
The shared body precomp asserts template sizes26/24. Compile/runtime evidence
remains pending the complete closure; successful declarations alone cannot
prove template bounds, callback identity or dialog creation correctness.

OPENNT-HOST-105 additionally restores the complete RtlInitLargeUnicodeString
body in rtl/chartran.c and the complete FindCharPosition, TextCopy and
wcsncpycch bodies in rtl/random.c. Pinned OpenNT revision is
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c; source SHA-256 identities are
0F9F0B7FDE3AAA475492A50F333745508698228C38FDBF9A21CBD4BFBEAD383B
(chartran.c, function at587) and
5147FCE6825D1F7E8D69298E4DA617F0FE414150963CFF391EE53C856A0F5B2A
(random.c, functions at80,108,150). Original Microsoft notices are retained.
Direct consumers are original dlgbegin/lboxctl3. The required slice has no
window, server, scheduler or private-host-state dependency: only counted
string declarations, CRT length/copy and caller-owned buffers. Other random.c
functions remain excluded. Explicit standalone includes and subset selection
are the only adaptations; all four function bodies match original after
whitespace normalization. The existing provider selects the two RTL owners;
there is no second autonomous implementation to retain or remove.
The control ABI also retains original winuserp.h WCSToMB/MBToWCS ACP macros
(1316-1322), inc/user.h IsEmptyString/StringLength inline leaves (290,298),
and usercli.h dialog collaboration signatures (2149-2168). Declaration-only
additions do not prove their complete implementation closure. The supplemental
original-string-boundaries target links actual RTL bodies and existing NLS/
allocation services; its fixture covers zero capacity, truncation, termination,
byte limits and return counts. Whole-group original-bodies-r5 passes this
actual-body fixture; full control/guest execution remains unproved. The r4
duplicate body/prototype/include insertion was removed without changing the
remaining original implementation; see the S3 source ledger.

T447 S3 registers OPENNT-HOST-106: the direct dlgbegin/dlgmgr
_GetNextDlgTabItem dependency retains the five-function Tab traversal closure
from original client/dlgmgrc.c: _PrevControl, _GetChildControl,
_NextSibblingOrAncestor, _NextControl and _GetNextDlgTabItem. Source revision
is the pinned OpenNT revision above; SHA-256
678D2CD4B79A15A79E6C0257B4BD93AD69FA7BE95A513B627565490256CB1AFC.
Its only additional behavior dependency is original rtl/winmgr.c
_IsDescendant (432), retained in that existing subset; source SHA-256
D7E87476CC3E703C94B88D53B9A0761642B284A6776C0152738DAF4C74290D2B.
All six complete bodies are unchanged. Public HWND wrappers and unrelated
group navigation are excluded, not replaced; required prototypes come from
usercli.h1714-1745 and inc/user.h2580. GetWindowCreator is the unchanged
inc/user.h1935 macro used by dlgend, not a second relationship resolver.
The algorithms consume the controlled WND graph through existing same-address
REBASE macros; no native USER pointers, geometry sampling or new traversal
algorithm is introduced. Graph construction/retention remains a delivery
prerequisite, so the added body is compile/fixture-selected but not production
selected. dialog-navigation-boundaries exercises the actual algorithm over a
synthetic consistent graph (empty, forward/reverse, nested, invisible,
disabled, wrap, foreign subtree, no tabstops); it is staged, unrun, and cannot
prove real HWND generation/lifecycle. No guest source or binary is changed.
The dialog client precomp also binds original NtUserCreateWindowEx to the
existing ADAPTER-WOW-063 enrolled creator, not the incompatible native17-slot
service. No original creation call or algorithm is edited.

OPENNT-HOST-106 also completes the already admitted relationship/geometry
slice with original rtl/wow.c _IsChild, _ClientToScreen and _ScreenToClient
(682,749,836; SHA-256
71E5B523BD44077AB4E86FFC6093544922C815C759BB753C32B4155E3206691D), and
rtl/winmgr.c IsVisible/GetTopLevelWindow (460,703; identity above). The direct
consumers are combo.c, dlgmgr.c, lboxctl1.c and the control coordinate calls.
These five bodies have no new outgoing service dependency and match the
original after whitespace normalization. Modern IsChild/IsWindowVisible or
coordinate calls are not substituted. The existing edit-view fixture now
covers strict child vs self, null, top-level, minimized ancestor vs self,
negative screen origin and round-trip coordinates; the grouped graph includes
its actual three original owners as original-window-view-boundaries.
This test is staged/unrun; it cannot establish live relationship publication.

Declaration completion after original-bodies-r1 also retains unchanged
usercli.h EDIT allocation/limit/modifier/undo constants (source lines577-619).
OPENNT-HOST-086 additionally retains its original list allocation/search/
selection constants (lines837-855) and LBItem/LBODItem records (1021-1029).
Source SHA-256: 11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA.
These extend existing declaration subsets only: no new algorithm, storage
owner, guest change or native private-layout interpretation. Grouped body
compilation remains incomplete; these additions have not been rebuilt.

T447 S3 registers OPENNT-HOST-074: original control validation/initialization
macros and LOOKASIDE declarations from usercli.h (the OPENNT-HOST-072 source
identity), plus AllocateFromZone/FreeToZone and the complete InitLookaside,
AllocLookasideEntry and FreeLookasideEntry bodies from client/rtlinit.c.
This declaration selection also retains usercli.h's exact CBEDITEXTRA=6:
clinit.c registers six private bytes, not sizeof(PED). EDITWND contains only
the first four-byte pointer; its size is not the class-registration extent.
The pinned rtlinit.c SHA-256 is
D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C.
The existing capture branch remains selected unchanged; a separate
WOW_ORIGINAL_EDIT_LOOKASIDE branch stages the recovered allocator. The
private wow_edit_lookaside_bindings boundary supplies a process-lifetime
initialized critical section and native process-heap operations only.
Its two original zone declarations derive from public/halkit/inc/nthal.h,
SHA-256 160A574E5C4B0CCF1494735408923A32A1C76CA4E88E5DB27F1562B96DE003E9;
no kernel zone service is imported. Original free-list, zeroing, fallback,
cleanup guards and failure ordering are retained rather than replaced by
an adapter allocator. Guest text remains owned by original WOW callbacks.
This source is unselected pending the complete EDIT dependency group; grouped
acceptance must cover pool reuse/exhaustion, allocation failure and terminal
control cleanup. No guest source or executable modification is admitted.

T447 S3 registers OPENNT-HOST-073: the seven complete original EDIT C units
under windows/core/ntuser/client, pinned OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Their function bodies, includes,
conditionals and Microsoft notices are retained. They are staged source,
not yet selected build inputs or proved runtime providers. The S3 control
source audit records the finite outgoing groups: original WOW memory and
word-break callbacks, window-owned state/lifetime, native USER call scopes,
clipboard notification, drawing/font leaves, system timers, A/W conversion
and combo-parent retirement. No full USER/GDI server or guest source enters
the host build. A target-private build shim, not a new mirror precomp.h or
rewritten editor, must satisfy those dependencies before selection.

| Original file | SHA-256 |
| --- | --- |
| editec.c | 13E3AA26649C7B21A49A3F3114A73BC06FDFC6472B782663BB10879CABC0DDB6 |
| edecrare.c | B50E0F8F305F6456B3A952DDD63C815357136F222328C6D83DD0E76E697B5634 |
| editsl.c | 5D730399CE35740C88E7D5F98F1A4C4882802A6E66A830D11B3D8E4BFAD9EB1C |
| edslrare.c | B33DEA4AA66B7713B651B259A37D7F993802F40C042514122B73580BAEC4F0DF |
| editml.c | 5F65E9AFF922918240243C3D91AEEC4C0317010C80A01AF43626F828B0F95DEC |
| edmlrare.c | FB9DD31919B7B90DB2F713DAA83684F1231558E7653AE72CD72458F1212000F1 |
| edmlonce.c | AC63E8B953B97C91FE4F0C7813919CF1FA482EA253E2EDFD3EF7CF606E5940A2 |

The pending recovered consumer replaces native EDIT as the implementation
of admitted WOW EDIT controls, while native USER continues to own HWNDs and
other controls. This is not authority to replace unrelated native process
controls or reduce the original editor to a text-buffer synchronization shim.
Verification remains the full S3 dependency/lifecycle/guest group, not source
presence or a leaf fixture. No redistribution-rights change is claimed.

T447 S3 registers OPENNT-HOST-072: declaration-only original EDIT state from
windows/core/ntuser/client/usercli.h, revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, SHA-256
11EDC7C5B3F32CE40114BD0C4026E43381F2B131F036506376E23C8EB1DB5BBA.
Retain EDITCHARSETPROC and the complete UNDO/ED/EDITWND declaration range,
including all fields, bitfields, comments and the intervening macros. The
existing inc/user.h subset adds its original ICH/LPICH declarations (same
source identity as its client-view row below). The consumer is the pending
seven-unit original EDIT package and its window-owned native state; these
types must not be overlaid on modern USER extra bytes or guest WND backing.
All unrelated controls, USER client globals and private precompiled imports
are excluded, not replaced with new editor state. Function bodies imported by
this row: zero. Selected/build/runtime acceptance: pending the full S3 group.

T447 S3 registers OPENNT-HOST-071: original GetEditDS/ReleaseEditDS from
windows/core/ntuser/client/client.c, pinned OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, SHA-256
787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472.
The existing client.c mirror keeps both bodies unchanged in a separately
selected WOW_ORIGINAL_DIALOG_EDIT branch. The private precompiled environment
is replaced by wow_edit_memory_bindings.h; the existing registration owns the
callback table. No new allocator, segment registry or copied callback table
is introduced. The original GHND/GMEM_SHARE/256 allocation and LOWORD free
remain intact. Full client.c cannot compose because it also contains excluded
USER server/CSR dependencies; these two functions have only the registered
WOW callback dependency. No existing implementation is displaced because the
functions were absent. The retained upstream comment mentions DS_LOCALEDIT;
actual dialog call-site polarity is documented in the S3 ledger and must not
be inferred from that comment. This subset is staged, not selected, built or
accepted yet. Dialog creation/failure/shared-child/retirement acceptance is
required with the complete EDIT group. No guest modification is involved.

T447 S2 P2 registers OPENNT-HOST-070 against the same
handles.c source/revision/hash as OPENNT-HOST-069 below. One executable line
in GetFullUserHandle replaces `&gSharedInfo.aheList[dwHandle]` with
`WowSharedHandleEntry(dwHandle)`. The bounded adapter copies only the selected
shared HANDLEENTRY's type/flags/uniqueness DWORD through the existing CCPU
page-domain owner; the accessor exposes that call-local row. Index bounds,
TYPE_WINDOW selection, MAKELONG reconstruction and dead/non-window fallback
remain original. The prior cleanup-table view is removed from this query;
strict ownership lookup and cleanup still use it. Direct guest pointer access
is prohibited; leasing/copying the complete 1 MiB table per query is rejected.
No parallel table, native object registration, guest change or DDEML import
is added. shared-view-batch-r1 verifies observed and desktop identities with
an empty cleanup table, retired/free/non-window fallback and transport failure
separately from an absent handle. delivery-r2 verifies and publishes the paired
worker/provider; full relationship-graph activation remains open. The P1
evidence below describes the prior baseline, not the P2 storage accessor.

T447 S2 registers OPENNT-HOST-069: the unchanged GetFullUserHandle function
from windows/core/ntuser/client/handles.c, canonical OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, source SHA-256
29742B05EC55182515971CA67B54E3E0C4AD724FEECD482FD50EA2E398E9C2F2.
The subset retains its Microsoft notice. Direct consumers are original
WOW32 FULLHWND32 message/DDE conversions through pfnGetFullUserHandle.
The existing object table supplies type/uniqueness through a call-local
selection of thread-local view pointers; there is no new handle allocator.
The original absent/non-window short-handle fallback replaces the owned
registered callback's zero-on-missing behavior. Strict internal lookup is
retained for WOWEXEC registration and menu publication. The surrounding
DDEML handle manager, allocation and callback machinery are rejected as
unneeded outgoing closure. Function-body edits: zero; guest edits: zero.
Tests/evidence: T447 S2 process-retirement fixture and window-graph audit;
built and published in the bounded S2 P1 baseline, not complete WOW recovery.

T433 S5 registers OPENNT-HOST-065 in base/win32/client/vdm.c: byte distances
are computed before narrowing; STARTF_USEHOTKEY and UndoCreation retain their
original DWORD/ULONG numeric carriers through ULONG_PTR. The unused config
body in the selected translation unit receives the same byte-distance change.
Original classification/environment/control ordering and x86 semantics remain.

OPENNT-HOST-066 registers local CSR capture pointer-table width/alignment in
base/ntdll/csrutil.c and its ntcsrmsg.h declaration carrier. x86 ULONG_PTR and
alignment remain32/four bytes; x64 preserves complete process-local pointers.
Length/count and wire fields remain32. The standalone provider still ignores
capture rebasing metadata; no original CSR transport is restored. Tests and
exact mirror accounting belong to the S5 native launcher evidence.

OPENNT-HOST-067 corrects only the existing standalone zwapi.h declaration
carrier under base/ntos/rtl: modern local NT VM API lengths and region pointers
use SIZE_T/PSIZE_T, and ZeroBits uses ULONG_PTR. The original environ.c body
is unchanged. x86 declarations retain their original widths. Native mutable
environment/clone/error tests exercise the selected original implementations.

OPENNT-HOST-063 selects the unchanged gabObjectCreateFlags table from
`windows/core/ntuser/kernel/handtabl.c` (SHA-256
`F0B56314A93423599F06645D17104391F19A0CA07C3944853B065D8866493F7C`),
original TYPE declarations from `inc/user.h` and OCF declarations from the
existing `kernel/userk.h` subset. The cleanup owner now links this table instead
of accepting caller-defined object classification. Source verification is in
verify-wow-cleanup-bounds.ps1. The selected non-FE_IME profile has 16 types.
HMAllocObject/HMFreeObject are not selected: they generate their own handle
identity and own allocation, whereas this profile uses native USER objects.
This is not evidence that native object enrollment or every resource lifecycle
has been implemented; the original table defines ownership, not publication.

The same original `inc/user.h` subset now optionally exposes unchanged
LARGE_UNICODE_STRING, HEAD/SHROBJHEAD/THROBJHEAD, WND and DESKTOPINFO bodies
for the S42 guest client view (source SHA-256
`ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1`).
`wow_user_client_view_layout.h` supplies only the finite compile boundary and
x86 offset assertions against the pinned USER.EXE. Its hook count uses the
original Win4 WH_MIN=-1/WH_MAX=12, not the modern SDK extension. This restores
the data schema, not a private USER server or a claim of runtime publication.

OPENNT-HOST-062 recovers the complete WOWCleanup owner and its selector/window
helpers from `windows/core/ntuser/kernel/cleanup.c`, SHA-256
`DFB0FB8D81AFEB555B3631ADC5456F4CEB13142F2E954A6F2B22631C49142E5C`.
ClientDied and its kernel exception dependency are not selected. Explicit
context replaces ambient USER globals; the 13-entry procedure table is indexed
by element count instead of its original erroneous byte count. The WOW BOOL
ABI returns FALSE on allocation/probe/native class-release failure, preserving
the failed node instead of retrying forever, and TRUE on completed branches.
This adds failure visibility, not transactional rollback: prior cleanup actions
remain applied. Original module/task/thread selection and resource ownership
tests are retained. `verify-wow-cleanup-bounds.ps1` checks the complete body.
Formal compilation and the cleanup fixture do not establish production object
membership or 20-slot registration; those bindings remain required.

T417 S4 registers OPENNT-HOST-031: the existing standalone
BaseSrvDOSWorkerWaitPending query in `base/win32/server/srvvdm.c` is not
original OpenNT source. It is a minimal registered divergence in its existing
original BaseSrv owner: the body keeps the original DOS lock, record and event
as the sole state owners, and adds no task state machine or transport policy.
The former private fragment has been retired; strict mirror policy forbids
adding a new file for this small owner-local correction.

This is the original non-MVDM OpenNT host-service mirror. It contains only
complete source-audited packages used by a selected MVDM caller. It is not an
alternate MVDM provider, a compatibility layer, or a general collection of
host utilities.

## Package admission and current package

### S40 original read probe

OPENNT-HOST-061 imports unchanged ProbeForRead from base/ntos/ex/probe.c,
SHA-256 `0D1CAE414B609C624E6BDF04FA28EBD84FF38DBB9687A63D8BC9E840C2F2372B`.
It checks alignment, arithmetic overflow and user address range only; it does
not access pages. ADAPTER-WOW-046 supplies the current process address ceiling
and native RtlRaiseStatus, while retail assertion/page annotations are elided.
No kernel image or service dependency is introduced. Actual caller reads must
remain inside original SEH and use the existing guest-memory boundary. This
core is ready for WOWCleanup but that complete receiver is not installed yet.

### S40 original class registration wrapper

OPENNT-HOST-060 restores _UnregisterClass from the same pinned class.c and
routes existing unregister through it. Original private/public lookup, window
association check and menu pointer selection are preserved. Besides explicit
context, the native DestroyClass binding can fail and its failure is propagated
instead of reporting false success. ADAPTER-WOW-045 keeps outputs private until
success and retains native USER's final live-window/ownership check. Original
class-list selection is removed from the autonomous unregister implementation.

OPENNT-HOST-059 recovers _RegisterClassEx in class.c, separately compiled from
lookup. Original source SHA-256:
`82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A`.
Only an explicit boundary context parameter is added. Original CPD lookup,
registration failure, worker/menu/WOW-word publication and conditional TDB
hTaskWow ownership remain intact. ADAPTER-WOW-044 requires actual class
registration and CPD resolution operations and supplies current thread state;
neither callback has a default success implementation. The existing owned
class entry now stores the source fields, replacing duplicate adapter words
and menu pointer storage. The class-client publish path now uses this core,
verified through native class registration. Worker callback-table publication,
production CPD ownership and WOWCleanup remain incomplete.

### S40 original input membership owner

OPENNT-HOST-058 additionally recovers RecalcThreadAttachment from the same
pinned input.c. Only current-thread parameter plumbing and its Recalc2 call
change. Original queue reuse, allocation ordering, failure break and reference
increments remain unchanged. ADAPTER-WOW-043 requires queue allocation for its
only AllocQueue(NULL,NULL) call; native input publication remains separate.

OPENNT-HOST-057 admits AddAttachment and Recalc2 from original
`windows/core/ntuser/kernel/input.c`, SHA-256
`B7656BCA55E892A42BA825BE0324211450AD298C40717E1AE5A889D084467529`.
AddAttachment is unchanged; Recalc2 adds only an explicit current-thread
parameter. ADAPTER-WOW-043 binds implicit context, desktop membership and
required queue release. The original fixed-point traversal owns explicit
edge and same-process 16-bit grouping. Native queue assignment/publication,
message redistribution and focus/capture remain unbound; these two compiled
bodies are not a claim that ReattachThreads or USER registration is complete.
The original release call may reenter; the owner must retain and serialize
all views. No invented input.c content substitutes for the omitted owners.

### S40 original WOW priority ordering owner

OPENNT-HOST-047 admits only unchanged InsertTask from
`windows/core/ntuser/kernel/taskman.c`, SHA-256
`E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E`.
The entire 744-line source has been reviewed. This selected function contains
the actual stable-priority remove/reinsert algorithm needed by original
xxxInitTask, sleep, yield and send/reply scheduling. Its only outward calls
are the original retail-elided CheckCritIn/UserAssert checks; it allocates
nothing, waits on nothing and does not access kernel objects. ADAPTER-WOW-039
provides only a finite list/priority view, not NT4 private structure layouts.
This is a source-owner recovery, not admission of the USER server or a
replacement scheduler. The caller must own serialization and task lifetime.
OPENNT-HOST-048 additionally retains exact WakeWowTask and DirectedScheduleTask
bodies, and CurrentTaskLock with only an explicit original-process argument
instead of PpiCurrent. ADAPTER-WOW-039 supplies the same-domain event counter,
process/thread/message field views and native event signal operation (the
original call discards KeSetEvent's result). Full event/message/thread/process integration cannot be
substituted with no-ops.
Formal compilation and finite ordering tests do not establish runtime use.

OPENNT-HOST-049 additionally recovers unchanged DestroyTask and its exact
CLOSE_PSEUDO_EVENT/ExitWowCritSect support from original kernel/userk.h
(SHA-256 `9260E0BA3CC89974E10B3804ED66D18B9864F4AE7D1175CCE6DDE8EE79D66E8C`).
The original body retains event-count subtraction, waiter/task unlinking,
execution-owner release and successor/WOWEXEC signaling. Binding macros
select the explicit process's shared domain, process-heap records and an
independently retained native idle-event handle. Other kernel objects and
the USER server are not imported. Destruction does not clear the original
thread's ptdb after freeing it; full thread teardown must not reuse that
record or call this destructor twice. Runtime cleanup hookup is still open.

OPENNT-HOST-050 recovers xxxSleepTask and xxxUserYield plus the exact original
EnterWowCritSect helper. Only the current-thread lookup becomes an explicit
parameter, and UserYield passes its original thread to SleepTask. ADAPTER-039
binds the reached non-timeout alertable WaitAny to native multi-event wait,
retains NT4 queue-mask values and delegates message/death/idle/outer-lock
operations to a required host contract. These are not optional success stubs.
The complete production owner must supply them before exposing the callbacks.
Bounded real-event wait tests do not prove USER messages or WOW task execution.

OPENNT-HOST-051 restores xxxDirectedYield from the same pinned taskman.c.
Only PtiCurrent becomes an explicit current-thread argument, also passed to
SleepTask. Original caller/target event posting, transient priority boost,
missing-target return and OldYield policy remain unchanged. ADAPTER-WOW-039
requires lookup by existing host thread ID under the shared USER lock, with
borrowed thread lifetime retained through the call; it creates no ID registry.
Six host cases include two native threads executing the original wait/yield
handoff. Actual WOW task registration and USER output-table hookup remain open.

OPENNT-HOST-052 recovers xxxRegisterUserHungAppHandlers as a true subset of
original kernel/exitwin.c, SHA-256
`45B92F7236FBA49D58E2C3D912126067C3843EC560C28CDD8A20167C62AAEF65`.
Only current-process lookup is an explicit parameter. Original zero-init,
event-reference failure release, callback/client-event recording and process
list publication remain intact. ADAPTER-WOW-040 retains a same-access native
event handle after checking EVENT_ALL_ACCESS and event type; no kernel object
pointer escapes. Full USER shutdown code is not imported. The original caller
registers once per WOW process; repeat registration is not made idempotent.
Native failure/reference tests pass, but production registration and ordered
process teardown remain open. The output ABI still requires a bound wrapper.

OPENNT-HOST-053 restores complete xxxInitTask, SetAppCompatFlags and
_ShowStartGlass from original kernel/queue.c, SHA-256
`0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8`.
Current thread/process are explicit, InitTask passes its process to the
start-glass helper, and the native-thread image-name fallback reads the
captured process view instead of ETHREAD/PEB. Name capture, startup policy,
TDB insertion, compatibility parsing and publication order remain original.
ADAPTER-WOW-041 exposes the finite original fields and required profile,
input attachment/journal and cursor operations with no production defaults.
USER Unicode literals are retained even in the ANSI provider build; numeric
compatibility parsing uses native NTDLL RtlUnicodeStringToInteger. No full
USER queue/server is imported. Six initialization fixtures use real heap/TDB
and process registration, but controlled profile/input/cursor observers;
they do not prove those native services or a real Win16 task initialized.

OPENNT-HOST-054 corrects one original host defect in SetAppCompatFlags:
FastGetProfileStringW consumes a WCHAR count but the caller passed the byte
size of WCHAR szHex[80]. Pass sizeof(szHex)/sizeof(szHex[0]) instead. Original
profile.c multiplies this count by sizeof(WCHAR), then copies the resulting
string, so the previous 160-character promise exceeded the 80-WCHAR array.
A guarded red test reproduces the wrong requested count without writing out
of bounds; corrected capacity and long-value tests pass. No guest is changed,
and this does not establish a cause for any historical WRITE failure.

OPENNT-HOST-055 retains byte-identical FastGetProfileStringW from original
kernel/profile.c, SHA-256
`8ECBB58954ECA0FECE4AB180BB56A3953B8B4408400D3C7AA01D50B62CAAA531`.
Only the unavailable include/registry/pool boundary is bound by ADAPTER-WOW-042.
The selected PMAP_COMPAT machine key uses read-only native access; other
sections, impersonation and full profile mapping were not imported by S2. Default,
query-buffer allocation, truncation and returned-length policy stay original.
T447 S3 extends this same subset with byte-identical FastGetProfileIntW
(profile.c lines 548--582, same source hash) for EDIT's two overlap preferences.
ADAPTER-WOW-042 additionally opens only PMAP_DESKTOP=5 read-only through the
current process user's Control Panel/Desktop key. PMAP_COMPAT remains the
unchanged machine key, and task compatibility lookup still rejects other IDs.
ADAPTER-WOW-058 supplies scoped EDIT system inputs, not a SERVERINFO overlay:
SYSTEM_FONT metrics through the already admitted GdiGetCharDimensions,
SPI_GETWHEELSCROLLLINES, and original sysmet.c's GetDoubleClickTime()*4/5
scroll interval. init.c supplies overlap defaults3; original ntuserk.h names
are LeftOverlapChars/RightOverlapChars, not the C field names. No global
profile cache, impersonation, kernel initialization or new editor is imported.
The complete dependency-group build/runtime acceptance remains outstanding.
OPENNT-HOST-056 adds only original KEY_VALUE_PARTIAL_INFORMATION and the
complete KEY_VALUE_INFORMATION_CLASS enum from public/sdk/inc/ntregapi.h,
SHA-256 `0FB164317335D8B48C51122182922259C902C7F60AFB28111BAFA9FB75735A6A`.
The unrelated legacy nt.h macro incorrectly said Full=2; it is corrected to
Full=1 (Partial=2), and this reader uses the original enum. Missing-key paths
are native-tested without registry writes. Existing-value/truncation tests
use the actual reader with a test-only opening boundary for existing OS
metadata, not a changed product registry path. Production task hookup remains
unaccepted.

### S40 original ANSI class client composition

T447 S2 extends OPENNT-HOST-046's existing ntcftxt.h subset with the unchanged
GetClassInfo body from canonical OpenNT (SHA-256
`E7F3D42CE8939BBE0B14EA34BC693107334C19F8B2D7862EC15B75A501D11DAB`).
It reuses the selected capture/copy/cleanup macros and original client.c
compilation. The owned NtUserGetClassInfo boundary queries actual native
class fields, using retained GetClassPtr for enrolled identities, and returns
their logical procedure/client menu instead of the gateway/cleared menu.
No private USER cursor/CPD server or GetClassInfoEx API is imported. Native
unenrolled classes retain host behavior; original server version-policy and
standard-class metadata recovery are not claimed by this bounded connection.
The T447 S2 evidence records source-body equality and query/runtime limits.

OPENNT-HOST-046 compiles the retained ntcftxt.h ANSI class operations in a
second composition of original client.c, alongside its unchanged original
RegisterClassWOWA wrapper (client.c SHA-256
`787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472`).
ntsend.h now also carries original COPYLPSTRIDW and its three selected ANSI
aliases. The dispatch-only composition is unchanged; no new mirror path is
invented. Existing OPENNT-HOST-037 cleanup correction remains registered.
ADAPTER-WOW-038 provides explicit task/module-version inputs and scoped native
class publication; original capture, validation, menu ownership and old-version
return policy remain original. Formal output-table hookup and task/creation
lifecycle acceptance are still open, not satisfied by host registration tests.

### S40 original class-menu resource client

OPENNT-HOST-043 imports original CommonLoadMenu and LoadMenuA/W from
`windows/core/ntuser/client/clmenu.c` (SHA-256
`DEE4BDB0C79E58993B25AB65C37EB71CC4CFC9A551A9A0E07B5A2F67EB8461CD`).
OPENNT-HOST-044 restores WOWFindResourceExWCover in its existing clres.c owner
(original SHA-256 `C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5`).
OPENNT-HOST-045 also restores MenuLoadWinTemplates, MenuLoadChicagoTemplates
and CreateMenuFromResource from the same source. Only the unavailable
TIF_16BIT read becomes an explicit invocation parameter carried through
recursion, and two cast-postincrement expressions become standard pointer
advancement. CommonLoadMenu's private WOW-only entry supplies TRUE. The
Chicago parser body is unchanged. Private USER object structures remain
excluded; ADAPTER-WOW-037 binds creation/insertion/destruction/help IDs to
native menu objects. No new parser or fake task/TEB state is introduced.
Original
W32 resource callbacks own guest/native discrimination and materialization.
These clients are required by original createw.c/ntcb.h automatic class-menu
loading; registering a packed WOW module with native USER does not supply
that callback. Host lifecycle tests do not close that creation integration.

### S40 original class-word query client

OPENNT-HOST-041 retains the unchanged GetClassWOWWords body from original
`windows/core/ntuser/client/ntstubs.c`, SHA-256
`7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01`.
OPENNT-HOST-042 retains only its seven original call/error/capture/cleanup
macros from `windows/core/ntuser/inc/ntsend.h`, SHA-256
`B708F1FEE46E93735C4C931D556E16DAA9679E6352D72620904D18AD21396ABF`.
Unrelated stubs and private USER precompiled/thread-connect dependencies are
cropped. The direct caller is WOW32 walias.c::FindClass16 through the pending
USER output table. ADAPTER-WOW-035 binds its captured-name lookup to existing
scope-retained class words and original GetClassPtr policy. Full private
kernel CLS/desktop-heap mapping cannot compose; the local view has zero
server/client address delta and never enters guest state. This recovers the
original client algorithm, not a USER server or shared guest graph.
Formal x86 linking and actual capture/TLS/native class lifetime tests pass;
service registration and real FindClass16 guest execution remain unaccepted.

### S40 original USER ANSI capture dependency

OPENNT-HOST-040 retains the complete unchanged `RtlCaptureAnsiString` body
from `windows/core/ntuser/client/rtlinit.c`, original SHA-256
`D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C`.
Original class registration and class query require this capture contract.
The remaining USER initialization and capture routines are excluded because
they require the private USER initialization environment, not this bounded
string operation. The retained body and line endings match the original.
ADAPTER-WOW-034 supplies its original IN_STRING declaration and finite heap,
conversion and existing worker TLS bindings; no new TEB or conversion policy
is introduced. The formal x86 provider links the original body. A fixture
using that object and the real worker TLS object passes null/empty, forced
allocation, the original static-buffer threshold, oversized rejection and
two-thread isolation. This is dependency evidence, not class-service or
real Win16 acceptance; allocation/conversion failure injection remains open.

### S40 finite original class metadata lookup

OPENNT-HOST-039 retains only `_InnerGetClassPtr` and `GetClassPtr` from
original `windows/core/ntuser/kernel/class.c`, SHA-256
`82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A`.
Both function bodies, including comments and CRLF, are unchanged. Required
consumers are the pending WC bindings for WOW32 FindPWC/FindClass16 and
class registration/query/unregistration. Same-module public/private classes,
module-HIWORD matching, deferred-destroy filtering and system fallback must
retain this original owner rather than grow a replacement lookup algorithm.

The complete outgoing closure is list traversal and HIWORD comparisons.
`wow_class_lookup_bindings.h` (ADAPTER-WOW-028) supplies caller-owned native
metadata links and binds the original system/client-module roots to an
explicit context. This is not the kernel CLS/PROCESSINFO layout, a guest
projection or a runtime USER server. Full class.c cannot compose: its other
functions require desktop heaps, process/thread objects, kernel locks and
handle destruction. Those are excluded; native USER still owns registration
and windows. No kernel allocator, scheduler or USER service is imported.
This finite semantic slice follows the package-interface stopping boundary,
not directory-wide server recovery. No autonomous lookup implementation is
superseded because none has been admitted. The formal WOW32 graph selects
the original slice; metadata population, synchronization, WC lifetime and
real guest callers remain unconnected and unaccepted.

### S40 original USER window dispatch owner

OPENNT-HOST-038 retains client/client.c::DispatchClientMessage unchanged,
from original windows/core/ntuser/client/client.c SHA-256
`787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472`.
Other USER client entry points and the private precompiled environment are
cropped. `wow_window_dispatch_bindings.h` supplies a synchronous HWND/WW/
callback view in place of kernel PWND and global callback lookup. The original
retail assertion remains non-evaluating. The original tagged-procedure branch
and callback arguments/return remain byte-exact; no guest pointer decoding or
message-thunk algorithm is duplicated. The function enters the formal WOW32
x86 link and focused nested-call test. Real window lifetime, callback routing
from the native window gateway and Win16 acceptance remain incomplete.

### S40 USER class client recovery in progress

OPENNT-HOST-036 retains the required class-client subset of original
`windows/core/ntuser/inc/ntcftxt.h`: StringDuplicate, InitClsMenuName,
RegisterClassExWOW and UnregisterClass. Source SHA-256 is
`E7F3D42CE8939BBE0B14EA34BC693107334C19F8B2D7862EC15B75A501D11DAB`.
Direct MVDM consumers are wuclass.c's RegisterClassWOWA output slot and
direct UnregisterClass call. The other text API families and private USER
environment are cropped. The finite outbound contracts are native allocation,
original string conversion/capture, expected-version/client metadata, GDI
validation, and NtUserRegisterClassExWOW/NtUserUnregisterClass resource
bindings. Full USER kernel/desktop/CSR imports are not admitted.
This recovered source is currently selected by the x86 class lifecycle
fixture only. Production callback dispatch, class/clone metadata and shared
graph integration remain open; no partial output table is installed.

OPENNT-HOST-037 adds exactly two LocalFree calls at InitClsMenuName's
capture-error exit. Original capture may fail after duplicating both client
menu strings, before a class owns them. The unchanged source retains two
allocations in the negative control; the corrected mirror retains zero.
Success, resource-ID and failed-kernel-registration rules remain original.
The original unregister client is tested on failure followed by successful
cleanup. See the S40 USER evidence for source/test identities and limitations.

### S40 finite USER bitmap owner

OPENNT-HOST-035 retains original `windows/core/ntuser/client/cldib.c`
scaling code, replacing only its precompiled-header dependency with the
consuming bitmap binding. Source SHA-256:
`E16FB36805F44767B45B597510A6C08D1DBC47271783E5D1F8F09273C0F782FB`.
It supplies clres.c's SmartStretchDIBits/ScaleDIB closure using public GDI
and local allocation; it is not a USER server import. Original ChangeDibColors
is also retained in clres.c, with original usercli.h stretch/color macros
bound to public GDI/GetSysColor. The bitmap owners and bounded DC environment
now enter the WOW32 link. Generic stretch/color-flag acceptance is
not implied by the fixed-argument WOW bitmap regression.


OPENNT-HOST-033 selects original `windows/core/ntuser/client/clres.c`
bitmap conversion functions and constants/old-header structure as a subset.
OpenNT and OpenNT-4.5 copies have identical SHA-256
`C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5`.
Original WU32LoadBitmap reaches this owner through WOWLoadBitmapA.
The retained chain is CopyDibHdr/HowManyColors/TrulyMonochrome,
ConvertDIBBitmap/BitmapFromDIB/CreateScreenBitmap/Convert1BppToMonoBitmap.
Unrelated resource families and the USER precompiled shell are cropped;
native fixtures and WOW32 select the bounded DC/capability binding.
Resource-name loading and actual guest/output-table integration remain required.

The original WOWLoadBitmapA body is now retained unchanged as part of
OPENNT-HOST-033. Its MBToWCS macro reuses the already selected original
chartran.c MBToWCSEx(0, ...). Only its fixed zero-dimension/zero-flag LoadBmp
call maps to public LoadBitmapW; this does not supply a general replacement
for original LoadBmp. The finite binding uses LocalAlloc/LocalFree consistently
with the existing chartran allocator. Native data, named-resource and
missing-name/ID fixtures pass; real guest/output-slot acceptance remains open.

OPENNT-HOST-034 fixes the original old-icon/cursor header branch: capture
`upOldIcoCur->abBitmap` before redirecting `upbih` to local `Fake`, because
the macro aliases that variable. Previously returned bits pointed into the
callee's stack. Only the order of two assignments changes. The fixture
selects unchanged upstream as a failing negative control or this correction
to assert the input bits and converted header. This is a host correction,
not a guest change or evidence that current Win16 workloads use old formats.

### S40 finite USER DDE data owner

OPENNT-HOST-032 admits the original `windows/core/ntuser/client/hdata.c`
`FreeDDEData` body as a true subset. Direct callers are original
`mvdm/wow32/wumsg.c` through `pfnOut.pfnFreeDDEData`. Source SHA-256:
`3BEC21C4BB3CAEF0798208693A89012DF6005C6A330F0F670F974A1CE821FCA2`.
The full translation unit requires excluded USER DDEML instance, transaction
and handle machinery; those APIs are cropped with registered markers.
The retained function owns original release-flag and format-dependent nested
object cleanup, unchanged. Its outgoing closure is GlobalLock/Unlock/Size/Free,
DeleteObject, DeleteMetaFile and DeleteEnhMetaFile, all public native APIs.
`wow32-dll/include/wow_dde_data_bindings.h` retains the original DDE_DATA
layout and retail USER memory macros; it owns no replacement DDE algorithm.
This source is selected in the WOW32 provider build and native fixtures,
but not registered as a working WOW output slot. Evidence and remaining
real-Win16 acceptance are in the indexed
S40 USER profile audit. No full USER package or new server is imported.

This root is not limited to BaseSrv/BaseClient. Every separately admitted
non-MVDM OpenNT host package belongs below this root with its original
relative path preserved. A package is admitted only after the shared tracker
records its direct `mvdm-host` caller, complete outgoing boundary, final file
disposition and any required package-private adapter binding. Discovering a
same-named function body is not package admission.

`base/win32` is the first selected package slice. It contains the original
BaseSrv/client VDM command implementation and its direct original declaration
carriers:

- `base/win32/server/srvvdm.c`, `srvvdm.h`, `basesrv.h`, and `srvinit.c`;
- `base/win32/client/vdm.c` and `basedll.h`; and
- `base/win32/inc/base.h`, `basevdm.h`, and `basemsg.h`.

Every file above is byte-identical to its canonical selected OpenNT source
except registered true subsets and minimal same-shaped adaptations required
by a selected formal runtime.
The source selection, rejected OpenNT-4.5 `vdm.c` candidate and initial
interface dispositions are recorded in
[`m0-t291-s5-p2-basesrv-source-package-audit-001.md`](../../docs/etc/evidence/m0-t291-s5-p2-basesrv-source-package-audit-001.md)
and the shared MVDM/OpenNT host trackers.

`base/ntos/rtl/environ.c` is the selected D11 RTL slice. It is directly
reached by original `base/win32/client/vdm.c` and
`mvdm-host/dos/command/cmdenv.c`. Its source body was imported from
`O:\repos.external\OpenNT\base\ntos\rtl\environ.c` (source SHA-256
`6c67b0c09c40e9186ce28a735221634bade1d9402be085d965223092aa7d4f00`).
The selected body has only its three include/declaration seams changed:
`ntrtlp.h`, `environapi.h`, and `zwapi.h` retain the finite standalone
PEB/VM and private-symbol boundary; they provide no environment algorithm.
The imported file SHA-256 is
`4c0e0e870d97f351cd8b046c2901ba841b439c83b75fdb8a6e936b0c89cf2b4b`.

`base/ntos/rtl/error.c` and its generated `error.h` table are the selected
D12 RTL slice.  They are directly reached by Base VDM, COMMAND, DEM,
Redirector, NetLib and SoftPC error consumers.  They were imported from
`O:\repos.external\OpenNT\base\ntos\rtl\error.{c,h}` (source SHA-256
`88bf09ab98778e0ca58d9d232d3b7aba6115223f43eddf981f6e06b786e77e48` and
`179794837a8610488218e5e48768d4d79f3d160bb952c92b172d12f6807984d2`).
The imported contents differ only by the repository terminal-newline form.
The original table and mapping algorithm remain intact; the sole finite
standalone binding supplies the original per-thread `LastStatusValue` field.

The package is a source mirror only at this point. It is not an enabled build
or runtime input until every reached CSR/private-host interface has a named,
same-shaped disposition. A later package-named subfamily under
`adapter-opennt-host` is permitted only for a private interface that the
owning original source proves cannot compose directly; it must not absorb
MVDM or generic Win32 semantics.

`netapi/netlib/ntstatus.c` is the second selected package slice. It is the
complete, byte-identical status-conversion unit directly reached by original
`mvdm-host/vdmredir/vrnetapi.c`. It preserves only the original
`NetpNtStatusToApiStatus` and `NetpApiStatusToNtStatus` algorithms; it does
not admit NetAPI services, RAP transport, NetBIOS, CSRSS, or a network
provider. Its identical three-source provenance and bounded disposition are
recorded in
[`m0-t324-s1-original-redirector-owner-binding-baseline-001.md`](../../docs/etc/evidence/m0-t324-s1-original-redirector-owner-binding-baseline-001.md).

`netapi/netlib/copystr.c` is the third selected NetLib slice.  It is the
complete, byte-identical OpenNT and OpenNT-4.5 conversion unit directly
reached by original `mvdm-host/vdmredir/vrnetapi.c` through
`NetpCopyWStrToStr`.  Its source-defined OEM conversion and bounded
destination contract remain in the original translation unit; it does not
admit a NetAPI/RAP transport or a replacement Redirector provider.  Its
secondary NetLib references are intentionally exposed to the formal link
audit rather than reimplemented in an adapter.

`netapi/netlib/allocstr.c` and `netapi/netlib/initoem.c` are the complete,
byte-identical OpenNT/OpenNT-4.5 direct NetLib support units reached by that
same `copystr.c` body.  They retain the original allocation, OEM-string
initialisation and error direction; their remaining NetLib dependencies are
likewise admitted only when the formal link audit proves a finite original
source path.

`netapi/api/apibuff.c` retains the source-identified `NetapipBufferAllocate`
true subset required by the selected `allocstr.c` unit.  It preserves the
original internal forwarding contract to public `NetApiBufferAllocate`; the
unreached RPC/MIDL allocator routines from the same original file are not
admitted merely to make the larger historical API product shell link.

## Divergence register

OPENNT-HOST-014 restores BaseGetVdmConfigInfo (209 original lines, one call-site
change) with original SHA-256
`2d57d7b0edaf2534e9a5654d3a2e30a6d91e2a24aea04fcc9e282af747a8c667`.
Its hardcoded system32/ntvdm search is replaced by the configured worker path
boundary; original quoting, -f/-i/-w/-a construction remains source-owned.
base_config.h/.c supply validated immutable app configuration instead of the
NT4 WOW registry. No host registry, environment or system-directory mutation.

OPENNT-HOST-014 also restores unchanged BaseCheckForVDM (39 lines) from the
same pinned base/win32/client/vdm.c for the launcher's task-exit query, not
image classification. Normalized function SHA-256:
`c6cf2dfdab281ee73c257c5f0a051eebe3439355f273d4a2a5eae5fa7e9d7b85`.
Native NtQueryEvent uses the original event ABI through declaration-only
base_event.h; original BaseSrvGetVDMExitCode retains completion consumption.
No replacement exit policy or full Kernel32 process shell is imported.

OPENNT-HOST-031 admits the three local capture-memory functions from pinned
OpenNT `base/ntdll/csrutil.c`, not its CSR/LPC transport. Original source file
SHA-256 is `7a176482498e1aca00e9651c326a9b62a8ba2ada0aa7f90f8b585acd1a376e6a`;
the unchanged 235-line group has LF-normalized SHA-256
`41d54b575c6895809c956136c3a35067dc3b87610a6226d8ec23fab431e11cab`.
CsrAllocateCaptureBuffer, CsrAllocateMessagePointer and CsrFreeCaptureBuffer
retain their pointer tables, alignment and original calling convention (/Gz).
A local process-owned heap replaces the port heap; tags are metadata only.
No shared port section, pointer rebasing or CsrClientCallServer body is imported.
The original library replaces the test-owned allocator; heap walks, actual
heap exhaustion, source identities and link ownership are verified in S2.

OPENNT-HOST-030 admits only `_UserTestTokenForInteractive` from pinned OpenNT
`windows/core/ntuser/server/exports.c` (original file SHA-256
`dbeb0ad48349bf5887a1333f21b427d8e0fdd3cb226977a9dc786373e961c71f`).
This directly serves srvvdm.c's OkToRunInSharedWOW. Its 67-line body is unchanged
(LF-normalized SHA-256
`7b6c9072220144e4b77aca973682838333994c3bc962eb802143e210e912a520`).
The true subset crops all unrelated winsrv exports and replaces precomp.h with
the finite private binding. Outgoing closure is native token query, public
LocalAlloc/LocalFree and the admitted scoped interactive-logon predicate.
No USER/Win32k subsystem, windowstation list, CSR shell or new token-query
policy is imported. Original BaseSrv SYSTEM/impersonation policy is unchanged;
actual SYSTEM impersonation is not tested by this fixture. Source-first
admission and the exact replacement predicate are recorded in S2 evidence.

T412 S2 registers OPENNT-HOST-028 in `base/win32/server/basesrv.h`: the
`OPENNT_BASE_VDM_SERVER` compilation group includes the finite BaseSrv-private
declaration binding and omits the unrelated DFS device header. The original
header is otherwise retained and srvvdm.c remains byte-identical. This removes
the generated empty DFS header from the selected original-owner library test.
OPENNT-HOST-029 restores original RtlConvertLongToLuid in the selected
`public/sdk/inc/ntrtl.h` subset, including its __inline storage shape; only
trailing blank-line whitespace is normalized. Origin is the same pinned
OpenNT public/sdk/inc/ntrtl.h. No generated function-body replacement is
needed; positive and negative sign-extension assertions pass. See S2 evidence.

T412 S2 extends OPENNT-HOST-014 with the complete original GetNextVDMCommand
body (390 lines including its signature/comment), selected by
`OPENNT_BASE_CLIENT_VDM_COMMANDS` for the focused original-client/server graph.
The default product still selects the environment cohort; the command provider
is not yet selected in the deployed product. Source: pinned OpenNT revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`,
`base/win32/client/vdm.c`; LF-normalized restored function SHA-256
`47cf285cbc505d9d43a50af16e283ea51d1d9ef8d2ff093795086d0ca327addd`.
Only cohort guards and the finite declaration include are added around it;
the function body is unchanged. The same cohort now also restores original
ExitVDM, SetVDMCurrentDirectories, GetVDMCurrentDirectories, CmdBatNotification
and RegisterWowExec as one contiguous 320-line source group. Its LF-normalized
SHA-256 is `2936de23dcbbdec23cf9ea23416606a7a3faeb96473d407e4bfaee00f89ac3aa`,
from the same pinned source/revision. No additional outgoing CSR operation
shape is needed beyond the four declared bindings. See the
[S2 execution evidence](../../docs/etc/evidence/m0-t412-s2-original-lifecycle.md).

BaseUpdateVDMEntry is also restored unchanged in that cohort (57 lines;
LF-normalized SHA-256
`de56cfccd08922ae57df7332cc8df70f60db186f44f177336188c2d236e9fd26`).
Its process-reference to wait-handle update is exercised against the original
server. The independent `OPENNT_BASE_CLIENT_CLASSIFIER` cohort now restores
GetBinaryTypeW (211 lines) and BaseIsDosApplication (60 lines), unchanged from
the same pinned source. LF-normalized hashes are respectively
`f7fd9766ea08e676a2fd457fb3fa508384630640032ce0615f752dc8a43f9fbd` and
`9487407a2efc02764466cedbabb4e89424fd12d87d11601cce9fa5ad477f3711`.
Its finite private header renames the imported Kernel32 symbol collision,
uses native NT file opening and supplies image declarations. The path binding
requests the native full-path result and marks the original relative result
empty, retaining the original full-path branch without passing an undersized
NT4 structure to modern NTDLL. Classifier bodies/branches are unchanged.
The focused x86 graph selects these bodies; final run16 selection is pending.

The command cohort also restores complete BaseCheckVDM (876 lines, same pinned
source; LF-normalized SHA-256
`579840e2d5598051d157d9e92871027b543ecfcba8d299a745928bfd71c6386e`).
No body change is made: command-tail/OEM conversion, PIF search, current
directory, startup capture and cleanup remain original. Private declarations
bind the finite process fields, modern Console capability predicate and the
historical Microsoft swprintf signature. The actual original client/server
test covers dispatch/deep copy, hotkey conversion and pre-dispatch failures.

| Identifier | Original purpose | Reason for divergence | Selected implementation | Files |
| --- | --- | --- | --- | --- |
| OPENNT-HOST-064 | BaseSrv native completion wakes the original GetNext command wait. | A decrement before the first no-command Get loses its notification at NtResetEvent; deterministic early/late service tests and DOS/native/DOS failure prove this host defect. | Preserve one coalesced pending native-return bit in the original Console record under its original critical section. Consume it only on RETURN_ON_NO_COMMAND with no queued command. Existing second-time, task completion, queue priority, count and event reset remain. This is local owner state, not a wire field or scheduler. | `base/win32/server/srvvdm.c`, `base/win32/server/srvvdm.h` |
| OPENNT-HOST-064 | BaseSrv native completion wakes the original GetNext command wait. | A decrement before the first no-command Get loses its notification at NtResetEvent; deterministic early/late service tests and DOS/native/DOS failure prove this host defect. | Preserve one coalesced pending native-return bit in the original Console record under its original critical section. Consume it only on RETURN_ON_NO_COMMAND with no queued command. Existing second-time, task completion, queue priority, count and event reset remain. This is local owner state, not a wire field or scheduler. | `base/win32/server/srvvdm.c`, `base/win32/server/srvvdm.h` |
| OPENNT-HOST-001 | `public/sdk/inc/ntexapi.h` declares the historical NT Ex system-service surface. | The selected non-MONITOR `softpc.new/host/src/nt_eoi.c` preserves its original include, and `nt_timer.c` reaches only its performance-counter/tick declarations. Importing the complete header would falsely enlarge the user-mode host closure. | Retain the source-identified true subset as the same-named OpenNT declaration carrier. The selected MVDM sources remain unchanged; no Ex service, controller, or kernel-VDM substitute is supplied. | `public/sdk/inc/ntexapi.h` |
| OPENNT-HOST-002 | `public/sdk/inc/ntpsapi.h` declares historical process/thread services. | Reached `nt_timer.c` needs only `NtAlertThread(HANDLE)` through the broad NT4 platform include closure; importing all process services would falsely enlarge the modern host boundary. | Retain that one byte-identical declaration. Its session-owned cancellation implementation belongs to the named MVDM host-out adapter, not this source mirror. | `public/sdk/inc/ntpsapi.h` |
| OPENNT-HOST-003 | `public/sdk/inc/ntrtl.h` is the original NT runtime declaration carrier. | The complete NT4 header reaches private product declarations outside the selected user-mode closure. | Retain a source-identified true subset of declarations reached by imported MVDM/OpenNT source; implementation binding remains in `adapter-mvdm-host-out/win32`. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-004 | `ntrtl.h` supplies the historical `DbgBreakPoint` declaration. | Modern SDK selection does not expose this exact declaration consistently. | Preserve only the source-facing declaration; the debugger adapter owns any enabled behavior. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-005 | `ntrtl.h` supplies the NT4 `TIME_FIELDS` layout and time-conversion declarations. | The selected modern include route omits this small public-NTDLL subset. | Preserve the original layout/declarations only; no CMOS policy or host behavior is added. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-006 | `ntrtl.h` supplies SoftPC timer RTL arithmetic, critical-section, and process-control declarations. | The selected modern SDK does not supply a coherent source-compatible declaration set. | Preserve the reached signatures and original `(HANDLE)-1` pseudo-handle spelling; an adapter must provide any non-exported behavior. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-007 | `public/sdk/inc/nturtl.h` carries NT runtime utility declarations. | Its full historical source closure is broader than the selected MVDM callers. | Retain only the reached declarations; this mirror adds no filesystem or process behavior. | `public/sdk/inc/nturtl.h` |
| OPENNT-HOST-008 | `ntpsapi.h` declares the reached `NtAlertThread(HANDLE)` surface. | The source-shaped public adapter must provide the historical name locally; ordinary `NTSYSAPI` import decoration would force an unavailable/private NT4 import instead. | Retain the original signature and select a local declaration only in the named adapter-backed SoftPC build; other consumers retain the historical declaration form. | `public/sdk/inc/ntpsapi.h`; `../adapter-mvdm-host-out/win32/{include/nt_thread_alert_compat.h,source/nt_thread_alert_compat.c}` |
| OPENNT-HOST-009 | `public/sdk/inc/ptypes32.h` carries the small historical 16/32 portability declaration set used by original SoftPC serial sources. | `nt_com.c` and `nt_wcom.c` directly include it, while the modern SDK has no same-named carrier. | Preserve the byte-exact selected OpenNT public header (SHA-256 `fbc65366df6b091aadbdc0ce12eb842bbd48691ccb1b9ce34c6d503226a7139b`); it contributes declarations only and no serial provider behavior. | `public/sdk/inc/ptypes32.h` |
| OPENNT-HOST-010 | `ntrtl.h` declares the original mutable RTL environment APIs reached by COMMAND. | The selected declaration subset initially omitted this small source-facing cluster, leaving original callers to compile by implicit declaration. | Retain the original declaration shapes; selected consumers enter the private `OpenNtRtl*` symbol domain implemented by the admitted original D11 body, not modern NTDLL or an adapter algorithm. | `public/sdk/inc/ntrtl.h`; `base/ntos/rtl/{environ.c,environapi.h}` |
| OPENNT-HOST-011 | `ntrtl.h` declares `RtlOemToUnicodeN`, reached by original SoftPC keyboard conversion. | The selected true subset omitted this fixed-buffer OEM-to-Unicode form although the original `nt_keycd.c` includes and calls it. | Restore only the original declaration shape; the public Win32 implementation remains adapter-owned. | `public/sdk/inc/ntrtl.h`; `../mvdm-host/softpc.new/host/src/nt_keycd.c` |
| OPENNT-HOST-012 | `ntrtl.h` declares OEM conversion forms reached by original NetLib `copystr.c`. | The admitted true subset omitted the fixed-buffer conversion and size contracts although the selected original Redirector dependency includes them. | Restore only the original signatures; the selected x86 NTDLL/import-library binding is verified as part of the Redirector network closure. | `public/sdk/inc/ntrtl.h`; `netapi/netlib/copystr.c` |
| OPENNT-HOST-013 | `netapi/api/apibuff.c` provides historical public and internal NetAPI buffer entry points. | The selected Redirector NetLib closure reaches only its `NetapipBufferAllocate` compatibility forwarder; importing the other functions falsely pulls an RPC/MIDL product shell. | Preserve the original function unchanged as a source-identified true subset and call the public modern `NetApiBufferAllocate` contract. | `netapi/api/apibuff.c`; `netapi/netlib/allocstr.c` |
| OPENNT-HOST-014 | `base/win32/client/vdm.c` supplies `BaseCreateVDMEnvironment`, `BaseDestroyVDMEnvironment`, and `BaseGetEnvNameType_U` before original Base creates an NTVDM child. | The full source file pulls the excluded Base/CSR process-creation product shell, while the selected CLI needs this exact environment projection before its direct SoftPC entry. | Preserve the three original bodies as a source-identified true subset. Its clone uses the admitted original D11 RTL body; the separate adapter owns only current-process installation/restoration around direct entry. | `base/win32/client/vdm.c`; `base/ntos/rtl/environ.c`; `../ntvdm/win32/mvdm_base_vdm_environment.c` |
| OPENNT-HOST-015 | `base/ntos/rtl/environ.c` provides original create/clone/query/mutate/destroy ownership for ordered UTF-16 MULTI_SZ environments. | Original MVDM/VDM callers require the complete D11 algorithm, but standalone cannot expose NT4's real PEB or private kernel VM service shell. | Preserve the selected body under its original-relative path. Its only outgoing closure is a private PEB projection, PEB lock, public user-mode NTDLL `Zw*VirtualMemory` declarations and private symbol names, supplied by the bounded adapter seams. | `base/ntos/rtl/{environ.c,ntrtlp.h,environapi.h,zwapi.h}`; direct consumers `base/win32/client/vdm.c`, `../mvdm-host/dos/command/cmdenv.c` |
| OPENNT-HOST-016 | `base/ntos/rtl/error.c` and generated `error.h` map NTSTATUS values to DOS/Win32 errors and record the reached status in the TEB. | Modern NTDLL exports the historical names, so it cannot be mixed with the selected original table under the public symbol names; the standalone process also cannot expose the NT4 TEB layout. | Preserve the selected original source/table under the original-relative path and route selected callers to a private symbol domain. The adapter contributes only the source-shaped TLS `LastStatusValue` carrier; it supplies no mapping entries or fallback policy. | `base/ntos/rtl/{error.c,error.h,ntrtlp.h}`; `public/sdk/inc/ntrtl.h`; `../adapter-mvdm-host-out/win32/{include/nt.h,source/opennt_support_rtl.c}` |
| OPENNT-HOST-068 | BaseSrv stores numeric PID/TID and wait-receipt identities in HANDLE-shaped local carriers. | AMD64 rejects direct DWORD/pointer conversions and truncation in parent bit-zero masking. | Seven expressions convert through ULONG_PTR; tag mask retains full carrier width. Native srvvdm TU alone binds NULL=0, avoiding eleven irrelevant literal-zero edits. No structure, wire, scheduling, completion or cleanup algorithm changes; x86 remains supported. [Audit](../../docs/etc/evidence/m0-t433-s6-native-service-mirror-audit.md). | `base/win32/server/srvvdm.c`; `../../tools/build/New-NativeWorkerNinja.ps1` |
