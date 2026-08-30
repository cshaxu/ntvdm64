# M0 T310 S26 P1 — video-controller direct-source boundary

## Question

Can the selected Win32/x86 CCPU40 machine retain original
`softpc.new/base/video` controller sources directly, rather than compile
generated replacement translation units for their callback declarations?

## Selected original controller package

The selected original `base/video/sources` manifest contains nineteen units:
the EGA, VGA, CGA and V7 controller state/mode/port units; controller update
and memory-handler selection; and the EGA read/write, trace and write-mode
providers.  Its controller side consumes S25's original C-VID table owners;
it does not own host-window presentation.

The original relationship is:

- register/mode/port units select the controller state and handler forms;
- `gfx_updt.c` selects marking/update routines and preserves their table
  order;
- `ega_read.c`, `ega_writ.c`, `egawrtm0.c`, and `egwrtm12.c` provide the
  selected EGA memory-handler forms; and
- S27 alone owns later host display/window endpoint binding, while S49 owns
  integrated guest-visible behavior.

## Direct-source disposition

The former build-time generator created five replacement source files and
introduced autonomous `t310_*` wrappers, including fabricated `FALSE` return
values for originally `VOID` marking providers.  It is removed.  The formal
Ninja graph now compiles each of the nineteen selected files directly from
`src/mvdm-host/softpc.new/base/video`.

Two registered declaration-only mirror corrections remain:

- `MVDM-HOST-DIV-135` records the actual `VOID` result contract for the five
  `UPDATE_ALG` marking callbacks.  Each selected caller discards the result;
  the original CGA/EGA providers and the simple dirty-state marker are void.
- `MVDM-HOST-DIV-136` replaces K&R empty declarations at EGA writer-table
  boundaries with exact parameter declarations taken from the same original
  later definitions.  Table order, bodies, guest values and controller
  selection remain unchanged.

Neither correction creates a Bochs route, a MONITOR route, a host-pointer
identity, a mapping token, or a hand-written video controller.  The selected
source remains the original SoftPC controller package.

## Evidence result

A fresh Win32/x86 CCPU40 build root
`build/M0-T310/S26/formal-video-x86-r3` compiled all nineteen direct original
translation units and archived `original-softpc-video.lib`.  Historical
non-width diagnostics remain visible; this record neither suppresses them nor
claims host-display or integrated controller behavior.

## Non-claims

This is source/binding closure for S26 P1.  S27 owns host display bindings;
S49 owns end-to-end controller behavior.  No guest video execution is claimed
by this evidence.
