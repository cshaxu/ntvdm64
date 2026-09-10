# M0 T382 S1 — Current-formal SoftPC LPT owner ledger

## Question

Does the current formal CPU40/x86 observation at original
`ios.c::inb(0x3BD)` establish that the LPT owner package blocks pure DOS
startup, and therefore must be repaired before original COMMAND `54:08`?

## Inputs

- Current formal product identity and stopped register/stack record from
  [T381 S2 P2](m0-t381-s2-p2-current-formal-predecessor-observation-001.md).
- Current CPU40 formal graph at
  `build/M0-T371/S2/formal-x86/build.ninja`.
- Original current mirrors: `reset.c`, `bios.c`, `ios.c`, `printer.c`,
  `printer_.c`, `host/src/config.c`, and `host/src/nt_lpt.c`.
- The earlier source/package closure in
  [T363 S1--S4](m0-t363-s1-conventional-device-owner-ledger-001.md).

## Current owner chain

The current formal graph compiles all six original owner units.  The stopped
image address maps to `ios.c::inb`; its caller maps inside
`printer_.c::printer_io`; stack argument `0x3BD` is LPT1 status.  The stopped
CPU state has `EAX=0` and `EDX=0`, so the original call is `AH=0`, `AL=0`,
adapter zero -- the standard INT 17 character-output case, not an OpenNT BOP
selector, monitor operation, or a dongle probe.

The original initialization/data flow is complete:

1. `reset.c` invokes `printer_init(adapter)` immediately followed by
   `printer_post(adapter)` for every LPT adapter.
2. `printer_init` installs the original `printer_inb` IOS callback, maps the
   LPT port range, sets `status_reg=0xDF`, and sets `STATE_READY`.
3. `printer_post` writes LPT1 base `0x3BC` and the original timeout `0x14`
   into the BIOS data area.
4. `printer_io` loads those exact BDA values.  Its `AH=0` loop reads `0x3BD`
   and exits on status bit `0x80`.
5. In the selected `NTVDM`/non-`MONITOR` profile, `printer_inb` in
   `STATE_READY` returns `(0xDF | STATUS_REG_MASK)`, still with bit `0x80`
   set.  The loop therefore exits after the first status read.

The host-specific `nt_lpt.c::host_read_printer_status_port` route is reached
only for original `STATE_DONGLE`; the current `STATE_READY` route does not
open `LPT1`, does not call the NT4-only `\\.\$VDMLPT1` transport, and does
not require a virtual printer or a modern replacement.  Its direct-access
failure behavior remains a later optional-host-capability concern, not this
startup path.

## Disposition

The snapshot records a valid moment inside original LPT status dispatch, but
it is not evidence of a loop, missing callback, unavailable printer, or an
LPT terminal.  In particular, a timeout observer samples one instruction
without measuring call count or duration; the source-established first-read
exit contradicts interpreting that sample as a blocked printer poll.

No LPT source, adapter, configuration, guest media, or port value is to be
changed.  T382 has therefore identified a **false predecessor transfer**:
the LPT package was already source-shaped and selected.  The active recovery
returns to the original COMMAND `cmdExec32` cohort in T381.  This does not
claim that pure DOS or the native child now runs; it only removes an
unsupported LPT-blocker inference.
