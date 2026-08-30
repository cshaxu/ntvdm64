# M0 T310 S19 P1 — input-controller contract boundary

## Exact selected original workset

The selected `softpc.new/base/keymouse/sources` manifest contributes the five
original controller translation units: `keyba.c`, `keybd_io.c`, `mouse.c`,
`ppi.c`, and `mouse_io.c`.  The formal CCPU40 source manifest also selects
the adjacent original host forms `nt_keycd.c` and `nt_mouse.c`.  The latter
remain host-input bindings, so their public console/UI operations belong to
S20; S19 owns only the controller-side call shapes that reach them.

The static ABI inventory identifies nine direct callable/representation
records in this cluster.  The accompanying ledger records every one, including
the three old-style callback slots, the BIOS and INT 33h table forms, and the
two existing registered mirror divergences.

## Source-first decisions

- The five controller bodies remain selected original source.  None may be
  replaced with a synthetic keyboard, mouse, IRQ, or BOP provider.
- `MVDM-HOST-DIV-058` and `MVDM-HOST-DIV-059` are not automatically accepted
  merely because they once compiled.  S19 must read their declarations,
  definitions, initializers and all selected callers beside the OpenNT source;
  it must retain, shrink, move to an overlay, or remove each one.
- `nt_keycd.c` uses the existing OpenNT-shaped `RtlOemToUnicodeN` facade
  (`ADAPTER-WIN32-027`).  That is a direct public binding decision, not a new
  keyboard algorithm.  Console input acquisition is explicitly S20 work.
- The selected x86 profile treats SoftPC backing pointers as private machine
  state.  No mapping-manager token is appropriate unless a source value
  actually crosses a host/guest or component identity boundary.

## Required S19 closure proof

For every ledger row, S19 must re-read original declaration, definition,
table assignment/initializer and selected callers, then record one source
disposition.  A formal Win32/x86 CCPU40 build and a focused controller
behavior or exact-unavailable result must cover the resulting contract.
Host-console, fullscreen and product input policy are intentionally excluded
until S20/S27; they cannot be faked by a controller callback.
