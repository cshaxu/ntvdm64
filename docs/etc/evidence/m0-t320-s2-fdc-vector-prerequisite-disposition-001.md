# M0 T320 S2 — FDC/INT15 prerequisite disposition

## Result

No missing FDC vector or ROM-residency prerequisite remains to recover in the
selected original CPU40 path.  Creating one would be synthetic device success,
which this packet forbids.

## Source facts

- `softpc.new/base/disks/floppy.c::wait_int` saves CPU state, selects
  `wait_int_seg:wait_int_off`, and invokes the original `host_simulate`.
- The selected NTVDM defaults in `base/keymouse/keybd_io.c` are
  `F000:0CE0` (`RCPU_WAIT_INT_SEGMENT/OFFSET`); `base/inc/sas.h` supplies the
  same constants.
- `softpc.new/roms/bios4.asm` places the original `wait_int` ROM routine at
  `RCPU_WAIT_INT_OFFSET` and the original INT15 trampoline at `F000:E970`.
- T319 selected the original generic `base/system/rom.c::rom_init` body.  It
  loads that BIOS image into SoftPC SAS RAM and marks `F0000:FFFFF` ROM before
  CPU40 starts; `ccpusas4.c::sas_init` calls it before `host_main` reaches
  `InitialiseDosEmulation` and `host_start_cpu`.
- Later NTIO BOP `5F` calls original `MS_bop_F -> kb_setup_vectors`, which
  replaces the defaults with NTIO keyboard-table addresses.  Its absence from
  the short console marker list is not evidence that the default FDC vector
  is invalid; it is an internal SoftPC BOP, not an adapter provider.

## Disposition

The remaining failure belongs to the original CPU40 recursive execution and
device-event scheduling contract: `floppy.c::wait_int -> host_simulate ->
c_cpu_simulate`, including the FDC/ICA completion state consumed by that ROM
routine.  It is not a missing BOP, DEM/COMMAND provider, firmware file, or a
safe local FDC/vector repair.  T320 therefore takes its permitted narrow owner
transfer exit.  The next package must audit and recover that complete CPU40
execution/event contract rather than seed an interrupt or redirect the guest.
