# M0 T391 S7 — second COMMAND native-child proof

On 2026-09-05, the published `O:\\ntvdm64\\ntvdm32.exe` was run through a
separate real Windows Console with no `MVDM_*` diagnostic variables.  The
fixture launched:

```text
command.com /c cmd.exe /d /s /c echo MVDM-NATIVE-CHILD
```

It waits for normal product completion and requires the literal marker in the
Console buffer.  The recorded result was:

```text
container=clean-console-no-mvdm-diagnostics
case=command.com /c cmd.exe /d /s /c echo MVDM-NATIVE-CHILD
product-exit=0
result=0
```

This proves the second DOS COMMAND selected and completed a native child in
the intended `54:08` child-product route.  It complements the command matrix
proof that a nested DOS `COMMAND.COM /c ver` executes and returns with the
DOS `VER` value, and that interactive `exit` returns normally.
