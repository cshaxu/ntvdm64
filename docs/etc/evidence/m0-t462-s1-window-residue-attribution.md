# M0 T462 S1 — WOW window-residue attribution

## Result

S1 does not admit a product repair.  The owner-reported strip is not reproduced
by the current packaged `WINMINE.EXE` under the checked-in private-desktop
startup contract, so no painter can be attributed safely.  Cosmetic masking,
caption geometry changes and guest changes remain prohibited.

## Reproducible observation

The retained no-input fixture used the T461 S3 staged runtime, a short physical
stage, the existing `console-startup-observer.exe` private desktop contract,
and the existing `wow_mine_gameplay_probe.c` ready-only mode.  Its environment
contains only `SystemRoot`, `WINDIR`, `COMSPEC`, `PATHEXT`, stage `TEMP`/`TMP`
and `PATH=<stage>\\system32;<SystemRoot>\\System32`; inherited build variables
are not guest input.

`build/t462-s1/ready-probe.txt` records one visible, responsive Minesweeper
HWND, unchanged title/native frame, and patterned board/counter pixels without
input or close dispatch.  A temporary read-only full-frame `PrintWindow`
witness was made through that same already-resolved HWND, then removed from
the tracked probe.  The resulting `ready-mine-window.bmp` contains no reported
button-bottom strip.  It is a diagnostic frame, not a claim that `PrintWindow`
is a physical-screen substitute.

Early failed probes are retained only as runner corrections: inheriting build
variables produced the known `COMMAND` `chain` fault; missing `TEMP`/`TMP`, a
hidden startup flag, and absent inherited `CONIN$/CONOUT$` each changed launch
conditions.  None is evidence about the reported pixels.

## Source attribution boundary

Normal WOW window procedure flow remains original callback dispatch:

- `src/mvdm/wow32/wmdisp32.c::W32Win16WndProcEx` calls the guest procedure.
- `WM32NCPaint` maps the region sentinel/16-bit DC only; `WM32NCCalcSize`
  retains the original parameter conversion.
- `src/mvdm/wow32/wmsg16.c` retains the matching `WM_NCPAINT`/`WM_NCCALCSIZE`
  conversion.
- Native `DefWindowProc` is a specified compatibility/failure fallback, not
  evidence that it painted the normal observed frame.
- `mvdm_softpc_wow_page_domain.c` projects host `GetWindowInfo` geometry into
  the WOW view; it is a data projection, not a painter.

Therefore neither the original source audit nor the current valid capture
identifies a responsible painter, DC origin/clip, or client/non-client mismatch.

## S2 handoff

If the exact residual recurs, capture one full native frame and its owning
process/desktop identity before cleanup, then trace only the message/DC chain
for that HWND across `WM_NCCALCSIZE`, `WM_NCPAINT`, activation and expose.
Until then T462 has no source-shaped repair admission; this is a finite
non-reproduction result, not a claim that the historical report was invalid.
