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

## Disposition

This is a guest/CPU40 execution-continuity boundary, not a BaseVDM record,
path-layout, app prompt-reader, or BOP leaf defect.  No S6 source change is
selected from this one observation.  A later owner package must statically
map the post-`54:04` guest transition and repair the first missing original
machine/guest prerequisite before attempting another interactive input run.

