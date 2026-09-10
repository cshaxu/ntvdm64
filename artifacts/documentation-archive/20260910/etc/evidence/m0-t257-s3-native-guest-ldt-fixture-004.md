# M0 T257 S3 — Native Guest GDT/LDT Lifecycle Fixture

## Question

Can native Bochs consume a DOSX-shaped descriptor-table lifecycle directly
from ordinary guest RAM, without a DPMI-aware `bx-mantle` API, a descriptor
cache in `bx-vdm`, or an NT4 process-LDT import?

## Inputs

- The ownership decision in
  [map 003](m0-t257-s3-native-descriptor-lifecycle-ownership-map-003.md).
- Native `BX_CPU_C::LLDT_Ew` and normal segment-load paths in
  `src/bx-core/cpu/protect_ctrl.cc` and the CPU descriptor machinery.
- `tests/bx-mantle/t257_s3_native_guest_ldt_fixture.cc`.

## Procedure

The fixture supplies bare guest bytes only. In real mode it loads a GDTR whose
base is guest physical `0400h`, sets `CR0.PE`, and far-jumps to a 32-bit guest
code descriptor. The protected-mode bytes then:

1. Load selector `18h` and execute `LLDT`, consuming the guest GDT LDT
   descriptor whose table base is `0700h`.
2. Load selector `0Ch` into `DS`, consuming index 1 from that guest LDT.
3. Execute `UD2`.

The selector-blind generic #UD bridge stops only on the final `UD2`. A
descriptor validation/load failure therefore cannot produce the passing
terminal. The fixture contains no BOP bytes, no DPMI selector, no `bx-vdm`
source and no special Bochs configuration.

## Observations

- A fresh formal MSVC x64 `/MT`, CPU5/P-MMX Ninja graph at
  `build/M0-T257-S3/formal-r3/` built all **154/154** targets.
- `t257-s3-native-guest-ldt-fixture.exe` exited `0`.
- The only execution terminal was the deliberately emitted `UD2`; native
  `LLDT` and the following LDT-backed `DS` load completed first.

## Result

Guest descriptor bytes are sufficient for native Bochs GDT/LDT mechanics.
`53:0F` may retain only the fixed-width guest table identity established by
the source seam; `53:00` must not recreate `NtSetLdtEntries`, a host LDT, or a
second descriptor store. This is a mechanical prerequisite proof, not an
active `53:00` service route or DPMI runtime-completion claim.
