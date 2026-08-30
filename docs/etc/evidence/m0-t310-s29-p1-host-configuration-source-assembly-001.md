# M0 T310 S29 P1 — host-configuration source assembly

## Original source selection

The original MVDM support `oemuni` package is now selected as a two-source
library (`file.c`, `process.c`).  It preserves the OEM-to-Unicode conversion
and public Win32 call sequence used by the original PIF, DEM and COMMAND
callers.  No replacement OEM path/environment implementation was added.

This directly supplies `ExpandEnvironmentStringsOem` for original
`nt_pif.c:GetPIFConfigFiles` and also removes the selected OEM file, directory,
volume and environment unresolved cluster from the formal graph.

## Private console configuration dispositions

- `SetConsoleKeyShortcuts` preserves its original signature and returns
  `FALSE`/`ERROR_CALL_NOT_IMPLEMENTED`.  Modern public console APIs cannot
  reserve the original global key combinations.
- `ConsoleMenuControl` preserves its original signature and returns
  `NULL`/`ERROR_CALL_NOT_IMPLEMENTED`.  Modern public console APIs cannot
  attach the original command-range event route to a console menu.

Neither result manufactures a keyboard hook, system menu, or partial Console
Server protocol.

## Formal evidence

Fresh `build/M0-T310/S29/formal-host-configuration-x86-r2` completed the
380-edge selected Win32/x86 CCPU40 candidate and forced-link audit.  The audit
contains no unresolved selected OEM API, `SetConsoleKeyShortcuts`, or
`ConsoleMenuControl` symbol.  The remaining residue is owned by LIM, DEM/VDD,
SIM32, WOW, DPMI/debugger, or video integration packets.
