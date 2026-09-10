# M0 T310 S28 P3 — host-platform owner disposition

## Result

The selected Win32/x86 CCPU40 host-platform surface has a complete local
disposition.  No forced-link residue remains that requires a further S28
implementation.

## S28 dispositions

- `ExitVDM` is source-shaped Base-client binding through
  `base_vdm_client.c -> base_vdm_local_exit`.
- `RegisterConsoleVDM` retains its original public signature in
  `adapter-mvdm-host-out/win32`; it returns the explicit unavailable result
  because the historical Console Server registration transaction is private.
- `trace_file` is supplied directly by original SoftPC
  `base/debug/trace.c`; the MVDM `dbg` product is excluded.
- `ActivityCheckAfterTimeSlice` is supplied by the retained NTVDMx64 patch's
  original empty body.  This is a finite link disposition only, not an idle
  semantics claim.

## Forced-link residue assigned out of S28

- `LIM_*` and `sas_overwrite_memory`: EMS/LIM and SIM32 owners.
- OEM file, environment and path APIs, `NtVdmControl`, module and SFT-shadow
  APIs: DEM/COMMAND and VDD/monitor owner packages.
- `SetConsoleKeyShortcuts` and `ConsoleMenuControl`: selected configuration
  and host-input policy; S29 owns their public/unavailable dispositions.
- `ShowStartGlass` and `NtRaiseHardError`: WOW/private UI behaviour; later
  WOW owner package.
- `NtAllocateVirtualMemory`/`NtFreeVirtualMemory` from `nt_sec.c`: historical
  fixed `0xA0000` fullscreen regeneration mapping.  This is host-video
  presentation integration, not a generic platform allocation seam; it stays
  unavailable pending the later integrated display boundary.
- `DpmiDispatch`, `DBGDispatch`, and `DBGInit`: their respective BOP,
  DPMI/debugger owner packages.

## Verification

Fresh `build/M0-T310/S28/formal-host-platform-x86-r8` completed the selected
377-edge candidate and forced-link audit.  The audit contains no unresolved
`trace_file`, `ActivityCheckAfterTimeSlice`, `RegisterConsoleVDM`, or `ExitVDM`.
