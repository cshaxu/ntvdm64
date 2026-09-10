# M0 T318 S2 P7 — CPU40 V7 VGA firmware configuration recovery

## Question

Can the selected original CPU40 SoftPC startup profile select its retained
original video-ROM input without inventing a firmware alias or changing
NTIO/NTDOS/COMMAND ownership?

## Source evidence

`mvdm-host/softpc.new/obj.vdm/cdefine.inc` selects `CPU_40_STYLE`, `CCPU`,
`C_VID`, `SPC386`, `SIM32`, and `V7VGA` for the original non-hardware-V86
CCPU product form.  `mvdm-host/softpc.new/base/system/rom.c::read_video_rom`
loads `v7vga.rom` when `V7VGA` is present, otherwise `vga.rom`.

The retained `mvdm-softpc-firmware/softpc.new/{data,roms}` input package
contains the original `v7vga.rom` and contains no `vga.rom`.  Therefore the
previous product graph omitted an original configuration carrier: it requested
an unavailable filename rather than the selected original V7 VGA profile.

## Recovery

The formal product graph adds only `/DV7VGA` to its shared CPU40 flag set.
No ROM was renamed, copied under an alias, synthesized, or modified.  The
carrier is deliberately common to the x86 and x64 source rows because the
selected CPU40 machine configuration is independent of host pointer width.

## Verification

- Fresh V7 VGA CPU40 formal Ninja graphs linked on x86 and x64.
- A second invocation of each graph reported `ninja: no work to do`.
- A bounded, isolated x86 run of
  `original-softpc-process.exe -f -o --ordinary-child` was inspected with the
  existing public-Win32 window observer.  The former dialog requesting
  `vga.rom` did not occur.  The first observed original dialog instead was
  `NTVDM has encountered a System Error — The handle is invalid.`

## Interpretation and transfer

This closes the original V7 VGA *configuration and retained-media selection*
issue.  It does not prove `NTIO.SYS`, NTDOS, COMMAND, guest EXEC, or parent
return: the new invalid-handle error occurs later on the original host startup
route and must be attributed to its original SoftPC/NT host owner before any
new implementation is admitted.
