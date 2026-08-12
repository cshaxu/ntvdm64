# T95 S6 Native Minimum-Profile Boundary Audit

## Question

Can the current intact Bochs 2.6 native lifecycle be reduced to a bare
CPU/RAM/ROM machine solely through original declarative configuration, while
preserving the no-rewrite and no-one-off-device-patch rules?

## Inputs

- Runtime evidence from
  `t95-s6-native-reset-controlled-stop-observation-002.md`.
- Imported `src/bochs/config.cc:806-829,1198-1203,2857-2898`.
- Imported `src/bochs/iodev/devices.cc:175-236`.

## Observations

| Mechanism | Original configuration control | Runtime effect / limitation |
| --- | --- | --- |
| PCI / i440FX | `pci: enabled=0` sets `BXPN_I440FX_SUPPORT=0` in `config.cc`. | Prevents the guarded PCI, PCI-to-ISA, ACPI and PCI IDE loading block. |
| ATA media channels | Original `ataN: enabled=0` parameters default disabled. | Avoids hard-disk/IDE use only when no channel is configured; it does not suppress the core controller graph. |
| VGA extension | `vga: extension=none` is parsed by original configuration. | Suppresses VBE/Cirrus extension behavior, but `devices.cc` still loads the core VGA plugin. |
| Plugin control | `plugin_ctrl` controls listed optional plugin behaviors. | It cannot suppress the unconditionally loaded core devices. |
| CMOS/DMA/PIC/PIT/floppy/VGA/keyboard | No cohesive `enabled=0` declarative machine-profile gate exists for these core loads. | `devices.cc` loads them unconditionally in original lifecycle order. |
| I/O APIC | Compiled under `BX_SUPPORT_APIC`. | `devices.cc` loads it whenever compiled; no observed profile gate. |

The r002 log confirms those source facts at runtime: PCI/440FX/PIIX3, CMOS,
DMA, PIC, PIT, floppy, VGA/VBE, ACPI, I/O APIC, keyboard and IDE initialized.

## Decision

A **bare CPU/RAM/ROM** profile is not declaratively expressible in the intact
Bochs 2.6 lifecycle.  Reaching it would require changing `devices.cc`'s core
device composition or introducing a parallel lifecycle—exactly the
incremental SoftPC/Bochs reimplementation route rejected by the architecture.
It is therefore not an admissible T95 implementation target.

The smallest coherent runtime boundary is instead the **native PC reset
floor**: retain the original core device manager as a whole, declaratively
disable the available optional groups (starting with PCI/i440FX, attached
storage, optional plugins and VGA extensions), and make no claim that compiled
support equals guest capability.  CMOS/PIC/PIT/DMA/keyboard/VGA form the
unavoidable original firmware-reset substrate, not adapter functionality.

## Follow-Up

Do not add per-device source conditions.  A subsequent profile-design packet
may propose one coherent original-configuration profile for the PC reset floor
and name its available declarative disables.  It needs a new execution
admission and must prove the resulting runtime trace; neither this audit nor
r002 authorizes a third execution.
