# M0 T361 — SoftPC window font and exact-client presentation

## Decision

The app window presents the selected SoftPC text plane with a copied snapshot
of the selected original BIOS/VGA CP437 glyph data, never a Windows stock or
installed font. The 256x16 table is read through the original SoftPC SAS
contract by the presentation adapter; app never aliases ROM or VGA memory.
The current selected default is the 4096-byte `EGA_HIFONT` table. A later
mode/font-load path may update the copied snapshot only through the same
bounded adapter/session boundary.

Windowed mode has an exact client-area contract: text `80x25` is `640x400`;
each graphics mode uses its source-selected pixel width and height.  The outer
nonclient frame is calculated from that contract; app must not add a white
border, fixed 960x720 client surface, padding, or host-font metric.

NXVM's `w32adisp.c` is comparison evidence for a CP437 bitmap/attribute
bitblt frontend only.  It is not a source, runtime, build or acceptance input.

## S plan

### S1 — Original font and window-contract provenance

Map selected SoftPC BIOS/VGA font storage, active font-selection state,
initial CP437 8x16 bytes, current session text-plane dimensions, graphics
dimensions and app window sizing. Record the exact SAS source address,
copy-size contract, source owner and failure behavior. Select the smallest
same-shaped adapter/session seam; no source behavior changes.

### S2 — Font snapshot and session presentation seam

Implement the selected original-font snapshot through the existing
SoftPC/session ownership boundary. Preserve original font-load selection and
render no text when a valid 4096-byte snapshot is unavailable; never fall back
to a host font.

### S3 — Bitmap text renderer and exact client geometry

Replace app `ANSI_FIXED_FONT`/`TextOutA` rendering with CP437 glyph bitblt and
attribute colors.  Size the initial and resized window with the exact client
contract through public User32/GDI calculation, while preserving app-owned
window lifecycle, copied presentation snapshots and input routing.

### S4 — Focused verification and selected integration

Verify original-SAS snapshot identity, CP437 row-bit order, attributes,
no-host-font fallback, 640x400 text client geometry, native graphics
dimensions, close and Alt+Enter lifecycle. Run one bounded CPU40/x86 presentation observation only
after local closure; it may classify a predecessor but cannot select a
trace-derived repair.

## Non-goals

No `X86GFX`, Console Server graphics registration, CSRSS/kernel VDM, Bochs,
guest font replacement, arbitrary font selection, host font fallback, direct
VGA aperture access, second mapping manager, or SoftPC controller rewrite.
