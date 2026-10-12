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
[the component resource ledger](../../../src/win16-style/resources.md).

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
