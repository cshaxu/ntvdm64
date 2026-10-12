# Original Win3.1 visual resources

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
