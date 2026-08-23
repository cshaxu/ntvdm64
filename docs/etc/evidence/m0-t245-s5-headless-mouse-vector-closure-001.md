# M0 T245 S5 — Headless mouse-vector first-profile closure

## Question

Can the source-built bootstrap cross its reached `C4 C4 C8` mouse-install
predecessor without enabling a host mouse, a SoftPC mouse device, GUI, IRQ
handler, PIC-mask change, INT71 or video behavior?

## Source And Recovery Decision

OpenNT's guest `dos/v86/doskrnl/bios/spcmse_4.asm:InstSpcMse` constructs the
`mio_table` and invokes `BOP C8`.  SoftPC's
`base/bios/bios.c` maps C8 to `v_host_mouse_install1`, whose NT host body is
`host/src/nt_mouse.c:host_mouse_install1`; that body invokes
`base/keymouse/mouse_io.c:mouse_install1`.

The full historical operation installs mouse state, an EOI hook, INT71,
PIC masks, INT33, optional INT10, hardware reset and host UI.  The immediate
guest continuation only requires the first MONITOR table pair —
`MOUSE_IO_INTERRUPT_OFFSET` and `MOUSE_IO_INTERRUPT_SEGMENT` — published as
the INT 33h vector.  The selected source-derived rehost therefore retains
only that table-read/publication contract.  It is explicitly not a mouse
device implementation.

## Implementation

- `src/bx-vdm/bop/opennt/softpc/mouse_install1_headless.c` follows the
  historical `host_mouse_install1 -> mouse_install1` ordering, with every
  excluded device-owned statement documented at the divergence point.
- `src/bx-vdm/bop/shim/softpc_mouse_vector_shim.{h,c}` supplies per-thread
  copied CS:BX, reads two words through checked RAM, and atomically writes
  the four byte INT 33h IVT entry at physical `0xCC`.
- `src/bx-vdm/bop/softpc_mouse_vector_v2_generic_ud_bridge.{h,c}` accepts
  exactly real-mode `C4 C4 C8`, preserves registers/flags, and resumes at
  `RIP+3`.
- C9 and B8..BF have no route.  The global dispatch remains entirely within
  bx-vdm; bx-core and bx-mantle receive no selector or mouse semantics.

## Verification

The outside-sandbox formal Ninja graph at
`build/M0-T245-S5/formal-r1` built the actual x64 `/MT`, CPU5/P-MMX closure.
`t245-s5-headless-mouse-vector-fixture.exe` passed.  It proves the direct and
global C8 routes copy a seeded four-byte CS:BX table into IVT 33h, return
`RIP+3`, preserve GPR/flags, reject C9 and B8, and reject an out-of-aperture
CS:BX table without publishing a vector.

One finite 1,000,000-tick source-built guest run then accepted C8 at
`8E08:78CB`; its next new selector was `17` at `0211:02D5`, which declined.
OpenNT maps selector 17 to `printer_io`, a distinct LPT/printer host package.
No follow-on implementation was selected from this observation.

## Result And Transfer

S5 closes the declared C8 first-profile vector handoff.  It does not claim
mouse interaction, guest EXEC continuity, or printer support.  C9/B8..BF
remain intentionally unavailable; selector 17 transfers to a future complete
printer/LPT owner package.
