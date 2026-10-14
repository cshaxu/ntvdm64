# NTW16S Win3.x window-style DLL

S9 owner-directed cleanup retires the temporary title-marker module and its
two diagnostic exports. Attachment never adds or strips caption text; initial,
dynamic A/W and dialog titles remain application-owned. Policy is verified by
actual attachment, frame state and drawing, not a title prefix.

T445 S5 moves the entire S4 owner-accepted core here. One source family builds
ntw16s32.dll and ntw16s64.dll; ntw16s.h is the public same-thread C API.
W16Style.exe imports the matching DLL, not a static renderer. The public
font lease, window/control attachment and control-creation scope contracts
below are unchanged. Ntw16sGetAbiVersion reports NTW16S_ABI_VERSION.

Build with tools/build/New-Ntw16sNinja.ps1 for each architecture, then invoke
its build-style.cmd from an owned background runner. Production links only
the DLL import library. Private fault-injection fixtures may statically link
the same implementation; they are never release inputs. Headers other than
ntw16s.h and its declared public includes are private, not consumer ABI.

Callers retain their LoadLibrary/import reference until all HWND attachments,
control scopes and font leases are released on their owner threads, with no
outstanding calls/callbacks. DllMain never enumerates windows, installs hooks,
creates fonts or performs cleanup under loader lock. No guest/WOW/Hook or
process-selection dependency is introduced. A 16-bit guest cannot load this
PE DLL; WOW32 integration calls the x86 DLL from its native host.

The following S3/S4 design and limitations remain applicable to presentation;
references to experiment-only packaging are superseded by S5 publication.

S9 owner-selected typography uses one actual20-pixel System bitmap font for
styled captions, menus and supported standard controls. font.c rasterizes the
host's16-row System glyphs, scales them once with COLORONCOLOR and registers
a process-private in-memory FNT. There is no embedded VGASYS.FON, global font
installation or second title stretch. Same-GUI-thread references retain the
resource until the final HWND/DC user releases it. Control removal restores
the application's latest borrowed WM_SETFONT value; native EDIT/list/combo
layout, caret, selection and font-driven integral-height changes remain native.
The current exact-glyph inventory is CP1252 characters32..255; wider Unicode
fallback and non-ANSI System realizations are not certified by this fixture.
The title grows to at least20 rows at low DPI; captionless resize geometry
does not grow. This is the owner's explicit typography adaptation, not a claim
that original VGA caption metrics were20. Sample01 has larger control spacing
to expose the entire inventory at this size.

S4 owns presentation for the six standard-control families listed below.
controls.h exposes same-thread explicit install/remove; control_scope.c
attaches supported descendants at creation within an explicit root scope.
Native state and notifications remain native. Gallery20 shows checkbox,
three-state and radio variants; gallery21 adds push/default buttons, labels,
group boxes and static boxes. Gallery22 adds single/multiline, read-only,
disabled and password EDIT examples. Native EDIT selection/caret/text/undo
remain intact; the component owns its border, shared font and the opt-in
scrollbar presentation described below. These are implemented surfaces,
not a claim of complete original-pixel or physical-interaction acceptance.
EDIT consumes WS_BORDER during native creation, so win16_style_edit_install
requires the original border bit from the creator/creation observer. The generic
install refuses EDIT rather than inferring this bit from formatting margins.
Gallery23 adds native string LISTBOX single/multiple selection and disabled
states. Its client rows are drawn by the component; strings, selection and
notifications remain native. Single- and multicolumn lists are supported;
owner-drawn and no-data lists remain excluded. LBS_USETABSTOPS requires the
explicit known-state attachment described below, not generic installation.
Simple WS_BORDER lists have a black external edge without changing client
geometry. LISTBOX and EDIT non-client scrollbars share the fixed-thumb
painter/tracker through the explicit opt-ins and range limits below.
Gallery24 covers string CBS_DROPDOWNLIST, CBS_DROPDOWN and CBS_SIMPLE with
original COMBO artwork where a dropdown button exists and an attached list
renderer. Native child EDIT remains the input/selection/undo owner and is
excluded from the surrounding combo painting. A separately reattached list
is not removed by its former combo owner. Full physical interaction, custom
application owner-draw and scrollbar coverage are not established by this slice.
Creation-scope attachment is implemented, but does not imply support for
every style variant or application-owned drawing. Complete physical interaction
coverage remains open. Original glyph provenance is in resources.md.

Gallery25 adds standalone horizontal/vertical/disabled SCROLLBAR controls.
Sample01 also includes the legacy SBS_SIZEBOX. It retains native creation,
alignment and parent size/maximize commands, outside the ordinary scrollbar
tracker. Its painter uses the original inner-border offset and scrollbar
system-color brush; it never draws a Win95 diagonal grip. Current native
window/scrollbar metrics are retained rather than resizing an existing control
to a historical VGA metric. COLOR_SCROLLBAR is read, never changed globally;
it is independent of the component's button-face preset. SBS_SIZEGRIP remains
outside the Win3.x profile.
The component draws original arrows and fixed-size thumbs with its shared
geometry/tracker; native range state and application scroll notifications are
retained. The demo handles those notifications to update position. Sample16
and sample01 exercise styled window non-client scrollbars as well. Native
accessibility geometry, complete DPI coverage and physical held-input
acceptance remain open; see the detailed contracts below.

The outer-system-menu mouse diagnostic is explicitly opt-in with the fixture
environment variable WIN16_STYLE_MOUSE_DIAGNOSTIC=1. It currently fails before
observing a popup on the private desktop. Default tests report that route as
not verified; keyboard-route success does not waive the owner's mouse report.

Project-authored same-thread static library with owner-approved original VGA
button pixels embedded locally; see [resource provenance](resources.md).
The title uses the host System font by owner-approved visual exception;
no guest/mirror executable code is imported. S1's source ladder rejects
NTUSER server composition at the existing private server boundary; this is the
owner-admitted local presentation feature, not an NTUSER implementation.

win16_style_install/remove operate only on the calling GUI thread's HWND.
The window retains its application procedure, client drawing and Windows
system-command semantics. No worker identity, broker, Hook, polling or thread
is introduced. No production executable links this experiment.

layout.c owns geometry shared by paint/hit-testing. win16_style.c owns the
subclass, local GDI resources, palette, title painting and button gestures.
The public header is a process-local API, not a wire format.
Normal installation/removal preserve client origin and dimensions.
Maximization keeps native window management; while maximized, client sizing,
painting and hit testing share the visible monitor-work-area intersection so
the host's off-screen resize border cannot clip the custom title/buttons.
Destruction
removes the subclass; explicit removal is required before unloading code.
Repeated installation fails with ERROR_ALREADY_EXISTS; unsupported frame forms
fail with ERROR_NOT_SUPPORTED and remain native.

The S3 profile accepts top-level caption-button combinations, captionless
sizeable/thin frames, borderless windows and dialog-style windows.
WS_EX_DLGMODALFRAME explicitly selects the wider caption-colored
dialog border and light inner outline; no title or button-absence heuristic is
used. WS_DLGFRAME alone cannot distinguish this because WS_CAPTION includes it.
Client-edge/tool/layered windows and minimized/maximized installation remain
unsupported. S4 adds frame scrollbars, string menu bars and
the standard MDI merged system/restore controls; arbitrary application
owner-drawn or bitmap menu bars are not supported.
S4 now supports normal child/MDI frames with parent-relative client preservation
and the original child system-box image. Maximized MDI children retain native
management while the parent presents the merged system/restore controls.
Consumers must not introduce unsupported forms while attached. System and
ordinary string popup menus use retro painting with native menu tracking.
System-menu tracking remains native, not a separate TrackPopupMenu loop.
If the host reports an empty native system-button rectangle (modern dialog
frames), our button enters the system menu through SC_KEYMENU/space after
release inside its rectangle, using existing capture/cancellation tracking.
While this fallback menu runs, a scoped thread-local WH_GETMESSAGE hook maps
double-clicks inside our system button back to native NC handling after the
loop exits. USER's menu capture uses screen coordinates and can consume the
click before WH_MSGFILTER. Disabled-close handling remains native; no delay,
global mouse hook or global setting change is used. The binding is restored
and the hook removed before returning, including window removal/destruction.
Normal non-client activation, cursor/hover and compatibility caption-draw
messages belong to the custom renderer; delegating their drawing can leak
native buttons into the shorter caption's client area. Client cursor handling,
WM_ACTIVATE/focus and system commands remain downstream. Minimized activation
and mouse presentation remain native. No extra repaint timer or larger frame
is used to cover native drawing.
This is not a creation/Hook adapter or real title-prefix implementation.

The supplied reference palette is a preset, not a universal Win3.1 default.
High contrast reads host colors; no global settings are written. The shared
20-pixel System bitmap font described above supersedes the earlier separate
caption scaling and clipped text canvas. Its process-private FNT is generated
from the host font, not an embedded original font. Exact VGA glyph fidelity,
wider Unicode and non-ANSI System realization are not claimed.
The demo alone opts into per-monitor DPI awareness; the library never does.

Build/test (invoke from an owned background PowerShell process):

    tests/observation/verify-win16-style.ps1 -BuildRoot <repo>/build/<run>

This generates independent x86/x64 Ninja graphs, /MT /O2 /W4 /WX archives and
GUI-subsystem win16-style-demo.exe. Without arguments it opens a 27-entry
selector: double-click a row or choose Open selected. Close samples closes
only the demo's sample HWNDs. Entry 1 is the combined control showcase;
entries 2--14 exercise styled top-level forms/states. Entry 18 exercises styled
MDI frames with native MDI management.
Entry 15 exercises the retro menu bar and popups; entry 16 exercises styled
window scrollbars. Entries 17 and 19 remain native child/ordinary references;
entries 20--27 cover focused standard-control variants.
Minimized presentation is also still native. F2 changes title,
F3 changes caption colors and Esc closes a sample (including borderless ones).
--test <bitmap> runs on its own private desktop and captures only its HWNDs.
--menu-test <absolute-build-bitmap-path> uses the input desktop and only its
owned windows for real popup, selected-pixel and close-command checks.
Run it in an owned, bounded background process; private-desktop dispatch
checks deliberately do not claim real menu tracking.
The build also produces native-menu-probe.exe and native-menu-v6-probe.exe:
same standalone native source without this library, respectively without and
with a Common Controls v6 manifest. They isolate host menu drawing. No arguments
opens a manual comparison. `--probe <absolute-build-bitmap-path>` captures a
visible owned system popup after 500 ms and exits; optional `--popup`,
`--popup-no-animation`, `--file`, `--file-no-animation`, `--file-keyboard` or
`--system-dpi` selects a diagnostic control. Exit 5 is unavailable observation,
not success. A final optional delay argument (1..10000 milliseconds) tests
whether drawing recovers without hover; the default remains 500 milliseconds.
After an explicit delay, `--trace` logs only this thread's native popup messages.
`--isolated-print`/`--isolated-client` print into an owned DIB on a private
desktop; `--isolated-hidden-print`/`--isolated-hidden-client` sample before show.
`--isolated-return` additionally wraps only the probe's native popup procedure
to record its unchanged WM_PRINT return. `--isolated-classic` is a process-local
theme negative control. None is physical-display evidence or a runtime fix.
No global animation/theme setting changes. These controls do not
repair menu drawing. `--queued-menu-test` measures 18 native menu-loop cases,
including disabled Close and outside-button input; these are queued messages,
not physical mouse input. `--gallery-trace <build-log-path>` records only this
demo thread's queue, dispatch and modal-filter messages for physical diagnosis.
The repaired gallery-12 double click also has a separate actual-input check.
PrintWindow capture is rendered-window evidence, not a physical-input/RDP test
or capture of DWM composition. The fixture itself uses no computer-use automation.
The non-client regression additionally reads existing WindowDC pixels without
PrintWindow: assert a white client-top strip, unchanged caption/buttons through
cursor/hover, title/icon/theme/activation changes and minimize/restore. It also
checks all resize cursor directions and application client-cursor delegation.
Owner subsequently authorized actual-desktop computer-use inspection of the
standalone demo; it is separate evidence, not replaced by these probes.

S4 work in progress: menu_paint supplies borrowed-DC row measurement/drawing.
menu_scope provides same-thread native popup-frame scopes. The shared window
component now enters/exits these at WM_ENTERMENULOOP/WM_EXITMENULOOP and cleans
them up on removal/destruction; applications do not wire a separate border hook.
Nested scopes support an outer owner's removal while an inner owner is active.
It neither owns HMENU data nor replaces input tracking. Only that thread's
popup windows shown/painted within the scope are temporarily subclassed.
No global hook/theme/animation change is made. Ordinary string popup entries
are temporarily adapted and restored; application owner-draw rows remain with
their owner. Tests now exercise actual queued popup loops for keyboard and
hover switching, nested tracking, MDI frame/child WM_NEXTMENU transitions,
merged system-button double-click, and callback removal/destruction. These
are bounded automated cases, not full physical-interaction acceptance.
Final S4 review and owner visual acceptance remain open. The private-desktop no-animation menu
tests and direct-print images do not certify the deferred native animation bug.
## Consolidated control review

Gallery sample 01 is the combined review surface for all six original standard
control families: BUTTON, EDIT, STATIC, LISTBOX, COMBOBOX and SCROLLBAR. It uses
the same scoped creation attachment API as the focused samples, not separate
demo-only drawing. Samples 20--27 retain focused behavior tests, including
multicolumn list geometry, painting/clipping and native horizontal scrolling. The combined
page is not evidence that every style variant is complete.

STATIC text includes left/center/right wrapping labels, LEFTNOWORDWRAP (no
automatic word wrap, but explicit line breaks and tabs remain), and SIMPLE's
single-line original fast path. The combined page shows the latter two as
well. Text/creation style and the application's borrowed font remain native;
pixel tests compare source-backed formatting vectors and removal state.
STATIC icons borrow the native STM_GETICON handle and preserve native loading,
replacement and size management. The painter follows the referenced original
STATIC image branch, including unchanged disabled-image colors. CENTERIMAGE is
explicitly a host extension, not a Win3.1 style. It uses natural-size clipping;
temporary GetIconInfo bitmaps are released, never the application's icon.
Sample01 includes an authored monochrome icon. Pixel/lifetime tests cover
enabled/disabled, ordinary/centered, replacement, clearing and detach/destroy.
SS_BITMAP is a host extension with no themed text or chrome: retain native
painting and image management, rather than adding another blitter. Printing
saves/restores the caller DC and clips it to the client (the native centered
painter otherwise leaves an excluded-image hole in that DC). Eight pixel and
lifetime vectors compare ordinary/centered, enabled/disabled, small/large,
replacement and clearing against the original OpenNT image branch. A cleared
bitmap handle paints nothing; it is not the icon's background-fill contract.
Sample01 includes an authored24-bit checker bitmap. These image fixtures do
not certify arbitrary alpha-image conversion or application owner-draw.

The S4 coverage audit must also cover disabled/focused/pressed/checked/mixed
states, text alignment, single/multiple selection, all three combo forms,
horizontal/vertical and embedded scrollbars, icon/bitmap presentation, and
window nonclient scrollbars. Application owner-drawn/custom controls are not
standard renderer-owned pixels and must remain explicit boundaries. Modern
common controls absent from Windows 3.x are not silently claimed as original
Win3.x standard controls. Top-level nonclient scrollbars now share the control
scrollbar painter and fixed-thumb hit geometry, retaining native ranges;
sample16 and the combined sample01 exercise them. Probe coverage and its
physical-interaction limitations are recorded below and in the scrollbar audit.

List/ComboLBox nonclient painting is now owned directly by the style subclass,
without a native-scrollbar prepaint. It does not forward nonclient hover/leave
messages to start the modern hover skin. Without the scrollbar opt-in, native
click/drag tracking remains; the opt-in uses the shared tracker below.
Captured list motion repaints only when the native top index, selection or
caret changes. The visible-window-DC regression covers two list forms and all
three combo forms, without calling WM_PRINT to repair the surface before
comparison. These early surface checks alone do not certify intermediate
pixels; the later opt-in tests separately cover tracking and fixed geometry.

scroll_layout.c/.h now supplies independently tested original-profile fixed-thumb
geometry and hit ordering, with checked native-width arithmetic. Provenance and
the explicit modern-width extension are recorded in
[the scrollbar audit](../../docs/etc/evidence/m0-t445-s4-scrollbar-fidelity-audit.md).
The standalone SCROLLBAR class now uses this geometry for drawing and mouse
hit testing together. Embedded bars not covered by the list/edit opt-ins
below still use native tracking and retain the recorded transient leak.
Built-in GetScrollBarInfo continues
to expose native geometry; accessibility compatibility is not complete.

scroll_track.c/.h supplies the corresponding callback-free per-instance
tracking model: bounded notices, fixed-delay/repeat timer actions, grab offset,
outside/back and source-observed cancel/end ordering. The HWND adapter must
apply those actions and revalidate attachment and gesture identity after each
application callback. controls.c supplies that adapter for SCROLLBAR,
with full-width track-position queries and live horizontal/vertical drag and
callback-removal tests. Held-repeat, further reentrancy and intermediate-paint
acceptance remain open; this is not a completed repair of every embedded bar.

Standalone SCROLLBAR focus uses the system gray blinking caret with the retro
thumb's geometry. Because native setters are forwarded without native redraw,
their native caret-position update cannot be relied upon. The attachment owns
focus/size/position synchronization, hides the caret during capture, cleans it
up on lost focus/removal and restores native focus presentation on removal.
Blink timing is not changed globally and no extra blink timer is introduced.
Frame WM_HSCROLL/WM_VSCROLL handling distinguishes child-control notifications
from the frame's own bars, avoiding unrelated parent-frame repaint.

The opt-in `win16_style_list_scrollbar_enable` now extends this tracker to
single-column native lists/ComboLBox on both axes and horizontal multicolumn lists.
The creator must own its
per-window unthemed configuration; the component itself does not replace an
unknown theme association. Native list data/range/selection remain native;
forwarded mutations suppress native repaint with nesting, and thumb requests
use full-width LB_SETTOPINDEX. Multicolumn thumb positions are columns,
converted to the first item using the current client height and item height;
line/page commands retain native WM_HSCROLL semantics. Popup capture is retained
across a gesture and attachment removal.
Fresh owned fixtures pass wheel/drag/motion/intermediate-pixel and removal
checks in both widths. The ordinary combined showcase, LISTS, COMBOS and
MULTILIST pages opt in their fresh demo-owned lists and manage the per-window
theme setting explicitly. Ordinary horizontal lists use native WM_HSCROLL
pixel-position commands; only ranges fitting the16-bit message position use
custom tracking. Wider ranges remain native pending a full-width binding.
Frame bars use the separate explicit API binding below. Accessibility geometry
and unsupported-range native-skin fidelity remain incomplete.

Single-column list text painting now applies the native content xOrigin:
LB_GETITEMRECT extends its right edge rather than subtracting the horizontal
offset from its left edge. Translate that rectangle by client.right-row.right,
matching USER's separate drawing-origin adjustment. Do not infer content origin
from SetScrollInfo, which can update a bar without scrolling its contents.
The horizontal-content fixture verifies actual pixel translation and then
custom thumb dragging, unchanged vertical position, both bar surfaces and
native-paint boundaries. Sample01's right-hand list demonstrates both axes.

`win16_style_edit_scrollbars_enable` is a trial for an already styled
multiline EDIT under the same caller-owned per-window theme contract. It reuses
the embedded tracker and fixed-thumb painter; each gesture records its axis,
so only that axis receives tracking pixels and pressed-arrow state. Thumb motion
uses original native WM_VSCROLL/WM_HSCROLL commands, retaining native parent notification
semantics. The combined showcase
opts in its owned multiline edit. Focused tests cover real content position,
wheel/drag/capture, retained text/selection, undo after removal and intermediate
native paint boundaries on both axes. Drag sends no extra EN_VSCROLL/EN_HSCROLL, same-position release
does not invent a move, and ordinary line movement retains its notification.
This is not full EDIT acceptance: at65535 or more lines, native EDIT reads its
own inaccessible tracking position instead of the message's16-bit position.
That range retains native geometry/input (and the unresolved native-skin leak),
not truncated custom motion. Resize/font and held-input transitions still need
coverage. Horizontal effective positions above65535 also retain native input;
the public thumb message must not silently truncate them. The both-axis fixture
checks horizontal movement does not change the vertical origin or bar pixels.

Menu bars, popup/system-menu text, titles and supported controls use the shared
actual20-pixel font. Measurement, hit testing and drawing use that same font;
there is no separate title stretch. The older r162 stock-font comparison is
historical evidence for the small-File defect, not the current font contract.
win16_style_font.c verifies realized metrics and CP1252 raster coverage;
controls retain the application's latest borrowed font for restoration.

The custom frame owns WM_PRINT composition as well as visible nonclient
painting. Client printing uses the actual client origin/clip, followed by
visible child windows (and owned windows when requested), with saved DCs.
Do not strip PRF_NONCLIENT and delegate the rest unchanged: that loses the
native client-origin adjustment. Conversely, forwarding native frame printing
leaks native menu text under the shorter retro bar. The bounded public-API
composition follows the referenced DWP frame/client/child ordering without
importing private USER code. Visible caption/theme handling is unchanged.

The demo accepts `--capture-test <bitmap-prefix>` for a quick isolated
showcase-only diagnostic. It records native PrintWindow and explicit-message
captures separately, and asserts client origin and blank bands around the
controls. Full acceptance still uses `--test` and the existing scroll/API
gates. Use original-detail image inspection: the tool's default preview has
displayed black for files whose raw pixels and original-detail view are valid.

Frames with initial or later-added scrollbars reuse the Win3.x fixed-thumb tracker.
The standalone demo explicitly calls `win16_style_get_scroll_info` and
`win16_style_set_scroll_info` in its frame scroll handler. The first preserves
native data while supplying the active32-bit track position; the second keeps
native state updates but replaces immediate native painting with the affected
retro bar. Unstyled windows delegate normally. This local binding is needed
because SB_HORZ/SB_VERT queries bypass subclass messages; it is not an installed
process hook or transparent arbitrary-application integration.

Each bar composes into a temporary bar-sized bitmap and publishes once, keeping
the checker phase. No persistent buffer or repaint timer is added. Caption
capture cleanup no longer redraws the whole frame for scrollbar gestures.
r173 proves positions above65535, unrelated-axis position/pixel preservation,
capture release, cancellation and active detach/reinstall on both widths.
Physical held-input acceptance, other scrollbar API entry points,
accessibility geometry and broader reentrant notification behavior are
still open; the full S4 is not closed by these focused passes.

Each attached frame keeps the tracker ready even before bars exist. Native
nonclient calculation updates USER's scrollbar-presence state before our
client rectangle replaces its result. Skipping that calculation left bars
added after attachment with zero native rectangles. This does not forward
native nonclient painting. The late-bar fixture adds both axes, hides/shows
them, then exercises the existing 32-bit drag and cleanup matrix.

## Current verification boundaries

The r231 both-width standalone run passes the retained27,261 gallery checks,
font, menu/control-presentation and scrollbar gates. It also compares native
and styled list text origins (single-column2px, multicolumn0px). This is not a
claim that27,261 checks cover every S4 requirement. The native tab-stop oracle
in tests/app/win16_style_list_tabs.c establishes set-time pixel preservation
across font changes. Subsequent known-state tab tests are described below;
generic installation still rejects unknown LBS_USETABSTOPS state. No opaque
host tab state is read or reset to disguise that unsupported attachment case.

Control WM_PAINT composes the dirty region in a transient bitmap and presents
once; native intermediate state drawing is suppressed while native state,
focus, capture and notifications remain active. Caption-button feedback updates
only its own rectangle. Focused tests observe native-paint requests, screen
fills, presentation, radio/checkbox behavior and preserved distant frame pixels.
These tests support the click-flicker repair; they do not replace the owner's
physical acceptance. WM_PRINTCLIENT remains a borrowed-DC drawing contract.

Private-desktop PrintWindow showcase captures have an unresolved duplication
artifact not present in the direct WM_PRINT capture. Neither capture route
proves physical composition. The native-menu fresh-session failure remains
deferred to S7 independently of the retro menu implementation. S4 remains open
for unsupported admitted variants, remaining fidelity/interaction review,
final delivery review and owner acceptance. Detailed run history stays in
[the scrollbar audit](../../docs/etc/evidence/m0-t445-s4-scrollbar-fidelity-audit.md),
not in this current component contract.

## Tabbed string lists

win16_style_tabbed_list_install accepts the caller's known current pixel tab
positions: zero entries means default tabs, one is a repeated interval, more
than one specifies explicit stops. It copies that input before attachment and
never calls LB_SETTABSTOPS to replace the original native configuration.
Unlike this pixel-state input, subsequent ordinary LB_SETTABSTOPS messages take
dialog units. The subclass retains their successful set-time pixel conversion;
font changes do not recompute existing explicit stops. Original-style text
drawing uses TabbedTextOutW with the item origin, retaining selection/disabled
palette and native list data/interaction. Native allocation/set failure leaves
the previous drawing state; removal releases the copy without changing tabs.
Invalid or unrepresentable requests fail before changing native state.

There is no native public tab-position getter. Explicit late attachment needs
truthful caller-supplied state, just as EDIT attachment needs its consumed
creation border bit. The generic creation scope does not assume that other
creation callbacks left default tabs. Sample01 includes a fresh demo-owned
two-column tabbed list, explicitly attached with its known initial default,
then configured through ordinary LB_SETTABSTOPS. Wider automatic integration
is not claimed. Tests compare initial state, updates/reset, custom palette,
failed requests and native state after removal against the native oracle.
