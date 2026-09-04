# M0 T391 S5 — PermCom Initial-Record Recovery

## Scope

This closes only the source-shaped initial BaseVDM record and the resulting
PermCom-to-second-`COMMAND.COM` startup path.  It does not claim the later
DOS-originated native-child return required by S6.

## Recovered contract

The app publishes exactly one original-style initial record:

- the permanent first `COMMAND.COM` remains PermCom and receives no app-made
  `/C <target>` command line;
- `AppName` is the requested target image;
- `CmdLine` is only that target's CR/LF-terminated argument tail; and
- an explicit `command.com` is consequently an ordinary second guest child,
  with no target-specific app path and no `/C` appended to it.

The only bounded `/C` tail retained by the standalone app is the no-argument
case: it gives the otherwise commandless single-session product a normal
terminal disposition after PermCom requests another record.  Guest
`COMMAND.COM` is unchanged.

## Fixed short-root observation

The nested build runtime path was 54 characters and produced a startup
`0x41` before image identity.  That was a path-capacity failure, not a
COMMAND outcome.  The authoritative product container is now the short root
`O:\ntvdm64`; formal build paths are inputs only.

The fixed `command.com` run used:

```text
product: build/M0-T389/S6/formal-x86-r2/original-softpc-process.exe
runtime root: O:\ntvdm64
command: command.com
container: console-owning-nondebug
```

Its retained local evidence is `O:\ntvdm64\t391-permcom-command*.txt`.
The continuation report proves both original `54:0F` environment calls
returned successfully (`BX=0139`), the HMA resident entries are live, and the
following original `54:01` BaseVDM acquisition returns success.  The Console
report proves source-owned Console registration and two screen writes with
343 and 465 nonblank cells.  The same run delivered `ver` only after the
original buffered DOS input edge reported ready.

The observer's copied stdout file remains blank because it does not read the
Console screen buffer.  It is not evidence that guest output was absent.

## Pure-DOS control observations

The same fixed image and `O:\ntvdm64` container were also run without any
native host-child workload:

- `MEM.EXE` completed with process exit `0` in the fixed non-debug container;
  its continuation report shows the original environment path and both
  `54:01` acquisitions, including the normal target record (`AX=000e`,
  `BX=0001`).
- Explicit `COMMAND.COM` remained alive for the ten-second observation
  budget.  The observer waited for the original DOS buffered-console-input
  edge, delivered `ver`, and recorded `ready=yes`, `delivered`, and zero
  queued input remaining.  The source-owned Console presentation report again
  advanced from 343 to 465 nonblank cells.

The retained local reports are
`O:\ntvdm64\t391-pure-dos-mem*.txt` and
`O:\ntvdm64\t391-pure-dos-command-ver*.txt`.  These are pure guest-DOS
observations only.  In particular, they neither invoke nor imply a working
`cmdExec32` route.

## Product acceptance observation

The owner manually ran the staged product from `O:\ntvdm64` with the explicit
`command.com` target and confirmed that the ordinary second guest
`COMMAND.COM` displays its banner and executes commands correctly.  This is
direct product acceptance of the PermCom-to-normal-child shell behavior; it
matches the fixed-container Console/keyboard evidence above.  It remains a
pure-DOS result and does not claim that a DOS program can yet launch a native
32-bit child through `cmdExec32`.

## Result

S5 is closed: the initial record reaches original DOS `EXEC`, a normal second
`COMMAND.COM`, original environment/resident initialization and original DOS
Console input/output without an app parser, synthetic BOP, guest rewrite or
console-output substitute.  S6 remains the next owner work: a DOS-originated
native child must return non-zero through the original `cmdExec32` lifecycle.
