# M0 T310 S27 P2 — host-video formal assembly

## Recovered contract

The original SoftPC `nt_vga.c`, `nt_ega.c`, `nt_cga.c`, `nt_graph.c`, and
`nt_hosts.c` remain the host-video callers.  No controller source was changed.

- Public `CreatePalette`, `SelectPalette`, and `SetPaletteEntries` now resolve
  through `gdi32.lib` in the formal link graph.
- `InvalidateConsoleDIBits` and `SetConsolePalette` retain their original
  source-facing signatures in `adapter-mvdm-host-out/win32`.  They dispatch a
  typed host-only event to the bound session presenter and return
  `ERROR_CALL_NOT_IMPLEMENTED` when none is bound.
- `SetLastConsoleEventActive` retains its void form; it records the original
  activity direction in the session and notifies an installed presenter when
  present.  It does not fabricate the retired Console Server event protocol.

The session event carries only private host handles and a copied rectangle;
it never carries a guest pointer, a guest mapping ID, controller state, or a
window-management policy.

## Verification

Fresh formal composition root:

`build/M0-T310/S27/formal-host-video-x86-r2`

Its selected Win32/x86 CCPU40 forced-link audit reports zero unresolved entries
matching `InvalidateConsoleDIBits`, `SetConsolePalette`,
`SetLastConsoleEventActive`, `CreatePalette`, `SelectPalette`,
`SetPaletteEntries`, or the five S27 original caller objects.

Focused contract root:

`build/M0-T310/S27/host-video-contract-x86-r1`

The existing console compatibility fixture now verifies both directions:

```text
PASS: console compatibility input and video contracts
```

It proves an unbound session returns the explicit invalid-handle failure; a
bound presenter receives the original rectangle, palette and active-event
orders; and the session records activity without requiring a window.

## Non-claims

This does not create a window, render a DIB, replace a SoftPC controller,
reconstruct CSRSS/conhost, or prove guest-visible graphics.  Those presentation
and whole-machine claims remain S49 integration work.
