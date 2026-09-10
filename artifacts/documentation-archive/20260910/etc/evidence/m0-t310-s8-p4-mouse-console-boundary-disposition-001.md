# M0 T310 S8 P4 — mouse console boundary disposition

## Original contract

`nt_mouse.c:MouseAttachMenuItem` calls
`ConsoleMenuControl(hConsoleOutput, IDM_POINTER, IDM_POINTER)`, then uses the
returned menu to expose the mouse command.  The original Console Server client
and server sources prove that this is not merely a menu-handle lookup: it
stores the selected command-ID range on the specific console screen buffer so
that later menu activity becomes an input event for the VDM.

## Modern boundary

The declaration bridge preserves the original `ConsoleMenuControl` signature,
so the original mouse source has no implicit-int or pointer-width call ABI.
Modern public console APIs do not expose the paired per-output-buffer
command-range/event-routing operation.  Returning a `GetSystemMenu` handle
alone would silently omit the original input-routing half of the contract and
is therefore not a valid recovery.

The required future owner is the named session console-command-range broker
under `adapter-mvdm-host-out/monitor`: it must retain the original command
range, bind it to the current one-session output surface, and deliver the
corresponding source-shaped input event.  P4 does not create that broker or
claim mouse-menu runtime closure.

## Remaining mouse diagnostics

- The signed 16-bit mouse-coordinate assignments are fixed guest fields on
  both x86 and x64, not native-width conversions.
- The original unused shift expression in `WindowedGraphicsScale` is retained
  as a visible source-quality warning.  It is outside the x86/x64-only P4
  repair scope.

The worklist records these three dispositions explicitly; no warning has been
suppressed and no original mouse body has been edited.
