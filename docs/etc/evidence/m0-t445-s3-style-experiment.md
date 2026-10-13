# T445 S3 standalone frame experiment

## Scope, inputs and source disposition

Owner admits the isolated src/win16-style static library and independent Win32
GUI fixture; requires probe screenshots and prohibits computer use. Baseline
c12090805. No product links, original mirror/guest diff, release image or
installed configuration change. S1's documented source ladder remains:
NTUSER server dependencies are a stopping boundary, not reusable Win3.1
rendering code. This implementation is independently authored local USER/GDI.
No reference image or historical font becomes a runtime/build resource.

## Implementation and test entrypoints

- src/win16-style/layout.c: one geometry/hit-test owner.
- src/win16-style/win16_style.c: same-thread HWND subclass, palette, caption,
  frame/buttons, local resource teardown and original system-command dispatch.
- tests/app/win16_style_demo.c: standalone GUI; no arguments opens four
  windows; --test <bitmap> creates a private desktop, runs assertions and
  PrintWindow-captures its own HWNDs. It never switches the user's desktop.
- tests/observation/verify-win16-style.ps1: fresh build-owned x86/x64 Ninja
  graphs, MSVC /MT /O2 /W4 /WX /utf-8, static archive and GUI-subsystem EXE.
- tests/observation/compare-win16-style.ps1: explicit fixture-region pixel
  inspection and nearest-neighbour detail sheet; not a generic visual scorer.

The library has no thread, IPC, launch/task policy or product dependency.
Clients retain native application painting, title APIs and system commands.
Initial unsupported forms return ERROR_NOT_SUPPORTED, with no attachment.
The library does not yet implement propagation or the title-prefix feature.

## Actual verification

Background-owned evidence: build/M0-T445/S3/r001--r011, with outer PID/start/
deadline records and per-fixture ownership. r011 passes 849 assertions for
each architecture. Tests cover 96/144/192 layout and missing-button hit regions,
actual HWND client origin/size preservation, repeated installation rejection,
unsupported-menu rejection, title/palette updates, button press/cancel/release,
maximize/minimize/restore, 80 window create/destroy cycles, 120 attachment
cycles and WM_CLOSE destruction. All test output and sources are retained.

r001 fails compilation on a C enum spelling; fixed using the API's DWORD
attribute carrier. r002--r005 expose a flawed whole-process GDI equality test.
r009 x64 also shows that a fixed warm-up count does not isolate library
resources; these process-wide counters alone do not attribute the growth.
The final test retains all
window cycles and separately measures attachment churn on one surviving HWND:
both final runs report GDI 51 before and 51 after. Earlier failures remain
evidence, not silently passing attempts or proof of a product leak.

Physical mouse/keyboard, RDP, monitor migration and actual high-contrast mode
were not exercised. Message-based gesture and pure DPI-layout assertions do
not substitute for them. Menu/MDI/custom-frame integration remains out of scope.
No machine/guest runs apply because no runtime component links this code.

## Pixel comparison findings

Owner asks for finer original-reference comparison after the first screenshot.
The reference is S1 r002 owner-control-panel.png. Region-based measurements:

- Its active-title dominant color is #A4C8F0; the preset now matches it.
- Original control box is an outlined narrow horizontal bar, not the original
  trial's thick solid stripe. Caption is centered; minimize-only has no max slot.
- GDI probe: requested System cell heights 20,22,24,26 all resolve to 20 here;
  the 16-pixel strike is 16 high with 3 leading, weight 700, and Control Panel
  measures 87 pixels wide at base scale. A 24-pixel request is NOT 1.5x VGA.
  The final painter scales the base raster instead of letting GDI choose a
  different strike. Stroke thickness and arrow scale were also reduced.
- Reference contains intermediate colors from resampling. Its unscaled origin
  has not been established. Enlarged crops are evidence of remaining differences,
  not a zero-error pixel-identity claim. Do not reproduce screenshot blur as
  guest behavior or declare the font asset identical.

Latest owned-window captures are r030/x86/capture.png and x64/capture.png,
plus capture.bmp-pressed.png and capture.bmp-canceled.png for both widths.
The pixel comparison uses explicit known capture rectangles, preserving
aspect ratio; it does not assert whole-window similarity between different
applications. Both the first screenshot and improved screenshot were shown
to the owner during work.

### Reference-derived relief correction

Owner rejected the approximate relief. r015's fourfold crops demonstrate
that applying one symmetric bevel to all parts was incorrect. In the supplied
Control Panel image the control-box face is flat; the horizontal bar itself
has a lower/right shadow. The minimize button instead has a narrow highlight
and a broader shadow. The local pixel scan at reference x=0..9,y=100 returns
505052,202020,A0A0A0,C0C0C0,A0A0A0,202020,807D78,FFFBF0,...:
two dark outlines around a grey band, not a white outer bevel. The old probe
at x=520..529,y=100 had 000000,FFFFFF,FFFFFF,C0C0C0,000000,... .

The correction removes that outer white bevel and the control-box bevel,
gives its bar a 13x3 logical-pixel outline and one-pixel offset shadow, and
uses one logical highlight/two shadow pixels for arrow buttons. These are
reference-derived geometric interpretations, not an exact reconstruction of
the screenshot's unknown resampling filter. r016/r017/r018 pass both widths;
r018 passes 856 assertions each, including actual PrintWindow pixel checks
for the grey frame band, flat control-box face and arrow-button layer bounds.
GDI attachment counts remain 51 to 51 in both r018 runs.

r020 adds a held maximize-button capture and a separate canceled capture.
Actual rendered highlight/shadow pixels are asserted in all three states;
capture ownership and absence of an unintended maximize command are checked.
Both x86/x64 pass 920 checks, with attachment counts 51 to 51. These are local
message-driven gestures on owned HWNDs, not physical desktop interaction.

### Dialog-frame correction

The supplied codex-clipboard-c576f52b-032e-42e4-a19b-7829be582fd8.png
Color dialog has a dark outer edge, caption-colored band and white inner edge.
At y=180, x=91 is 070707, x=93..97 is A4C8F0, x=98 is D1E3F7 and
x=99 onward is FFFFFF. The former fixture merely ORed WS_DLGFRAME into
WS_CAPTION, which already contains that bit: it did not establish a distinct
dialog-frame request. The fixture now explicitly uses WS_EX_DLGMODALFRAME;
layout carries that flag and selects a six-logical-pixel frame, while painting
uses caption color and a light inner outline. Ordinary frames remain unchanged.
No title/program-name or missing-button inference is used. r022 passes 944
assertions on each width, including actual dialog-edge pixels and all prior
normal/pressed/canceled states. Client geometry is still preserved. This
dialog-style HWND is not evidence for later modal-creation adapter integration.

The comparison script now saves raw frame scanlines and aligns title crops
near their glyph origins; a differently positioned crop must not be mistaken
for a title-centering defect. Font resampling and all frame variants still
need visual review; no pixel-identity or full appearance acceptance is claimed.

## Delivery and remaining acceptance

### Owner-requested original binary research

Owner requests read-only reverse engineering of installed Win3.1 USER/GDI
rather than further screenshot guesses. O:/win31 is absent; the previously
used O:/w31 tree exists. Its SYSTEM.INI selects VGA.DRV and VGASYS.FON (the
Video 7 descriptive label is not the selected driver). No installed file was
modified. Selected SHA-256 identities:

- USER.EXE: 98036967F8396B387DA431699A9D3BE95EB56FCD0118F529E1FC2F25105EFAC6
- GDI.EXE: 6D28146961AB4733E167B1657289206CE16A856E431F843E4EE3D504836DFB6F
- VGA.DRV: 53312ED013BDA513C74E33659F4EE7D1B4F2B1CF1A06AED672432A8FBC4A820A
- VGASYS.FON: 7BAEFA14EF8EF2355374D0B5061043DD5F4F8C33FF57680366E4A725F91646F8

tests/observation/inspect-win31-frame-resources.ps1 decodes selected NE RT_BITMAP
records read-only, with bounded 4-bit CORE/INFO DIB support. Background r024
returns zero and preserves offsets/dimensions and a 4x nearest-neighbour
research sheet. OEM names are matched against src/mvdm/wow16/inc/windows.h.
REDUCE/ZOOM/RESTORE and their depressed variants are 19x18; CLOSE is 36x18.
The latter contains differently sized bars, but its slicing/use is not yet
traced. The depressed bitmaps do NOT simply swap normal highlight/shadow:
they omit the normal white/right-bottom relief and displace the glyph. This
contradicts the prototype's assumed symmetric pressed inversion and requires
following the actual USER bitmap placement before the next appearance change.
Bitmap extent alone does not determine the full caption-button slot extent.

NE entry-table anchors for subsequent disassembly (not executed-call proof):
USER DefWindowProc ordinal107 = segment1:A7FC, LoadBitmap ordinal175 =
segment12:167E, GetSystemMetrics ordinal179 = segment1:A92D;
GDI FastWindowFrame ordinal400 = segment1:420E; VGA FastBorder ordinal17 =
segment1:2532. No historical bitmap, font or disassembled body is imported
into the runtime/library. Research output remains build-only. The existing
candidate dialog changes are independently measured and tested, not evidence
that the entire original USER drawing algorithm has already been recovered.

### Static call-chain follow-up and pressed-state repair

New authored inspect-win31-ne-segment.ps1 decodes NE segment locations and
relocation chains and invokes local ndisasm in 16-bit mode. Raw far-call
operands are unrelocated; resolve their selector/import sites through the JSON,
not their apparent segment numbers. r025 initially omitted module labels due
to PowerShell integer-key types; r026 corrects that. r027's segment13 offset
wrapped at 64 KiB because UInt16 was shifted without widening; it is INVALID
disassembly, not evidence of guest code. r028 widens file offsets first and
rechecks segment13 at file 124320. Resource inspection now widens too; r024's
seven selected OEM offsets were below 64 KiB and are unaffected. No original
file changed. The disassembler prints some shift-by-one immediates incorrectly
(D1 E0 shown as zero); instruction bytes, not that textual immediate, govern.

Static USER chain: ordinal107 stub at 1:A7FC dispatches to 1:5D4F;
WM_NCPAINT 0x85 reaches 1:612E -> 1:572D -> relocated 13:074E;
13:080D calls 1:9F5C for frame/caption painting. USER 3:0FB8..1015 loads
REDUCE/ZOOM/RESTORE and depressed resources through 3:0881. That helper obtains
BITMAP dimensions and stores width/height. Normal painting at 1:A450..A4A4
uses GDI ordinal34 BitBlt. The tracking routine around 1:65AE selects separate
normal/depressed sources and switches them on pointer inside/outside/release
at 1:661D,6691,66BD,66EC. These are static decoded edges, not a runtime trace.

Decoded original glyph bounds (excluding the left edge): REDUCE x6..12,y7..10;
REDUCED x7..13,y9..12; ZOOM x6..12,y6..9; ZOOMD x7..13,y8..11. The independent
prototype now uses the measured +1,+2 logical displacement and removes its
incorrect depressed lower/right white bevel. No original resource/code body
is copied into the renderer. r030 passes 944 checks on each width with actual
normal/pressed/canceled captures and GDI attachment 51->51. Full original
slot placement, title composition and all restore details remain open research;
this bounded repair does not claim complete USER-algorithm reconstruction.

The runnable experiment stays under build, not assets/release or system32.
S3 remains open for owner visual/stability acceptance; S4 is not admitted.
Final documentation/link/diff checks and unchanged ten-image identity are
recorded in r012 and refreshed in r031. This P delivers a tested prototype, not integrated retro
appearance or pixel-perfect Windows 3.1 compatibility.

### Owner-admitted original button resources

The owner now explicitly permits embedding original Win3.1 resources in the
isolated experiment. This supersedes the earlier no-import decision above,
not the product-integration boundary. Source policy and the active packet
record the narrow admission. Source/rights/extraction details live in
[the component resource ledger](../../../src/ntw16s-dll/resources.md).

Six 19x18 VGA.DRV OEM button images now replace hand-drawn triangles/bevels.
The header encodes their actual decoded pixels losslessly as four palette
roles; original state selection chooses normal/depressed and zoom/restore.
This is direct resource reuse with a small GDI presentation adapter. Whole
16-bit USER execution is not composed because it requires the historical
USER/GDI runtime; no executable instructions or mirror changes are imported.
No fallback hand-drawn version of those six buttons remains. System-menu bar,
font and broader frame layout are still separate outstanding fidelity work.

r032 independently decodes the original file and verifies all 2,052 embedded
pixels. Source hash matches the identity above. r033's old guessed-bevel
assertion correctly failed after switching to original resource geometry;
it is replaced by all-source-pixel-center checks, not a relaxed tolerance.
r034--r038 additionally exposed PrintWindow clipping of the off-screen
maximized border on the private desktop (window rect -11,-11,1931,1019 at
144 DPI). A trial WM_GETMINMAXINFO override did not resolve that observation
and was removed. These are failed diagnostic runs, not passes.

r039 checks all six states through the actual HWND WM_PRINT path without
PrintWindow's visible-region clipping. Ordinary four-window screenshots still
use PrintWindow. Both x86/x64 /MT /O2 /W4 /WX builds pass 5,739 checks,
including actual maximized restore hit/click, cancellation, HWND lifetime and
attachment GDI 51->51. All runs are owned hidden background processes with
logs/deadlines under build/M0-T445/S3. No computer use or physical input is used.
Full-raster correctness does not prove that native maximized positioning leaves
the entire frame visibly reachable; that edge remains to inspect separately.
No production image or original installation file is modified.

Next: finish original CLOSE resource slicing/font composition and resolve
maximized visible-frame placement before claiming visual stability. S3 and
the thread goal remain active, and S4 remains unadmitted.

### Private original-font composition

Read-only VGASYS.FON inspection locates RT_FONT #31 at 1,344 with dfSize 5,441
(the NE resource includes alignment padding). Its version is 0x0200, height16,
ascent13, weight700, proportional ANSI. Original Microsoft copyright bytes are
retained. The native memory-font API accepts both the whole FON and the raw FNT
on this host. No custom rasterizer or global installation is necessary.

The first r041 memory-only probe succeeded; a subsequent expanded probe build
in that directory failed to link because its new USER calls lacked user32.lib.
The old result file is NOT evidence for that expanded build. Fresh r042 adds
the required link dependency and proves a private face alias selects original
glyph data: 95 ASCII widths and 11,264 pixels, zero mismatches. The alias changes
only dfSize/dfFace and appends a name in a temporary memory copy; original source
and embedded FNT bytes remain exact. The default shader/bitmap font data are not
retouched. Public GDI performs text shaping/rendering within its ANSI limits.

r043 composes the embedded resource into the actual style library and performs
the same source identity/glyph tests independently on x86 and x64. Each width
also passes 5,739 HWND/pixel/interaction/lifetime checks; GDI attachment count
remains 51->51. The returned memory font is owned per attachment, removed after
the HFONT, and never enumerated or installed globally. Installation fails if
the required resource cannot load rather than silently using the host font.
The revised test runner accepts an explicit -ReferenceFont for the pinned
read-only original comparison; the demo itself has no external font dependency.

Final r045 explicitly selects ANSI_CHARSET/OUT_RASTER_PRECIS, matching the
original font and focused probe rather than depending on host default charset.
It repeats both-width 5,739 checks and zero-mismatch original glyph results.

This proves the ASCII original-resource boundary, not all extended ANSI/Unicode
fallbacks, original title-slot sizing or physical DPI transitions. System-menu
resource slicing and maximized visible-frame placement remain open. Source and
API references are recorded in the component ledger and
[Microsoft's memory-font contract](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-addfontmemresourceex).

### Original top-level system-menu bitmap

Further static USER decoding resolves CLOSE: initialization 3:0F3C loads OEM
32754 into descriptor 0x600; 3:1114 halves its width (opcode D1 /7, despite the
disassembler's incorrect printed zero shift). Painting 1:A388..A3E9 chooses
source x=0 for ordinary/popup top-levels and x=CXSIZE for child non-popup frames,
then BitBlt copies CXSIZE by CYSIZE from atlas offset0x744. Therefore the 36x18
resource is two 18x18 window-kind images, not pressed/unpressed artwork.

The complete original resource is embedded losslessly; the currently admitted
top-level experiment uses its left half. Child/MDI remains unimplemented and
unadmitted in S3. The hand-drawn system-menu bar is deleted. The same small
palette/DIB path now paints all buttons, accounting for the arrow resources'
included left separator versus CLOSE's separate frame separator.

r047 independently compares all seven resources / 2,700 pixels with VGA.DRV.
r048 passes x86/x64 6,711 actual-window/pixel/interaction checks and the original
font byte/ASCII-width/11,264-pixel proof. Attachment GDI counts stay50->50.
Probe captures are reviewed under r048. The product package and original files
remain unchanged. Next remaining question is maximized visible-frame placement;
resource fidelity does not close that earlier observation or owner acceptance.

### Maximized visible-frame correction

r050 measures a native maximized rectangle extending 11 pixels outside each
work-area edge at 144 DPI. The current private-desktop work area is640x408;
normal geometry65,68,485,295 becomes -11,-11,651,419. This is actual HWND geometry,
not a bitmap-offset guess. r051 proves that a WM_GETMINMAXINFO override can
return the work-area size when directly queried yet does not remove the reached
native maximized inset. That trial fails the exact-rectangle assertion and is
removed, not retained as another policy hook.

The retained fix keeps Windows' maximization geometry. One measure_window
routine intersects a maximized window with its monitor work area and computes
the style layout there. Its translated rectangles serve WM_NCCALCSIZE, paint
and hit testing alike. Normal window geometry is unchanged. This is real
client/frame geometry, not cropping a screenshot or offsetting only pixels.

r052 passes the strict public GetClientRect/ClientToScreen comparison with the
expected work-area-relative client, plus the original button raster. r053 adds
the retained maximized PrintWindow capture. Both widths pass6,734 checks,
including PrintWindow verification of the maximized normal/pressed arrow pixels
and actual restore hit/click; the earlier direct-WM_PRINT workaround is removed.
The embedded-font source/95-glyph/11,264-pixel checks still pass. Attachment GDI
counts are stable52->52. The reviewed maximized image includes the off-screen
native margin as black but the entire style frame/title/buttons lie inside its
visible work-area portion. Physical monitor/RDP interaction remains unobserved;
the fixture does not manipulate the user's desktop or switch desktops.

S3 remains open for owner visual/stability acceptance and final layout review;
this closes the measured maximization clipping defect, not all Win3.1 behavior.

### Window-form gallery and native system-menu interaction

Owner requests all discussed forms in the demo and reports that double-clicking
the system box does not close. The earlier code did have a direct double-click
handler, but first-click processing entered a generic TrackPopupMenu loop rather
than native non-client system-menu tracking. Its mere presence was insufficient
proof of the real gesture. Remove both that loop and the unconditional posted
SC_CLOSE shortcut; delegate these messages through DefSubclassProc to the native
window owner. No timer, click replay, hook or menu-state replica is added.
The public contract is documented by
[Microsoft's non-client double-click message](https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-nclbuttondblclk).
Original USER remains the recorded server stopping boundary; no mirror changes
or copied USER executable code are required for this public-API binding.

The same layout now handles absent captions and zero/thin/sizeable borders;
painting avoids inventing a caption on those windows. The existing 18-pixel OEM
images and embedded font are unchanged. tests/app/win16_style_gallery.c adds a
19-entry selector to the standalone demo: 14 styled top-level form/state samples,
then five explicitly labelled native reference samples for menus, scrollbars,
child windows, MDI and ordinary host framing. The minimized host presentation
is explicitly native too. This does not claim S6 retro menu/MDI/icon support.
Owned popups actually carry their owner HWND. Each sample can be opened and
closed independently; Esc also closes captionless/borderless samples.

r055 builds but cannot create a private desktop under the restricted token
(ERROR_ACCESS_DENIED); it is failed, not runtime evidence. r056 reruns under the
approved diagnostic permission and passes both widths. Final r057 adds all
eight button-bit combinations at 96/144/192 DPI, captionless border combinations,
and the actual selector Open/Close handlers. Both widths pass 6,914 fixture
assertions plus all 19 gallery and 19 selector cases. The menu probe observes
SC_MOUSEMENU on first click, cancels that real loop on WM_ENTERIDLE, checks that
disabled Close rejects a double-click message, then enables Close and observes
exactly one SC_CLOSE and destroyed HWND. These are owned-message tests, not a
claim of physical double-click timing/RDP acceptance. Physical owner testing
still needs to confirm the complete first-click/second-click gesture.

The original font probe remains zero-mismatch across 95 ASCII glyphs and
11,264 pixels. The 80 create/destroy and 120 attach/detach loops retain stable
52->52 attachment GDI counts. Five gallery PrintWindow pages are retained per
width (capture.bmp-gallery-1 through -5.png), alongside existing normal,
pressed/canceled and maximized probes. Captures show alternating active/inactive
styles, not whole-window Win3.1 pixel-equivalence. Each page captures normal
geometry for legibility; actual maximize/minimize/restore is separately asserted.

All runners are owned background processes with 120-second outer and 30-second
fixture deadlines. They never switch desktops or inject user input. No production
ten-image, guest, original source/resource or runtime configuration changes.
S3 remains pending owner visual/gesture acceptance; S4 is not admitted.

### Native caption leakage: hover and activation

Owner reports both static native-button bottoms below the custom title and
native buttons appearing on mouse hover. Owner then explicitly authorizes
computer use of the standalone demo after earlier probes miss the artifact.
This overrides the initial no-computer-use scope only for this demo. Real
desktop observation of r057 and r060 reproduces the residue; later desktop
access becomes black/unavailable, and a physical Escape stops computer use.
No further desktop input is performed after that stop. Owner's r062 manual
test confirms hover fixed but static residue still present.

The original PrintWindow path repaints the frame and is not evidence that
existing displayed pixels are clean. r059 adds existing WindowDC reads, but
only compares hover against the initial bitmap. That baseline can already
contain the defect. r062 passes 6,988 checks per width and catches the missing
non-client cursor handler; r063-control links the unchanged HEAD renderer
against the new tests and fails the WM_SETCURSOR return check. This proves the
new handler contract, not absence of the separately reported static residue.

r064 initially fails an activation-cycle pixel comparison because its private
desktop baseline was inactive; r065 explicitly establishes the same active
presentation and passes 7,025 checks. Its saved raw surface nevertheless shows
native-caption contamination already in the baseline. Therefore r066 adds an
independent oracle: the application's top 12 client rows must be white, not
merely equal to an earlier picture. This fails with 3,304 contaminated pixels.
Actual client rectangle 6,37,414,221 exactly matches the custom layout at
144 DPI; the defect is not incorrect client geometry or insufficient coverage.

r067 changes only the WM_NCACTIVATE forwarding to custom painting/TRUE return:
the same strip becomes zero contaminated pixels, across initial, hover, title,
icon, inactive/active and theme cases. Both widths pass 7,033 checks and the
original font probe (95 ASCII glyphs, 11,264 pixels, zero mismatches). The
recovered source boundary remains the S1 NTUSER-server stopping boundary;
these are minimal project-owned modern presentation fixes, no mirror changes.

The final handler retains native minimized activation, owns non-client cursor
shapes and hover, consumes the known 0xAE/0xAF caption/frame draw requests
without interpreting their private payloads, and paints theme-change updates.
Normal activation is no longer delegated for native drawing even with lp=-1.
Client cursor behavior, WM_ACTIVATE/focus, menu tracking, moving/resizing and
window commands stay with their original owners. No window-height increase,
global theme mutation, new thread, hook or repaint loop is introduced.
Primary API/Chromium comparison provenance is recorded in
[the component ledger](../../../src/ntw16s-dll/resources.md#host-non-client-painting-boundary).

r068/r069 extend the oracle through minimize/restore: the strip stays clean,
but two outer clip pixels at (1,1) and (418,1) become black on native restore.
They are outside the caption/buttons/client. The final test records these
outer-frame changes and compares the caption/button/client interior on restore;
all non-geometry steps still require full sampled-pixel equality. It never
allows non-white client-strip pixels. This is a stated geometry distinction,
not a relaxed native-button check.

Final r070 passes x86/x64 7,046 checks each plus the 19 gallery/selector cases,
original-font identity and 95-glyph/11,264-pixel zero-mismatch proof. All nine
client-strip samples have zero contaminated pixels, resize cursor directions
and application cursor delegation pass, and minimized activation reaches the
native application procedure. Attachment GDI counts remain stable. Builds/tests
are owned background runs with 120-second outer/30-second fixture deadlines.
Raw surface captures are capture.bmp-nc-0 through -8.bmp (and PNG) per width;
the before artifact is r066/x86/capture.bmp-nc-0.bmp.

Owner subsequently confirms r070 fixes the static-residue problem too. r071
passes documentation governance, relative links, diff checks and all ten
assets/deployed hashes against the unchanged release manifest. Separate final
review confirms no frame metrics, client geometry, original resources, mirrors,
product images or runtime configuration changed. S3 stays open: owner next
requests raised/pressed system-box correction and a complete original-style
comparison of implemented frame forms. This is not S4 integration admission.

### System-box selection and straight sizing-band divisions

Question: does the normal system box need arrow-like relief, and which original
frame details are absent? Owner's paired original screenshots bb43a2ea
(selected) and fdcd9396 (normal) resolve the first issue. Read-only pixel samples
in the box interior show dominant face C0C4C8 changing to 84888C. The bar's
black/white roles reverse too. Do not manufacture a raised outer bevel or use
the child half of CLOSE as a pressed bitmap. These are resampled screenshots,
not a claim of exact original VGA RGB capture.

Implementation keeps original resources and native menu tracking. It observes
the system WM_INITMENUPOPUP notification, selects black/white and face/shadow
role pairs, and restores normal drawing on WM_EXITMENULOOP. Application
notifications remain forwarded, with existing detach/reentrancy protection.
The formerly unreachable HTSYSMENU comparison against min/max capture state is
replaced by the actual independent menu state. No timer, hook or replacement
menu loop is added. Source policy and private-resource scope remain unchanged.

Read-only original USER disassembly retained at r026, segment 1:A06B..A1DE,
contains eight PatBlt strokes segmenting the thick sizing frame. The local
renderer had only two outlines. It now adds those straight corner/edge marks
using its current DPI-scaled metrics, excluding fixed/dialog frames. No bevel,
caption-height adjustment or client-geometry change is introduced.

Verification: r073 deliberately required menu-idle entry; this failed on the
non-input private desktop, although native SC_MOUSEMENU was dispatched. The
earlier dispatch-only assertion was insufficient to claim actual popup tracking.
r074 separates the notification/rendering contract from that physical boundary:
8,729 checks per width. r075 adds selected/restored probe images and passes
11,485 checks per width, all 19 gallery/selector cases and the pinned original
font checks. Existing white-client-strip/native-caption regression tests remain
unchanged. Menu pixels are checked while selected, after repaint and after
exit, and a non-system notification must leave the normal image intact.
Eight sizing marks and their absence on a fixed frame are pixel-checked.

r075/x64/capture.bmp-system-selected.png and -system-restored.png are owned
rendering probes, not screenshots of an open physical popup. Native mouse
tracking and double-click behavior remain forwarded; this iteration does not
claim a new physical menu acceptance. Original normal/selected comparison and
straight-frame appearance are for owner review in the independently launched
r075/x64/win16-style-demo.exe. Builds/tests ran in owned background processes
with 120-second outer/30-second fixture deadlines; outputs stay under build/.

Audit disposition: OEM resource pixels and font are verified; accepted native
caption guards are retained; selection and missing frame marks are repaired.
Original USER 1:A50D..A5DE measures/positions/clips title text, whereas the local
renderer still uses DT_END_ELLIPSIS. That remains a concrete next audit/repair,
along with corner hit geometry (currently only border-square corner hits).
Do not label this partial delivery as completion of the full divergence sweep.
Menu/MDI/child/scrollbar restyling remains explicitly later scope.

Owner then questions a one-pixel stroke offset and requires original-code
inspection, not adjusting the picture by eye. The completed metric chain is
recorded in src/win16-style/resources.md: 18x18 system artwork plus one divider,
19-wide arrows, full frame F, left stroke F+18 and right stroke right-F-19.
r075's inherited 20-wide slot is therefore not the final geometric contract.
r077 corrects horizontal slots, and r078 additionally removes extra vertical
artwork padding: CYCAPTION=20 includes boundaries, leaving 18 content rows and
one separator inside the frame. Frame height changes here are source-derived
fidelity work, not an attempted workaround for the already-fixed leakage bug.

r078 is the current independently runnable candidate: both widths pass 11,495
checks, 19 gallery/selector cases and original font comparison (zero of 11,264
pixels differ). The stroke tests now also compare their x coordinates with the
actual system/rightmost button separators and y coordinates with caption bottom.
Install/remove client geometry and all nine native-caption client-strip checks
remain passing. Reviewed capture.png and selected/restored images show the
flat band, aligned strokes and distinct system-box states. Physical menu
acceptance and the full divergence audit remain open. No guest/mirror or
ten-product-image change is involved.

### All eight sizing strokes: scale-width correction

The owner spots another apparent one-pixel offset above the maximum button.
Reading r078's saved WindowDC pixels confirms x=385..386 is the OEM's black
separator, whereas the top sizing mark covers only x=385. Same start coordinate
was insufficient: the old test did not check full thickness. Original USER uses
the same border unit throughout all eight PatBlt strokes; local floor(dpi/96)
did not match the stretched original raster.

r080 adds a negative oracle against the unchanged renderer: actual OEM black
run length must equal the frame unit. It fails at that assertion. New checks
then examine every pixel in all eight stroke rectangles and grey neighbours on
both sides, rather than sampling a single point. The one-pixel unit is now
rounded with MulDiv, like the artwork dimensions. A preliminary ceil candidate
passed at 144 DPI (r081) but failed the expanded actual-raster oracle at 99 DPI
(r082); it is not retained. No per-button shifts, DPI special cases or new state.

r083 passes x86/x64 13,885 checks each, including all eight marks at the owned
144-DPI HWND, and the actual 19-column GDI COLORONCOLOR separator at every
integer DPI 96..768 (673 scales). The latter is an offscreen raster test, not
673 physical monitors/windows. All existing lifecycle, client preservation,
font, selected/restored system-box, gallery and native-caption regression tests
remain. Source/font resource data is unchanged. Reviewed r083/x64/capture.bmp-nc-0.png
shows aligned two-column stroke/button edges. Raw SIZING log records all eight
rectangles; they include top-right [385,2,387,4) and its bottom counterpart.
Final runnable candidate is r083/x64/win16-style-demo.exe. Full original-style
audit and physical popup acceptance remain open; no product integration claim.
# Owned-dialog menu follow-up (r086/r087, not fixed)

Owner reports gallery 12's system box does not open its menu. The new
menu_tests matrix covers ordinary, dialog-frame and owned popup windows,
including custom hit testing, native SC_MOUSEMENU dispatch, disabled close
and double-click close. r086/r087 background x86/x64 runs pass those bounded
checks; this is not menu-popup acceptance.

r087's GetMenuBarInfo(OBJID_SYSMENU) observation distinguishes the cases:
ordinary window has native screen bounds 36,41,69,74; both dialog-frame
variants return an empty 0,0,0,0 rectangle despite WS_SYSMENU. Their custom
button centre is 45,49 and hits HTSYSMENU. All three deliver one SC_MOUSEMENU;
all three private-desktop runs observe zero real WM_INITMENUPOPUP messages.
Thus message dispatch is intact, and the modern dialog's missing native
system-box geometry is a concrete mismatch with the custom frame. The exact
menu-loop rejection still requires a real popup observation; no repair or
physical-interaction pass is claimed. Do not weaken the popup requirement to
the command count. Existing explicit notification tests prove painting only.

## Input-desktop menu evidence and candidate correction

r088's unactivated windows also produced no popups. r089/r090 explicitly
activate only their own windows. r090 proves Alt+Space's SC_KEYMENU entry
opens all three real menus, but WM_INITMENUPOPUP's system-menu handle does
not equal GetSystemMenu(FALSE). The library's extra handle comparison was
therefore wrong; HIWORD(lParam) supplies the system-menu classification.

The candidate removes that comparison. A custom system button with a
successfully queried but empty native icon rectangle uses SC_KEYMENU/space;
ordinary native icon tracking and double-click handling remain delegated.
r091 x86 input-desktop probe observes one real click popup for both dialog
variants, verifies selected pixels in actual WM_ENTERIDLE, cancellation,
disabled-close and double-click-close. No global input injection is used.
r091 private-desktop run times out because this menu entry waits for input;
it is a failed fixture run, not a passing regression. Private-desktop tests
now explicitly intercept that command after counting it; --menu-test is the
separate real-popup contract and requires the input desktop. No claim is
made about a physical second click while a native menu is already tracking.

r092 completes x86/x64 private-desktop regression: 25,862 checks each.
The separate --menu-test then passes on both widths with foreground/active
identity confirmed and one actual click popup per dialog variant. These
are candidate build results, not whole-S closure or physical-click acceptance.

r093 adds all sixteen sides of the eight corner-hit boundaries at original
96-DPI geometry, with and without a caption. Both widths pass 25,896 checks;
separate input-desktop probes again observe both dialog click popups and
keyboard popups. The menu probe now requires an explicit capture pathname:
`win16-style-demo.exe --menu-test <absolute-build-path>/menu.bmp`.
An r089 failed pixel assertion had written `(null)-failure.bmp` at the repo
root; that exact file was moved to r089/menu-failure.bmp, and the required
pathname plus build working directory removes that accidental output route.
Title appearance remains awaiting the owner's reference-version clarification.

The same tested source also replaces long-title ellipses with measured,
left-clipped text, following original USER 1:A526--A5DE; it does not change
the embedded font or its scaling. A long-title independent pixel oracle
exercises this in r093. This clipping correction is not the requested font
appearance rollback, which remains explicitly pending. Final review separates
that pending visual choice from menu correctness and derived corner geometry.

## Title scaling candidate r095

Owner permits a typography exception and requests the earlier appearance.
Comparing r070 and r093 confirms unchanged private font bytes but different
20-row versus 18-row text canvases. At 144 DPI, nearest-neighbour vertical
replication changes despite the same nominal font height. r095 restores the
20-row canvas and scales it to 30 physical rows, centred and clipped inside
the corrected 27-row caption. Button/frame geometry stays unchanged. This
is an explicit typography exception, not original 18-row caption fidelity.
Both widths pass 25,896 checks, including the updated independent overflow
oracle and unchanged original-font pixels. Compared with r070's title-only
black-pixel region x=70..319/y=7..29 at a -1-row offset, mismatches fall from
476 (r093) to 299 (r095). This is not exact restoration: corrected button
widths also change the horizontal raster phase. Owner appearance acceptance
and the requested earlier-version clarification remain open.

## Rejected menu-loop double-click probe

r096/r097 attempted to send WM_NCLBUTTONDBLCLK synchronously from menu
WM_ENTERIDLE and require immediate HWND destruction. r096 fails. r097's
unstyled native window control receives the same synthetic message, records
one SC_CLOSE, and also remains alive. Thus this artificial reentrant message
sequence is not evidence of a style defect or a faithful physical double click.
The new assertion/instrumentation is removed; its failed logs remain under
those build roots. No product workaround is added from that result. Physical
second-click tracking stays unverified; the previous popup and separate
double-click/disabled-close checks retain their narrower meaning.

## Host font and release-triggered dialog menu

Owner explicitly selects host System font and reports gallery 12's menu
vanishes on button release. Remove system_font.h and the private memory-font
registration/lifetime, retaining the 16-pixel host System request and existing
title raster sizing. Historical original-font probe remains a standalone
optional research tool, not a renderer font source; it no longer compares an
embedded header. No original font is linked into the demo.

The prior candidate invoked keyboard-menu tracking during mouse-down. The
repair reuses existing pressed/hot/capture state: display selected system box
while pressed, cancel on capture loss, and post SC_KEYMENU/space only after
release inside the button. No timer or global mouse hook is added. r099 passes
25,906 checks per width and actual dialog popup probes after the explicit
release sequence. r100 adds selected-pixel and cancel-without-opening negatives.
These probes do not synthesize physical input; owner must confirm the menu
remains open after an actual click and the physical second-click behavior.

## Blank-menu isolation and physical double-click regression

Owner confirms release persistence but reports blank initial items, hover-only
text and failure to close on double-click. Actual computer-use interaction of
r103 gallery 12 reproduces both: single click opens a mostly white menu with
Restore alone, and a physical double click leaves the dialog alive. The owner
stopped computer use with Escape; no further computer-use input was attempted.
Owner subsequently reports both system and File menus in native gallery 15
affected. This is not evidence of a styled-only defect.

r102 adds first-idle and 500-ms popup WindowDC snapshots. r103's native control
reveals that occluded WindowDC captures can contain other-window pixels. These
captures alone cannot establish on-screen drawing. The retained probe now
checks WindowFromPoint on a popup grid and skips unavailable captures; it also
removes the GetSystemMenu call from WM_INITMENUPOPUP logging to avoid creating
a private menu copy during observation. No PrintWindow, invalidation, hover or
global input injection is used by the snapshot probes.

`tests/app/native_menu_probe.c` is an independent negative control. Its link
contains no win16-style object/archive, no subclass or custom frame procedure.
It uses a native window, default procedure and three ordinary File-menu items.
The bounded `--probe <absolute-build-bitmap> [mode]` entry opens only its own
menu, waits 500 ms, reads visible pixels and exits. Exit 5 means unavailable
capture, not a passing menu. No arguments opens a manual native comparison.
Modes are default system keyboard entry, `--system-dpi`, `--popup`,
`--popup-no-animation`, `--file`, `--file-no-animation` and `--file-keyboard`.
Only the named popup invocation receives TPM_NOANIMATION; no system setting
is changed. Own-process module inventories showed only Windows modules beyond
the probe image; this is not a survey of the host or proof of an OS internals bug.

Observed runs, all below build/M0-T445/S3:

- r104 native entry failed to open before explicit own-window activation;
  exit 5 is retained as unavailable, not a visual result.
- r105 x64 native keyboard entry captures a visible system menu with only
  Restore drawn. This reproduces independently of the entire style library.
- r106 x86 and x64 default system TrackPopupMenuEx both leave the text region
  blank; the same executable/menu with TPM_NOANIMATION draws all items.
  Region x=45..179/y=40..214 has 0 versus 500 pixels with all RGB channels
  below 100 on each width. Both menus pass the visibility guard.
- r106 system-DPI entry remains blank, so PMv2 alone is not the cause.
- r107 ordinary File popup draws all three entries with and without animation.
- r108 same-source v6 Common Controls manifest control still has blank system
  items. Thus adding that manifest does not repair the reproduced issue.
- r109 real File menu-bar keyboard entry (SC_KEYMENU/f) draws all three items.
  This does not reproduce the owner's mouse/menu-bar failure and does not
  invalidate it. r109 default system popup is blank except the highlighted
  Close item; its following no-animation attempt returns unavailable (exit 5).
  The earlier positive no-animation observations remain independently retained.
- r109 both-width existing style tests pass 27,220 checks. These remain
  geometry/drawing/message/lifetime tests, not physical double-click acceptance.

The strongest conclusion is a native system-menu animation-path failure on
this host, independent of our frame code. It does not justify global animation
changes, a forced-repaint timer, or claiming the whole menu problem solved.
The ordinary menu-bar mouse case still needs an equivalent observation.

Double-click is a separate known regression. OpenNT research-only
windows/core/ntuser/kernel/syscmd.c maps HTSYSMENU double-click to SC_CLOSE with
disabled-item checks; mnloop.c routes double-clicks inside its modal menu loop
through its own menu hit geometry. Our modern dialog has an empty native icon
rectangle, and keyboard fallback does not establish equivalent mouse tracking.
The earlier direct-message tests bypassed this loop and cannot certify it.
No original source is imported and no speculative runtime workaround is added
in this diagnosis. Correct menu-loop integration and physical close/disabled-
close/cancel verification remain required before S3 acceptance.

Read-only OpenNT source identities (SHA-256): syscmd.c
`8A76AEF2359465A34957AAC4754FCFD218E2AED37FBD31476C86021785E65E9B`;
mnloop.c `7B21FC770E85031CE8AC380AA4F506BC35ABDEBE54E4200D244897E00359E153`.
r110 verifies the r109 C/header/build-script identities, documentation/link
gates and unchanged ten-image assets/deployed hashes. This delivery contains
diagnostics and evidence only, not a menu repair or S3 closure. Concurrent
Queue/WOW-residue proposal changes are unrelated and excluded from this commit.

## Dialog double-click queue boundary (r113--r120)

Owner reauthorizes computer use only on the standalone demo, then directs
probes/private desktops first and actual input only for gaps they cannot cover.
No production component, guest, mirror or global host setting changes here.

The r114 actual-input trace confirms the failure; its private-desktop test
attempt separately fails with CreateDesktop access denied and is not a pass.
r115 repeats the build/tests with the required execution permissions, passing
27,220 checks per width. The trace entry records only the demo GUI thread;
no cross-process or global mouse hook is used.

r116's queued native-menu-loop probe exposes what direct SendMessage tests
missed: NC double-click closes a normal window but leaves both dialog-frame
forms alive. The first candidate used WH_MSGFILTER. It passes the corresponding
queued-message cases in r117, but actual input still fails, so it is rejected
as the final mechanism. The physical queue trace at
build/M0-T445/S3/r117/physical-queue.txt records:

- first WM_NCLBUTTONDOWN, HTSYSMENU, screen point (224,214);
- capture release WM_LBUTTONUP followed by SC_KEYMENU/space;
- second WM_LBUTTONDBLCLK with the same **screen** coordinates, not client
  coordinates, seen in queue peeks/removal but not WH_MSGFILTER;
- menu exit without SC_CLOSE.

Read-only OpenNT mnloop.c's xxxMNLoop has a pre-filter double-click branch:
native FindNCHit can classify the point as caption and consume it before
_CallMsgFilter. menu.c's xxxMNFindWindowFromPoint rejects a system-menu button
when _HasCaptionIcon is false. These are source explanations of the unavailable
native geometry, not imported executable code or proof of identical modern
implementation. The modern actual trace independently proves early consumption.
Additional source SHA-256, windows/core/ntuser/kernel/menu.c:
`300D2D9645C729F8E9D7F4A91D009CCB766640FFB431BF1FD73465473B0A8FA6`.

The final component repair scopes WH_GETMESSAGE to this thread only while an
empty-native-icon dialog runs SC_KEYMENU/space. On removal of a double click
for this HWND inside its custom system-button rectangle, consume that message,
end the menu, unhook/restore the previous thread binding, and post the native
WM_NCLBUTTONDBLCLK outside the loop. Native disabled-Close and application
handling remain downstream. The existing reference guard protects detach or
destruction during modal reentrancy; no timer, global input hook, repaint hack,
guest or mirror change is introduced.

r119 x86 actual computer-use double click closes gallery 12; a second fresh
window-tree read confirms the owned popup disappeared. r119's initial queued
oracle had an incorrect expectation for a normal window's client-form message
after correcting it to screen coordinates; the native control closes correctly.
r120 fixes that expectation without a runtime-source change. Both widths pass
27,220 ordinary checks and all 18 queued-menu cases (three frame forms, two
message forms, enabled Close / disabled Close / outside-button). Tests require
actual menu-loop entry, exact close-command count and HWND liveness. Queued
cases are not represented as physical input, and the actual-input check was
x86 gallery 12 only. r118/r119 remain intermediate evidence, not final gates.

Menu first-draw blankness is **not** repaired by this change. Independent native
animation controls and the owner's ordinary File-menu report remain open.
S3 is not closed and production integration is not admitted by these results.

## Native menu delayed-drawing controls (r122)

Question: is the blank popup merely captured before its animation finishes?
The independent native probe accepts an optional bounded observation delay and
logs read-only menu animation/fade/show-delay settings and its popup styles.
It still does not link the style library, invalidate/repaint, hover, inject
input or change host settings. Each process owns only its own window, has a
12-second deadline, captures only a visibility-checked popup, and exits.

Both x86/x64 report animation enabled, fade enabled, show delay 400 ms.
System popups at 3,000 ms remain blank. The x86 500-ms control is also blank.
The same system menu passed to TrackPopupMenuEx with TPM_NOANIMATION displays
all items at 3,000 ms. Both widths now reproduce the owner's File-menu issue:
SC_KEYMENU/f shows the highlighted first item only, with the rest blank.
The x64 File submenu with TPM_NOANIMATION displays all three items.
Every retained capture reports visible=1, and all processes exit 0.

Quantification uses dark pixels (each RGB channel below 100) in popup region
x=45..179, y=40..min(213,height-1): system animated 0 versus no-animation 500
on each width; x64 File animated 0 versus no-animation 360. PNG conversions
of the original BMPs retain those pixels under build/M0-T445/S3/r122.
This supersedes the earlier ordinary File-menu non-reproduction; it does not
invalidate the earlier normally drawn observations. Drawing is intermittent
across those historical runs, not an application-specific guarantee.

This rules out insufficient 500-ms observation time and proves an independent
native animation-path failure on this host, but not its internal OS/driver
cause. TPM_NOANIMATION is a documented per-popup flag (Microsoft Learn,
TrackPopupMenu); SC_KEYMENU does not accept that flag. Directly substituting
TrackPopupMenuEx(GetSystemMenu()) is not an equivalent complete system-menu
contract: the captured normal-window control even leaves Restore enabled.
No such substitution or global animation toggle is delivered as a repair.
The owner rejects advancing retro-menu drawing as a substitute for diagnosis:
first identify the cause while retaining native menus. S3's scope is unchanged.

r122 both-width independent builds/private-desktop regressions pass 27,220
checks each. This delivery changes diagnostic code only; the runtime remains
the already tested dialog double-click repair. Menu blankness remains open.

### Paint-path investigation, not a root-cause claim (r123--r127)

Read-only thread-local message traces on the independent native probe show
different paths: the blank animated popup receives WM_PRINT with flags 0x0e
(client, nonclient, erase background) while initially hidden; the normal
nonanimated popup receives NC/background painting followed by menu/item
drawing notifications. The first-item-only File control has one item-drawing
notification after selection, rather than the normal full item sequence.
Waiting three seconds does not produce the missing sequence. The flags do
not omit PRF_CLIENT. These observations localize a path but do not identify
the exact implementation defect or prove a compositor/driver cause.

The attempted WH_CALLWNDPROCRET observation of the animation bitmap does not
produce usable WM_PRINT return samples and must not be cited as proof of an
empty animation bitmap. A separate direct WM_PRINT diagnostic to a probe-owned
off-screen DC is not yet validated: r127 visible attempts terminate early with
exit 5. Menu traces include external keyboard input, so visible probes are
stopped to avoid interfering with the owner. Continue with isolated-desktop
diagnostics before any further actual-desktop attempt. No root-cause claim,
menu replacement, forced repaint or global setting change is authorized by
these results. Concurrent Queue/WOW proposal changes remain unrelated.

### Native print failure localized (r128--r134)

Private-desktop controls now avoid interfering with owner input. These are
off-screen rendering/message evidence, not captures of the physical desktop.
r129 tests both widths, hidden/visible menus, and WM_PRINT/WM_PRINTCLIENT into
a probe-owned DIB. All eight controls render text (region x45..179/y40..213
contains 356--400 dark pixels). Thus neither hidden-window state nor generic
WM_PRINT support explains the failure. Capture occurs once at the selected
visibility transition; the idle timer no longer overwrites that evidence.

r130/r131 metadata finds the native animation DC inaccessible to user-mode GDI
(GetClipBox returns ERROR, last error 6), whereas the owned DIB DC has a full
259x227 clip and is drawable. This alone is NOT proof of a stale/deleted DC:
original OpenNT mndraw.c uses a system-owned animation DC and explicitly changes
ownership around owner-draw callbacks. Original source identity is SHA256
60E9E44BC2F350424A47A414812E4C68A7A146A082BECB40D4E50E2CEB0F714D.
The read-only tracing hook preserves incoming LastError.

r131's Windows-10-supportedOS plus v6-manifest control and r132's process-local
classic-theme control retain the native animation path that omits client
printing. Neither control is a product change or proposed repair. The latter
uses [SetThemeAppProperties](https://learn.microsoft.com/en-us/windows/win32/api/uxtheme/nf-uxtheme-setthemeappproperties)
only inside the disposable process, before creating its windows; no global
setting changes. Those traces alone are not new physical blank-menu captures.

r133 adds an explicitly selected, probe-only native popup procedure wrapper.
It forwards unchanged to the original procedure and records WM_PRINT return.
For system and ordinary File popups the actual animation request returns 0;
the same popup's later print into an owned DIB returns 1 and renders all text.
The observed last error is 87, but LastError is not a documented WM_PRINT
failure-reason contract and is not used to assign the underlying GDI cause.
Unwrapped earlier traces show the same missing client-print path. The wrapper
is instrumentation, not production logic or proof that subclassing repairs it.
The unsuccessful WH_CALLWNDPROCRET sampling has been removed.

Read-only disassembly of the installed win32kfull.sys, matched to Microsoft's
public PDB, localizes the abort: xxxDWPPrint at RVA 0x144EE0 draws the nonclient
frame, then at 0x144FE0 calls GreSaveDC (0x104D38). A zero result branches at
0x144FEA to 0x14521C, returning FALSE before WM_ERASEBKGND/WM_PRINTCLIENT.
The observed frame notification, unchanged flags 0x0e, failed native return,
and missing subsequent client notifications match this branch. This is a
control-flow deduction from static code plus runtime messages/return, not a
claimed live kernel breakpoint. It explains the empty content followed by
individual items appearing on hover. It does NOT yet establish why that
system animation DC cannot save its state (allocation/lifetime/session state
remain unproven), nor justify a blanket Windows-version/driver accusation.
Independent-process reproduction excludes the style library from the current
failing call stack; it does not exclude prior shared-session state corruption.

Binary SHA256: 9895F8F73B09619CF35CC3F92712D1ECD857B62EF59B7736749D832D76332C81.
Public PDB identity: 54AF189C311681B6ABC0F98F58CD324B, age 1; SHA256
D36FED9330B0589FEC1311AA5D5BC12F376464C1EF3FA1EE19E8B0E442E73E4B.
Downloaded from Microsoft's symbol server; PDB/disassembly stay under build,
not imported into product sources or committed as Microsoft binary assets.
r134 rebuilds both widths and passes 27,220 existing checks each. Its eight
private-desktop system/File x86/x64 animation/no-animation controls pass the
diagnostic assertions: four animation requests return 0, all eight direct
owned-DIB prints return 1. These are expected-failure diagnosis checks, not
menu acceptance passes. Native-menu
failure is localized, not repaired; S3 remains open and no production image
or style-library implementation changes in this diagnostic delivery.

### Shared-state isolation limit (r135/r136)

Further static inspection resolves the saved-DC clone ownership operation to
win32kbase!GreSetDCOwnerExFastOpt (RVA 0x1A1700). GrepSaveDC contains multiple
failure paths, including allocation and ownership transfer. Static inspection
does not prove which executes, so neither resource exhaustion nor ownership
failure is established. The base binary was read-only: SHA256
BFB8C3929EA7D1645F07F06FC1666A2603CBB5385E35899A090DA8A8BABDAA42;
Microsoft public PDB GUID A74A7E6053665B0DE1E691B07865180A, age 1.

r135 rechecks the private-desktop native popup and gets the same failed native
print followed by successful owned-DIB print. A disposable probe then tries
CreateWindowStationW followed by SetProcessWindowStation and CreateDesktopW,
without switching the input desktop. r136 separates the errors: creation
itself is denied with ERROR_ACCESS_DENIED (5), before binding. No new station
or session-isolated comparison was obtained. This is an unavailable control,
not a test pass or evidence of a menu defect. The unverified station option
was removed; tracked probe source returns exactly to the r134-tested version.
Retained logs/binaries are under build/M0-T445/S3/r135 and r136. S3 remains
open; no change to the host configuration, menu appearance or runtime library.

### Owner-directed S3 closure and resequencing

On 2026-10-12 the owner explicitly closes S3 and defers the native-menu
fresh-session comparison to T445 final acceptance (S7). This is an accepted
limitation, not a repair or passing menu first-paint result. S4 now owns
standalone retro menus and MDI; propagation moves to S5 and integration to S6.

r139 compiles seven historical library revisions with one x64 diagnostic
harness. Keyboard system-menu animation print fails in all seven, including
initial 1a5d14b6c. The button-message harness opens the old TrackPopupMenu path
in the first two revisions but is unavailable in five later revisions; those
five are not passes. No first-bad commit was established. r140 then reruns the
retained r107 native-menu-probe.exe without rebuilding: SHA256
59AAB16EFCBE6DDA500FA777712A87F81F4E8281944291346A5BD574F677A50C.
Its original File capture showed all three items; the new animated capture is
blank, while --file-no-animation again shows all three. Both runs exit 0 and
report unobscured popup pixels. Exit 0 means captured, not visually passed.
This rules out later code as necessary to reproduce the current failure, but
does not identify who changed shared state. No global setting was modified.

Closure reuses r134's 27,220 checks per width only after C/header/build-script
identity validation; r120 retains queued menu/double-click evidence. The
standalone owner demo is build/M0-T445/S3/r134/x64/win16-style-demo.exe (x86
sibling available). No production component or release image changes here.
Final closure gates are under build/M0-T445/S3/r141. Source review identifies
unchecked SaveDC failure handling as a hardening follow-up for S4; it is not
proven to cause the shared-state failure. S7 must obtain a fresh-session native
control before and after demo execution, or record an explicit owner disposition.
