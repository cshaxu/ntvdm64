# M0 T390 S5 P2 — original second-COMMAND Console proof

## Scope

This evidence completes only the pure-DOS explicit-child `COMMAND.COM`
Console-input proof.  It makes no claim that DOS applications, DOSX/DPMI,
native-child return, Win16, graphics, PIF fullscreen, line editing, Ctrl+C or
`exit` lifecycle are runtime-complete.

## Fixed input

The observation used the immutable T390 S2 pure-DOS PIF/media selection and
the formal CPU40/x86 product.  The product identity was
`original-softpc-process.exe` SHA-256
`65375d2f617ad4466e8569f059413a989e5e93fbbbf0a6cc7f6de5aaab44bdc5`.  The launch was
the explicit product form `ntvdm.exe command.com`; no guest image, BOP service,
CPU flag, DOS input buffer or host command parser was changed.

The observer waited for the original CPU40 `INT 21h/AH=0Ah` witness, then sent
the normal Console key sequence for `ver` and Return via the existing original
Console worker.  It did not write guest RAM or invoke an alternate input path.

Durable raw reports are retained outside the repository with the product stage:

* `O:\ntvdm64\m0-t390-s5-pic-reject-ver-then-exit.txt.console-ready.txt`
* `O:\ntvdm64\m0-t390-s5-pic-reject-ver-then-exit.txt.console.txt`
* `O:\ntvdm64\m0-t390-s5-pic-reject-native-child.txt`

## Observed original path

The child-only report first records the original DOS buffered-input boundary:

```text
MVDM-DOS-CON-LINE-INPUT
```

For each submitted normal Console key, the same report then records the
unmodified ownership chain:

```text
MVDM-CONSOLE-KEY …
MVDM-KBD-OFFER … accepted=1
MVDM-KBD-ICA-REQUEST …
MVDM-KBD-PORT60 …
MVDM-CPU-HW-INT-SERVICE vector=09
MVDM-KBD-EOI …
```

The screen-cell Console capture is encoded as historical `CHAR_INFO` data, but
visibly contains the unmodified guest copyright banner, `O:\NTVDM64>` prompt,
the entered `ver`, the resulting `Microsoft Windows [Version 10.0.26200.9278]`
text, and a new `O:\NTVDM64>` prompt.  The native-child report independently
records the original COMMAND execution path:

```text
MVDM-CMD-PAYLOAD origin=guest-tail bytes=3 command=VER\\r\\n
MVDM-CMD-PAYLOAD origin=worker bytes=34 command=C:\\WINDOWS\\System32\\cmd.exe /c VER
MVDM-CMD-EXEC phase=24 … phase=1 … status=1
```

Thus `ver` was not interpreted by an app/session parser: original guest
`COMMAND.COM` accepted the DOS Console line, used its reached original
COMMAND provider path, displayed its result, and returned to its original
prompt.

## Correction required by the evidence

Before the successful run, a stale host-thread IRQ acknowledgement could make
the original `ica_intack()` rejection value `-1` narrow into unsigned vector
`FFh`.  The retained CCPU40 code now preserves the original rejection by
testing its signed result before the existing vector transfer.  This is
registered as `MVDM-HOST-DIV-221`; successful IRQ1 delivery remains unchanged.

The same run also proved that `CONSOLE_UNREGISTER_VDM` must not free the
caller-visible text buffer before the unchanged original `doNullRegister()`
caller returns.  The public Console compatibility binding now retains it until
session disposal or safe replacement, as registered by `ADAPTER-WIN32-032`.

## Result and boundary

S5's required source-defined normal child banner/prompt/DOS-CON wait and one
ordinary Console-key route are proven.  The observer's optional scripted
`exit` tail was not sent because its one-shot AH=0Ah witness intentionally
latches after the first line; that harness timeout is not a product failure
and is not treated as an `exit` proof.  A later workload/lifecycle package
must prove child exit, EOF, Ctrl+C and return ordering.
