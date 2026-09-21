# T420 S40 WOW USER profile evidence

## Question and inputs

Determine whether immutable USER16 uses native-x86 client optimizations, which
original WOW32 alternatives exist, and what must be restored without guest
patches or duplicate API algorithms. This is supporting research, not S40
closure or functional acceptance.

Inputs are selected `src/mvdm/wow16/user`, `src/mvdm/wow32`, the deployed
`O:/winnt/system32/USER.EXE`, and read-only original
`O:/repos.external/OpenNT/windows/core/ntuser/{client,rtl}/wow.c`.

## Reproducible procedure and observations

### Pinned consumer layout and original platform owners

The profile gate now checks 47 instruction patterns in named routines of the
unchanged, hash-pinned USER.EXE. It passes and emits `binarySharedAbi` alongside
the 26 source mappings. These are binary consumer requirements, not claims that
the current product supplies them:

| Consumer | Actual required layout |
| --- | --- |
| HMValidateHandle | SHAREDINFO pointers at 0/4; handle count at SERVERINFO+4; entry stride 16, object +0, type +8, uniqueness +10 |
| GetSystemMetrics / GetSysColor | SERVERINFO arrays at 0x170 / 0x2a0, counts 76 / 25 |
| GetCursorPos | SERVERINFO coordinates at 0x304 / 0x308 |
| ValidateHwnd / GetDesktopWindow | TEB self +0x18, client info +0x44, expected version +0x50, desktop info +0x5c, client delta +0x60, cached HWND/PWND +0x6c/+0x70 |
| GetTickCount / GetCurrentTime | Fixed linear address 0x7ffe0000; DWORD tick/multiplier at +0/+4; multiply then shift by 24 |

In particular, the consumer's `shl index,4` requires a 16-byte handle entry.
The original user.h has a 12-byte non-DEBUG entry and an additional pointer in
its DEBUG layout. Choosing a host compiler's current sizeof is therefore not
a valid way to publish this particular guest ABI. This observation does not
by itself identify the build options of every other guest structure.

Original NT supplies the platform preconditions outside DOSX:

- base/ntos/ke/x86/x86init.c initializes KGDT_R3_DATA as flat user data and
  KGDT_R3_TEB as a user data descriptor. SHA-256:
  88B122CB286317057A103EE4A8CC2538F51EF2545C88EC0A482E4A7D93E64351.
- base/ntos/ke/x86/ctxswap.asm updates all three TEB descriptor base fields
  from the scheduled thread's ThTeb. SHA-256:
  8FFAFDBFB8C27D0CB6F85754ED03C7DA0923CE651CA45AD5F3272428DA5C5240.
- base/ntos/ke/x86/clockint.asm maintains the shared time data. SHA-256:
  C4FE36FA2041AF90DB2516FD151E2BA687EFBBE9287706A20EC84D4F91DE9373.

The selected WOW DOSX selector definitions start at 0x80; dxboot.asm clears
its table before publishing its predefined descriptors. Current InitializeDosx
copies the source table to separate CCPU GDT/LDT shadows. Those copies alone
do not implement the above NT kernel-owned descriptors or per-thread state.
No current worker provider for the required original TEB/client-info layout
was found in the inspected worker, DPMI32 and opennt-host sources. This is a
source audit result, not a runtime snapshot of every descriptor.

The fixed clock address also needs a real guest mapping. The current physical
mapping adapter bounds registrations to guest RAM; CCPU phyR rejects physical
addresses outside Length_of_M_area before alias resolution. With paging off,
a high linear address is not made readable by publishing a host pointer.
Original CCPU paging can translate linear addresses before physical access;
whether to use that mechanism requires integration validation, not an opcode
exception or a claim that paging is already enabled. No CPU, DOSX or guest
media was modified by this investigation.

The next implementation boundary must consequently cover descriptor publication,
thread-bound original-layout data, shared USER object lifetimes and clock
mapping together. A zeroed SHAREDINFO or partial callback table is not an
acceptable substitute. Existing native callback fixtures do not close these
remaining real-guest requirements.

Run `tests/observation/verify-wow-user-profile.ps1` from the repository.
The check hashes both selected and deployed USER.EXE, parses the NE segment
table, resolves symbols in the retained original map, checks actual instruction
bytes, and enumerates all LOCALAPI mappings with their original function bodies
and dispatch entries. Observed result: `WOW_USER_PROFILE_SOURCE_AND_BINARY_OK`.
This read-only check neither launches nor changes the guest.

Both images have SHA-256
`6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C`.
Segment 1 starts at file offset 0x25e0. At mapped IGetSystemMetrics (0x61c6),
the image loads DS=0x23 and FS=0x3b. HMValidateHandle (0x08f8) loads the
DWORD at CS:0x5da2, the mapped wow16gpsi symbol, then dereferences shared data.
The retained userk.lrf selects wowk.obj and wowkr.obj. These observations
establish the selected PMODE32 guest profile, not merely a possible source path.

The original UserRegisterWowHandlers returns `&gSharedInfo` (SHAREDINFO), not
a bare SERVERINFO pointer. WU32NotifyWow copies that value and &CallCsrFlag
into guest destinations only when those destinations are supplied. USER16
init.c supplies them under PMODE32; its other build branch supplies zero.
Neither branch is a runtime switch in the current guest binary.

The current S40 provider-formal-r4 wusercli.obj contains
WU32GetClientRect, WU32GetSystemMetrics and WU32GetTickCount (dumpbin /symbols).
They are guarded by !PMODE32. This proves retained host thunk bodies, not
guest selection or successful execution. Host compilation and guest profile
must not be conflated. Changing host PMODE32 also changes other original
conditionals and is not an admitted global fix.

## Complete 26-mapping dependency disposition

Every row has an original WU32 thunk in wusercli.c and a wutbl2.h entry.
The following dependencies describe the immutable optimized guest algorithm.

| API | Original data dependency |
| --- | --- |
| ClientToScreen | Validated WND.rcClient; original signed-coordinate saturation. |
| GetClassName | WND class reference, CLS and ANSI string. |
| GetClientRect | WND client rectangle, minimized/version flags and system metrics. |
| GetCursorPos | SERVERINFO cursor position and signed-coordinate saturation. |
| GetDesktopWindow | Thread CLIENTINFO, DESKTOPINFO and desktop WND. |
| GetDlgItem | WND child/sibling links and control ID. |
| GetMenu | WND menu reference or child control ID. |
| GetMenuItemCount | Validated MENU item count. |
| GetMenuItemID | MENU item array, ID and submenu reference. |
| GetMenuState | Original recursive item lookup, type/state and popup count. |
| GetNextWindow | Same guest GetWindow implementation as the explicit GetWindow row. |
| GetParent | WND child/popup type, parent and owner links. |
| GetSubMenu | MENU item array and submenu reference. |
| GetSysColor | SERVERINFO color array and original bounds. |
| GetSystemMetrics | SERVERINFO metrics and CLIENTINFO expected-version corrections. |
| GetTickCount | Shared-user-data tick/multiplier; also serves GetCurrentTime. |
| GetTopWindow | Desktop/window first-child link. |
| GetWindow | WND graph, desktop type and compatibility flags. |
| GetWindowRect | Returns rcWindow address for USER16's subsequent copy. |
| IsChild | Child flag and ancestor links. |
| IsIconic | WND minimized flag. |
| IsWindow | Handle type/uniqueness and destruction flag. |
| IsWindowEnabled | WND disabled flag. |
| IsWindowVisible | Ancestor visibility and desktop termination. |
| IsZoomed | WND maximized flag. |
| ScreenToClient | Validated WND.rcClient and signed-coordinate saturation. |

Handle-based rows additionally depend on original handle validation, CLIENTINFO
and pointer rebasing. A structure header alone cannot satisfy that graph.
PMODE32 GetTickCount directly reads MM_SHARED_USER_DATA_VA; gpsi alone cannot
satisfy it. CallCsrFlag fallback is generated only for srvcond calls, not for
all 26 mappings. Setting it cannot redirect all existing guest entrypoints.

## Recovery decision and limits

Retain original thunk algorithms and immutable embedded guest algorithms.
No new mirror algorithm or guest patch is justified by this mapping audit.
Modern public USER supplies native resources; any necessary original-layout
guest data must have one worker-local owner and use existing address mapping.
Exact layouts, mutation publication, callback ordering, stale-handle behavior,
thread switching and cleanup remain implementation/validation obligations.
Zero added mirror algorithm lines is a design target, not measured completion.

Recovery ladder: the guest algorithms are already composed in original media;
ordinary thunk algorithms are already available as original host source.
Direct modern-private-data reuse is not established. A bounded same-shaped
adapter remains the candidate for unavailable NT4 data publication. Importing
USER/GDI server or authoring a parallel window manager is not authorized by
this evidence. No CPU instruction interception or guest patch is proposed.

The searched local OpenNT/OpenNT-4.5/opennt-src-2 trees yielded no distinct
non-PMODE32 USER.EXE; the two located opennt-src-2 copies have the hash above.
This is a bounded local search, not proof no historical edition exists.
Separate original-path guest probes may verify ordinary thunk contracts;
they cannot claim full non-PMODE32 USER16 media acceptance.

Next validation must cover real optimized reads and ordinary dispatch,
creation/mutation/destruction, invalid handles, version-sensitive metrics,
coordinates, time progression and worker teardown. Existing DOS regression
and provider linkage do not prove these properties. S40 remains incomplete.

## Default-window-procedure dispatch filter recovery

The existing ADAPTER-WOW-013 supplied only WM_NCCREATE and WM_NCCALCSIZE.
Original USER16 user.asm::CheckDefWindowProc returns zero for unmarked
messages, so that subset suppresses normal default processing including
WM_CLOSE, WM_PAINT and WM_GETTEXT. The original USER owner does not select
this filter according to which messages an earlier trace happened to reach.

The adapter now materializes the original non-FE_IME union from
windows/core/ntuser/kernel/server.c (39 plus 30 entries, including the retained
duplicate WM_QUERYDRAGICON). A read-only token-sequence comparison against both
original arrays passes, with exact order. The original
client/client.c::WowGetDefWindowProcBits explains the union and maximum-message
contract. Kernel server initialization/desktop allocation cannot compose at
this public USER boundary; only its constant message data is retained here.
No USER server, window policy, mirror file or guest modification is added.
Private numeric values are from original public/internal/windows/inc/winuserp.h.

Source hashes (SHA-256): server.c
`3C6131EFB37173F5A280BD77EC1D887529AFB35EEF03B6F0248A3C75E7887047`;
client.c `787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472`.
The original Microsoft copyright attribution is retained beside the tables.

Build root: build/M0-T420/S40/user-bitmap-r1. MSVC 14.43.34808 Hostx86/x86,
/MT /W3, compiles wow_def_window_bits_fixture.c and the production
wow_private_user_compat.c with its owning include directory, linking user32.lib
and gdi32.lib. Result: WOW_DEF_WINDOW_BITS_BOUNDARY_OK, exit 0. It checks both
message families, private messages, the original maximum, excluded messages,
null/short spans and buffer guards. The initial fixture incorrectly treated
99 bytes as too short; WM_PRINT=0x317 requires exactly 99 bytes. The corrected
negative case uses WM_PRINT/8 (98 bytes). Production compilation still emits
the pre-existing resource-type C4047 warnings outside this function.

This is a host-only boundary fixture, not CCPU execution or real Win16 UI
acceptance. The registration table is still incomplete, so product use and
real close/text/paint behavior remain unverified. FE_IME is not added by this
non-FE_IME source selection. S40 remains open.

## Registration and historical-binding audit

Original wowuserp.h has 21 input callbacks and 20 output callbacks plus the
build DWORD. Original client/client.c stores the incoming callbacks, publishes
the output functions and returns &gSharedInfo. Populating output addresses
alone does not restore resource, window-procedure or dialog callbacks.

| Output slot | Current recovery obligation |
| --- | --- |
| CsCreateWindowEx | Original creation flags, WOW words and callback-before-return ordering. |
| DirectedYield | Existing adapter needs original cooperative task/wait/lifetime proof. |
| FreeDDEData | Original DDE ownership/release flags; no generic GlobalFree substitute. |
| GetClassWOWWords | Source-shaped class words and instance/name identity. |
| InitTask | Existing adapter ignores version, placement and hotkey; incomplete. |
| RegisterClassWOWA | Native proxy plus original class words/resource identity. |
| RegisterUserHungAppHandlers | Existing validation-only adapter; absent producer must be explicitly dispositioned. |
| ServerCreateDialog | Original dialog conversion/initialization callback and modal/modeless flags. |
| ServerLoadCreateCursorIcon | Existing resource adapter requires original-format and identity verification. |
| ServerLoadCreateMenu | Existing adapter requires resource/callback/lifetime verification. |
| WOWCleanup | Existing success-only adapter relies on a window owner absent from current source. |
| WOWFindWindow | Original matching and class identity. |
| WOWGetIdFromDirectory | Original LookupIconIdFromDirectoryEx call recovered; resource test outstanding. |
| WOWLoadBitmapA | Original resource-data/size and bitmap ownership contract. |
| WowWaitForMsgAndEvent | Message/event result and cooperative scheduler integration. |
| YieldTask | Existing adapter needs original cooperative task/wait/lifetime proof. |
| GetFullUserHandle | Shared handle identity/uniqueness contract. |
| GetMenuIndex | Original submenu-index/failure contract. |
| WowGetDefWindowProcBits | Source union and x86 boundary proof above; real registration/UI still outstanding. |
| FillWindow | Original parent-color callback/brush/DC behavior. |

Nine output slots have named candidate functions in wow_private_user_compat.c;
this corrects the earlier conversational count of eight. These candidates are
not a working registration table and several are partial. Current W32Init
still invokes the system UserRegisterWowHandlers. No local complete registrar
is present in the selected source.

The current README's ADAPTER-WOW-003 names wow_window_projection.c/.h, but
neither exists in the current tree. FindPWW/FindPWC still call GetWindowLong
and GetClassLong with private indices. Historical snapshot ef1c3cb28 retains
a projection candidate under adapter-mvdm-host-out/wow. Inspection
shows host-only class/window words, a native proxy and a separate sequential
16-bit window-ID namespace. It has no SHAREDINFO/guest WND graph, and writes
several creation fields only after CreateWindowEx returns (creation callbacks
occur before that). It therefore cannot be bulk-restored as a complete PMODE32
solution. Its reuse remains per-mechanic, not a revival of that implementation.

The native probe wow_user_private_words_probe.c creates only its own invisible
message window, queries private indices/low16 aliases, then destroys its objects.
Initial deployment review rejected two attempts; the later completed r3 run
below supersedes that historical lack of runtime evidence.

## Follow-up source review and mirror minimization

The pending W32Init registry edit contained unreachable if(FALSE) DEBUG
blocks and had removed that branch's cb declaration and final key close.
Restoring the original DEBUG block reduces its pending HEAD delta from 106
changed lines to +15/-3 (18). Selected retail behavior remains the original
absent-value ThunkNLS computation and no registry probe. DIV-281 already
belongs to S38 debug-register recovery; the WOW edit is registered as DIV-291.
The current provider-formal-r4 Ninja command for obj/wow32.obj was executed
directly under MSVC x86 /MT and returned 0. No final DLL relink, deployment or
real WRITE success is claimed. An empty KnownDLL list is not proof of correct
package search; that remains loader acceptance work.

Further original callback contracts are now pinned:

- kernel/ntstubs.c::NtUserGetMenuIndex validates both menus, searches immediate
  items and returns -1 when valid menus have no matching child. Receiver
  validation precedes that default assignment; its failure result must be
  traced separately rather than inferred from the search loop.
- WOWFindWindow calls _FindWindowEx with FW_16BIT. kernel/winmgr.c filters
  owning threads by TIF_16BIT. Bare modern FindWindow could return an unrelated
  native application and is not an equivalent adapter.
- kernel/paint.c::xxxFillWindow/xxxPaintRect performs parent-DC clipping,
  brush-origin adjustment/restoration and WM_CTLCOLOR lookup for color-index
  brush values. Bare FillRect loses those semantics. CS_PARENTDC clipping
  needs actual client geometry; the original explicitly avoids GetClientRect
  there because minimized legacy compatibility can return different geometry.

These constrain implementation; they are not runtime-passed callbacks or
authority to import the USER server.

## Native USER private-word probe: successful authorized execution

After the owner approved all similar test deployments, the native x86 /MT
probe executed from O:/winnt/tests/S40USER. This supersedes the earlier
not-run state, not the recorded history of the rejected deployment attempts.
No guest media or runtime product executable was modified. The probe owns an
invisible message-only window and its class and destroys both before exit.

Inputs and reproduction:

- Source: tests/adapter-mvdm-host-out/wow/wow_user_private_words_probe.c,
  SHA-256 929C6AABC3A4D1F676A5B80F10ED753967B013101ED103D4277A88227634EFB0.
- MSVC 14.43.34808 Hostx86/x86, `/MT /W3`, user32.lib, output
  build/M0-T420/S40/user-bitmap-r1/wow-user-private-words-probe-r3.exe,
  SHA-256 FD20BF4C8D9120AF8B33A27A39ED866E2AA00ED5D6CEEF113ED5E5C0949AAED6.
- Host C:/Windows/SysWOW64/user32.dll version 10.0.26100.8117,
  SHA-256 BFF75906F4ABCC050CF85E38E20090EA0EDF7DC33E0ED60CB74CF9E1BC007527.
- Runtime log: O:/winnt/logs/s40-user-private-words-r3.txt; process exit 0
  means the observational probe completed, not that WOW compatibility passed.
- Original contract: wowuserp.h::WW (eight DWORDs), walias.h::WC (two DWORDs),
  winuserp.h private indices; OpenNT USER client/classc.c and kernel/getset.c,
  kernel/class.c implement the original read-only pointer/setter contract.

Observed results:

| Contract | Actual observation | Disposition |
| --- | --- | --- |
| GWL_WOWWORDS (-1) | Nonzero, committed readable pointer; checked 32-byte copy via VirtualQuery and ReadProcessMemory. | A nonzero return is not absence, but is also not NT4 ABI compatibility. |
| Original WW layout | DWORD 6 differs from GWL_STYLE; DWORD 7 differs from GWL_HINSTANCE. | Cannot cast this pointer to original PWW. |
| GWL_WOWDWORD1/2/3 (-30/-31/-32) | Every setter returns error 1413, ERROR_INVALID_INDEX; expected fields remain unchanged. | Original three-field write contract unavailable for this native window. |
| GCL_WOWWORDS (-27) | Zero, no error, before and after setter attempts. | No readable original WC published. |
| GCL_WOWDWORD1 (-28) | Setter returns 1413. | Not an original writable WC slot. |
| GCL_WOWDWORD2 (-29) | Setter and restoration report no error, but WC pointer stays zero. | No claim that this index still has its original meaning; a no-error return alone proves neither storage nor layout. |
| Original sign-extended low16 HWND | IsWindow accepts full and narrowed identity; both fail immediately after DestroyWindow. | This individual handle-alias/lifetime check passes; handle-reuse and guest integration still need verification. |

The probe never writes through the returned pointer. A successful setter is
restored with its returned previous value; rejected window writes require no
restoration (r3 logs restore_error=0 for that skipped operation). No unrelated
window, system service, host registry or guest participates.

Consequences for recovery: selected walias.c::FindPWW currently casts -1's
return and consumes iClass/flState without a layout check; FindPWC consumes
-27 as PWC. The current provider build does not substitute these private
indices. This is a demonstrated integration-contract mismatch, not proof that
CCPU or original WOW32 algorithms are wrong, nor proof of the first real WRITE
failure. Restore the original private-word ABI at the finite USER binding;
keep public Get/SetWindowLong and class operations native outside those exact
indices. Do not revive historical sequential window IDs merely because word
storage needs a binding. The PMODE32 shared-object layout is a separate guest
consumer contract and is not supplied by these native pointers.

## Source-shaped menu and resource recovery

The selected USER boundary is still one incomplete S40 package, not a new
sequence of trace-selected mini-packages. These recoveries implement audited
output-slot contracts; they do not complete registration or guest acceptance.

### Immediate menu index

ADAPTER-WOW-021 retains the original NtUserGetMenuIndex validation/search
ordering. Sources: OpenNT windows/core/ntuser/kernel/ntstubs.c, SHA-256
A5997517B8BBB0B9E69AFC973F8BF26D8859B6FB9F4C8C1513553C52E3EE2918;
validate.c, SHA-256
661748C142E662966844C3820CCEB09670F192C9B981586F80FA696106A15A62.
BEGINRECV_SHARED(DWORD,0), ValidateHMENU and MSGERROR prove that invalid handles
return zero, before the valid-but-absent -1 default is assigned. Validation
sets ERROR_INVALID_MENU_HANDLE. The original loop searches immediate items,
not grandchildren. The real WOW consumer is wmsg16.c's WM_MENUSELECT thunk.

Direct composition of the kernel translation unit requires USER critical
sections, PMENU and desktop/thread internals, a declared stopping boundary.
The smallest same-shaped adapter keeps the original search rule and error
ordering, using public IsMenu/GetMenuItemCount/GetSubMenu for those unavailable
reads. Only already-validated sign-extended WORD menu aliases are compared to
the returned native child identities; there is no new handle namespace. No
mirror intrusion or autonomous menu policy is required. Native serialized
tests do not prove cross-task scheduling/atomicity; that remains the USER
package's cooperative-task integration requirement.

The x86 /MT fixture wow_menu_index_fixture-r2.exe passed 14 assertions covering
immediate match, not recursive, absent/self, both narrowed arguments, invalid
arguments, detach and destroyed-child handling and cleanup.
Log: O:/winnt/logs/s40-wow_menu_index_fixture-r2.exe.txt.
SHA-256 F03C4BF6D170DC5DA4D4C37DEA35C1687B3AFB42683483709B15515C8743000B.
Ten output slots now have candidate functions (including this addition), but
several remain partial and none of these native tests closes the registrar.

### Cursor/icon conversion: remove the duplicate DIB implementation

The original private WowServerLoadCreateCursorIcon and public
CreateIconFromResourceEx share ConvertDIBIcon in client/clres.c, SHA-256
C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5.
The original private function ignores dwExpWinVer; it is not a resource-format
version. It uses default dimensions and shared-resource flags, adds
LR_MONOCHROME for cursors, and never falls through to a named resource after
failed supplied-data conversion. WU32LoadCursor already supplies Unicode names.

The existing 193-line manual conversion/wrapper was replaced with 29 lines:
same-shaped callback, source-defined flags, resource format 0x30000 for the
public conversion, and LoadImageW only for the original no-data branch.
This removes 164 lines from that block and restores core-header, true-color,
default-size, hotspot and invalid-data behavior. Full clres.c composition
would bring USER caches, CURSORDATA and server internals; the public entry is
the finite same-algorithm boundary, so neither a second DIB parser nor a
mirror intrusion is needed. The original guest alias owner remains
wcuricon.c::SetupResCursorIconAlias (current source SHA-256
5F654AB2A98B2925474DFFD49C5A3ABAAEFACF6193375AF3794E70731C4E8673).

Modern API contracts were also checked against Microsoft's
[CreateIconFromResourceEx](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createiconfromresourceex)
and [CopyImage](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-copyimage)
documentation. Their contracts guide testing, not a substitute for the
original source or actual host results.

Baseline r1 rejected all three independently constructed valid resources;
the direct public route created them with the expected dimensions/hotspots.
Expanded r2-r5 initially used an invalid oracle: a monochrome cursor was
required to have no color bitmap or a 1-bit GetIconInfo result. Original
ConvertDIBBitmap distinguishes source-monochrome from requested-monochrome;
the former assertion was too strong. Readback of actual RGB pixels in r6/r7
proves that modern 32-bit storage still contains black/white cursor pixels,
while the corresponding icon preserves non-black/white pixels. No extra
CopyImage conversion is required; experimental copies were removed from the
retained fixture rather than added to the product.

Final current-binding fixture wow_cursor_resource_fixture-r7.exe passes:
INFOHEADER 1/24-bit icon/cursor, COREHEADER icon/cursor, default 32x32 size,
icon-center/cursor-scaled hotspots, actual pixel semantics, shared stock-arrow
identity, and invalid/empty/wrong-type refusal without fallback.
Log: O:/winnt/logs/s40-user-cursor-binding-r7.txt.
SHA-256 7FBACA57E979C06257660C3C4CC1D10855E1E606D459C155D32961CB5BC5CA1F.
Repeated raw-data creates return different handles even with LR_SHARED on this
host; no cached-identity claim is made. Original guest alias reuse, mutation,
task/module cleanup and resource accounting still require real integration.
Shared test resources are process-owned until test exit; GetIconInfo bitmap
copies are explicitly deleted. All payloads are self-authored; no original
guest media or product executable was changed.

The default-window bitmap regression also passes on the same production
source (wow_def_window_bits_fixture-r2.exe, SHA-256
3EDA645C122AA5969C1AA086BAA417E4B0CA737E1198F2CEFB1AB75DA74CD36D).
These are native x86 /MT boundary tests, not a CCPU/WOW guest run or formal
product acceptance. Full table, shared-data, scheduler and callback lifecycle
remain S40 obligations.

The current production adapter also compiles with the selected formal
provider-formal-r4 command for obj/adapter-wow/wow_private_user_compat.obj:
MSVC x86 /MT, CPU_40_STYLE/CCPU/C_VID, exit 0. This is an object rebuild, not
a final DLL link or publication. Relative to HEAD, the adapter C file is
+92/-215 (net -123 lines, including earlier S40 bitmap recovery and the menu
addition); its header adds one declaration. No mirror file changed in this
menu/resource step, and no overlay was added. The queue and experience-repair
proposal edits made elsewhere were preserved without modification.

### Original CCPU high-linear-page execution proof

The existing bounded CCPU fixture now tests the actual original paging
mechanism before any product mapping is implemented. Selected c_tlb.c is
byte-identical to OpenNT, SHA-256
A83A7DD3F7E3FBB83723047AACDDBD22D4001591C6D444D4BD6D45DD73E38FD8.
The selected c_page.c and c_reg.c differ only by their existing declaration
includes/comments; this step changes no CPU or other production source.

Reproduce with New-T313CcpuLifecycleNinja.ps1, Architecture x86 and BuildRoot
build/M0-T420/S40/ccpu-shared-page-r1. The graph's selected source commands
were executed directly with MSVC x86 /MT after the owned Ninja process stopped
making progress; that process was explicitly terminated before retrying.
The old fixture link had duplicate c_sas_touch/c_VirtualiseInstruction stubs
and missing host_hwint_hook/host_applClose seams. Duplicate stubs are removed
in favor of the selected real fallbacks. Unexpected hardware-hook traversal
now aborts; the bounded adapter's one close call is counted. Those seams do
not claim real host/device/scheduler teardown coverage.

First runtime r1 failed with a native access violation: the fixture wrote
fixed extended-memory addresses without the original allocation/commit
contract. nt_mem.c initially commits only real-mode RAM plus A20 wrapping
space, not the entire SAS reservation. The fixture now obtains and releases
six pages through original VdmAllocateVirtualMemory(..., TRUE) and
VdmFreeVirtualMemory. This is a test correction, not a CCPU repair.

Final r4 passes both markers, exit 0:

- CCPU_HIGH_LINEAR_PAGE_BOUNDARY_OK: map 0x7ffe0000 into allocated ordinary
  physical RAM; user read succeeds, user write is rejected, absent page is
  rejected; scalar and SAS bulk reads span two noncontiguous physical pages;
  CR3 reload invalidates cached mappings on remap and unmap.
- CCPU_HIGH_LINEAR_CLOCK_INSTRUCTIONS_OK: actual c_cpu_simulate reached through
  original host_simulate executes a bounded self-authored program at CPL=3,
  DS=23h. It reads the high page and executes the same MOV/MUL/SHRD forms
  observed in USER16's clock helper, producing the expected value before
  original D6 FE unsimulate returns. No guest executable was patched.

Artifact: O:/winnt/tests/S40USER/ccpu-shared-page-r4.exe, SHA-256
695F3945E552C17AB72655FF379308B4293240783387C88A3EB3239C9E1055C9.
Logs: O:/winnt/logs/s40-ccpu-shared-page-r4.stdout.log and matching stderr.log.
The stderr pre-initialization bad-TLS diagnostic belongs to the existing
negative accessor test; final acceptance requires exit 0 and both markers.

This proves original CCPU machinery can serve the required high linear page
without a new instruction or physical-access algorithm. It does not prove a
live USER16 task, actual clock updates, thread-local TEB, or product lifecycle.
Before enabling it in the worker, the binding must preserve identity access
to ordinary RAM, account for the lease provider's current physical-size
range check, and maintain paging/descriptor state through DOSX mode changes
and teardown. Current modesw.c does not yet own that paging lifecycle. The
20-slot USER registrar and shared object graph also remain incomplete.

### Existing lease publication and object-family consumer offsets

Follow-up r5 adds a bound worker session and its unchanged guest-memory lease.
A direct high-address lease is correctly refused. Writing the low canonical
backing through the existing lease immediately changes the value read by the
high-address guest instruction program. No broader lease address policy is
needed for this publisher. The fixture then disables PG before clearing PE,
checks the low-address lease in that state, sets PE before restoring PG and
re-executes the original clock instruction forms successfully. This is the
ordering required by original mov.c::MOV_CR, which faults on PG=1/PE=0; it is
not yet a test of real DOSX transition frames or the product's binding hooks.

R5 exits 0 with the previous two markers plus
CCPU_SHARED_PAGE_EXISTING_LEASE_AND_MODE_ORDER_OK. Artifact:
O:/winnt/tests/S40USER/ccpu-shared-page-r5.exe, SHA-256
1D6192A113BDBC8812508C9B1D16A50EA2C7728E78911B90848ECCD474DBD3CE.
Logs use O:/winnt/logs/s40-ccpu-shared-page-r5.{stdout,stderr}.log.

The immutable USER profile gate now passes 44 instruction-pattern checks.
Its binarySharedAbi additionally records WND parent/child/next/owner links,
rectangles, class and menu/control-ID references, compatibility/style bytes,
desktop function ID, CLS ANSI-name pointer, HANDLEENTRY destruction flag,
and MENU/ITEM count, array, type/state, ID and submenu fields. ITEM stride is
64 bytes. Names are cross-checked with original user.h and generated
wowk.asm/wowrtl.asm; instruction offsets are checked against the deployed
hash-pinned binary. These are consumer fields, not a reconstructed full
sizeof(WND) or proof that a live object graph has been published.

Current wow32.c only receives/stores gpsi; wuman.c publishes it to guest.
That does not require a native dereference of gpsi elsewhere in WOW32, so a
guest-numeric SHAREDINFO remains viable. The separate &CallCsrFlag publication
still needs an owned guest-visible byte; leaving that raw native pointer is
not justified by these tests. CPU, lease and guest media remain unchanged.

### Native backing and lifecycle audit

Question: can the existing memory owner expose native-owned clock backing
without another physical-map algorithm or a retained guest lease?

Original `base/ntos/mm/mminit.c` creates the read-only user mapping of the
kernel-owned shared page. Reference SHA-256:
5266F711CC5BF4574C43DC78E6723B41A0B66BD42009B00A7D130E9F55876B9C.
Original `base/ntos/ke/x86/clockint.asm` updates UsTickCountLow independently
of WOW transitions. Consequently, publishing time only at BOP/callback entry
would freeze a guest-only polling loop; that is not an acceptable clock.
PFN, IRQL and kernel VAD machinery are not a standalone implementation.

R6 extends the existing bounded fixture: native VirtualAlloc backing is
registered by original VdmAddVirtualMemory, exposed through the existing
physical translation and original CCPU page table, and read by actual guest
instruction execution. An InterlockedExchange on that native backing is
observed by the next execution; user writes remain rejected. Cleanup removes
the guest mapping before freeing its native backing. No CPU, mapping adapter,
lease or immutable media code changed for this extension.

X86 build and O:/winnt/tests/S40USER/ccpu-shared-page-r6.exe pass, exit 0,
with all prior markers and CCPU_SHARED_PAGE_NATIVE_BACKING_OK. SHA-256:
1A4042A86D5D81BD3A562FE2F8E9CCC5C05AC27FCEC8B8F078760C15D451B569.
Logs: O:/winnt/logs/s40-ccpu-shared-page-r6.{stdout,stderr}.log.
This is sequential publication evidence, not concurrent clock acceptance.

Lifecycle findings constrain the pending production binding:

- nt_bop.c resolves the WOW exports before W32Init; missing exports unload
  the DLL. Registering persistent worker callbacks from DLL_PROCESS_ATTACH
  could therefore leave callbacks pointing into an unloaded provider.
- mvdm_standalone_worker.c ends guest memory and unbinds the thread before
  session disposal. A generic teardown callback cannot assume a live current
  session/lease. Stop and join a publisher before releasing backing, and
  perform guest allocator removal while its worker binding is still valid.
- The fixture is not a timer implementation or a DOSX transition test.
  Production clock publication, mode/thread lifecycle, USER shared objects
  and the 20-slot registration remain unimplemented or incomplete; none is
  accepted by this result.

### Independent publication during uninterrupted guest execution

R7 addresses the sequential-only limitation of r6. Two native-owned pages
are registered through original VdmAddVirtualMemory. The clock page is guest
read-only; a separate writable page carries a test handshake. A self-authored
CCPU program first writes the handshake, then loops reading the clock until
the expected new value appears. There is no BOP in that loop. A native thread
waits for the handshake, updates the clock with InterlockedExchange, and
returns without accessing CPU TLS, SAS or a guest-memory lease. The original
CPU observes the update and reaches the existing unsimulate opcode.

The fixture joins the publisher before removing/freeing its backing,
including failure cleanup. Its host-side handshake wait is bounded; the
outer test runner also terminates this test process on a 15-second timeout.
This is an asynchronous visibility/ownership test, not a production periodic
timer, scheduler, real USER16 application or abnormal-worker cleanup test.

X86 build and runtime pass, exit 0; all four prior markers plus
CCPU_SHARED_CLOCK_WITHOUT_BOP_PROGRESS_OK are required. Artifact:
O:/winnt/tests/S40USER/ccpu-shared-page-r7.exe, SHA-256
613ACBD62BB3B40B3EE1CC5B7DC0955B2F2A21614BA7D22EBA9A464D15ECAD98.
Logs: O:/winnt/logs/s40-ccpu-shared-page-r7.{stdout,stderr}.log.
The result permits a native-owned independent publisher design without a
new CCPU instruction, a host pointer exposed as guest linear memory, or
guest execution hooks on every clock read. Formal mode/thread lifecycle and
the actual WOW32 registration remain mandatory before product acceptance.

### FillWindow finite public binding

The selected original WU32FillWindow already dispatches through pfnOut. A new
ADAPTER-WOW-022 candidate supplies that same four-argument shape; the mirror
thunk and registration table are unchanged. Source owners and SHA-256:

- kernel/paint.c, xxxFillWindow, xxxPaintRect, UT_GetParentDCClipBox:
  15C5B8F566E6AE40D9E1F69424134508A5CA380615D22C7D2743D16D89A4D50E.
- kernel/random.c, xxxGetControlColor:
  B3ED1082041C1C600C92487B45FF0C4750A50456E5BA7EA14A3EC8EF83A1F9CD.
- client/ntstubs.c, private client entry:
  7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01.

Paths are under the pinned OpenNT windows/core/ntuser tree; original Microsoft
1985-1996 notices remain there. USER server import is prohibited. This adapter
uses public window snapshots, DC clipping/origin and color-message operations;
it is not a copied USER server or a generic Win16 UI implementation. Native
SysWOW64 USER32 export inspection found no FillWindow. Plain FillRect would
lose parent-DC clipping and control-color/brush-origin behavior.

The original color helper compares PpiCurrent against the parent's process,
despite a comment saying thread; same-process callbacks must not be suppressed
merely because they belong to another thread. Public binding uses HWNDs where
the server owns PWND, and restores brush origin after painting. Invalid public
DC/geometry operations fail instead of using an uninitialized output rectangle.

Native x86 fixture r1 passes exit 0, WOW_FILL_WINDOW_BOUNDARY_OK, checking
CS_PARENTDC pixel boundaries, callback origin, origin restoration, invalid
brush fallback, null brush-window fallback, empty clip and null DC. Artifact
O:/winnt/tests/S40USER/wow-fill-window-r1.exe, SHA-256
2D99CE393CB5ADDAA5540D30D477E3E085DAECCFA8D1D0C5626D9F5B04C2FE7B.
Logs: O:/winnt/logs/s40-fill-window-r1.{stdout,stderr}.log.
The subsequent guard for a disappearing brush-window process lookup is
included in rebuilt r2, which also passes exit 0 and the same marker:
O:/winnt/tests/S40USER/wow-fill-window-r2.exe, SHA-256
37C89E92AB1E98C2D6F6BD690770660470D08BE4ACBCC6935D76582EC4EA97B6;
logs use s40-fill-window-r2.{stdout,stderr}.log under O:/winnt/logs.
Non-MM_TEXT modes, callback destruction, cross-process behavior and
actual guest pfnOut dispatch are not covered by r1; no full equivalence or
Win16 acceptance is claimed. This is a named candidate, not a populated slot.

FillWindow follow-up r3 passes x86 compile and runtime, exit 0, including
WOW_FILL_WINDOW_NONUNIT_DC_OK and WOW_FILL_WINDOW_CALLBACK_DESTROY_OK.
The original GreGetClipBox(TRUE), ntgdi/gre/rgngdi.cxx, explicitly applies
device-to-world conversion. Reference SHA-256:
4BBD8E9B35BAE5B231AAC1205BC7018B4A71C206C0A20D4F82387E0B7DD2C158.
The added test sets a 2:1 anisotropic viewport and checks exact painted pixel
edges, preserving the original logical-clip/client-intersection order. A
second case destroys the receiving child in its color callback, returns a
still-valid independent brush, and verifies origin restoration. This does
not prove callback destruction followed by invalid-brush fallback, foreign
process callbacks, arbitrary transforms or handle reuse.

R3 artifact O:/winnt/tests/S40USER/wow-fill-window-r3.exe, SHA-256
714F32021FB615D7B6DF0B9D7004D4E1AAB3E53A2D64ADCF95CFAF085CF699A1;
logs use s40-fill-window-r3.{stdout,stderr}.log under O:/winnt/logs.

### FreeDDEData original-source reuse candidate

Selected wumsg.c calls pfnFreeDDEData in two message-conversion failure
paths, passing TRUE/TRUE. This is not FreeDDElParam or a DDEML data handle:
it owns a GlobalAlloc data block and format-dependent nested resources.
OpenNT windows/core/ntuser/client/hdata.c contains the exact function;
SHA-256 3BEC21C4BB3CAEF0798208693A89012DF6005C6A330F0F670F974A1CE821FCA2.
Its release flag, format dispatch and nested object disposal should be
retained, not replaced with a single GlobalFree or invented DDE policy.

Outgoing closure is finite: retail USERGLOBALLOCK/UNLOCK and WOWGLOBALFREE
expand to GlobalLock/GlobalUnlock/GlobalFree in client/usercli.h; remaining
calls are GlobalSize, DeleteObject, DeleteMetaFile and DeleteEnhMetaFile.
inc/ddetrack.h defines the original 8-byte WORD/WORD/DWORD DDE_DATA layout.
No DDE server, CSR, USER private object table or guest execution is needed
for this one function. The containing hdata.c package is not wholly admitted.

The original file is absent from opennt-host. Mirror-new-file prohibition
prevents placing this independently reusable original owner at its actual
path without an explicit exception. Owner was asked asynchronously about a
single registered hdata.c subset; no source was imported and no alternate
adapter implementation was invented while that decision is pending.

### Remove unbound task scheduler and success-only cleanup

Original kernel/taskman.c, SHA-256
E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E,
shows that NtUserWaitForMsgAndEvent delegates to xxxSleepTask(FALSE,event).
This deschedules the caller, considers task priorities/event counts and
send/receive locks, waits for task/virtual-interrupt events, and reacquires
the WOW execution critical section before returning. WK32WowWaitForMsgAndEvent
documents FALSE for message processing and TRUE for interrupt-only wakeup.
A native MsgWait return or SwitchToThread alone cannot implement this.

Current-source review found an unbound autonomous 16-slot task table with
per-task events, arbitrary first-peer selection and indefinite event waits.
Its InitTask ignored startup/version fields; its cleanup returned TRUE and
never removed slots or closed their events. This is not original task policy.
Search of src/tests/tools found no caller or build remap outside declarations,
the implementation itself and README claims. The USER registration remains
unpopulated. Removed 203 implementation lines and 7 header declaration lines,
including these four unbound output candidates, not any original WOW source.
README no longer claims that the removed wrappers provide scheduling or
projected-window lifecycle. This removes misleading code, not the obligation
to recover original scheduling/cleanup before S40 acceptance.

Rebuilt the entire remaining adapter into four x86 native fixtures; all
run from O:/winnt/tests/S40USER and exit 0 with their required markers:

| Artifact filename | SHA-256 |
| --- | --- |
| wow_fill_window-cleanup.exe | B53A921DD801AEB6854E14FA221C576BC8DE3057C683758708242416D74BC813 |
| wow_cursor_resource-cleanup.exe | A963272487C2D15885BFDEBB60729760E4CE747A3EE3D5DA8DABAF1CF2DC7629 |
| wow_menu_index-cleanup.exe | 87C48C8CB230FE6EE491E931E9766CD28FF262FEA375D0F4B474C8AF64C1C323 |
| wow_def_window_bits-cleanup.exe | DDA60D368B6785A5F0666826C6DC5524008C203862859A1DC8C1BBEA2190A0E1 |

Logs use O:/winnt/logs/s40-<artifact stem>.{stdout,stderr}.log. FillWindow
also passes its non-unit DC and callback-destroy cases. These tests do not
exercise a WOW task scheduler or replace full product regression/acceptance.

### Process registration is a required scheduler owner

Source audit corrects ADAPTER-WOW-012's former claim that registration is only
an optional hung-app notification. Original xxxRegisterUserHungAppHandlers
allocates zeroed WOWPROCESSINFO, references the WOWEXEC event, records the
callback/client event, attaches pwpi to its process, and links the WOW list.
InitTask and SleepTask dereference this owner. On event-reference failure
the allocation is freed. Process exit dereferences the event and unlinks/frees
pwpi (queue.c). A non-null-arguments success return establishes none of this.

Source hashes under OpenNT windows/core/ntuser/kernel:

| File | SHA-256 |
| --- | --- |
| exitwin.c | 45B92F7236FBA49D58E2C3D912126067C3843EC560C28CDD8A20167C62AAEF65 |
| queue.c | 0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8 |
| cleanup.c | DFB0FB8D81AFEB555B3631ADC5456F4CEB13142F2E954A6F2B22631C49142E5C |

The recovery contract is indivisible across these boundaries:

| Phase | Original required behavior | Standalone boundary still to implement |
| --- | --- | --- |
| Register | Allocate process WOW owner; retain event and callback; rollback on failure. | Owned public event reference and process-local state; no kernel object pointers. |
| InitTask | Copy app identity; startup placement/hotkey; publish expected version and task flags; insert priority/event record; make new task active. | Worker task/window metadata and guest CLIENTINFO publication; not just validate a name. |
| Yield/Wait | Priority/event ordering, directed scheduling, send/receive locks, exclusive execution ownership, distinct message/interrupt return. | Public waits/messages around retained original transitions; no second CCPU executor. |
| Module/task cleanup | Invalidate unloaded-selector window callbacks; defer in-use class deletion; distinguish module and task notification. | Owned window/class/callback lifetime; not blanket DestroyWindow or a success stub. |
| Thread/process exit | DestroyTask removes events/list and wakes successor; final thread cleanup releases task-owned USER objects; process releases WOW event/state. | Ordered public-resource cleanup before worker memory/thread teardown. |

NtUserWOWCleanup's original receiver returns FALSE after calling a void
cleanup routine. Its BOOL must not be interpreted as conventional success;
resource/state effects and caller use, not a fabricated TRUE, prove cleanup.
The retained original source is evidence; this audit does not authorize a
USER server import. Full lifecycle binding and actual WOW execution remain
open, including cross-thread message reentrancy and failed initialization.

The remaining unbound registration stub was removed: 17 implementation and
6 header lines. Together with the prior task cleanup, 220 implementation
lines and 13 header lines of unbound autonomous policy/stubs are removed.
No original WOW source or existing registered product route was removed.

All four x86 native fixtures rebuilt and passed again, exit 0. Artifacts
under O:/winnt/tests/S40USER; logs under O:/winnt/logs use
s40-<artifact stem>.{stdout,stderr}.log:

| Artifact filename | SHA-256 |
| --- | --- |
| wow_fill_window-lifecycle-audit.exe | 5B5AC96E91E4894D61F560B62572CCD0F36E82EEA58F8AB9BCDA99AC3752E7CE |
| wow_cursor_resource-lifecycle-audit.exe | 41358EB7CC4640C587FEDBF9FB9CB2C5DA561781E012357EC2B166B9E6F34B70 |
| wow_menu_index-lifecycle-audit.exe | 5F4AD9554ED9CC0F59C4C8CA830107120D4D0D890D14AF215FEE0A99F5F08546 |
| wow_def_window_bits-lifecycle-audit.exe | CF7BCFE9DE3F5FBBF568E727CF68350DC9C37C962DC9BAF8992519055AB3454F |

### Native window identity reuse

Original client/handles.c::GetFullUserHandle (SHA-256
29742B05EC55182515971CA67B54E3E0C4AD724FEECD482FD50EA2E398E9C2F2)
indexes the current shared handle table. A live TYPE_WINDOW returns the
current index/uniqueness pair; otherwise it returns the original WORD, not
zero. The source explicitly retains that fallback for late DDE termination.
walias.h distinguishes HWND32's sign extension from FULLHWND32's table lookup;
the latter is used by wmsg16.c, wumsg.c and walias.c, not only window creation.

The x86 private-word probe now creates and destroys only its own message-only
windows, bounded to 4096 attempts. It stores observed full values by low WORD,
and on index reuse compares stale full validity against the new window's
unique GWLP_USERDATA marker queried through its sign-extended low alias.
This is observation only: no handle mapping provider or private USER table
mutation is introduced.

R4 observed reuse at attempt 2: low=0e4e, old=010d0e4e, new=010e0e4e;
the old full handle was invalid, while the narrow alias resolved the new
window's marker. Exit 0. The existing WW/WC probes still show incompatible
private words; native alias support does not make that private ABI usable.
Artifact O:/winnt/tests/S40USER/wow-user-private-words-r4.exe, SHA-256
1E7D4FC9097454EB45D74289AFAA22B1619B0AAC716527FB76E566E3E12F4E9B.
Logs: O:/winnt/logs/s40-user-private-words-r4.txt and matching stderr.log.

Consequences for the pending binding: never freeze low16 -> first full HWND;
invalidate owned object state on destruction, publish the new uniqueness and
object when a native index is reused, and preserve the original WORD fallback
when no current window is present. Do not invent a second sequential window
ID space. A public IsWindow check validates an alias but does not return the
full identity. Cross-process objects, creation-time callbacks and actual
guest shared-table consumption remain unverified; this probe alone supplies
no complete GetFullUserHandle implementation.

### Full selected WOW32 x86 rebuild after cleanup

Generated a fresh provider-formal-r5 graph from the current build script,
selecting all 77 original WOW32 C bodies, eight support sources, three ABI/
entry carriers, original resources and four system import libraries. Fixed
the generator's obsolete src/session include root to src/ntvdm-exe/session;
no mirror source changed for that correction.

The initial Ninja process was positively identified as this build, remained
alive without children/output, and was explicitly stopped before the fallback.
Executed the same graph's 94 commands directly in order. Commands 1-88
compiled successfully; step 89 failed because rc.exe was absent from this
shell's PATH. After locating the installed 10.0.22621.0 x86 SDK tool, resumed
only steps 89-94. Resource compilation, import libraries and final DLL link
all passed. Existing upstream warnings were retained, not suppressed by
additional mirror edits. Logs under build/M0-T420/S40/provider-formal-r5:
formal-direct-build.log and formal-direct-build-resume.log.

Output wow32.dll SHA-256:
C8762994D2E9D7E586F6FBD4C05A1764C34E1B7CC2B2DEF45327C7683D5F95D6.
Parent import library build/M0-T420/S40/formal-x86-r3/ntvdm.lib SHA-256:
3F39A986E387F3553DC45D027CFDF0545121E8D39EF78329E2976C2398681DA4.
PE inspection confirms x86/14C DLL and exports W32Init, W32Dispatch,
GetCommHandle, GetCommShadowMSR and W32HungAppNotifyThread. The link map has
none of the removed five task/registration wrapper symbols. Current tracked
adapter diff versus HEAD is +143/-428 lines; wow32.c is +15/-3. These are
file-local working-tree counts, not total S40 or upstream mirror counts.

The DLL still imports UserRegisterWowHandlers from modern USER32 and its
parent symbols from ntvdm.exe. Complete linkage does not recover the missing
USER registration or shared-state lifecycle. This artifact was not copied
over O:/winnt's accepted product and was not claimed as a real Win16 pass.
The immutable USER source/binary profile gate was also rerun successfully.

### Immutable USER registration build identity

Read-only disassembly of the pinned USER.EXE locates LibMain at code-segment
offset 4810 (file offset 6df0). At file offsets 6e0c and 6e11, consecutive
16-bit stores write 0000 to [bp-12h] and 8400 to [bp-10h]. Together these
are UserInit16.dwBldInfo = 84000000. The retained map links segment offset
482c to init.c line 99, the WOWDBG branch. This is actual artifact evidence,
not an inference from host compiler flags or a guessed WINVER constant.

Provenance:

- init.c SHA-256 B5DB0214A5E50BA9972DE49B0B86718AF3E880DA69FB7E709EA57B49490ADD2D.
- k/daytona/user.map SHA-256 078BE44676C7B878B9FA97930B52063E73B001FCE9D2B6096866A1C13956B871.
- USER.EXE remains the original hash at the head of this record; neither
  mirrored nor deployed guest media were modified.

The existing gate now pins the build-info stores plus both shared-pointer
destination stores, passes 47 counted instruction-pattern checks and emits
userRegistration: buildInfo=84000000, comparedHighWord=8400,
userAbiVersion=0400, debugLayout=true. The existing actual 16-byte HANDLEENTRY
stride agrees with original user.h's DEBUG-only extra lock-record pointer.

Original wuman.c compares HIWORD of this value with pfnOut.dwBldInfo and
displays a mismatch warning when unequal. A future compatible registration
must implement the actual debug-layout consumer ABI and publish its matching
identity; setting 84000000 merely to suppress the warning is not a repair.
This does not require changing the native WOW32 optimization/debug switches,
nor does the guest debug-layout flag prove that host USER internals are usable.
No registrar or production shared-object table is installed by this gate.

### GetETM error-to-success defect in the linked GDI adapter

The linked wow_public_user_facade.c GetETM wrapper previously rejected only
ExtEscape result 0. Negative errors were reported as TRUE, followed by a
kerning query. Original wgdi.c copies its local EXTTEXTMETRIC to guest only
when GetETM succeeds; falsely returning TRUE therefore permits unsuccessful
metrics to be published. This is a host-adapter defect, not a guest defect
or evidence that the observed WRITE failure already reached this branch.

Original owners under OpenNT windows/core/ntgdi/client:

- dcquery.c::GetETM, SHA-256
  210EE355DDFC264FD0639E4F124817358E790C8BE9CA3DAB8082AA6B82ECD8E6,
  returns the NtGdiGetETM BOOL and adjusts the kerning count.
- output.c::ExtEscape, SHA-256
  177AE2B20778D8099E50565F3A2FF650F069548584B52B7652E31BC492DF8EBC,
  handles GETEXTENDEDTEXTMETRICS via that helper and returns 1 or 0.
- The [modern ExtEscape contract](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-extescape)
  also specifies negative values as errors, zero as unsupported and positive
  values as success for this operation.

Changed one adapter line from result == 0 to result <= 0. No mirror, ABI or
guest-media change. A fixture compiles the actual adapter body, controlling
only ExtEscape and GetKerningPairsA to exercise otherwise driver-dependent
errors. Before repair it exits 1: escape=-1 accepted=1 expected=0. After repair
-1, -1234 and 0 are rejected without kerning/output publication; 1 and 17
succeed with the expected kerning field. Null output does not call GDI.
This tests the wrapper's own output policy, not an assertion that every
original internal GetETM failure left all output bytes untouched.

The same fixture compiled without dependency substitution tests a real
memory DC and selected Arial font: native ExtEscape=1, wrapper=1, metrics
match after the original kerning adjustment. No printer or font installation
is involved. Both fixed x86 fixtures exit 0 with their respective
WOW_GDI_ETM_RESULT_CONTRACT_OK / WOW_GDI_ETM_NATIVE_BOUNDARY_OK markers.
Artifacts under O:/winnt/tests/S40USER:

| Filename | SHA-256 |
| --- | --- |
| wow-gdi-etm-before.exe | 462721A00E7C76ACE65EA835E464E2E5E234C8C841E9AACA43E9FADF02E37A0D |
| wow-gdi-etm-after.exe | 248517A7521350D20FEB953C8C7ED34F784844913D740413957ADCBC9AABCF31 |
| wow-gdi-etm-native.exe | E97C5D7529065A3D098F6CDC22AFCC9FE5863D8A48B508E81AD9F6460BFCEE52 |

Logs: O:/winnt/logs/s40-gdi-etm-{before,after,native}.{stdout,stderr}.log.
Recompiled the affected formal provider object and relinked successfully.
Current build/M0-T420/S40/provider-formal-r5/wow32.dll SHA-256:
111786054FF239F5595389D6395237CBB9D942E2E85AB8E7C0E9653396C49B8F.
The previous linked DLL is retained beside it as wow32-before-etm.dll.
Build log: formal-etm-rebuild.log. Neither provider was deployed over the
accepted product. Real guest GDI dispatch and full WOW32 acceptance remain open.

### Font tracking drive classification and task ownership

Compared the actual ADAPTER-WIN32-055 body with OpenNT
windows/core/ntgdi/client/font.c, SHA-256
8C74BE8F5336DE7E204897241BA375CA63C481717A781C25EF8320BD7A59282D.
The original bFileIsOnTheHardDrive excludes exactly REMOVABLE, FIXED,
CDROM and RAMDISK from tracking; non-drive paths are tracked without querying
their UNC root. The adapter instead required DRIVE_REMOTE and parsed/queryed
UNC roots. That can omit a successfully loaded font from task cleanup when
the drive query is unknown or unavailable. Restored the original predicate
inside the existing boundary, removing the unnecessary UNC parser. No mirror
or guest-media modification is involved.

Recovery ladder: the complete font.c translation unit calls private GDI
services including GdiAddFontResourceW and owns unrelated font-engine state;
it is not directly composed or newly imported here. The retained existing
same-shaped AddFontResourceTracking/RemoveFontResourceTracking/UnloadNetworkFonts
boundary uses public font operations. This repair restores the exact original
classification policy inside that admitted binding; no external-code intrusion
or newly invented cleanup rule is selected. Allocation failure after a successful
font load still returns that successful result, exactly as the original does.
It is not changed into an autonomous rollback policy.

Added tests/adapter-mvdm-host-out/wow/wow_font_tracking_fixture.c. It compiles
the actual adapter and controls only font load/remove, drive type and allocation
failure. Real path expansion, locking and list management execute. No host font
is installed or removed. Build root: build/M0-T420/S40/font-tracking-r1,
MSVC Win32/x86 /MT /W4, public user32/gdi32 libraries. Before repair it exits 1
on DRIVE_UNKNOWN classification. After repair it exits 0 with
WOW_FONT_ORIGINAL_DRIVE_CLASSIFICATION_OK and
WOW_FONT_TASK_REFERENCE_AND_FAILURE_CONTRACT_OK. Coverage includes all seven
drive types, UNC without drive queries, case-insensitive repeated load,
distinct owners, failed explicit removal retaining its count, successful
removal decrementing it, repeated unload, one removal per outstanding load,
failed load, allocation failure and excluded local fonts.

Runtime artifacts under O:/winnt/tests/S40USER:

| Filename | SHA-256 |
| --- | --- |
| font-tracking-before.exe | 39D851275C3CEB2DCE607FF2798E9B76DFBEA2A2FBE4E61067244A180B0E8CDB |
| font-tracking-after.exe | 183E95428B1FA864499F407ECFF68FA48164DAE6D20E4471E75CCDB9FA55F8C4 |

Logs: O:/winnt/logs/s40-font-{before,after}.{stdout,stderr}.log.
Recompiled the formal provider object and relinked wow32.dll successfully;
provider-formal-r5/formal-font-rebuild.log records the build. New DLL SHA-256:
4282FAD5111E1A6E3AA9AF973EC3842EF247087E02AE99A99A9B2EFA5E3D59AC.
It is a research build, not deployed or accepted as a real guest pass.

The remaining path contract is explicitly incomplete: original bMakePathNameW
searches the Windows fonts directory before the default search path and uses
Unicode storage; the current adapter uses ANSI GetFullPathNameA and an ANSI
list. This fixture does not prove those different resolvers equivalent, network
font installation, or real Win16 task exit. Those remain S40 obligations.

### Font path recovery follow-up

The preceding path-search gap is now repaired at the existing font boundary.
The source audit follows font.c::bInitSystemAndFontsDirectoriesW,
bMakePathNameW (the tracking callers pass pfl == NULL) and nlsconv.h::vToUnicodeN.
The caller's ANSI name is converted with CP_ACP, loading/removal use the W
entry points, and stored identities and teardown paths remain Unicode.
Relative names search GetWindowsDirectoryW plus fonts first, then the default
SearchPathW route; absolute names skip the first search. A path-resolution
failure still preserves the preceding successful font operation's return value.
No registry-driven font-sweeper migration is selected: the original guards
that behavior with a non-NULL pfl, absent in both tracking callers.

This remains a same-shaped adapter, not a claim that original font.c was
compiled. The prior recovery-ladder disposition applies. Public
MultiByteToWideChar(CP_ACP) binds the original RtlMultiByteToUnicodeN operation
for valid terminated names. The bounded facade refuses failed/overlong
conversion and overlong resolved output, rather than consuming an unterminated
fixed buffer. It computes the fonts directory in a bounded local buffer instead
of recreating GDI's process-wide system/fonts allocation cache and semLocal.
These finite mechanics are explicit retained differences, not recovered private
GDI engine behavior. No mirror file or guest binary changed.

The extended real-body fixture verifies fonts-first success, default-path
fallback, failed search without registration, and a resolved Unicode filename
containing U+4E2D surviving task teardown unchanged. Its public search/drive/font
dependencies are controlled; ACP conversion, list, lock and cleanup are real.
An additional WOW_FONT_NATIVE_SEARCH build reads an existing TTF filename from
the host fonts directory, verifies relative/absolute resolution against public
SearchPathW, and rejects a nonexistent name. This native variant never invokes
font load/remove; no installed font, registry or guest media is changed.
Both x86 /MT /W4 variants exit 0. Runtime identities under
O:/winnt/tests/S40USER:

| Filename | SHA-256 |
| --- | --- |
| font-tracking-unicode-final.exe | A36D1D173CF63F0A59AEA5A2A15155345D3BB06477B3908BFD683178F1E17F33 |
| font-tracking-native.exe | C3D330DB6E0C3C83D915D292B1FF9B7407AFAC591E619E70463F0EBECFE6F6C0 |

Logs: O:/winnt/logs/s40-font-{unicode-final,native}.{stdout,stderr}.log.
Markers include WOW_FONT_SEARCH_ORDER_AND_UNICODE_UNLOAD_OK and
WOW_FONT_NATIVE_READ_ONLY_SEARCH_OK, in addition to the prior ownership markers.
Build input/output root remains build/M0-T420/S40/font-tracking-r1; each variant
has a distinct executable filename. Recompiled the affected provider object and
relinked the full research DLL; formal-font-unicode-rebuild.log records success.
Current provider-formal-r5/wow32.dll SHA-256:
1C004435855A4C1084155135B1C0F255AB5E469691BD51EE237ADDDC5E1AE0D2.
No product deployment or real network-font/Win16 task acceptance is claimed.
USER registration, shared-object publication, scheduler and actual guest
task/callback cleanup remain S40 completion obligations.

### USER creation-state publication before callbacks

The registration review confirms that filling twenty output pointers alone is
insufficient. WU32CreateWindowEx builds a WW record and passes it to the private
_CreateWindowEx slot; FindPWW/FindPWC and message dispatch subsequently read
GWL_WOWWORDS/GCL_WOWWORDS independently of the registration table. Original
kernel/createw.c copies lpWOW into pwnd->adwWOW at lines 232-233, before
HCBT_CREATEWND at 394, pre-create WM_GETMINMAXINFO at 796, and WM_NCCREATE at
841. Source SHA-256:
DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7.
Therefore a binding that adds metadata only after CreateWindowEx returns,
or only on WM_NCCREATE, cannot preserve the reached callback contract.

The kernel translation unit is not imported: it owns private PWND/desktop
allocation and locked server callbacks. Added a native, test-only public-
boundary experiment, wow_window_creation_order_fixture.c. A current-thread
WH_CBT hook copies fixture metadata to a private property at HCBT_CREATEWND;
the window procedure verifies its presence on every message, preserves the
caller's CREATESTRUCT.lpCreateParams, and leaves GWLP_USERDATA available to
the application. Nested creation has a separate pending frame and record.
The record survives through DefWindowProc(WM_NCDESTROY) before release.
Only invisible windows and a hook on the test's own thread are used; no global
hook, remote process, guest media or production registrar is changed.

Actual Win32/x86 /MT /W4 /WX run: three records created and destroyed, zero
live records, three WM_GETMINMAXINFO deliveries, zero missing-state/identity
failures. Cases are ordinary creation, reentrant nested creation from
WM_NCCREATE, and deliberate FALSE return from WM_NCCREATE. Marker:
WOW_PUBLIC_PRECREATE_NESTED_AND_FAILURE_ORDER_OK. Executable
O:/winnt/tests/S40USER/wow-window-order-r2.exe SHA-256:
0633458BB9280D42E43AD2C53A3B5BB5E900CCC6A2F994E794D417E1B2AF5804.
Build root: build/M0-T420/S40/window-order-r1. Logs:
O:/winnt/logs/s40-window-order-r2.{stdout,stderr}.log.

This proves a finite public mechanism for these creation cases, not original
WOW registration or full WND semantics. The experiment is not selected in the
product. Before production binding, cover an existing hook vetoing creation
(WM_NCDESTROY cannot be assumed), property/allocation failure, system controls
and dialog creation, native/guest class identity and all original private
word consumers. It must coexist with the source-shaped guest object graph and
task cleanup; a property alone is not that graph. The next implementation must
not reuse modern private USER offsets or steal application window-extra bytes.

### Creation veto: observed destruction, not assumed absence

Extended the same native experiment with another current-thread CBT hook in
both installation orders, plus injected allocation and property-attachment
failure. This changed the design decision: on this host a CBT-vetoed window
still receives WM_NCDESTROY. The tentative immediate release after
CallNextHookEx returns a veto removes previously attached state too early.
Original createw.c also sends its failed-create route through MemError and
xxxFreeWindow; there is no source justification for freeing the WOW record
at the hook return while later destruction can still consume it.

The corrected experiment leaves an attached record through destruction and
tracks residual ownership in the creation attempt. Only after CreateWindowEx
returns NULL can a still-owned record use the fallback release. All fixture
frames outlive their windows; this is not a production pointer-lifetime model.
No production registrar or window provider is introduced. A destruction message
for a window rejected before any attachment is valid, not a missing-state
error. Windows without an attached record must never free another attempt's
record.

Both variants build Win32/x86 /MT /W4 /WX. Real results:

- Early rollback counterexample exits 3: four attached, three destruction-time
  releases, one premature rollback, four unbound destruction messages.
- Corrected run exits 0: four attached and four destruction-time releases,
  five allocations/five frees, zero live records, two CBT vetoes, three valid
  unbound destructions (upstream veto, allocation refusal, property refusal).
  The post-return fallback is present but not reached on this host; it is not
  counted as a tested missing-destruction path.

Markers: WOW_PUBLIC_PRECREATE_NESTED_AND_FAILURE_ORDER_OK and
WOW_PUBLIC_PRECREATE_VETO_AND_RESOURCE_FAILURE_OK. Artifacts under
O:/winnt/tests/S40USER:

| Filename | SHA-256 |
| --- | --- |
| wow-window-early-rollback.exe | 4A5517AA27845D80A8B1C3AEBA59D793B22438424F0E05C928790BD9280DB3D8 |
| wow-window-veto-r2.exe | 9AF4A568DB5779495031EF0A14091A44BC2AD58569B29DD6D637B45546D6C63C |

Logs: O:/winnt/logs/s40-wow-window-{early-rollback,veto-r2}.{stdout,stderr}.log.
Build root remains build/M0-T420/S40/window-order-r1 with distinct executables.
The earlier wow-window-veto and wow-window-veto-negative runs both failed their
initial assumptions and remain diagnostic evidence, not passing results.
System controls/dialogs, real WOW dispatch, class/object identity and the guest
shared graph remain unimplemented. This evidence narrows the finite binding
design; it does not close S40 or permit publishing an empty callback table.

### Built-in dialog/control lifecycle and initialization timing

Original client/dlgbegin.c::InternalCreateDialog calls WOWDlgInit after setting
up the dialog state but before WM_SETFONT and before creating its controls.
Source SHA-256:
C3DF77A2987E97DF8872C3DAB8F6E7C10A08850C5EBCB439C0EC1DC88B0CDC44.
The direct consumer is WU32DialogBoxParam; W32DialogFunc immediately reads
GWL_WOWWORDS and requires vpfnDlgProc. Consequently, replacing the private
dialog slot with a public CreateDialog call without the initialization binding
does not preserve the original contract. The existing modal public facade is
also a consumer of this same missing initialization responsibility.

Added wow_dialog_lifecycle_fixture.c, a standalone native boundary experiment.
It creates an invisible standard #32770 dialog with a built-in Edit control,
uses own-thread CBT and WH_CALLWNDPROCRET hooks, and does not replace either
native window procedure. A marker property is available during every dialog
callback and is removed after WM_NCDESTROY returns. The Edit control receives
and returns S40 text normally. No WOW table, guest or production hook changed.

Actual runs discovered one additional IME window created automatically by
USER. The first run failed its two-window assumption (three attached/three
released, no missing state). The corrected gate identifies the required one
dialog and one Edit separately, records auxiliary classes and still requires
every observed attachment to be released. This is not permission to label
every future native auxiliary window as a guest/WOW window.

Both template profiles were then tested:

- DS_SETFONT: dialog creation 1, auxiliary IME 2, first dialog callback
  WM_SETFONT 3, Edit creation 4, WM_INITDIALOG 5.
- No DS_SETFONT: dialog creation 1, IME 2, Edit creation 3, first dialog
  callback WM_INITDIALOG 4. There is no earlier DLGPROC callback to use for
  initialization before child creation.

Both runs exit 0 with WOW_NATIVE_DIALOG_CONTROL_POST_DESTROY_OK; each has three
attachments/three releases, zero missing state, and valid original init lParam.
The no-font result disproves a proposed generic lazy initialization on the first
DLGPROC callback: it would be too late for original pre-control semantics.
Use an earlier dialog-creation binding with exact scope and original callback
ordering; do not rely on WM_INITDIALOG or DS_SETFONT being present. The observed
post-window-procedure hook can preserve state through built-in destruction,
but is not yet production ownership or arbitrary hook-chain verification.

Win32/x86 /MT /W4 /WX artifacts under O:/winnt/tests/S40USER:

| Filename | SHA-256 |
| --- | --- |
| wow-dialog-lifecycle-r2.exe | 1A5A022144B90EFDD618841D567805FB7C2D55249E4C510FFFED196C1A916189 |
| wow-dialog-no-font.exe | 673BEB6D303326D85C597AFB26D43092B7595BF3ABC0F7746904DB9D6915DDAC |

Build root: build/M0-T420/S40/dialog-lifecycle-r1. Logs:
O:/winnt/logs/s40-dialog-{lifecycle-r2,no-font}.{stdout,stderr}.log.
The no-font variant defines WOW_DIALOG_NO_FONT; the first variant uses a font
template. Real Win16, class/WW publication, callback initialization and production
cleanup integration remain open. Native fixture success is not guest acceptance.

### Original W32InitDlg compiled and exercised unchanged

Reviewed wcall32.c::W32InitDlg and its complete reached body: it returns 0 for
a null DLGDATA or missing GWL_WOWWORDS; otherwise it writes exactly
GWL_WOWiClassAndflState = MAKECLASSANDSTATE(WOWCLASS_DIALOG, WWSTATE_ICLASSISSET)
and GWL_WOWvpfnDlgProc from DLGDATA, then returns dwUserInitParam. It does not
execute a guest callback, allocate an alias or depend on initialized native
dialog-private fields. Therefore this policy must stay in the original function;
a new dialog initializer is unnecessary. The missing dependency is the
source-shaped private window-word getter/setter plus the original call ordering.

Compiled the entire existing src/mvdm/wow32/wcall32.c unchanged using the formal
provider command from ninja -t commands obj/wcall32.obj, with /Gy and only
/DGetWindowLongA=fixture_GetWindowLongA and
/DSetWindowLongA=fixture_SetWindowLongA, writing the object under
build/M0-T420/S40/original-dialog-init-r1. This is neither a copied function nor
a rewritten stand-in. Added wow_original_dialog_init_fixture.c and linked /OPT:REF
with import aliases to its two controlled field accessors. The link map retains
_W32InitDlg@8 from wcall32.obj and discards W32EditNextWord, W32LocalAlloc and
WOWRtlGetExpWinVer.

The initial link failed on fourteen references in unselected functions: MSVC
resolves them before dead COMDAT removal. Test-only linker aliases for
GetPModeVDMPointerAssert, LocalAlloc16, LocalReAlloc16, LocalLock16,
LocalUnlock16, LocalSize16, LocalFree16, CallBack16, malloc16, free16,
ParamMapUpdateNode, WOWGlobalAllocLock16, WOWGlobalUnlockFree16 and the imported
RtlImageNtHeader terminate with exit 90 if unexpectedly reached. They never
return fabricated success and are not runtime providers. The original-unit
compile retains its two pre-existing implicit-declaration warnings for the
unselected global-allocation functions; the fixture compiles /MT /W4 /WX.

The actual original body passes: one getter, two setters in original order,
exact class/state and procedure values, six untouched DWORDs, unchanged returned
user parameter, null argument without API access, missing state without writes.
Exit 0 and WOW_ORIGINAL_W32INITDLG_TWO_FIELD_CONTRACT_OK. Artifact:
O:/winnt/tests/S40USER/wow-original-dialog-init.exe SHA-256
8983A0B476D307B6A5D5D0DFEC70F686E7E35E8EB38BDB572C68A9363AB5F6A8.
Logs: O:/winnt/logs/s40-original-dialog-init.{stdout,stderr}.log. The build root
also retains compile.log and wow-original-dialog-init.map. No production or
mirror edit was needed for this original-body test. Real dialog entry binding,
window-word lifetime and guest callback execution remain unaccepted.

### Original dialog initializer joined to the native creation sequence

Extended the existing native dialog fixture with WOW_ORIGINAL_DIALOG_INIT.
It links the unchanged original wcall32.obj from the preceding original-body
test (SHA-256 53B80EED7084E9B56AFD6689FC7ECE135E9CA2AD8EF52BE68E32AB158847C324).
The underlying wcall32.c is byte-identical to OpenNT, SHA-256
0F72AE39E691F3EF672DF4F1AEB3EBB6443E87F091FB6322EF1C13604AB3D571.
The same two private import bindings now access the actual dialog's property-
backed eight-DWORD test record rather than accepting a sentinel HWND. The
original initializer runs after the standard dialog's WM_CREATE returns
successfully, via the own-thread return hook. The native dialog procedure
verifies the original fields on every callback and substitutes the original
returned user parameter for the native WM_INITDIALOG carrier.

Both x86 /MT /W4 /WX profiles pass with the actual original body:

- Font template: original initializer sequence 3, WM_SETFONT 4, Edit create 5,
  WM_INITDIALOG 6.
- No font: original initializer sequence 3, Edit create 4, WM_INITDIALOG 5.
- Exactly one original invocation and two original writes in each; no missing
  state and all three native property attachments released after destruction.

This establishes a tested earlier binding point for the standard #32770 route,
including the no-font case where lazy DLGPROC initialization was too late.
It does not claim every dialog class has that ordering. The fixture has one
explicit test dialog context, not a product-global registration or task owner;
nested/modal/custom-class scope must retain per-call/per-thread ownership when
composed. No guest callback is executed, and the nonselected original function
dependencies retain the prior hard-failure trap aliases. No product import,
mirror source, registry or guest binary was changed.

Artifacts under O:/winnt/tests/S40USER:

| Filename | SHA-256 |
| --- | --- |
| wow-original-native-font.exe | 8802502278117328C939C5CF112050221B80485E79FE47B1688A59C006800F54 |
| wow-original-native-no-font.exe | 104490E351834A1E14B3732993512B4214CA2301D112451E1B4190CDAE96F4A9 |

Logs: O:/winnt/logs/s40-wow-original-native-{font,no-font}.{stdout,stderr}.log.
Build root: build/M0-T420/S40/dialog-lifecycle-r1; the no-font variant additionally
defines WOW_DIALOG_NO_FONT. Both exit 0 with
WOW_ORIGINAL_INIT_BEFORE_NATIVE_DIALOG_CALLBACKS_OK and
WOW_NATIVE_DIALOG_CONTROL_POST_DESTROY_OK. Full WOW32 registration, general
window/class bindings and real Win16 acceptance remain S40 work, not test passes.

### Private window/class field owner audit

The next binding must preserve original ownership, not extrapolate the two-field
dialog fixture into a general window model. Read the complete relevant original
switches and current WOW32 callers:

| Original file under windows/core/ntuser | SHA-256 |
| --- | --- |
| kernel/getset.c | F0D22E4285E8B33B873854913A11587C67D78E50E1F5DB28EC89EF8B6FF06D3D |
| kernel/class.c | 82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A |
| rtl/getset.c | 4A2CB2067F5DD776676D7F808F40F6468082DB018E16BCF9A77DAF00BDDB34C9 |
| client/classc.c | 1DA5A77B48A073A0C6001D7F56B530C941C90B55C8E995FD6C10914AC8E8D937 |

rtl/getset.c returns the WW pointer for GWL_WOWWORDS and individual first-three
DWORDs for GWL_WOWDWORD1/2/3. kernel/getset.c::xxxSetWindowData writes those
three fields directly, without changing WNDPROC, invoking callbacks or modifying
the shared class. Original WOW32 wuwind/wmdisp32/wumsg retain the procedure and
class-selection policy; moving it into a private-word setter would duplicate it.
The private setter cases do not assign dwOld before the common return, unlike
ordinary documented fields. All reviewed SETWL/SETWC calls consume their side
effects and ignore that return. Do not claim the fixture's previous-value return
is a proven original private-setter result contract or reproduce an undefined
return as a required product feature.

Class ownership differs: kernel/class.c::xxxSetClassData sets pclsBase and each
pclsClone for both GCL_WOWDWORD1/2. client/classc.c exposes the class-owned WW
pair via GCL_WOWWORDS. Therefore a per-window independent WC copy is incorrect.
The source-shaped class record must be keyed by original class/module scope,
shared by its windows, and disposed according to class lifetime; full HWND and
WORD alias identity are separate from class identity. No private class registry
has yet been implemented or accepted.

WW is eight DWORDs, not only the first three: its tail aliases native USER state,
state2, ExStyle, style and hModule. Concrete active callers include wcntl32,
wmsgcb and wmsglb (owner-draw/HASSTRINGS policy), wmdisp32 and wmsg16 (MDI
extended style and creation thunking), wmdisp32 (guest instance), and wmsgem
(Edit instance). A zeroed tail can choose the wrong message representation even
when dialog initialization succeeds. Current direct PWW field searches found
no named dwUserSrvState/2 read outside their declarations, but that is not proof
the guest PMODE32 graph or indirect macros do not consume the equivalent flags.
The guest projection remains a separately verified obligation.

Required implementation boundary is consequently one owned window record with
the exact WW prefix and correctly maintained reached native attributes; one
shared source-shaped WC per registered class; private field accessors bound for
all selected callers, while normal public indices retain native behavior; and
creation/subclass/destruction synchronization. Keep original WOW32 policy and
the previously tested W32InitDlg intact. This audit rejects a simplistic
per-window two-field shim; it does not introduce a substitute USER engine or
claim that general window/class bindings are already implemented.

### Window callback calling-convention repair

Class-registration audit traced the tagged WNDPROC route through original
USER client.c::WowWndProc and usercli.h dispatch macros into
pfnWowWndProcEx. The declared PFNWOWWNDPROCEX is WINAPI with six arguments;
the selected wmdisp32.c::W32Win16WndProcEx definition is cdecl under the formal
/Gd build. Its formal object exports _W32Win16WndProcEx (no @24), while
wow32.c previously forced it into the table with a cast. Thus a reached table
call has mismatched stack cleanup independently of pending USER registration.

Original WMDISP32 source remains unchanged (SHA-256
4A9004C1F57AF28280D0176AFD72CAE76A7FBAE90FFB758426C287435E5912D4).
Selected wowuserp.h SHA-256:
10CE21B98498EC4170806AF9909854429C99929083B4730D370E5A7CC4E479D8.
Recovery ladder: keep the original message body; bridge only its unavailable
calling-convention match in the existing worker-local callconv carrier. No
server import or new message implementation is needed. ADAPTER-WOW-023 adds
one six-argument WINAPI forwarding function, and MVDM-HOST-DIV-292 changes
one table assignment plus its marker. Original parameters, WW pointer and
return bits are preserved. The other existing callback bridges remain intact.

Added wow_window_callback_abi_fixture.c, linked to the actual callconv object.
Only the original message body is replaced with a six-argument checker; any
unexpected call to other original callback bodies terminates the test with 90.
A controlled negative variant performs the former cast. The /Od /Oy- x86
fixture records ESP and restores it after measuring the negative call:

- Direct cast exits 3: stack_delta=-24, calls=1, errors=0, result=87654321.
- Actual bridge exits 0: stack_delta=0, same arguments/result, marker
  WOW_WINDOW_CALLBACK_X86_ABI_OK.

Artifacts under O:/winnt/tests/S40USER:

| Filename | SHA-256 |
| --- | --- |
| callback-direct.exe | F686017FD4CBFEBE7B013F346FC8C90D51D9C19FC9EF7CDE661A2980B4F0EB3E |
| callback-bridge.exe | 94CACABAB60A89CF7D6C0B7F635DB3A81627A78FA097DF0962FD6F6FC11D1BD5 |

Build root: build/M0-T420/S40/window-callback-abi-r1. Logs:
O:/winnt/logs/s40-callback-{direct,bridge}.{stdout,stderr}.log.
Recompiled the two affected formal objects and relinked WOW32 successfully;
formal-window-callback-rebuild.log records the build. The actual bridge object
defines _mvdm_wow_user_window_proc@24 and calls _W32Win16WndProcEx.
Current research DLL SHA-256:
48BCD043F7F40EC5A9A337BB2C6DFEA1FBE1A17E870135E31BC024B76209A020.
It has not replaced the accepted runtime package. This is a demonstrated host
ABI defect, not proof that the current WRITE trace reached this callback or
that correcting it alone restores WOW32. Full registration/guest acceptance
remains open.

### DLGPROC ABI sibling repair

The follow-on audit deliberately extended beyond PFNWOWHANDLERSIN. Original
W32DialogFunc in wudlg.c/wudlg.h had no WINAPI; the formal x86 object defined
_W32DialogFunc. WU32DialogBoxParam cast it to DLGPROC for modal and modeless
creation, while wumsg.c and wuwind.c stored it in DWL_DLGPROC and compared its
address. These are four-argument WINAPI callback consumers, not cdecl callers.

MVDM-HOST-DIV-293 adds WINAPI to the existing declaration/definition, plus one
marker. No message-body change, wrapper, alternate function address or new
window policy. This is smaller than replacing every callback and identity
comparison with a separate shim and preserves their common function identity.
The original source's callback meaning is retained under the selected /Gd x86
compiler; the convention correction is the required composition boundary.

Recompiled wudlg, wumsg and wuwind, then relinked the formal research provider.
dumpbin confirms _W32DialogFunc@16 in the defining object and both external
consumers; its disassembly contains the callee-cleanup `C2 10 00` / `ret 10h`
epilogue. Extended verify-wow-user-callback-abi.ps1 to check these three objects
in addition to the 21 input slots; both checks pass. This is compilation/object
ABI evidence, not a runtime invocation of the real guest dialog body.

Build log: build/M0-T420/S40/provider-formal-r5/formal-dialog-abi-rebuild.log.
Current research WOW32.DLL SHA-256:
F5C69A77EB22BC25CD3F8DAEC7824CFB4A2B91E829CFA0FC2012390668256212.
No accepted runtime package was overwritten. Real dialog/WOW task acceptance
remains open; this defect is not claimed as the current WRITE failure's cause.

### Complete USER input-table ABI gate

Added tests/observation/verify-wow-user-callback-abi.ps1 and ran it against
provider-formal-r5 after the window-callback repair. It reads the original
PFNWOWHANDLERSIN declaration, checks exactly 21 unique direct assignments in
W32Init, requires each declared type to be WINAPI, then inspects the formal
caller and defining COFF objects with dumpbin /symbols. Every slot must have
one matching decorated definition and the actual caller's undefined reference
with the expected x86 argument-byte suffix. A force-cast, missing/duplicate slot,
wrong suffix or absent definition fails the gate. It records source/header and
object hashes so stale build evidence is visible rather than silently described
as current-source verification.

Result WOW_USER_INPUT_CALLBACK_ABI_21_OK: all 21 pass. Seventeen bind their
original decorated WINAPI bodies; four bind the existing ABI bridge family
(empty clipboard, window procedure, fake dialog class and clipboard handle).
The global-free hook is outside this 21-slot input table and is not counted
as a table pass. The repaired window callback is the only new calling-convention
repair identified in this table sweep. The prior real stack-delta fixture
remains the positive/negative execution evidence for that specific repair.

Command: tests/observation/verify-wow-user-callback-abi.ps1 -BuildRoot
build/M0-T420/S40/provider-formal-r5. Full read-only result and object identities:
O:/winnt/logs/s40-user-callback-abi.json. This gate does not prove argument
meaning, callback body semantics/reachability, the twenty-slot output table,
or any real Win16 workload; those remain separate S40 obligations.

### Package calling-convention root-cause follow-up

The sibling callback sweep found three further native callbacks compiled as
cdecl in provider-formal-r5: W32EnumFontFunc (wgfont.c), W32GrayStringProc
(wuser.c), and W32EnumWindowFunc (wuwind.c). Their callers cast them to
FONTENUMPROC, GRAYSTRINGPROC and WNDENUMPROC. This is not evidence of a guest
defect or of a particular WRITE crash; it is an object-level ABI mismatch.

Before adding further mirror declarations or forwarding functions, inspected
the original build owner. OpenNT-4.5 nt/public/oak/bin/i386mk.inc lines 144-154
select /Gz and STD_CALL unless 386_STDCALL explicitly selects cdecl (0) or
fastcall (2). Its SHA-256 is
DFB1B7E48348AADE19DDD3A98F6FADF37675ABF53DC7B8BB0A3FF77C9848AC95.
The selected WOW32 sources/makefile do not override 386_STDCALL. The original
wow32.prf independently records W32Win16WndProcEx@24, W32EmptyClipboard@0
and W32EnumFontFunc@16. The current provider generator instead forces /Gd.
Consequently the prior description of original bodies as necessarily cdecl
was incomplete: it described our selected compilation, not the original
package's default build ABI. DIV-292/293 and existing callback bridges must
be reconsidered after restoring the package build convention, not propagated
indefinitely to additional callbacks.

An isolated whole-provider experiment uses
build/M0-T420/S40/provider-stdcall-r1, the same 77 original bodies, eight
support units and three generated units, with /Gd changed to /Gz /DSTD_CALL
only in its generated graph. The accepted product graph/runtime is unchanged.
An initial tool PATH failure produced no objects; a subsequent Ninja process
stalled before spawning a compiler and was identity-checked and terminated.
The same 94 graph commands are being executed sequentially with a full log
in sequential.log. The already produced wgfont.obj now defines
_W32EnumFontFunc@16 without any wgfont source edit. Full compile/link and
cross-DLL ABI validation are still required; this experiment is not accepted
as a package or guest pass.

The experiment completed: all 88 C units compiled, all five resource/import
library commands succeeded, and only the final link failed (94 commands,
one failed command), with 58 unique unresolved symbols. This is positive
source-composition evidence and a failed DLL build, not a deployable artifact.
Without edits to these callback bodies, dumpbin now proves
W32EnumFontFunc@16, W32GrayStringProc@12, W32EnumWindowFunc@8,
W32Win16WndProcEx@24 and W32EmptyClipboard@0. Object SHA-256 values:

- wgfont.obj: EA09C192020911FCCA1DB427CA3400BCE3EFCA9D95DE730C6C15952A1A93A328.
- wuser.obj: 89059B24437DAE2B04C94BB3EA8827827E1B18A171F0CDB20A47775797E5F8A5.
- wuwind.obj: 32BD7998F5430BC8F4DED25848E9395521DB9ADE95E5237E51ED087E0FC9A53A.

The unresolved set includes worker CPU get/set functions, host simulation,
idle/timer, DEM/VDD, debugger, mapping and lease exports; two system-private
imports (UserRegisterWowHandlers@8 and GdiQueryTable@0); WOWSysErrorBox@20;
and the generated DllMain bridge's explicit cdecl W32DllInitialize reference,
whose new definition is W32DllInitialize@12. The exact set is retained in
sequential.log. These need declaration/definition/import-library comparison:
a COFF name alias cannot turn a cdecl implementation into stdcall and must
not conceal a real stack-cleanup mismatch. Do not change the whole worker's
default calling convention to solve this DLL-local boundary.

Next action is to restore the original WOW32 package compilation convention
with explicit, verified declarations for the finite modern/worker boundary,
then remove bridges and local mirror annotations made redundant by that
restoration. The existing /Gd research DLL and earlier ABI fixture results
remain historical evidence of that build profile, not proof that the added
bridges are permanently necessary. No runtime package was changed.

### Restore package flags and verify the lease ABI edge

The provider generator now uses /Gz /DSTD_CALL and declares its generated
DllMain forwarding target W32DllInitialize as stdcall. Two missing system
COFF aliases bind UserRegisterWowHandlers@8 and GdiQueryTable@0 to their raw
system import-library spellings, not to cdecl worker implementations.
Comparison with formal-x86-r3/ntvdm.exe.map confirms the remaining CPU,
idle/timer, DEM/VDD, debugger, memory and error-box bodies exist as cdecl.
call_ica_hw_interrupt is the existing export alias for ica_hw_interrupt.

ADAPTER-WOW-024 explicitly preserves cdecl on the worker lease acquire/release
and effective-address declarations, including the frame adapter's private
fixed-width effective-address declaration. Merely predeclaring an explicit
cdecl prototype before an unchanged /Gz declaration was tested and rejected
by MSVC with C2373; that is not an alternative to fixing the boundary.
Recompiled the affected lease and generated objects. The DLL link now has
52 unresolved symbols instead of 58 (provider-stdcall-r1/boundary-final.log).
It still fails and must not be deployed. Remaining original worker declarations
need verified ABI binding, not stack-incompatible COFF aliases. Removal of the
now-redundant callback bridges and DIV-292/293 awaits complete composition.

Added wow_worker_lease_abi_fixture.c: actual /Gz frame-binding object linked
to cdecl worker stand-ins. It checks selector/offset, address/span/access,
success, release/commit and cleared view, rejected selector and failed acquire.
Native exit 0: stack_delta=0, address=3, acquire=2, release=1, errors=0;
marker WOW_STDCALL_TO_CDECL_WORKER_LEASE_ABI_OK. This is mock ABI evidence,
not real CCPU/session mapping or a guest callback pass. Fixture uses
/MT /Gz /W4 /WX /Od /Oy- and explicit cdecl main/worker exports. An initial
command incorrectly applied /TC to the object input and omitted cdecl on main;
both were corrected before the successful build.

Runtime: O:/winnt/tests/S40USER/worker-lease-abi-fixture.exe, SHA-256
C7A16EA8C607503BB81371A0371E99C44B6DADBB5B8499CFB890C7F7D9AFD000.
Logs: O:/winnt/logs/s40-worker-lease-abi.{stdout,stderr}.log.
Accepted runtime and guest media remain unchanged. S40 is still incomplete.

### Original worker declaration boundary

The remaining 52 names were compared to formal-x86-r3/ntvdm.exe.map and
generated/ntvdm-wow32-provider.def, not assumed absent because /Gz could not
link them. Register get/set, simulation, CPU thread entry, timer/idle, DEM/VDD,
debugger, memory mapping and WOWSysErrorBox already have cdecl implementations.
The interrupt export is the existing call_ica_hw_interrupt=ica_hw_interrupt
alias. No new provider implementation is warranted for this set.

MVDM-HOST-DIV-294 adds explicit cdecl only to the corresponding declarations
in inc/{softpc,dbgexp,demexp,memapi,wowmmcb}.h and wow32/wkman.h. The register
header has repeated conditional declarations; the same function must retain
one convention in each declaration. Parameters, return types and all bodies
are unchanged. Direct original /Gz composition was attempted and failed at
this existing /Gd worker boundary; qualifying the declaration is smaller than
introducing 52 forwarding bodies or changing the worker's whole ABI.

The non-MVDM nt_vdd.h carrier is kept byte-identical to original OpenNT:
SHA-256 7E89FD4420381A54BC89E9807E54CCFE138A0767996AB7C74712E5E96AEB6609.
A WOW-local include shim qualifies its three selected declarations only,
includes that original carrier, then undefines the temporary macros. It does
not rewrite call sites, rename symbols, supply code, or enter a mirror root.
The temporary direct ABI-carrier edit was reverted before composition.

Full 94-command recompilation is recorded in provider-stdcall-r1/
cdecl-boundary-full.log. Its conclusion and subsequent callback-bridge removal
must be checked before claiming this recovery is complete. Mirror edits keep
their existing LF index format; no full-file newline changes are intended.

### Completed stdcall composition and callback bridge removal

The full 94-command rebuild compiled all units and narrowed the link failure
to one duplicated local demClientErrorEx declaration in wdos.c. DIV-294 replaces
that declaration with the existing demexp.h include. Recompiling wdos and
relinking succeeded (wdos-boundary.log). No missing worker implementation was
added, and no stdcall name was redirected to a cdecl function to hide mismatch.

Then restored the original WOW32 callback assignments (including the original
PFNWOWWNDPROCEX return-type cast), RegisterWowBaseHandlers target and original
W32DialogFunc declaration/definition. Removed the entire five-function
wow_user_callback_callconv.c/header and its build entry, plus the obsolete
mock fixture specific to that removed bridge. The production files remove
38 lines relative to HEAD (27 C + 11 header); the additional in-progress
six-argument bridge is also gone. DIV-292/293 and ADAPTER-WOW-023 are retired,
not reused. The dialog C/header now have no diff relative to HEAD. The worker
ABI declaration changes and 14-line VDD include shim are necessary retained
binding, not a claim of zero total added lines.

Recompiled wow32, wudlg, wumsg and wuwind and relinked without the bridge
object: success, original-callbacks.log. Final research WOW32.DLL SHA-256:
5835826F062B4618BAB2C5A39E15ECA9BCB71E4037D5662F6FA5F71E518BD732.
The current graph and link map contain neither the bridge object nor any
mvdm_wow_user_* bridge symbol. The old object may remain as unused research
build residue; it is not a link input.

The callback ABI gate now verifies all 21 original input targets, the dialog
definition/consumers, and W32EnumFontFunc@16, W32GrayStringProc@12,
W32EnumWindowFunc@8 and W32DDEFreeGlobalMem32@4. It permits only the exact
original window-procedure return-type cast and still requires the actual
callee/caller decorated symbols. Result WOW_USER_INPUT_CALLBACK_ABI_21_OK,
recorded at O:/winnt/logs/s40-user-callback-original-stdcall-abi.json.
This supersedes the prior description of seventeen originals plus four bridges:
all 21 now bind original bodies. It proves object ABI, not callback reachability,
output-table completeness or Win16 execution. Full worker rebuild/regressions,
USER shared-data/task integration and actual WRITE acceptance remain open.
No accepted O:/winnt runtime executable, DLL or guest media was replaced.

### Fresh worker build and established product regression

The retained formal-x86-r3 graph/runner still names S39/formal-x86-r2, so it
was not reused as a fresh-build proof. Generated worker-stdcall-boundary-r1
using New-T310OriginalSoftpcNinja.ps1, Architecture x86, ParallelJobs 4 and
O:/.nvm/versions/node/v22.22.1/bin/node.exe. Initial configuration picked the
default Node 24, but was regenerated with Node 22 before any compilation.
MSVC 14.43.34808 Hostx86/x86, SDK 10.0.22621.0, /MT and CCPU40 are selected.
Cold original-softpc-candidate + VDMREDIR.dll build: 512 commands, exit zero;
product-programs: remaining 16 commands, exit zero. Logs: formal-build.log and
product-build.log. VdmTib check passes (mvdm_vdm_tib.obj, 4208 bytes, no overlap).
Historical compiler warnings remain; this is not a warning-free claim.

| Tested artifact | SHA-256 |
| --- | --- |
| run16.exe | D5A71895767A32F9B53BBE87BA97A65D091D833BBC729FE1680F1423FD899F93 |
| basesrv.exe | 1A8C395B3FA0EAC982014228D5FE224D07712515C0E193F4C2EB7086F654C141 |
| ntvdm.exe | 502CB82D9BDA403746732664A3208027696ECC0EC428EC279D690FCD3EC1EFA9 |
| VDMREDIR.dll | E41F06AAC6C799D644D26A8930E245E3ECC08597B9BCA31A12B491EC30E0EDFD |

Confirmed no package process, backed up those four accepted runtime files in
the build root's accepted-before-test directory, and temporarily deployed only
the four candidates. Verify-CommandExitStatus.ps1 uses the freshly compiled
observer at O:/winnt/tests/S40ABI/observer.exe and the task build-root G7.COM.
The helper now derives generated guest input location from GuestFixturePath,
not the runtime observer directory; this fixes test layout without changing
product or original guest semantics.

All 17 transcript/exit-gated routes passed under s40-stdcall-product-r1:
interactive/direct COMMAND, three-depth COMMAND/MEM, repeated/direct MEM,
native streams/EOF, COMMAND /c, guest/native exit status and EDIT return.
The summary has 17 rows and zero expected/actual mismatches. Inputs, summary,
Console transcripts and individual reports are under O:/winnt/logs with that
prefix. The expected missing-command negative is separate from unexpected
Bad command or filename failures; normal routes require real Console text.

The finally path restored all four accepted runtime files and verified hashes:
ACCEPTED_RUNTIME_RESTORED_HASH_VERIFIED. The WOW research DLL was never
deployed. Relinking it against this cold build's ntvdm.lib also succeeds;
import-library SHA remains
3F39A986E387F3553DC45D027CFDF0545121E8D39EF78329E2976C2398681DA4.
Current research DLL SHA:
FB48C79BF3BD8C2E65A0146DCAC00381425ADDE41FAF65BC4648401058FA446D
(provider-stdcall-r1/current-parent-link.log). This supersedes the prior
pending worker-regression item, not the outstanding USER shared-data/task
integration or actual Win16/WRITE acceptance.

### Bitmap output-slot source boundary

Read-only follow-up audited WU32LoadBitmap in mvdm/wow32/wuser.c and
OpenNT/windows/core/ntuser/client/clres.c (SHA-256
C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5).
Procedure: follow WOWLoadBitmapA, CopyDibHdr, HowManyColors,
ConvertDIBBitmap, BitmapFromDIB, CreateScreenBitmap and
Convert1BppToMonoBitmap in that original translation unit. No bitmap
implementation, guest bytes, or runtime artifacts changed in this audit.

WU32LoadBitmap obtains original pointer aliases and delegates resource bytes to
the USER output slot; it does not itself normalize bitmap DIB headers.
WOWLoadBitmapA has two distinct branches: absent resource bytes use LoadBmp
(including ANSI-name conversion); supplied bytes use ConvertDIBBitmap with
zero requested dimensions, LR_DEFAULTSIZE and no returned header/mask.
The original function does not use cbResData. This fact is not permission to
read outside the worker's checked resource lease.

Correction from the subsequent caller audit: that lease is a required binding,
not a property already provided by WU32LoadBitmap. GETMISCPTR requests one
byte through GETOPTPTR; retail GetPModeVDMPointer drops the count and
GetPModeVDMPointerMacro returns FlatAddress plus offset. FREEMISCPTR is empty.
The original WOW16 fastres.c ILoadBitmap supplies SizeofResource as f4 and
holds LockResource until the thunk returns. The host must retain that original
synchronous lifetime while binding the full resource range; the mere presence
of f4 does not prove a bounded lease or safe host reads. Wcall16's separate
callback-frame lease changes do not fix these resource aliases.

The supplied-byte branch retains meaningful original algorithms:

- CopyDibHdr handles INFO and CORE headers, RGB triples/quads and the old
  icon/cursor header forms; HowManyColors determines the actual bits offset.
- True monochrome classification depends on palette contents, not just bpp.
- With this caller's fixed flags and arguments, there is no requested
  stretch, color remapping, icon-half-height adjustment or DIB-section flag.
- BitmapFromDIB still creates a screen-format surface, sets black/white
  conversion colors, uploads bits and restores its DC state. Its 1bpp path
  can convert the result to a true monochrome bitmap before returning it.

Recovery-ladder disposition: the whole clres.c translation unit is not yet
composed; it includes the historical USER precompiled environment and uses
USER-owned gcsHdc/ghdcBits2, oemInfo, gpsi, GETINITDC and other resource
families. Importing the complete USER client/server is not admitted. The
next candidate is the smallest original bitmap slice with bounded DC,
allocation and screen-capability bindings; its placement and complete
outgoing closure must be resolved before production implementation. No
external-code intrusion or independently authored conversion algorithm is
justified by this audit. In particular, a single CreateDIBitmap call is not
established as equivalent to the original branch.

Required focused verification includes INFO/CORE color resources, black/white
and non-black/white 1bpp palettes, integer/string resource identifiers,
conversion/allocation failure and GDI cleanup, followed by real WOW16
resource consumption. This is source evidence only, not a passed bitmap
slot, provider registration, or Win16 workload.

The first source-reuse experiment now passes. Test-only
`tests/observation/extract-wow-bitmap-header-fixture.ps1` pins the above
clres.c hash and mechanically extracts the original constants, OLDCURSOR,
HowManyColors, TrulyMonochrome and CopyDibHdr without editing their bodies.
Output stays in build/M0-T420/S40/bitmap-original-r1; it is not a production
source selection or a new mirror file. The companion
`tests/adapter-mvdm-host-out/wow/wow_original_bitmap_header_fixture.c`
supplies usercli.h-shaped declarations/BitmapWidth, a counted allocation
binding and a diagnostic-only macro. MSVC 14.43 x86 /MT /Gz /W4 compilation
succeeds (original signed-byte-pointer and assignment-condition warnings
remain; they are not grounds for modifying the original body).

Runtime O:/winnt/tests/S40USER/bitmap-header-fixture.exe exits zero with
WOW_ORIGINAL_BITMAP_HEADER_OK info=3 core=1 allocation-failure=1 invalid=1.
Its SHA-256 is
DA98F840066FB02ECEF7FE29FA80B5ED988B0E33E287B6378FE71E5297D8BA84;
stdout/stderr are O:/winnt/logs/s40-bitmap-header.{stdout,stderr}.log.
Assertions cover black/white and reversed white/black palettes, non-monochrome
colored 1bpp, CORE triple-to-quad conversion, source bits offsets, unchanged
input bytes, allocation failure, invalid header and four allocations/frees.
This proves these original parser bodies compose through finite bindings;
it does not test the old icon/cursor header branch, GDI bitmap creation,
name-based loading or guest/provider integration. Those remain required.

### Original bitmap GDI chain experiment

The same pinned extractor now also retains the unedited bodies of
Convert1BppToMonoBitmap, CreateScreenBitmap, BitmapFromDIB and
ConvertDIBBitmap. These are generated research inputs only. The fixture
binds original DC globals/locking to a private compatible DC and critical
section, obtains oemInfo plane/depth values from the screen, and uses native
GDI operations. Original CBM_CREATEDIB=0x02 (wingdip.h) and
LR_CREATEREALDIB=0x0800 (winuserp.h) are retained. Color-remapping and stretch
helpers excluded by WOWLoadBitmapA's fixed arguments are hard-fail traps;
they never fabricate a successful operation. This is not a generic
ConvertDIBBitmap implementation for other callers.

MSVC Win32/x86 /MT /Gz /W4 plus user32.lib/gdi32.lib builds successfully;
build/M0-T420/S40/bitmap-original-r1/compile.log retains original warnings.
The real-host run exits zero with WOW_ORIGINAL_BITMAP_GDI_OK:
30 conversions across INFO black/white, CORE black/white and colored 1bpp
resources. GetObject asserts dimensions and true mono depth where applicable;
GetPixel asserts both output pixels, and input resource bytes remain unchanged.
Allocation failure and screen-DC failure both return NULL through the original
path, with allocated headers released. Repeated output deletion leaves native
GDI object count unchanged (2 before, 2 after), and header allocation/free
counts match. Private working/read DCs are deleted afterward.

Latest fixture SHA-256:
A12F46CC198F34A748C935BD2BA272D5191A05571A340909C3C9832F264C9FAB.
Runtime location remains O:/winnt/tests/S40USER/bitmap-header-fixture.exe;
logs are O:/winnt/logs/s40-bitmap-gdi.{stdout,stderr}.log. This supersedes
the header-only fixture binary identity, not its retained earlier evidence.

Conclusion: the reached bitmap conversion algorithm, including the original
CBM_CREATEDIB/true-mono conversion on this host, can be reused through finite
resource bindings. A newly authored DIB parser/converter is unjustified.
Production placement remains unresolved under the no-new-mirror-file rule;
the experiment does not silently admit a new clres.c mirror or move USER
algorithms into a production adapter. Resource-name loading, old-format branch
coverage, full resource-span validation and real WOW16 consumption still need
verification before the output slot can be registered as complete.

### Owner clarification and old-format negative evidence

The owner clarified that the mirror rule forbids files invented by this
project, not importing actual original OpenNT files. Architecture/coding
authorities and source policy now say so consistently. The preceding
placement concern is superseded: audited necessary original clres.c and
hdata.c slices may enter their original opennt-host paths. Provenance,
complete dependency review, registered crops and bounded modern bindings
remain mandatory; this is not permission to import the entire USER subsystem.

Expanded original-header tests exercise 24bpp without palette, 16bpp bitfield
masks, explicit biClrUsed and old icon/cursor headers. The first old-icon
test fails: returned bits 006FF192 differs from source bits 006FF6C6, while
mono=1 and height=2. Original CopyDibHdr sets upbih to local Fake before
evaluating upOldIcoCur->abBitmap; the macro aliases upbih, so it no longer
refers to the input resource. This is an original host-source defect
candidate, not a guest limitation and not an accepted test failure. Do not
dereference the returned pointer after that function returns. Compare the
other pinned original tree and establish intended source lifetime before
admitting a minimal correction; the caller's actual format reachability also
remains to be established.

Failed fixture SHA-256:
500619829C98B7CC2D3D9D1CD3DEB1BF6DD1541DFD5A156823F2D935654D2B51.
Logs: O:/winnt/logs/s40-bitmap-old-format.{stdout,stderr}.log, exit 1.
The earlier INFO/CORE/GDI passing runs remain bounded historical evidence;
the expanded fixture is currently failing and must not be reported green.

### Original FreeDDEData import and first native verification

Following the owner's mirror-rule clarification, OPENNT-HOST-032 imports
only FreeDDEData into its original path
src/opennt-host/windows/core/ntuser/client/hdata.c. Original body comparison
(newline-normalized) passes with ORIGINAL_FREE_DDE_BODY_IDENTICAL. The
source identity is 3BEC21C4BB3CAEF0798208693A89012DF6005C6A330F0F670F974A1CE821FCA2.
The original module header and function documentation remain; crop/include
markers register the excluded DDEML API family and finite declaration binding.
The README records direct wumsg.c callers, outgoing closure and selection.

Recovery ladder: the whole translation unit depends on excluded USER DDEML
instance/transaction/handle machinery. Its FreeDDEData body has no such
dependency and is retained unchanged; original ddetrack.h DDE_DATA and
usercli.h retail lock/unlock/free macros are bound by
ntvdm-exe/wow/include/wow_dde_data_bindings.h. All seven outgoing functions
are public Global/GDI APIs. No external intrusion, replacement algorithm,
generic resource registry or native pointer in guest state is introduced.
The three-argument original interface and x86 stdcall are preserved.

Build: MSVC 14.43 x86 /MT /Gz /W4 /WX, SDK 10.0.22621,
user32.lib/gdi32.lib, build/M0-T420/S40/dde-original-r1. The original imported
file and tests/adapter-mvdm-host-out/wow/wow_original_dde_free_fixture.c
compile without warnings. Native fixture exits zero:
WOW_ORIGINAL_DDE_FREE_OK release override bitmap-ownership nested-dib null.
It checks retaining data when release is clear, forced release, bitmap
retention/deletion according to fFreeTruelyGlobalObjects, nested DIB release,
NULL nested data, lock balance and NULL outer input.

The initial test's immediate GetObjectType deletion assertion failed; after
GdiFlush, GetObject verifies that the object no longer exists. Only the test
observer changed, not the imported release code. Passing fixture SHA-256:
5A8F798BF09B79FA6259E4EC2FF550EB844B69192D25AAC5FBADCAA3FA6A529D.
Runtime O:/winnt/tests/S40USER/dde-free-fixture.exe; logs
O:/winnt/logs/s40-dde-free.{stdout,stderr}.log. Metafile/palette format
coverage and actual wumsg/WOW16 task integration remain required. The file
is fixture-selected only; no working pfnOut registration is claimed and no
product/guest artifact was replaced.

Follow-up native tests add CF_PALETTE with both ownership choices,
CF_METAFILEPICT/CF_DSPMETAFILEPICT, CF_ENHMETAFILE/CF_DSPENHMETAFILE
and CF_DSPBITMAP. Resources are verified live before the call, then checked
for original retention/destruction, including outer and METAFILEPICT handles.
All pass with exit zero. Latest fixture SHA-256:
C989ED5C918BCAE3236B9976488FB54D0852A58D6A0E2F0A142474B41DE50926.
Logs: O:/winnt/logs/s40-dde-free-all-formats.{stdout,stderr}.log.

The provider generator now includes obj/opennt-user/hdata.obj, separate from
adapter objects. Formal compilation exposed DDE_FRELEASE's missing explicit
ddeml.h dependency; the binding now includes that declaration header instead
of inventing the constant. The complete provider link succeeds in
provider-stdcall-r1 (dde-owner-build-r2.log); its map confirms
_FreeDDEData@12 from hdata.obj. WOW32.DLL SHA-256:
FF1F9EBF83BC2F599DE8D91817F2508BEF1923984C919751D0DDF4DB48576D9D.
This supersedes fixture-only selection. Actual pfnOut registration,
malformed-input boundaries and guest message cleanup remain unverified.
The DLL was not deployed and is not a real-Win16 acceptance result.

### Original bitmap owner recovery and old-header correction

OpenNT-4.5 nt/private/ntos/w32/ntuser/client/clres.c has the same SHA-256 as
the selected OpenNT clres.c. Both contain the observed old-header pointer
ordering defect. OPENNT-HOST-033 imports the seven original bitmap functions
and constants/header layout into opennt-host/windows/core/ntuser/client/clres.c
as a registered subset; original header retained, unrelated resource families
cropped, dependency definitions ordered before callers. It is fixture-selected
only: no generated research include is a product source dependency.

OPENNT-HOST-034 captures upOldIcoCur->abBitmap before assigning upbih=&Fake.
These are the only two executable statements reordered; no new parser or
extra state is introduced. All six other imported function bodies compare
identically with upstream after newline normalization. The unchanged original
CopyDibHdr negative result remains recorded above; defining
WOW_BITMAP_UPSTREAM_NEGATIVE selects it in the same fixture. The default
fixture now compiles the actual mirrored correction, not a copied substitute.

Corrected x86 run exits zero, both old icon and cursor bits equal input
abBitmap (00E7FBB2), mono=1 and converted height=2. INFO, CORE, 24bpp,
bitfields and explicit color-count checks pass, as do all 30 prior native
GDI conversions, allocation/DC failures and resource counts (2 before/after).
Markers: WOW_ORIGINAL_BITMAP_HEADER_OK info=6 core=1 old=2
allocation-failure=1 invalid=1; WOW_ORIGINAL_BITMAP_GDI_OK repetitions=30.
Fixture SHA-256:
03DBCAD850B40CEC0E42EB86A007EA6FC6556252809A7DB8CF5B05184D11447E.
Logs: O:/winnt/logs/s40-bitmap-corrected.{stdout,stderr}.log.
This resolves the fixture's old-header failure; it does not prove actual
Win16 old-format reachability, full resource bounds or provider integration.
Production bitmap bindings and name-based resource loading remain unfinished.

### Bitmap auxiliary dependency closure

The unresolved SetBestStretchMode is an original usercli.h macro over public
SetStretchBltMode, not a missing server routine. ChangeDibColors exists in
clres.c and is now retained unchanged; SYSRGB and RGBX have finite public
GetSysColor and original byte-order macro bindings. SmartStretchDIBits lives
in original cldib.c and includes a specific ScaleDIB interpolation path;
mapping it blindly to StretchDIBits would lose original behavior.

OPENNT-HOST-035 therefore retains cldib.c under its original opennt-host
path (source SHA E16FB36805F44767B45B597510A6C08D1DBC47271783E5D1F8F09273C0F782FB),
replacing only the precompiled include with an annotated consumer-binding
requirement. It has local scale/color-map algorithms, public GDI queries and
StretchDIBits, allocation/free and assertions, not kernel/USER server calls.
The bitmap fixture now composes these original helpers instead of hard-fail
substitutes in its corrected-source mode. The unchanged-upstream negative
mode retains its own explicitly unreachable color helper trap.

This x86 composition compiles and all prior header/pixel/cleanup tests pass,
exit zero, unchanged GDI counts 2/2. Fixture SHA:
F62BB2092EECC19FCD0FCDD3671B653C2B3A577E5C149CD5723C6246B258B52E.
Logs: O:/winnt/logs/s40-bitmap-helpers.{stdout,stderr}.log.
The fixed WOWLoadBitmapA argument tests do not exercise requested scaling or
color-remapping flags; importing/linking those helpers is not their runtime
acceptance. Production lifecycle binding and real provider use still remain.

### Separately compiled bitmap owner and invocation binding

ADAPTER-WOW-025 now supplies wow_bitmap_bindings.h/c. Both original clres.c
and cldib.c compile as separate translation units with that header; the
existing source-body fixture explicitly selects its own fault-injection
environment. The binding uses public heap/GDI operations and retains the
original usercli.h bitmap macros. Each invocation owns a compatible DC and
screen capabilities; explicit enter/leave push/pop a thread-local context.
One recursive critical section serializes the original shared scaling maps.
DCs are released at return, while terminal shutdown releases the lock only
after all calls finish. It is not a restart API or guest resource manager.

MSVC x86 /MT /Gz /W4 compiles these two owners, the binding and
tests/adapter-mvdm-host-out/wow/wow_bitmap_binding_fixture.c independently.
Build root: build/M0-T420/S40/bitmap-binding-r1, compile.log. Native exit zero:
WOW_BITMAP_BINDING_OK nested=30 gdi-before=0 gdi-after=0.
Tests check NULL context rejection, 30 nested entry/conversion/leave pairs,
restored previous TLS state, mono dimensions, cleared exited contexts and
repeated terminal cleanup. SHA-256:
9C24967102F597F52B974A823D80A842435EB66DD57544D9F7C0A1577D0D945D.
Runtime O:/winnt/tests/S40USER/bitmap-binding-fixture.exe; logs
O:/winnt/logs/s40-bitmap-binding.{stdout,stderr}.log.

The binding has not yet entered the provider graph. Initialization fault
injection, concurrency, shutdown wiring and full guest-resource range binding
are still required; no real Win16 or DLL teardown pass is implied.

The next binding run passes four simultaneous threads x 50 conversions plus
30 nested calls, with GDI objects 0 before/after and restored TLS contexts.
Test-only API substitution also verifies critical-section initialization,
GetDC and CreateCompatibleDC failure/retry; outer context survives and screen
DC acquire/release counts match 4/4. No test control is added to production.
Both exit zero. Parallel fixture SHA:
7AA7EA6EF5E6C46940A640D4BB7BEDCFA4B2569665FB4D4A69324B04FB077D6F;
failure fixture SHA:
03674F27553C32935379C46CEF41C58E82F260505B0537A660DF651530D2C810.
Logs: O:/winnt/logs/s40-bitmap-binding-{fixture,failure}.{stdout,stderr}.log.

Provider composition now includes clres.obj, cldib.obj and the binding.
The CRT bridge calls shutdown after original W32DllInitialize on normal
detach only (reason=0, reserved=NULL), not process termination. All active
calls must have finished before normal unload. Full link succeeds in
provider-stdcall-r1/bitmap-owner-build.log; map contains ConvertDIBBitmap@24,
SmartStretchDIBits@52 and the three binding methods. WOW32.DLL SHA:
DB620FC47AA1C82B437B7EF16D666D939464D3CD17D6E63A2B88AF787BF847B0.
This supersedes fixture-only selection and unwired shutdown, not runtime DLL
unload acceptance, full resource validation or real WOW16/output-slot use.

### Original WOWLoadBitmapA entry composition

The original entry body is now retained in clres.c without algorithm changes.
Its integer-ID predicate and MBToWCS macro preserve winuserp.h shape;
MBToWCSEx is supplied by the already admitted original chartran.c, not a new
conversion routine. The reached LoadBmp call always supplies zero dimensions
and flags and binds to LoadBitmapW. This binding is limited to that caller;
it does not claim full LoadBmp cache/file/resize policy. Bitmap local allocation
now uses LocalAlloc/LocalFree, matching the existing chartran native allocator
so ANSI-name buffers are not freed through a different allocator contract.

The separately compiled binding fixture now calls original WOWLoadBitmapA
for its 30 nested and 4x50 parallel data conversions. It also checks absent
string and integer-ID resources return NULL through their native branch.
All pass, exit zero, GDI count 0/0. Fixture SHA:
EFB08DCD4CEDA48B193CF73B4314EF8005B48517FCB8D433225BE48289ECCDB8.
Logs: O:/winnt/logs/s40-bitmap-entry.{stdout,stderr}.log.
Provider full link also succeeds (bitmap-entry-build.log), WOW32.DLL SHA:
58E2F09DAD3DE6EF75357D354EF18D1A7AF28DA4505B07062F7683EF8A6E2EEB.
Positive native named-resource loading, bounded guest resource/name leases,
output-slot registration and real WOW16 calls remain unverified. The private
entry requires its conversion environment to be bound around supplied-data
calls; no raw guest caller has been enabled by this composition work.

### Positive native resource names and guest-span audit

The binding fixture now embeds a synthetic host bitmap under integer ID 101
and string name S40BITMAP. tests/observation/new-wow-bitmap-test-resource.ps1
generates its 66-byte BMP strictly under the build root; the checked-in RC
selects it twice. Neither the generator nor RC modifies historical guest media.
Both original WOWLoadBitmapA name branches load successfully from the fixture
module. GetObject checks 2x1 dimensions and GetPixel checks white/black pixels;
the test deselects/deletes the resulting objects and DCs. Missing-name/ID,
30 nested and 4x50 parallel data conversions remain passing. The reported
GDI 0/0 count measures the data-conversion loop before the named tests, not a
separate leak measurement for resource caching.

Native exit zero: WOW_BITMAP_BINDING_OK nested=30 parallel=4x50 named=2
missing=2 gdi-before=0 gdi-after=0. Fixture SHA-256:
2779F83BD908DED854D207DCF5C97E0CC13C56EA7E81055F50245D4FB7D85E22.
Logs: O:/winnt/logs/s40-bitmap-named.{stdout,stderr}.log.
An initial inline RC-data attempt failed compilation; a subsequent accidental
run used the preceding artifact and is not named-resource acceptance. Only
the rebuilt resource-bearing artifact/hash above supplies that evidence.

The guest-path audit confirms wow_callback_frame_acquire_vp translates a
selector/offset and acquires a linear span; mvdm_softpc_protected_address
uses the original descriptor decoder but does not check a requested length
against segment limits. Original WU32LoadBitmap GETMISCPTR/retail FlatAddress
also does not provide that check or a bounded lease. Do not describe either
as a complete resource-validation contract. Before changing the caller, retain
original non-present-selector callback/loading behavior and account for
SizeofResource/LockResource lifetime and resources spanning selector-sized
regions; imposing an unproved single-selector cap is not a valid repair.
Actual guest resource/name binding and pfnOut integration remain unfinished.

### Resource span and explicit address-mode decision

Further original-source inspection resolves two design questions without
changing guest or CPU semantics. kernel31/resaux.asm ISizeofResource shifts
rn_length through DX:AX according to rs_align, retaining a 32-bit result;
ILockResource calls GlobalLock and handles nonresident resources. USER16
fastres.c keeps the loaded/locked resource through the WOW call and unlocks
and frees only afterward. kernel31/ldseg.asm explicitly describes a huge
allocation as one memory block represented by multiple selectors; secondary
selectors are not separately lockable handles. kernel.api's huge-buffer
contract explicitly permits access beyond a 64K segment. Therefore a newly
invented single-selector/64K limit cannot replace resource-size binding.

Address mode is also significant. Original resource GETMISCPTR routes through
GetPModeVDMPointerAssert, independently of current CPU mode. W32Dispatch's
original comment and WOWGetVDMPointer call explicitly permit real-mode error
entry. The existing wow_callback_frame_acquire_vp instead delegates to
mvdm_softpc_effective_address, which chooses real/protected addressing from
current PE/VM. It cannot be blindly reused for these resource pointers.
The existing worker mvdm_softpc_protected_address already retains the original
selector_outside_GDT_LDT/read_descriptor_linear mechanism independently of
PE/VM; it is not yet exported to this DLL. No new selector decoder is needed.

Selected next implementation boundary: retain original non-present-selector
resolution before acquiring the resource; use explicit protected translation
and the supplied 32-bit resource length for a synchronous read-only lease;
release after the native operation, before guest unlock/free. Preserve separate
handling of integer resource identifiers and terminated names. Test a resource
larger than 64K, real-mode host-entry with a protected resource selector,
invalid descriptor, end-of-memory/overflow rejection and lease cleanup.
These are implementation obligations, not claims of existing resource safety.

### Explicit protected resource lease implementation and focused verification

The existing callback lease binding now offers
`wow_callback_frame_acquire_protected_vp`. It delegates to the existing worker
`mvdm_softpc_protected_address`, newly exported with explicit cdecl ABI, then
the existing linear lease. No descriptor decoder, memory manager, segment
limit policy or guest-media modification was added. Original resource demand
loading/locking must still precede this call; no actual WU32 resource call has
been redirected yet. This is ADAPTER-WOW-001's address-mode binding, not a
replacement for original resource policy.

The native `wow_protected_resource_lease_fixture.c` composes the production
WOW binding and production bounce-lease implementation with mocked descriptor
lookup and backing storage. `/MT /Gz /W4 /WX` x86 build and runtime pass:
98,304-byte read spanning 64 KiB, explicit protected lookup while the unused
current-mode stand-in would produce a different address, invalid selector,
32-bit range overflow, backing-end overrun, zero length, invalid access,
failure cleanup and successful retry ending exactly at backing end. No writes
occur; seven protected calls, zero current-mode calls, zero stack delta.
This does not test the real descriptor decoder or prove Win16 execution.

The previous frame ABI fixture also passes unchanged expectations: three
current-mode lookups, two acquisitions, one release and zero stack delta.
Logs: `O:/winnt/logs/s40-resource-lease.stdout.log` and
`O:/winnt/logs/s40-frame-abi.stdout.log` (paired stderr files empty).
Build root: `build/M0-T420/S40/resource-lease-r1`.

Artifact SHA-256 identities:

- Resource fixture: `8D8F1C4E1935C6099FE2CEA051FFAD5477CDDBF4B38276F3D0454122FC410B18`.
- Existing-frame fixture: `DB69B0F20D42F1DEF4F0D0F9B1A677387A51D3E70F7869994F3205BA129317A9`.
- Relinked worker: `75017812A0296C442BA84C76569069975C15DE145CDBFA2945128FF403144449`.
- Relinked WOW32: `4657C299BAC319BB678AEA6C7EF7AADF2BDE3004FA8965AE0561F7CC37A661A3`.

The regenerated worker `product-programs` target and WOW32 target link
successfully; `product-build.log` and `provider-build.log` record this. Neither
new product binary was published to the runtime root. The prior 17-route pass
does not certify these new artifacts. Actual resource-name/span integration,
provider registration, real Win16 workload and renewed product regression
remain required before S40 closure.

### WU32LoadBitmap resource lease call-site integration

MVDM-HOST-DIV-295 connects the explicit protected read lease to the actual
original WU32LoadBitmap body. Original GETPSZIDPTR/GETMISCPTR still run first,
including original demand-loaded-selector resolution. The original pfnOut
call receives leased bytes and the unchanged DWORD resource size. A failed
acquisition skips USER and returns the existing zero result through original
cleanup. After a successful call the lease releases without writeback before
the original pointer cleanup. Native resource-name/ID handling, bitmap handle
aliasing and the original output-table ABI are unchanged. No guest source or
binary was changed.

`extract-wow-bitmap-call-fixture.ps1` extracts the current function body from
wuser.c; the fixture does not maintain a separately copied implementation.
The native /MT /Gz /W4 /WX fixture observes that actual body's macros and
provider/lease boundaries: successful 96 KiB resource delivery, failed
acquisition without provider entry, and null-payload native resource dispatch.
All three call-order/cleanup cases pass with errors=0. This is a mocked
boundary test, not actual demand-loading or guest bitmap acceptance.
Fixture SHA-256:
`BF13A259672650551ECFF6B902AEC573405D895CD5F804391217AEA13FA170C6`.
Log: `O:/winnt/logs/s40-bitmap-call.stdout.log`; stderr is empty.

The actual wuser.c translation unit and WOW32 DLL also compile/link in the
formal x86 provider graph. Build log:
`build/M0-T420/S40/bitmap-call-r1/provider-build.log`.
DLL SHA-256:
`F91C372C7CB18A5CA1FFA90E24F0B1D632287D89C34B5CEEA6DED80681AB26DC`.
The DLL is not deployed. The original bitmap algorithm still ignores its
cbResData argument; a full-sized lease does not prove malformed DIB safety.
Output registration, native bitmap context entry/exit, bounded guest names,
malformed-resource handling and real Win16 execution remain open obligations.
The semantic patch is 12 added/1 removed text lines (`--ignore-space-at-eol`).
The tracked baseline was LF despite pinned original CRLF. After checking the
original bytes, the final working file restores CRLF as requested by the
owner. Raw Git statistics therefore include whole-file line-ending churn;
these formatting lines are not autonomous logic or semantic mirror growth.
Final wuser.c SHA-256:
`970EC8D1FB89343F9C76E0A36881D93F52C67FE8F80DB668A15D2A916F1939AC`.

### Bitmap output-slot environment and joined native chain

ADAPTER-WOW-025 exposes `wow_bitmap_load` with the original four-argument
stdcall output-slot ABI. Resource calls enter the native DC environment,
execute unchanged original WOWLoadBitmapA and leave; native name/ID calls
go directly to the original owner. No parser or resource policy was added.
The function compiles in WOW32 but is not installed by a complete registrar;
W32Init still calls the modern system registrar.

`wow_bitmap_thunk_integration_fixture.c` joins the extracted current
WU32LoadBitmap, production protected-address and bounce-lease implementations,
the new output entry, original clres/cldib algorithms and actual public GDI.
Guest descriptor lookup, original pointer macros and handle alias conversion
are mocked. A 98,304-byte allocation containing a 128x255 24-bpp DIB produces
correct dimensions and first/last white pixels. An overrun is rejected before
USER; leases and GDI objects are released. This is not guest execution or
PMODE32 publication acceptance.

The existing binding fixture now calls this output entry for 30 nested,
4x50 concurrent, two named/ID and two absent-resource cases; all pass. The
failure fixture proves DC failures skip conversion, preserve the outer
context, release 7/7 screen DCs and permit retry. Fixtures compile with
/MT /Gz /W4 /WX; the actual provider graph relinks successfully.

Logs in O:/winnt/logs: s40-bitmap-chain.stdout.log,
s40-bitmap-slot-binding.stdout.log and
s40-bitmap-slot-binding-failure.stdout.log. Paired stderr files are empty.
Build root: build/M0-T420/S40/bitmap-call-r1; provider build log:
native-chain-provider-build.log. SHA-256 identities:

- Joined chain: `AB9314D47E45D489C59EA0225765623EA96E847DCB7CFAE14566B03D15C4F64B`.
- Binding: `7227E65E5C6297CEB9D04637B33C3A46AC903269CB50C554D0164E91D688F93E`.
- Failure: `AB2A114FCF672DB8F5384B0F232214B341775441AF50BE88F4732BDE8880B9E0`.
- WOW32: `6885AF16E8DEF5D39517143F653EA6E49B5050AF000B8D404BF207B2D780FC10`.

No product binary was deployed. Complete USER registration/shared graph,
original scheduler/task lifecycle, bounded guest names, malformed-DIB
disposition and real Win16 acceptance remain necessary for S40 closure.

### Original USER handle validation executes on CCPU

The existing bounded CCPU fixture now optionally loads the unchanged
127-byte HMValidateHandle function at segment-1 offset 08F8 from pinned
USER.EXE. It retains the original CS-relative wow16gpsi reference at 5DA2;
only its normal runtime shared-data input is initialized in fixture memory.
The program executes at CPL=3 with flat DS, a TEB-backed FS, original page
walking and original host_simulate. No CPU instruction, guest-media file or
original function instruction byte is modified. Source correspondence is
OpenNT windows/core/ntuser/rtl/wow.c ValidateHandleMacro and its WOW-specific
DESKTOPVALIDATE, not the different ordinary USER32 desktop fallback.

Ten actual guest-function cases pass: matching uniqueness; stale uniqueness
rejection; zero and FFFF uniqueness wildcards; out-of-table index rejection;
wrong object-type rejection; TYPE_GENERIC; object below desktop base; last
byte within desktop range; and object exactly at the excluded upper bound.
The first valid object is exactly at the included lower bound. The original
function applies ulClientDelta only inside the desktop range. Every call
returns through its original RETF with correct EAX and restored stack depth.
The existing clock/paging/native-backing tests pass in the same run.

Runner: tests/observation/verify-wow-original-handle-validator.ps1. It pins
USER.EXE SHA-256 before and after, bounds execution and requires both the
ten-case marker and final CCPU boundary marker. The optional environment
input is restored afterward. This is a real immutable guest function in a
synthetic shared graph, not real window lifecycle or full WOW acceptance.

Build: build/M0-T420/S40/ccpu-shared-page-r1; logs user-validator-build.log
and user-validator-rebuild.log. Runtime logs:
O:/winnt/logs/s40-user-handle-validator.stdout.log and paired stderr.
The stderr Bad Id line is the existing intentional pre-initialization TLS
negative test, not the result of these handle cases. Fixture SHA-256:
`F3271B414E85D65A652DBBC6A0AE06059691758FD8AC33F4316F3DA83376ED44`.

The first run timed out because the new test encoded CS/FS descriptor base
bytes in the limit field. Only those test constants were corrected. The
first-failed stdout/stderr logs remain under O:/winnt/logs; that run is not
acceptance and supplies no evidence of an original USER or CCPU defect.

Recovery implication: do not author a second handle-validation algorithm.
Publish the source-defined shared table, desktop range, client delta and
thread data so the immutable original can perform this work. Production
publication, mutation, stale-object cleanup and all other optimized USER
consumers remain incomplete. No production code was added in this step.

### Window WOW-word ownership before first callback

Original W32CreateWindow initializes the first three DWORDs of a stack WW,
then passes its address through pfnCsCreateWindowEx. Original USER
kernel/createw.c copies only sizeof(pwnd->adwWOW), not sizeof(WW).
rtl/getset.c returns a pointer at that embedded prefix for GWL_WOWWORDS.
The remaining fields belong to the native window owner, not to the caller's
stack. Existing consumers in wcntl32/wmsgcb/wmsglb read dwStyle; wmsg16 and
wmdisp32 read dwExStyle; W32Win16WndProcEx uses the low WORD of hInstance.
Original walias.h HMODINST32 packs guest instance and module identities;
substituting a native HINSTANCE would therefore corrupt callback identity.

The existing creation-order fixture now includes the actual original
wowuserp.h ABI and compile-time checks: WW=32 bytes, adwWOW=12 bytes and
dwStyle offset=24. Its creation input ends exactly before a PAGE_NOACCESS
guard page, so copying the complete WW would fault. The copied 12-byte prefix
and separately retained synthetic guest module identity are available before
the first native message; the native class/module continues to use the real
HINSTANCE. No application userdata or CREATESTRUCT parameter is stolen.

The /MT /Gz /W4 /WX fixture passes normal/nested create, WM_NCCREATE refusal,
both CBT-veto orders, allocation/property failures and final destruction:
created=4, destroyed=4, live=0, allocations=5, frees=5, failures=0.
Original anonymous-union warnings are suppressed only around the unchanged
original header; no ABI header is modified. Output marker:
WOW_ORIGINAL_WW_PREFIX_GUARD_OK input=12 view=32.
Build root: build/M0-T420/S40/window-words-r1.
Runtime logs: O:/winnt/logs/s40-window-words.stdout.log and paired empty stderr.
Executable SHA-256:
`70EB34BBFC47EA71C43C464F72ECFFB1C1AC1C08F749F42280963F1BBBEF362C`.

This is native lifecycle feasibility, not a production window provider.
Internal USER state flags, class data, mutation publication, real guest
callbacks and their shared graph remain unimplemented. The next production
binding must preserve the split ownership above; it must not copy 32 bytes
from the original caller or expose native module identity as guest identity.

### Class-registration source closure selection

The complete client path is not only client.c::RegisterClassWOWA. That wrapper
copies WNDCLASSA into WNDCLASSEXA, sets hIconSm/cbSize, then calls
inc/ntcftxt.h::RegisterClassExWOW (ANSI TEXT_FN instantiation). The latter
owns material compatibility policy before kernel registration. Replacing it
with public RegisterClassExA plus an auxiliary record would skip original
behavior and is rejected as the production implementation plan.

Pinned original sources under windows/core/ntuser:

- client/client.c: `787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472`.
- inc/ntcftxt.h: `E7F3D42CE8939BBE0B14EA34BC693107334C19F8B2D7862EC15B75A501D11DAB`.

Selected original closure for recovery comprises RegisterClassWOWA,
RegisterClassExWOWA, StringDuplicateA and InitClsMenuNameA. Its finite external
bindings are original-shaped expected-version/client-info lookup, string
capture/conversion/cleanup, native GDI handle validation and the final
NtUserRegisterClassExWOW resource/metadata boundary. The existing original
chartran conversion owner must be reused. The source includes these policies:

- Negative class/window extra sizes fail; invalid cbSize is only diagnosed
  at this layer, not immediately returned as failure.
- A null module is normalized; USER-module rejection depends on expected
  version rather than an unconditional modern-host rule.
- Unknown class-style bits are rejected for versions above 3.1, but stripped
  for older applications.
- An invalid non-system background brush fails for versions above 3.0;
  older applications receive a null brush.
- Menu names retain separate ANSI and Unicode copies; integer resource IDs
  remain IDs. Cleanup and successful class ownership are not interchangeable.
- Versions below 3.1 receive strict BOOL rather than the returned class atom.

kernel/ntstubs.c::NtUserRegisterClassExWOW is the probe/capture boundary;
kernel/class.c owns the original class and clone lifetime. Modern kernel
pointers cannot supply the historical guest/client layout. Reuse the client
policy first, then bind unavailable native ownership and two original WOW
class DWORDs separately from per-window WW. Retain guest module scope even
when native registration uses a valid host module. A per-window copy is not
a class record, and ordinary native callbacks cannot receive tagged 16:16
WNDPROC values.

Recovery ladder: the original client bodies are available (first rung);
their NT4 environment needs the finite second-rung facade above. Importing
the whole USER kernel/desktop engine or altering host USER is not selected;
new class-validation algorithms are unnecessary. No incomplete registration
provider was added or installed this turn. Native callback dispatch, class
unregistration/clone propagation and shared graph publication must compose
before declaring this owner restored. The newly identified client policy
closure replaces the simpler proposed public-registration wrapper plan.

### Original class client executable contract

`tests/observation/extract-wow-class-registration-fixture.ps1` now hash-checks
the two sources above and original inc/ntsend.h
(`B708F1FEE46E93735C4C931D556E16DAA9679E6352D72620904D18AD21396ABF`).
It extracts the four complete original function bodies into the disposable
class-registration-r1 build root, together with the unchanged call/capture
macros. This is a source test, not a new production mirror or provider.
Generated function include SHA-256:
`6B5AE594381B93717F9DBB78DB65AD2894CB92FB06B1F3B6992411F7A55270A1`.

`tests/adapter-mvdm-host-out/wow/wow_class_registration_fixture.c` compiles
those bodies under MSVC 14.43.34808, SDK 10.0.22621.0, x86 `/MT /Gz /W4 /WX`.
Only original ENDCALL(BOOL)'s ATOM return narrowing warning is suppressed
around the extracted source. The fixture links the existing original
chartran conversion object, not a new MBToWCS algorithm. Reused provider
objects and hashes are:

- adapter-win32/chartran.obj:
  `21CB12E9B6072E0F869A46C88E64DEC0FAB4ECEF0449F27F6668ADDF5D043430`.
- adapter-win32/ntuser_rtl_compat.obj:
  `F24DD60BD4E7873A68220DF58DE2217F47A8295B161D130D27A80ECB3A951288`.

After running the extractor, the build command in an x86 MSVC environment is:

```text
cl /nologo /MT /Gz /W4 /WX /Ibuild/M0-T420/S40/class-registration-r1 tests/adapter-mvdm-host-out/wow/wow_class_registration_fixture.c /Fobuild/M0-T420/S40/class-registration-r1/fixture.obj /Febuild/M0-T420/S40/class-registration-r1/fixture.exe build/M0-T420/S40/provider-stdcall-r1/obj/adapter-win32/chartran.obj build/M0-T420/S40/provider-stdcall-r1/obj/adapter-win32/ntuser_rtl_compat.obj user32.lib ntdll.lib
```

The isolated runtime executable is O:/winnt/tests/S40USER/class-registration.exe,
SHA-256 `C91E43F8CDD6EF6AFF80C20B4A99C26045EB9824A8B4B2EB8D691F4769FDC9D2`.
O:/winnt/logs/s40-class-registration.stdout.log reports:

```text
WOW_ORIGINAL_CLASS_MENU_CAPTURE_FAILURE retained=2 expected=2
WOW_ORIGINAL_CLASS_REGISTRATION errors=0 cases=16 captures=0 menus=8/8
```

Exit is zero and paired stderr is empty. Fifteen normal/negative cases verify
the original compatibility rules listed above, two-DWORD WOW pointer delivery,
ANSI flag, DDE fnid normalization, class-name capture failure, kernel failure
cleanup, and the WNDCLASS-to-WNDCLASSEX wrapper. Kernel registration, expected
version, client info, GDI validity and capture are test doubles; no actual
class registration, guest callback or Win16 application acceptance is claimed.

The sixteenth case deliberately exposes an original **host** cleanup defect:
InitClsMenuName duplicates both menu strings, then a failed COPYLPTSTRID jumps
to its errorexit. RegisterClassExWOW returns immediately on that failure,
before its menu cleanup. Two allocations remain. The test asserts this
observation and releases them itself only afterwards; the final 8/8 counter
is fixture hygiene, not proof that original failure cleanup is correct.
This is not an immutable-guest exception and must be repaired with a registered
minimal host correction when this original owner is composed for production.
Retain the failure injection and require zero outstanding allocations without
fixture rescue for that production acceptance.

The outgoing lifetime audit also finds that WU32UnregisterClass calls the
public UnregisterClass directly, not an output-table slot. Replacing only
pfnRegisterClassWOWA cannot close class lifetime. The original kernel
_UnregisterClass rejects live-window references, returns the two client menu
pointers, and destroys the class; its client counterpart frees those pointers.
Production recovery must bind both registration and this direct unregister
consumer, preserve guest module scope, and synchronize class/clone updates.
kernel/class.c's comment says "5 WOW dwords", but the copy length is the
actual `sizeof(pcls->adwWOW)`; the structure, not the stale comment, controls
the ABI. No production registration slot has been installed yet.

### Class capture and unregister recovery, minimal host correction

The next audit replaces the test's independently written ANSI capture with
the unchanged original client/rtlinit.c::RtlCaptureAnsiString, source SHA-256
`D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C`.
The fixture supplies heap fault injection and a test-only TEB scratch field;
conversion calls the native NTDLL routine. Original chartran remains selected
for client menu duplication. No real TEB is read or changed. The complete
original ntcftxt.h::UnregisterClass body also enters this test.

The earlier mock capture was supporting evidence only. The new original
capture reproduces the same two-allocation leak. The finite original class
client subset is now imported at its real upstream-relative path:
src/opennt-host/windows/core/ntuser/inc/ntcftxt.h (OPENNT-HOST-036).
It retains StringDuplicate, InitClsMenuName, RegisterClassExWOW and
UnregisterClass; unrelated text APIs are explicitly cropped. The smallest
host fix adds two LocalFree calls only at InitClsMenuName's capture-error
exit (OPENNT-HOST-037). On this exit both menu pointer fields have been
assigned; failed secondary conversion assigns NULL. Resource-ID capture
does not take the allocation-failure exit. Successful class ownership and
caller cleanup after kernel-registration failure are unchanged.

The extraction script now also verifies the complete retained source token
stream against the pinned original subset, allowing only those exact two
adjacent calls once. It reports `WOW_CLASS_MIRROR_SOURCE_MATCH correction=2`.
Mirror SHA-256 after restoring original CRLF formatting:
`43EEADB99CA6519A954E195F7175C00EEA276E6402471F9FEA2745924C6012C9`.
This is two added executable lines, not a new validation or class-state
algorithm. The mirror is selected in the focused fixture; it is not yet
installed into the formal WOW32 output table or its direct unregister import.
The required native class/callback/shared-graph binding remains unfinished.

Build uses the preceding x86 recipe with the original mirror include path
`/Isrc/opennt-host/windows/core/ntuser/inc`. Build `original.exe` without
WOW_CLASS_MIRROR and `corrected.exe` with `/DWOW_CLASS_MIRROR`, using distinct
same-named object outputs under class-registration-r1. The original arm
includes the hash-pinned extracted functions; the corrected arm includes
the actual mirrored ntcftxt.h plus the unchanged original RegisterClassWOWA
wrapper. Both use the actual extracted original capture and unregister.

Both isolated runtime arms exit zero with empty stderr:

```text
original:  WOW_ORIGINAL_CLASS_MENU_CAPTURE_FAILURE retained=2 expected=2
corrected: WOW_CORRECTED_CLASS_MENU_CAPTURE_FAILURE retained=0 expected=0
both:      WOW_ORIGINAL_CLASS_REGISTRATION errors=0 cases=18 captures=0 menus=10/10
```

Executable identities under O:/winnt/tests/S40USER:

- class-registration-original.exe:
  `0DD3887D2932FAF85096B5A8C4D3043E538A5C9A832BFEA16891767CEAAE0B82`.
- class-registration-corrected.exe:
  `379A6D02D5B1041860D2E4C3B13063933FACE79E1D30B5936B13CF38125967EC`.

Logs are O:/winnt/logs/s40-class-registration-{original,corrected}.stdout.log
and paired stderr. The original arm still explicitly rescues leaked menu
allocations after observing them; the corrected arm does not. Two added
checks drive unregister refusal and subsequent success through the original
client body. The simulated kernel refuses with ERROR_CLASS_HAS_WINDOWS;
the client retains both menu strings until the successful call returns them.
That establishes client cleanup, not actual native live-window enforcement.

The first run failed a fixture expectation that unregister would reuse TEB
scratch: original ANSI FIRSTCOPYLPTSTRID actually aliases COPYLPSTRIDW and
forces heap capture. Corrected the test expectation from two allocations to
four across register plus two unregister attempts; no product or original
macro changed. Retained failure output is
O:/winnt/logs/s40-class-registration-original-first-failed.stdout.log.
Consequently this selected ANSI class path does not need the TEB static
scratch branch, though other callers of RtlCaptureAnsiString can do so.

Follow-up remains one production class lifetime closure: finite native
resource binding, guest module identity, shared class/clone state, real WOW
callback dispatch and direct unregister interception. Neither the local
mirror recovery nor these 18 source/host tests prove real WRITE execution,
complete WOW registration, or current product regression. No runtime package
was replaced in this step.

### Native class scope eliminates a proposed replacement registry

Re-read original kernel/class.c::_InnerGetClassPtr and GetClassPtr before
designing metadata storage. The original comparison deliberately uses only
HIWORD(hModule); its 1992 history explains that WOW instances of the same
16-bit executable share a class. GetClassPtr searches module-private first,
then process-public (CS_GLOBALCLASS), then the special/system fallbacks.
The earlier assumption that public registration must substitute a real PE
module handle was too broad and is superseded by this native experiment.

`tests/adapter-mvdm-host-out/wow/wow_native_class_scope_fixture.c` uses real
RegisterClassExA, GetClassInfoExA, CreateWindowExA, Get/SetClassLongA and
UnregisterClassA, not a simulated class backend. It supplies synthetic packed
WOW module/instance identities and native callback functions; creates only
its own invisible windows; and destroys/unregisters successful objects before
exit. Build: MSVC x86 `/MT /Gz /W4 /WX`, public user32.lib only, output
build/M0-T420/S40/class-registration-r1/native-scope.obj and native-scope.exe.

Observed properties on this Windows host:

- Same class name and module high word but another instance low word fails
  registration with ERROR_CLASS_ALREADY_EXISTS (1410).
- Another module high word registers the same class name and gets the same
  name atom but an independently selected class and callback.
- GetClassInfo and CreateWindow find the first class through the second
  instance; string and atom lookup both work. Native GWL_HINSTANCE retains
  the supplied instance, not a substituted host module.
- Class extra data is shared across windows of one module/class and isolated
  from the other module's same-name class.
- A missing private module/class fails creation with ERROR_CANNOT_FIND_WND_CLASS.
  Unregister with live windows fails with ERROR_CLASS_HAS_WINDOWS; destroying
  windows allows unregister by another instance of the same module.
- CS_GLOBALCLASS is unique across modules. A module-private same-name class
  coexists and takes precedence only for that module. Other modules reach the
  public class. After private unregister, its module finds the public class;
  public unregister also works from the other module scope.

Runtime executable O:/winnt/tests/S40USER/native-class-scope.exe SHA-256:
`3D3AA3AF18070D7B1871BB80B5D6CFE91BA1369B49E17D7D0947563A59F18F28`.
Logs O:/winnt/logs/s40-native-class-scope.stdout.log and paired empty stderr;
exit zero, `WOW_NATIVE_CLASS_SCOPE errors=0 create_a=3 create_b=2`.
An earlier harness invocation had a stray PowerShell command after the
successful executable; it was rerun with a checked process exit before
acceptance. No product or original source was changed to achieve this result.

Design consequence: retain native class name/atom/module scope and native
window-reference refusal. Do not introduce class-name mangling, host-module
substitution or a second class-resolution algorithm for these demonstrated
cases. This reduces the required new binding to missing WOW metadata and
callback/shared-data publication; it does not eliminate them. A native class
still cannot consume a tagged guest 16:16 WNDPROC as an executable host pointer.
WU32GetClassInfo tests WNDPROC_WOW to recover the guest procedure and subtract
the original eight extra class bytes, so returning a native proxy address
unchanged would break it. Registration, direct GetClassInfo/GetClassLong/
SetClassLong/unregister consumers and window callback ownership must be bound
together. System-class fallbacks, WOW-deferred destruction and guest resource
loading are not proved by this native scope fixture.

The packed identity is valid here as a class scope key, not as a PE image
containing guest resources. Original guest menu/icon loading stays with WOW32.
The ADAPTER-WOW-020 README explanation was corrected accordingly; earlier
module-substitution assumptions in this chronological evidence are superseded.
The private ABI header also now states that private word constants require
an S40 binding; it no longer claims missing storage is implemented.

### Original DispatchClientMessage enters the provider link

Source inspection identifies the original forwarding owner rather than
requiring a newly written tagged-procedure dispatcher:
windows/core/ntuser/client/client.c::DispatchClientMessage. It tests
WNDPROC_WOW, calls pfnWowWndProcEx with HWND/message/parameters/tagged
procedure and pwnd->adwWOW, or calls the native WNDPROC directly. WOW32's
W32Win16WndProcEx owns selector-tag decoding, message thunking and CallBack16;
none belongs in this native binding.

The complete DispatchClientMessage function body is imported byte-for-byte
at src/opennt-host/windows/core/ntuser/client/client.c (OPENNT-HOST-038).
The original copyright/header is retained. Its private precompiled environment
and other client API families are cropped. Mirror SHA-256:
`C6BFBCAED042AE9CC3A63964A9E464E8E1BDE913DF3802543B074D27611015D8`.
The exact retained range from `LONG DispatchClientMessage(` through its
closing brace compares equal, including CRLF, with the pinned original
client.c identified above. No executable line was changed or added inside it.

ADAPTER-WOW-026 (`wow_window_dispatch_bindings.h`) binds the source's PWND/HW,
adwWOW and callback name to a caller-provided synchronous view. It is not a
kernel WND layout or a guest object. It contains the actual native HWND,
borrowed window-owned WW pointer and exact six-argument WINAPI callback.
Original retail UserAssert is non-evaluating; no fabricated desktop delta or
host TEB field is supplied. A native dispatch does not require a guest
callback, and each invocation owns its own view so nested calls do not
overwrite global/TLS callback state. The future window gateway must retain
the record while callbacks execute, including reentrant window destruction;
this header does not itself implement kernel thread locking or lifetime.

`wow_original_window_dispatch_fixture.c` links the actual mirrored body, not
a copied implementation, under x86 `/MT /Gz /W4 /WX`. It verifies untagged
native dispatch, two distinct tagged values (forwarded unchanged), exact
HWND/message/wParam/lParam/WW address, return-bit preservation, and nested
native calls from the test WOW callback. The callback observes mutations to
the borrowed WW, not a copied three-DWORD snapshot. The callback is a test
double, not W32Win16WndProcEx/CallBack16, and HWND is a fixture identity.

Fixture build root is build/M0-T420/S40/class-registration-r1. Final link
explicitly uses `/manifest:embed /manifestuac:level='asInvoker' uiAccess='false'`.
The initial unmanifested executable was refused at launch as requiring
elevation; it was not counted as a run. No UAC setting was changed and the
fixture was not run elevated. After embedding the ordinary-user manifest,
O:/winnt/tests/S40USER/original-window-dispatch.exe exits zero:
`WOW_ORIGINAL_WINDOW_DISPATCH errors=0 guest=2 native=3`.
Executable SHA-256:
`7FCF59A029C5E73A28AAF305D8EA791B97AC0A04D8A9A1F5D93B5D990F78685D`.
Logs: O:/winnt/logs/s40-original-window-dispatch.stdout.log and empty stderr.

The formal New-T404S5Wow32ProviderNinja.ps1 graph now includes original
client.obj. Regenerating provider-stdcall-r1 and running x86 Ninja -j4 -v
completes all 87 commands; log is class-registration-r1/dispatch-provider-build.log.
The final map contains `_DispatchClientMessage@20` from client.obj.
WOW32.DLL SHA-256:
`1CD52C6AD7838F517D599F12AD35F66BA94AF87995D02DD6DAEAB80E34B9A9D8`.
Existing DEF DESCRIPTION/duplicate-export warnings remain; no new link failure.
The new DLL was not deployed. Formal link proves composition, not reachability:
no native window gateway invokes this recovered function in the product yet.
Class metadata/direct API binding, window creation and lifecycle, shared
objects, original task scheduling and actual Win16 execution remain required.

### WW storage retained across reentrant native destruction

Original DispatchClientMessage relies on a kernel-thread-locked PWND.
kernel/userk.h increments PHEAD::cLockObj; kernel/handtabl.c::ThreadUnlock1
pops the PTHREADINFO lock stack before the final HMUnlockObject may destroy
storage. handtabl.c SHA-256:
`F0B56314A93423599F06645D17104391F19A0CA07C3944853B065D8866493F7C`.
These kernel objects and destruction callbacks are not available to the worker.
Importing the complete USER kernel handle/thread manager crosses the stopping
boundary. Modern USER retains HWND lifetime ownership; only the additional
original-layout WW bytes require local callback retention. Reuse of original
WW with a finite native storage binding is the selected second-rung recovery,
not reproduction of the complete kernel locking engine.

ADAPTER-WOW-027 adds wow_window_words_binding.c and its declaration header.
A native property identifies original WW storage for a process-owned HWND;
property ownership and borrowed references control release. The SRW lock
covers only property/reference publication. Detach removes property ownership
without invalidating existing borrows. There is no current-window global,
substitute handle namespace, class resolver, guest mapping, message thunk or
scheduler. The caller must supply correct complete WW, attach before callbacks,
detach on creation rollback/destruction and release every borrow. This does
not populate missing USER state/state2 or serialize WW field updates.

wow_window_words_lifetime_fixture.c creates its own native window and invokes
the actual original DispatchClientMessage. The test WOW callback acquires a
nested reference, destroys the window, then reads/writes the same WW. Its
WM_NCDESTROY handler detaches the property. Only the final outer release frees
the storage. The actual binding is compiled with a test-only forced header
replacing HeapAlloc/HeapFree/SetPropW: separate VirtualAlloc pages become
PAGE_NOACCESS immediately on free; the property wrapper normally calls real
SetPropW. This catches premature release rather than relying on freed heap
contents remaining readable. Failure injection covers allocation and property
publication; null input, duplicate attachment, absent lookup, repeated detach
after destruction and null release/value are also checked.

Build uses x86 `/MT /Gz /W4 /WX`, src and WOW include roots, and test-only
`/FItests/adapter-mvdm-host-out/wow/wow_window_words_fixture_heap.h` plus `/I.`
for the binding fixture object. Link original client.obj, user32.lib and the
asInvoker manifest. The first compile lacked `/I.` and failed to find the
forced header; the test recipe was corrected without production include changes.
All objects remain under build/M0-T420/S40/class-registration-r1.

Runtime O:/winnt/tests/S40USER/window-words-lifetime.exe SHA-256:
`7479696B281BE7AC3CC3EE7FE451BF114E74A9F755D6417ABC0B1F422825621E`.
It exits zero, paired stderr is empty, and
O:/winnt/logs/s40-window-words-lifetime.stdout.log reports:

```text
WOW_WINDOW_WORDS_DESTROY_DURING_CALLBACK errors=0 active=1 destroyed=1
WOW_WINDOW_WORDS_POISONED_FREE allocations=3 releases=3
```

The formal graph compiles the uninstrumented binding separately and links
successfully: nine incremental Ninja commands, words-provider-build.log in
the same build root. WOW32.DLL SHA-256:
`D013AD7D12B47DE0FA1AD895E4FDB16CFDCB3EA3CB68D5ADE11E1CC3C1E217BB`.
No runtime package was replaced. This proves the native storage lifetime, not
real guest callback execution, attachment before the first creation message,
all direct private-word APIs, cross-task field synchronization or DLL unload
with windows. The native product window gateway must still connect these
operations and original dispatch as one lifecycle before provider acceptance.

### Unified window dispatch lifetime, native message and exception evidence

ADAPTER-WOW-027 now supplies wow_window_dispatch_bound: acquire the HWND's
existing WW, form the finite original dispatch view, call byte-exact
DispatchClientMessage, release in __finally. Missing storage is reported as
ERROR_NOT_FOUND; invalid output/procedure or a missing callback for a tagged
procedure fails before dispatch. The output is unchanged on pre-call failure.
Native procedures require no WOW callback. The function propagates exceptions
after releasing its borrow; it does not convert them into guest success or
invent window-procedure policy. This uses the same single WW allocation,
not an additional window-state copy or registry.

The lifecycle fixture no longer holds an outer WW reference on behalf of
the binding. It releases its preparatory inspection references, then uses
real SendMessageA -> native test WNDPROC -> wow_window_dispatch_bound ->
original DispatchClientMessage -> test WOW callback. That callback destroys
the native window; WW remains valid solely because of the binding's borrow.
A second case destroys another window then raises exception 0xe0000040;
the caller's handler sees that exact exception, the sentinel result remains
unchanged, and all allocated storage has been released. Poisoned-free pages
remain enabled. A direct native-procedure case with NULL WOW callback also
passes. Missing storage, missing tagged callback, zero procedure and NULL
result negative cases leave the output and storage ownership intact.

The existing x86 fixture build recipe remains valid. Updated runtime
O:/winnt/tests/S40USER/window-words-lifetime.exe SHA-256:
`85CB87F9C67BC5DD197BDAB9EAA24BE12DF723237A29827A23B173C36832BF9C`.
It exits zero with these markers in the existing lifetime stdout log:

```text
WOW_WINDOW_WORDS_DESTROY_DURING_CALLBACK errors=0 active=1 destroyed=2
WOW_WINDOW_WORDS_POISONED_FREE allocations=4 releases=4
WOW_WINDOW_DISPATCH_GATEWAY native_message=1 exception_cleanup=1
```

Formal x86 provider build succeeds; gateway-provider-build.log records the
incremental compilation/link. Its map contains the stdcall gateway @28,
original DispatchClientMessage @20 and the gateway's SEH cleanup table.
WOW32.DLL SHA-256:
`92049E89F0C6E433EFC345CE7AE1EC4499E7587B471B5DE2929E80A7645F5ED3`.
No new DLL was deployed and no real guest callback is claimed. Native tests
initialize WW after creating a window, so they do not establish first-message
ordering. Production class registration/creation must still publish WW
before callback dispatch and connect direct private-word APIs to this same
storage. Original rtl/getset.c returns its three DWORDs and their pointer;
kernel/getset.c mutates those fields. A future binding must not create another
copy or release a returned borrowed pointer across an unexamined callback
boundary. Full USER shared-object publication and scheduling also remain open.

### Creation-time WW publication composed with original dispatch

The creation-order fixture now links the uninstrumented production
wow_window_words_binding.c and original client.c DispatchClientMessage.
Its test-only CBT hook publishes the complete WW before creation messages;
the native WNDPROC enters wow_window_dispatch_bound, and a test callback
checks all 32 WW bytes against the expected creation record. WM_NCDESTROY
detaches that same production storage. Nested creation uses distinct records,
without consuming GWLP_USERDATA or replacing CREATESTRUCT.lpCreateParams.

The input remains exactly three DWORDs at a readable page's end, followed
by PAGE_NOACCESS. This verifies copying only the original 12-byte creation
prefix, not reading a complete WW from the caller. The fixture covers nested
creation, WM_NCCREATE rejection, CBT veto both before and after publication,
and test-record allocation/property failure. These failures do not replace
the separate production-allocation and poisoned-free lifetime tests above.

Build: x86 `/MT /Gz /W4 /WX`, source and WOW include roots, clean
words-binding-native.obj plus original client.obj, user32.lib, and an
embedded asInvoker manifest. Build output is
build/M0-T420/S40/class-registration-r1/precreate.exe. The isolated runtime
O:/winnt/tests/S40USER/precreate-bound.exe has SHA-256
`FB050388F1978465336729B6DDED624DFAB2F9633DA5BFEC3495640408E8E22B`.
It exits zero; O:/winnt/logs/s40-precreate-bound.stderr.log is empty and
the paired stdout log reports:

```text
created=4 destroyed=4 live=0 minmax=3 failures=0
vetoed=2 rollback=0 allocations=5 frees=5 unbound_destroy=3
WOW_PUBLIC_PRECREATE_NESTED_AND_FAILURE_ORDER_OK
WOW_PUBLIC_PRECREATE_VETO_AND_RESOURCE_FAILURE_OK
WOW_ORIGINAL_WW_PREFIX_GUARD_OK input=12 view=32
WOW_PRECREATE_BOUND_ORIGINAL_DISPATCH_OK callbacks=18
```

This closes the first-message ordering gap for this composed native fixture,
not for the product creator. In particular, matching the test's unique
lpCreateParams cannot identify arbitrary guest creates, NULL parameters or
foreign reentrant creates. No production hook or output-table slot is
installed by this test, no runtime DLL is replaced, and no guest executes.
The actual creator must also preserve the original _CreateWindowEx contract:
version/exstyle checks, MDI pre/post processing, ordinal names and
CW_FLAGS_DIFFHMOD. Original ntstubs.c performs MDI preprocessing before
NtUserCreateWindowEx; mapping that syscall blindly to public CreateWindowEx
would risk applying public MDI processing twice. The native test's non-MDI
success does not validate that mapping. Class query/set/unregister coherence,
private-word APIs, shared-object publication and scheduling remain required.

### Original dialog initialization and dispatch use one WW allocation

The existing wow_original_dialog_init_fixture.c now uses an invisible native
STATIC HWND and production wow_window_words_binding instead of a sentinel
HWND and independent static DWORD array. It still links the actual original
W32InitDlg from wcall32.obj; no initializer body was copied or rewritten.
The fixture holds an explicit synchronous borrow across the original call.
Its two private import seams read that allocation and acquire/write/release
the same allocation, respectively. Afterwards wow_window_dispatch_bound and
original DispatchClientMessage pass that identical WW address to the test
callback, which observes the class/state and dialog procedure written by the
original initializer. All six untouched DWORDs and the returned user parameter
retain their original values. Null parameter causes no private calls; removing
the binding makes the original missing-WW case return zero without writes.

The reused original object SHA-256 remains
`53B80EED7084E9B56AFD6689FC7ECE135E9CA2AD8EF52BE68E32AB158847C324`,
and current wcall32.c remains
`0F72AE39E691F3EF672DF4F1AEB3EBB6443E87F091FB6322EF1C13604AB3D571`.
Compile the fixture x86 `/MT /W4 /WX` with source and WOW include roots;
link wcall32.obj, clean words-binding-native.obj, client.obj and user32.lib,
retaining the prior test-only private-import and hard-failure aliases,
`/OPT:REF` and the asInvoker manifest. The bound-dialog.map under
build/M0-T420/S40/original-dialog-init-r1 records original W32InitDlg@8,
original DispatchClientMessage@20 and the production dispatch gateway@28.

Runtime O:/winnt/tests/S40USER/bound-dialog.exe SHA-256:
`8756573CE11954017514155A2CFF025A0E2C35C19C0928B2A51E6F734A4F456B`.
Exit zero, empty O:/winnt/logs/s40-bound-dialog.stderr.log, paired stdout:

```text
WOW_ORIGINAL_W32INITDLG_TWO_FIELD_CONTRACT_OK
WOW_ORIGINAL_DIALOG_INIT_AND_DISPATCH_SAME_STORAGE_OK
```

This is native storage integration, not a real dialog or guest callback pass.
The fixture owns the raw-pointer borrow explicitly; a general product
GetWindowLong(GWL_WOWWORDS) cannot acquire/release internally and return an
unprotected pointer. Source recheck confirms additional users outside the
window-dispatch gateway: FindPWW callers, dialog callbacks, message thunks,
and the CBT create hook. In particular wowhooks.c reads WW before WM32Create,
while original kernel/createw.c copies adwWOW before invoking HCBT_CREATEWND.
Therefore publication only before the first WNDPROC message is insufficient
for guest hooks. Likewise the WW tail aliases live style/exstyle/instance,
not a permanent creation snapshot. These remain implementation obligations;
the bounded storage test does not authorize a temporary raw-pointer getter,
zeroed native flags, duplicate class storage, or installing a partial provider.

### Dispatch refreshes the native-owned WW fields

Original wowuserp.h defines WW's tail as aliases of WND state/state2,
ExStyle/style/hModule. Original rtl/getset.c::GetWindowData exposes the latter
three through GWL_EXSTYLE, GWL_STYLE and GWL_HINSTANCE. Original createw.c
assigns the supplied hInstance directly to pwnd->hModule before CBT dispatch.
The selected wuwind.c creation call supplies HMODINST32 and CW_FLAGS_ANSI;
it does not request CW_FLAGS_DIFFHMOD. The latter appears in original DDEML
creation, not this direct WOW caller. It must not be used to invent a separate
guest instance translation for this path or block it on unrelated DDEML work.

ADAPTER-WOW-027 now refreshes the three native-owned attributes before calling
original DispatchClientMessage. GetWindowInfo and GetWindowLongA provide the
same native HWND's attributes; all three queries must succeed before any
field is changed. The existing finally releases the borrow on query failure,
normal return or exception. The three WOW DWORDs and private state/state2
are preserved. Original getters directly require mapped PWND/server fields;
that unavailable mapping is why these native attribute reads belong in the
existing storage adapter, not a newly invented mirror algorithm or USER
server import. No mirror source changed for this correction.

This is dispatch-entry sampling, not continuous shared-memory equivalence.
Direct private getters, native updates during an outstanding call, hook
publication, scheduler serialization and guest WND projection still require
their complete bindings. The two private state DWORDs are not fabricated
from unrelated modern flags. The creation fixture now compares the native
tail with the actual HWND, rather than requiring its deliberately distinct
test-only instance to overwrite native hModule semantics.

The poisoned-free fixture changes native style to include WS_DISABLED,
exstyle to include WS_EX_TOOLWINDOW and instance to 0x23455678 after binding.
It first verifies those changes actually took effect, then checks that the
original dispatch sees them. Query-failure injection separately fails
GetWindowInfo and GWL_HINSTANCE: no callback, unchanged output and unchanged
complete WW; eventual exact free counts prove both failed calls released
their borrows. Callback destruction and raised-exception cleanup still pass.
Creation ordering and the actual W32InitDlg/storage/dispatch integration also
pass. All three are x86 /MT fixtures, not guest acceptance.

Final runtime identities under O:/winnt/tests/S40USER:

| File | SHA-256 |
| --- | --- |
| live-words.exe | EE4B42E4A166DB55DA44DCE35B7871598880D8ADE4E4ABDDBA943750A2632ED9 |
| live-precreate.exe | 99DF030C5278D766D39A46CFEDD781736D57C44712260D6B8A387C78D1B2520F |
| live-dialog.exe | 20F17D9D1B5DA0A8BFE1C23836473E6E39512D7B66DC3C6B379A85B4E2B4E201 |

Each exits zero with empty stderr. Logs are
O:/winnt/logs/s40-live-{words,precreate,dialog}.{stdout,stderr}.log.
The new marker is WOW_WINDOW_DISPATCH_LIVE_NATIVE_FIELDS_OK; existing
destruction, exception, guarded-prefix, nested-create and original-dialog
markers remain passing. Build outputs stay under the existing S40 fixture roots.

The first formal Ninja invocation remained alive with no child compiler,
empty output and unchanged CPU. Its exact PID/command/parent were checked;
only that owned build was deliberately terminated (exit -1), not reported as
passed or restarted on an observation timeout. The authorized-environment
retry completed the formal compile/link in two steps. Evidence:
build/M0-T420/S40/class-registration-r1/live-fields-provider-build-retry.log.
Formal provider SHA-256:
`BF222DAE25CB76B5D7E73F5A828787E71E75991333020600AE08C7229D910FFA`.
Only the previously recorded DESCRIPTION/duplicate-export linker warnings
remain. No formal product runtime package was replaced, no guest was changed,
and no new real-Win16 or 17-route product pass is claimed by these fixtures.

### Class owner identity is not the GetClassInfo instance or mutable style

Source review of original kernel/class.c registration, _GetClassInfoEx,
_InnerGetClassPtr and GetClassPtr establishes two distinct contracts. Lookup
uses private/public list membership and module HIWORD. However, the version
4.0 GetClassInfo branch returns the caller's hModule (or NULL for hModClient),
not necessarily pcls->hModule; the older branch can return the class module.
Therefore public GetClassInfoEx's hInstance cannot identify the selected
registration's WC. This is a caller-version compatibility rule, not a missing
class resolver in modern USER. A product class query must also retain the
original guest-version result rule rather than assuming the native process's
version equals the Win16 task's version.

The extended native class-scope fixture verifies the current host:
querying private module 2345 with instance 5678 returns 23455678, although
registration used 23451234. Querying the public class through 45671234
returns 45671234, again not the registration owner. For actual windows,
GCL_HMODULE returns the registered 23451234. The test then registers both
a public and a private class with the same name AND same module. They share
the class atom and GCL_HMODULE but retain distinct class-extra values.
Changing the private class's public GCL_STYLE to include CS_GLOBALCLASS
does not move it into the public lookup list: outside-module lookup still
selects the original public class, and same-module lookup the private one.
After private unregistration, lookup correctly exposes the public class.

Consequently atom+module alone, GetClassInfo's returned hInstance, and the
current mutable CS_GLOBALCLASS bit are each insufficient metadata keys.
Class-wide WC needs registration identity and original registration scope,
shared by all windows until successful original/native unregistration or
the admitted task-cleanup route. Re-registration must not reuse a stale WC.
This evidence constrains the pending binding; it does not introduce a parallel
class policy or claim that such a record/cleanup implementation already exists.

The two extra DWORDs in WU32RegisterClass are already owned by original WOW:
one saves a guest's attempted cbClsExtra modification and one flags it.
WU32Get/SetClassLong and Get/SetClassWord read/write those trailing bytes.
They are not free storage for a WC pointer; reusing them would corrupt the
original compatibility behavior. The original WC pair is separately owned
by CLS.adwWOW. No mirror change was needed to establish this distinction.

The same x86 /MT /Gz /W4 /WX native fixture rebuilds under
build/M0-T420/S40/class-registration-r1 with user32.lib and asInvoker manifest.
O:/winnt/tests/S40USER/native-class-owner.exe SHA-256:
`E686CCE1C04DCEBBCC68BA46C43F3F280ED14B9C7B0C8EE0B0B3E4A63B2D7264`.
Exit zero; s40-native-class-owner.stderr.log is empty. The paired stdout in
O:/winnt/logs reports both WOW_NATIVE_CLASS_QUERY_INSTANCE_IS_NOT_OWNER_OK
and WOW_NATIVE_CLASS_SAME_MODULE_PUBLIC_PRIVATE_DISTINCT_OK, with
WOW_NATIVE_CLASS_SCOPE errors=0 create_a=4 create_b=3. All test windows and
classes are destroyed/unregistered. This is native boundary evidence, not a
Win16 class-registration pass or deployment of a partial USER table.

### Original class client composed with real native registration and teardown

Extended the existing class-registration fixture with a native-backend arm.
The original RegisterClassExWOW/UnregisterClass bodies and capture/conversion
remain selected. Their unavailable NtUserRegisterClassExWOW and
NtUserUnregisterClass operations now call real RegisterClassExA and
UnregisterClassA for this arm, while retaining the client's menu allocation
ownership until successful unregistration. The controlled version/TEB/GDI
environment and earlier failure cases remain unchanged. This is not a product
NtUser provider: it does not publish the supplied WC, accept a guest tagged
WNDPROC, reproduce fnid handling, or implement a multi-class record table.

The new arm registers the native DefWindowProcA class with packed module
23451234 and an allocated menu name. A duplicate actual registration fails
with ERROR_CLASS_ALREADY_EXISTS and releases only the duplicate attempt's
two menu allocations. A real invisible child window prevents unregistration
with ERROR_CLASS_HAS_WINDOWS; the original client retains the live class's
menu buffers. Destroying the child permits unregistration, after which the
original client frees those two buffers. Both capture and menu counters end
balanced. This demonstrates that the original client's successful/failed
native lifecycle can compose without another locally invented cleanup policy.

Re-ran the pinned extraction/mirror check:
WOW_CLASS_MIRROR_SOURCE_MATCH correction=2. Built both original-native and
corrected-native x86 /MT /Gz /W4 /WX executables using the existing recipe,
original chartran/RTL binding objects, user32.lib, ntdll.lib and asInvoker
manifest. The corrected arm includes actual mirror ntcftxt.h. The unchanged
source arm still demonstrates and explicitly rescues the known two-allocation
original capture-failure leak; the corrected arm requires zero without rescue.
Both pass all earlier cases and the new real-native lifecycle, with markers:

```text
WOW_ORIGINAL_CLASS_CLIENT_NATIVE_LIFECYCLE_OK
WOW_ORIGINAL_CLASS_REGISTRATION errors=0 cases=19 captures=0 menus=14/14
```

Both exit zero and paired stderr logs are empty. Runtime files under
O:/winnt/tests/S40USER and SHA-256:

| File | SHA-256 |
| --- | --- |
| class-original-native.exe | 5518781B90A96A371D9AAE2B0C763DBC87ABFE511278E19AEDC8CEF6858826D4 |
| class-corrected-native.exe | 73F3DA09810EBC88065E0C2CA5BAB04797A49E586076C73D9558502EDDD1C222 |

Logs: O:/winnt/logs/s40-class-{original,corrected}-native.{stdout,stderr}.log.
Build outputs stay in build/M0-T420/S40/class-registration-r1. No product
source, formal package or guest changed in this extension. Installing the
registration slot still requires shared WC ownership, coherent direct class
queries/setters/unregistration, guest/native procedure identity, and actual
WOW task lifetime; native lifecycle success does not waive those obligations.

### Original class lookup owner recovered without a USER server

The class identity tests require original list membership, not a new policy
keyed on queried hInstance or mutable class style. Recovery ladder review:
full kernel/class.c cannot compile standalone because its other operations
own desktop heaps, kernel process/thread objects, locks and handle destruction.
Those are not needed by _InnerGetClassPtr/GetClassPtr: the complete outgoing
closure is two list traversals and HIWORD comparisons. Reusing those exact
functions with a finite declaration view is the second-rung choice. No
external-source patch or autonomous lookup algorithm is needed. The package
interface stopping rule excludes the USER service engine, not an independently
composable metadata lookup; no recursive server import is admitted here.

OPENNT-HOST-039 imports those two functions only at their real original path,
src/opennt-host/windows/core/ntuser/kernel/class.c. Original SHA-256:
`82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A`.
Mirror subset SHA-256:
`FD2A69EE3CE88856DDE0DDADD5D47778F537411134FBC06903FC43309E4E5122`.
The original notice is retained; the selected bodies, comments and CRLF are
byte-exact. The existing pinned class extractor now verifies this explicitly
alongside the two-line client cleanup correction. No existing source owner
was overwritten, no mirror algorithm was changed, and no guest was modified.

ADAPTER-WOW-028 is a declaration-only list view. It preserves the leading
next link required by the original `(PPCLS)*ppcls` traversal and exposes atom,
module and deferred-destroy flag. An explicit caller context supplies private,
public, system and client-module roots instead of kernel process globals.
This is not a kernel CLS/PROCESSINFO layout and cannot be exported as the
guest PMODE32 object graph. The eventual registration binding must own record
population, locks, lifetime and class WC fields; lookup does none of that.
Returned pointer-to-pointer links are valid only while that ownership holds.

The x86 /MT /Gz /W4 /WX fixture links the actual mirror object. Fourteen checks
cover zero/missing atom, module-HIWORD instance sharing, private-before-public,
public-before-system, deferred-destroy exclusion, client-module fallback,
later-list matches and the actual link returned for removal. It exits zero:
WOW_ORIGINAL_CLASS_LOOKUP errors=0 checks=14. Runtime
O:/winnt/tests/S40USER/original-class-lookup.exe SHA-256:
`917059249DA67290B6BA07B411F78AC716979193C628FCCEBC8EB21EA874D4B3`.
Logs: O:/winnt/logs/s40-original-class-lookup.{stdout,stderr}.log; stderr empty.

The formal provider graph now compiles obj/opennt-user/class.obj from this
mirror and links successfully. Log: build/M0-T420/S40/class-registration-r1/
class-lookup-provider-build.log. WOW32.DLL SHA-256:
`6EA101D22FDF1FF67601BA938B017CFB937E793D5FE3949E1B4820DA3D385BB2`.
The known linker warnings remain unchanged. This is original-source and
formal-build recovery, not runtime provider completion: no product class
context is populated, no output-table slot is installed, and no real Win16
registration/WRITE acceptance is claimed. The native class-scope evidence
and exact-original lookup test constrain the same pending shared WC binding.

### Class atom namespaces: original owner and native isolation

The pending WC binding must resolve class identity without assuming that a
Win32 local/global atom is a USER class atom. Original kernel/userk.h maps
FindAtomW to UserFindAtom before defining FindClassAtom. Kernel/atom.c uses
its own UserAtomTableHandle for lookup/add/delete; base/win32/client/dllatom.c
uses BaseLocalAtomTable for public local-atom APIs. These are different
original owners, not a modern loss of a previously shared atom namespace.
Pinned OpenNT evidence (not imported runtime dependencies):

| Source | SHA-256 |
| --- | --- |
| windows/core/ntuser/kernel/atom.c | FFE5FC2133AF083C146926E6BF41E1EBEEEE4AA314FEE518EE77147550959A9F |
| windows/core/ntuser/kernel/userk.h | 9260E0BA3CC89974E10B3804ED66D18B9864F4AE7D1175CCE6DDE8EE79D66E8C |
| base/win32/client/dllatom.c | 51C16938C73A2CFD6F54C088FD683825E89EFA99AA809C932AC369550D03417C |

The native class-scope fixture now adds and deletes the same class name in
both public atom tables while the native class remains registered. Class
lookup by native atom and by name still selects the original procedure;
deleting those public atoms does not unregister or damage the class. It
deliberately does not require numeric atom inequality: independent tables
can allocate equal numbers. Earlier public/private/module ownership tests
remain enabled. Formal x86 /MT /Gz /W4 /WX compilation and runtime exit zero:

```text
WOW_NATIVE_CLASS_ATOM_NAMESPACE_ISOLATION_OK
WOW_NATIVE_CLASS_SCOPE errors=0 create_a=4 create_b=3
```

Runtime: O:/winnt/tests/S40USER/native-class-atom-isolation.exe; SHA-256
`B68D4382978B1394E091DFE0C04D4314EB2F204DDB7CA3DD92DD07CABDA63807`.
Logs: O:/winnt/logs/s40-native-class-atom-isolation.{stdout,stderr}.log;
stderr empty. Build artifacts remain in the S40 class-registration-r1 root.
GetClassInfoExA returned 50128, equal to the registered atom on this run,
but its public BOOL contract alone is not a portable atom-query guarantee.

Consequence: do not bind original FindClassAtom to public FindAtomA/W or
invent an interchangeable namespace. WU32RegisterClass/GetClassInfo/
UnregisterClass also contain original GetAtomName conversions for atom-form
inputs; that caller contract requires separate end-to-end evidence before
claiming arbitrary guest atom input support. This test neither installs WC
registration in the provider nor proves a real Win16 task. No product or
guest semantics changed; complete class publication remains pending.

### Shared WC storage and native lifetime

ADAPTER-WOW-030 supplies native storage for the two original CLS.adwWOW
DWORDs (walias.h's WC). Original class.c remains the lookup owner; its desktop
allocation/lock engine is unavailable without importing the USER server.
The finite heap/refcount binding preserves its head insertion and delegates
selection to original GetClassPtr. It creates no class policy, atom namespace,
guest graph or per-window WC duplicate. Existing WOW extra-class bytes already
implement the original legacy-size hack and cannot be repurposed for this.

Prepare precedes native registration, failure releases the prepared record,
and success publishes it. Successful native unregister permits detaching the
list reference; failure must retain it. Existing readers keep the same words
until their final release. The caller owns publication/write serialization;
all context entries must be owned bindings. Source audit additionally proves
_UnregisterClass only checks private/public process lists, not GetClassPtr's
later client-module/system query fallback. Do not use that broader query
scope to decide which record to unregister.

The x86 /MT /Gz /W4 /WX fixture uses real native class registration and HWND
creation. Same-module instances borrow one WC; same-name public/private
classes retain independent values. Live-window unregister fails with 1412
without losing WC; successful unregister unlinks it while an outstanding
reader remains valid, and new lookup selects the public class. Final native
unregister and release leave both lists empty. Exit zero, empty stderr:
`WOW_CLASS_WORDS_NATIVE_LIFETIME errors=0`.

Runtime O:/winnt/tests/S40USER/class-words-lifetime.exe SHA-256:
`9D2816361A64554B6D99AC1BCDDDA72CD28205DF10A4BBFC2EDC7B3F7A8F03C7`.
Logs: O:/winnt/logs/s40-class-words-lifetime.{stdout,stderr}.log.
Formal provider now selects the binding and links successfully; log:
build/M0-T420/S40/class-registration-r1/class-words-provider-build.log.
WOW32.DLL SHA-256:
`A96FBC0181BF5BF9A68D8E3E4FDC9B43C118C170B9E2CAC1661E9E50F9217DF3`.
Existing linker warnings are unchanged. No runtime package was deployed.

Name resolution, original client capture/registration integration, task
lifetime, native procedure bridge, coherent setters and safe raw-WC call
epochs remain required. This is native lifetime plus original lookup evidence,
not Win16 registration/WRITE acceptance, mirror-diff reduction or S40 closure.

### Original class client composed with shared WC binding

The class-registration fixture now combines the actual ADAPTER-WOW-030
object and byte-exact original class lookup with the original client capture,
RegisterClassExWOW/UnregisterClass bodies and real native registration.
This replaces the native fixture arm's previous omission of class words;
it does not replace its controlled thread/version providers with real WOW.

The native seam prepares WC before RegisterClassExA, releases it on duplicate
failure while preserving the native error, and publishes only on success.
Its fixed test class name uses the recorded registration atom; it is explicitly
not a production class-name resolver. Unregister selects only original private/
public lists, performs native unregister, and detaches only after success.
The original client still owns captured-string and menu-string cleanup.

New checks prove the two input WC DWORDs survive registration, a duplicate
registration neither replaces the shared record nor leaks captured/menu
strings, live-window refusal retains both menu allocations, and successful
unregister leaves empty class lists while a prior borrow remains valid until
release. Both unchanged-original and corrected-mirror arms pass all 19 cases:

```text
WOW_ORIGINAL_CLASS_CLIENT_SHARED_WC_OK
WOW_ORIGINAL_CLASS_CLIENT_NATIVE_LIFECYCLE_OK
WOW_ORIGINAL_CLASS_REGISTRATION errors=0 cases=19 captures=0 menus=14/14
```

The unchanged arm still observes and test-rescues the previously documented
two-allocation capture-failure leak; the corrected mirror requires zero retained
allocations on that path without rescue. No new original-source change was
made. The source extractor again verifies byte-exact lookup and only the
registered two client cleanup lines.

Both compile x86 /MT /Gz /W4 /WX and run with exit zero and empty stderr.
Files under O:/winnt/tests/S40USER:

| Artifact | SHA-256 |
| --- | --- |
| client-wc-original.exe | D403DAA156615E9825A76B004AE1A6D36A10A7FF4E2601ADFCD133FEDE3F08DE |
| client-wc-corrected.exe | F0B2EF141E4EF3D884CEDBFDE2C8531641F7C7FDEA2CA833E3052446CE045963 |

Logs: O:/winnt/logs/s40-client-wc-{original,corrected}.{stdout,stderr}.log.
Build files remain in S40/class-registration-r1. These executions do not
validate task/version lookup, guest callbacks, production name resolution or
WRITE. No incomplete output table or formal runtime package was deployed.

### Native class transactions moved into the formal binding

The preceding test-only register/unregister sequencing is now removed from
the fixture and implemented once by ADAPTER-WOW-030. Its public interface is
register, unregister, acquire, value and release; prepare/publish/detach are
private. Register preallocates shared WC, calls RegisterClassExA, publishes
only on success and preserves native failure across allocation cleanup.
Unregister uses original _InnerGetClassPtr's private/public selection, calls
native UnregisterClassA by the known class atom, and detaches only on success.
Query-only system/client fallback is not used for removal. The caller still
serializes transactions and supplies the original-client-validated class with
a callable native procedure and resolved nonzero module; NULL-module fixup
remains in the original client, not a second policy in this binding.

Both client fixture arms now call these actual product transactions, retaining
only their explicitly test-controlled class-name/atom association, version,
thread and menu-ownership transport. The lifetime fixture also no longer
manually publishes or unlinks records. All three x86 /MT /Gz /W4 /WX builds
and runtime executions pass with exit zero and empty stderr, preserving the
earlier lifetime marker and both 19-case client markers.

Files under O:/winnt/tests/S40USER and SHA-256:

| Artifact | SHA-256 |
| --- | --- |
| class-transaction.exe | FBE7D9A42544E0B3CA3E3225861B566514D626A348EB733F461B2514A9461BA6 |
| client-txn-original.exe | 25C26E8CC538639A2B6685AFD97EE63753915314FCF5037AF1EF303FFA0E5637 |
| client-txn-corrected.exe | A0EA4064CC50387CA17253B01505FC61C688292BCCA1223AF1164408093D2AA0 |

Paired logs use O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log.
The formal x86 provider also rebuilds successfully; log:
build/M0-T420/S40/class-registration-r1/class-transaction-provider-build.log.
WOW32.DLL SHA-256:
`BA6C0CD2F20D72A7629BF54FD09BDEC5FE34760545FBB21D100368677FA7C3E0`.
Known linker warnings are unchanged. Original mirror bodies did not change.
This advances product-side native transaction ownership but does not install
the full WOW output table or solve the remaining guest/task/name/procedure
bindings. No real-Win16 or product-regression acceptance is inferred.

### Captured class name to native atom binding

ADAPTER-WOW-030 now copies the original client-captured Unicode class name
into the shared record's single allocation. The caller must supply the capture
of the same name passed to native registration. No independent atom ID is
allocated. A name lookup returns only the corresponding native registration
atom; original GetClassPtr still selects the class by module/list policy.
Integer identity inputs are returned without dereferencing their low pointer.
Malformed odd lengths or lengths exceeding MaximumLength are rejected before
copy/comparison. This finite cache does not discover unregistered native or
system classes; their original population remains a separate obligation.

Source rationale: original kernel/userk.h routes class names through the USER
atom table. base/ntos/rtl/atom.c::RtlpHashStringToAtom uses Unicode uppercase
hashing and length/case-insensitive name comparison. The binding uses native
RtlEqualUnicodeString with case-insensitive TRUE, not Win32 local/global atoms
or a new CRT-locale-dependent string implementation. It is a finite native
metadata association, not an import of the RTL hash-table algorithm or proof
of every historical Unicode/collation corner case.

The registration fixture no longer retains a global known atom to unregister:
it passes the real original capture to register/find_atom and uses the actual
product transaction. Both original and corrected client arms retain all 19
passing cases. The lifetime fixture mutates the caller's old name buffer after
registration, queries a case variant, queries by integer atom, tests a missing
name and confirms the name disappears after final unregister. All three
x86 /MT /Gz /W4 /WX builds and runs exit zero with empty stderr.

Files under O:/winnt/tests/S40USER and SHA-256:

| Artifact | SHA-256 |
| --- | --- |
| class-name.exe | 99A79FC34F2F8CC452A148AE77EB433A38A060BEAC07903811AD8B3D2C05C8E6 |
| client-name-original.exe | 05735CF8404AA9C088C6C2E5C22D5EFF7EC69EA6E864E83F071BA24C57CB09BA |
| client-name-corrected.exe | 242F07D3ECBEFED36DF0FC971AA17E7DE68585F5B3DD4B4C69607B37B0959E20 |

Logs: O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log.
Formal compilation first exposed a conflicting duplicate declaration, then
a missing decorated import. The adapter now uses the existing original RTL
declaration when available. The existing build-only NTDLL alias/import list
adds RtlEqualUnicodeString@12, with no wrapper body. Final formal build passes;
map and dumpbin imports prove the function resolves to NTDLL.DLL, not the
project's separate RTL compatibility implementation. Build log:
build/M0-T420/S40/class-registration-r1/class-name-provider-build.log.
WOW32.DLL SHA-256:
`09C4600BAC2B60EA060A6CB7A780700915545618625FA3BBB83FA7ADC2A53204`.
Prior linker warnings are unchanged. No original mirror body or guest media
changed. Formal table installation, native/guest procedure conversion and real
task/version/Win16 acceptance remain incomplete; no runtime package deployed.

### Window storage retains its shared class record

Original kernel/createw.c assigns pwnd->pcls, calls ReferenceClass, then
obtains the possibly cloned class from the window before exposing callbacks.
Its destruction path calls DereferenceClass. The native binding now expresses
only that storage-lifetime edge: attach_class transfers one acquired WC
reference on success, leaves it with the caller on failure, and releases it
when the last borrowed window record is released. Access through the window
returns the same WC pointer, not a per-window copy. No native class reference
count or destruction policy is replaced, and the original lookup stays intact.

The new real-window fixture registers one native class through the product
transaction, creates two windows and attaches both to the same class record.
Duplicate property publication fails without consuming its supplied borrow.
Both windows observe each other's WC updates. Original DispatchClientMessage
then invokes a test callback which destroys both HWNDs, unregisters the native
class and still reads/writes WC through the in-flight window borrow. It returns
normally; the final caller release ends the remaining storage lifetime.
This explicitly tests a supplied callback, not execution of guest instructions.
It attaches after creation, so it does not prove before-first-callback ordering.

Native test exits zero, stderr empty:
`WOW_WINDOW_CLASS_SHARED_LIFETIME errors=0 callbacks=1`.
O:/winnt/tests/S40USER/window-class-lifetime.exe SHA-256:
`121EDD9BE4C2BDBBF1DD2060D11115CDE31F16F9A844ED0A74D6F5A78B9DDB91`.
Logs: O:/winnt/logs/s40-window-class-lifetime.{stdout,stderr}.log.

The previous WW failure/reentrancy fixture was recompiled with the new binding
and its class dependencies. Allocation/property failures, poisoned freed pages,
live native field refresh, native dispatch and exception cleanup still pass:
errors=0, active=1, destroyed=2, allocations=4, releases=4, exception_cleanup=1.
O:/winnt/tests/S40USER/ww-class-regression.exe SHA-256:
`6F2F377A0E97164120B2967E07BEAFC019106F130FC298E4C09E7278A3C258F8`.
Logs: O:/winnt/logs/s40-ww-class-regression.{stdout,stderr}.log; exit zero,
stderr empty. Both builds use x86 /MT /Gz /W4 /WX.

Formal WOW32 compilation/link succeeds with unchanged known warnings; log:
build/M0-T420/S40/class-registration-r1/window-class-provider-build.log.
WOW32.DLL SHA-256:
`09A5983289BB73B677A98619E41500FCA754C29869A2364D97CBAA72D5826FA8`.
No original mirror source changed or formal runtime package was deployed.
Actual creation ordering, task bindings and real Win16 execution remain open;
this does not extend the previous DOS regression claim to this DLL hash.

### Pre-create binding uses shared WC and preserves application parameters

The creation-order fixture previously assigned a deliberately different WOW
instance from the real native instance. That was a storage-discrimination test,
not acceptance of original createw.c's actual hInstance semantics. The fixture
now copies CREATESTRUCT.hInstance and verifies it equals the native creation
request. Registration uses the product class transaction; the CBT hook acquires
the actual WC and transfers it through attach_class before any WNDPROC message.
Every test callback verifies the shared WC, including first min/max and nested
creation messages. Failure releases an untransferred borrow; window destruction
releases the attached class borrow. Final unregister leaves empty class lists.

The follow-up removes the private FRAME pointer from lpCreateParams entirely.
Every attempt passes the same opaque application value 0x12345678 unchanged.
A pending native frame claims only one matching class-atom/instance/parameter
creation notification before invoking downstream hooks. It does not replace,
dereference or require uniqueness of application data; nested admitted attempts
push their own frame. This is still a test-owned hook and pending-frame stack,
not a installed product creator or a complete proof against every foreign hook.

Both x86 /MT /Gz /W4 /WX versions pass, exit zero, stderr empty:
created=4, destroyed=4, live=0, minmax=3, failures=0; vetoed=2,
allocations=5/frees=5, callbacks=18. Guarded 12-byte original input remains
intact. The final version additionally emits
`WOW_PRECREATE_OPAQUE_APPLICATION_PARAMETER_PRESERVED_OK`.

Runtime files under O:/winnt/tests/S40USER:

| Artifact | SHA-256 |
| --- | --- |
| precreate-class.exe | C086FEBC6E5DEF6EC7BA466AB8115784AE23BF48C5BCA2093F9911FDC58CDA24 |
| precreate-opaque.exe | B330C0799103C3AF5DC4F4145E234D0266D452460E369BA025211D2A2619FF77 |

Logs: O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log.
This validates the storage/class/source-dispatch composition before native
messages and narrows the creator binding needed next. It does not prove real
WOW task state, Win16 callbacks or WRITE, and makes no runtime-package change.

### Formal native creator and identity-safe rollback

ADAPTER-WOW-031 now moves creation publication into product code rather than
leaving it in the observation hook. Original createw.c's private object/desktop
allocation and thread locks cannot compose without the USER server; public
CreateWindowExA supplies actual creation and failure policy. A thread-scoped
CBT hook supplies only the missing WW/WC publication before downstream hooks
and WNDPROC. It claims the selected atom/instance/unchanged parameter once,
supports reentrant scopes, and restores/unhooks/releases in a finally block.
The complete initialized WW is a typed input; this helper never treats the
original three-DWORD lpWOW carrier as a full WW. The composition owner still
must supply task-derived state, native procedure conversion and original
client/MDI policy. This is not installation of the full CsCreateWindowEx slot.

Creation retains a borrow of its published window record. Failure rollback
uses new detach_if to compare that identity under the publication lock; it
cannot remove a replacement property merely because the HWND matches. Holding
the old borrow prevents its address being recycled into a replacement record.
The existing unconditional detach remains the terminal WM_NCDESTROY operation.

The new fixture invokes the actual product creator, actual native class
transaction and byte-exact original DispatchClientMessage. Its callback is a
test function, not Win16 execution. On this host it verifies metadata before
first messages, nested creation, unchanged opaque lpCreateParams, NCCREATE
rejection, downstream CBT veto and successful creation after veto. Result:

```text
WOW_NATIVE_CREATION_BINDING errors=0 callbacks=25 nested=1 rejected=1 vetoed=1 destroyed=5
```

The shared-window fixture additionally replaces a publication on the same
live HWND while retaining the old borrow. Old-token detach leaves the new
binding intact; both old/new views remain valid until released. Its existing
shared-WC and destroy/unregister-during-callback checks also pass.

Both x86 /MT /Gz /W4 /WX runs exit zero with empty stderr. Runtime files under
O:/winnt/tests/S40USER:

| Artifact | SHA-256 |
| --- | --- |
| native-creation.exe | 0BE4D5B73EBE1BC3F3E2C05F74D4563C0C5C51FC07BD914EF7C54F375DBC7D99 |
| window-identity.exe | 0D9B38C3ECCD8AA6F6DFE5D9849DF882028D4C0CF79F1CF93FD574BCBB83B753 |

Logs: O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log.
Formal x86 WOW32 selects the creator and links successfully; log:
build/M0-T420/S40/class-registration-r1/native-creation-provider-build.log.
WOW32.DLL SHA-256:
`7A3C4D71B7C42DE21A31BC3A63398D2E92FD0735BF10598C6EDA7E65AE5A3706`.
Known linker warnings remain unchanged; original mirror and guest media are
unchanged. No runtime package was deployed. Full task/guest/USER integration
and real Win16 acceptance remain open, including hooks outside this fixture's
controlled native creation scope. Neither S40 nor T420 is closed.

### Original window-procedure decoder and dispatch chain

Before adding a native procedure gateway, the existing semantic owner was
tested directly. Current src/mvdm/wow32/wmdisp32.c is byte-identical to pinned
OpenNT/base/mvdm/wow32/wmdisp32.c; both SHA-256:
`4A9004C1F57AF28280D0176AFD72CAE76A7FBAE90FFB758426C287435E5912D4`.
The new extractor verifies both hashes and copies the complete unchanged
W32Win16WndProcEx body into a build-only fixture include. It does not rewrite
the mirror or replace the formal provider's complete translation unit.

An initial isolated link of the entire /Gy object still required 48 external
symbols from other message thunks. That attempt failed and is not acceptance.
The final bounded fixture selects the exact complete owner function instead;
it controls TD/TEB and intercepts CallBack16. Messages are >= WM_USER, so no
message thunk body is claimed as verified. Unexpected FindPWC, standard-class
lookup or WM32NoThunking execution fails the test; there are no successful
no-op implementations for those paths. Real native STATIC HWND/default handling
is used; no modern TEB fields or guest media are patched.

The first run calls the original body; the second links actual mirrored
DispatchClientMessage and reaches the same body through its callback pointer.
Both prove: tagged 92340056 decodes to 12340056; tagged 92300056 restores the
high-selector/LDT bits to 92340056; wParam and hInstance use original 16-bit
truncation; successful guest-boundary return propagates; failed CallBack16
uses original DefWindowProc; TDF_IGNOREINPUT bypasses CallBack16. Three actual
callback-boundary invocations have three balanced BlockWOWIdle pairs.

Both x86 /MT /Gz /W3 /WX fixture builds and executions pass (exit zero, empty
stderr): `WOW_ORIGINAL_WNDPROC errors=0 callbacks=3 idle_pairs=3`.
Runtime files under O:/winnt/tests/S40USER:

| Artifact | SHA-256 |
| --- | --- |
| original-wndproc.exe | 482A53F978338303E53358711064B7DCE473A51F6D2A8D44AED9C57257295459 |
| original-wndproc-chain.exe | 330FC24CFB3DCFDD939457077287B4C96A7277E825D8A016F3C681B48A2D4F27 |

Logs: O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log.
Consequence: a native gateway must preserve the encoded procedure DWORD and
delegate through original DispatchClientMessage/W32Win16WndProcEx, not decode
the selector a second time or reconstruct PARM16. Real TD/task ownership,
message thunks below WM_USER and execution beyond CallBack16 remain unverified
by this fixture. No new product implementation, formal rebuild or deployment
is claimed in this decoder verification.

### Native window gateway integration

The product creation binding captures the encoded procedure and callback with
WW publication. wow_window_native_proc delegates to original
DispatchClientMessage without selector decoding or PARM16 reconstruction.
WM_NCDESTROY finally cleanup removes only its borrowed publication identity.

The real native creation fixture now uses that product gateway and passes:
`WOW_NATIVE_CREATION_BINDING errors=0 callbacks=25 nested=1 rejected=1 vetoed=1 destroyed=5`.
The original-wndproc native fixture uses SendMessage on real windows through
the gateway and original DispatchClientMessage/W32Win16WndProcEx and passes:
`WOW_ORIGINAL_WNDPROC errors=0 callbacks=3 idle_pairs=3`.
Both exit zero with empty stderr. Only WM_USER+7 delegates to the original
WOW callback body; other fixture messages use DefWindowProc. TD/TEB are
controlled and CallBack16 intercepted: no guest execution is claimed.

| Artifact | SHA-256 |
| --- | --- |
| O:/winnt/tests/S40USER/native-gateway.exe | 59E71BE5FE9BF078DEE98DA758615E2854B5670485E534C830E409FF33ECEB09 |
| O:/winnt/tests/S40USER/original-wndproc-native.exe | 93F6C7441B76F71DF879632D4E7926EEEE23919358E757CBD66EE04597FFEB84 |
| build/M0-T420/S40/provider-stdcall-r1/WOW32.DLL | 5AE7531E44820564C8D899CF8E0BDF78AFA9D46B91CAD415D9AB5393CBEF7BEB |

Runtime logs: O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log.
Formal x86 provider build passed; log:
build/M0-T420/S40/class-registration-r1/native-gateway-provider-build.log.
Existing DESCRIPTION and duplicate-export warnings remain. This provider was
not deployed as the accepted runtime, and prior DOS regression results do not
verify this new hash. Output-table composition, real task ownership, message
thunks and actual Win16/WRITE execution remain S40 gates.

### Native gateway failure and exception cleanup

The native creation fixture additionally rejects a zero procedure, a recursive
gateway target and a tagged procedure without its WOW callback, checking
ERROR_INVALID_PARAMETER and no publication. An unbound WM_CREATE returns -1
with ERROR_INVALID_DATA. A controlled callback raises exception E0420040 on
WM_NCDESTROY: both dispatch reference cleanup and gateway detach execute,
the property is absent afterward, and an explicitly retained borrow survives.
The test then releases that borrow and destroys its still-live STATIC window.
This exception call is direct, deliberately outside USER's WNDPROC exception
policy; it does not claim native exception-dispatch or guest acceptance.

The x86 /MT /Gz /W4 /WX build and hidden execution passed, exit zero and
empty stderr. Existing native creation checks remain green (errors=0,
callbacks=25, nested=1, rejected=1, vetoed=1, destroyed=5).
Artifact: O:/winnt/tests/S40USER/native-gateway-failure.exe;
SHA-256 B07927EA23BE12D0878FAF748E3B98A82CBD20194F5DCC1A6E777266E2487948.
Logs: O:/winnt/logs/s40-native-gateway-failure.{stdout,stderr}.log.
No production implementation or accepted runtime artifact changed in this check.

### Creation entry ownership: source audit and next composition boundary

Reinspection locates the complete original output entry at
OpenNT windows/core/ntuser/client/ntstubs.c::_CreateWindowEx, not client.c.
client.c only publishes its address. The original entry owns expected-version
lookup, the historical ExStyle 0x800 conversion, version-dependent validation,
MDI validation/preparation/completion, ANSI/Unicode class capture, ordinal window
names, CW_FLAGS_DIFFHMOD and cleanup around NtUserCreateWindowEx. Consequently
installing wow_window_create_boundA directly in pfnCsCreateWindowEx would omit
original policy and is not an admissible completion.

W32CreateWindow in wow32/wuwind.c initializes only iClass/flState, vpfnWndProc
and vpfnDlgProc, then passes their address as lpWOW. createw.c copies exactly
sizeof(pwnd->adwWOW), three DWORDs. The rest of WW aliases server WND fields:
style, extended style, module and state. It is invalid to read sizeof(WW) from
that input or treat its uninitialized tail as authoritative. The current native
binding deliberately accepts a complete WW and is not that input ABI.

Expected version has an existing original owner: pfnIn.pfnGetExpWinVer is
W32GetExpWinVer in wcall32.c. For a native module it calls WOWRtlGetExpWinVer;
for packed guest identity it reads NE version through GETMISCPTR, falling back
to RET_GETEXPWINVER via CallBack16. Do not substitute the host executable's
version for a Win16 module or invent a default task version in the adapter.

A direct name search in current WOW32 C/H files finds no consumer of
dwUserSrvState/dwUserSrvState2. This narrows the native callback prerequisite;
it does not establish that USER16 has no direct shared-state consumers.
The legacy winmisc2.asm visibility loop is within the non-WOW block and is not
evidence for the selected WOW build. The x86 PMODE32 build instead includes
k/daytona/wowk.asm and wowkr.asm. The latter's __IsWindowVisible reads object
offsets 171 (visible bit), 122 (type), 24 (parent), and 12 (pointer rebasing).
This is a graph/layout contract beyond WW; the native WW property cannot serve
as its replacement. Source inspection alone does not prove those instructions
are reached in the deployed guest; the existing immutable-media audit remains
the authority for binary selection and real execution still requires tracing.

Next integration order: retain the original client entry policy at its owner,
bind its unavailable capture/native creation boundary, use the registered
original version callback, and materialize only the documented WW prefix into
owned complete storage before native callbacks. Complete MDI and guest shared
graph acceptance must remain explicit, not be hidden by an ANSI-only helper.

Source identities (SHA-256):

| Source | Hash |
| --- | --- |
| OpenNT ntuser/client/ntstubs.c | 7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01 |
| OpenNT ntuser/kernel/createw.c | DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7 |
| current wow32/wcall32.c | 0F72AE39E691F3EF672DF4F1AEB3EBB6443E87F091FB6322EF1C13604AB3D571 |
| current wow32/wuwind.c | AB270E2669CFD383DA4BAAC471BB63E7C9C603581F4F7592AD447E8A783953DC |
| current wow16/user/k/daytona/wowkr.asm | EBF0F3014A585E75512150B94656299C8779E6AF3B632A662C55623D6C2690C3 |

### Complete original client creation entry fixture

`extract-wow-create-entry-fixture.ps1` pins ntstubs.c and ntsend.h, extracts
the entire unchanged _CreateWindowEx body into build-only storage, and uses
the original call/cleanup macros. `wow_original_create_entry_fixture.c`
supplies finite test-only version, capture, parent validation, MDI and kernel
creation bindings. Its LARGE_STRING and PWND views are fixture carriers, not
claims of production NT4 ABI compatibility. The source algorithm itself is
not rewritten or reduced to a hand-selected successful branch.

Ten cases pass: old ExStyle bit conversion; post-4.0 invalid-style rejection;
pre-4.0 pass-through with DIFFHMOD; ANSI class capture and ordinal window title;
Unicode class/ordinal title; invalid MDI parent; failed MDI preparation;
successful preparation/update/completion; backend failure with capture release;
and capture failure forwarding. Each reached backend receives the unchanged
three-DWORD WOW pointer. MDI/native capture providers remain controlled doubles,
so these results do not prove actual MDI windows, string conversion or guest UI.

The final negative case confirms a real property of the pinned original body:
it ignores RtlCaptureLargeAnsiString's FALSE return and still calls
NtUserCreateWindowEx. The controlled failing capture initializes a safe empty
descriptor and the backend rejects it. This proves continued dispatch, not
the runtime consequence of an actual allocation failure in original USER.
Do not count that forwarding as correct recovery or hide it with a successful
stub. Production integration must retain an explicit failure-path decision.
This is host source, not an immutable guest defect.

Build: x86 /MT /Gz /W4 /WX, with 4701/4703 disabled only around the unchanged
original function (conditional capture cleanup and MDI out-parameter analysis).
Exit zero, empty stderr:
`WOW_ORIGINAL_CREATE_ENTRY errors=0 cases=10 capture_failure_forwarded=1`.
Artifact O:/winnt/tests/S40USER/original-create-entry.exe,
SHA-256 A36E95BDC6D2A3F2D2A79CABB198B4DB8800AF74FBFF7B34D392BA6446F96F2B.
Logs O:/winnt/logs/s40-original-create-entry.{stdout,stderr}.log.
No production provider or deployed guest changed; this establishes the original
client policy test baseline for replacing the unavailable kernel boundary.

### Original creation capture replaces fixture conversion

The creation-entry fixture now extracts and executes complete unchanged
RtlCaptureLargeAnsiString from client/rtlinit.c and both RtlInitLarge*String
functions from rtl/chartran.c. The LARGE_STRING/ANSI/Unicode declarations are
also exact extracted user.h definitions, replacing the prior simplified view.
NTDLL performs real RtlMultiByteToUnicodeN conversion; the reached backend now
asserts the captured UTF-16 CLASS text, not just a non-null address. Only heap
allocation/failure injection and finite unused TEB scratch are test bindings.
No native TEB is changed. MDI and kernel creation remain controlled doubles.

The actual original capture with forced allocation failure assigns Buffer=NULL
and returns FALSE; the unchanged original creation entry still reaches the
backend. The backend checks only that known Buffer field on this failed path,
not unspecified length/state fields. Thus the earlier forwarding result no
longer relies on a handwritten capture implementation. This still does not
execute original kernel validation or claim a specific production crash.

Ten cases pass, exit zero and empty stderr:
`WOW_ORIGINAL_CREATE_ENTRY errors=0 cases=10 capture_failure_forwarded=1`.
Build remains x86 /MT /Gz /W4 /WX; additional warnings 4100 (unused original
limit) and 4057 (original UINT* versus NTDLL PULONG, both x86 32-bit) are
suppressed only around unchanged capture/initializer bodies.
Artifact O:/winnt/tests/S40USER/original-create-capture.exe,
SHA-256 5CE62CBF77CB7AF893881DC42867BAFF5661DE55C977A86610CE80F6D5E8F536.
Logs O:/winnt/logs/s40-original-create-capture.{stdout,stderr}.log.

Additional pinned upstream identities:

| Source | SHA-256 |
| --- | --- |
| client/rtlinit.c | D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C |
| rtl/chartran.c | 0F9F0B7FDE3AAA475492A50F333745508698228C38FDBF9A21CBD4BFBEAD383B |
| inc/user.h | ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1 |

No production provider, guest or accepted runtime was changed by this fixture.

### Original entry to native creation integration

The WOW_CREATE_NATIVE arm of wow_original_create_entry_fixture links the
unchanged original _CreateWindowEx and original capture/initializers with actual
class metadata registration, original class lookup, product native creation,
product WNDPROC gateway and original DispatchClientMessage. Its test-only
NtUserCreateWindowEx boundary resolves the captured class name, copies exactly
the original three-DWORD prefix into owned WW, and invokes the existing binding.
It strips the internal ANSI-creator flag before public CreateWindowExA.
That boundary is intentionally only an ordinary ANSI test composition, not a
production replacement for the complete output slot or MDI/Unicode support.
Task/version state is still controlled; callbacks validate metadata and use
DefWindowProcA, not CallBack16 or guest instructions.

The input prefix ends exactly at a PAGE_NOACCESS boundary. Successful create,
SendMessage(WM_APP+7) result propagation, destruction and a second NCCREATE
rejection all pass without reading a WW tail from that input. Both creates
observe the unchanged opaque application parameter and original prefix before
callbacks; class unregister succeeds and its metadata lists are empty afterward.
ANSI capture uses original conversion, with two allocations and two frees.

All prior ten controlled policy cases also run. x86 /MT /Gz /W4 /WX build
passes with the previously documented scoped original-source warning disables.
Runtime exit zero, empty stderr:

```text
WOW_ORIGINAL_CREATE_ENTRY errors=0 cases=10 capture_failure_forwarded=1
WOW_ORIGINAL_CREATE_NATIVE created=2 destroyed=2 messages=1 captures=2 frees=2
WOW_ORIGINAL_CREATE_NATIVE errors=0
```

Artifact O:/winnt/tests/S40USER/original-create-native.exe,
SHA-256 E7BB9F3416773ABEE664DA470B9ABC669DD85779319EA58EB41FBD43C25857A9.
Logs O:/winnt/logs/s40-original-create-native.{stdout,stderr}.log.
This provides integration evidence for the selected ordinary creation boundary,
not installed WOW32 output-table, MDI, task scheduling or real Win16 acceptance.

### Shared ANSI/Unicode native creation binding

ADAPTER-WOW-031 now exposes wow_window_create_boundW alongside boundA, sharing
the same scope, class borrow, CBT publication, identity rollback and cleanup.
The public A/W CREATESTRUCT carriers have the same size and field layout;
the W wrapper copies their representation, never converts or dereferences a
string through the A type. Only the final native CreateWindowExA/W call differs.
No original mirror algorithm or guest media changed.

The original-entry integration additionally executes Unicode class-name
creation and atom-class Unicode creation rejection. Unicode title readback,
message result and cleanup pass. Both A and W retain the 12-byte prefix guard
page; only ANSI class-name calls allocate conversion buffers. Four NCCREATE
callbacks match four NCDESTROY callbacks, two messages return the expected
value, and both captured ANSI names are freed. Class registration remains ANSI:
this does not verify Unicode-class procedures or arbitrary non-ACP characters.
The output slot and MDI remain uninstalled.

x86 fixture exit zero, empty stderr:
`WOW_ORIGINAL_CREATE_NATIVE created=4 destroyed=4 messages=2 captures=2 frees=2`
and `WOW_ORIGINAL_CREATE_NATIVE errors=0`; all ten prior policy cases also pass.
Artifact O:/winnt/tests/S40USER/original-create-aw.exe,
SHA-256 71F43C140D5D1BD5182BBC52BB23013A3AFC1A95D95C9D656F5CF569496DF184.
Logs O:/winnt/logs/s40-original-create-aw.{stdout,stderr}.log.

Formal x86 WOW32 build passed after restoring the VC/SDK environment (the first
attempt could not find cl.exe and did not compile). Existing DESCRIPTION and
duplicate-export warnings remain. Final log:
build/M0-T420/S40/class-registration-r1/native-aw-provider-build.log.
Provider SHA-256 5B05FDBE070A625ACADE65E39A2DAB3C675B82E4BCCFB746F3D6086EF7761972.
This provider is not deployed or real-Win16 accepted; prior DOS regression
results are not attributed to its new hash.

### Real MDIClient creation: reproduced and repaired publication mismatch

Original client/mdiwin.c (SHA-256
77471975010484FC6935B91A56CF7450225D44402F6D9E3709FBBDFF1C8A706E)
shows CreateMDIChild/MDICompleteChildCreation own PMDI child/tile counts,
child ID, maximized-child restoration, system menu ownership and activation.
These functions cannot simply run around modern public CreateWindowEx's MDI
creation: the native MDIClient already owns that state and creation policy.
No second PMDI state machine or private USER object layout is imported here.

A real hidden frame and native MDICLIENT reproduce a separate adapter bug:
public USER wraps lpCreateParams in MDICREATESTRUCT for WS_EX_MDICHILD, exactly
as original _CreateWindowEx does. The existing publication matcher compared
that temporary pointer with the original opaque parameter and never attached
WW. Both A/W creates failed before their intended callbacks (fixture exit 3,
three assertions, error 1168). This is a current binding defect, not a guest
or CCPU bug.

ADAPTER-WOW-031 now compares MDICREATESTRUCT.lParam for native MDI creation,
after matching instance and class. It neither modifies nor retains that
temporary wrapper; normal creation retains its direct comparison. The same
fixture now creates and destroys both real MDI children, validates the wrapper
and parent, and uses native DefMDIChildProcA. Existing nested creation, rejection,
CBT veto and exception-cleanup checks still pass. Native USER supplies MDI
behavior; this is not acceptance of all MDI menus/activation/guest callbacks or
installation of the original complete output entry.

| Artifact under O:/winnt/tests/S40USER | SHA-256 | Result |
| --- | --- | --- |
| native-mdi-before.exe | 32DF12E4A62765941CBCCFF1A6B9C8AFDFB42D360FFB6CD0327CE52386AC8C9D | exit 3, reproduced |
| native-mdi.exe | 3B454EB64CF8A3E2D1D942AA464D46C32C72E527A9C9BEFF2572BA1A36526CB2 | exit 0, repaired |

Logs O:/winnt/logs/s40-<basename>.{stdout,stderr}.log. Repaired stderr is empty:
`WOW_NATIVE_CREATION_BINDING errors=0 callbacks=43 nested=1 rejected=1 vetoed=1 destroyed=7`.
Formal x86 provider compilation/link passed with unchanged existing warnings;
log build/M0-T420/S40/class-registration-r1/native-mdi-provider-build.log.
WOW32.DLL SHA-256
514C132C7780B59E52290C9F1C451E8E913ADD86E830EA123896D5DB24B4DC90.
The provider is not deployed and has no new full DOS/Win16 acceptance claim.

### Formal private-word wiring audit

The current formal map contains wow_window_native_proc and both creation
bindings, but still resolves __imp__GetWindowLongA@8 and
__imp__SetWindowLongA@12 to USER32.dll. The selected walias.obj imports
GetWindowLongA/GetClassLongA/SetWindowLongA; source FindPWW calls
GetWindowLong(GWL_WOWWORDS), FindPWC calls GetClassLong(GCL_WOWWORDS), and
SETWL/SETWC remain SetWindowLong/SetClassLong macros. Thus compiled storage
bindings do not yet connect these actual WOW32 consumers. Native fixture
passes must not be represented as formal private-word restoration.

The original owners and minimum coupled contract are:

- rtl/getset.c::GetWindowData returns the WW address for -1 and its first
  three DWORDs for -30/-31/-32. client/classc.c::_GetClassData returns the
  class WC address for -27. These are live native aliases, not guest pointers.
- kernel/getset.c::xxxSetWindowData writes those three WW DWORDs. Original
  FindPWW sets iClass/flState through this path before returning its pointer.
  A getter-only redirection therefore cannot complete even FindPWW itself.
- wuclass.c::WU32SetClassLong updates encoded GCL_WNDPROC and the original
  WC procedure word separately; it depends on the previous encoded procedure
  result. Native gateway addresses must not escape in place of this value.
  Existing windows versus subsequent class-created windows require separate
  window/class procedure ownership, not a single mutable shared target.
- Bare WW/WC pointers outlive the getter invocation. The integration must keep
  the corresponding borrow across the consuming thunk/callback, including
  nested destruction and exception cleanup. Returning a pointer followed by
  immediate release is not justified by the existing scoped fixture results.
  W32Dispatch and W32Win16WndProcEx are actual consumer scopes to audit; no
  new always-live registry or leaked reference is an acceptable substitute.

Next implementation unit is therefore private window/class access plus scoped
ownership and procedure exchange, not an isolated GetWindowLong wrapper.
Ordinary public indices must retain native USER calls. This is still an open
S40 integration obligation, not an approved omission.

Pinned original owners (SHA-256): rtl/getset.c
4A2CB2067F5DD776676D7F808F40F6468082DB018E16BCF9A77DAF00BDDB34C9;
kernel/getset.c F0D22E4285E8B33B873854913A11587C67D78E50E1F5DB28EC89EF8B6FF06D3D;
client/classc.c 1DA5A77B48A073A0C6001D7F56B530C941C90B55C8E995FD6C10914AC8E8D937.

### Scoped WW/WC alias lifetime implementation

ADAPTER-WOW-032 now supplies nested thread-local borrow scopes over the existing
window/class storage. The unavailable original dependency is WND/CLS object
retention behind private pointer getters, not their indexing algorithm. Full
USER desktop/object locks are a stopping boundary; public getters expose no
NT4 WW/WC. The finite binding retains existing references, deduplicates within
each scope, and releases them on strict LIFO leave. It creates no IDs, guest
pointers or independent window/task policy. Calls outside a scope fail with
ERROR_INVALID_STATE; allocation failure releases the incoming reference and
returns ERROR_NOT_ENOUGH_MEMORY. Class operations still require the caller's
existing publication synchronization. Heap-failure injection and cross-thread
acceptance remain pending, not inferred from successful native tests.

wow_window_dispatch_bound now enters this scope and leaves it in finally.
The class-lifetime fixture verifies repeated reads, shared WW/WC identity,
duplicate-enter rejection, invalid leave-order rejection, callback destruction
of both HWNDs and class unregister, and survival of an outer class alias after
all explicit window borrows release. The first test run failed because its new
comparison used shared_words before assignment; correcting that fixture-only
ordering produced exit zero, not a product fix. The native creation fixture
also borrows WW before raising its controlled exception, verifies scope removal
afterward, and reruns ordinary/MDI creation, nested creation and rollback.

Both corrected fixtures exit zero with empty stderr:

| Artifact under O:/winnt/tests/S40USER | SHA-256 |
| --- | --- |
| user-borrow-scope.exe | CF8BEC162E513F4C8C5A9EBAF0DD28214DA77A50E34F065C7D45D264E661E038 |
| native-mdi-scoped.exe | 2D679D446C994BBB104B3AD394181AB70BCA8079641241E80C8D0C36F4F5BF69 |

Markers: WOW_WINDOW_CLASS_SHARED_LIFETIME errors=0 callbacks=1;
WOW_NATIVE_CREATION_BINDING errors=0 callbacks=43 nested=1 rejected=1
vetoed=1 destroyed=7. Logs O:/winnt/logs/s40-<basename>.{stdout,stderr}.log.
Formal x86 WOW32 build passes with the previously recorded linker warnings;
log build/M0-T420/S40/class-registration-r1/user-borrow-provider-build.log.
Provider SHA-256 E6C92B610A0F1FC8410B8B265E368DC0352BEC53ADFDEE6CA9543E3AEDD55556.
The build selects the new binding, but original private Get/SetWindowLong and
Get/SetClassLong redirection, procedure exchange, and W32Dispatch scope wiring
are not yet implemented. This does not close the coupled integration unit or
constitute Win16 acceptance; no accepted runtime was replaced.

### Original thunk-call scope integration

DIV-296 now changes only the two LPFNW32 invocation expressions in original
wow32.c (W32Dispatch and W32TryCall) plus the binding include. The adapter
wow_user_invoke_thunk includes original precomp declarations and calls the
original LPFNW32/PVDMFRAME ABI without pointer casts or a new frame layout.
It enters a native private-alias scope, returns the original thunk result,
and leaves in finally. It does not catch exceptions, choose task state, alter
guest frames or replace original W32Exception. The selected CCPU provider uses
these C entry points, not FastWOW assembly; no assembly guest patch is added.

Formal x86 object inspection shows W32Dispatch@0, W32TryCall@8 and an actual
undefined reference to wow_user_invoke_thunk@8 resolved by the selected adapter.
The new fixture compiles with the formal provider flags and links its actual
adapter object. Two typed thunks nest, borrow WW, destroy its window, and
retain the outer alias until return; a second nested call raises a controlled
exception and verifies that the scope no longer exists after unwind. The
original result 76543210 and unchanged frame.wAX are checked. This validates
the call binding ABI/lifetime, not execution of W32Dispatch's guest stack,
thunk-ID patching or real Win16.

Fixture exit zero and empty stderr:
`WOW_USER_THUNK_SCOPE errors=0 calls=4 exception=1`.
Artifact O:/winnt/tests/S40USER/user-thunk-scope.exe,
SHA-256 39333945DF1FDE3026AEA53DCF0693FFA5B0A5E774B6305167A0A5F338B3EB6E.
Logs O:/winnt/logs/s40-user-thunk-scope.{stdout,stderr}.log.
Formal x86 provider build passed, retaining the existing linker warnings;
log build/M0-T420/S40/class-registration-r1/user-thunk-provider-build.log.
Provider SHA-256 0BCAE463D4883D4DE0761E3DA6E3261775E438B0295E0DA2B61FF5394BFDCF28.
Private API indexing/procedure exchange and real guest acceptance remain open.
No accepted runtime was replaced or S/T closure claimed.

### Window procedure storage exchange

The existing WW binding now reads/exchanges the opaque per-window dispatch
procedure using Interlocked operations; native gateway dispatch snapshots the
same value. It neither decodes selectors nor changes WW.vpfnWndProc. The
registered callback is immutable for this publication. Invalid zero/recursive
gateway/missing callback targets are rejected at this storage API, whose caller
must already have performed USER policy. This is not installed as full
SetWindowLong: original kernel/getset.c's CPD/A-W mapping, zombie checks and
client/server mapping remain integration obligations, as does class exchange.

Real HWND subclass/gateway tests verify unchanged old encoded values, independent
targets in two windows, tagged-to-native and native-to-tagged transitions,
original DispatchClientMessage selection, rejected gateway recursion without
changing the output parameter, unchanged WW guest procedure words and cleanup.
Result: exit zero, empty stderr, `WOW_WINDOW_PROCEDURE errors=0 tagged_calls=3`.
Artifact O:/winnt/tests/S40USER/window-procedure.exe,
SHA-256 7CF80055D0AD79FFA532ABD9CB8DE51DF1F9F67928F463A65543B611AC6AC035.
Logs O:/winnt/logs/s40-window-procedure.{stdout,stderr}.log.
Formal x86 provider build passed with existing linker warnings, log
build/M0-T420/S40/class-registration-r1/window-procedure-provider-build.log.
Provider SHA-256 E303C227297DDB0914A4444538D3C0EBD7067CE0E4C2DF102F92833305A91DA6.
No real guest or full private-API acceptance is claimed and no runtime deployed.

### Formal private WW/WC index binding

DIV-297 adds one final include binding to original precomp.h, leaving original
FindPWW/FindPWC and SETWL/SETWC call bodies intact. ADAPTER-WOW-033 intercepts
only GWL_WOWWORDS, window DWORD -30/-31/-32, GCL_WOWWORDS and class setters
-28/-29. All other indices retain native USER32 calls, including procedure
indices whose full conversion remains pending. The selected ANSI provider's
ordinary API aliases now reach this boundary; Unicode API interception is not
claimed. A missing scope/storage produces no alias, never modern private data.

The source owners were inspected before implementation: GetWindowData selects
the original WW pointer/three words; _GetClassData returns shared WC; original
class setters replicate to base/clones. Current windows of one registration
already share one WC, so the same write reaches each without clone copies.
Both original private setter functions declare dwOld but do not initialize it
in these private cases. The binding explicitly returns the prior word; current
SETWL/SETWC statement callers discard this result. This deterministic native
boundary is documented, not represented as byte-exact original return behavior
or an immutable guest limitation. No original public-index policy is rewritten.

Formal walias.obj now references wow_user_get_window_longA@8,
wow_user_set_window_longA@12 and wow_user_get_class_longA@8 rather than importing
those operations directly from USER32. A full provider rebuild (87 steps)
passed with existing linker warnings. The shared lifetime fixture links the
formal access-binding object and verifies all three window words, pointer
identity, shared class write/read/restore via sibling windows, native USERDATA
set/get and class atom passthrough, followed by nested destruction/unregister.
This is actual index-binding evidence, not actual execution of a Win16 thunk.

Fixture exit zero, empty stderr:
`WOW_WINDOW_CLASS_SHARED_LIFETIME errors=0 callbacks=1`.
Artifact O:/winnt/tests/S40USER/private-access.exe,
SHA-256 66FD0CFE6DE8396F69561132918B02E5EDE58649C1446036D8CDF2B5AF714EE1.
Logs O:/winnt/logs/s40-private-access.{stdout,stderr}.log.
Formal log build/M0-T420/S40/class-registration-r1/private-access-provider-build.log;
WOW32.DLL SHA-256 71AF5859C6AAE5BB3EBB712C44F84F1F21BDDC8AAB46C66F921700FC6C768FDF.
Standard-class enrollment, window/class procedure policy, complete output-table
composition and real guest remain pending. No accepted runtime was replaced.

### Original alias consumers executed

`extract-wow-alias-fixture.ps1` verifies both the selected mirror and original
OpenNT walias.c against SHA-256
6118B6837C7ED291CC9D70446B7D691699D49C9EF6F2BE60D8E0FAC478D93B65,
then extracts unchanged GetStdClassNumber, FindPWW and FindPWC bodies into the
S40 build root. The native fixture executes these original bodies through the
formal thunk scope and private-index binding, not substitute alias functions.

The run checks unknown-class initialization and its no-overwrite rule,
SETWL reset/reclassification, shared WC lookup and SETWC mutation, original
case-insensitive STATIC identification, and retained aliases after native
window destruction and class unregistration until the thunk scope returns.
After return there is no active alias scope and both owned class lists are
empty. STATIC receives fixture-owned WW only: this does not prove automatic
standard-class enrollment or standard-class WC initialization.

Artifact O:/winnt/tests/S40USER/original-alias.exe, SHA-256
6666812B189C9B9347174BF1450277D31AEAECCFC7F5B243B155CC970B37FBAE.
Hidden native execution exited zero with empty stderr and
`WOW_ORIGINAL_ALIAS errors=0 calls=1`.
Logs: O:/winnt/logs/s40-original-alias.{stdout,stderr}.log.
No guest instructions were executed by this fixture and no product was deployed.

The next integration gate is concrete: original WU32SetWindowLong in wuwind.c
passes either an existing native thunk target or an encoded WNDPROC_WOW target
to SetWindowLong(GWL_WNDPROC), and only then updates the WW words. Current
ADAPTER-WOW-033 forwards this public index directly to modern USER32. That is
not a completed WOW subclass path: a guest encoded target must never become a
native callable WNDPROC. The existing opaque exchange helper is storage only,
not a sufficient replacement. Original rtl/getset.c GetWindowData and kernel/
getset.c xxxSetWindowData additionally enforce process ownership, deleted-window
rejection, client/server procedure mapping and ANSI/Unicode callproc conversion.
Their return value feeds original WOW thunk-versus-guest decoding. The pending
integration must preserve these source contracts and the dispatch gateway,
including restoration of native procedures; private-word fixture success does
not waive this gate or establish real Win16 readiness.

### Native CallProc observation and raw-procedure invariant

The window-procedure fixture now creates a real Unicode class/window and
queries its procedure through both public API variants. In this x86 run,
GetWindowLongA and SetWindowLongA returned the same native conversion token
`ffff0943`, while GetWindowLongW returned the actual Unicode procedure.
CallWindowProcA correctly delivered an ANSI WM_SETTEXT payload as Unicode;
restoring the returned token through SetWindowLongA restored IsWindowUnicode
and the original W procedure. Three text calls and a WM_USER return were
checked. The concrete token is run-local, not a stable identifier or a claimed
portable modern ABI.

Source review distinguishes this from DispatchClientMessage: original
client/clmsg.c CallWindowProcAorW validates CPD tokens and translates messages
before CALLPROC_WOWCHECK. Original kernel/getset.c xxxSetWindowData resolves a
CPD before assigning lpfnWndProc. DispatchClientMessage receives that resolved
procedure and may therefore use WNDPROC_WOW selection. These are different
entry contracts, not interchangeable dispatch helpers.

The current storage adapter previously accepted the reserved FFFF CallProc
tag as a WOW target. Attachment, exchange and raw dispatch now reject it with
ERROR_INVALID_PARAMETER before invoking any callback. Negative tests preserve
the caller's old-value/result sentinels and observe no guest callback. Existing
tagged/native transitions still pass. This is the original raw-storage
precondition; it does not implement public SetWindowLong CPD conversion, which
remains required above it. No valid original guest target is replaced with a
failure policy and no mirror algorithm was changed in this correction.

Fixture: O:/winnt/tests/S40USER/window-procedure-cpd.exe,
SHA-256 603678542F6F625CFD2BA3062108AFDB4884581684CEF9B3A143083E5F739F70.
Exit zero, empty stderr, `WOW_WINDOW_PROCEDURE errors=0 tagged_calls=3` and
`WOW_NATIVE_CALLPROC ... text_calls=3`.
Logs: O:/winnt/logs/s40-window-procedure-cpd.{stdout,stderr}.log.
Formal x86 provider rebuild passed; SHA-256
AD55E02154310566FC04371B51C146B0CE40BCD3E8622FBD81A2F686D29186FA.
Log: build/M0-T420/S40/class-registration-r1/callproc-provider-build-authorized.log.
The first restricted build launch remained live without compiler children or
output; its identity was verified and that Ninja alone terminated before the
authorized retry. It is not counted as a successful build or a concurrent run.
The new provider has not been deployed or given real guest/full DOS acceptance.

### Original WOW subclass policy contract

The new extract-wow-subclass-fixture.ps1 pins both original and mirror
wuwind.c to AB270E2669CFD383DA4BAAC471BB63E7C9C603581F4F7592AD447E8A783953DC
and extracts the entire unchanged WU32SetWindowLong body. The fixture uses the
formal provider's actual headers, argument layout and calling convention.
FindPWW, native setter results and thunk lookup/allocation are controlled test
seams; no guest address is executed and this is not native USER integration.

Eight cases pass: ordinary 16:16 encoding/old-procedure decoding; high-selector
bit recovery; old CPD routed to GetThunkWindowProc without guest decoding;
old native procedure routed to that same thunk allocator; failed guest-target
set preserving every WW byte; failed native-thunk set preserving every WW
byte; successful restoration of a native thunk clearing vpfnWndProc and
updating class/state; missing WW rejecting the operation without native calls.
The restoration case deliberately uses an FFFF native callproc value returned
by IsThunkWindowProc: this proves that the public setter must accept and
resolve such a value even though the raw storage layer must reject it.
Tests assert the exact native argument, return value, private-word writes and
thunk allocator calls, not just a successful process exit.

Result: `WOW_ORIGINAL_SUBCLASS errors=0 cases=8`, exit zero, empty stderr.
Artifact O:/winnt/tests/S40USER/original-subclass.exe, SHA-256
2CC76CA3A3555BBE5ED14A54BE824A0711C8B45561D255BC6D53E30420D43DD7.
Logs: O:/winnt/logs/s40-original-subclass.{stdout,stderr}.log.
No mirror change or production provider change was needed for these policies.
The required integration order is native procedure validation/conversion and
successful exchange, then the unchanged WOW32 WW update and old-value decode.
The fixture's controlled setter cannot stand in for that pending implementation.
Dialog and non-procedure branches are compiled but are not exercised here.

### Admission: native gateway procedure query binding

The next bounded ADAPTER-WOW-033 integration binds the existing ANSI
GetWindowLong(GWL_WNDPROC) caller. Original rtl/getset.c GetWindowData returns
the client-visible procedure, never a private dispatch gateway. Its direct
translation unit requires private WND/CLS and CPD/server tables unavailable
to this worker. Recreating those tables merely to query a native window is
unnecessary: public GetWindowLongA retains the native handle/process check and
native CPD/A-W conversion. For windows whose returned procedure is exactly
our ANSI gateway, the smallest seam returns the already registered logical
target from retained window metadata. Other procedures, including native
CallProc tokens, pass through untouched. This is a source-contract binding,
not a new subclass policy or a complete Get/SetWindowLong implementation.
Missing bridge metadata must fail rather than expose the private gateway.
Class procedure changes, Unicode gateway composition and setter conversion
remain separate required integration work. Tests must compare the native API
and provider-facing API, prove updates after target exchange, preserve native
CPD behavior, and reject missing metadata/destroyed handles.

Implementation and verification: the existing private-access adapter now
performs that native query first and substitutes only an exact gateway match.
No imported source body or additional provider API was introduced. The fixture
links the rebuilt formal wow_user_private_access.obj and compares native and
provider-facing queries before/after both tagged and native target changes.
It checks native Unicode CPD passthrough, destroyed-window failure, and a live
gateway with intentionally absent metadata returning ERROR_NOT_FOUND rather
than its address. Existing dispatch/CPD-rejection tests also remain passing.

Artifact O:/winnt/tests/S40USER/window-procedure-query.exe, SHA-256
F85BC01B4B3DA29CA5C1DEBB8E1E77B610187B9A7589AC6E0246CA0663FA3F76.
Exit zero, empty stderr, `WOW_WINDOW_PROCEDURE errors=0 tagged_calls=3`;
the native CallProc test also records three successful Unicode text calls.
Logs: O:/winnt/logs/s40-window-procedure-query.{stdout,stderr}.log.
Formal x86 WOW32.DLL SHA-256
29377FC996F927FBE4C8A9B9EABC3C4581D53E007677F3E3F6E090F192EC4055;
build log build/M0-T420/S40/class-registration-r1/procedure-query-provider-build.log.
The initial standalone compilation of the adapter lacked its formal private
include set (wingdip.h); verification instead links its actual formal object,
not a local substitute definition. No product deployment or guest pass claimed.

### Reached short-HWND failure in original getter integration

The alias fixture now also executes the entire hash-pinned original
WU32GetWindowLong body with its actual GETWINDOWLONG16 frame and HWND32 macro,
original FindPWW, formal private-access/thunk objects, and real native windows.
Only the final guest thunk allocation is a controlled GetThunkWindowProc seam.
This is stronger than passing a full HWND directly to the adapter.

This test FAILS: four procedure queries return error 1400. The native diagnostic
isolates the first boundary: full HWND 018a134c and its zero/sign-extended 134c
are accepted by IsWindow; GetPropW on 134c retrieves the published property,
and actual native GetWindowLongA returns the native gateway. In contrast,
GetWindowThreadProcessId on 134c returns zero. The binding's own_window check
uses that last API before acquiring WW, so original FindPWW fails. This is a
project integration defect, not an original WOW32/guest defect or evidence that
modern USER universally rejects short handles. The earlier full-HWND getter
tests remain valid but do not prove the original thunk path.

Original client/handles.c GetFullUserHandle supplies the relevant owner:
it obtains the window's current uniqueness from the original USER handle table
and retains the input low word when no window entry exists. Recovery must bind
that source contract to owned published native identities before using APIs
that require a full HWND; neither changing HWND32 in the mirror nor trusting
an arbitrary foreign window property as a native pointer is acceptable.
This must join the still-pending USER handle/publication contract, not create
a second guest handle namespace or enumerate unrelated desktop windows.

Failed artifact O:/winnt/tests/S40USER/original-alias-query.exe, SHA-256
1D520870264BB0D5745926A177646122582742C8914B9664EFDD35E78698ABF0.
Exit 3; `WOW_ORIGINAL_ALIAS errors=4 calls=1`; stderr empty.
Logs: O:/winnt/logs/s40-original-alias-query.{stdout,stderr}.log.
The failure is retained as an integration gate, not classified as passed or
waived. No accepted runtime was replaced.

### Short-HWND correction: reuse native procedure ownership validation

Further evidence supersedes the proposed need for a new full-handle lookup
at this particular boundary. Original rtl/getset.c GetWindowData(GWL_WNDPROC)
enforces TestWindowProcess. The native public procedure query retains that
same-process restriction and accepts the short HWND. A separate fixture
creates a hidden child process's window with a deliberately invalid private
property pointer (value 1). Full, zero-extended and sign-extended HWND forms
all return ERROR_ACCESS_DENIED from the native procedure query. This proves
the operation can validate ownership before any property dereference.

ADAPTER-WOW-027 own_window now uses that native procedure query instead of
GetWindowThreadProcessId plus a local process-ID comparison. No mirror change,
new lookup table, fabricated handle ID, desktop enumeration or guest change
is required. The original getter/alias integration now passes all four
previously failing queries, including selector-high-bit decoding and native
thunk selection. The broader USER handle-table/shared-graph obligations remain;
this correction only removes the wrong requirement from owned WW access.

All runs exit zero with empty stderr:

- original-alias-query.exe SHA-256
  CF1870CFF3E745537177B5E66C432FE2939EB18BADD8C8E2A28FB56E5604148A:
  `WOW_ORIGINAL_ALIAS errors=0 calls=1`.
- window-ownership.exe SHA-256
  D81EB39528D4253A055C1BAAE59648D09383CFD1FF417A24AB7C04612806EF2E:
  `WOW_WINDOW_OWNERSHIP errors=0 variants=3`. The binding rejects acquisition
  and attachment with ERROR_ACCESS_DENIED and leaves the foreign property
  untouched. The fixture releases its child through an event and checks exit.
- short-hwnd-lifetime.exe SHA-256
  6D095624706F0167F7004746E7044E660F79881FCF9D43866E24B156B8245088:
  destruction during callback errors=0, four allocations/four poisoned frees,
  native dispatch, exception cleanup and live-field refresh all pass.

Artifacts are under O:/winnt/tests/S40USER; corresponding logs are
O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log. Handle values are
run-local; zero/sign forms may coincide when the low word's sign bit is clear.
Formal x86 WOW32 rebuild passes, SHA-256
E29FE5D3863A4B8D165125C57B4AC0467AA4FA8BCB7C51211388C0297F7D75AF.
Log: build/M0-T420/S40/class-registration-r1/short-hwnd-provider-build.log.
No formal runtime was deployed and real guest/full regression is not claimed.

### Same-defect sweep: FillWindow short handles

The remaining executable GetWindowThreadProcessId use in the WOW binding
family was ADAPTER-WOW-022's control-color branch. The original owner is
kernel/random.c xxxGetControlColor: its actual condition compares process
identities (despite the adjacent comment saying thread), uses DefWindowProc
for a foreign process, and otherwise sends the color message with invalid-brush
fallback. The private server identity cannot compose directly; the already
verified native procedure-query ownership contract provides the same decision
without changing the original branch order or introducing handle mappings.

The existing pixel/brush-origin fixture, now also built with
WOW_TEST_SHORT_HWND, failed before correction with error 1400 and zero color
callbacks. Failed artifact SHA-256:
4D7461DC36774B0B99A2613EF493DEBF2287CF58EB7FF365F97CE44F84880FE8.
The correction uses native GetWindowLongA(GWL_WNDPROC); ERROR_ACCESS_DENIED
selects the original foreign-process default branch, other query failures
restore brush origin and fail, and successful queries retain the send branch.
The native short handles remain unchanged and no mirror file is modified.

After correction, short and complete HWND variants both pass exact pixel
bounds, brush-origin restoration, invalid-brush fallback, non-unit DC mapping,
empty clipping, invalid DC and destruction from the color callback. The
ownership fixture also calls FillWindow with the foreign child window as
brush owner for all three handle forms. That child waits without pumping
messages; the default branch returns promptly and the child exits through
the explicit completion event. All runs exit zero with empty stderr.

Artifacts under O:/winnt/tests/S40USER:

- fill-short-hwnd.exe:
  DEBA8EFDBAD406B4EA9DB9F273217F31E71CF479BFF20A18557FB8F36CE17292.
- fill-full-hwnd.exe:
  85A8754BB7FBE951F1695F5A30608262B70D5C95489AC78CF517F6B9A9991E87.
- window-ownership-fill.exe:
  93DA0E9DB787CAE1C1BF47A56DE09BBB4052498B4B5A56F9BCD210175609C2B9.

Corresponding logs use O:/winnt/logs/s40-<artifact-basename>.{stdout,stderr}.log.
Both fill runs report WOW_FILL_WINDOW_BOUNDARY_OK and the non-unit/DC-destroy
markers; ownership reports errors=0 variants=3. The fixture main is explicitly
__cdecl for the /Gz build (an initial /WX compilation caught that mismatch).
Formal x86 WOW32.DLL SHA-256:
BDEDBD24AB2E766BFD375C3438D80ABFA39F5C34333301630C66A416A6B04181.
Build log: build/M0-T420/S40/class-registration-r1/fill-short-provider-build.log.
No remaining executable GetWindowThreadProcessId use exists in the inspected
ntvdm-exe/wow family or wow_public_user_facade.c. This is a scoped same-defect
sweep, not an assertion about every process-ID call in the whole repository.
Output-table registration, setter conversion and real guest remain unaccepted.

### Original Get/Set chain for already-bound gateway windows

ADAPTER-WOW-033 now connects the existing provider-facing SetWindowLongA
GWL_WNDPROC index to opaque target exchange only when the native procedure is
already exactly the worker gateway. The original GetWindowData/xxxSetWindowData
owner exposes/replaces the logical client procedure; the public native window
cannot hold an executable guest address. The admitted storage binding retains
that logical target while keeping the native entrance installed. The existing
exchange performs same-process/storage validation and refuses recursion/CPD
tokens; no selector decoder, WW update or new class policy enters the adapter.
The original WU32SetWindowLong body still performs all those WOW decisions.

This is incomplete integration, not a changed final contract: valid CPD
restoration must later resolve through its owner before raw exchange;
unbound standard windows still require enrollment. These are pending functional
failures, not approved exclusions. Concurrent subclass/destruction and complete
Unicode procedure policy are not established by this single-thread test.
No product is deployed on the strength of this intermediate route.

The alias fixture now executes unchanged WU32GetWindowLong, WU32SetWindowLong,
FindPWW and FindPWC with real windows, original short HWND arguments and formal
access/thunk objects. Only guest thunk lookup/allocation and the terminal guest
callback are controlled. It verifies guest-target encoding and prior-value
decoding, exact WW updates, native-target restoration, and retained native
gateway identity. Real SendMessage calls after the changes return the new
encoded target via the callback and 42 via the native procedure respectively.
Attempting to set the gateway itself fails without modifying any WW byte or
the prior query result. Destruction and scoped references still complete.

Artifact O:/winnt/tests/S40USER/original-subclass-bound.exe, SHA-256
835293E8A659E4F30076E1A4F25092FAB5CDBE4149C858C3349F8A726807D775.
Exit zero, empty stderr, `WOW_ORIGINAL_ALIAS errors=0 calls=1`.
Logs: O:/winnt/logs/s40-original-subclass-bound.{stdout,stderr}.log.
Formal x86 WOW32.DLL SHA-256:
558AFE128F786669C12DD52C8343D649FA231D3F72087AF0C1312E84DDB7B01F.
Build log: build/M0-T420/S40/class-registration-r1/procedure-set-provider-build.log.
No mirror algorithm changed. This is native/original-consumer integration,
not real Win16 instruction execution or a complete S40 acceptance result.

### Whole output-table composition checkpoint

`tests/observation/verify-wow-output-composition.ps1` pins original client.c,
extracts all 20 UserRegisterWowHandlers assignments, checks their field set
against the selected original wowuserp.h, and reports exact decorated symbols
and owners from the formal map. It deliberately reports symbol presence, not
runtime acceptance or callback installation. A W32Init sentinel rejects an
unrecognized map instead of silently declaring every slot absent. The parser
was corrected for its final ABI delimiter and CRLF before accepting results.

For provider 558AFE128F786669C12DD52C8343D649FA231D3F72087AF0C1312E84DDB7B01F
and map 331D2F4547829F9B9C3E5024FF1CEC24AC7C722C14CDA27B1E34EBB4217F535D:

- Two original output service bodies are linked: FreeDDEData (hdata.obj) and
  WOWLoadBitmapA (clres.obj).
- Six named candidate bindings are linked: cursor/icon loading, menu loading,
  resource-directory selection, menu index, DefWindowProc bitmap and FillWindow.
- Twelve output contracts have neither their exact original service symbol
  nor one of those complete candidate bindings in this map. Existing lower
  mechanisms/fixtures do not make those service slots implemented.
- UserRegisterWowHandlers@8 still comes from legacy-wow-user32:USER32.DLL,
  not the recovered original registration body. The 20-slot table has not
  been installed by these newly linked implementations.

The remaining service composition groups, without dropping any slot, are:

| Group | Original output slots still requiring composition |
| --- | --- |
| Class/window/dialog | CsCreateWindowEx, GetClassWOWWords, RegisterClassWOWA, ServerCreateDialog |
| Task and cleanup | DirectedYield, InitTask, RegisterUserHungAppHandlers, WOWCleanup, YieldTask |
| Wait | WowWaitForMsgAndEvent |
| USER identities/search | GetFullUserHandle, WOWFindWindow |

This checkpoint prevents treating the recent original-consumer/WW fixes as
full USER registration. Class and creation recovery must be joined at the
source-defined service interfaces; task scheduling and the shared USER graph
cannot be replaced with successful no-ops to make registration appear ready.
All eight linked bodies/candidates still require actual registration and
applicable guest verification. S40 and the full T objective remain incomplete.

### Original ANSI capture dependency: formal composition and actual TLS

The next class-service dependency now composes as OPENNT-HOST-040:
`windows/core/ntuser/client/rtlinit.c::RtlCaptureAnsiString`. The original
source SHA-256 is
`D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C`.
The complete function body matches the pinned original including comments
and restored CRLF. Full USER initialization cannot compose through this
finite binding; unrelated routines and private precompiled headers are
cropped. No kernel, shared USER heap or second TLS implementation is added.
ADAPTER-WOW-034 reuses the existing worker TLS scratch descriptor, public
process heap and NTDLL conversion. Its declaration provenance is registered
in the component README. External intrusion and new capture algorithms are
unnecessary because the original body composes through this finite ABI.

Formal Ninja x86 provider rebuild passes; map resolves
`_RtlCaptureAnsiString@12` to `rtlinit.obj`. Provider SHA-256:
`73DF43D47DDB4285427E5A92421A39331D70C3D02495D5596EDBE4502FEB53CA`.
Build log: `build/M0-T420/S40/class-registration-r1/capture-build.log`.
Only the existing DESCRIPTION/duplicate-export linker warnings remain.

`wow_user_capture_fixture.c` links the formal capture object and actual
worker `opennt_support_rtl.obj`, not fake TLS or string conversion. Compile
uses provider cflags (without /TC, /c or /showIncludes), /WX, /MT, /Gz and
ntdll/user32/advapi32/legacy_stdio_definitions libraries. Initial harness
commands needed the formal include/define closure, removal of /TC for object
link inputs, and legacy stdio linkage; these were harness setup failures,
not product failures or changes to the recovered function.

Hidden execution of `O:/winnt/tests/S40USER/user-capture.exe` returns zero:
`WOW_USER_CAPTURE errors=0 real_tls=1 real_conversion=1`; stderr is empty.
Artifact SHA-256:
`900E4CA28F648DF9D09289777D4468A1407A6E123A7B7622A00CA820A79A0C27`.
Logs: `O:/winnt/logs/s40-user-capture.stdout.log` and matching stderr log.
Cases cover null with/without force, empty, short static and forced allocation,
129/130-character threshold, 65535-character rejection and concurrent
two-thread scratch isolation/content preservation. Allocated captures are
freed. Heap failure and conversion failure injection are not covered yet.

This adds a reusable original dependency, not a completed output-table slot.
No deployed product binary was replaced. Class service integration, the
20-slot registration, actual Win16 and latest-hash DOS regression remain
unaccepted; old 17-route evidence is not attributed to this artifact.

### Class-client menu ownership moved out of the fixture

Source review found a genuine composition gap: the class-client fixture,
not the production class metadata, retained client menu pointers between
RegisterClassExWOW and UnregisterClass. Original kernel/class.c stores
`lpszClientAnsiMenuName` and `lpszClientUnicodeMenuName`, and returns them
only after the live-window check succeeds. Its source hash remains
`82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A`.
The existing ADAPTER-WOW-030 native record now holds exactly those pointers;
it never retains the input captured-string descriptor. Original client code
still performs conversion and LocalFree. Failed native registration frees
only unpublished metadata; failed unregister leaves output and ownership
unchanged. Successful unregister transfers pointers before unlinking, so
retained WC borrowers neither free the menus twice nor extend client ownership.
Resource IDs are returned unchanged, not passed to a new allocator.

This is a necessary storage binding behind original policy, not a replacement
registration algorithm. The original kernel translation unit still cannot
compose wholesale because it requires desktop heaps and private USER objects.
No new mirror diff, ID namespace, menu conversion, or overlay is introduced.
CLSMENUNAME declaration provenance is registered in the adapter README.
All existing binding callers were updated for explicit optional menu input/
output; native-backed tests no longer use the fixture's global menu carrier.
Mock-only original branch tests still intentionally have a fake server.

Formal x86 WOW32 link passes with only existing linker warnings, SHA-256
`519D160244D9A1D6924BDC47D84FFD89B17C1333986A9E6BB48CD4D60BC8A9DC`.
Log: `build/M0-T420/S40/class-registration-r1/class-menu-build.log`.
The source extractor still verifies exact original lookup and class-client
tokens except the two previously registered capture-failure frees.

Hidden runtime tests under O:/winnt/tests/S40USER return zero, stderr empty:

- `class-menu-owner.exe`, SHA-256
  `6AE51CFFC070A1B0246B02E3ED088D23AF0138E16FDAA7509EFE8D31F4B95BF8`:
  original class-client 19 cases pass, captures=0, menus=14/14. Includes real
  duplicate-registration failure, missing output refusal before native
  unregister, live-window refusal with byte-unchanged output, successful
  original client cleanup, and retained WC after class unlink.
- `class-lifetime-menu.exe`, SHA-256
  `08B6807CF2D79AF2F7FF5982B6281296489F03A9A3216CC656C3BC038CEE2D31`:
  native private/public lookup, lifetime and resource-ID menu return pass;
  returned pusMenuName is NULL rather than a stale capture descriptor.
- `native-mdi.exe`, SHA-256
  `A1B6E2EF38CEC8E6B6A3DCCC481506319AD16EF4967BDE5D5185BB35C14868DD`:
  errors=0, callbacks=43, nested=1, rejected=1, vetoed=1, destroyed=7.
  The first rebuild command omitted the already-required borrow-scope object;
  adding that existing object fixes the harness link without a source change.

Logs use O:/winnt/logs/s40-<executable-stem>.stdout.log and stderr.log.
The original alias/getter/subclass fixture was rebuilt against the changed
class ABI as `original-alias-menu.exe`, SHA-256
`2511528F127E8C93F6F38ABF830564FD3CBF777C918C23E008162EF48AC7C850`;
it also returns zero with `WOW_ORIGINAL_ALIAS errors=0 calls=1`. This preserves
the previously bounded host/controlled-thunk evidence, not guest execution.
No product deployment or guest mutation occurred. Public class-menu mutation,
task-wide teardown, original class-service publication, real guest window
callbacks and full regression remain mandatory, unaccepted S40 work.

### Original GetClassWOWWords enters the formal provider

OPENNT-HOST-041 imports the exact complete client/ntstubs.c::GetClassWOWWords
body; OPENNT-HOST-042 imports its seven exact ntsend.h macros. Both retain
the original notices and CRLF. Pinned source identities and excluded portions
are registered in the opennt-host README. Comparison confirms the entire
function body occurs verbatim in the original and all seven macro blocks
match. The original FIRSTCOPYLPSTRW, not an ID-accepting variant, remains
selected. Original capture, class-not-found error mapping, pointer adjustment,
query and cleanup order are unchanged.

ADAPTER-WOW-035 supplies the unavailable private CLS binding. The existing
borrow scope carries an explicit caller-owned class context, inherited by
nested scopes and cleared on exit. Lookup reuses existing case-insensitive
name/native-atom mapping and original GetClassPtr. WC shares the same two
words returned through FindPWC; no alternate registry/atom namespace exists.
The private _GetClassData fixed GCL_WOWWORDS expression maps to that WC view.
Original classc.c returns `(DWORD)pcls->adwWOW` for this index; its other
indices, kernel pointers and class policies are not reproduced here. The
client delta is zero for this local view, not a claim of original shared USER
heap or guest graph availability. Missing context fails; the original client
maps NULL to ERROR_CLASS_DOES_NOT_EXIST. Caller serialization and context
lifetime remain explicit responsibilities, not implicit thread safety.

The original source can compose through this bounded binding; a rewritten
query algorithm or external intrusion is therefore unnecessary. Full
ntstubs.c and private NtUserGetWOWClass cannot compose because their unrelated
USER/thread/server and desktop-heap dependencies remain excluded. This finite
query consumes the same original lookup owner already admitted in S40.

Formal x86 provider links `_GetClassWOWWords@8` from `ntstubs.obj`.
After restoring source CRLF, final DLL SHA-256 is
`26CF3A5F10857A7B23C8A0CF538AC690B990BB6AF8DA1CD23E8F8B2C060420FC`;
log: `build/M0-T420/S40/class-registration-r1/class-query-final-build.log`.
Existing linker warnings only; this candidate is not deployed.

The new `wow_original_class_query_fixture.c` links formal query, capture,
class/borrow/window records and original dispatch/lookup objects, plus the
actual worker RTL/TLS object. No replacement query, string capture or TEB is
used. Its native class has a 160-character name to exercise capture allocation.
It verifies missing scope, NULL/empty/missing name, case-insensitive identity,
shared WC, nested inherited context, nested context isolation/restoration,
failed query after unregister, and retained WC data until the outer scope
ends. It never dereferences a returned pointer after scope release.

Hidden runtime artifacts under O:/winnt/tests/S40USER all exit zero, with
empty stderr and matching logs under O:/winnt/logs/s40-<stem>:

- `class-query.exe`: `WOW_ORIGINAL_CLASS_QUERY errors=0 real_capture=1 real_tls=1`,
  SHA-256 `709B6E195A6C5916F103F8F852498157DF7C708E8CA218FF462DC53E22435465`.
- Recompiled `native-mdi.exe`: errors=0, callbacks=43, nested=1, rejected=1,
  vetoed=1, destroyed=7; SHA-256
  `CE44B70995BD4EF7D8793E93C76EC11A10A76D4CBE1C710B4474DF040F65D119`.
- Recompiled `original-alias-class-query.exe`: errors=0, calls=1; SHA-256
  `5798ADAF126A1CBE4B6363698570E9B0BEC1E017BDEFE8AF1EFE7EE34ABC14D2`.

The latter regressions rebuild the changed borrow-scope layout, not stale
objects from the previous class-menu run. Documentation governance and diff
checks pass. This advances the class-query service body from absent to linked
and host-tested; it does NOT install pfnGetClassWOWWords, publish a production
class context, establish task synchronization, or prove guest FindClass16.
Original registration, creation, task/cleanup, wait, shared graph and actual
WOW16 execution remain mandatory. No S/T closure or full-product pass is claimed.

### Class procedure identity and registration prerequisites

Source review establishes two distinct version inputs, not a single safe
constant. Original wkman.c passes WOWINITTASK16.dwExpWinVer to pfnInitTask;
kernel/queue.c records it in both THREADINFO and CLIENTINFO. In contrast,
GETEXPWINVER invokes the registered W32GetExpWinVer module callback, which
reads the guest NE expected version or uses the original guest callback
fallback; native module identities take WOWRtlGetExpWinVer. Registration
must bind both original sources, not copy the fixture's VER31 values into
production. Source identities reviewed: original queue.c SHA-256
`0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8`;
current wcall32.c
`0F72AE39E691F3EF672DF4F1AEB3EBB6443E87F091FB6322EF1C13604AB3D571`;
current wkman.c
`475FEA6B8390A23ABAA1F4ECD804745D6129DF2B860DCAD1A1727C964BDA5291`.
Task initialization remains a real dependency, not an excuse for a no-op.

The same audit found a concrete missing class field: original CLS.lpfnWndProc
is separate from each WND.lpfnWndProc. The native gateway must not replace
its logical identity in WOW class queries. The existing ADAPTER-WOW-030 class
record now optionally captures a logical target/callback by value for a
gateway registration. The ordinary native registration API delegates with no
target. Original encoded procedures remain opaque; no selector transformation
or CPD resolution is duplicated. Raw zero, recursive gateway, CPD and tagged
procedure without callback are invalid at this storage boundary.

ADAPTER-WOW-031 creation now inherits the class target unless given an explicit
resolved target, retaining the class throughout callbacks. ADAPTER-WOW-033
GetClassLongA(GCL_WNDPROC) resolves only our gateway to that retained logical
class target; missing metadata fails instead of publishing the gateway.
Other native class results, including the Unicode standard-class path, pass
through unchanged. Existing window-local target exchange never changes the
class. Original DispatchClientMessage still owns tagged/native dispatch.
No mirror body changed in this step. These additions fill native metadata
gaps; they are not reported as autonomous-code reduction.

Formal x86 provider rebuild passes (existing linker warnings only), SHA-256
`7A9D01BA52FD12F7A9027A3FE8DFCFA475C0A49DF26B173DDF32E71DEB80B04A`.
Build log: build/M0-T420/S40/class-registration-r1/class-procedure-build.log.
Rebuilt hidden fixtures under O:/winnt/tests/S40USER all return zero, stderr
empty, with O:/winnt/logs/s40-<stem>.stdout.log and matching stderr logs:

- class-procedure.exe, SHA-256
  `A9F9DC9C90A6A3D19ADDD547DA605D13408D15BCDC270FF929D71D40EED2A988`:
  errors=0, callbacks=13. Raw CPD/missing callback rejected; caller target
  mutation does not change registered state; two windows share class identity
  while retaining independent explicit/inherited window targets. Real native
  SendMessage reaches original dispatch. A window-local native subclass does
  not alter GCL_WNDPROC. A native Unicode STATIC class result is unchanged.
  The final WOW callback is controlled; no guest instructions are claimed.
- native-mdi.exe, SHA-256
  `550A70E9A88F91BF9A393068F487D60E873EAB1C0A53B2BBE2FA64312D2F3496`:
  errors=0, callbacks=43, nested=1, rejected=1, vetoed=1, destroyed=7.
- class-menu-owner.exe, SHA-256
  `BCAB635F8C95CAE95488D098030DF05A6B56A870D1B526919DD0274CA2DC2AC1`:
  errors=0, 19 cases, captures=0, menu allocations/releases=14/14.
- class-query.exe, SHA-256
  `94AA45C8AD1E83C2E76FB0B9E054ED8F1717CE8DFB19B6F071D414FE9EF1706F`:
  errors=0 with real original capture and worker TLS.

Full class-wide SetClassLong, original registration service publication,
valid CPD policy, standard-class enrollment, task synchronization/cleanup and
actual Win16 remain open. No accepted runtime package was replaced, guest
media changed, or complete DOS/Win16 regression claimed for this new hash.

### Enrolled class procedure exchange and creation snapshot

Original kernel/class.c::xxxSetClassData(GCL_WNDPROC) changes the CLS
procedure, not existing WND procedures. Original createw.c copies the selected
class procedure into the new window before callbacks (source SHA-256
`DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7`).
The prior native-bound setter still passed GCL_WNDPROC directly to native
USER, bypassing the newly retained logical class target. It now exchanges
that target only when the native class uses our gateway, and keeps the
gateway installed. Ordinary native classes continue to use native USER.

The class metadata API returns a value snapshot instead of a pointer to its
mutable target. Procedure read/exchange is atomic; callback identity remains
immutable. Creation copies the resolved target before native CreateWindowEx,
so later class mutation cannot change the pending window's selected target.
An explicit invalid zero target still reaches the original binding refusal;
it is not confused with an absent optional target. Window/class references
protect storage lifetime. No GUI lock is held over native callbacks. This
does not introduce a replacement class table, message dispatch or selector
encoding algorithm and does not modify any mirror source.

The same bounded raw-target restrictions apply: missing binding, zero, CPD,
recursive gateway and WOW tag without registered callback cannot mutate the
class. Public CPD resolution, ANSI/Unicode conversion and server-procedure
mapping are STILL incomplete; this evidence is limited to enrolled ANSI
gateway classes with resolved raw targets. It cannot justify full
WU32SetClassLong acceptance or a guest-runtime pass.

Formal x86 provider rebuild succeeds, SHA-256
`4366E34E50A3E121377EF473B55E92EE5C1CE8E589E8FD4154DD3BFCD6BDD9C7`;
build log: build/M0-T420/S40/class-registration-r1/class-exchange-build.log.
Hidden current fixtures all exit zero with empty stderr:

- class-procedure.exe:
  `B9F35FA57B71D53ADA0A9ECDEAC862D973923B9A202460BC90E4213983F77C21`,
  errors=0, callbacks=23. Class exchange returns the old logical target;
  two existing windows retain their different procedures and a subsequently
  created third window inherits the new class target. Rejected CPD/recursive
  mutations preserve state; native standard-class passthrough still passes.
- native-mdi.exe:
  `8883FF315EDD99F28BCF5E0AA271D7E1E571BD0E84782D4E4867751C82069ABF`,
  errors=0, callbacks=43, nested=1, rejected=1, vetoed=1, destroyed=7.
- class-menu-owner.exe:
  `7A70DE732C3EB93192F97AAE5ACAE0E4FA3B40932D57BE9633A59629BEC2C785`,
  errors=0, cases=19, captures=0, menus=14/14.
- class-query.exe:
  `F7D7DBD21620E153646EB163AFC6C600F9474DA836F4E6D3876AF823D8115376`,
  errors=0 with original capture and actual worker TLS.

Artifacts remain under O:/winnt/tests/S40USER and stdout/stderr logs under
O:/winnt/logs/s40-<stem>. The serial creation tests prove old/new-window
separation; a forced concurrent/reentrant mutation during native creation
has not been exercised. Production task context, service registration,
original guest callbacks and latest-hash full regression remain open.

### Reentrant class mutation and actual original WOW class setter

The previous untested reentrancy item now has a concrete native run. The
class-procedure fixture changes GCL_WNDPROC during WM_NCCREATE and recursively
creates another window. The pending window retains its old logical procedure;
the recursive window receives the new class target; existing windows keep
their independent targets. Real native creation, callbacks and SendMessage
are used, with only the final WOW callback controlled. This exercises
reentrant mutation after WW publication, not a scheduler-controlled
cross-thread race before CBT publication.

`class-procedure.exe` now returns zero, `WOW_CLASS_PROCEDURE errors=0
callbacks=40`, SHA-256
`BE7CAE44671F8E27B6CCE8728BC34AA2980C95A55D0D3939BE7B893FB1EF9998`.
No production source changed in this verification step; the provider remains
the preceding `4366E34E...` artifact, not newly real-guest validated.

The original-subclass extractor now additionally verifies both selected
mirror and OpenNT wuclass.c SHA-256
`12CDD44DFEC6B74A426165977F941DA86D90BBAD7AA9C59A085010D02528629D`
and extracts the complete unchanged WU32SetClassLong body into the S40 build
root. The expanded original-alias fixture invokes that body through the
formal wow_user_invoke_thunk scope with the actual SETCLASSLONG16 argument
layout and short HWND. Actual native class/window metadata and formal private
Get/SetClassLong bindings are used. This is stronger than calling the adapter
directly but remains host-side integration evidence, not CPU guest execution.

Cases prove original high-selector encoding, returned old guest procedure,
WC.vpfnWndProc updates, switching to a native procedure and back, and unchanged
window-local procedure. The original GCL_CBCLSEXTRA compatibility branch is
also reached: with eight app bytes plus the original two reserved DWORDs,
successive values 5 and 7 return 8 and 5; native cbClsExtra remains 16, while
reserved words hold value 7 and flag 1. Thus the logical class procedure tag
reaches an actual original consumer instead of a test-only predicate.

The existing fixture still controls task TLS and guest thunk allocation/
lookup; the unselected menu-string guest pointer function is an explicit
failure sentinel. It was needed to link the entire original setter rather
than cutting out its other branches; executing that sentinel fails the test.
This does not validate GCL_MENUNAME, arbitrary guest mapping or real callbacks.
The fixture enrolls metadata around a native window to exercise these APIs;
it is not evidence that production RegisterClassWOWA is connected.

`original-class-subclass.exe` exits zero with `WOW_ORIGINAL_ALIAS errors=0
calls=1`, empty stderr, SHA-256
`95C80BC10F729E80FA2821DE5F8F8DB7925C4910E9A08EA46B5A2765B5E87762`.
Runtime artifacts: O:/winnt/tests/S40USER; logs:
O:/winnt/logs/s40-class-procedure.stdout.log and
O:/winnt/logs/s40-original-class-subclass.stdout.log, with matching stderr.
The current-status wording now explicitly distinguishes the earlier 17-route
worker baseline from these undeployed USER candidates. All remaining S40
production context, CPD/Unicode, task, registration and actual guest gates
remain open; neither guest media nor mirror implementation changed here.

### Menu resource client: native-parser equivalence disproved

OPENNT-HOST-043 imports exact CommonLoadMenu/LoadMenuA/W bodies; 044 adds
exact WOWFindResourceExWCover to clres.c. Source hashes and crop boundaries
are registered in opennt-host/README. Formal provider SHA-256:
`755D38C8F524588623DCF6274E4782C5F65E6DE1E2E8136D3CBECA8814E91742`.
It is not deployed or real-Win16 accepted.

Automatic class-menu loading follows createw.c -> xxxClientLoadMenu in
ntcb.h -> LoadMenuW -> CommonLoadMenu -> W32 resource callbacks. Earlier
attribution to W32CreateWindow was incorrect. WU32LoadMenu's materialized
resource is a separate consumer. Original CommonLoadMenu deliberately
unlocks without FreeResource to preserve Win16 owner-draw resource pointers.

Controlled W32 callbacks, imported client bodies and real native menu objects
pass 16 A/W, name/ordinal and resource failure cases. Bitmap regression with
the expanded formal clres object also passes nested=30, parallel=4x50,
named=2, missing=2; unused resource callbacks are fail-fast sentinels.
Bitmap artifact SHA-256:
`0B18C05B3E2BB50094C0E48C09CD6B22E721D5CAF4A7E9E7A83B70D508BB0BC4`.
Earlier missing link objects and missing synthetic bitmap resource were
fixture setup failures, corrected before this pass. None is a guest test.

Original ConvertMenuItems16 writes a packed 16:16 pointer instead of Unicode
text for nonempty owner-draw items. Current wres16.c SHA-256:
`24324D4B95B7C5AC102E9751872AF1DB779EFC0CFF223B32AA066034CB874AE9`.
Original MenuLoadWinTemplates checks TIF_16BIT and reads that DWORD as
itemData, advancing four bytes. Native string parsing is not equivalent.

Retained `menu-resource.exe --owner-draw` proves this defect through the
imported client path: expected itemData 12345678, actual 009A3020, type
00000100, ID 23, child exit 1. Ordinary 16 cases still exit 0. SHA-256:
`096BBE54CC5B906BDFAA04AEE7F4B9948CEF192DCF4CBD8DEE195566BA94E58A`.
Logs: O:/winnt/logs/s40-menu-resource.stdout.log and
O:/winnt/logs/s40-menu-owner-probe.stdout.log, matching empty stderr.
An extra trailing zero bounds the native string walk; this is a synthetic
test template, not modified guest media. The actual host address is incidental;
failure to preserve the packed value is the contract violation.

This is a project binding defect, not an original guest limitation. The same
native parser is used by ADAPTER-WOW-020 and original WU32LoadMenuIndirect.
All three consumers require original parser recovery with actual task flags;
do not fabricate a global TIF_16BIT or count this negative result as a pass.
MenuLoadWinTemplates, MenuLoadChicagoTemplates and CreateMenuFromResource
have been read completely. Preserve recursion, flag validation, popup failure
cleanup, version/header dispatch, help IDs and owner-draw data. Native menu
creation/insertion/destruction/help-ID APIs are candidate object bindings,
not yet verified substitutions. Original cast-postincrement expressions need
registered modern-C adaptation; no new parsing algorithm is warranted.

### Original parser recovery and three-consumer regression

OPENNT-HOST-045 now restores all three parsers in original clmenu.c. Changes
are limited to an explicit BOOL replacing the unavailable TIF_16BIT read,
propagation through recursion, two standard-C pointer increments, and the
WOW-only CommonLoadMenu call supplying TRUE. The native-mode FALSE case is
also tested. This is an invocation contract for the selected WOW entry points,
not a fabricated process-wide TEB flag. MenuLoadChicagoTemplates is unchanged.
ADAPTER-WOW-037 supplies native menu object operations and copied original
private constants/template declarations; it contains no parsing algorithm.

The three consumers now share this owner: resource client, private
load/create-menu slot, and WU32LoadMenuIndirect's redirected API binding
(MVDM-HOST-DIV-298 in precomp.h; the thunk itself is unchanged).
Formal x86 provider built successfully, SHA-256:
`70ED26C912A25701FDE982EE21FD3F84BB67534888B73A9C3EFA0B24C5143A87`.
Existing historical warnings remain; this is not a warning-free build claim.

Final fixture SHA-256:
`FA5FCEE2C794EE7DA2997A2308CFF82444063E66C1C83AC7C0E13D436B3C3FE1`.
Both runs exit 0 with empty stderr. Ordinary run: resource_cases=16,
parser_cases=7, invalid_repeats=64, errors=0. Owner-draw run: expected and
actual itemData both 12345678, errors=0, including actual linked adapter
load/create and indirect-menu functions. Logs use s40-menu-resource and
s40-menu-owner-fixed under O:/winnt/logs.

Additional parser cases cover a popup child, empty owner-draw, native-mode
owner-draw pointer identity, MENUEX, unsupported version, invalid MENUEX
flags and invalid nested MENUEX child. Repetition exercises failure paths
but is not a measured proof of no USER handle leakage. The initial negative
fixture wrongly used 0x8000 as an invalid version-0 flag; original MF_VALID
allows it, so the unterminated test walked out of bounds and exited with
access violation. The fixture was corrected to a version-1 invalid type bit;
the product parser was not changed to accommodate invalid test assumptions.

No original guest bytes changed. Actual WU32LoadMenuIndirect guest execution,
WM_MEASUREITEM/WM_DRAWITEM callbacks, production class loading, newest-provider
DOS regressions and full S40 runtime acceptance remain open. These passes
prove bounded parser/resource/adapter behavior, not completed WOW32 recovery.

### Actual original menu thunks and conversion chain

`extract-wow-menu-fixture.ps1` pins both mirror and original source hashes:
wumenu.c `0FC60E98713F9C37F6FEE8C5DF25DCBA55277DBAEF5B82C49A90E62AE6DB72C6`,
wres16.c `24324D4B95B7C5AC102E9751872AF1DB779EFC0CFF223B32AA066034CB874AE9`.
It extracts complete unchanged WU32LoadMenu/WU32LoadMenuIndirect,
ConvertMenu16/ConvertMenuItems16 and original conversion macros. The fixture
uses formal provider compile flags and original VDMFRAME/argument layouts,
not an independently reimplemented thunk or conversion algorithm.

Real original conversion of a synthetic ANSI owner-draw resource at guest
address 1234:0020 yields itemData 1234:0028 through both original consumers.
Actual linked ADAPTER-WOW-020/037 and original parser objects create a real
native HMENU. Original temporary allocation/free ordering is exercised;
the native menu retains the guest value after conversion storage is freed.
Direct name/ordinal and indirect calls pass, including injected allocation
failure before dispatch. Named calls exercise actual MBToWCS and original
LocalFree cleanup. Six cases pass with allocations=0, child exit 0 and empty
stderr. Final menu-thunks.exe SHA-256:
`30F4CE70109B85C78ABEF985CC248A4FEF83934C29B19234F7A0AA11CAD48631`.
Logs: O:/winnt/logs/s40-menu-thunks.stdout.log and matching stderr; fixture
is under O:/winnt/tests/S40USER. No production source changed in this step.

Explicit controlled boundaries: GetPModeVDMPointerAssert maps the synthetic
resource/name; GetExePtr16 supplies the selected module; malloc_w/free_w use
a counted native heap with failure injection; the output observer records
the full handle before original USER16 truncation. Unselected W32 resource
callbacks raise a noncontinuable exception if reached. This is not CPU guest
execution, production FlatAddress publication, real thunk dispatch-table
registration, short-handle reuse, or WM_MEASUREITEM/WM_DRAWITEM guest callback
acceptance. None of those gates is waived by this result.

### Formal original class-client composition

OPENNT-HOST-046 brings original RegisterClassWOWA and the retained ntcftxt.h
ANSI closure into the formal provider, using a second compile of the existing
client.c owner. Original ntsend COPYLPSTRIDW and selected ANSI aliases are
restored in their existing header. The original bodies continue to own style,
version, capture, menu ownership and pre-3.1 BOOL return policy. Existing
registered two-free capture-failure correction remains the only ntcftxt body
change. No invented mirror file or parallel registration policy is added.

ADAPTER-WOW-038 replaces the fixture's kernel registration double with the
existing native class-word backend. An explicit invocation context carries
task expected version, module-version callback, class context and WOW callback;
thread-local binding is strictly synchronous and restored in finally. The
boundary requires the admitted ANSI WOW path (no alternate worker/fnid).
Native registration substitutes the gateway and leaves automatic native menu
loading unset; original client menu names remain retained for subsequent
WOW creation. The caller must serialize the class context and own its lifetime.

Formal x86 provider links, SHA-256:
`997C9552D7CBA5BACB4928F1F96A1D9741D8E7D8FEC4B7836D90DC22B967628D`.
The first compile found an omitted original header include path; source paths
were fixed without changing original validation policy. Existing unrelated
historical warnings remain.

The new class-client fixture links actual formal class-client, capture, worker
TLS and native class storage objects. It checks successful registration and
retained target/words, no native packed-module menu loading, duplicate refusal,
unregister cleanup, pre-3.1 BOOL return, invalid style refusal and scope restore
after a version callback exception. Child exit 0, errors=0, version_calls=8,
empty stderr. SHA-256:
`7DBCA57FEFFEF880603D466D71FD3DCFA5E1A27179CE8FBEEECD6CB60D801690`.
Artifact: O:/winnt/tests/S40USER/class-client.exe; logs:
O:/winnt/logs/s40-class-client.stdout.log and matching stderr.

Task/module versions and WOW callback are controlled inputs in this fixture;
the callback fails if called because no window is created. This is not
InitTask, USER output-table registration, class-menu creation, real guest
callbacks, or full worker acceptance. Those remain required. The formal
provider still imports native UserRegisterWowHandlers; the new composition
must not be described as an already connected production registration slot.

### Original task initialization and priority ordering recovery

Re-read original client InitTask, kernel queue.c::xxxInitTask, complete
taskman.c (744 lines) and W32DestroyTask. The expected version comes from
WOWINITTASK16 into InitTask and is stored in both USER THREADINFO and its
CLIENTINFO. WOW32 TD is not the USER task-order record. Initialization also
owns name/startup placement/hotkey, task flags, queue attachment, task events,
priority insertion and exclusive execution ownership. Cleanup includes both
original WOW32 list removal and USER resources/scheduler transitions. A
version-only successful InitTask would repeat the removed incomplete stub.

OPENNT-HOST-047 recovers the byte-exact InsertTask body at its original path.
All other taskman functions remain required but are not imported under fake
event/message bindings. Package/source/outgoing-boundary disposition is in
opennt-host/README. ADAPTER-WOW-039 contains only the three finite field views;
it introduces no independent policy, task count, table capacity or executor.
Original taskman SHA-256:
`E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E`.
Formal provider builds as
`00C5C0966C2E6F9E352150EB63ADE313CC5C78C28598AF18B67D8C519E906228`.

The fixture links the formal InsertTask object. Five sequences verify stable
priority insertion, equal-priority reinsertion, moving an existing tail to
the front after promotion, demoting the head, and repeat insertion without
duplicate linkage. Child exit 0, errors=0, empty stderr; artifact SHA-256:
`DF3DEDAFB7F2653DA777065F8F950C632114FB9F56E0E57EBA733602ED8CDF19`.
Artifact and logs: O:/winnt/tests/S40USER/task-order.exe and
O:/winnt/logs/s40-task-order.stdout.log (matching stderr).
The source's retail CheckCritIn/UserAssert are non-evaluating; this does not
prove or provide concurrency protection. No production caller has yet been
connected, no Win16 task has executed through it, and no scheduler lifecycle
gate is closed. This step is original-algorithm recovery, not completion of
InitTask or a narrower definition of S40.

### Original send/reply transitions, wake predicate and task lock

OPENNT-HOST-048 adds unchanged WakeWowTask and DirectedScheduleTask plus
CurrentTaskLock with only an explicit current-process argument replacing
PpiCurrent. The existing taskman.c source identity applies. ADAPTER-WOW-039
extends the finite view with original event counts, scheduled thread, message
flags, task-lock/send/receive counts and event handle. All peer process views
must share one event-counter domain, corresponding to original gpsi; this is
not one independent counter per process. KeSetEvent's ignored-result call is
bound to native SetEvent. No wait, peer-selection algorithm or executor was
invented, and these functions are not yet connected to runtime SMS delivery.

Formal x86 provider builds as SHA-256
`07DDD1A227FBD580E342676EC51D5DC9D3AA8943772955984E0B4B69B11D6B1F`.
Expanded task-order fixture exits 0, errors=0, sequences=5, send_reply=4,
locks=6, empty stderr; SHA-256
`29B8D4ABCF3304445794083884B36AE841D0438870D71B6837772B14E5B5387E`.
It checks send/reply counter and message-flag balance, transient priority
boost restoration, event signaling only with no scheduled task, nested lock
cookies, invalid-cookie retention and absent WOW owner. Native-thread cases
have a NULL pwpi; they are not disguised WOW threads. Events are real native
auto-reset events and closed at test end. The process/thread/message records
are controlled views within one host fixture, not actual cross-process SMS
or a concurrent guest scheduler. Artifacts/logs retain the task-order paths.

Original userk.h::EnterWowCritSect/ExitWowCritSect were also inspected:
they track/assert CSLockCount and CSOwningThread; they do not acquire a mutex.
Actual mutual exclusion depends on USER's outer critical section and event
scheduling. Recovering those two helpers alone cannot prove serialized CCPU
execution. Full initialization, message/wait integration, outer lock release
and reacquisition, task destruction and real guest acceptance remain open.

### Original task destruction and pseudo-event ownership

OPENNT-HOST-049 recovers exact DestroyTask in taskman.c plus original
CLOSE_PSEUDO_EVENT and ExitWowCritSect in the original userk.h path. The
registered header source hash and finite ownership contract are in the mirror
README. Shared domain macros refer to the explicit process, not a second
global task manager. HeapFree binds the original pool-release calls; idle
events are independently retained native handles, not borrowed caller handles.
The original pseudo-event macro preserves ON/OFF sentinels and signals a
real event before releasing its reference. No task selection algorithm changes.

Formal provider builds, SHA-256:
`F728F3810EA50BDE6A99AA93AE81868B90B5FADF7D388C43B6D5813A6178B62B`.
The expanded fixture uses actual heap records, native events and a duplicated
observer handle. It verifies waiter/task unlinking, event-count subtraction,
retained-handle closure, inactive-task exit, active-successor wake, fallback
WOWEXEC wake, final-task exit, native active-thread exit without TDB and both
pseudo-event sentinels. Prior ordering/send/reply/task-lock tests also pass.
Final child exit 0, errors=0, sequences=5, send_reply=4, locks=6,
destruction=7; empty stderr. SHA-256:
`9C9600BEF76FCFFBF8DBF6BEF3253643E63FAE0309CA905798FC7B86184729E5`.
Artifact/logs retain O:/winnt/tests/S40USER/task-order.exe and
O:/winnt/logs/s40-task-order.stdout.log plus matching stderr.

An initial sentinel assertion assumed a previously closed numeric handle
would remain invalid after creating new events; Windows reused it. The test
was corrected to check real event retention and observer state instead of
numeric non-reuse. Product code was not altered for this test assumption.
Original DestroyTask frees ptdb without clearing the pointer in the surviving
thread view; complete thread teardown must retire the view and must not call
destruction again. This is not an idempotent destructor claim. Real W32DestroyTask
hookup, concurrent waits/messages, initialization and guest cleanup remain open.

### Original SleepTask and UserYield with native waits

OPENNT-HOST-050 recovers complete xxxSleepTask and xxxUserYield, changing
only the explicit current-thread argument/read and UserYield passing it.
Selection, rescan, counters, send/reply correction and return policy remain
original. Exact EnterWowCritSect is restored; it tracks ownership rather
than acquiring synchronization. Required host operations provide the outer
lock, messages, death/APC and idle semantics; no production defaults exist.
The original alertable WaitAny without timeout maps to native multi-event
wait. Original five-slot array allocation remains thread-owned. NT4 queue
masks explicitly exclude modern QS_RAWINPUT; WOWEXEC fsWakeMask is 20ff.

Formal provider SHA-256:
`EAEED012A6E04BC0787D0840ECDA04DB69CE5E0F2E592BA2B06FB40C348A6FA1`.
Fixture SHA-256:
`675483C52E0612080ED9147EE8767A977DDE24957F210FF7615B78B9BA1BC5C7`.
Child exit 0, errors=0, sequences=5, send_reply=4, locks=6, destruction=7,
waits=4, empty stderr. Existing task-order artifact/log paths apply.
New cases use actual native events, CRITICAL_SECTION and another host thread:
self-reschedule; original receive/wait/receive UserYield ordering; asynchronous
WOWEXEC wake returning TRUE versus ordinary FALSE; and REMOVEME returning
without waiting or restoring execution ownership. Lock/death-check counts,
event counts, queue mask and resulting ownership pass.

Initially the fixture established WOWEXEC handles too late. Original NULL
comparison selected the WOWEXEC path during an ordinary wait and caused
invalid-handle waits. Correcting initialization, not original scheduling,
resolved the extra waits. A compile conflict with NT's leave macro was fixed
by naming the host operation release_lock. These were fixture/binding errors.
Execution approval questioned whether the test EXE was guest media; read-only
path/hash checks proved it was the previously generated independent host
fixture, not a reparse point or guest component. The same operation then
proceeded with that evidence. No alternative location bypassed the check.

Receive/death callbacks are controlled observers; idle hooks and APC delivery
fail if unexpectedly reached. This does not test actual SMS, host queue
publication, task death, or WOW guest execution. Production registration,
initialization, host operations, full DOS regression and real Win16 acceptance
remain open; native waiting is not a full scheduler completion claim.

### Original DirectedYield and two-thread scheduling handoff

OPENNT-HOST-051 restores xxxDirectedYield from the pinned original taskman.c
(E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E).
Source comparison passed after exactly three substitutions: explicit current
thread parameter, PtiCurrent read replaced by that parameter, and passing it
to the recovered SleepTask. No event/priority/error branch was changed.
The required ADAPTER-WOW-039 lookup receives an existing native thread ID and
returns a borrowed view under the domain lock; no production ID registry,
lookup default, or Sleep(0) approximation is introduced.

The formal x86 provider builds with its pre-existing DESCRIPTION/duplicate
export warnings. SHA-256:
`56168FA11711BB8F73B902449253FE4CF8BF131321081CB5C8C160628003AEC6`.
The /MT /Gz /W4 /WX fixture links the actual formal taskman.obj. SHA-256:
`829FCA31F96BDCADD4DC8F3B2532957D627B889576A9725B600BE57A36B711BF`.
O:/winnt/tests/S40USER/task-order.exe exits 0; stdout at
O:/winnt/logs/s40-task-order.stdout.log reports errors=0, sequences=5,
send_reply=4, locks=6, destruction=7, waits=4, directed=6. Stderr is empty.

New cases cover OldYield without lookup, self-directed yield, a native target
without TDB, missing target, invalid native caller and two native threads.
Missing-target return retains the caller's posted event exactly as original;
self-directed yield posts twice and consumes once. Tests drain those remaining
events using original SleepTask, not an adapter counter reset. In the actual
two-thread case, the helper's real CreateThread ID is the lookup input; the
original scheduler selects it, it consumes its event through original SleepTask,
then REMOVEME relinquishes execution so the first thread resumes. Shared
counters return to zero, priority returns to ten and ownership returns to the
first thread. Locking and event waits are real, while lookup/message/death
operations still use controlled fixture views/observers.

All taskman function bodies are now represented, but this is not full USER
composition or runtime registration. Required production host operations,
InitTask/process lifetime and the twenty-slot output-table hookup still have
no acceptance evidence. Full DOS regression and real immutable Win16 execution
remain mandatory. This run replaced only the independently generated host
fixture, not product EXEs/DLLs or any guest medium; no commit/push occurred.

### Production task ownership and registration gate audit

Read-only source/map audit after OPENNT-HOST-051 establishes a concrete
remaining integration boundary, not a failing scheduler algorithm:

| Stage | Original owner and required state | Current disposition |
| --- | --- | --- |
| Process registration | exitwin.c:508 xxxRegisterUserHungAppHandlers allocates zeroed WOWPROCESSINFO, references WOWEXEC event, records client event/callback and links the process domain. | No production binding; cannot substitute a non-null-argument success check. |
| Task initialization | queue.c:658 xxxInitTask captures name, startup placement/hotkey, shared-WOW idle record, thread/client flags and version, queue attachment and compatibility flags, then inserts TDB and makes it active. | No production binding; class registration needs this version/context, not a fixed Win3.1 default. |
| Module/task resource cleanup | cleanup.c:182 _WOWCleanup handles selector-based procedure invalidation, class deferred destruction and task window procedure retirement; wkman.c:1725 calls it before deleting WOW32 TD. | Not equivalent to DestroyTask and must not free the scheduler TDB at this callback. |
| USER thread destruction | queue.c:1989 invokes DestroyTask after window and send-message cleanup. | Recovered original function is only fixture-called. Requires explicit USER thread lifecycle attachment. |
| Process destruction | queue.c:3741 releases WOWEXEC event, unlinks process and frees WOWPROCESSINFO. | Must follow thread retirement, not module unload. |

Original source hashes: queue.c
`0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8`;
exitwin.c `45B92F7236FBA49D58E2C3D912126067C3843EC560C28CDD8A20167C62AAEF65`;
cleanup.c `DFB0FB8D81AFEB555B3631ADC5456F4CEB13142F2E954A6F2B22631C49142E5C`.
These are research owners, not wholesale USER-server import admission.

The existing worker already provides session_register_thread_hook and reverse
unbind callbacks. No production caller currently registers one. host_ExitThread
in original nt_thred.c calls ExitThread after CCPU cleanup; the worker build
force-includes thread_start_compat.h, mapping that call to opennt_exit_thread,
which unbinds the session before native ExitThread. This is an existing candidate
attachment point, not proof of complete USER cleanup: normal returns, exception
paths, main WOW thread, provider unload order and pending callbacks must all be
verified. A hook must distinguish threads that actually acquired USER task state
from ordinary timer/COM/CPU threads. Do not add another generic thread manager
or map every unbind directly to DestroyTask.

The formal map still resolves UserRegisterWowHandlers@8 to
legacy-wow-user32:USER32.DLL, not the imported original registration body.
The output-composition observation now reports this explicitly as
external-dll-import and separately identifies four recovered core symbols
(DirectedYield, UserYield, SleepTask and renamed original class registration).
Core ABI presence does not establish a callable output slot.
Running verify-wow-output-composition.ps1 succeeds as an observation with all
20 source/ABI slots accounted for; -RequireLocalRegistration correctly fails
on this same provider. The retained expected-negative log is
build/M0-T420/S40/class-registration-r1/output-registration-gate.log.
Local registration is only a necessary gate, never sufficient runtime evidence.

Next production work must bind process/task initialization and ordered resource,
thread and process teardown as one lifetime contract, then publish the recovered
callbacks through the original-shaped registration boundary. No success stubs
or early TDB destruction are acceptable shortcuts. This audit changes no
product runtime or guest media and does not close S40.

### Original process registration and retained event binding

OPENNT-HOST-052 imports only xxxRegisterUserHungAppHandlers from the pinned
exitwin.c into its original path. Automated source comparison passes with
exactly two changes: explicit process parameter and PpiCurrent replaced by it.
Original allocation, zeroing, failure free, event/client callback recording and
linked-list publication are preserved. WOWPROCESSINFO views gain only the
original next-process and callback fields plus their shared-domain list head.
The original API is once-per-process registration, not an idempotent API.

ADAPTER-WOW-040 owns the unavailable ObReferenceObjectByHandle operation:
DuplicateHandle with DUPLICATE_SAME_ACCESS retains the object, SDK public
NtQueryObject records prove original EVENT_ALL_ACCESS rights and Event type,
and all failure paths release query memory and the retained handle. The
original NULL type argument did not validate Event type; the new check is
explicitly a binding safety condition for the later event operations. No
kernel-private layout, access upgrade or error-code translation is added.
The source consumes Boolean success only. The native type query is dynamically
resolved from the already loaded NTDLL, not a new injected provider.

Formal x86 provider build passes with existing link warnings. SHA-256:
`C697D0745BA8710DBA0024627BC8E22DA634F7D19726FE60B3EC579F94A57479`.
The expanded /MT /Gz /W4 /WX fixture links actual formal taskman.obj,
exitwin.obj and wow_task_event_binding.obj; SHA-256:
`6FE392A128DA5B9D537DB41A69210F45144177CB4ECDAD2B0BDE427E5A5823C0`.
Child exit 0 and empty stderr; existing task-order stdout reports all prior
cases plus registration=6 and errors=0. Negative cases repeat 32 times each:
NULL handle, synchronize-only event, semaphore and process pseudo-handle.
No process/list publication occurs and native handle count stays unchanged.
Positive registration preserves zero state and callback/client-event identity,
links two process views, accepts the original NULL callback form, and retains
event behavior after the original caller handle closes. Releasing one process
reference leaves the other's event usable. Retirement here is fixture-owned,
not proof of production process teardown; allocation-failure injection is not
claimed. Test artifact/log locations remain the independent S40USER fixture.

The composition observer now names this fifth recovered core separately from
the still-missing output-table wrapper. No production USER registration,
task initialization or Win16 acceptance is asserted. Guest and formal runtime
package remain untouched; S40 is still open and this work is uncommitted.

### InitTask integration: distinct version and queue contracts

The current class context's task version and module-version callback are not
duplicate policy. Original queue.c::xxxInitTask publishes its dwExpWinVer to
THREADINFO and CLIENTINFO. Original ntcftxt.h::RegisterClassExWOW first uses
GetClientInfo()->dwExpWinVer to reject USER hInstance for version 4.0 tasks;
then independently calls GETEXPWINVER(hInstance) for invalid style/brush
compatibility and pre-3.1 return policy. Collapsing these values into one
constant, or deriving task version from the class module, changes original
semantics. The binding header now explicitly documents their distinct owners.

The actual-original class-client fixture now tests all four combinations of
task 3.1/4.0 and module 3.1/4.0 with invalid class style bits: old module strips
the bits and registers, new module rejects them, independent of task version.
A fifth case proves task 4.0 rejects USER hInstance before querying even an
old module version. Existing resource ownership, native class publication,
duplicate rejection and exception scope restoration checks continue to pass.
Child exit 0, errors=0, version_calls=14, version_domains=5; empty stderr.
Artifact O:/winnt/tests/S40USER/class-client.exe SHA-256:
`18B313609917506C1ED186D12112CA12D79BBB14BB9ED45733A0BA70C300B7BE`.
Logs: O:/winnt/logs/s40-class-client.stdout.log and matching stderr.
Current rebuilt provider SHA-256:
`4CF91F374281019D45C78F25AF27565F72D00547DA3758EDACC966F980442720`.
This is controlled task-context input, not a real InitTask/guest pass.

Queue integration also has a concrete original requirement, not just a
successful ReattachThreads return: input.c's attachment closure automatically
groups TIF_16BIT threads sharing the same PROCESSINFO, along with explicit
attachment requests. ReattachThreads retains foreground/active queue identity
and redistributes input; it explicitly must not invoke callbacks/send messages
during attachment. Its source SHA-256 is
`B7656BCA55E892A42BA825BE0324211450AD298C40717E1AE5A889D084467529`.
A public AttachThreadInput binding is only a candidate host mechanism, not
proof of this whole contract. Production InitTask must publish the one actual
task version to every consumer and establish shared input ownership before
activation; it must not merely allocate a TDB then return success. No new USER
server code or guest changes were made during this investigation.

### Original InitTask and compatibility/startup policy recovery

OPENNT-HOST-053 imports a 353-line queue.c carrier at its original path:
complete xxxInitTask, SetAppCompatFlags and _ShowStartGlass from the pinned
0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8 source.
Source comparison confirms all three bodies with exactly six substitutions:
explicit current-thread/process parameters and reads, passing the process to
_ShowStartGlass, and reading the captured process image name instead of the
kernel ETHREAD/PEB fallback. Original name allocation/capture, startup flags,
hotkey/placement, idle-record lookup, queue-attachment ordering, compatibility
lookup/parsing/publication, TDB insertion and activation are retained.

ADAPTER-WOW-041 adds only finite field views and required profile/journal/
reattachment/cursor operations, not implementations of those host services.
The existing task/process views gain the original consumed fields, rather than
introducing a second task registry. Original USERSTARTUPINFO layout differs
from native STARTUPINFO and is retained as its own view. USER kernel TEXT
literals are Unicode even though WOW32 client composition is ANSI. The first
compile caught that boundary mismatch and a missing STATUS_OBJECT_NAME_INVALID
declaration; fixing the adapter macros, not original bodies, resolved both.
Native RtlUnicodeStringToInteger is added to the existing NTDLL import alias
composition; no replacement integer parser or compatibility database is added.

Formal x86 provider builds with the existing link warnings. SHA-256:
`7266927AC4229B6376F058BF1DB27E87C77142C35E53410126CD8157C4B4FBD6`.
Task-order fixture links actual formal queue/taskman/exitwin/event objects
plus native NTDLL; SHA-256:
`5BE087EAB503E1FA1B1001037D6342F45EA9F341C2472EA07370C18B66104621`.
Child exit 0, errors=0, all previous cases retained plus init=6; empty stderr.
Existing O:/winnt/tests/S40USER/task-order.exe and s40-task-order log paths apply.

Cases cover separate task/default placement, shared task with matched idle
record and explicit placement, shared special ID -1/version 4.0, journal
record/playback suppressing reattachment, and invalid PAGE_NOACCESS name memory
followed by successful retry. They check name contents/ownership, task/client
version and flags, hotkey/show-window/position/size policy, actual original
hex compatibility parsing, ordering of queue and cursor calls, event counts,
priority and activation. Name capture exception leaves no TDB/name/startup-list
publication. Actual registration allocates the process and the original task
destructor retires its TDB; remaining fixture-owned name/process references
are released explicitly. Allocation exhaustion and real native input/cursor
services are not tested here. Profile, journal and presentation are controlled
observers; this remains a boundary fixture, not Win16 execution.

The output-composition observer now distinguishes the recovered InitTask core
from its still-unbound original output-slot ABI. Production context binding,
native operations, complete ordered teardown, USER shared-data publication,
real Win16 and DOS regression remain required. No source recovery above permits
publishing an incomplete success table. Governance and diff-whitespace checks
pass; no product deployment, guest modification or commit/push occurred.

### Original host profile capacity defect: red/green verification

OPENNT-HOST-054 changes only SetAppCompatFlags's profile output capacity from
sizeof(szHex) to sizeof(szHex)/sizeof(szHex[0]). The original array holds 80
WCHARs, whereas sizeof reports 160 bytes. Original profile.c::FastGetProfileStringW
receives cchBuf, allocates cchBuf*sizeof(WCHAR) plus its query header, sets the
last WCHAR at index cchBuf-1 to zero and copies to the caller with wcscpy.
The intervening UT_ wrapper forwards capacity unchanged. Thus a sufficiently
long configuration value can exceed the caller's array. profile.c SHA-256:
`8ECBB58954ECA0FECE4AB180BB56A3953B8B4408400D3C7AA01D50B62CAAA531`.
This is an original USER host defect, not a guest defect and not introduced
by prior standalone code. No historical WRITE causality is asserted.

The guarded negative fixture checks the original declared array bound before
any output write. Against the unfixed formal queue.obj, child exit 1 reports
PROFILE_CAPACITY_MISMATCH declared=80 requested=160 on every reached old-task
profile query. It intentionally refuses the unsafe write; no stack overwrite
or exploit was attempted. Red fixture SHA-256:
`357F26E05EFD015F47D01189B5C9BB61FA7343688470E0EBF2DFA3560B59C0AE`.
Its retained logs are O:/winnt/logs/s40-profile-capacity-red.stdout.log and
matching stderr. Subsequent assertions fail because the guarded provider
returns no data; those are expected secondary failures, not separate defects.

With the one-expression fix, the same capacity guard and all prior cases pass.
A seventh initialization case models truncating a 199-character numeric value
to the advertised 80-WCHAR destination and returning the full original data
length, preserving the historical reader's return convention. The original
native integer parser handles the bounded NUL-terminated output. This uses
a controlled profile callback, not a real registry write or a claim of having
executed profile.c. Original source proves units; the fixture proves the actual
compiled caller argument and its consumption. Existing short-value behavior
remains unchanged.

Formal provider SHA-256:
`E76838E97A274880D810AB0460069F0E77506E22826C6BD8948D3BBFAB2BE7BF`.
Green fixture SHA-256:
`E5D92D52102463E062FD716376E9E1294A87BF1A124ABBC7ED6A970210468614`.
Child exit 0, errors=0, init=7 plus every prior task-order group; empty stderr.
Green logs retain the ordinary s40-task-order paths. The imported USER subset
has only this FastGetProfileStringW-family consumer and its binding macro;
the analogous native profile-reading implementation has not yet been imported.
This fix does not complete the native profile/input/cursor binding, production
registration, Win16 or DOS acceptance. Guest and formal runtime remain unchanged.

### Read-only native compatibility profile binding

OPENNT-HOST-055 composes exact FastGetProfileStringW from the pinned profile.c;
automated body comparison passes unchanged. ADAPTER-WOW-042 binds only
PMAP_COMPAT to the original machine path Software/Microsoft/Windows NT/
CurrentVersion/Compatibility, with KEY_READ and explicit native-machine view.
Other sections are rejected. Native key query and RegCloseKey retain ownership;
default and returned-length rules remain in original source. This machine-only
family does not need the original per-user impersonation/mapping shell.
No registry key or value was created, deleted or modified.

OPENNT-HOST-056 imports original partial-value record and information-class
enum declarations. Compilation exposed an existing nt.h macro bug:
KeyValueFullInformation was 2, but the original enum defines Full=1, Partial=2.
The macro is corrected to 1; the profile composition undefines the macro and
uses the original enum. Previously registered worker consumers still require
full product regression after this shared-header correction. The change is
an adapter ABI correction, not an original-source bug or proof of a previous
application failure. The query operation uses NtQueryValueKey's native ABI,
not a synthesized Win32-to-NT query record.

Read-only inspection found PMAP_COMPAT absent in both 32-bit and native 64-bit
views. The fixture independently verifies absence and then executes the actual
formal original reader and native binding for 32 rounds of empty-default,
explicit-default and NULL-default cases, plus four invalid-entry cases.
No successful existing-value query or truncation branch is claimed. Static
assertions cover Full/Partial enum values and partial-data offset 12.
The initial fixture's lstrcmpW assertion introduced one extra process handle;
changing only that assertion to exact memcmp left before/after counts at 98/98.
No production code was changed to hide the observation, and the exact identity
of that comparison-related retained handle was not inspected.

Final fixture child exit 0, errors=0, native_absence=1, repeats=32, invalid=4;
empty stderr. O:/winnt/tests/S40USER/task-profile.exe SHA-256:
`DEA745164BEF377DD4475A47D2DB7E540B471AB64368DB289ADC6B20466261C5`.
Logs: O:/winnt/logs/s40-task-profile.stdout.log and matching stderr.
Formal provider SHA-256:
`4350212842332828CFF49AE4AEA7AF2E671A69E3699A2965983715636641686A`.
The shared header triggered 115 build actions; formal x86 build succeeded
with existing legacy warnings. The fixture compiles with /MT /Gz /W4 /WX.
The first fixture compile replaced constant-condition checks with compile-time
ABI assertions; the unsuccessful build produced no runnable artifact.

This removes the need to invent default/profile parsing policy, but the
callback is not yet installed into a production InitTask context. Native queue
attachment, presentation, ordered teardown, shared USER data and output-table
registration, positive query coverage and full DOS/Win16 acceptance remain
open. No product deployment, guest mutation or commit/push occurred.

### Real profile query/truncation and InitTask integration

The profile fixture now has a separate WOW_PROFILE_EXISTING_KEY composition:
it links the actual formal original profile.obj and native NTDLL, but supplies
a test-only opening boundary for the existing read-only machine CurrentVersion
key. It does not link or change the production PMAP_COMPAT opening adapter.
RegQueryValueExW independently verifies ProductName is a bounded REG_SZ and a
fixed test value is absent before the checks. No value content is logged and
no registry write occurs. This permits real successful NtQueryValueKey and
STATUS_BUFFER_OVERFLOW paths despite the product compatibility key being absent.

Thirty-two rounds verify full string/returned length, capacities two and one,
two-sided WCHAR sentinels, termination, full untruncated length on truncation,
missing-value explicit default and NULL default, and unchanged handle count.
All pass. An initial compile hit Windows' small macro as a fixture variable;
renaming it guarded fixed only the test source. The original reader is unchanged.

The task-order fixture's eighth initialization case installs the real
wow_task_profile callback, linking original InitTask -> SetAppCompatFlags ->
production read-only adapter -> original FastGetProfileStringW. The observed
native absent-key path publishes zero compatibility flags to thread, client
and process. Queue/journal/cursor operations remain fixture observers, so this
is a real profile dependency integration, not a production USER registration
or complete InitTask acceptance claim.

Latest formal provider SHA-256:
`D466E8163A3F5AFECAB02E9D7128A5D7E3BAC9B6938079101D12E24093ABA8CC`.
All three fixtures were built from latest formal objects and exit 0 with
empty stderr:

- task-order.exe: errors=0, all prior groups, init=8;
  `D17CEC13A014D2A5E7642DE29521561AA3DD9D59806C39FB94D8FCCE29F3F6A4`.
- task-profile.exe: errors=0, repeats=32, invalid=4, handles 98/98;
  `212629979563E0C66124E1B7C247E039BE821F7526E692318F8C31DF89A4DDD1`.
- task-profile-existing.exe: errors=0, repeats=32, success=1, truncation=2,
  missing=2; `DCE66C7C9AB2381A0B9F02B87ED7605EB2E2C39ED0F56702EF3B0B496DDB2E5B`.

Artifacts are under O:/winnt/tests/S40USER; logs use the corresponding
O:/winnt/logs/s40-<stem>.stdout.log and stderr names. Profile allocation-failure
injection and malformed/non-string registry data remain untested; no broader
profile family is admitted. Actual task context/queue/presentation/teardown,
USER shared-data publication, callback registration and full DOS/Win16 tests
remain open. Product package and guest media remain untouched; no delivery
commit or push is claimed.

### Native input-group mechanism and peer-thread exit

wow_native_input_group_fixture.c creates exactly two private USER input queues
inside its own process using PeekMessageW. It does not create visible windows,
attach to user/Console threads, call SendInput or touch guest state. SetKeyboardState
modifies only those test queues. Native event handshakes serialize observations.

Actual AttachThreadInput passes the following boundaries: independently set
keyboard tables are isolated before attachment; the peer sees changes while
attached; a different key remains isolated after detachment; self-attachment
is rejected. A second attachment followed by the helper's normal return lets
the remaining thread keep using its input state; attachment to the exited
thread is rejected while its handle remains retained. No TerminateThread or
forced user-process termination was used.

The test also observes after_attach_a=0: the local artificial key-down state
does not survive attachment. Original RecalcThreadAttachment/Recalc2 groups
same-process TIF_16BIT peers and explicit links; original AttachToQueue separately
transfers focus, activation, capture, caret, cursor counts and pending input.
Original AllocQueue in queue.c initializes a new key-state table from an
explicit source or asynchronous state. Therefore native state reset alone
does not prove a divergence or authorize restoring a saved keyboard table.
No such workaround is added. Physical input, focus/capture transfers, journal
hooks, pending-message redistribution and callback non-reentrancy are not
covered by this windowless mechanism test.

The fixture compiles x86 /MT /Gz /W4 /WX and exits 0, errors=0, isolated=2,
shared=1, self_reject=1, peer_exit=1, empty stderr. Artifact:
O:/winnt/tests/S40USER/input-group.exe, SHA-256
`6B6C401AEF902E9556A8F5ADAA6CD55A2259AAEEAEA9E434005362D644B241D5`.
Logs: O:/winnt/logs/s40-input-group.stdout.log and matching stderr.
This supports AttachThreadInput as a finite native mechanism candidate, not
a completed replacement for original ReattachThreads. No production source,
formal product deployment, guest media, commit or push changed in this run.
Production attachment must still derive membership from the original task
owner and integrate failure/exit ordering with registration and teardown.

### Input-group lifetime: middle exit followed by a new member

The expanded private-thread probe disproves the sufficiency of simply attaching
each new WOW thread to one surviving thread. Start with native links A-B and
B-C. After B exits normally, A and C still share keyboard state. However, when
a new D joins through A-D, C loses sharing: observed old-peer=0, newcomer=1.
The initial desired-postcondition test fails (child exit 1), SHA-256
`517C103CEBA829C976F372701F60513140CA75BD8108BE9C060A2CF82A6F393E`.
Its evidence is preserved at O:/winnt/logs/s40-input-group-naive-red.stdout.log.
This is a native mechanism behavior exposed by an incomplete binding strategy,
not a guest defect or a demonstrated defect in the current product runtime.

The comparison path explicitly connects surviving A-C before adding D; then
both old and new members share as required. Connecting A-C after the negative
case also restores both members. The final fixture retains the naive split as
an expected-negative observation and independently asserts the maintained
membership path. It does not relabel the naive strategy as correct. Final
child exit 0, errors=0, naive_join_split=1, maintained_join=1; empty stderr.
SHA-256 `63BB3B28CA4B93572949886133EE1E75A8AAE86B835C9DBB46D552D78F6F2B6B`.
Artifact/logs remain S40USER/input-group.exe and s40-input-group stdout/stderr.

Original input.c::Recalc2 explicitly recalculates the equivalence group of
same-PROCESSINFO TIF_16BIT threads, independently of surviving explicit links.
The standalone adapter must maintain native links for that original membership
across joins/exits; retaining apparent sharing after one thread exits is not
sufficient evidence. This does not authorize a new task-ID table, copying
keyboard snapshots or importing the full USER server. Native link ownership
and failure rollback belong to the binding; desired task grouping stays with
the original source owner.

Another ordering constraint matters for implementation: original xxxInitTask
sets the 16-bit flag and calls ReattachThreads before InsertTask. Therefore
enumerating only the scheduler TDB list would miss the newly initializing
thread. Original USER has already linked that thread into the process/thread
membership list. Production recovery must provide that earlier thread view,
not delay attachment until the first Yield or move original insertion order.
These tests are private native mechanism evidence, not production attachment,
focus/capture/message-transfer or Win16 acceptance. No product code or guest
media changed, and S40 remains open.

### Native input attachment record ownership

The private native fixture now covers two attachments of the same thread
pair. One detach preserves shared keyboard state; the second detaches the
pair; a third removal is rejected. This matches original input.c
_AttachThreadInput, which allocates a record for each attachment and removes
only one matching record per detach, returning FALSE without a match.
No adapter reference-count emulation is justified by this evidence.

The final x86 /MT /Gz /W4 /WX fixture exits 0 with errors=0 and repeated=1;
the earlier isolation, middle-exit expected negative and maintained-membership
checks also pass. Artifact O:/winnt/tests/S40USER/input-group.exe SHA-256:
`F20DD6AB814368E62F571E895A2F7AAA13FE85FAD96B420C462A992469201DAB`.
Output is O:/winnt/logs/s40-input-group.stdout.log; stderr is empty.
Only fixture-owned threads and their artificial keyboard-state tables are
used; no visible window, SendInput, user-thread attachment or guest change.

Source reinspection confirms Recalc2 traverses the desktop THREADINFO list,
following explicit ATTACHINFO edges and same-process TIF_16BIT membership.
It does not traverse the scheduler TDB list. Thus native duplicate-edge
semantics are reusable, but cannot replace recovery of original implicit WOW
membership. The pending production integration must supply that earlier USER
thread view and native lifetime binding; this host test does not prove the
20-slot callback registration, focus/capture or actual Win16 execution.

### Production USER lifetime: late registration and native helper threads

Current source and the formal wow32.dll.map disprove treating registration of
one session thread hook as complete USER lifetime integration. Worker begin
in mvdm_standalone_worker.c binds the main thread before WOW provider startup.
session_register_thread_hook only appends callbacks: it neither initializes
already-bound threads nor records which threads ran a particular hook.
session_thread_unbind invokes every currently registered hook. Registering a
WOW hook late without an explicit current-thread initialization would therefore
miss main-thread setup while still invoking its teardown. This is a pending
integration hazard, not a reproduced failure of an already registered WOW hook
(no production registration caller exists yet).

The formal provider map independently contains both ntvdm host_CreateThread /
host_ExitThread imports and direct KERNEL32 CreateThread / ExitThread imports.
Its compile command force-includes nt.h, not thread_start_compat.h. In wkman.c,
WowMsgBoxThread is created by native CreateThread; W32HungAppNotifyThread has
direct ExitThread paths and is also supplied as a USER callback. These ordinary
helper threads must not automatically acquire TIF_16BIT or a scheduler TDB.
Conversely, any required USER boundary state cannot rely solely on the existing
host_CreateThread wrapper to initialize every caller.

Original queue.c unlinks process membership, cleans sent/received messages
after window cleanup, and only then removes a 16-bit task from the scheduler.
DestroyTask alone is not a complete USER exit hook. Production binding must
explicitly initialize the already-running main caller, lazily or explicitly
bind other reached USER callers, mark 16-bit membership only through original
InitTask, and retain the original resource/message/scheduler teardown order.
Do not register a hook with an unconditional DestroyTask, or classify every
session thread as a WOW task. No mirror, session policy or production binary
was changed by this audit; it fixes the integration requirements before wiring
the recovered callback table.

### Original input grouping recovery (OPENNT-HOST-057)

Imported original input.c AddAttachment and Recalc2 at its original path.
The former is unchanged; the latter has one explicit current-thread parameter.
A direct source comparison after removing that signature addition passes for
both complete bodies. Original source SHA-256 remains
`B7656BCA55E892A42BA825BE0324211450AD298C40717E1AE5A889D084467529`.
Original CRLF is retained. ADAPTER-WOW-043 exposes original desktop membership,
edges and queue references through the existing USER task views. Grouping
policy is no longer a proposed autonomous traversal: the original loop is now
compiled in the formal provider. Required queue destruction is an explicit
operation; no default success, new task IDs or guest structures are added.

Formal x86 provider builds successfully; wow32.dll SHA-256:
`75DF52ED45030029880482C1778B6CCEF1492DC5BB5672467E9220CD3BDD24E8`.
The /MT /Gz /W4 /WX input-membership fixture links the formal input.obj and
passes original fixed-point traversal, before-TDB membership, explicit-edge
transitivity, other-process isolation, idempotence, old-queue release and
recalculation after a member leaves and a new member joins. Child exit 0,
errors=0; fixture SHA-256
`65F0D10906177269755B507380DA9C792C16D6E49A3BBEE427C1442D5C65FD90`.
It is a source-algorithm test: queue release is observed, not native USER
queue destruction; provisional queue reset is fixture-owned, not a claimed
implementation of RecalcThreadAttachment/AttachToQueue.

Because existing thread/queue view layouts changed, the original task-order
fixture was rebuilt with the new formal objects and rerun: child exit 0,
errors=0, sequences=5, send_reply=4, locks=6, destruction=7, waits=4,
directed=6, registration=6, init=8. SHA-256:
`FF13E0B7ABC4BA3F5394552A4FB56D4A5F1581D5D5A8FC849EC16DF5ED3AF6EF`.
Artifacts are in O:/winnt/tests/S40USER; corresponding input-membership and
task-order stdout/stderr logs are under O:/winnt/logs, stderr empty.
No production deployment, real Win16 pass, full DOS regression, guest change,
commit or push occurred. Original queue allocation, native publication,
focus/capture and teardown integration remain required before registration.

### Original provisional queue selection (OPENNT-HOST-058)

Recovered RecalcThreadAttachment from the same pinned original input.c,
following AddAttachment/Recalc2 in original order and CRLF. Only its explicit
current-thread parameter and forwarding that argument to Recalc2 differ.
The original algorithm reuses singly owned queues, allocates when shared,
increments new references and breaks on allocation failure. Its void return
must not be translated into an invented successful native publication.
ADAPTER-WOW-043 adds required allocation and the original thread pq field;
there is no new grouping or queue-selection policy in the adapter.

The final x86 formal provider SHA-256 is
`481A589D48193831F0AE7AF87377F362C67724A318FDE5EA6A7C032ECD6DA7BE`.
The original-object input-membership fixture passes queue reuse, shared-queue
allocation, same-process grouping, repeat-call idempotence and injected
allocation failure that stops before later members are processed. It checks
unchanged original queue reference counts because this phase only prepares
pqAttach; it does not yet publish native input state. Child exit 0, errors=0,
queue_selection=1, allocation_failure=1 and all preceding membership checks.
Fixture SHA-256:
`66E64D158177912FF5F09A5133704C754C72C547FD7BE47EF2A15EEBBDD1597E`.

Shared layout regression was rebuilt and passes all existing task-order
checks including init=8, registration=6 and directed=6, child exit 0/errors=0.
Fixture SHA-256:
`92139AC31BF915298CE4578B84AA67F61748D79DD291A6CC2D92B414CEA611E3`.
Both tests run under O:/winnt/tests/S40USER, output under corresponding
O:/winnt/logs/s40-input-membership and s40-task-order stdout/stderr (empty).
Production USER queue allocation/publication and teardown are still unbound.
These are algorithm and layout regressions, not real Win16 acceptance or the
17-route DOS regression. No guest mutation, production deployment, commit or
push occurred; S40 remains open.

### Full attachment caller contract and non-transactional failure

Complete source review of input.c ReattachThreads (5701 onward) and
AttachToQueue (5493 onward) refines the earlier allocation warning. The
original outer function calls void RecalcThreadAttachment, publishes every
available provisional assignment, and returns TRUE; it does not translate a
partial allocation failure into FALSE or roll back all earlier work. A new
failure return or transactional rollback would be an invented policy, not a
source restoration. Original xxxInitTask also ignores ReattachThreads' return.

The formal-object fixture now puts a singly owned queue before a shared queue,
then fails the shared queue's allocation. The first thread retains its
provisional assignment and later threads remain unassigned. It passes with
errors=0, partial_failure=1 and all previous checks, child exit 0. Fixture
O:/winnt/tests/S40USER/input-membership.exe SHA-256:
`B045E1A95D1B436138AA4729E7820A2736EA3D1B00B26115973F997B2A33D767`.
Logs remain O:/winnt/logs/s40-input-membership stdout/stderr, stderr empty.
This is original host behavior, not a guest defect and not a new fix.

Publication is substantially more than assigning pqAttach to pq. Original
AttachToQueue resolves active/focus/capture/tracking and previous-active state,
transfers caret/queue flags/cursor counts, redistributes pending input and
releases the old queue. ReattachThreads clears pqAttach before publication,
restarts desktop-list traversal, manages the journal queue lock and updates
foreground state only on the active input desktop. It explicitly prohibits
application callbacks or sent messages during this operation. A native
AttachThreadInput binding must be assessed against these reached obligations;
keyboard-state sharing alone is insufficient acceptance. Do not model native
foreground/focus as an unrelated second USER state machine or substitute
unconditional success callbacks for these owners. This audit adds no new
runtime strategy and does not claim the recovered grouping is yet connected.

### Native input focus/caret/capture: private desktop evidence

The new wow_native_input_focus_fixture creates its own Windows desktop and
never displays or switches to it. Failure to bind that desktop aborts before
window creation. Two private threads create hidden windows there; no SendInput,
user-thread attachment or guest mutation occurs. It creates focus, caret and
capture state on the owner, proves the peer initially lacks all three, calls
native AttachThreadInput, then proves the peer sees the same native windows.
After detach the peer sees none of the three and owner focus remains intact.
Custom window procedures count callbacks during the attach/detach calls; zero
are observed. Threads/windows/class/events/caret/capture and desktop are
released, and the main thread returns to its original desktop.

Compiled x86 /MT /Gz /W4 /WX, child exit 0, errors=0, private_desktop=1,
capture=1, callbacks=0; attached_focus=1, attached_caret=1. Artifact
O:/winnt/tests/S40USER/input-focus.exe SHA-256:
`1A80F8AFCAB4227358C0206EA3B71CFBA8C153166A3AFDED87E971438DC26D7F`.
Logs O:/winnt/logs/s40-input-focus.stdout.log and stderr (empty).

This is evidence for native USER owning reached focus/caret/capture sharing,
not a reason to duplicate those states in the worker. It is not a full proof
of original AttachToQueue: conflicting focus owners, active foreground
selection, journal hooks, pending input redistribution, mouse tracking and
callback reentrancy under other window states remain untested. Native links
must still reflect original implicit WOW grouping over join/exit, as the
middle-exit probe already disproved a naive chain. No product runtime binding,
formal deployment, real Win16 acceptance, commit or push is claimed here.

### Conflicting native input state and GUI-thread lifetime

The private desktop fixture now gives both threads their own focus, caret and
capture before attachment. It tests both AttachThreadInput argument directions,
and queries both threads after each join. All three shared states consistently
refer to the peer window in this setup; reversing API arguments does not reverse
the winner. No synchronous window-procedure callback occurs during attachment
or detachment. Do not encode an unproven rule that the first or second API
argument alone determines state precedence.

Original CheckTransferState (input.c:5443) first checks actual ownership of old
state, then existing destination state, and finally foreground-state ownership.
Thus original precedence depends on queue preparation/traversal and foreground,
not simply the public API argument order. This source comparison does not prove
modern conflict precedence equivalent for every foreground/window scenario.

An intermediate fixture failed cleanup: after repeated state conflicts,
SetThreadDesktop(original) and CloseDesktop returned ERROR_BUSY (170). Child
exit 1; original log retained at
build/M0-T420/S40/class-registration-r1/input-focus-desktop-busy.stdout.log,
artifact SHA-256
`E78F443D65C60DCBA52EDF8062FFCC3693741346835E5D7BE210DA030F232CD7`.
The precise remaining native GUI resource was not identified; this is not
evidence of a product or original OpenNT defect and is not counted as a pass.

The corrected fixture owns the private desktop on a dedicated GUI test thread.
It destroys its windows/class and normally returns; the non-GUI parent waits
for that thread's actual exit, then closes the desktop successfully. It neither
switches the visible desktop nor forces thread termination. Final x86 /W4 /WX
child exit 0, errors=0, private_desktop=1, capture=1, callbacks=0, both conflict
directions and previous sharing/isolation checks pass. Artifact SHA-256:
`7DDC60FBC30701B0A1D635B00794E26D3A87B72BF596428693F81A1274D4C1F7`.
Path O:/winnt/tests/S40USER/input-focus.exe; corresponding stdout/stderr under
O:/winnt/logs (stderr empty). This supersedes the earlier fixture's manual
switch-back cleanup. Production binding and real Win16 acceptance remain open.

### WOWCleanup original module-unload bound defect

Full cleanup.c review found that its 13-entry PROC spfnwp table is searched
using iSel < sizeof(spfnwp), with the same byte-count comparison for the
not-found fallback. In the selected x86 ABI this is 52 iterations, not 13.
Index 13 is already outside spfnwp if a later system-class atom matches;
continued unsuccessful search also runs outside the system atom table.
This is original host source, not guest media or an adapter-introduced change.

The pinned-source, symbolic-only verifier
tests/observation/verify-wow-cleanup-bounds.ps1 reports entries=13,
OriginalIterationBound=52, FirstUnsafeProcedureIndex=13 and corrected bound=13.
Source hash is DFB0FB8D81AFEB555B3631ADC5456F4CEB13142F2E954A6F2B22631C49142E5C.
Log: build/M0-T420/S40/class-registration-r1/cleanup-bounds.log.
It does not execute invalid accesses or count symbolic checks as runtime pass.

The selected WOW32 has a real source caller: wuser.c ModuleUnload passes
hTaskWow=0 and the NE segment table/count to pfnWOWCleanup when ne_usage <= 1.
That is the module-cleanup branch containing the faulty loop. Actual runtime
arrival at that branch is not proved: the local cleanup receiver is not yet
registered. Therefore this defect is not claimed as the current WRITE failure.
Restoring cleanup must correct both loop and not-found tests to element count,
retain original class/window lifetime policy, and cover matching and unknown
class atoms without out-of-bounds reads. It must not be copied unchecked or
classified as an immutable guest limitation. No production implementation or
guest has been changed by this audit; source recovery and validation continue.

### Cleanup must reuse registration without conflating reference domains

Current class bindings already own the native atom, original module identity,
private/public list membership, two WOW words, client menu pointers and logical
procedure/callback target. Window bindings retain that same class record;
there is no need for a second cleanup registration table. However, the binding
references field counts registration and all borrowers, including callback
aliases. It is not original CLS.cWndReferenceCount and supplies no hTaskWow.
The source/header now explicitly state this contract to prevent using storage
references as a live-window or task-ownership test.

Original class.c ReferenceClass/DereferenceClass own the separate window/class
association count (including base/clone counts). createw.c releases a class
association during window destruction; handtabl.c can release and replace it
when changing owning process. Neither relationship is a temporary alias count.
WOWCleanup module/task/thread branches require the actual association and
original task identity, not a native thread ID substituted for hTaskWow.
Those projections must be bound to existing registration and task owners;
the present adapter does not yet provide them.

The existing window-class-lifetime fixture was rebuilt with latest formal
objects and passes actual hidden-window creation/destruction, class unregister,
and retained words after native windows and registration are gone, inside an
original dispatch callback. Child exit 0, errors=0, callbacks=1, stderr empty.
This proves the borrower/association distinction in the current binding; it
does not claim WOWCleanup has executed. Fixture SHA-256:
`C7E0ABABE6BF8952D6EDEFAF8470D5332AEECD30F7A6DA0559ED4EBEAAA5D6F5`.
Path O:/winnt/tests/S40USER/window-class-lifetime.exe, corresponding stdout and
stderr under O:/winnt/logs. Rebuilt provider SHA-256:
`3816608666945A6A537E50FB9279602117448DFA055A1E359B993E6F4F3BF474`.
Only adapter contract comments changed, not runtime policy. No deployment,
guest mutation, full-product acceptance, commit or push occurred.

### Enrolled class/window association counter binding

The existing class lookup record now carries cWndReferenceCount separately
from storage references. ADAPTER-WOW-030/027 increment it only after successful
window-property publication and decrement only when that association is removed,
under the same publication lock. No extra class/object registry is introduced.
Failed duplicate publication and stale detach tokens do not alter the count;
retained callback aliases do not keep a destroyed window counted as associated.
This projects enrolled associations, not every native window in the process;
native USER still owns class clone policy and its own count. Production cleanup
must not assume complete enumeration until all relevant creation paths enroll.

The rebuilt window-class-lifetime test observes two live enrolled associations,
failed duplicate attach retaining two, detach reducing to one, replacement
restoring two, stale rollback retaining two, and both destroyed windows reducing
to zero while callback aliases remain usable. Child exit 0, errors=0,
callbacks=1. SHA-256:
`C220B3563D6FBA6B0A97BA4F316EACA34DDA28F6DBC30A2D7E0F31C323A0DA57`.
The original class-client regression was rebuilt for the changed finite view:
exit 0/errors=0, version_calls=14, version_domains=5. SHA-256:
`8623770FA6460EBD9B8244A0B340B6146E0A6DEFFD19447DF456254F2011E6AA`.
Both fixtures and corresponding stdout/stderr are under S40USER and O:/winnt/logs
(stderr empty). Formal x86 provider builds; SHA-256:
`BCCBCFB65877A5F6F740F2ABF582FC5BF281B2C47019C4F0821400303989AA82`.

Original class.c _RegisterClassEx sets hTaskWow only from a TIF_16BIT caller
with a TDB, otherwise zero. No task ID field has been fabricated in this change;
recovering that original registration wrapper and binding its current-thread
context remains necessary. WOWCleanup is still unregistered. No guest change,
production deployment, 17-route regression, real Win16 acceptance or commit/push.

### Original class server registration wrapper (OPENNT-HOST-059)

Restored complete _RegisterClassEx in existing class.c under a separate compile
profile. Its original CPD resolution, InternalRegisterClassEx failure handling,
worker/menu/WOW word stores and TIF_16BIT plus TDB task-owner assignment remain
unchanged. A direct source comparison passes after removing the sole added
explicit context parameter. Original class.c SHA-256:
`82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A`.
ADAPTER-WOW-044 supplies current thread and required class-registration/CPD
resolution operations; neither has a success default. Shared class entries now
hold hTaskWow, worker, client menu pointers and two WOW words. Former duplicate
words/menu storage was removed from the private binding rather than mirrored.

The new class-server fixture links the formal original wrapper and performs
native class registration/unregister through existing owned storage. It passes
non-WOW/no-TDB, WOW/TDB, WOW/no-TDB, non-WOW/TDB, valid CPD conversion and failed
registration with unresolved CPD. Only WOW/TDB gets task 0x4321; null WOW words
stay zero, native class atoms return correctly and menu pointers retain their
original identities. CPD resolution is fixture-owned, not a production modern
CPD provider. Child exit 0/errors=0, registrations=6, cpd_resolutions=2.
Fixture SHA-256:
`C5C641372DFB5CC37D0DD2A7293676EC8FA601D05C56876885DC87582D38F72A`.

Rebuilt shared-layout regressions also pass: window-class-lifetime errors=0,
callbacks=1, SHA-256
`888AE78EF5B1DD073287EE991663634E629F31B69ACA17EB37DA4E90604699C1`;
class-client errors=0, version_calls=14, version_domains=5, SHA-256
`22B0DD4EFB5E77F142FC642361E2AFD470B4DC6AAC4847821937A333CA7E4063`.
All fixtures are under O:/winnt/tests/S40USER with corresponding stdout/stderr
under O:/winnt/logs, stderr empty. Formal x86 provider SHA-256:
`F5C58C4D78ED4B207B91B0E2C9EDC1055D98FCD11076901538059686C46F0355`.

The current client publish function still goes directly to native class storage;
the recovered wrapper is not yet substituted into production registration.
This step establishes the original owner and shared metadata, not complete
WOWCleanup or 20-slot registration. No guest change, deployment, full DOS/Win16
acceptance, commit or push occurred. S40 remains open.

### Class client now reaches original server registration

The existing wow_class_client_publish path now enters original _RegisterClassEx,
whose required native-registration operation reuses the existing owned class
storage and native gateway. The original wrapper, not a new adapter condition,
publishes hTaskWow from the explicit current thread/TDB and writes WOW metadata.
No second task/class registry is added. The client context now requires a real
thread view; missing context fails ERROR_INVALID_STATE before version queries
or class creation. Existing profile restrictions on worker/fnid/flags remain.

CPD ownership is explicit: without a resolver, the previously unsupported CPD
registration remains rejected. With a supplied owner, original server code
resolves before native publication. A null resolution leaves the tagged value
and existing target validation rejects it; no success shim is introduced.
The test supplies a controlled resolver, not a complete modern CPD provider.

The class-client fixture links final formal objects and passes the connected
original-client -> original-server -> native registration/unregister path.
It observes task 0x4321 in the real owned class record, preserved WOW words,
menus/targets and previous version/error/exception restoration tests. New
cases cover missing thread, absent CPD resolver, failed resolution and valid
resolution. Child exit 0/errors=0, version_calls=20, version_domains=5,
server_owner=1, cpd=2. SHA-256:
`2320CAEDFB1399FEEEF062AF0AFE91895026DD692A1E25A4768C5A3DF5C47850`.
Window-class-lifetime rebuilt/relinked and passes errors=0/callbacks=1,
SHA-256 `C93325F9FB4054B1A92C5AA74760925E58C6396DD5E0022A99131928442A0E0B`.
Artifacts/logs use O:/winnt/tests/S40USER and corresponding O:/winnt/logs
stdout/stderr; stderr empty. Formal x86 provider SHA-256:
`03B8B3BB1FECEC7E671B9CD3D5C2A532A9C635A5C646DDA0555E26A161F09158`.

This supersedes the previous note that class-client still bypasses the original
wrapper. It does not supersede the unresolved top-level USER registration:
worker current-thread initialization and the 20-slot table are still incomplete,
and no actual Win16 class creation or WOWCleanup pass is claimed. No guest
mutation, production deployment, full DOS regression, commit or push occurred.

### Repeatable original-owner body gate

tests/observation/verify-wow-recovered-owner-bodies.ps1 now verifies pinned
OpenNT source hashes, original CRLF and complete AddAttachment, Recalc2,
RecalcThreadAttachment and _RegisterClassEx bodies. It permits only the exact
registered explicit-context signature/call substitutions. All four pass.
It does not normalize arbitrary whitespace or ignore added statements.

A negative test copies the two mirrors only under
build/M0-T420/S40/owner-body-negative and changes hTaskWow's default from 0 to 1.
After restoring CRLF, the verifier rejects the semantic mutation specifically
as an unregistered _RegisterClassEx body change (not merely a line-ending
failure). The expected rejection is preserved in that directory's result.log.
No active mirror, runtime binary, original source or guest media is mutated by
this negative test. The copied negative fixture is not a build input.

This is a repeatable source-fidelity gate, not proof of finite view/binding
equivalence, 20-slot registration or actual Win16 execution. No product behavior,
deployment, commit or push changes in this step; S40 remains open.

### Original class unregistration now owns removal policy

OPENNT-HOST-060 restores _UnregisterClass in the existing class.c and routes
wow_class_words_unregister through it. Private/public list lookup, original
window-association rejection and menu pointer selection have been removed from
the autonomous unregister implementation. ADAPTER-WOW-045 supplies only finite
context, native class destruction and output-transfer gating. Apart from the
explicit context, the mirror propagates failure from native destruction rather
than unconditionally returning TRUE: modern USER can still find live windows
outside our enrolled association count. That is a registered native boundary,
not a replacement window-count algorithm. Server outputs remain local until
success, preventing accidental menu ownership transfer on native failure.

The window-class-lifetime fixture verifies both rejection paths: native windows
exist before enrollment (count zero, native ERROR_CLASS_HAS_WINDOWS), then after
enrollment (count two, original rejection). Failure leaves menu output sentinels
and class membership intact. Destroying both windows permits successful removal
while retained callback aliases remain usable. Child exit 0/errors=0/callbacks=1;
SHA-256 `720E6C56A798F769D1E894A75AB09D4B469CB3E56B1E3A51DA58843F51847B04`.
Connected class-client regression passes errors=0/version_calls=20/cpd=2,
SHA-256 `CFFA33600C9E15D914CB4461613E60ABF3A524E2A8E7AA8419B5025A7B36E6BA`.
Class-server regression passes errors=0/registrations=6/cpd_resolutions=2,
SHA-256 `4F0926E240C8A07B7CBD9E1850F8CFA4B05F3D6C23BF221574C9E8F9648FD6C5`.
Artifacts are in S40USER with corresponding O:/winnt/logs stdout/stderr (empty).

The source-body gate now covers five original functions and passes, allowing
only registered context substitutions and the explicit destruction-failure
branch. Formal provider built for these tests has SHA-256
`C30E052EF309DC28545472A7368BD6CDC9A504DBD3CD5B1153C2632142030355`.
After the source registration comment/CRLF update only, the final rebuild is
`2D9D8BFE707E1BD75F8814119004AA4BDF02FA9A2C4A92003C8C352A01C20DFC`;
the body gate still passes. This restores class unregister, not full task/module
WOWCleanup or the worker's top-level USER callback table. No production deploy,
guest mutation, full DOS/Win16 acceptance, commit or push occurred.

### Native class module matching needs no extra adaptation

The connected original class-server/native-unregister fixture additionally
registers a private class under packed module 0x23451234 and unregisters using
0x23455678: original high-word matching and final native destruction both
succeed. A public class also unregisters using different module 0x45671234.
No module-replacement workaround or new matching rule is required. An unrelated
high-word request for the private class is rejected with
ERROR_CLASS_DOES_NOT_EXIST, preserving its record and all menu output sentinels.
These are owned classes in one fixture process, not other-process operations.

The existing class-words-lifetime regression was rebuilt with original removal
and also passes public/private same-name selection, native live-window failure,
menu ownership and retained data lifetime: child exit 0/errors=0, SHA-256
`26EEC75F93A96768A3FD3A57C3A107B702CD46CB03731F71DEDAA63D23606128`.
No product code changes were needed for these module cases. They validate
existing boundary behavior, not WOWCleanup or real Win16 execution.

Final expanded class-server test exits 0/errors=0, registrations=8,
cpd_resolutions=2. SHA-256:
`0CDCD9F5E0901D621C6EFEC15D760B23FA019FF9F0EC4DE2E44C2D408D36BC97`.
Artifact/logs use O:/winnt/tests/S40USER/class-server.exe and corresponding
O:/winnt/logs/s40-class-server stdout/stderr (empty). Successful unregister
may leave the earlier failure's LastError untouched; the fixture therefore
interprets LastError only on failure, without changing product error state.
No formal product deployment, guest change, commit or push occurred.

### Single original class-procedure field

ADAPTER-WOW-030 now stores the enrolled logical class procedure in existing
class entry lpfnWndProc, removing the private target.procedure field. Only the
immutable callback binding remains private. Getter/exchange/publication use
that same field; native USER still holds the gateway and WND instances retain
their independent procedure snapshots. This makes the original class field
available to cleanup without a second procedure value or a synchronization
copy. Total target-storage bytes are unchanged; this is ownership convergence,
not a claimed net byte reduction or new dispatch policy.

Latest formal objects pass the class-procedure fixture including direct field
assertions, replacing a class target without changing existing instances,
new-window inheritance, nested creation, instance override and invalid CPD/
gateway rejection. Child exit 0/errors=0, callbacks=40. SHA-256:
`78FA15BE8DAFC58C5DD5FD77B4A5D44C91747D93B3D630F31DD461664F2D0E91`.
Connected class-client passes errors=0/version_calls=20/server_owner=1/cpd=2,
SHA-256 `4088AFD35C14831344BD651C94D2C3096D7910D241823166975DF3B7077A6F27`.
Window-class-lifetime passes errors=0/callbacks=1 including both unregister
failure paths; SHA-256
`F1D6DC226F9DE15484F322C4E949EF19D2796034011F4CF121A89B25D799CD21`.
Fixture paths/logs remain S40USER and corresponding O:/winnt/logs stdout/stderr
(empty). Formal x86 provider SHA-256:
`ECA8BF1380C82D596358E2BFF74B33EA9E77E58AB46282FD83C67D4F8D56BAEE`.
No original mirror body changes were needed. Full WOWCleanup and worker USER
registration are still incomplete; no real Win16 pass, full DOS regression,
production deployment, guest change, commit or push occurred.

### Original read-probe recovery

OPENNT-HOST-061 imports the original `base/ntos/ex/probe.c` header and unchanged
ProbeForRead body at its original mirror path. Original SHA-256:
`0D1CAE414B609C624E6BDF04FA28EBD84FF38DBB9687A63D8BC9E840C2F2372B`.
ADAPTER-WOW-046 supplies the native process address ceiling and RtlRaiseStatus;
it does not invent page scanning or change the original range/alignment policy.
The source gate now checks six recovered owner bodies, including this full tail.

The x86 formal provider built successfully, SHA-256:
`3B9551C5CFEA62C9760C4446DD3C4C08225A6F1A4D24F36A9E3CCB265E82E28D`.
`O:/winnt/tests/S40USER/user-probe.exe` SHA-256:
`50BD53D32D287843CAE19049FB9EF1D2D8E4F6E566E238706DEBA729C6EE0F13`.
Its child exit is zero, with `errors=0 alignment=5 range=3 no_page_touch=1
access_fault_caught=1`; stdout/stderr are the corresponding
`O:/winnt/logs/s40-user-probe` logs, stderr empty. Checks include zero length,
alignment-before-range, wrapping, the exclusive ceiling, and a PAGE_NOACCESS
allocation: the original probe does not touch pages, while the actual later
read raises an access violation caught by SEH. A successful range probe is not
proof of accessible memory and cannot replace the guest-memory lease.

This is a cleanup dependency, not complete WOWCleanup integration or real
Win16 acceptance. No production deployment, full DOS regression, guest change,
commit or push is claimed by this fixture result.

### Cleanup destruction progress contract

The pinned original cleanup.c has two zero-reference class destruction sites
whose loop progress depends on DestroyClass unlinking the current node. The
pinned original class.c implements void DestroyClass and unconditionally
assigns `*ppcls = pcls->pclsNext`; it is not a fallible native unregister API.
The extended `verify-wow-cleanup-bounds.ps1` verifies both sites and that
original unlink operation, separately from its existing 13-versus-52 table
bound finding. Its source analysis does not execute an unbounded retry loop.

The current formal class-removal objects were linked into the extended x86
window-class-lifetime fixture. Two actual native windows exist before private
enrollment: the private class count is zero, but native UnregisterClass rejects
release with ERROR_CLASS_HAS_WINDOWS. Thirty-two bounded retries all retain
the exact list node, its WOW words and caller menu-output sentinels. Existing
enrollment/borrow/dispatch checks then run; actual window destruction permits
successful unregister and outstanding borrows remain valid until release.

Child exit 0, `errors=0 callbacks=1 native_failure_retains_node=32`; fixture
SHA-256 `624A0E9A13A58D3BC9D968509481B1D8DE4C1C7562F3520EAFE6E492ADAFA09B`.
Artifact: O:/winnt/tests/S40USER/window-class-lifetime.exe; stdout/stderr:
O:/winnt/logs/s40-window-class-lifetime.stdout.log and .stderr.log (empty).

Integration requirement: do not substitute the existing fallible native
destructor into the original cleanup loop without explicit failure handling.
Neither freeing a still-live class record nor treating the enrolled count as
the complete native window count is valid. The original caller distinguishes
module, task and thread cleanup; DestroyTask is not a replacement for these
resource operations. Original DestroyClass frees its server menu-name storage,
not CLS.lpszClientAnsiMenuName/lpszClientUnicodeMenuName; do not infer client
menu ownership transfer from that body. Full window/object enrollment and the
cleanup entry remain incomplete. This result demonstrates a binding contract,
not a current production hang or a real Win16 cleanup pass. No production
deployment, full DOS regression, guest modification, commit or push occurred.

### Whole original WOWCleanup owner recovered

OPENNT-HOST-062 now imports cleanup.c's complete module/task/thread owner and
selector/window helpers, excluding unrelated ClientDied. The original hash is
the pinned cleanup.c identity above. ADAPTER-WOW-047 supplies explicit required
operations and borrowed owner views, sharing existing task process/class roots
and input desktop state. Production live handle/window membership and target
mutation are not supplied by the fixture and remain mandatory before use.

Registered changes: explicit current context, element-count bounds for the
13-entry procedure table, and BOOL results (matching the WOW cleanup callback
result shape) instead of void. Allocation/probe/native class deletion failure
returns FALSE; successful branches return TRUE. Earlier effects are retained,
not rolled back, and failure is not converted into successful resource release.
The extended source gate reverses only these changes and matches the entire
remaining original owner, including all three branches, against pinned OpenNT.

First actual integration found a second boundary: ordinary GetClassPtr skips
CSF_WOWDEFERDESTROY entries. Calling ordinary _UnregisterClass from cleanup
therefore failed with ERROR_CLASS_DOES_NOT_EXIST, even though cleanup held the
correct node. Both callers now reuse one bottom native deletion primitive;
ordinary unregister keeps original lookup, while cleanup uses the already held
link, as original DestroyClass did. Native USER continues to own native class
resources. No second class registry or copied class-selection policy was added.

Formal x86 provider SHA-256:
`4DDA78089BCD3584AB0D386855FAC91B55A392C98FC83496476C2ADDDAE835FE`.
Cleanup fixture SHA-256:
`574541024831A6206C09A5DB2D54F66DCBFC93C04202A71DDC8BEC5B15F77C7D`;
child exit 0, errors=0, module=1/task=1/thread=1/native_failure=2/bound=13/
probe_failure=1. Both deletion sites use real native window-caused rejection;
after window destruction retry succeeds. Three real native menus prove that
only the selected process-owned, unlocked task object is destroyed; foreign
owner and already-destroying entries remain untouched. Controlled window views
check logical target neutralization, server-proc exclusion and foreign-process
isolation; this is not an actual Win16 callback or complete native object graph.
PAGE_NOACCESS selector input proves original SEH frees temporary storage on
failure. The fixture does not execute original out-of-bounds reads.

Recompiled affected fixtures all return child exit 0/errors=0:

- class-client: `221F6E20E74E7DF3F9D83F4D22EC3757A17ACA7B8DF34C73AAAAF73267C86BF9`.
- window-class-lifetime: `D24C6234F5B251A81ED671DD83D5AD6047A20C3C16662CEFF9D2B72E4DCAF5A6`.
- task-order: `ED6AC3010F5A8D335045FB0EA9332FA7A29E7D6B79F667461942F852BE8B1FE4`.
- input-membership: `F0DD1E0EE7432FCD706C44BE731D5294B0EACCE0DE0A6D6DB2BEC001D4CA4F90`.

Artifacts are O:/winnt/tests/S40USER/<name>.exe; corresponding logs are
O:/winnt/logs/s40-<name>.stdout.log/.stderr.log, with empty stderr. Six existing
owner-body gates also pass. No production deployment, real Win16/full DOS
acceptance, guest mutation, commit or push occurred. S40 remains incomplete.

### Cleanup now changes the actual window dispatch field

ADAPTER-WOW-027 moves the window procedure from private target.procedure into
the window-owned original-shaped lpfnWndProc cleanup field. Dispatch and public
private-field read/exchange now use that exact field; the registered callback
remains separate and immutable. The same borrowed storage exposes class/tree/
thread fields to the original cleanup owner. Native attach initializes its
actual retained class entry, leaving thread/tree membership to the USER owner.
No second persistent procedure, cleanup snapshot or target write-back is used.
These required lifecycle fields add storage; this is not a net byte-reduction
claim or proof that all production windows have already been enrolled.

The extended cleanup fixture creates real native windows through the existing
CBT/class binding and sends WM_USER+42 through the original DispatchClientMessage
owner to a controlled WOW callback. Task cleanup changes the actual field to
DefWindowProcA: a subsequent real SendMessage returns zero without another WOW
callback; GWL_WNDPROC still names the native gateway. Module selector cleanup
also restores the original class procedure, and a subsequently created window
inherits DefWindowProcW and does not dispatch to the old WOW target. Destruction
and class removal complete while a retained window borrow remains valid.
The controlled callback is not guest CallBack16; no real Win16 pass is inferred.

Cleanup child exit 0/errors=0/live_dispatch=2; SHA-256
`62A308830A2D73B4DE8783F8CD5DE3A2DD2EB40E02C5DF02E7E1C19517AA9DD8`.
Rebuilt class-procedure (callbacks=40), window-class-lifetime (callbacks=1,
native_failure_retains_node=32) and class-client (version_calls=20, cpd=2) each
pass child exit 0/errors=0. Their respective SHA-256 values are:
`D323D53521B3A68BB749FA9992A3DEB49908AA84F21E3D3480BD3BD96E30C1B2`,
`E6E7E418D1BE82BB3CBDC8F8F36E61FC63F72A0AF8575A167F7CFDA59C184C46`,
`1C692AD0EA8D2B49E6950376E4C32F475D9ABD643D29269F621800DB2F7B3605`.
Artifacts/logs retain the S40USER and s40-<name> paths above, stderr empty.
Formal x86 provider SHA-256:
`68065E0590211322A68E849EE36A17C5313B2F73C1E5EC433A78EFC31911C8F8`.
No additional mirror-body changes were needed; complete cleanup source gate
passes. Full worker membership, 20-slot publication and real Win16 acceptance
remain outstanding. No production deployment, full DOS regression, guest
change, commit or push occurred.

### Native creation publishes the supplied USER thread

The A/W creation boundary now requires a USER thread with process/class roots,
rather than only a class context. It publishes the same thread in the actual
window fields inside the CBT scope before subsequent hooks and WNDPROC calls.
Cleanup fixtures no longer patch the created window's thread by hand. Detach
clears the thread pointer in retired borrowed storage; the USER owner still
must retain thread lifetime while windows are published. This does not invent
thread identity, implement a scheduler or establish full handle membership.

All affected call sites were updated. Native-only fixtures use a shared test
helper for one controlled process/thread, not a production current-thread
provider. Callback checks prove earliest publication; retained borrows from
WM_NCCREATE rejection and CBT veto prove that rollback clears the thread.
Normal cleanup/destruction also clears it while the retained WW remains valid.
Missing creation thread is rejected before native creation. Nested A/W and
MDI creation, class-procedure inheritance and the original creation/callback
owners were rebuilt and rerun, not assumed compatible with the changed ABI.

Five fixtures pass child exit 0/errors=0, at the usual S40USER executable and
s40-<name> stdout/stderr paths (stderr empty):

- native-creation-binding: callbacks=43, nested=1, rejected=1, vetoed=1,
  destroyed=7; `EB9917CE81082EACEDB8F5E0F97C6FBE6D76942EBCC1088E9D3FD2C318384223`.
- original-wndproc-chain: callbacks=3, idle_pairs=3;
  `727CCEF29A3AEBCCAF95298744ADE75ADB7C8EA025236AFB9989FF4F82802CA5`.
- original-create-entry-native: cases=10, native created/destroyed=4/4,
  messages=2, captures/frees=2/2;
  `31027AF3A1D6B5C8411A50D3B773D44F24D5C33B26B5F4BA73158524287EC4F7`.
- class-procedure: callbacks=40;
  `5B366A0D5C89BEC80F8DDD3F4A7FA7B62FF531CA890E90D8A5F342D84C2C6561`.
- cleanup: module/task/thread plus native_failure=2 and live_dispatch=2;
  `E4634D1FAA690BDF290A2C8C4294D4EB881832AA5DB58DC522D7353E2214E299`.

The original callback fixture supplies a controlled final CallBack16 and TEB;
its link excludes the duplicate production TEB implementation. Original
Unicode creation is compiled with UNICODE/_UNICODE defined before forced
headers; neither adjustment changes production source. Formal provider hash:
`A9E0DE8D3BD67E067EF38671DBE1E1167620324F4AA961D0EC0898F679798647`.
No production deployment, real Win16/full DOS acceptance, guest changes,
commit or push occurred; worker registration remains outstanding.

### Original object classification replaces caller-defined flags

OPENNT-HOST-063 imports the unchanged gabObjectCreateFlags table and original
TYPE/OCF definitions at their actual source paths. The cleanup context's
object_create_flags field and three-entry fixture classification are removed.
Source verification pins handtabl.c, user.h and userk.h and compares the exact
selected declarations/table; current non-FE_IME compilation has 16 types.
Original HMAllocObject generates a USER handle through HMHandleFromIndex and
owns object allocation. It is not interchangeable with registering an already
created native HWND. A separate original-generated namespace would require
full handle conversion across USER calls/callbacks, not a local cleanup fix.
No substitute allocator or handle namespace was added here.

The cleanup fixture now creates and destroys actual native menu, cursor and
accelerator objects through the original process/task-owned selection. The
foreign-process and already-destroying menu entries remain alive. Final cleanup
child exit 0/errors=0, resource_kinds=3/live_dispatch=2; SHA-256:
`5E414A11EFCABBFFF9DC4A12B59D60ADD4CDA1C098DA1E80CDAC8859E8DE0C27`.
Recompiled original task-order regression also passes child exit 0/errors=0,
including registration=6/init=8; SHA-256:
`909FAC5C3B50426E345000A07E5FAC17BE19A3961366908953697E03D549F64F`.
Paths/logs retain O:/winnt/tests/S40USER and O:/winnt/logs/s40-<name>, stderr
empty. Formal x86 provider SHA-256:
`AA3688225CE9B37B05F910784FF90C39D0DF6735A01C7FB8FAE759185EC58B1E`.
Three exact repeated README paragraphs were removed (two WOW binding records
and the repeated RTL error mapping description); distinct records were kept.

This establishes original classification and three actual native release
kinds, not complete handle enrollment or all sixteen resource lifecycles.
Window registration/retirement in a complete worker object view remains open.
No production deployment, real Win16/full DOS acceptance, guest change,
commit or push occurred.

### Bound windows automatically publish and retire cleanup membership

ADAPTER-WOW-048 supplies a native identity companion table, not a handle
allocator. Indexing follows the selected WOW 16-bit alias (`walias.h` USER16 /
GETHWND16) and original user.h HMINDEXBITS. The native high word is retained
in original-shaped wUniq, never generated or incremented here. A live alias
collision fails without overwriting the old entry; retirement requires both
the native identity and a still-borrowed matching object pointer. Slots zero
and FFFF are rejected for window publication. Property publication/retirement
holds the existing property lock; the complete USER owner remains responsible
for serializing cleanup and retaining table/thread lifetime.

Bound A/W creation publishes the actual WW cleanup fields and USER owner before
hooks/WNDPROC. Creation rollback and terminal detach automatically remove the
entry before releasing storage. The real-window cleanup tests now borrow this
table directly instead of manually filling handle slots. Existing original
cleanup and dispatch bodies are unchanged. The first implementation retained
full native HWND in each entry; using original wUniq instead brings x86 entries
from 16 to 12 bytes and saves 262144 bytes over 65536 slots. The fixture asserts
the final 786432-byte entry array. This is companion metadata, not proof of
guest-published NT4 binary pointers or complete resource enrollment.

Final x86 provider SHA-256:
`54CAF42746EC8BBD22E66C991AF419ADA77A557E4D47A37E71568D8954C88A8D`.
Recompiled fixtures pass child exit 0/errors=0, at the existing S40USER and
O:/winnt/logs/s40-<name> paths with empty stderr:

- cleanup: native_membership=1, stale_retire=1, live_dispatch=2,
  resource_kinds=3; `24A0C2F43EBE73A1959DE01A91EB2EA0E284527D7A046C956FE81E563145A9FF`.
- native-creation-binding: callbacks=43, nested=1, rejected=1, vetoed=1,
  destroyed=7, table_empty=1;
  `92D418D48D29ACB75B5DFD76D580235179822C83ACAE82B2DA5BF938F1DFFA23`.
- class-procedure: callbacks=40;
  `48CCE57899AD7C5D064CEC0D9A0F2C3763AE770DF6905AC431769F3222722F23`.
- original-wndproc-chain: callbacks=3, idle_pairs=3;
  `BD4CF8922E7E95B63DE4176D7B9B16A45F36E6187F182A015A42126F571DF38A`.
- original-create-entry-native: cases=10, created/destroyed=4/4,
  captures/frees=2/2;
  `31ECC00E83F2168DA6EA6896BDF08628D74A4E0B5876F8677156FF6F70A3E5D3`.

The identity-collision/old-retire negative checks use controlled identities,
not a claimed collision reproduced by native USER. Actual native-window tests
check table publication on every callback and no entries left after nested,
rejected, vetoed and MDI creation/destruction. Recompiled task-order regression
also passed before the final entry-layout tightening; its process view does
not embed that table. Full worker domain initialization/locking, native window
tree binding and other resource publication remain open. No real Win16 or
full DOS acceptance, production deployment, guest modification, commit or push.

### Native window tree composed for the original cleanup owner

The bound cleanup entry derives temporary links among live registered window
views from current native GetAncestor(GA_PARENT), contracting unbound native
intermediate nodes. Original PseudoDestroyClassWindows still owns class/thread
selection and procedure neutralization. Its non-xxx operation has no callback
or order-sensitive mutation; this view is only for cleanup, not ordinary USER
enumeration or z-order policy. The product binding does not change native
parents. The USER owner must serialize the domain and supply non-callout
release operations; worker-wide enforcement remains outstanding.

The fixture creates a STATIC parent, WOW window, STATIC intermediate and WOW
descendant. Original module cleanup neutralizes both WOW procedures and leaves
the STATIC procedure unchanged. Two checks observe the tree during original
selector-buffer allocation, before/after real SetParent changes; each call
reflects current native relationships. Only the fixture reparents windows.
Allocation failure and a controlled allocation exception both restore the
prior desktop root and clear temporary links; the exception propagates.
Cleanup composition is separate from basic window publication to avoid making
the native membership primitive depend on the complete cleanup owner.

Cleanup child exit 0/errors=0, native_tree=2/live_dispatch=4, with prior three
cleanup branches, native failure, resource and identity checks also passing.
SHA-256 `0E8A2037D129B09A4C70BC97F316F974EFA6ADAA961BB5E70561DFBB176C77F5`.
Artifact/logs: O:/winnt/tests/S40USER/cleanup.exe and
O:/winnt/logs/s40-cleanup.stdout.log/.stderr.log (empty). Formal x86 provider:
`CED404B0A71C7A58D96E2F55232CABF41F6A2CDF6DE5A0BC51466C08C5363B45`.
Original cleanup/type/classification gates pass without new mirror body
changes. This is real native tree/dispatch testing, not guest Win16 execution.
Other resource producers, worker locking/registration and full product
acceptance remain open. No deployment, guest change, full DOS regression,
commit or push occurred.

### Worker-local USER domain binding

Question: can the standalone worker supply the narrow synchronization carrier
that original USER requires without holding a lock through a client callback
or inventing an NT4 USER process/handle heap?

Inputs: pinned OpenNT `windows/core/ntuser/kernel/validate.c` SHA-256
`661748C142E662966844C3820CCEB09670F192C9B981586F80FA696106A15A62` and
`ssend.c` SHA-256
`E4B9600381683902A05FF8BA3E7E0F6876246A97960482F8B0BF6E51DA601844`.
`EnterCrit` acquires the exclusive USER resource and establishes the current
thread; `LeaveCrit` clears/relinquishes it. The exact original `MAKECALL` and
`MAKECALLCAPTURE` macros use `LeaveCrit(); KeUserModeCallback(...); EnterCrit();`.
This is source evidence that a blanket lock around a native reentrant callback
would be wrong.

Implementation: ADAPTER-WOW-049 adds `wow_user_runtime`, a caller-owned
worker-local process domain and TLS current-thread binding. It has no guest
mapping, USER object allocation, scheduler selection, native HWND namespace or
published shared USER structure. Its only synchronization operations are
exclusive enter/leave and the explicit callout leave/reenter pair. It is linked
only by the formal `ntvdm.exe` worker, not by WOW32.DLL: a DLL-local copy would
split TLS and lock state inside one worker process.

Verification: the x86 `/MT /Gz /W4 /WX` fixture compiles the production
`wow_user_runtime.c`, binds two real native threads to one domain, proves the
first thread cannot unbind or recursively enter while exclusive, releases for
a callout, lets the peer acquire/release, then reacquires in the original
thread. It exited 0 with
`WOW_USER_RUNTIME errors=0 peer=1 callout_reacquire=1`; stderr was empty.
Artifact SHA-256 is
`12D599D87917E551B9AB66C4B74522C1FE6215490C90638F1F2179B08148B557` at
`build/M0-T420/S40/user-runtime-r1/user-runtime.exe` with logs beside it.
`wow_user_session_binding` attaches the already-bound worker thread and
registers the same binding for subsequent session-bound original worker
threads; worker cleanup detaches it before session unbind. The x86 session
lifecycle fixture compiled production `session.c`, guest-memory support and
both USER binding sources, then passed with
`WOW_USER_SESSION_BINDING errors=0 child=1 lifecycle=1`; stderr was empty.
Its SHA-256 is
`B9B0B53F415A5D9F1AEF0917738591C65A709BC4029D6411D1746F201153BF5F` at
`build/M0-T420/S40/user-session-runtime-r1/user-session-runtime.exe`.
The formal x86 worker relinked successfully. Its map has 10 USER-runtime
symbols from `wow-worker-bindings`; the freshly regenerated WOW32 map has
zero, proving the intended one-instance composition. Worker/provider SHA-256
values are respectively
`9096AAF4C8B8C9FF3F928891C191B7F06B9FC5A1F3FE45ED5F6378F0B5CA6E86` and
`5CF1FE34B5BE70A494D612C22356CFAFD3218D13B84CD5776C1A83205019302B`.

Interpretation: this proves only the worker-local synchronization carrier and
the required drop/reacquire ordering. It does not prove a current USER task,
local `UserRegisterWowHandlers`, any output slot, shared PMODE32 graph or a
Win16 workload. Those remain unchecked W1 work in the production checklist.

### W1/W2 scheduler dependency audit

Question: can the five W1 task-lifecycle output slots be treated as an
independent, guest-accepted component before the W2 message/window owner and
the complete USER registration table exist?

Inputs: the selected original `src/mvdm/wow32/wkman.c`, the imported original
`opennt-host/windows/core/ntuser/kernel/{queue.c,taskman.c,exitwin.c}`, and
pinned OpenNT `windows/core/ntuser/{client/client.c,kernel/{ntstubs.c,
taskman.c,cleanup.c}}`.

Observation: original `WK32InitializeHungAppSupport` registers the WOWEXEC
event through `pfnRegisterUserHungAppHandlers`; `W32Thread` then calls
`pfnInitTask`. The recovered `xxxInitTask` constructs the original TDB and
scheduler state. `pfnYieldTask`, `pfnDirectedYield`, and
`pfnWowWaitForMsgAndEvent` all reach the same recovered `taskman.c` state.
This is not equivalent to a native `Sleep(0)` or a per-thread event shim.

The dependency is concrete rather than hypothetical. Original
`xxxUserYield` calls `xxxReceiveMessages`; original `W32DestroyTask` calls
`pfnWOWCleanup`. The former is a USER server message-delivery operation with
queue/send-message state; the latter is W2's original class/window/resource
cleanup owner. The original `CheckForClientDeath` also raises
`STATUS_PORT_DISCONNECTED` and never returns when an NT4 paired USER client
dies. It cannot be replaced by a successful no-op in a standalone worker.

Original `client.c::UserRegisterWowHandlers` writes all 20 output slots in a
single aggregate. A local table containing only the five W1 entries would
permit startup while leaving callable null entries and would violate the
selected original contract. Therefore W1/W2 implementation can and must
proceed through their complete original bodies and bounded worker bindings,
but a real immutable-Win16 witness is only meaningful after W7 publishes the
whole recovered table. The production checklist records this as an explicit
aggregate gate, not as deferred cleanup or a claim that either component has
already passed.

No code, guest media, runtime package, artifact deployment, commit or push
resulted from this audit.

### W2 message-owner boundary resolved

The direct W1 dependency was traced one level further instead of being left as
an unnamed scheduler hook.  Pinned OpenNT
`windows/core/ntuser/kernel/userk.h` defines `xxxReceiveMessages(pti)` as a
loop over `xxxReceiveMessage(pti)` while `QS_SENDMESSAGE` is set; the concrete
definition is `windows/core/ntuser/kernel/sendmsg.c::xxxReceiveMessage`.
That 100,534-byte USER-server translation unit consumes private SMS receive
lists, `PWND`/window-procedure state, client callbacks and server cleanup.  It
is therefore inside the explicit USER-server stopping boundary in the source
policy, not an independently composable OpenNT host carrier.

The required W2 implementation is consequently precise: its adapter must use
the actual modern USER32 message queue for send/receive and dispatch, and must
bind the recovered class/window procedure and cleanup owners around that
queue. Modern USER32 does not expose an independent non-consuming probe for a
pending synchronous send: a normal queue pump is what makes it observable.
The bridge must therefore own the single real queue's synchronous and posted
dispatch rather than manufacture private `fsWakeBits`/SMS state. It must test
synchronous dispatch, posted dispatch, nested dispatch,
cancel/failure and task/worker teardown.  A synthetic receive success, a
separate message-state machine, or recursively importing `sendmsg.c` would
each violate the selected boundary.  W1 remains open until that W2 bridge is
implemented and exercised through the same worker domain.  No guest media,
runtime artifact, commit or push resulted from this source-boundary audit.

### W2 native message bridge: first production boundary

`ADAPTER-WOW-050` is now production source at
`src/ntvdm-exe/wow/wow_user_message_bridge.c`, included in the selected WOW32
provider source group.  It establishes the current thread's actual USER32
queue, requires that same owner thread for every operation, and drains that
single queue with `PeekMessageW`/`TranslateMessage`/`DispatchMessageW`.
Cross-thread `SendMessageW` is therefore released only by real target-thread
dispatch. Posted messages share that same queue and are dispatched in order;
there is no separately authored SMS list, fake wake bit, or shadow window
state. A received `WM_QUIT` records its code and returns
`ERROR_CANCELLED`, rather than being discarded or converted to success.

The x86 `/MT /Gz /W4 /WX` fixture compiles this production source and creates
a real message-only native window. A second native thread blocks in
`SendMessageW`; it cannot complete before the bridge pumps the owner queue,
then returns the real window-procedure result `73`. The same run posts and
dispatches a distinct message, verifies it has left the queue, posts `WM_QUIT`
and verifies explicit cancellation. It passed with
`WOW_USER_MESSAGE_BRIDGE errors=0 sends=1 posts=1 quit=1` at
`build/M0-T420/S40/user-message-bridge-r1/wow-user-message-bridge.exe`.
The same source also compiled under the formal provider's exact x86 flags as
`provider-message-bridge-r1/obj/adapter-wow/wow_user_message_bridge.obj`.
The host's Ninja launcher again stopped at its known `.ninja_lock` condition
before it could drive that complete graph. Following the existing recorded
fallback, the generated provider link inputs from the last complete
`provider-stdcall-r1` object set plus this newly compiled object were linked
verbatim in the x86 VS environment into the isolated
`provider-message-bridge-link-r1/wow32.dll`. The link succeeded (only the
pre-existing original DEF DESCRIPTION/duplicate alias warnings), has SHA-256
`033F7A49C36A270578F33CBE6832224B03AB2ABF6704CEE0A25209F5FF846A19`, and its
map contains all four bridge functions. This is an incremental composition
check, not a claim that every current source file was freshly rebuilt.

This is not W2 or W1 closure: it has not yet been bound to an original task
record, scheduler wait, class/window lifecycle or the aggregate WOW callback
table. No guest media, runtime deployment, commit or push resulted from this
increment.

### Reproducible message-bridge boundary check

The production message bridge was rebuilt independently after the boundary
audit, using the same x86 `/MT /Gz /W4 /WX` command line and no fixture copy
of its implementation.  The resulting
`build/M0-T420/S40/user-message-bridge-r2/user-message-bridge.exe` again
reported `WOW_USER_MESSAGE_BRIDGE errors=0 sends=1 posts=1 quit=1`.

This repeat run proves the exact native queue contract required by the later
task-lifecycle binding: a sender is blocked until the target thread consumes
the real queue; a posted message is dispatched by that same consumption
operation; and `WM_QUIT` becomes an explicit cancellation rather than a
successful wake-up.  It does **not** prove that `xxxSleepTask` can substitute
this queue for its original task-event/scheduling state.  The latter must be
implemented by W1's single task-lifecycle owner before either W1 or W2 can be
checked.

### W1 implementation ledger: no scheduler shortcut

The next production composition is fixed by the reached original ownership;
it is not a choice among interchangeable host shims:

| Original output entry | Original body | Required standalone binding |
| --- | --- | --- |
| `pfnRegisterUserHungAppHandlers` | `exitwin.c::xxxRegisterUserHungAppHandlers` | duplicate and retain the supplied WOWEXEC event, publish exactly one `WOWPROCESSINFO` view, and release it at worker teardown. |
| `pfnInitTask` | `queue.c::xxxInitTask` | create one actual current-thread task view before entering the body; bind its native queue, input membership, class roots, object companion table and message bridge; roll all of them back on any status failure. |
| `pfnYieldTask` | `taskman.c::xxxUserYield` | enter the one worker USER domain, consume the real native queue through W2, then retain the original task-list/event ordering. |
| `pfnDirectedYield` | `taskman.c::xxxDirectedYield` | locate a live task by the real OS thread ID written to the original guest TDB; it may not use a separately invented task-ID table. |
| `pfnWowWaitForMsgAndEvent` | `taskman.c::xxxSleepTask` | wait for the retained task/WOWEXEC events and the one native queue, preserve alert/cancellation handling, then return the original BOOL direction. |

`W32Thread` creates the real thread through `host_CreateThread`; the existing
`thread_start_compat.c` already attaches that new thread to the worker session
and thus to `wow_user_session_binding`.  The lifecycle owner must extend that
existing current-thread binding, rather than create another thread or a second
scheduler.  `W32DestroyTask` and `wuser.c` both call `pfnWOWCleanup`; W2 must
therefore remove the same task's native class/window/object associations before
the thread binding is released.  `opennt_exit_thread` is the final session
unbind point, not a substitute for this cleanup.

The current binding header's direct `WaitForMultipleObjectsEx` macro is not a
candidate production solution: it cannot observe a native queue's synchronous
send.  It will be replaced only as part of the above lifecycle with one
explicit owner operation that waits both task events and the W2 bridge's real
queue; a broad user-mode scheduler or synthetic wake-bit list is prohibited.

### W1 production carrier: original task lifecycle and public queue wait

`ntvdm-exe/wow/wow_user_task_lifecycle.c` now supplies the single W1 carrier
specified above.  It calls the recovered original
`xxxRegisterUserHungAppHandlers`, `xxxInitTask`, `xxxUserYield`,
`xxxDirectedYield`, `xxxSleepTask` and `DestroyTask` bodies.  It uses one
worker-local runtime domain, records the actual host thread ID, retains a
WOWEXEC event, and owns one real USER32 queue/message bridge per original task
view.  The old direct wait macro is replaced by a host operation that calls
`MsgWaitForMultipleObjectsEx` over the original task/WOWEXEC event set plus
that one queue.  On a queue wake it schedules the original TDB event and lets
the reached original `xxxUserYield -> xxxReceiveMessages` order dispatch the
message; it creates neither an SMS list nor a second scheduler.

The x86 `/MT /Gz /W4 /WX` direct production-source fixture at
`build/M0-T420/S40/user-task-lifecycle-r3/user-task-lifecycle.exe` reported
`WOW_USER_TASK_LIFECYCLE errors=0`.  It establishes an active worker session
binding, then performs original registration, InitTask, UserYield and
DestroyTask, including the real current-thread death check.  An earlier
fixture attempt without that session binding correctly terminated with
`ERROR_PROCESS_ABORTED`; this was a fixture precondition failure, not a
scheduler result.  The corrected test demonstrates that the carrier calls the
same original production bodies under their required worker context.

This does not close W1 or W2.  The current provider still imports
`UserRegisterWowHandlers` from modern USER32; therefore no W1 function is
published into the 20-slot table.  In particular, W2 must still give the same
task its class/window/object cleanup and native message lifecycle before the
original all-or-nothing registrar can replace that import.  No guest medium
was modified and no artifact was deployed.

### W2 integration: task-owned class/window cleanup

The W1 carrier now owns a real enrolled-object table, original class roots and
the finite cleanup desktop context required by the recovered
`kernel/cleanup.c`.  It supplies USER32's actual `DefWindowProcA/W` only for
the original task-cleanup fallback; it does not create a replacement window
manager.  Its cleanup release callback refuses a process-owned object type
that has not installed its own producer-specific release operation.  This
prevents a silent free of an opaque native resource.

`wow_user_task_window_lifecycle_fixture.c` compiles the production lifecycle,
original taskman/queue/exitwin/cleanup/class/hand table/probe bodies and the
same class/window/object bindings.  Its x86 output at
`build/M0-T420/S40/user-task-window-r1/user-task-window.exe` reported
`WOW_USER_TASK_WINDOW_LIFECYCLE errors=0`.  The fixture establishes the
worker session and original task, registers a real native class, creates and
enrols a native window in that task's object table, invokes original task
cleanup, verifies the procedure is neutralized to native DefWindowProc,
then destroys/unregisters and performs original DestroyTask.

This is a production-source integration witness for W1's direct W2 cleanup
dependency.  It is not a complete W2 pass: it has not yet entered through the
original WOW output table, has no guest WND/WW consumer, and does not cover
cross-task send/reply, dialogs, module-selector cleanup or every object
producer.  Those remain explicit W2/W3 closure work.

### W1/W2 integration: original Yield dispatches a real posted callback

The same task/window lifecycle fixture now registers an encoded WOW procedure
(`0x92350020`) with its real callback, posts `WM_APP + 120` to the enrolled
native window, first invokes `wow_user_task_lifecycle_wait` with the actual
WOWEXEC event, and then invokes `wow_user_task_lifecycle_yield`.  It does not
call the message bridge directly after posting.  The callback count is one,
which witnesses the selected route:

```
PostMessageA -> taskman.c::xxxUserYield -> xxxReceiveMessages
             -> wow_user_message_bridge_receive -> USER32 DispatchMessage
             -> original DispatchClientMessage -> WOW callback
```

The x86 `/MT /Gz /W4 /WX` rebuild and execution of
`build/M0-T420/S40/user-task-window-r1/user-task-window.exe` reported
`WOW_USER_TASK_WINDOW_LIFECYCLE errors=0`.  This is deliberately narrower
than a W2 completion: it proves the production carrier does not bypass the
original yield/receive ordering for a posted window message, but it does not
cover cross-task synchronous send, nested reply, dialogs, guest WND data or
the all-or-nothing 20-slot registration.

The test also records an important original return convention:
`taskman.c::xxxSleepTask` returns `FALSE` after the ordinary path that has
rescheduled the current task.  The worker lifecycle wrapper preserves that
value rather than converting it into a fabricated success result; the test
checks that it returns without an error and that the subsequent original yield
dispatches the posted callback.

### W1/W2 integration: two original tasks, synchronous send and teardown

The production-source task/window fixture also now creates a second thread,
binds it to the same active worker session and the same worker-local USER
domain, and initializes a second original `xxxInitTask` task (`0x4321`).  The
second task owns a separately enrolled native window with encoded procedure
`0x92350021`.  The primary thread sends `WM_APP + 121` by
`SendMessageTimeoutA`; the second task, and only that task, calls the recovered
`xxxUserYield`.  Its W2 native queue receiver performs the synchronous
dispatch and returns `0x4a21` to the sender.  The fixture then performs the
second task's original window/class cleanup and `DestroyTask` before the first
task's own lifecycle cleanup.

The same x86 executable reported `WOW_USER_TASK_WINDOW_LIFECYCLE errors=0`.
This is evidence for a real two-task cross-thread send/reply boundary using
the production lifecycle carrier; it is not a claim that the NT4 private SMS
representation was recreated.  USER32 remains the concrete public queue and
send owner.  Nested client/guest callback return, dialogs, module-selector
cleanup and the complete guest WND/WW contract remain W2 work.

### W2 dialog boundary revalidation

The current x86 `/Gz /W4 /WX` build of
`wow_dialog_lifecycle_fixture.c` required its CRT entry to be stated
explicitly as `int __cdecl main(void)`; this is an ABI declaration correction
for the fixture, not a WOW behavior change.  The rebuilt PE at
`build/M0-T420/S40/dialog-lifecycle-r1/wow-dialog-lifecycle.exe` reports the
existing `WOW_NATIVE_DIALOG_CONTROL_POST_DESTROY_OK` marker, including native
`#32770`, `Edit`, the observed auxiliary IME window, callback ordering and
post-`WM_NCDESTROY` release.

The two previously built original-body executables in
`build/M0-T420/S40/original-dialog-init-r1` were also rerun successfully:
`WOW_ORIGINAL_W32INITDLG_TWO_FIELD_CONTRACT_OK` and
`WOW_ORIGINAL_DIALOG_INIT_AND_DISPATCH_SAME_STORAGE_OK`.  Together with
OpenNT `client/dlgbegin.c`, this establishes the non-negotiable W2 ordering:
the native dialog must have its WW published before `W32InitDlg`, and
`W32InitDlg` must set `vpfnDlgProc`/dialog class state before the first
dialog callback and before child-control creation.  A plain public
`CreateDialogIndirectParam` call cannot satisfy that contract by itself.

The next production binding therefore has a finite scope: thread-local native
dialog creation hooks must publish/retire the existing WW/object ownership and
call the unchanged `W32InitDlg` at the original creation point, while a
wrapper preserves the returned user init parameter for `WM_INITDIALOG`.
It must not import `dlgbegin.c` recursively or synthesize a successful private
USER server call.  No partial handler is published from this evidence.

### W2 dialog creation binding: original initialization with scoped WW access

`ntvdm-exe/wow/wow_dialog_creation_binding.c` now supplies the bounded public
USER32 creation boundary for the original `pfnServerCreateDialog` call shape.
It does not parse a dialog template or reproduce the private USER server's
`InternalCreateDialog`.  A thread-local CBT hook instead recognizes the native
`#32770` creation, publishes an existing WW projection and task ownership, and
the `WM_CREATE` return hook calls the unchanged `wow32/wcall32.c::W32InitDlg`
at the original ordering point.  The dialog procedure wrapper provides the
returned original `DLGDATA` user parameter only for `WM_INITDIALOG`; it retires
the property, projection and task enrollment at `WM_NCDESTROY`.

The initially reached failure was precise rather than a provider ambiguity:
`W32InitDlg` accesses `GWL_WOW*` through the bounded WW accessor, which rejects
unscoped access with `ERROR_INVALID_STATE` (5023).  The binding now enters the
existing worker-local `wow_user_borrow_scope` at both original-code callout
boundaries: before `W32InitDlg`, and around the retained dialog procedure.
This preserves the access rule and its lifetime reference; it does not weaken
the accessor or expose a host USER pointer to the guest.

The production-source x86 fixture
`build/M0-T420/S40/task-dialog-r1/task-dialog.exe` now reports:

```
WOW_USER_TASK_DIALOG_LIFECYCLE errors=0 init=1 terminal=1
```

It establishes the actual worker session/runtime/task lifecycle, invokes the
production `wow_user_server_create_dialog` with an original-shaped ANSI
template and `SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE`, and checks all
of the following on the one object instance: original `W32InitDlg` publishes
the dialog class/state value `0x0001000A`, stores its original dialog-procedure
word, passes `DLGDATA[1]` to `WM_INITDIALOG`, retains WW through
`WM_NCDESTROY`, and removes it after teardown.  The same fixture rejects an
invalid null/zero template.  The provider graph independently compiled the
same binding and original `wcall32.c` under x86 `/MT /Gz /W4 /WX`.

This is a W2 production-boundary witness only.  It does not check W2 complete:
the original all-or-nothing 20-slot registrar remains unpublished, and a real
immutable Win16 dialog must still reach this call path during W7 acceptance.

### W2 window creation entry: selected WOW32 call shape

The selected provider has one `pfnCsCreateWindowEx` call site:
`wow32/wuwind.c::WU32CreateWindowEx`, and it passes `CW_FLAGS_ANSI` with the
three-DWORD `WW.adwWOW` input prefix.  The original USER client body is
`client/ntstubs.c::_CreateWindowEx`; its final `NtUserCreateWindowEx` call is
inside the excluded private USER server boundary.  The production replacement
therefore retains the caller ABI and the observable prefix/order without
copying a server implementation.

`wow_window_creation_binding.c::wow_user_create_window_ex` is now the exact
`PFNCSCREATEWINDOWEX` carrier.  It accepts the selected ANSI form, resolves
the already registered original class in the worker-local class owner, copies
only `sizeof(WW.adwWOW)` from the input, and delegates creation to the
existing bounded native gateway.  The gateway publishes the complete WW,
native style/instance and task ownership before the first native callback.
Unsupported historical create flags are rejected rather than silently mapped
to a different modern USER32 policy.

The x86 `user-task-window.exe` regression was rebuilt using this entry (rather
than its earlier direct `wow_window_create_boundA` helper) and reported:

```
WOW_USER_TASK_WINDOW_LIFECYCLE errors=0
```

It covers original task initialization, class registration with the original
encoded procedure/callback target, PFN-shaped window creation, cross-task
synchronous send/reply, original posted-message yield/receive, original
cleanup and terminal destroy.  This proves the selected current WOW32 caller
can enter the finite public boundary without changing the established W1/W2
lifecycle.  It does not establish a real guest `WU32CreateWindowEx` call or
authorize publication of a partial USER output table; both remain W7 work.

### W1.1 thread ABI correction: original host thread entry remains WINAPI

The selected original `softpc.new/host/src/nt_thred.c::host_CreateThread`
accepts `LPTHREAD_START_ROUTINE`; on x86 this is a `WINAPI` (`__stdcall`)
entry.  The standalone compatibility header had applied the cdecl-thread
adapter indiscriminately to every `CreateThread` caller.  That was not an
OpenNT semantic replacement, but an ABI mismatch: a real `W32Thread` entered
through `host_CreateThread` could have both the callee and the cdecl thunk
remove its parameter.

`thread_start_compat.h` now selects an explicit WINAPI thunk only for the
original `nt_thred.c` compilation unit (`OPENNT_HOST_CREATE_THREAD_ABI`).  The
existing cdecl adapter remains selected for the original timer/redirector
callers whose historical entry declarations actually are cdecl.  Both thunks
perform the same worker-local session bind/unbind; no scheduler, task record
or USER policy was introduced.

The focused x86 production-source witness was compiled from the generated
`thread-abi-r1` graph's freshly compiled `obj/host/nt_thred.obj`, the new
production WINAPI thunk, and the worker session/WOW runtime bindings.  It
calls the original `host_CreateThread` with a real stdcall entry, verifies the
sentinel exit value, one CCPU new-thread hook and the expected session/runtime
binding, then reports:

```
WOW_HOST_THREAD_WINAPI_ABI errors=0 ccpu_new=1
```

This closes neither W1.1 nor W1.5: it proves the precise `host_CreateThread`
ABI and thread-local binding edge, while actual `W32Thread` creation and the
real WOW16 BOP callback/return are still required by those checklist gates.

### Formal provider source and ABI closure

The selected original WOW32 package is now a linkable x86 DLL, rather than a
collection of independently compiling source fragments.  The formal provider
graph selects 77 original WOW32 C bodies and 105 original declaration/carrier
files.  Its parent is the same formal x86 `ntvdm.exe` composition, not a test
replacement.  The completed artifacts are:

```
build/M0-T420/S40/formal-parent-r2/ntvdm.exe  3216384 bytes
build/M0-T420/S40/provider-closure-r2/wow32.dll  555520 bytes
```

The last unresolved import was
`_OpenNtRtlNtStatusToDosError@4`, called by the selected original
`wow_user_task_lifecycle` flow.  Its body was already selected in the parent
from the original OpenNT RTL object; the parent simply did not expose it to a
late-loaded WOW32 DLL.  The product now exports that existing body under the
private name `OpenNtRtlNtStatusToDosError`, and the provider's generated
decorated-name import alias binds to it.  No replacement error mapping, USER
policy, scheduler or guest change was added.

This is only the code/ABI closure prerequisite for W1--W6.  It deliberately
does not tick a checklist row: the current provider still imports the modern
`UserRegisterWowHandlers` surface and therefore has not yet published the
original complete 20-slot output contract, nor has immutable Win16 reached
the linked DLL on this artifact.  The pinned read-only profile check continues
to pass as `WOW_USER_PROFILE_SOURCE_AND_BINARY_OK`; that validates the PMODE32
consumer requirement and source mapping, not runtime acceptance.

### Modern USER32 registration observation

The isolated x86 test
`tests/adapter-mvdm-host-out/wow/wow_user_registration_surface_fixture.c`
loads the actual current `USER32.DLL`, resolves the historical export and
passes zeroed original-shaped input/output records in its own process.  It
reports:

```
WOW_USER_REGISTRATION_SURFACE shared=C0000022 build=00000000 slots=0
```

`0xC0000022` is `STATUS_ACCESS_DENIED`.  Thus the present system still exposes
the historical spelling, but does not grant a standalone process the NT4
USER/CSRSS registration service.  The result is intentionally not interpreted
as a pointer, and all 20 `PFNWOWHANDLERSOUT` slots remain zero.  This is a
direct, reproducible explanation for why provider linkage cannot be promoted
to W1 acceptance.  The fixture is isolated from the product, does not launch
an NTVDM worker, and changes neither guest media nor USER32 state outside its
own terminating process.

### W1.0.a production paging carrier

The original `WU32NotifyWow(FUN_FINALUSERINIT)` writes `gpsi` into immutable
USER16 memory as a DWORD. In NT4 that value was useful because the USER shared
section was mapped at corresponding linear addresses in both the ordinary user
process and the VDM client. A CCPU worker has a separate guest translation
domain, so passing an unmodified modern USER32 pointer would not recreate that
condition.

The first production W1.0 gate therefore adds a bounded worker-local paging
carrier, not a substitute USER implementation. At the existing `BOP 51`
pre-`W32Init` boundary it uses the selected original `VdmAddVirtualMemory`
body to register one native backing page, creates an ordinary CCPU page
directory/table allocation through the selected original
`VdmAllocateVirtualMemory`, identity maps the existing SAS physical range,
and maps that backing at the original user-read-only `0x7FFE0000` clock
coordinate. The existing DPMI real/protected transition functions withdraw
and restore the same CCPU paging state; the existing SAS shutdown path removes
the mapping before original memory teardown. This neither changes a CCPU
instruction/memory accessor nor publishes a callback, host pointer, USER
handle, TEB or zero-filled shared structure.

Focused x86 compilation from `build/M0-T420/S40/w10a-page-map-r1` succeeds
for the new carrier and each of its three production integration translation
units:

```
obj/adapter-wow-worker/wow_shared_page_mapping.obj
obj/host/nt_bop.obj
obj/host/sim32.obj
obj/dpmi/modesw.obj
```

This is not W1.0.a closure yet. The remaining requirements before its checkbox
can be ticked are a full formal `ntvdm.exe` link, a real BOP-51/WOW run proving
the actual mode state and non-regression, explicit map/failure/teardown
witnesses, and the established text-gated DOS matrix. The W1.0.b ABI plane
remains intentionally unstarted: there is still no guest-visible
`SHAREDINFO`, `SERVERINFO`, handle table or TEB publication.

The production carrier was subsequently linked into the complete x86 CCPU
fixture graph at `build/M0-T420/S40/w10a-page-map-r2`. The generated command
graph compiled all 399 translation units and linked
`ccpu-wow-shared-page-mapping-test.exe`. Its runtime completed with exit code
zero and emitted:

```
CCPU_WOW_SHARED_PAGE_PRODUCTION_LIFECYCLE_OK
```

That witness uses the production `wow_shared_page_mapping_begin`,
`wow_shared_page_mapping_leave_protected_mode`,
`wow_shared_page_mapping_enter_protected_mode`, and
`wow_shared_page_mapping_end` functions, rather than duplicating page-table
logic in the fixture. It proves the fixed clock coordinate resolves for a
user read, rejects a user write, survives the exact real-to-protected
transition sequence, and releases the mapping. It does not prove a live BOP
51 invocation or a bounded USER shared range, so W1.0.a remains open.
### W1.0.a production image link

The same `w10a-page-map-r2` formal x86 graph linked the production
`ntvdm.exe`; its post-link `Verify-VdmTibStorage.mjs` audit passed with
`VdmTib owner=mvdm_vdm_tib.obj bytes=4208 address=0x7d7490; no overlapping
symbol`. The graph's generated combined link-and-audit command has a Windows
`cmd` quoting defect after the successful link, so the unmodified audit was
executed separately with the configured Node 22 runtime. This records a real
product link and its required ownership audit, but is still not a live BOP 51
or DOS regression witness.

### W1.0.a carrier withdrawal and allocator finding

The preceding carrier text is retained as attempt chronology, not as an
accepted production design. A real immutable `WRITE.EXE` run reached `BOP 51`
and first proved that the original `VdmAddVirtualMemory` failed with
`STATUS_NO_MEMORY`. Source review then found a mismatched platform condition:
the selected CPU40 build follows the original non-`i386` XMS allocation path,
but the selected MSVC `_M_IX86` configuration had entered the original kernel
VDM `i386` sizing arm. That arm creates only XMS plus conventional memory even
though the same source's WOW profile requests 16 MiB of DPMI headroom. The
CPU40 configuration now selects the original headroom calculation; direct
`COMMAND.COM /c ver` regression passed on the rebuilt isolated worker.

With capacity restored, the experimental native backing registration returned
successfully (`prepare-ok`) but the live worker stopped before its following
page-directory activation call. This proves that the proposed BOP-time
`VdmAddVirtualMemory` rebind is not a valid live carrier, even though the
bounded CCPU fixture proves the individual walker and page-protection
operations. The experimental carrier, its production call and its
carrier-specific fixture target were removed from the product graph. W1.0.a
remains open and must derive the real DOSX/PMODE32 mapping lifecycle before a
new production implementation is admitted.

### W1.0.a — recovered DOSX page-table route (not yet implemented)

The follow-up source audit establishes the route a replacement must use.  It
is materially narrower than the withdrawn private-CR3 carrier:

* `dpmi/dxvcpibt.asm::SetupPageTables` creates DOSX's `DXPD` directory,
  records its physical base in `V86ToPm.zaCr3VTP`, and later
  `VCPIBootStrap` replaces that recorded base after copying the DOSX system
  area to extended memory.  DOSX is therefore the owner of CR3 across its
  real/protected transitions.
* `dxvcpibt.asm::GrowPageTables` shows the original way DOSX enlarges its
  user-table capacity: it allocates backing, updates the active `DXPD`, and
  copies entries into the selected `SEL_DXPT` view.  A standalone binding may
  add a single fixed mapping only through that live DOSX directory; it must
  not retain a parallel page directory or reload CR3.
* The unmodified CCPU walker in `ccpu386/c_tlb.c::xtrn2phy` reads the active
  CR3 physical page, then its PDE and PTE through `phy_read_dword`; its
  existing `flush_tlb()` is the required cache invalidation primitive.  No
  CCPU instruction, access routine or host-pointer shortcut is required.
* PMODE32's immutable `usercli.asm` reads the clock from linear
  `0x7FFE0000`.  For that address the original walker uses PDE index 511 and
  PTE index 992.  A user-readable, read-only clock PTE consequently needs
  `P|U` flags, while the containing PDE must also be user-accessible and
  writable only to the degree required by the bounded USER range.  The CCPU
  combines PDE/PTE permissions exactly as x86 does.

The existing `VdmAddVirtualMemory`/`VdmSetPhysRecStructs` seam can still own
the native backing: it returns a bounded **guest physical** page within the
SAS mapping.  It cannot directly represent `0x7FFE0000`, because that is a
guest *linear* address and the current physical record table is deliberately
bounded to SAS pages.  The missing W1.0.a binding is therefore exactly:

```
native backing
  -> original VdmAddVirtualMemory
  -> bounded SAS physical page
  -> PTE in the live DOSX CR3 directory for 0x7FFE0000
  -> unchanged CCPU walker -> immutable USER16 read
```

Before implementation, the carrier must additionally locate the original
DOSX transition notifications that make a freshly bootstrapped CR3 observable
to the worker.  A BOP-only installation is insufficient: DOSX may replace the
directory during bootstrap.  The implementation must bind/rebind after that
real transition, withdraw the PTE before the backing is released, and prove
the same mapping from the actual `MS_bop_1 -> W32Init` path.  This evidence is
not a W1.0.a pass and does not authorize a partial `UserRegisterWowHandlers`
table.

The notification owner is now identified: `dpmi32/dpmi32.c` dispatches the
unmodified DOSX `DPMI` BOP subfunction `53:01` to
`DpmiCpu40SwitchToProtectedMode`.  That selected CPU40 function already
performs the source-defined transition at the only suitable point: it has the
DOSX frame, has installed the published GDT/IDT images, and calls
`setMSW(...|MSW_PE)` before restoring the protected selectors.  The W1.0.a
binding must be called only after that restoration succeeds, with the active
CR3 read at that point.  The complementary `switch_to_real_mode` and
`DpmiSwitchToRealMode` paths are the withdrawal/dirty-state observers.  This
uses an existing host DPMI boundary rather than modifying immutable DOSX
media, intercepting a CCPU instruction, or inventing a BOP.

### W1 correction — original shared-section handoff, not a host-pointer ban

The earlier wording that a host pointer could not be handed to USER16 was too
coarse.  The original architecture does hand a 32-bit value directly through
WOW32, but the value is valid because it names a USER shared section that NT4
maps into both the USER32 client process and the VDM's protected linear view.
The standalone requirement is therefore a same-address guest view of the
original shared ABI, not an arbitrary prohibition on pointers.

Read-only source comparison against pinned OpenNT records the complete handoff:

1. `ntuser/client/clinit.c::UserClientDllInitialize` calls
   `CsrClientConnectToServer`, then copies `USERCONNECT.siClient` to
   `gSharedInfo` and assigns `gpsi = gSharedInfo.psi` (lines 92--119).
   `USERCONNECT` is explicitly an output record containing `SHAREDINFO` in
   `ntuser/inc/user.h` (lines 1074--1078).
2. `ntuser/client/client.c::UserRegisterWowHandlers` only stores the 21 WOW
   input callbacks, fills all 20 output slots with USER-owned functions, and
   returns `(DWORD)&gSharedInfo` (lines 2256--2315).  It neither creates the
   section nor initializes its producer data.
3. `mvdm/wow32/wuman.c::FUN_FINALUSERINIT` writes the returned `gpsi` directly
   to the guest's `UserInit16.lpgpsi`; immutable `wow16/user/usercli.asm`
   declares `_wow16gpsi` as a DWORD.  PMODE32 then dereferences it directly.

This also identifies a larger semantic boundary than the former one-page
experiment: `SHAREDINFO` points to `SERVERINFO` and a handle table, while
`SERVERINFO` contains the metric, color, cursor, function-vector and resource
state used by USER16.  The original `CallCsrFlag` is likewise written as a
pointer during `FUN_FINALUSERINIT`.  A lone clock mapping, a zero structure,
or a BOP-time `VdmAddVirtualMemory` call cannot be a valid W1 implementation.

The locally recovered `src/opennt-host/.../client/client.c` is currently a
registered selected-function subset, not the complete original client
translation unit.  It therefore cannot be relabelled as the original USER
shared-section producer.  The actual producer path crosses the explicitly
excluded CSRSS/USER-server boundary.  Any standalone replacement must retain
the source-defined layouts, output ordering, invalid/stale-handle behavior and
teardown, while using a finite public modern boundary; it cannot be presented
as a direct import of `UserRegisterWowHandlers` alone.

The previously attempted BOP-time page-map carrier is withdrawn and remains
attempt chronology only.  Its `VdmAddVirtualMemory` registration succeeded
after the CPU40 allocation correction, but the live worker stopped before
page-directory activation.  No portion of that carrier is selected in the
current production graph.  W1 stays open until its complete producer, guest
mapping, thread state and all-or-nothing callback contract can be reached in
one production path.

The original server-side mapping owner is now also pinned.  In
`ntuser/kernel/server.c::InitMapSharedSection` (lines 1488--1548), USER maps
its read-only section into the client process, then calculates every returned
client address as `pClientBase + (server-address - section-base)`.  It writes
the relocated `psi`, `aheList`, optional DLL list, default-message tables and
every control-message table into `USERCONNECT.siClient`.  Later
`NtUserProcessConnect` validates the `USERCONNECT` request (lines 2142 onward).
This is the exact source reason that original `UserRegisterWowHandlers` can
return `&gSharedInfo` and that WOW16 can dereference its descendants without
per-pointer conversion.

The finite standalone seam must therefore be specified as a **relocated,
read-only USER client view**, with separately owned mutable per-task client
state; it is not a generic host-memory alias and cannot use a made-up fixed
base.  The exact client-view address must be the value returned by the
recovered registrar and simultaneously valid to PMODE32.  This eliminates the
earlier unproven `0x7ffd0000` range assumption from future implementation.
