# M0 T361 — SoftPC window font and exact-client presentation closure

T361 closes the original-font and exact-client presentation package.

## Delivered behavior

- The selected original V7 VGA font is the `EGA_HIFONT` table at SAS address
  `0xC3990`: 256 CP437 glyphs times 16 rows, or 4,096 bytes.
- `adapter-mvdm-host-out/softpc` copies that table through original
  `c_sas_loads` into a caller-owned buffer. The app never receives a ROM,
  VGA, guest, or host pointer.
- The app uses the copied glyph rows and source text attributes to paint a
  bitmap DIB. `ANSI_FIXED_FONT` and `TextOutA` are no longer a presentation
  path.
- Windowed text is exactly `columns * 8` by `rows * 16` (the normal `80x25`
  surface is `640x400`). Windowed graphics is exactly the source graphics
  surface. User32 computes only the outer nonclient frame.

## Source and divergence disposition

The provenance ledger is
[M0 T361 S1 original font/client contract](../etc/evidence/m0-t361-s1-original-font-client-contract-001.md).
It follows original `sas.h`, `rom.c`, and `v7_video.c`; the selected V7 ROM
hash is recorded there. `ADAPTER-SOFTPC-042` is the sole new bounded seam:
historical Console Server presentation did not expose a safe app font copy,
so the adapter retains original SAS read semantics and returns a fixed-copy
buffer instead. It neither changes ROM/VGA state nor introduces a host-font
fallback.

## Verification

- Focused x86 Ninja fixture `presentation-font-snapshot-fixture.exe` proves
  the exact SAS address, 4,096-byte size, copied result, and invalid-capacity
  rejection.
- Focused x86 Ninja fixture `presentation-window-contract-fixture.exe` opens
  the app window and proves an exact `640x400` client rectangle, then closes
  and disposes the session.
- The formal x86 `original-softpc-process.exe` graph completed successfully
  (396 actions), including the new adapter and app presentation code.

## Explicit limits and follow-up

This package intentionally does not claim an arbitrary VGA font-selection
lifecycle, X86GFX, Console Server/CSRSS behavior, direct aperture aliasing, or
execution of a DOS graphics program. The next relevant work is queue item 11,
which owns a bounded SoftPC graphics workload and presentation integration.
