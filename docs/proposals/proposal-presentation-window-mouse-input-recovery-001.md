# Presentation-window mouse input recovery

## Objective

Recover end-to-end mouse input for the selected text-mode DOS workload
`EDIT.COM` without replacing the existing original OpenNT/SoftPC mouse path.
The product presentation window must translate focused Win32 mouse input into
the same standard console-input records consumed by the selected original
event worker.

## Reached owner path

The selected formal x86 graph already contains the original host event worker
(`nt_event.c`), host mouse bridge (`nt_mouse.c`) and built-in NTVDM INT 33h
driver (`base/keymouse/mouse_io.c`).  Its current app presentation surface
opens `CONIN$` and writes `KEY_EVENT` records for keyboard messages, but does
not write `MOUSE_EVENT` records for `WM_MOUSE*` messages.  The immediate
unavailable dependency is therefore a modern product-window input binding,
not a missing guest driver, BOP interface or SoftPC mouse algorithm.

## Proposed boundary

Recover the existing Console-to-SoftPC contract through the smallest app-owned
binding:

```text
Win32 presentation-window client pixels
  -> bounded text-grid column/row coordinates
  -> MOUSE_EVENT_RECORD written to that window's CONIN$ handle
  -> original nt_event -> nt_mouse -> built-in INT 33h route
```

The candidate may change only `src/app/presentation_window.c` and its
admitted documentation/evidence/build-selection records.  It must retain the
existing `CONIN$` handle and `WriteConsoleInputW` route, use current
session-owned text dimensions rather than an assumed 80x25 grid, preserve
button state across move/down/up records, and use normal Win32 capture only
while a button remains pressed.

## Exclusions

No change may add or load `MOUSE.COM`, invent a mouse BOP, modify guest media,
modify original SoftPC or OpenNT mouse logic, alter graphics-guest coordinate
semantics, add a generic input broker, or claim system-console click support
without its own evidence.  Wheel behavior and graphics presentation input are
separate candidates unless a bounded source-owned contract is demonstrated.

## Required evidence and acceptance

- Source review proving the selected original event, host mouse and INT 33h
  route remains unchanged.
- Focused x86 build and link using the formal product graph.
- Unit- or focused boundary evidence for pixel-to-text-grid clamping and the
  injected record's position, buttons and event flag.
- Owner runtime test of `EDIT.COM` in the presentation window: movement,
  left-button press/release, and a drag that leaves then returns to the client
  surface.
- Deploy only the successfully linked x86 `ntvdm32.exe` to
  `O:\ntvdm64\ntvdm32.exe`; keep runtime observations beneath
  `O:\ntvdm64\logs\`.

## Admission dependency

This is an unnumbered candidate only.  It may receive a numeric task and an
active S brief only after the current active package closes and the owner
explicitly admits it.
