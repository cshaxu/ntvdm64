# M0 T346 S3 — App window lifecycle contract

## Selected boundary

S3 adds one app-owned public Win32 presentation window for one active session.
It is not an implementation of the historical Console Server fullscreen
protocol.  The window owns its creation, UI thread, keyboard messages,
repaint scheduling, close request and Alt+Enter transition.  `session` owns
the bounded presentation planes; the host-out adapter alone continues to own
the source-facing writable pointer that original SoftPC code receives.

## Original-source facts

- `nt_cga.c`, `nt_ega.c` and `nt_vga.c` acquire
  `sc.ConsoleBufInfo.hMutex`, update the DIB, release that mutex, and only
  then call `InvalidateConsoleDIBits`.
- `nt_graph.c` owns DIB construction, palette handling and resize order.
  Its original private graphics-buffer allocation was already replaced by the
  bounded binding recorded as `MVDM-HOST-DIV-176`; no S3 source edit is
  needed in any graphics controller.
- `nt_hosts.c` gives the original host event path `sc.InputHandle` from the
  process console (or `CONIN$` if standard input was redirected), and
  `nt_event.c` consumes that handle through `ReadConsoleInputExW` and writes
  returned records through `WriteConsoleInputVDMW`.
- The original Alt-key recovery in `nt_event.c` and fullscreen functions in
  `nt_fulsc.c` are tied to Console Server focus/fullscreen state.  `X86GFX`
  is deliberately unselected and remains so.

## S3 public replacement

1. The original adapter video-event callback posts a repaint request to the
   app window and succeeds when that bounded request is accepted.  It does not
   pass pixel pointers, palettes or handles as the app data ABI.
2. On repaint, the app copies a bounded text/graphics snapshot and presents it
   with public User32/GDI operations.  A fresh invalidation may supersede a
   queued repaint; no per-pixel or controller logic moves into app.
3. Normal keyboard messages become public console input records targeting the
   same current process console input endpoint that the existing original
   SoftPC host path uses.  This preserves the existing `nt_event.c` consumer
   and avoids a second guest keyboard implementation.
4. `WM_SYSKEYDOWN` for Alt+Enter is consumed by app and toggles only the
   app window's bounded windowed/maximized presentation state.  It never calls
   `nt_fulsc.c`, `SetConsoleDisplayMode`, private console APIs or X86GFX.
5. Window creation failure is deterministic: presentation binding is refused
   before the original entry starts, console-only original execution remains
   available, and no session cancellation or guest state mutation occurs.
6. A user close requests controlled session cancellation.  It does not
   terminate the process, alter global standard handles or directly change
   SoftPC controller state.

## Exclusions and later work

This contract does not recreate Console Server focus/menu ownership, hardware
fullscreen, legacy system-menu hooks, mouse regeneration, CSRSS, kernel VDM,
or an arbitrary GUI framework.  S4 alone may claim paired presentation and
selected DOS graphics-workload evidence.
