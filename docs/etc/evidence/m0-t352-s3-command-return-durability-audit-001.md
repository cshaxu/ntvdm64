# M0 T352 S3 — COMMAND return durability audit

## Original COMMAND call audit

The complete reached original COMMAND region is structurally direct:

- `cmd.h` defines `PFNSVC` as `VOID (*)(VOID)` and declares
  `cmdSetInfo(VOID)` with that exact shape.
- `cmddisp.c` defines `apfnSVCCmd[]`; index five is `cmdSetInfo`, and
  `CmdDispatch` invokes that slot before its ordinary `TRUE` return.
- `cmdmisc.c::cmdSetInfo` is straight-line: it calls the existing scalar
  registration facade, sets original CF/AX on its failure path, and returns.
- `mvdm_command_guest_state_set_scs_scalars` performs only thread/session
  validity checks and fixed-width real-mode-location calculations; it has no
  wait, I/O, callback, lock acquisition or loop.
- The current x86 linker map resolves `_CmdDispatch`, `_cmdSetInfo`, and
  `_apfnSVCCmd` from `original-mvdm-command`.

Thus source review finds no missing sibling, incompatible table signature, or
blocking call inside the selected COMMAND cohort.  It does not manufacture a
runtime success claim.

## Observer audit

The existing observer writes its companion `.console.txt` only after the
watchdog, using `ReadConsoleOutputCharacterA` on the final console screen.
This is not an append-only record.  Because NTDOS/SoftPC output can scroll or
overwrite console cells after an observation line, absence of
`MVDM-BOP-RETURN` from that snapshot cannot distinguish a missing return from
a completed, later-overwritten return.

## Selected S4 recovery cohort

S4 may preserve the same state-neutral record but make it durable through a
new child-only observer environment path, analogous to the existing
main-return and exception report paths.  The adapter may append only the
already-live fixed-width return record to that path.  The observer owns the
path creation/lifetime; no guest handle, guest memory, BOP result, CPU state,
or production session state may change.  One fresh formal product and one
fixed observation then provide a conclusive return/no-return result.
