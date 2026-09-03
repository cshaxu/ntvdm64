# M0 T388 S5 source-shaped display arbitration

## Question

Can a single CPU40/SoftPC session retain its original character Console while
entering the existing app presentation surface only at a source-owned graphics
boundary or an explicit Alt+Enter request, without turning app into a DOS
input, BOP or guest-memory owner?

## Inputs

- `mvdm-host/softpc.new/host/src/{nt_fulsc.c,nt_event.c,nt_graph.c}`
- `adapter-mvdm-host-out/win32/source/console_compat.c`
- `session/session.[ch]`
- `app/{entry.c,presentation_window.[ch]}`
- focused x86 Ninja graphs and fixtures listed below.

## Source disposition

1. Original `ConsoleInit` in `nt_fulsc.c` calls `graphicsResize()` only when
   original `sc.ModeType == GRAPHICS`. Normal text continues through the
   original Console registration/text-buffer route.
2. Original `nt_event_loop` owns Console input. NT4 Console Server consumed
   Alt+Enter for its fullscreen controller before that keyboard worker.
3. Modern public Console has neither the private graphics surface selector nor
   NT4 hardware fullscreen. `RegisterConsoleVDM` therefore retains the
   original accepted zero-length hardware-state result. PIF `fullorwin`
   disposition remains S6; no PIF value is read or invented here.

## Implemented contract

- `app` prepares a passive session video sink while the session is ready. It
  creates no window at startup.
- After the original graphics-buffer request has completed, the same-shaped
  Console adapter emits `SESSION_VIDEO_EVENT_GRAPHICS_READY`. The app opens
  its copied-pixel presentation surface only from that event.
- `ReadConsoleInputExW` removes only Alt+Enter key-down from a consuming
  source batch and emits `SESSION_VIDEO_EVENT_DISPLAY_TOGGLE`; ordinary key
  records remain on the unchanged original SoftPC keyboard/IRQ1 route.
- Alt+Enter in the app surface destroys that window and best-effort restores
  public Console focus. It does not inject a key, alter SoftPC screen state,
  fabricate a Console Server mapping or force a text surface for a
  graphics-only guest.
- The UI waits for `ShowWindow`/`UpdateWindow` before reporting ready, and
  uses public system-DPI awareness before its first window. Its client
  rectangle is therefore the selected SoftPC surface (the focused 80x25 text
  fallback is exactly 640x400), not a host DPI-virtualized size.

The sole new adapter divergence is `ADAPTER-WIN32-047`. It transfers no guest
address, guest handle, BOP value, raw DIB pointer, palette handle or mutex;
the pre-existing graphics mutex remains in the session host-resource mapping
manager and app receives snapshots only.

## Verification

- `Generate-T361PresentationFontNinja.mjs` generated
  `build/M0-T388/S5/display-arbitration-x86`; its x86 `verify` target passed:
  - graphics-ready opens a source-sized presentation window;
  - an explicit display-toggle opens, closes, and reopens the same
    session-bound window lifecycle;
  - a real `CONIN$` batch containing Alt+Enter plus `A` yields only `A` to
    the original consumer and exactly one typed display-toggle event.
- The prior focused Console adapter contract was rerun and passed
  `PASS: console compatibility input and video contracts`, including the new
  graphics-ready event from `MvdmPresentationGraphicsBuffer`.
- The formal CPU40/x86 graph relinked
  `build/M0-T386/S3/base-env-x86/original-softpc-process.exe` successfully.
  Its retained historical `LNK4070` `/OUT:ntvdm.exe` versus product-filename
  warning is pre-existing and was not suppressed.

## Limits and handoff

This is a local display-ownership closure, not interactive COMMAND runtime
proof. It does not decide PIF fullscreen (`S6`), prove a prompt/edit/Ctrl+C/
`exit` matrix (`S7`), restore NT4 hardware fullscreen, or change guest media.
