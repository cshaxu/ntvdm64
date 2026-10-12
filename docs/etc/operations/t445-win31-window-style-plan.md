# T445 Windows 3.1 window-style working plan

## Authority and stage boundary

S1 closed through its [installation design](../evidence/m0-t445-s1-installation-design.md).
The owner inserts accepted uncommitted palette repair delivery as S2.
The previously admitted standalone experiment moves to S3; integration moves
to S4--S7. On 2026-10-12 the owner closes S3 and resequences S4 as standalone
menus/MDI, S5 as propagation and S6 as integration. Historical evidence and
build directories retain their original numbers. S7 retains the unresolved
native-menu animation fresh-session control; it is not a passing capability.

| Stage | Bounded result |
| --- | --- |
| S1 | HWND ownership, original Win3.1 references, task/launch policy owners and installation design; preserve explicit remaining implementation proofs. |
| S2 | Audit, commit/push and close the owner-accepted NTVWM palette geometry repair, tests and published assets. |
| S3 | Implement experimental src/win16-style and independently launch an authored Win32 GUI test program linked directly to it. Prove basic frame visuals/interaction without any other product component; obtain owner stability acceptance. |
| S4 | Standalone retro system/ordinary menus, MDI and remaining supported interaction/state coverage in the shared component and demo; no product integration. |
| S5 | Both CLI options, explicit/inherited policy resolution, end-to-end propagation, per-Win16-task isolation and real-window title marker; build, verify, publish, commit/push for owner acceptance. No automatic activation of experimental retro painting in production. |
| S6 | Reuse, integrate and harden the tested common frame/menu/MDI, font/palette/DPI and geometry through WOW/Hook paths; do not build a second renderer. |
| S7  | Integrated visual/interaction and serial product regression, coherent release/publication and final owner acceptance. |

## S3 standalone experiment contract

Owner follow-up admits a selectable demo gallery for all discussed forms.
Extend the local renderer to captionless/borderless top-level windows and
all caption-button combinations. Keep native-reference menu/scrollbar/child/
MDI examples explicitly labelled, not disguised as retro support; their
renderer is now S4. Repair system-menu double-click close through native
non-client menu handling, including disabled-close and cancellation tests.

- Library source remains src/win16-style. The authored Win32 API demo and tests
  live under tests/app; build/test orchestration under tests/observation. Produce
  executable, objects, logs and captured images only under build/M0-T445/S3.
- Launch the demo directly, not with run16 and not by injecting into an unrelated
  application. Direct linking is deliberate: it isolates the UI from transport,
  guest behavior and Hook installation. No other product library/runtime is a
  dependency. Build x86/x64 variants; this is one fixture source, not new workers.
- Show ordinary min/max, minimize-only, and dialog-style windows, plus a native
  comparison. Exercise activation, title changes, palette changes, pressed
  buttons, move/size, restore, close and repeated create/destroy. Preserve the
  actual client area. Explicitly label any incomplete menu/MDI behavior.
- Initially use owned-window probes. Owner subsequently authorizes computer use
  for this demo after native caption-button leakage escapes those captures.
  Compare actual desktop output as well as message/layout tests; a private-desktop
  PrintWindow capture does not establish physical pointer or RDP acceptance.
  Do not operate unrelated applications or change host settings.
- Keep library API small and explicit. Reuse the experiment's actual layout,
  painter and window state later; do not make a disposable mock UI or a general
  skin framework. Select/verify original-looking metrics and fonts here, with
  later integrated refinement. Do not redistribute unreviewed historical assets.
- Deliver the experiment as a build-owned test executable for visual review,
  not a replacement in assets/release or O:/winnt/system32. No deployed ten-image
  behavior changes, no RPC/Hook/WOW integration, no global host theme changes.
- Stability means focused repeatable tests plus owner acceptance of the visual
  trial, not merely a successful compile. Owner closed this gate with the
  native-menu fresh-session retest explicitly deferred to S7.

## S4 development checkpoint

r052 review repair separates STYLE_NCUAHDRAWCAPTION/FRAME from
WM_NCMOUSEMOVE: drawing notification parameters are not pointer coordinates
and must never enter menu hover selection. The focused fixture enters keyboard
menu state, sends both drawing notifications with payloads landing inside a
real direct-command menu cell, and asserts selection remains unchanged.
Both widths pass this case, all retained 27,261 checks, menu/MDI and lifetime
tests. Hidden runner PID35852 (2026-10-12 13:38:59, 180-second deadline) exits0;
glyph verification succeeds. This is a project-owned S4 dispatch correction,
not a guest/mirror change or a diagnosis of native popup animation blankness.
Latest standalone trial is build/M0-T445/S4/r052/x64/win16-style-demo.exe.

r051 aligns the public header and gallery labels with the implemented menu/MDI
capabilities. The owned hidden runner (PID33020, 2026-10-12 13:36:58,
180-second deadline) completes with exit0. Both x86/x64 rebuilds pass 27,261
retained checks each, queued menu/MDI routes and callback-lifetime negatives;
original menu glyph verification also succeeds. Reviewed x64 system-popup
and maximized-MDI captures show the two-command dialog menu and merged
system/restore cells respectively. These are rendered probe observations,
not physical mouse or final original-pixel acceptance. Runnable owner trial:
build/M0-T445/S4/r051/x64/win16-style-demo.exe (x86 sibling also built).
No production image or runtime deployment changes.

Coverage audit after r050: the public README still described menu entry 15
and maximized MDI controls as native/pending; it now distinguishes implemented
retro presentation from retained native management and explicitly retains the
physical-acceptance limit. Header/gallery label alignment was still pending
at that checkpoint and is now verified in r051 above. That correction does
not add runtime evidence or close S4. Remaining delivery gates are separate
final source review, current rendered-image review against original references,
governance/link/diff checks, commit/push and owner acceptance. Unsupported
arbitrary owner-drawn/bitmap menu bars are not certified by the string-menu
tests. Native animated-menu blankness remains the independent S7 investigation;
retro popup success is not evidence that its root cause was fixed.

r050 adds six real callback lifetime negatives per architecture. An owned
window removes its style or destroys itself from WM_INITMENU,
WM_INITMENUPOPUP, or WM_NEXTMENU; the latter is reached by a queued left arrow
inside actual popup tracking. Each asserts exactly one callback, no watchdog
expiry and no remaining attached style. Removal cases additionally verify
original item type restored and successful reinstall/remove; destruction
cases assert HWND retirement. Both widths pass these, retained 27,261 checks,
menu/MDI cases and glyph checks. Hidden runner PID 19624 starts 2026-10-12
13:32:05 local, deadline 180 seconds, exits 0; build/M0-T445/S4/r050.
These are observed callback-safety cases, not an exhaustive allocator/handle
reuse proof. Production images remain unchanged; S4 review is still open.

r048/r049 implement same-thread styled-owner navigation using the native
WM_NEXTMENU result established by r047. At a keyboard boundary the shared bar
asks the current window, validates the returned live window and attached
application/system HMENU, and changes the iterator's current owner. Current
style state is explicitly pinned across callbacks/owner changes; transitions
do not recursively call open_bar. The original focus is carried across owners.
r048 real popup loops verify frame Window -> child system -> frame Window ->
Escape. r049 also traverses child system -> frame system -> child system ->
frame Window, checking the HWND receiving each WM_ENTERIDLE and original child
focus after cancellation. Both widths pass both routes and retained 27,261
checks, menu/MDI/resource gates. Hidden r048 PID 36200 starts 2026-10-12
13:29:01 local; r049 PID 43336 starts 13:30:10; 180-second deadlines, exit 0.
Evidence build/M0-T445/S4/r048 and r049. Arbitrary custom/unattached HMENU or
unstyled/cross-thread targets are not claimed supported by these cases;
callback destruction/invalid-target negatives still need explicit review.

r047 establishes the native cross-owner MDI navigation contract before adapting
it. Exact comparison: OpenNT windows/core/ntuser/client/mdiwin.c
DefFrameProcWorker WM_NEXTMENU (2047+) and DefMDIChildProcWorker WM_NEXTMENU
(2334+), SHA-256 77471975010484FC6935B91A56CF7450225D44402F6D9E3709FBBDFF1C8A706E.
The real styled frame/MDICLIENT/child fixture queries those native handlers:
frame bar left and frame system menu right return the active nonmaximized
child/system HMENU; child system left returns frame/system HMENU, right returns
frame/application HMENU. With the child maximized, frame returns no separate
owner (controls are merged). Both-width assertions pass, plus existing 27,261
checks and menu/MDI tests. Hidden PID 41496 starts 2026-10-12 13:26:27 local,
180-second deadline, exit 0; build/M0-T445/S4/r047. This is contract evidence,
not yet implemented cross-owner navigation acceptance.

Next implementation must call native WM_NEXTMENU at navigation boundaries,
validate its live same-thread window/menu result and move the existing menu
selection context there. Do not infer MDI ownership from a class/name or
introduce an MDI registry. Use an iterative owner transition with balanced
style-state lifetime and original-focus return, avoiding recursive C calls
for each boundary crossing. Full real-loop cross-owner tests remain required.

r045/r046 correct the gallery's dialog identity rather than editing native
system-menu contents. Visual review of r044 exposed Restore/Size/Minimize/
Maximize in sample 12: it was a CreateWindow popup with dialog-frame styling,
not a dialog. It now uses CreateDialogIndirectParam/DS_MODALFRAME and a dialog
procedure that leaves unhandled behavior to DefDlgProc. r046 asserts class
#32770, Move/Close present and those four other commands absent **before**
installing the style component. Captured system popup now has Move and Close,
matching the owner's reference structure. No application-menu deletion policy
was added. Both widths pass native-dialog checks, menu/MDI cases and 27,261
retained checks; GDI attachment baseline/final is 56/56. Hidden r045 PID 28880
starts 2026-10-12 13:22:51 local; r046 PID 43196 starts 13:24:04; deadlines
180 seconds, both exit 0. Capture:
build/M0-T445/S4/r046/x64/capture.bmp-system-popup.png. Original popup-only
diagnostics remain historical evidence, not equivalent genuine-dialog tests.

r044 strengthens r043's message-sequence proof with an actual recursive native
menu loop. From the Extra popup's WM_ENTERIDLE the gallery calls a second
TrackPopupMenuEx with TPM_RECURSE/TPM_RETURNCMD, selects its Inner command,
asserts that inner item attributes are restored while the outer item remains
adapted, then selects the outer command. After outer return its original
attributes and application focus are checked too. Both x86/x64 pass, with
27,261 retained checks and the existing menu/MDI/resource gates. Hidden runner
PID 19096 starts 2026-10-12 13:20:31 local, deadline 180 seconds, exits 0;
evidence build/M0-T445/S4/r044. This uses distinct HMENUs and queued keys on a
private desktop, not same-HMENU recursion or physical input acceptance.

r043 fixes a source-confirmed nested-loop cleanup defect: WM_EXITMENULOOP
previously restored every adapted item owned by the window, including a still
active outer menu. Temporary items now carry their owning menu-loop depth;
exit restores only that depth. Window removal/destruction still restores all.
Preparation failure restores only the affected popup, not unrelated menus.
The DC fixture drives real style subclass entry/init/exit messages for two
distinct HMENUs and asserts both become owner-drawn, inner exit restores only
inner, outer remains adapted, and outer exit restores it with no item nodes
left. Both widths pass this plus 27,261 retained checks, menu navigation/MDI
and GDI 55/55. Hidden PID 25972 starts 2026-10-12 13:18:51 local, 180-second
deadline, exits 0; build/M0-T445/S4/r043. This is component message-sequence
proof; same-HMENU recursive tracking and arbitrary live app mutations are not
certified by this case. No production release or S4 closure is claimed.

r039--r042 add queued mouse-motion switching using live shared bar geometry.
During popup tracking, a move to a different bar/system cell ends the old loop
and transfers selection; a popup opens, but disabled/direct-command items do
not execute from hover. Outside the popup loop, selected-bar nonclient motion
can reopen a popup. No desktop/global hook or input injection into user apps.
r039/r040 fail the new fixture: its stock SYSTEM_FONT metrics put Extra at
(403,207), outside the component's actual entry. r041 uses the component's
specified System font; Extra at (372,205) switches and executes exactly once,
but the fixture then sends an extra Escape after command completion, closing
the demo through its documented application shortcut. r042 cancels only the
still-active disabled/direct-selection cases, retaining the command-count and
focus assertions. Both x86/x64 pass hover negatives/selection, 27,261 retained
checks, MDI/menu-loop regressions, GDI 55/55 and all 273 glyph pixels.
Hidden runner r042 PID 47844 starts 2026-10-12 13:16:46 local, deadline 180
seconds, exits 0. Prior owned failures r039 PID 33920, r040 PID 30940 and r041
PID 39832 all exit 1 and remain as evidence. This is queued-message acceptance,
not physical pointer timing or full application-defined menu navigation.

r037/r038 replace the cyclic boundary tested by r036: a WS_SYSMENU frame now
selects its system cell between the first and last bar entries. Opening that
selection uses native SC_KEYMENU(space), not a copied/rebuilt system HMENU;
the bounded observer reports boundary arrows back to the iterative bar loop.
The selected system button is drawn pressed. r038 explicitly verifies File
left -> actual system-menu initialization -> Extra, Extra right -> actual
system menu -> File, and Escape at the system menu followed by leaving bar
mode restores the original focus without dispatching a command. Both widths
pass retained 27,261 checks plus those cases, MDI popup double-click and scope
lifetime tests; GDI remains 55/55 and all 273 glyph pixels match. Hidden r037
PID 22728 starts 2026-10-12 13:11:19 local; r038 PID 37288 starts 13:12:19;
180-second deadlines, both exit 0. Evidence build/M0-T445/S4/r037 and r038.
This verifies the ordinary same-window system boundary, not custom application
WM_NEXTMENU redirection, MDI alternate-owner boundary traversal, mouse hover
switching or physical timing. These remain open before full S4 acceptance.

r036 adds open-popup arrow handling to the bounded thread menu observer.
WM_MENUSELECT supplies the currently active HMENU; right-arrow on an enabled
submenu and left-arrow inside a nested popup remain native. Existing popup
columns also remain native, based on their item rectangles. Otherwise the loop
ends and the shared bar changes selection, opening a popup but never executing
a direct bar command solely on arrow movement. The fixture queues left from
open File, opens Extra, selects its command and verifies focus restoration.
Both widths pass this and retained 27,261 checks, MDI popup double-click,
nested scopes, GDI 55/55 and glyph verification. Hidden PID 13752 starts
2026-10-12 13:08:53 local, deadline 180 seconds, exit 0; build/M0-T445/S4/r036.

This is partial navigation, not original-fidelity acceptance. In particular
the fixture currently tests cyclic bar wrapping; original USER traverses the
window system menu at the boundary through WM_NEXTMENU/alternate-menu logic.
That boundary must be implemented before closure, and mouse cross-bar motion
and multicolumn/hierarchy negatives need real-loop tests. Reference only:
OpenNT windows/core/ntuser/kernel/menu.c xxxMNKeyDown, lines 560--730,
SHA-256 300D2D9645C729F8E9D7F4A91D009CCB766640FFB431BF1FD73465473B0A8FA6;
mnloop.c dispatches key input to the active popup first. Private USER state and
server code remain the previously registered no-import boundary. The current
wrapper is project-owned public-API adaptation, not recovered USER source.

r035 covers the previously open MDI popup-loop double-click route. open_bar
reuses the existing bounded same-thread queue-removal observer only while the
merged system popup is tracking; an in-button second click ends that loop and
reposts HTMENU double-click handling after return. The normal SC_CLOSE enabled
check and DefFrameProc child-command route remain authoritative. The fixture
now enters the real native popup with WM_NCLBUTTONDOWN and queues the second
click from WM_ENTERIDLE, then verifies only the child closes. A two-second
menu watchdog is asserted not to fire. Both x86/x64 real menu loops pass,
alongside 27,261 retained checks, nested-scope tests, GDI 55/55 and 273 original
glyph pixels. Hidden runner PID 36744 starts 2026-10-12 13:05:20 local,
180-second deadline, exit 0; evidence build/M0-T445/S4/r035. This improves on
r032's direct-dispatch test but remains queued-message evidence, not physical
mouse timing acceptance. Menu-bar switching and general mutation/lifetime
review remain open; no S4 closure or production release is claimed.

r033/r034 move popup border-scope lifetime from gallery callbacks into the
shared style component. The demo no longer includes menu_scope or manages a
scope property: real system/ordinary popup captures retain their black-edge
assertions with component-only wiring. Nested scope removal now unlinks the
specified owner instead of requiring LIFO teardown; the one thread hook stays
alive until its last scope ends. Per-window menu-loop depth avoids popping an
outer successful scope when an inner allocation/hook setup failed. Removal
and destruction end owned scopes without changing other owners or host state.
r034 adds nested outer-owner teardown/restart and two-level component
entry/exit plus removal-during-loop tests. Both architectures pass those,
27,261 retained checks, the unchanged GDI 55/55 attachment boundary, and the
273-pixel menu asset comparison. Owned hidden run r033 PID 25436 starts
2026-10-12 13:02:45 local, r034 PID 4268 starts 13:03:47; each has a 180-second
deadline and exits 0. These focused tests do not yet prove all reentrant
application item mutation or native animation behavior. S4 remains open.

r032 adds message-level double-click routing for the merged MDI system cell.
It queries the child menu's current SC_CLOSE availability and posts WM_COMMAND
to the frame (DefFrameProc owns child dispatch), never SC_CLOSE to the parent.
The real MDI fixture verifies disabled Close leaves both windows alive, then
enabled Close destroys only the maximized child and activates its surviving
sibling. This does not yet cover a second physical click consumed inside the
first click's native popup loop; that remains an explicit interaction gap.
The private-desktop maximized-frame capture is
build/M0-T445/S4/r032/x86/capture.bmp-mdi-maximized-popup.png (also x64).
Visual inspection confirms the system/Window/restore arrangement and absence
of modern merged minimize/close cells; no full pixel-fidelity claim is made.
Both architectures pass 27,261 retained checks, added MDI lifecycle assertions,
DC/menu fixtures and the 273-pixel resource comparison. Hidden runner PID
19864 starts 2026-10-12 13:00:14 local, deadline 180 seconds, exit 0. No product
component, release image or deployment changes; S4 remains open.

MDI merged-control checkpoint (r029--r031, 2026-10-12): the parent retains
its native HMENU and DefFrameProc command ownership. The presentation snapshot
recognizes USER's standard system/restore bitmap identities, presents the
old-UI system/restore pair and omits the modern merged minimize/close cells.
The original comparison is OpenNT windows/core/ntuser/client/mdimenu.c
(WFOLDUI merge) and mdiwin.c (DefFrameProc WM_COMMAND forwarding to the
maximized child); no historical executable code is imported. r029/r030 fail
at reading the actual merged menu: USER sets both MFT_OWNERDRAW and the
standard HBMMENU_MBAR_RESTORE identity. r030 logs type 0x4100, bitmap 2,
command SC_RESTORE. The reader now accepts that standard-control combination,
while still refusing arbitrary application owner-draw entries.

r031 creates a real MDICLIENT with a Window menu, maximizes the child, checks
the three visible entries (system, Window, restore), hit-tests the custom
restore rectangle, sends its nonclient click and dispatches the resulting
WM_COMMAND through DefFrameProc. Assertions prove the child restores and the
parent does not maximize. Both x86/x64 pass the retained 27,261 checks, gallery
MDI assertions and DC/menu fixture; GDI attachment baseline/final is 55/55.
Glyph extraction still verifies all 273 original menu pixels. Owned hidden
runner PID 5712 starts 12:58:19 local, deadline 180 seconds, exits 0; outputs
are build/M0-T445/S4/r031 and runner/r031.txt. Failed owned runs r029 (PID
31476) and r030 (PID 34324) both terminate with exit 1 and are retained.
This proves message-level restore routing, not physical interaction, complete
MDI visual fidelity, merged-system-menu/double-click behavior or S4 closure.

Uncommitted r003 implementation passes the existing 27,220 checks per width,
new outer/inner SaveDC failure-containment cases and actual MDI client-origin
preservation, activation, maximize/restore and child destruction assertions.
The normal MDI child uses the original CLOSE resource's second half and shares
the existing painter. Native DefMDIChildProc/DefFrameProc retain management;
maximized merged controls and all retro menu surfaces remain incomplete.
r002 screenshots exposed sample-parent text leaking through an unpainted child;
r003 corrects demo background ownership and captures the resulting frame.
This is partial S4 evidence, not closure or a native-menu-animation repair.

Subsequent uncommitted r010 verifies owner-drawn ordinary popup rows, original
VGA.DRV CHECK/MNARROW pixels (273 independently decoded pixels), stippled
disabled text, check toggling, nested command selection and Escape cancellation.
These are real native menu loops with posted input and TPM_NOANIMATION as a
diagnostic control, not physical-input or animated-first-paint acceptance.

r011 fails the actual popup border assertion: attaching only at CBT creation
misses a popup created before WM_ENTERMENULOOP. r012 replaces that trigger with
a same-thread, menu-loop-bounded call-window hook, covering popup show/paint.
The shared menu_scope module subclasses only native popup windows; it preserves
native tracking and client drawing, paints the non-client border, and removes
its hook/subclasses at scope exit. Callback pinning handles reentrant teardown.
r012 background owner PID 17044 (2026-10-12 12:21:32 local), deadline 180 seconds,
exits 0. Both widths pass 27,220 retained checks, the additional menu/MDI/DC
assertions and glyph verification. Actual PrintWindow popup images live at
build/M0-T445/S4/r012/{x86,x64}/capture.bmp-menu-popup.png; their top/bottom
border pixels are asserted black. This does not establish physical composition.
Menu bar, system-menu integration, maximized MDI merging, general menu adapter
and complete keyboard/mouse coverage remain open; no product publication or
S4 closure is claimed.

r013 removes the gallery's hard-coded E/C/M/N command-position dispatch.
Its owner-draw mnemonic handler reads the actual WM_MENUCHAR HMENU, fixture
item label and current enabled/highlighted state; duplicate mnemonics select
and cycle rather than execute an arbitrary item. Direct assertions cover
wrong-menu keys, disabled items, enable-state changes and duplicate wraparound;
the existing real native no-animation loops still exercise command/submenu/Esc.
Background owner PID 8288 (2026-10-12 12:23:43 local), 180-second deadline,
exits 0; both architectures retain 27,220 checks plus these focused assertions.
This is demo input correctness, not yet a general application menu adapter.

r014 adds a menu-kind discriminator (popup row, separator, menu-bar item) to
the same borrowed-DC renderer, with compact bar spacing and no popup glyphs
on bar items. Gallery File is now owner-drawn through that API, retaining
native menu tracking. Focused DC tests assert measurement spacing, selected
background and restored clip. Both-width background run PID 38348 started
2026-10-12 12:26:00 local and exits 0; retained 27,220 checks per width pass.
Reviewed r014/x64/capture.bmp-gallery-4.png: File text is drawn, but gallery 15
still has the explicitly labelled native frame and native menu-bar lower edge.
The complete frame/menu geometry integration remains unimplemented; do not
present this partial row renderer as completed menu-bar styling.

### Menu-bar geometry integration boundary

Read-only original-owner audit on 2026-10-12 identifies why merely removing
the installer's GetMenu rejection is not a valid integration:

- OpenNT windows/core/ntuser/kernel/calcclrc.c, xxxCalcClientRect, computes
  caption and borders from SYSMET, then calls xxxMenuBarCompute with that
  window-relative top. Shifting an incoming rectangle does not replace the
  caption metric: the call subtracts yTopOld.
- mncomput.c, xxxMNRecomputeBarIfNeeded, independently reconstructs the same
  top from window styles and system metrics; mndraw.c repeats that origin
  when drawing/recomputing a menu bar. Custom WM_NCCALCSIZE alone therefore
  cannot establish a shared custom/native item geometry contract.
- Source hashes: calcclrc.c
  49C51951B20CF984622AEB512F113BAB1E98D5E8A08D066D35FBD3DB99D82B6C;
  mncomput.c
  246F75077A834FAC3519C8096CD38BF0FB019C53E162FBBF2A02676612767769.
  These are historical contract evidence, not a trace of modern USER internals
  and not proof that every modern implementation has identical internals.

The standalone bar integration must use one component-owned item layout for
painting, hit testing, wrapping and popup placement below the custom caption.
Continue to use native HMENU state and native popup tracking; do not copy
private PMENU/PWND structures, detach application menus silently, mutate
system metrics, or fake alignment by moving only painted text. Keyboard entry,
cross-item navigation and MDI merged controls must be included in this layout
contract before the gallery's native-frame label can be removed. This is an
implementation boundary within S4, not a completed feature or expanded product
integration. The original USER/Win32k source remains a no-import stopping
boundary; this audit imports no executable source or original media.

r015/r016 add the component's stateless menu_layout primitive, consuming
renderer-measured sizes and returning window-relative item rectangles and
total height. Tests cover row wrapping, explicit breaks, oversized clipping,
empty input, right/bottom-exclusive hit testing, gaps and arithmetic limits.
Review of mncomput.c MBC_RightJustifyMenu corrected r015's flag-only suffix:
r016 groups all entries from the first MFT_RIGHTJUSTIFY item, including later
unflagged entries, with an explicit test. The authored layout arranges that
group per row; full historical multi-row fidelity remains an integration check,
not a claim inferred from this primitive's tests. No private USER structures
or source bodies are imported. Native popup tracking stays outside this unit.
r016 owner PID 17892 started 2026-10-12 12:30:47 local, deadline 180 seconds,
exit 0. Both widths pass focused layout/DC tests and 27,220 retained checks.
This layout is compiled into the library but not yet connected to HWND bar
placement; it is component evidence, not completed menu-bar integration.

r017 integrates string-menu bars into the frame's shared measure/paint/hit
path through transient menu_bar text/geometry snapshots. Application HMENU
stays attached and borrowed; no original menu data is rewritten. Gallery 15
now visibly combines the retro frame, File bar and black lower separator.
String bar entries can open their existing native popup or post their command.
Owner-drawn/bitmap bar entries are unsupported at initial installation.
r018's click-route test failed because it asserted before native queued
WM_COMMAND dispatch; r019 drains only the owned window's command messages and
proves HTMENU -> actual component click path -> native popup -> WM_COMMAND,
with command count 3 and no timeout on both widths. This is queued-message
private-desktop evidence, not physical mouse or animated first-paint proof.
r019 background PID 17016 starts 2026-10-12 12:35:44 local, deadline 180 seconds,
exits 0; both widths pass 27,221 retained checks and focused assertions.
Reviewed r017/x64/capture.bmp-gallery-4.png shows the integrated frame/bar;
r019 retains the same gallery capture path. Keyboard bar navigation, selected
bar highlighting, menu mutations/unsupported dynamic entries, narrow/empty
client behavior, system-menu conversion and MDI merging remain open. Native
GetMenuItemRect reports native geometry, not this custom bar's coordinates;
its caller contract must be resolved before general product integration.

r020 shares mouse/Alt-letter popup entry through open_bar. It delivers the
owner's WM_INITMENU before re-reading current text/state/geometry, highlights
the selected bar entry while tracking, and clears it afterwards only if the
attachment survived reentrant application callbacks. No HMENU selection state
is overwritten. Private-desktop assertions PrintWindow the real owner during
WM_ENTERIDLE and after return: selected background matches palette.active,
then palette.light. Mouse and SC_KEYMENU('f') each produce exactly one actual
queued WM_COMMAND and the expected check toggle. Both-width background run
PID 3296, start 2026-10-12 12:37:55 local, deadline 180 seconds, exits 0 and
retains 27,221 checks per width. These are message-driven tests, not physical
Alt-key input. F10/Alt-alone, arrow navigation, duplicate top-level mnemonics
and switching between open top-level popups still require implementation.

r021--r024 add bar-only keyboard selection from SC_KEYMENU(0), left/right,
Enter/Down, disabled-command refusal, Escape/F10 cancellation and restoration
of the previous live focus window. Removing the style while selecting restores
focus too. A popup canceled from keyboard leaves its bar selected; successful
commands leave menu mode. This is not yet open-popup cross-bar navigation.
Gallery adds disabled and direct-action bar entries. Focus tests use an owned
gallery child rather than a system EDIT control. r021 passed behavior but
failed the unchanged GDI resource boundary after EDIT initialization; the
attribution to EDIT caching is a hypothesis, not a proved host defect.
r022/r023 stop earlier at a caption-pixel assertion: PrintWindow captured only
the left portion of an off-screen second fixture on the 640x408 work area.
r024 positions each owned pixel-capture window at (0,0) for PrintWindow and
then restores it, retaining all pixel assertions. These captures explicitly
test on-screen frame pixels, not off-screen composition. Both widths now pass
27,261 checks, keyboard/focus assertions and unchanged GDI resource bounds
(x64 attachment baseline/final 56/56). Background PID 18876 starts
2026-10-12 12:42:52 local, deadline 180 seconds, exits 0. Retain failed runs;
neither the initial GDI attribution nor off-screen host behavior is declared
fully diagnosed. Tests inject the menu-entry messages, not physical F10/Alt.

r025 adds temporary native string-popup item adaptation in menu_items.
The frame forwards WM_INITMENUPOPUP to the application/native owner first,
then applies owner-draw presentation to ordinary text/separator items only.
Existing application owner-draw and bitmap entries stay untouched. Shared
measure/draw/mnemonic callbacks use the frame font/palette; exit/removal restores
the prior item data and owner-draw bit without overwriting current command IDs,
text or enabled/check state. Native menu tracking and system commands remain
native. Focused tests verify mnemonic resolution, disabled refusal, original
item-data restoration and a check-state change preserved across restoration.
Both-width r025 background run PID 10684, start 2026-10-12 12:45:34 local,
180-second deadline, exits 0; 27,261 retained checks and new adapter cases pass.
This does not yet prove full system-menu visual/physical acceptance. Popup
border scope integration, reentrant/nested loops, dynamic text changes and
mixed application owner-drawn mnemonic handling need follow-up. No publication
or S4 closure is claimed.

r026/r027 exercise gallery 12's actual SC_KEYMENU(space) native system-menu
entry, not an authored replacement HMENU. r026 catches Close remaining native:
the adapter's bitmap guard also excluded standard HBMMENU_POPUP_* artwork.
r027 temporarily removes only the four recognized close/restore/minimize/
maximize system bitmap handles while adapting their text, then restores them.
Custom bitmap items remain untouched. The real loop asserts Close is adapted,
captures its popup, cancels using queued Escape, and compares original Close
type/data/bitmap after return. Both-width r027 owner PID 34620, start
2026-10-12 12:48:21 local, deadline 180 seconds, exits 0; 27,261 retained checks
also pass. Reviewed x64/capture.bmp-system-popup.png shows rendered native
system-menu labels and the scoped black border. This is direct-print evidence,
not animation/physical acceptance. Dialog command visibility and disabled-item
fidelity against original Win3.1 still need audit: the host menu currently
retains its Restore/Size/Minimize/Maximize entries. Do not call that final visual
equivalence or alter command semantics without the original-style audit.

r028 restores only the reopening popup before forwarding WM_INITMENUPOPUP,
then rebuilds its presentation from current application labels. Other prepared
popups retain their bindings. WM_MENUCHAR now offers the original owner first
refusal; the component resolves its text only after MNC_IGNORE, preserving
application owner-drawn handlers. Focused tests prepare two menus, restore and
rename one, re-prepare it and assert the new/old mnemonic outcomes while the
other menu stays adapted. Both-width background PID 29932 starts
2026-10-12 12:50:34 local, deadline 180 seconds, exit 0. Full retained regressions
and adapter tests pass; mixed-owner callback precedence still needs a dedicated
real-window assertion beyond this source correction.
Read-only original mnsys.c SetSysMenu distinguishes framed-dialog state, while
mnapi.c xxxSetDialogSystemMenu installs ID_DIALOGSYSMENU explicitly. A framed
popup must not simply be assumed to have that separate dialog menu installed.
The Win3.1 original-instance comparison remains open; no command deletion was
introduced on the strength of this NT source observation alone.

## Policy contract

Use one policy with three values: off, Win16-only, all-window-programs.
CLI explicitness is separate from its effective value: omitted options inherit
the caller's controlled launch context; without one, default is off. An explicit
option overrides inherited policy; when both options occur, all wins. Do not
introduce an unrequested disable option or change existing run16 target parsing.

| Effective policy | Win16 window marker | Native x86/x64 window marker | Child propagation |
| --- | --- | --- | --- |
| Off | Absent | Absent | Off |
| Win16-only | Present | Absent | Preserve Win16-only, including through native intermediates |
| All | Present | Present | Preserve All |

The scope is the controlled launch chain, not every program on the machine.
Text/DOS intermediates preserve policy where they launch a supported descendant,
but neither their Console title nor NTCON's outer window becomes a style target.
Launching an entire Windows desktop as a DOS guest does not restyle its pixels.

Carry policy through existing launch/request/Hook context boundaries, with
appropriate version validation if records change. NTSRV retains task policy,
not a window registry. WOW retains policy per owning Win16 task, never as one
worker-global toggle or an unscoped current-task variable. A window uses its
actual owning task's policy even when shared-worker tasks interleave. Native
Hook contexts carry policy across both bitnesses and run16 redirection. Do not
alter global user environment, add IPC, polling, helper processes or filename
exceptions. Any unreached guest child-creation boundary must be audited before
claiming propagation; a direct-launch-only implementation cannot close S5.

## S5 visible marker

Reuse the owner-approved `src/win16-style` experimental library in S5: x86
for WOW32/nthook32, x64 for nthook64. Each consumer supplies its effective
window enable decision; the shared marker and later renderer do not determine
program type or own propagation. No temporary parallel marker implementation.

An enabled supported application window displays exactly one decoration:

`(Win16 Style) <application title>`

The same literal prefix is used for either option. Disabled windows remain
unchanged. Cover window/dialog initial titles, later application title changes,
empty titles, A/W paths and later GUI threads. Repeated notifications must not
accumulate prefixes. Keep the application's undecorated title separate from the
presentation prefix; do not infer policy by searching title text or remove an
application-authored matching string. The supported title-read semantics and
installation/detach ordering must be specified and tested before implementation
is accepted, avoiding reentrant WM_SETTEXT loops. The S1 decision uses a real
caption decoration: title-read APIs and exact-title lookup observe the changed
caption. Do not virtualize those APIs or claim unmarked-title equality; test
and report title-dependent application compatibility explicitly.

The marker uses the same per-window enable decision and installation boundary
that later rendering will consume, not a temporary process-wide caption scanner.
No production retro colours, font, borders, menus or layout are required in S5. A visible
marker proves enablement only, not delivery of the Win3.1 appearance.

## S5 acceptance matrix

- Default launches: existing Win16 and x86/x64 native windows remain unmarked.
- Each option: exact expected marker on real eligible windows, absent on excluded
  families; both options together; explicit override and omitted inheritance.
- Shared WOW worker: marked and unmarked tasks coexist in either startup order;
  create/update/destroy windows without leaking policy into another task or a
  subsequent launch. Worker reuse is part of the test, not bypassed by isolation.
- Launch chains: Win16 to Win16, Win16 to native to Win16, native to Win16,
  x86 to x64 and reverse, with both policy values. Verify the retained native
  intermediate policy even when its own window is unmarked. Include existing
  DOS/shell/run16 relay boundaries, returns and failed child launch.
- Real windows: normal windows and modal/modeless dialogs, secondary GUI threads,
  changing/empty/non-ASCII titles, application-authored prefix, destroy/recreate,
  and no doubled decoration or stale per-task/window state.
- Evidence combines actual HWND titles, owning task/process identities and
  child-launch provenance; a screenshot or a stored flag alone is insufficient.
  Preserve documented title-API behavior with the explicitly decorated caption
  and existing run16 CLI behavior, not identical unmarked title strings.
- External activation brokers, pre-existing target processes and unsupported
  custom captions are explicit boundaries, not silent passes. Do not broaden
  the existing controlled Hook propagation guarantee without owner review.
- Follow normal production delivery: background-owned builds/tests, focused
  positive/negative/lifecycle checks, serial required product regressions,
  coherent ten-image publication to assets/release and configured system32,
  hash verification, commit/push and owner acceptance before style implementation.

## Planning verification

This is a documentation-only replanning delivery. Governance, relative-link
and diff checks are retained under build/M0-T445/S1/r003. The subsequent
source-owner planning update admits the specialist directory without creating
it; checks for that update are under build/M0-T445/S1/r004. Neither planning
delivery changes a protocol, product binary, guest or installed configuration.
The new standalone-stage admission first hit the Current Work size limit in
build/M0-T445/S2/r001. Its closed S1 packet was retained in indexed evidence;
the corrected admission is checked under build/M0-T445/S2/r002. This is planning
only, not a built or launched UI experiment.
