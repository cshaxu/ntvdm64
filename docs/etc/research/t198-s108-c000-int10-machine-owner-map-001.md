# M0 T198 S108: C000 INT10 Machine-Owner Map

## Result

`C000:014A` is the saved native INT10 vector target in an unprovisioned
expansion-ROM range, but it is not the next capability to add.  The active
CLI stream profile has an original BOP `5F` initialization path that avoids
that transfer.  S107 did not reach that path after the S106 fast-I/O no-op;
therefore its C000 fault is regression evidence against that no-op disposition,
not an admission for VGA, a ROM image, or a Bochs device.

## Source and Runtime Map

| Fact | Evidence | Owner and disposition |
| --- | --- | --- |
| INT10 vector target | The retained r18 snapshot maps IVT INT `10h` to `C000:014A`; S107 copies the same fault CS:IP and a non-BOP `FF` window. | A machine image would be a `bx-mantle` input. No payload is admitted. |
| Current bytes | `bx_ntvdm_minimal_machine_c::initialize` initializes memory, empty port space, CPU/A20/reset, but supplies no ROM/image bytes; `bx-core/memory/misc_mem.cc` retains expansion-ROM backing. | Existing core fetch mechanics are correct; the missing byte identity is not adapter-owned. |
| Original display decision | `spckbd.asm` branches fullscreen through saved INT10, but `STREAM_IO=2` handles `AH=0E` in guest resident code. `AX=0E43` in the fault is that teletype form. | Display-state selection is host/machine composition and is represented by the existing `bx-vdm` stream profile transaction. |
| Corrected stream evidence | S89/S90 observes canonical `C4 C4 5F` at `8DC8:45A6`; its CS:SI table / DS-relative write transaction publishes state `2`, avoids C000, and reaches next `50:3D`. | No VGA, text sink, ROM, PIC, keyboard or timer capability was added. |
| Regressed observation | S107 accepts `50:42` through S106's no-op, records no BOP `5F`, then reaches `C000:014A`. | This cannot justify a machine expansion before the altered `50:42` continuation is reconciled. |

## Rejected Interpretations

- The `FF` instruction window is not BOP selector `15h`; it is empty
  expansion-ROM backing, as already proven in S78.
- The active profile does not need a synthetic C000 stub or a VGA ROM to
  continue its proven stream branch.
- The generic #UD seam has no ROM, BOP, DOS or display ownership.
- S107 does not prove that `C000:014A` caused the missed BOP `5F`; the
  chronology is the reverse: the missed initialization selects the native
  INT10 branch that exposes the absent image.

## Follow-up

S109 must source-map `handle.asm`'s `FastOrSlow` decision and the exact
`50:42` result contract before any runtime correction.  It must not execute a
guest run or add machine capability.

## Evidence

- `docs/etc/research/t198-s78-minimum-rom-ivt-closure-map-001.md`
- `docs/etc/research/t198-s80-display-mode-vector-chain-audit-001.md`
- `docs/etc/research/t198-s89-s90-native-spckbd-cs-si-closure-001.md`
- `docs/etc/research/t198-s107-fastio-noop-native-observation-001.md`
- `src/bx-mantle/bx_ntvdm_minimal_machine.cc`
- `src/bx-core/memory/misc_mem.cc`
