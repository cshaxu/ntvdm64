# Original Win3.1 visual resources

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
