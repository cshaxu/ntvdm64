# M0 T310 S20 P2 — host-input source-contract review

## Question

Which selected original SoftPC host-input call forms belong to the current
Win32/x86 CCPU40 input-binding packet, and which instead belong to a later
display/fullscreen owner?

## Inputs and procedure

The review reread the source-identical keyboard conversion file, the reached
`nt_event.c` console calls, and the mouse coordinate/menu call sites.  It
compared their declarations with the selected original `conroute.h` and
`conapi.h` carriers, then rebuilt the formal selected archive and performed
the whole-archive forced-link audit.

## Result

The companion ledger gives each selected function one disposition.  The
original keyboard algorithms remain in `mvdm-host`; the five historical
Console Server functions used by input routing have same-shaped public Win32
bindings in `adapter-mvdm-host-out/win32`.  In particular,
`GetConsoleInputWaitHandle` is deliberately cdecl because the original
`conroute.h` declaration is cdecl.  A modern console input endpoint is
waitable, so the binding returns `STD_INPUT_HANDLE` rather than creating a
synthetic event or a guest token.

`ConsoleMenuControl` is not an input acquisition contract.  It changes the
historical console system menu and remains an explicit original-unavailable
S27 display/fullscreen boundary.  No BOP, MONITOR, kernel VDM, Bochs or
`src.old` dependency was selected.

The current forced-link log retains no unresolved
`GetConsoleInputWaitHandle`, `ReadConsoleInputExW`,
`WriteConsoleInputVDMW`, `ShowConsoleCursor`, or `VDMConsoleOperation`.
It intentionally still reports `ConsoleMenuControl` under its S27 owner.

## Remaining S20 work

The source and selected-link boundary is now complete.  S20 still needs one
focused public-console behavior check for the admitted adapter result before
closure; integrated keyboard/mouse delivery remains S49 work.
