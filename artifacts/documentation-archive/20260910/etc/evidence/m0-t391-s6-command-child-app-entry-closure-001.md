# M0 T391 S6 — COMMAND child / app-entry closure

## Closed scope

S6 closes the selected single-process-to-child-product boundary.  It does not
change guest `COMMAND.COM`, DOS `EXEC`, PSP return, WOW bootstrap, BaseSrv or
the registry.

The published x86 baseline is `a67dea29c4fc` (`ntvdm32.exe`, SHA-256
`85a667dcd161329b897ba2580a46c8254591bd50ceb6e063bc8ba93e04761f51`).

## Recovered disposition

The only image classifier is the app entry of each product process:

1. Resolve one executable token beside the product, then through the ordinary
   current-directory/PATH search, using `.COM`, `.EXE`, `.BAT` precedence.
2. A resolved DOS or Win16 image enters that child process's normal
   BaseVDM/PermCom path; Win16 remains bootstrap-gated.
3. A resolved non-DOS/non-Win16 image is given to public `CreateProcess`.
4. An unresolved token is passed unchanged to `COMSPEC /c`.

The later original `COMMAND` `54:08` worker does no second classification.
When it has selected its historical `COMSPEC /c <tail>` boundary, the Win32
adapter starts the current architecture's NTVDM product with `<tail>`.  The
child product consequently reuses the four app-entry outcomes above.  The
unchanged original worker continues to wait and return through its own
`54:0B` path.

Direct guest DOS `EXEC` is distinct: recognized DOS images remain in the
existing VDM and retain their original DOS parent/PSP return.  Guest internal
commands also remain guest `COMMAND.COM` work; neither crosses this adapter.

## Acceptance

The direct app-entry controls passed from the published `O:\ntvdm64` package:

- `ntvdm32 "cmd.exe /c exit 37"` returned `37`, proving resolved native PE
  stays outside VDM startup.
- `ntvdm32 ver` printed the Windows version and returned `0`, proving an
  unresolved built-in reaches `COMSPEC /c`.
- The owner ran the normal Console form below and observed the expected DOS
  result and prompt return:

```text
O:\ntvdm64>ntvdm32 command.com /c ver

MS-DOS Version 5.00.500

O:\ntvdm64>
```

An automated observer/agent-launched container produced access/fast-fail
exceptions for the same textual command.  Its extra Console/diagnostic launch
shape is not equivalent to the owner's ordinary `cmd.exe` session and is
recorded as an S7 test-container discrepancy, not as a product failure.

## Result

S6 is closed.  S7 owns the executable flow explanation, the exact Console
regression matrix, and diagnosis of any issue reproduced in the ordinary
published-package console.
