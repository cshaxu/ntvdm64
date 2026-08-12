# T95 S6 PC Reset-Floor Profile Design

## Goal

Define one coherent original-Bochs declarative profile for the smallest
admissible intact runtime: a native PC reset floor plus BIOS ROM. This is not
a CPU/RAM/ROM substitute and introduces no adapter or guest policy.

## Candidate Profile

```text
config_interface: textconfig
display_library: nogui
cpu: count=1, ips=1000000, reset_on_triple_fault=0
megs: 4
romimage: file=ROM/BIOS-bochs-latest
vgaromimage: file=ROM/VGABIOS-lgpl-latest

pci: enabled=0
ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
ata1: enabled=0, ioaddr1=0x170, ioaddr2=0x370, irq=15
ata2: enabled=0, ioaddr1=0x1e8, ioaddr2=0x3e0, irq=11
ata3: enabled=0, ioaddr1=0x168, ioaddr2=0x360, irq=9
vga: extension=none, update_freq=5
floppy_bootsig_check: disabled=1
boot: disk
plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0, gameport=0
```

Every directive is parsed by imported Bochs. ATA addresses remain only because
the original grammar carries them; `enabled=0` is the behavioral setting. No
drive image, host directory, network, USB, sound, CD-ROM, adapter environment
or guest payload appears.

## Classification

| Class | Members | Reason |
| --- | --- | --- |
| Required reset floor | CPU, RAM, BIOS/VGA ROM, CMOS, DMA, PIC, PIT, floppy controller, core VGA, keyboard, compiled I/O APIC, one empty ATA controller/hard-drive plugin | Original intact lifecycle owns them as one firmware-reset substrate; its post-init GUI status call hard-links the hard-drive plugin even under nogui. |
| Declaratively denied | PCI/i440FX with ACPI/PCI-IDE; attached ATA media and channels 1--3; VBE/Cirrus; listed optional plugins | Original parser/device manager gates them without source composition changes. |
| Compiled but not capability | CPU extension, GUI, USB/network/sound and unused storage objects in the r4 link | Build closure is not runtime activation or guest exposure. |
| Not configuration-expressible | CPU/RAM/ROM-only composition; individual removal of reset-floor devices | Would require the rejected device-manager rewrite. |

## Required Next Observation

A separate fresh-root execution admission must materialize this profile under
`artifacts/analysis/`, run r4 once with the existing bounded benchmark, and
compare its trace against r002. Success requires no PCI/440FX/PIIX3/ACPI/PCI
IDE initialization, no ATA media/host path, core VGA rather than VBE, original
reset and controlled-stop markers, and no adapter/guest input. The unavoidable
reset-floor lines are expected and are not feature creep.

No profile has been written, executed or claimed by this design record.

## Observation 001 Correction

The first materialization of the earlier all-disabled-ATA draft successfully
denied PCI/440FX/PIIX3/ACPI/PCI IDE and VBE, then reached original
`main.cc:1044`.  That unconditional headerbar-status update calls
`DEV_hd_get_first_cd_handle`; with no hard-drive plugin,
`iodev/hdimage/hdimage.cc`'s stub panics and nogui waits for an interactive
answer until the observer watchdog terminates it. `devices.cc:886-897` proves
that enabling any original ATA resource loads the existing hard-drive plugin.
The revised profile therefore retains exactly `ata0` as an empty controller:
it has no disk/CD image, host path, PCI IDE or guest-exposed media. This is a
coherent intact-lifecycle dependency, not a new device feature or source patch.
