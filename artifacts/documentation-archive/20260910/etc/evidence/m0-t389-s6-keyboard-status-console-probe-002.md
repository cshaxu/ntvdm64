# M0 T389 S6 — source-owned keyboard-status Console probe

## Purpose

Correct the earlier fixed-container interpretation of the explicit
`ntvdm.exe command.com` run.  The earlier observer gated its ordinary Console
row only on the NTVDM BIOS `keyboard_io` `AH=2` waitio case.  The current
CPU40/x86 product instead reaches the original `AH=1` keyboard-status polling
case.  This probe makes that reached source edge observable without modifying
guest memory, BOP routing, CPU state, PIC state, or the original idle calls.

## Inputs and method

* Product: `O:\ntvdm64\ntvdm.exe`, SHA-256
  `3b42fb6666b716d764b811ddb3c1d2337c63140c0c027fd5af076ea8f7a0f884`.
* Formal graph: `build/M0-T389/S6/formal-x86-r2`; incremental rebuild and
  final link exited `0`.
* Fixed immutable package root: `O:\ntvdm64`.
* Launch declaration: `--command command.com`, retaining the S6 normal-child
  `COMMAND.COM + CR/LF` record.
* Container: rebuilt non-debug, Console-owning
  `build/tools/console-startup-observer.exe`, 30-second watchdog.
* Console row: `ver`, armed only after the default-off marker at original
  `keyboard_io` case `AH=0`, `AH=1`, or `AH=2`.

The marker is one latched child-only diagnostic.  It is emitted immediately
before unchanged source calls to `WaitIfIdle`, `IDLE_poll`, or `IDLE_waitio`.
The observer uses public `WriteConsoleInput` only after that marker.  It does
not write a guest DOS buffer or produce another Base VDM command record.

## Result

The result remains a 30-second watchdog timeout, but the corrected readiness
test now reaches its intended source boundary:

```text
scripted-console-input-ready=yes
MVDM-COMMAND-INPUT-READY
MVDM-CONSOLE-KEY ... down=1
MVDM-KBD-OFFER ... accepted=1
MVDM-KBD-ICA-REQUEST irr=03 isr=00 imr=18
MVDM-CONSOLE-KEY ... down=0
```

Thus the normal Console input is accepted by the original worker and reaches
the original SoftPC keyboard offer/ICA-request path.  The complete scripted
row is not accepted: no subsequent `MVDM-KBD-PORT60` drain record appears,
and the observer correctly reports the row as failed rather than claiming
that `ver` executed.  The same report records repeated CPU hardware-interrupt
deferrals while `IF=0`; this is evidence of the current machine/keyboard
delivery boundary, not a basis to change guest flags from the observer.

The native stopped-thread stack maps to the original selected path:

```text
ccpu -> c_cpu_simulate -> host_start_cpu -> host_main
     -> keyboard_io -> idetect -> idle_kybd_poll -> host_release_timeslice
```

It is therefore not a post-`54:04` missing BOP, a second Base VDM record, or
an app Console parser.  The original command record remains consumed exactly
once; the established `54:01`, `54:0E`, and `54:04` returns remain intact.

## Owner disposition

The first remaining runtime cohort is the original CPU40/SoftPC keyboard
delivery chain: Console worker event -> 8042 offer/port-60 drain -> ICA IRQ1
request/service -> CPU interrupt-frame continuation.  It must be audited and
recovered as one source-shaped machine cohort, including the observed IF/EOI
and controller-full conditions.  It must not be repaired by a BOP leaf,
synthetic command, host prompt reader, direct guest input, or a changed
`COMMAND.COM` binary.

This probe does **not** prove the child banner, `Do16BitPrompt`, DOS `CON`,
line editing, Ctrl+C, `exit`, or parent return.  It does prove that the former
AH=2-only readiness gate was too narrow and that product input reaches the
original machine-side keyboard path.
