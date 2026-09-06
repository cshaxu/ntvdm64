# M0 T391 S7 — app-entry and nested COMMAND flow

## Objective

Make the current, bounded launch ownership explicit and validate it only in
the ordinary `O:\ntvdm64` Console container.  Repair a newly reproduced
normal-console regression only at its first owning boundary.

## Flow

```text
ntvdm32 <text>
  -> app declaration: resolve executable token
     -> DOS / Win16: one BaseVDM record -> first PermCom -> original DOS EXEC
     -> resolved other image: public CreateProcess
     -> no resolved image: host COMSPEC /c

second guest COMMAND
  -> guest internal command: original COMMAND/DOS path
  -> direct DOS EXEC: current VDM, original PSP return
  -> original 54:08 COMSPEC /c worker
     -> current ntvdm32/ntvdm64 child with unchanged tail
     -> that child's app-entry classifier (the same three outcomes)
     -> original parent worker wait -> 54:0B -> parent prompt
```

`54:08` deliberately contains no image classifier.  In particular, it must
not decide between native, DOS, Win16, batch or shell syntax: process entry
remains the one decision boundary.

## Required regressions

- `ntvdm32 command.com` starts the normal second guest `COMMAND.COM`.
- In that second shell, a DOS program and a native 32-bit program both return
  to the prompt; `exit` terminates normally.
- `command.com /c ver` from the second shell prints `MS-DOS Version 5.00.500`.
- Direct `command/c ver` normalizes only the exact DOS `COMMAND` tight `/c`
  spelling before ordinary app-entry resolution, then prints the DOS version.
- `ver` in the second shell follows its source-selected host-command path and
  prints the Windows version.
- Direct entry preserves resolved-native and unresolved-token controls.

## Boundaries

No app parser, guest-memory write, guest-media change, registry mutation,
SoftPC/CPU workaround, WOW bootstrap or cross-process pointer/HANDLE transfer
is admitted.  A failure observable only from an injected observer or agent
terminal is first a test-container discrepancy; it does not authorize a
product semantic change.
