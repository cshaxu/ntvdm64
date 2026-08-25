# M0 T270 S5 P1 — Opennt-softpc source-identity ledger

## Question

Does every production input in `opennt-softpc` have one selected OpenNT
baseline and does any retained input differ from that baseline?

## Result

All 37 production inputs are byte-identical to their selected OpenNT
source/input.  There is no `opennt-softpc-overlay`, no divergence register row
and no current compiled consumer that changes a SoftPC body.  The prior
T263/S3 evidence correctly established the 27-file BIOS package and eight ROM
inputs, but its 35-count did not include the re-rooted selected `bios.c` and
`bios.h` inputs.  This ledger is the successor current count: 29 files under
`bios/` and eight under `roms/`.

## Per-file selected baseline

| Target | Selected OpenNT path | Disposition |
| --- | --- | --- |
| `src/opennt-softpc/bios/bios.c` | `base/mvdm/softpc.new/base/bios/bios.c` | exact SHA-256 match |
| `src/opennt-softpc/bios/bios.h` | `base/mvdm/softpc.new/base/inc/bios.h` | exact SHA-256 match |
| `src/opennt-softpc/bios/biosbop.inc` | `base/mvdm/softpc.new/bios/biosbop.inc` | exact SHA-256 match |
| `src/opennt-softpc/bios/biosseg.inc` | `base/mvdm/softpc.new/bios/biosseg.inc` | exact SHA-256 match |
| `src/opennt-softpc/bios/biostruc.inc` | `base/mvdm/softpc.new/bios/biostruc.inc` | exact SHA-256 match |
| `src/opennt-softpc/bios/devmark.inc` | `base/mvdm/softpc.new/bios/devmark.inc` | exact SHA-256 match |
| `src/opennt-softpc/bios/dosmsg.cmd` | `base/mvdm/softpc.new/bios/dosmsg.cmd` | exact SHA-256 match |
| `src/opennt-softpc/bios/makefile` | `base/mvdm/softpc.new/bios/makefile` | exact SHA-256 match |
| `src/opennt-softpc/bios/msbio.cl3` | `base/mvdm/softpc.new/bios/msbio.cl3` | exact SHA-256 match |
| `src/opennt-softpc/bios/msbio.cl4` | `base/mvdm/softpc.new/bios/msbio.cl4` | exact SHA-256 match |
| `src/opennt-softpc/bios/msbio.cl5` | `base/mvdm/softpc.new/bios/msbio.cl5` | exact SHA-256 match |
| `src/opennt-softpc/bios/msbio.cl6` | `base/mvdm/softpc.new/bios/msbio.cl6` | exact SHA-256 match |
| `src/opennt-softpc/bios/msbio.lnk` | `base/mvdm/softpc.new/bios/msbio.lnk` | exact SHA-256 match |
| `src/opennt-softpc/bios/msbio.skl` | `base/mvdm/softpc.new/bios/msbio.skl` | exact SHA-256 match |
| `src/opennt-softpc/bios/msbio1.asm` | `base/mvdm/softpc.new/bios/msbio1.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/mschar.asm` | `base/mvdm/softpc.new/bios/mschar.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/msend.asm` | `base/mvdm/softpc.new/bios/msend.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/msequ.inc` | `base/mvdm/softpc.new/bios/msequ.inc` | exact SHA-256 match |
| `src/opennt-softpc/bios/msgroup.inc` | `base/mvdm/softpc.new/bios/msgroup.inc` | exact SHA-256 match |
| `src/opennt-softpc/bios/msinit.asm` | `base/mvdm/softpc.new/bios/msinit.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/msint13.asm` | `base/mvdm/softpc.new/bios/msint13.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/ntio.sym` | `base/mvdm/softpc.new/bios/ntio.sym` | exact SHA-256 match |
| `src/opennt-softpc/bios/spcemm.asm` | `base/mvdm/softpc.new/bios/spcemm.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/spckbd.asm` | `base/mvdm/softpc.new/bios/spckbd.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/spcmse.asm` | `base/mvdm/softpc.new/bios/spcmse.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/sysconf.asm` | `base/mvdm/softpc.new/bios/sysconf.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/sysimes.asm` | `base/mvdm/softpc.new/bios/sysimes.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/sysinit1.asm` | `base/mvdm/softpc.new/bios/sysinit1.asm` | exact SHA-256 match |
| `src/opennt-softpc/bios/sysinit2.asm` | `base/mvdm/softpc.new/bios/sysinit2.asm` | exact SHA-256 match |
| `src/opennt-softpc/roms/bios1.asm` | `base/mvdm/softpc.new/roms/bios1.asm` | exact SHA-256 match |
| `src/opennt-softpc/roms/bios1.rom` | `base/mvdm/softpc.new/roms/bios1.rom` | exact SHA-256 match |
| `src/opennt-softpc/roms/bios2.rom` | `base/mvdm/softpc.new/roms/bios2.rom` | exact SHA-256 match |
| `src/opennt-softpc/roms/bios4.asm` | `base/mvdm/softpc.new/roms/bios4.asm` | exact SHA-256 match |
| `src/opennt-softpc/roms/bios4.rom` | `base/mvdm/softpc.new/roms/bios4.rom` | exact SHA-256 match |
| `src/opennt-softpc/roms/cmos.ram` | `base/mvdm/softpc.new/roms/cmos.ram` | exact SHA-256 match |
| `src/opennt-softpc/roms/profile.spc` | `base/mvdm/softpc.new/roms/profile.spc` | exact SHA-256 match |
| `src/opennt-softpc/roms/v7vga.rom` | `base/mvdm/softpc.new/roms/v7vga.rom` | exact SHA-256 match |

## Interpretation

This is only an ownership/provenance correction.  It does not admit the
historical SoftPC runtime, select a ROM, create an adapter dependency, or
enable a Bochs feature.  Any future non-identical retained source must be
locally marked, entered in the component README and assessed against the
private-overlay threshold before it can remain in this component.

## Current-consumer verification

`Export-OpenNtBopInventory.ps1` completed into the disposable external run
root `build/M0-T270-S5/r001/inventory/`. It read the re-rooted selected
`bios/bios.c` and `bios/bios.h` inputs and emitted both inventory outputs
without a source-path fallback:

```text
OpenNT BOP inventory exported: build/M0-T270-S5/r001/inventory
opennt-bop-inventory.json
opennt-bop-inventory.md
```
