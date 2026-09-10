# M0 T391 S7 — app-entry and second COMMAND final matrix

The published x86 product was verified from `O:\\ntvdm64` on 2026-09-05.
The VDM cases run through a separate real `conhost` Console, rather than the
Codex agent pseudoconsole.  The fixture clears all `MVDM_*` observation
variables before it starts each product process.

```text
container=clean-console-no-mvdm-diagnostics
case=command/c ver
result=0
case=command.com /c ver
result=0
case=command.com /c command.com /c ver
result=0
case=command.com [Console exit]
result=0
```

Each `VER` row above requires `MS-DOS Version 5.00.500` in the real Console
buffer.  Thus the exact tight form is normalized at app entry, ordinary and
nested DOS COMMAND `/c` ownership remains guest-owned, and an interactive
second COMMAND returns through `exit`.

The non-VDM branches were then checked independently:

```text
ntvdm32.exe ver                         exit=0
Microsoft Windows [Version 10.0.26200.9278]

ntvdm32.exe cmd.exe /d /s /c exit 37    exit=37
```

The first is the unresolved-token `COMSPEC /c` path; the second is the
resolved-native direct `CreateProcess` path.  Neither creates a VDM.

This matrix corresponds to the ownership diagram in
`m0-t391-s7-app-entry-command-flow-and-regression-001.md`: `54:08` starts a
child product with its tail unchanged, while only child app entry resolves and
classifies it.
