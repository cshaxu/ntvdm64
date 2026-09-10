# M0 T310 S27 P1 — host-video source boundary

## Question

Which original SoftPC host-video calls can remain direct public Win32/GDI
calls, and which crossed the retired NT4 Console Server boundary?

## Original selected callers

The selected CCPU40 host package keeps these original source bodies as the
video-binding callers:

- `softpc.new/host/src/nt_vga.c`, `nt_ega.c` and `nt_cga.c` calculate dirty
  rectangles and call `InvalidateConsoleDIBits` after a controller update.
- `nt_graph.c` owns the original graphics function vectors and updates the
  palette through `SetPaletteEntries` and `SetConsolePalette`.
- `nt_hosts.c` creates/selects the original GDI palette and calls
  `SetLastConsoleEventActive` after screen-description initialization.

The controller algorithms, graphics tables and guest video-memory writes stay
with closed S25/S26.  S27 owns only the host endpoint side effects.

## Final boundary dispositions

- `CreatePalette`, `SelectPalette` and `SetPaletteEntries` are public GDI
  APIs.  Their original call names, parameter forms and order can remain
  direct; selected final linking merely needs the public GDI import library.
- `InvalidateConsoleDIBits(HANDLE, PSMALL_RECT)` and
  `SetConsolePalette(HANDLE, HPALETTE, DWORD)` are NT4 Console Server graphics
  operations.  Current public Console APIs do not expose an equivalent DIB
  invalidation or per-console-output palette operation.  They require a
  session-owned host-video presenter capability under the existing
  adapter-win32 boundary; it must preserve the original success/failure
  direction and never reinterpret a guest address as a host identity.
- `SetLastConsoleEventActive` is a retired Console Server event-state
  notification.  No modern public API provides the paired event protocol.  A
  selected non-presenting profile may retain only session-local notification
  state; it cannot fabricate Console Server event routing or claim a visible
  update.

## Consequence for recovery

S27 will not write a replacement VGA/EGA/CGA controller, GUI, or window
manager.  It will first add the smallest typed presenter boundary required by
the original endpoint calls, then bind the existing source bodies to it.
Without an admitted presenter, the graphics invalidation/palette operations
must report their original failure direction rather than silently succeed.
The remaining display presentation and integrated guest-visible output stay
mandatory S49 work.
