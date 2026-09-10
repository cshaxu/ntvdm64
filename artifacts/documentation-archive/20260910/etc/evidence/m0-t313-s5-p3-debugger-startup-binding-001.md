# M0 T313 S5 P3 — Original debugger startup binding

## Decision

The selected original SoftPC startup calls `DBGInit(argc, argv)` from
`softpc.new/host/src/nt_msscs.c` after `CMDInit`, `DemInit`, and `XMSInit`.
The original implementation is the small initialization body in
`mvdm-host/dbg/dbg.c`: it queries `ProcessDebugPort`, stores whether a
debugger is attached, and always returns `TRUE`.

The rest of `dbg.c` is a separate NT4 debugger product: `DBGDispatch`, module
notifications, breakpoints, and VDM event transport require private debug
event / guest-frame composition.  Selecting it merely to satisfy startup would
incorrectly import that product shell.

`adapter-mvdm-host-out/debugger/source/dbg_init.c` therefore supplies only the
same-named `DBGInit` interface.  Its control flow and observable result match
the original body.  The only divergence, `ADAPTER-MVDM-DEBUGGER-001`, resolves
the same NTDLL export dynamically because the historic NT4 declaration is not
a stable public application API.  A missing export keeps the original failed
query direction (`fDebugged = FALSE`, `TRUE` return).

## Verification

Fresh external MSVC Win32/x86 CCPU40 graph:

```text
build/M0-T313/S5/debugger-init-r1
original-softpc-candidate + original-softpc-forced-closure.dll
result: 407/407, exit 0
```

The non-runnable forced-link ownership log no longer lists `DBGInit` as an
unresolved symbol.  It still lists `DBGDispatch`, `ModuleLoad`, `ModuleFree`,
`ModuleSegmentMove`, and `DbgPrompt`; these are deliberate debugger-product
operations, not startup initialization, and remain unselected.

## Reached remaining source boundaries

- `LIM_b_write` / `LIM_w_write` are not a missing package: their original
  definitions are inside selected `base/dos/emm_mngr.c`, but its
  `#ifndef NTVDM` guard excludes them while CCPU40 still sees `LIM` from
  `host_def.h`.  This must be resolved as an original EMS/CCPU configuration
  cohort, not by a fabricated adapter.
- `ShowStartGlass`, `NtAllocateVirtualMemory` / `NtFreeVirtualMemory`, and
  `NtRaiseHardError` belong to WOW/fullscreen/private-NT product paths.  Their
  forced-link appearance does not establish normal non-WOW startup reach.

This P does not claim a full `ntvdm.c` execution, debugger BOP support, EMS
runtime closure, or DOS boot.
