# M0 T420 — Runtime package completion closure

The owner accepted T420 on 2026-09-22 after S45 delivery `b314b8a84`.
T420 completed its selected non-WOW32 package-recovery and capability sequence:
canonical MVDM mirror recovery; CCPU40/C-VID/SoftPC/DOS/DOSX/DPMI/XMS/
SubAlloc/OEM/DEM/COMMAND/VDMREDIR/debugger/VDD closure; the three-process
product regression matrix; and a finite, read-only `NTVDM.REG`-first static
registry reader facade.

The final deployed `O:\winnt` artifacts passed all 17 transcript- and
exit-gated direct, interactive and nested COMMAND/MEM/EDIT routes. The worker
imports only `RegOpenKeyExA`, `RegQueryInfoKeyA` and `RegEnumValueA` for the
explicitly admitted read-only roots; it imports no registry mutation API.
Guest media remained unmodified.

The closure does not claim WOW32/WOW16/WRITE success. That work, including
the mutable in-memory registry overlay and atomic `NTVDM.REG` persistence
contract, remains assigned to the queued WOW32 runtime-recovery package. The
next admitted task is T421, which makes VDMREDIR and WOW32 DLL target
boundaries mechanically inspectable without moving canonical OpenNT mirror
source from `src/mvdm/`.
