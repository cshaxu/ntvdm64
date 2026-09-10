# M0 T396 S34 — first WOW32 dispatch frontier

## Question

After the admitted WOW32 provider returns from original `W32Init`, what is the
first original dispatch owner reached by the fixed x86 `WOWEXEC.EXE` workload?

## Inputs and procedure

The x86 parent was rebuilt from `build/M0-T396/S20/formal-x86` after the
CPU40 DPMI real/protected transition corrections, and the complete original
provider was the current `build/M0-T396/S32/wow32-x86/wow32.dll`. The existing
`console-startup-observer-module-snapshot.exe` ran the manifest-pinned
`O:\\t396` stage for 30 seconds without debugger attachment, breakpoints,
guest-memory access, or product input beyond the declared
`system32\\WOWEXEC.EXE` command.

The observer reported a live 31-module process with `WOW32.DLL` loaded. Its
bounded primary-thread snapshot included:

```text
win32u.dll!+0x123c
KERNELBASE.dll!+0x34df9
WOW32.DLL+0x2ae74
ntvdm32.exe+0x7bf13
```

The parent map has preferred base `0x00400000`; hence loaded address
`0x00ddbf13` is preferred `0x0047bf13`, inside original `MS_bop_1`
(`0x0047bda0`--`0x0047bf20`). The provider map has preferred base
`0x10000000`; `WOW32.DLL+0x2ae74` is inside original `WU32RegisterClass`
(`0x1002b980`).

## Observations

The preceding default-off CPU report records every formerly problematic DOSX
visible transfer as `D1CD`/`C9D9` while `pe=0`; it contains no CCPU fault.
The new run had no exception report and the observer ended it only at its
fixed 30-second timeout. It retained all provider-init conditions already
recorded by S34: the complete provider was loaded, and original `W32Init` had
returned before dispatch.

`MS_bop_1` calls `W32Dispatch` after initialization. The reached
`WU32RegisterClass` body calls:

```c
(pfnOut.pfnRegisterClassWOWA)(&t1, (DWORD *)&wc)
```

`pfnOut` is the original 21-field `PFNWOWHANDLERSOUT` result of
`UserRegisterWowHandlers`. The current stop follows the class-registration
output slot into modern USER/Win32u; it is neither the earlier missing
hung-app registration slot nor a null function-pointer/CCPU exception.

## Interpretation and follow-up

Confidence is high that DPMI32, original `MS_bop_1`, complete WOW32 loading,
`W32Init`, and first `W32Dispatch` are live in one product process. The
bounded wait does **not** prove a visible Win16 window, message loop, task
lifecycle, or graceful exit. It proves the next owner boundary narrowly: the
NT4-private `pfnRegisterClassWOWA` class-registration service. S35 must audit
that output-table contract and all source-recovery rungs before any attempt to
route it through public Win32.
