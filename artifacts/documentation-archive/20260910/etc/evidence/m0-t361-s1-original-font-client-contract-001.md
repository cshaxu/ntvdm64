# M0 T361 S1 — original font and exact-client contract

## Question

Which selected original SoftPC bytes provide the CP437 8x16 glyphs, and what
exact app client-area contract follows from the original text and graphics
surfaces?

## Inputs

- `src/mvdm-host/softpc.new/base/inc/sas.h`: `EGA_ROM_START=0xC0000` and
  `EGA_HIFONT=0xC3990`.
- `src/mvdm-host/softpc.new/base/system/rom.c`: selected `v7vga.rom` loads
  at `EGA_ROM_START`.
- `src/mvdm-host/softpc.new/base/video/v7_video.c`: the default 16-scan-line
  VGA path calls `load_font(EGA_HIFONT, 256, 0, 0, 16)`.
- Selected immutable firmware:
  `src/mvdm-softpc-firmware/softpc.new/roms/v7vga.rom`, SHA-256
  `970f105cd9e42ee56f07aae695bac89786d3455ab9d4c1ea9a1d1643b1e8f6f0`.
- The existing bounded session text/graphics snapshot and app lifecycle.
- `O:\repos.hobby\nxvm\src\vm\platform\win32\w32adisp.c`, reviewed only
  as comparison evidence for CP437 bitmap presentation and exact client
  sizing. It is not copied, built, linked or used at runtime.

## Procedure

1. Follow original ROM load and original default 8x16 selection.
2. Calculate `0xC3990 - 0xC0000 = 0x3990`; the source-selected table has
   `256 * 16 = 4096` bytes in CP437 character order.
3. Walk the current app text/graphics paths and identify host-font and fixed
   outer-window assumptions.
4. Select a bounded adapter copy through original `c_sas_loads`, not an
   exported asset, direct ROM/VGA alias or Windows-font substitute.

## Observations

- `EGA_HIFONT` is an original SoftPC physical/SAS address. The adapter can
  copy exactly 4096 bytes through `c_sas_loads`; the app receives only the
  copy and can neither retain nor modify a SoftPC pointer.
- The initial CP437 order and 8x16 scan-line order are therefore original
  V7 VGA data, not an independently designed font.
- Text client dimensions are `columns * 8` by `rows * 16`; normal `80x25`
  text is exactly `640x400`. Graphics client dimensions are exactly the
  bounded graphics-surface width and height.
- Outer size must be a public User32 calculation from the chosen client
  rectangle. A normal nonclient frame is permitted; client padding, a white
  border, fixed `960x720`, scaling and a host-font metric are not.

## Interpretation and confidence

High confidence: selected original source names the ROM, address, glyph count
and height. The adapter copy preserves the source-selected font state while
protecting source-owned memory. A later original font-load can be represented
by another copy through this same edge. S1 does not authorize `X86GFX`, direct
VGA aperture access, Console Server/CSRSS, or a host-font fallback.

## Follow-up

S2 implements the bounded `c_sas_loads` snapshot. S3 uses the CP437 bit order
and exact client dimensions for app-owned bitmap presentation. S4 verifies
the source-copy, colors, lifecycle and selected CPU40/x86 integration.
