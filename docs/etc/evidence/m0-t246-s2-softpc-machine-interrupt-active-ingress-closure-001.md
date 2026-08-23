# M0 T246 S2 — SoftPC 02/06 active-ingress closure

## Scope

S1 showed that the historical shared SoftPC `02/06` operation stream and its
typed machine actions already existed, but that the active adapter registry
never selected them.  This packet connects that complete pair to the current
generic `#UD` ingress.  It is not a Bochs change and it does not add DOS or
OpenNT selector vocabulary to `bx-core` or `bx-mantle`.

## Recovery

`src/bx-vdm/bop/softpc_machine_interrupt_v2_generic_ud_bridge.*` validates
only a real-mode `C4 C4 02` or `C4 C4 06` generic event, invokes the existing
`startup_machine_interrupt_v1` source-derived operation stream, then copies
the already typed CPU result into the existing fixed-width generic outcome.
`dem_v2_composition_bridge.c` registers this package ahead of the other
adapter-owned providers.

The retained source operation map remains:

| Selector | Original source owner | Observable retained mechanics |
| --- | --- | --- |
| `02` | SoftPC `bios.c → unexpected_int` | PIC ISR/IMR actions and BDA `0040:006B` publication. |
| `06` | SoftPC `bios.c → illegal_op_int → unexpected_int` | Saved `SS:ESP` CS:IP increment, then the same PIC/BDA sequence. |

The historical NT4 product-shell diagnostic/UI branch remains excluded from
this non-debug machine profile; it is not replaced by a new adapter policy.

## Verification

Fresh formal graph and focused executable:

```text
build/M0-T246-S2/formal-r3
t246-s2-softpc-machine-interrupt-ingress-fixture.exe
```

The fixture initializes the native minimal CPU5/P-MMX machine and calls the
actual active `bx_ntvdm_mantle_generic_ud_bridge_v1` symbol.  It proves:

1. `C4 C4 06` resumes at `RIP+3`, increments the saved frame IP from `1234h`
   to `1235h`, and writes `FFh` to BDA `0040:006B`.
2. `C4 C4 02` resumes at `RIP+3` and publishes the same source-shaped BDA
   result through the native default PIC port space.
3. An out-of-range `06` stack produces an accepted typed `STOP`, rather than
   a fall-through exception path.
4. `C4 C4 07` is declined by this package.

The executable exited zero.  The normal Bochs CPU/APIC reset log is expected.

## Result and transfer

S2 closes active route selection for the complete `02/06` pair.  S3 remains
responsible for the package regression sweep and one bounded source-built
native run proving that the former repeated `06` has advanced to a distinct
next owner.
