# M0 T310 S21 P2 — storage-controller call contract

## Review

The selected CCPU40 storage controller tables were reread as complete
declaration, initializer and indirect-call contracts:

- `diskbios.c` declares `disk_func[]` as `void(void)` for the selected ANSI
  form.  Every assigned `disk_reset`, status, read/write, format and
  parameter handler has the matching original `void IFN0()` body.  Its sole
  indirect call has no arguments.
- `fdisk.c` declares both `dispatch[]` and `activecmd` as `void(int)`.  The
  assigned command-state functions (`rsector`, `wsector`, `format`,
  `diagnose`, `setparams` and `bad`) all retain `void IFN1(int,state)` and the
  original `START`, `CONTINUE` and `BRDY` indirect calls pass exactly one
  state value.
- `fla.c` retains its original byte-port registration order.  It supplies
  `fla_inb`/`fla_outb` to the established I/O controller mechanism and does
  not reach a host media endpoint directly.
- The `floppy.c` default-int reports are K&R source form, not pointer-width,
  callback or table-layout changes.  They remain visible rather than being
  modernized inside the mirror.  Its fixed-width status/DMA values are guest
  controller fields, not native identities.
- The signed-short GFI result narrowing and the byte/word controller status
  assignments are original hardware-width values.  This x86 packet does not
  cast a host pointer/HANDLE into them.
- `emm_mngr.c` retains `MVDM-HOST-DIV-061`: `IHP` is private backing storage
  inside the original EMS manager.  It is not a guest-visible token and does
  not create a second mapping manager.

## Disposition

No mirror or adapter change is justified by this contract review.  The
controller algorithms, tables and source ordering remain original.  Actual
media operations are still the explicitly separate `host_fdisk_*`/floppy
endpoint boundary of S22, while integrated controller execution awaits S49.
