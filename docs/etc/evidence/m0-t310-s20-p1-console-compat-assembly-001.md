# M0 T310 S20 P1 — console compatibility assembly

The original host input sources call four NT4 Console Server client exports
that no modern public import library supplies. `adapter-mvdm-host-out/win32`
now provides the same source-facing names with public Console/User32 APIs:
record read/peek, record write, cursor visibility, and the reached client
rectangle/coordinate operations. Unsupported `VDMConsoleOperation` selectors
return `ERROR_CALL_NOT_IMPLEMENTED`; no success is fabricated.

`New-T310OriginalSoftpcNinja.ps1` selects the adapter in the formal graph.
The fresh `build/M0-T310/S20/console-input-x86` Win32/x86 CCPU40 candidate
generated and compiled all `370/370` edges, including `console_compat.obj` in
`softpc-win32-bindings.lib`. This is source/link closure only; live console
event behavior remains active S20 validation work.
