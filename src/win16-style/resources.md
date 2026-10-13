# Original Win3.1 visual resources

## S4 standard-control resource

scroll_glyphs.h embeds twelve original 17x17 arrow states from pinned VGA.DRV
(eight normal/pressed below, four disabled in the later audit section).
IDs32753/32752/32750/32751 are up/down/left/right; IDs32743/32742/32740/32741
are their pressed forms. Offsets respectively22368/22944/21792/21216 and
22656/23232/22080/21504. Resources are272 bytes with BITMAPCOREHEADER except
DNARROWD (320 bytes, BITMAPINFOHEADER). r078-scroll records the read-only
decode. Verify with -ScrollGlyphs -VerifyHeader src/win16-style/scroll_glyphs.h.
This preserves artwork only: native scrollbar geometry/state remains the
behavior owner, while track/thumb presentation is project-authored and still
requires original visual comparison. Local-experiment rights apply unchanged.
The [original thumb audit](../../docs/etc/evidence/m0-t445-s4-scrollbar-fidelity-audit.md)
confirms a remaining geometry divergence; exact arrow pixels do not close it.

combo_glyph.h contains RT_BITMAP32738 COMBO from the same pinned VGA.DRV:
offset24768, length96, BITMAPINFOHEADER, 7x9 pixels. r070-combo decodes the
original downward arrow and underline. Verify independently with
inspect-win31-frame-resources.ps1 -ComboGlyphs -VerifyHeader
src/win16-style/combo_glyph.h and a fresh build-owned output root.
The same local-experiment rights restriction applies. Combo bevel and sizing
are authored presentation, not imported or pixel-certified USER algorithms.
Read-only OpenNT client/combo.c xxxCBPaint confirms separate button and
edit/static/list ownership, but its later DrawFrameControl artwork is not
used as the Win3.1 pixel oracle. Private PCBOX/server composition remains
excluded by the recorded USER stopping boundary.

control_glyphs.h contains the original CHECKBOXES RT_BITMAP32759 from the
same pinned VGA.DRV below: offset24320, resource length336, BITMAPCOREHEADER,
one-bit 56x39 pixels. The atlas has four 14x13 columns and three rows showing
checkbox, radio and mixed patterns. Exact state selection remains a separately
verified behavior; resource extraction alone is not control implementation.
The Microsoft artwork remains under the owner-admitted local-experiment
restriction, not a new redistribution license. No USER executable code is copied.
Reproduce pixel verification with inspect-win31-frame-resources.ps1
-ControlGlyphs -VerifyHeader src/win16-style/control_glyphs.h and the pinned
read-only VGA.DRV plus a fresh build-owned output root.

## S4 menu glyphs

The owner-requested original-resource approach continues in the standalone
menu experiment: menu_glyphs.h stores VGA.DRV RT_BITMAP 32760 CHECK (14x14,
file offset 24032, BITMAPCOREHEADER) and 32739 MNARROW (7x11, file offset
24896, BITMAPINFOHEADER). Both are one-bit black/white images from the same
pinned VGA.DRV identified below. No original executable code is imported.
White pixels become the menu background and black pixels the current text
colour. DPI adaptation scales the original pixel cells; it is not a claim of
arbitrary-DPI whole-menu equivalence. Rights remain the same local-experiment
restriction as the frame artwork.

Reproduce with inspect-win31-frame-resources.ps1 -MenuGlyphs -Driver pointing
to the pinned original VGA.DRV, -VerifyHeader src/win16-style/menu_glyphs.h,
and a fresh build-owned -OutputRoot. The independent decoder compares all
273 pixels. Native menu frames, metrics and tracking are separate work.

## Frame resources

Owner explicitly admits resource embedding for the T445 S3 isolated experiment.
Original artwork is Microsoft's Windows 3.1 material, not project-authored or
relicensed artwork. Public redistribution rights have not been established;
this local experiment is not a production resource-release approval.

Source: owner-supplied installed `O:/w31/SYSTEM/VGA.DRV`, 73,200 bytes,
SHA-256 `53312ED013BDA513C74E33659F4EE7D1B4F2B1CF1A06AED672432A8FBC4A820A`.
The file remains read-only. The NE RT_BITMAP resources 32749, 32748, 32747,
32746, 32745 and 32744 are REDUCE, ZOOM, RESTORE and their depressed forms.
Resource 32754 CLOSE is a 36x18 pair of 18x18 top-level/child control-box images;
the top-level experiment selects its left half. It is not a normal/pressed pair.
Each is 19 by 18 pixels. `frame_buttons.h` losslessly encodes decoded pixels
as top-down rows of four shade roles, not approximate redraw instructions.
Palette roles map black/white/128-grey/192-grey to the active style palette.
Original default shades reproduce original decoded pixels exactly.

Reproduce and verify using the read-only inspector, in an owned background run:

    tests/observation/inspect-win31-frame-resources.ps1 -Driver <original-VGA.DRV> -OutputRoot <fresh-build-root> -VerifyHeader src/win16-style/frame_buttons.h

It decodes the source NE resources independently and compares all 2,700 pixels
with the embedded seven images. Output includes source identity, resource offsets,
dimensions and original previews. Padding/container headers are not artwork and
are not copied. No binary executable code or original module is linked.

USER's normal and tracking paths select distinct normal/depressed OEM images;
the experiment uses that selection contract. Public GDI scales the selected
raster once into the existing caption button slot. Slot dimensions and frame
layout remain the separately reviewed adaptation, not proof of complete USER
behavior equivalence. Original USER 1:A388..A3E9 chooses CLOSE's source half
from WS_CHILD/WS_POPUP bits, then copies the CXSIZE by CYSIZE region from its
atlas. Resource initialization at 3:0F3C selects CLOSE and 3:1114 halves its
stored width. These are static binary observations, not a traced guest run.
Original font composition is covered below. Remaining layout gaps are not
masked by resource reuse.

## Original title font

Historical provenance only: the owner subsequently selected the host System
font. The embedded header and private memory-font lifecycle have been removed.
The paragraphs below describe the prior composition, not current dependencies.

`system_font.h` contains the byte-exact 5,441-byte RT_FONT #31 at file offset
1,344 in owner-supplied `O:/w31/SYSTEM/VGASYS.FON` (7,280 bytes), SHA-256
`7BAEFA14EF8EF2355374D0B5061043DD5F4F8C33FF57680366E4A725F91646F8`.
Its original Microsoft copyright notice is retained in the FNT resource.
This is a version 0x0200 proportional 16-pixel, 700-weight ANSI font.

The renderer copies that resource into temporary memory, appends the private
face name `Win31 VGA System`, and adjusts only dfSize/dfFace. Private
AddFontMemResourceEx avoids host font installation and same-name ambiguity.
Each attached window owns its font resource handle and releases it after its
HFONT. No FON file is written or loaded at runtime. Original glyphs and widths
are unchanged. Ordinary GDI renders the title; no new font rasterizer exists.

The focused font probe verifies the embedded bytes against the pinned source,
private memory loading, and 95 ASCII glyph widths plus 11,264 pixels against
the original FNT bitmap records. This does not prove every extended ANSI or
Unicode fallback glyph. Format reference: Microsoft's
[Font-File Format developer notes](https://ftp.zx.net.nz/pub/Patches/ftp.microsoft.com/MISC/KB/en-us/65/123.HTM).

## Host non-client painting boundary

This is project-owned modern-host adaptation, not Win3.1 resource data or an
OpenNT mirror change. DWMNCRP_DISABLED does not remove USER/UxTheme's caption
painting. An attached custom frame owns its non-client cursor/hover handling
as well as WM_NCPAINT. Client cursor handling, system-menu tracking and window
commands still belong to the application/native window manager. Minimized
presentation remains native in S3.

Primary comparison inspected on 2026-10-12: Chromium's
[HWND message handler header](https://raw.githubusercontent.com/chromium/chromium/main/ui/views/win/hwnd_message_handler.h)
defines the undocumented compatibility draw messages 0xAE/0xAF, and its
[implementation](https://raw.githubusercontent.com/chromium/chromium/main/ui/views/win/hwnd_message_handler.cc)
documents native-caption artifacts from default cursor processing in OnSetCursor
and absorbs the two draw requests in OnNCUAHDrawCaption/OnNCUAHDrawFrame for
custom-drawn frames. These files identify Chromium Authors/BSD provenance;
no Chromium function body or subsystem is imported. Only the message identities
and host-boundary evidence are used here. Undocumented message behavior is a
compatibility boundary, not a guaranteed Microsoft API; no payload is parsed,
fabricated or forwarded by our handler.

The small authored WM_SETCURSOR mapping follows Microsoft's
[public contract](https://learn.microsoft.com/en-us/windows/win32/menurc/wm-setcursor):
set the frame's arrow/resize cursor and return TRUE; client/error/foreign-window
messages continue downstream. Neither process-wide theme settings nor
SetWindowTheme changes are needed. This avoids changing application client
theming or trying to reconstruct an unknown previous theme association.

Static residue has a separately reproduced cause: forwarding WM_NCACTIVATE
through the host default procedure with lParam=-1 still contaminated the
custom client top in r066. Removing that forwarding alone eliminates all
3,304 non-white pixels in the same 144-DPI fixture (r067). The renderer now
owns normal activation painting and returns TRUE to allow deactivation;
minimized windows continue downstream, as required by Microsoft's
[WM_NCACTIVATE contract](https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-ncactivate).
WM_ACTIVATE, focus, system commands and client painting are not intercepted.
No frame height, global setting or theme association changes are involved.

## Control-box selection and sizing strokes

Owner's paired original Program Manager screenshots (clipboard bb43a2ea,
selected, and fdcd9396, normal, 2026-10-12) resolve the flat-versus-depressed
ambiguity: the system box selects by reversing VGA shade roles, not by choosing
an arrow-style depressed bitmap. Black/white exchange, and face/shadow exchange.
The screenshot's resampled face changes from C0C4C8 to 84888C; this is not a
literal 24-bit RGB complement. The embedded CLOSE pixels remain unchanged.
The application menu owner still receives every notification. The local frame
observes WM_INITMENUPOPUP's documented system-menu flag and WM_EXITMENULOOP to
render and clear its selected state; it does not replace the native menu loop.
See [WM_INITMENUPOPUP](https://learn.microsoft.com/en-us/windows/win32/menurc/wm-initmenupopup).

Original USER.EXE segment 1:A06B..A1DE draws eight short PatBlt strokes dividing
the thick sizing band into four corners and four edges. The experiment now
draws the corresponding straight strokes with its existing scaled frame/button
metrics. Fixed/dialog frames do not acquire these marks. This is a local
rendering adaptation, not imported USER code or exact arbitrary-DPI equivalence.

Metric chain checked after the owner's one-pixel alignment objection:
USER 3:1114 halves CLOSE width to 18; 3:14EA..14F3 publishes 18x18 sizing
metrics; 3:190F..192A binds these to the drawing globals. 3:24BC..24DD computes
inner sizing-band thickness N and full frame F=N+border. For a one-pixel VGA
border, 1:A06B..A0C4 places top strokes at F+18 and right-F-19; the caption
system divider at 1:A39A..A3C3 is also F+18, and the 19-wide arrow starts at
right-F-19. Side strokes sit at F+18 and bottom-F-19. The former 20-wide
button slot was a project approximation, not an original metric.

Likewise, 3:10CE..10E2 derives CYCAPTION=20 including boundaries. At
1:A2D9..A351, the 18-row bitmap begins at F and the bottom separator at F+18.
The experiment now uses 19-wide slots and 18 content rows plus a separator;
it no longer adds top/bottom padding inside OEM artwork. DPI scaling remains
project adaptation; independent tests require sizing strokes to coincide with
the actual button edges after rounding. Original resources remain byte-exact.

Owner identifies a remaining 144-DPI mismatch: r078's OEM separator occupied
columns 385 and 386, but its sizing stroke only column 385. Shared unit was
floor(dpi/96), while bitmap dimensions were rounded. The correction rounds the
one-pixel frame unit with MulDiv as well. r080 preserves the failing old-renderer
test. r083 checks every pixel and adjacent grey bounds of all eight strokes,
and independently samples GDI COLORONCOLOR output at every integer DPI from
96 through 768 (673 values). The rounded unit matches the actual raster at all
these values; no per-corner offset or special-case DPI is used. This verifies
scaled separator consistency, not whole-window historical equivalence at every DPI.

### Multicolumn list contract (S4)

Read-only references under O:/repos.external/OpenNT/windows/core/ntuser/client:
`lboxmult.c` (SHA256
FB3569B4DDCD34E17A3426B97F3E23C6C9371FF8F2E874A61FBD7D16B013F363)
retains Microsoft's 1985--90 notice and identifies a Win3.0 source port.
`LBCalcItemRowsAndColumns` fills rows within a column; the horizontal-scroll
owner advances the top index by rows per column, preserving column alignment.
`lboxctl1.c` (SHA256
47681BF76054CC55DE0533D29A012D344D96EFCE7AAA60199857A166597B4B73)
expresses multicolumn scroll position/page in columns rather than pixels.

No source body is imported. Direct composition and a private-state shim are
excluded by PLBIV/PWND, CheckLock and internal USER window/scroll services,
the already documented USER server stopping boundary. No mirror intrusion is
needed: native LISTBOX retains those operations. The smallest authored
presentation seam queries LB_GETITEMRECT/LB_GETTOPINDEX, clips each item to
its native rectangle, and terminates by the horizontal bound for multicolumn
lists. It does not reconstruct rows-per-column, sort, selection or scrolling.
Tests compare item rectangles/state before attachment, second-column pixels,
inter-column clipping, native horizontal scroll and remove/reinstall state.

### Window nonclient scrollbars (S4)

Read-only OpenNT `windows/core/ntuser/kernel/calcclrc.c`, SHA256
49C51951B20CF984622AEB512F113BAB1E98D5E8A08D066D35FBD3DB99D82B6C,
documents the Win3.1 distinction at lines 137--157: reserve a horizontal bar
only when height is greater than its metric, a vertical bar when width is at
least its metric. Menu layout precedes scrollbar reservation. The experiment
retains that rule and host metrics for compatibility with native USER tracking;
left-side scrollbar placement is a modern-host adaptation.

The original translation unit requires private PWND/TestWF, presence flags,
hung-window and USER client-layout services. Direct reuse or a private-state
shim cannot compose at the established USER server stopping boundary. No
external intrusion or mirror modification is required. The authored frame
layout reserves the public native metric; public GetScrollBarInfo supplies
range-derived thumb geometry and hit rectangles. The existing control
scrollbar painter is shared, not duplicated. Native USER and the application's
WM_HSCROLL/WM_VSCROLL handlers retain all range, command and tracking behavior.
Programmatic rectangle/hit/range tests and private-desktop captures do not
prove uninterrupted real mouse tracking or absence of native intermediate paint.

### Idle mouse and group-box damage (S4)

OpenNT `windows/core/ntuser/client/btnctl.c`, SHA256
579205CFBD7DA6CF97E05235C74B3A221415DD07AE6F73C30649C9BF9E06EF2E,
WM_MOUSEMOVE at 1508 ignores idle motion unless BST_MOUSE is set; captured
motion goes through BM_SETSTATE. The GROUPBOX paint branch at 1245 draws its
frame and clears only the label rectangle, not the whole interior. Its later
DrawEdge appearance is not imported as a Win3.1 visual oracle. Private BUTTON,
PWND, locking and internal drawing services prohibit translation-unit reuse
at the existing USER stopping boundary; native controls continue owning input
and state, with the project subclass responsible for damage/presentation only.

The project trial incorrectly requested synchronous full paint for every
WM_MOUSEMOVE and filled group interiors. The focused regression counts paints
on visible group/button/list windows for 100 idle moves, and independently
paints a group into a sentinel-colour DC to prove interior preservation.
No claim that VGA.DRV owns the BUTTON state machine: driver resources and
USER/GDI interaction/layout contracts are reviewed at their respective owners.

### Disabled scrollbar artwork (S4)

The same owner-supplied VGA.DRV (SHA256
53312ED013BDA513C74E33659F4EE7D1B4F2B1CF1A06AED672432A8FBC4A820A)
contains separate 17x17 disabled-arrow resources, not recoloured normal arrows:
UPARROWI 32737 offset25024, DNARROWI 32736 offset25376,
LFARROWI 32734 offset25728, RGARROWI 32735 offset26080. Each has a 40-byte
bitmap header and occupies 320 resource bytes. The black perimeter and grey/
white embossed arrow are preserved in scroll_glyphs.h. Existing owner-scoped
resource rights/research restrictions apply; no executable driver code is used.

`inspect-win31-frame-resources.ps1 -DisabledScrollGlyphs -VerifyHeader ...`
independently decodes and compares all 1,156 source pixels. Runtime tests sample
each scaled source cell of both disabled arrows on horizontal and vertical
controls, checking selection of the original atlas rather than a monochrome
recolouring. `STATE_SYSTEM_UNAVAILABLE` selects the disabled artwork before
pressed/normal selection; the native scroll owner continues providing state.
