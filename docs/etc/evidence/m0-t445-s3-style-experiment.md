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

Latest owned-window captures are r020/x86/capture.png and x64/capture.png,
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

The comparison script now saves raw frame scanlines and aligns title crops
near their glyph origins; a differently positioned crop must not be mistaken
for a title-centering defect. Font resampling and all frame variants still
need visual review; no pixel-identity or full appearance acceptance is claimed.

## Delivery and remaining acceptance

The runnable experiment stays under build, not assets/release or system32.
S3 remains open for owner visual/stability acceptance; S4 is not admitted.
Final documentation/link/diff checks and unchanged ten-image identity are
recorded in r012 and refreshed in r021. This P delivers a tested prototype, not integrated retro
appearance or pixel-perfect Windows 3.1 compatibility.
