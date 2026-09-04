# M0 T389 S6 — current x86 explicit COMMAND child observation

## Scope

This is one bounded, non-debug, Console-owning observation of the current
formal CPU40/x86 product.  It validates the source-shaped explicit
`command.com` declaration against immutable staged DOS and firmware media.  It
does not inject a Console line, modify a guest byte, or claim prompt/input
completion.

## Product identity

* Product: `O:\ntvdm64\ntvdm.exe`
* SHA-256: `ee438b2f6a248134e835c587613c8c3b1987528f6ce5fd0883cb0909d4ecfa14`
* Formal source graph: `build/M0-T389/S6/formal-x86-r2`
* Formal result: `435/435`, `original-softpc-process.exe` link exit `0`, with
  a generated Ninja dependency database.
* Runtime root: `O:\ntvdm64` (10 characters; its own short spelling is also
  `O:\ntvdm64`).
* Declaration: `--command command.com`; the selected app record is the S6
  `COMMAND.COM + CR/LF` form, not `/C command.com`.
* Container: current-source `console-startup-observer.exe`, non-debug,
  Console-owning, 30-second watchdog.

## Observed result

The process remained live until the watchdog.  It did not crash or return a
top-level error.  The Base VDM report records exactly one available record
transition from `state=0104, dos-state=1` to `state=0000, dos-state=2`.

The BOP report records the source-owned sequence:

```text
54:01 -> return
54:0E -> return (original keyboard-layout fallback direction, CF=1)
54:04 -> return
```

No second `54:01` record is observed.  Thus the run does not support the
obsolete conclusion that a second Base VDM command producer is required.

After the successful `54:04` return, the guest continues to receive original
timer/IRQ activity but the observation stops at guest `CS:IP=00A7:151F` before
any of these required child-shell markers:

* non-first `COMMAND.COM` copyright/banner output;
* `DoReEnter -> Do16BitPrompt`;
* BIOS keyboard `waitio` / `MVDM-COMMAND-INPUT-READY`; or
* DOS `CON` input.

The captured Console is empty.  Therefore this evidence proves neither an
interactive shell nor keyboard input.  It locates the remaining work after
the declared record and before the source-defined child DOS-CON path.

## Gated Console-row check

The same product, media root and declared `command.com` target were then run
for 30 seconds with the observer armed to submit `ver` only after the original
`MVDM-COMMAND-INPUT-READY` marker.  The result was
`scripted-console-input-ready=no` and no input record was written.  The BOP
sequence again ended after successful `54:01`, `54:0E` and `54:04` returns.

This excludes an app prompt reader, a synthetic BOP record and a prematurely
queued Console key as explanations or repairs.  It also means the current
keyboard worker is not the first missing transition: the original child has
not reached its BIOS `INT 16h` wait boundary.

## Disposition

This is a guest/CPU40 execution-continuity boundary, not a BaseVDM record,
path-layout, app prompt-reader, keyboard-input bridge or BOP leaf defect.  No
S6 product-source change is selected from these observations.  A later
CPU40/guest-continuity owner package must statically map the post-`54:04`
transition and repair the first missing original machine/guest prerequisite
before attempting another interactive input run.
