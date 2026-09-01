# M0 T346 S3 P2 — app presentation-window closure

## Decision

The selected P19 cohort is an app-owned public Win32 presentation window. It
does not enable X86GFX, Console Server fullscreen, a kernel VDM route, or a
new video controller. Original SoftPC CGA/EGA/VGA renderers retain their
existing DIB target, mutex acquisition/release, palette call and invalidate
ordering.

## Boundary

- `app/presentation_window.{c,h}` owns window creation, a separate UI thread,
  normal shutdown, typed user cancellation, keyboard-to-`CONIN$` forwarding,
  repaint coalescing and the public `Alt+Enter` window-style transition.
- `session` owns only bounded text/graphics storage and copied palette values.
- `adapter-mvdm-host-out/win32` owns the source mutex binding. It publishes
  the native mutex through the existing session host-resource mapping manager,
  resolves it internally, waits, and copies pixels plus RGB palette values.
  No raw mutex, DIB pointer or `HPALETTE` crosses into app.
- A user `WM_CLOSE` requests `SESSION_CANCELLATION_REQUESTED`. Product
  teardown posts a distinct app shutdown message and waits for the UI thread,
  so normal exit does not manufacture a cancellation.

## Verification

- `build/M0-T346/presentation_window_fixture.exe` exits `0`. It proves
  prepare/open, session activation, text write/snapshot notification, normal
  close, retained active session state and final disposal.
- With `VsDevCmd.bat -arch=x86 -host_arch=x64`, Ninja rebuilt and linked
  `build/M0-T343/S3/record-x86/original-softpc-process.exe` after all P2
  changes. The graph rebuilt the app window and source-facing presentation
  adapter objects and completed the final link.

## Explicit limits

This closes the selected app/window composition boundary only. It does not
prove an actual DOS graphics-mode workload, complete keyboard/device behavior,
native guest continuity, X86GFX behavior or x64 execution. Those remain T346
S4 and their proper machine/guest owners.
