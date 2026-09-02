# M0 T382 closure — SoftPC LPT false-predecessor classification

T382 closes at S1 because its admission premise was disproved by the complete
original owner/data-flow ledger.

- The current formal binary contains the original CPU40 `reset -> printer_init
  -> printer_post -> IOS -> printer_io -> printer_inb` cohort.
- The frozen timeout snapshot at `inb(0x3BD)` is an `AH=0` INT 17 status
  read, not proof of a printer poll or a missing host transport.
- Original initialization supplies ready status `0xDF`; its non-MONITOR
  `printer_inb` result has bit 7 set, so the immediate output loop exits on
  its first read.
- The NT4-only direct LPT transport is a `STATE_DONGLE` branch and is not
  reached by this state.

No source or behavior changed.  The task deliberately does not perform the
planned later LPT recovery, formal rebuild, or second observation because
they would validate a premise already contradicted by the selected original
source.  T381 resumes as the active original COMMAND `cmdExec32` package.

Evidence: [T382 S1 ledger](../etc/evidence/m0-t382-s1-current-softpc-lpt-owner-ledger-001.md).
