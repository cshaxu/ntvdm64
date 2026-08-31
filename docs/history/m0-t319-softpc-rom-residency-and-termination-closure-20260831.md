# M0 T319 — SoftPC ROM residency and termination attribution closure

## Result

T319 is closed by its declared owner-transfer exit, not by a guest execution
claim.

- S1 proved that the selected CPU40 `NTVDM + X86GFX` branch skipped original
  generic ROM residency, despite `floppy.c::wait_int` requiring firmware
  vectors in the copied BIOS image.
- S2 selected the existing original `rom.c::rom_init` resource/SAS copy body
  for that one translation unit.  Fresh x86 and x64 products link normally.
- S3 preserved the fixed non-debug observer and established that the remaining
  `0xc0000005` does not pass through either original `ntvdm.c::main` return or
  the selected original known-thread exception filter.

The complete final evidence is
[S3 termination attribution](../etc/evidence/m0-t319-s3-post-rom-termination-attribution-001.md).

## Exact transfer

The next owner is the original CPU40 machine execution / raw-worker cohort
after COMMAND registration.  Its original raw `CreateThread` users include
event, heartbeat, floppy, detection and comms paths; the fixed observation
does not identify which one fails, and no debugger or device change was
admitted.  This transfer explicitly excludes BOP, DEM, COMMAND, XMS, DOS
guest images, ROM loading, and any synthetic FDC/PIC success behavior.

## Preserved limits

No claim is made that NTDOS, an ordinary child, or a full FDC/INT15 completion
path executed.  The staged original media and fixed observer remain the
baseline for the next admitted machine-execution package.
