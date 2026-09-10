# M0 T319 S1 P1 — SoftPC ROM/vector recursive-contract freeze

Date: 2026-08-31

Status: source contract frozen; no machine behavior change

## Question

What is the first source-owned prerequisite of the P76 zero transfer in
`floppy.c::wait_int`, and can it be recovered without changing BOP, DOS,
COMMAND, FDC result semantics, or the CPU executor?

## Inputs

- [P76 fixed-container observation](m0-t318-s2-p76-floppy-recursive-simulation-owner-transfer-001.md).
- Selected CPU40 build flags in
  `tools/build/New-T310OriginalSoftpcNinja.ps1`: `NTVDM`, `CPU_40_STYLE`,
  `NEW_CPU`, `CCPU`, `SPC386`, `SIM32`, `V7VGA`, and `X86GFX`.
- Original source mirrors:
  `softpc.new/base/disks/floppy.c`, `base/keymouse/keybd_io.c`,
  `base/ccpu386/ccpusas4.c`, `base/system/rom.c`, `host/inc/host_def.h`, and
  `base/inc/sas.h`.
- Retained source-identical firmware input
  `mvdm-softpc-firmware/softpc.new/roms/bios4.asm` and staged `bios4.rom`.

## Source contract

1. `floppy.c::wait_int` saves registers, sets `CS:IP` to the original
   `int15_seg:int15_off`, invokes `host_simulate()`, then invokes the separate
   `wait_int_seg:wait_int_off` recursive CPU routine.  It tests the existing
   `SEEK_STATUS` and returns the original timeout/failure result; it contains
   no legitimate synthetic-success route.
2. In the selected non-`GISP_SVGA` profile, `sas.h` defines those guest entry
   points as `F000:E970` (`INT 15`) and `FE00:0CE0` (`wait_int`).
   `bios4.asm` places respectively `int 15h ... BOP BIOS_CPU_QUIT` and the
   interrupt/timer wait loop at those exact offsets.
3. CPU40 `c_sas_init` calls original `rom_init()` before `copyROM()`.  With
   `NTVDM && X86GFX`, original `rom_init()` intentionally skips its generic
   `read_rom()` body and delegates ROM residency to `host_rom_init()`.
4. The selected original `host/inc/host_def.h` currently defines
   `host_rom_init()` as an empty host hook.  No selected product source
   provides a replacement that copies the already-staged `bios1.rom`/
   `bios4.rom` bytes into the selected SoftPC SAS guest backing store.
5. P76's exception address/IP zero is therefore consistent with the reached
   original recursive call attempting to execute unpopulated firmware.  The
   caller proof alone did not establish this; the CPU40 conditional source
   and retained ROM instruction locations now make it the first recoverable
   prerequisite.  FDC/PIC interrupt delivery remains downstream and is not
   yet diagnosed as defective.

## Disposition

The first recovery slice is the historical `host_rom_init` seam, not a new
floppy or BOP implementation.  S2 must provide a same-shaped host binding
that places the selected byte-exact firmware in the existing SoftPC SAS memory
using the session mapping boundary, preserves the original ROM write-protect
ordering, and leaves `floppy.c`, `keybd_io.c`, FDC, PIC, CCPU and guest DOS
source semantics unchanged.  It must load no firmware by an app-owned guest
loader and must not fabricate a vector or successful wait.

After that source-shaped ROM residency proof, a fixed-container run may
attribute the next reached condition to the original FDC/PIC/interrupt path.

Successor: [S2 P1](m0-t319-s2-p1-original-rom-loader-selection-001.md)
established that the same-shaped binding is unnecessary: the existing
original generic `rom_init()` body is directly composable and takes priority.

## Confidence and follow-up

High confidence that the first missing source-shaped prerequisite is ROM
residency: its selected conditional route is explicit, its hook is empty, and
the exact recursive instruction addresses are present only in the retained
firmware input.  Medium confidence that this is the sole later blocker; no
claim is made about FDC/PIC completion until the correct firmware bytes are
resident and the bounded observation is repeated.
