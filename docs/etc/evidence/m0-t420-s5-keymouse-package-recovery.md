# M0 T420 S5 — SoftPC keymouse package recovery

## Selected original manifest

The complete original `softpc.new/base/keymouse/sources` manifest is
`keyba.c`, `keybd_io.c`, `mouse.c`, `ppi.c`, and `mouse_io.c`.  It owns the
8042 keyboard controller, BIOS keyboard handoff, mouse INT 33h state and
callback setup.  It does not own the public Console event reader (host S12)
or the CCPU executor (S1).

## Recovery ledger

`mouse.c` and `ppi.c` are byte-identical to pinned OpenNT.  This S removes
86 mirror-side diagnostic lines from the other three selected units.  They
were default-off scalar observation calls and did not provide keyboard,
mouse, PIC, BOP, or guest policy.

Three source-first finite differences remain:

| Unit | Boundary | Reason it cannot be blindly restored |
| --- | --- | --- |
| `keyba.c` | XMS declaration and reset event | `xms.h` supplies the original A20 provider declaration; the keyboard reset uses public `cpu_interrupt(CPU_HW_RESET, 0)` rather than directly mutating CPU40's private bitmap. |
| `keybd_io.c` | NTIO vector-table and stream cells | `effective_addr` is a fixed-width guest physical value.  The original cast treated it as a host pointer; the selected worker combines it with original `Start_of_M_area` backing storage. |
| `mouse_io.c` | range endpoint signature and INT 33h callback EIP | The matching host implementation uses `MOUSE_SCALAR *`; CPU40 enters the registered callback's first instruction rather than applying the historical BOP `-2` offset. |

The last callback rule is not speculative: the earlier original-boundary
evidence records EDIT's registered code entry and the bytes immediately
before it; the `-2` position is ordinary guest code, not a BOP.

## Verification

Fresh formal graph: `build/M0-T420/S5/formal-x86-001`.

- Selected `original-softpc-keymouse.lib`: x86 build passed.
- The fresh four-EXE x86 product build passed; the separately invoked
  `Verify-VdmTibStorage.mjs` passed on its new `ntvdm.exe.map`.
- Deployed `O:\winnt` regression passed: direct MEM, repeat MEM, nested
  COMMAND→MEM, `COMMAND /c ver`, and EDIT.
- Console matrix `m0-t420-s5-video-*` passed six geometry cases and five
  short-window EDIT→MEM runs.  Its mouse case recorded source-path stages
  1, 2 and 4, coordinate conversion `19,9` to `152,72`, and button sequence
  zero → left-down → zero before EDIT exits and MEM continues.

## Next owner

S6 owns the selected original `softpc.new/base/bios` package, including its
firmware initialisation and host-facing failure paths.  It may consume this
package's original controller contracts but may not reinstate test observers
inside the keymouse mirror.
